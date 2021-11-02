#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include "server.h"

static pthread_t wthread[THREAD_SIZE];
static pthread_t rthread[THREAD_SIZE];

static void *reader(void *param)
{
  struct server_param *sparm = (struct server_param*)param;
  struct sock_server *s = sparm->server;

  sem_t *resource = s->resource,
        *rmutex   = s->rmutex,
        *service_queue = s->service_queue;
  
  // ENTRY SECTION
  sem_wait(service_queue);
  sem_wait(rmutex);

  s->readcount++;
  if (s->readcount == 1)  // if I'm the first reader,
    sem_wait(resource);   // request resource access for readers (writers blocked)

  sem_post(service_queue);
  sem_post(rmutex);
  
  // CRITICAL SECTION
  printf("%d Reader is inside\n", s->readcount);
  printf("----------------------------\n");
  printf("Total server hit\t[%d]\n", ++(s->server_hit));
  printf("Shared Data value\t[%d]\n", s->shared_data);
  
  sleep(5);

  // EXIT SECTION
  sem_wait(rmutex);
  s->readcount--;
  if (s->readcount == 0)  // the very last reader must release the resource lock.
    sem_post(resource);
  sem_post(rmutex);

  printf("----------------------------\n");
  printf("%d Reader is leaving...\n", s->readcount + 1);
  pthread_exit(NULL);
}

static void *writer(void *param)
{
  struct server_param *sparm = (struct server_param*)param;
  struct sock_server *s = sparm->server;

  sem_t *resource = s->resource,
        *service_queue = s->service_queue;
  
  // ENTRY SECTION
  sem_wait(service_queue);
  sem_wait(resource);   // request resource access for readers (writers blocked)
  sem_post(service_queue);
  
  // CRITICAL SECTION
  printf("Writer is inside\n");
  printf("----------------------------\n");
  printf("Total server hit\t[%d]\n", ++(s->server_hit));
  s->shared_data = sparm->value;
  printf("Changed Shared Data value\t[%d]\n", s->shared_data);
  
  // EXIT SECTION
  sem_post(resource);

  printf("----------------------------\n");
  printf("Writer is leaving...\n");
  pthread_exit(NULL);
}

void init_server(struct sock_server *server, int domain, int type, int protocol, int port)
{
  int socket_fd;
  struct sockaddr_in *s = NULL;

  // double pointer is tooooooooooooooo hard to handle delicately and intuitively.
  // server = (struct sock_server *)malloc(sizeof(struct sock_server));
  if (!server)
    error_handling(1, "server instance generation failed");
  memset(server, 0, sizeof(struct sock_server));
  server->socket_fd = socket(domain, type, protocol);
  if (server->socket_fd == -1)
    error_handling(1, "server socket number create failed");

  s = &server->addr;
  socket_fd = server->socket_fd;

  int opt_val = true;

  s->sin_family = domain;
  s->sin_addr.s_addr = htonl(INADDR_ANY);
  s->sin_port = htons(port);
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)); // disable socket bind time-wait

  // initalize shared data count
  server->readcount = 0;
  server->server_hit = 0;
  server->shared_data = 0;

  // initialize semaphore
  server->resource = sem_open("/resource", O_CREAT | O_EXCL, 0644, 1);
  server->rmutex = sem_open("/rmutex", O_CREAT | O_EXCL, 0644, 1);
  server->service_queue = sem_open("/service_queue", O_CREAT | O_EXCL, 0644, 1);
  sem_unlink("/resource");
  sem_unlink("/rmutex");
  sem_unlink("/service_queue");

  if (server->resource == SEM_FAILED)
    printf("sem_open(\"resource\") failed. errno:%d\n", errno);
  if (server->resource == SEM_FAILED)
    printf("sem_open(\"rmutex\") failed. errno:%d\n", errno);
  if (server->resource == SEM_FAILED)
    printf("sem_open(\"service_queue\") failed. errno:%d\n", errno);

  // Bind the socket to
  // the address and port number
  bind(socket_fd, (struct sockaddr*)s, sizeof(*s));

  // Listen on the socket
  // with MAX_CLIENT connection
  // requests queued
  if (MAX_CLIENT <= 0 || MAX_CLIENT > 100)
    error_handling(socket_fd, "too many MAX_CLIENT!(over 100)");
  if (listen(socket_fd, MAX_CLIENT) == 0)
    printf("Listening\n");
  else
    error_handling(socket_fd, "connection queue is full!");
}

void exit_server(struct sock_server *server)
{
  // initialize shared data count
  server->readcount = 0;
  server->server_hit = 0;
  server->shared_data = 0;

  // free the sock_server instance
  free(server);
}

/* error_handling(int socket_fd, char *error)
 * 
 * push message to server, and client threads
 * with formatted error string.
 */
void error_handling(int socket_fd, char *error)
{
  char msg[100] = {0};
  
  sprintf(msg, "[Error]Server to %d: %s\n", socket_fd, error);
  printf("%s", msg);
  send(socket_fd, msg, strlen(msg), 0);

  exit(-1);
}

/* int start(struct sock_server *server)
 *
 * main function for server.c
 */
int start(struct sock_server *server)
{
  int domain, type, protocol, port;
  int new_socket;
  socklen_t addr_size;

  // TODO: menu() function create for server init
  domain = AF_INET;     // ipv4
  type = SOCK_STREAM;   // TCP
  protocol = 0;
  port = 8888;
  init_server(server, domain, type, protocol, port);  

  struct sockaddr_in s_storage;
  int idx = 0;

  do {
    addr_size = sizeof(s_storage);

    // Extract the first connection
    // in the queue
    new_socket = accept(server->socket_fd,
                        (struct sockaddr*)&s_storage,
                        &addr_size);

    int try = 0;
    int choice = 0;
    struct server_param sparm = {
      .server = server,
      .value  = 0
    };
    recv(new_socket, &choice, sizeof(char), 0);

    // in fact, input value check routine exists on client-side
    // but we need to maintain the integrity of
    // user input on server-sid too.
    choice -= '0';
    printf("user input: %d\n", choice);
    if (choice <= 0 || choice > 2)
      error_handling(server->socket_fd, "user input check failed");

    // READ
    if (choice == 1) {  
redo_reader:
      if (pthread_create(&rthread[idx++], NULL,
                         reader, &sparm)
          != 0) {
          printf("[[Creation Error]] Failed to create reader thread\n");
          if (try++ < MAX_TRY_THREAD_CREATE)
            goto redo_reader;
          else if (try == MAX_TRY_THREAD_CREATE)
            error_handling(new_socket, "Something gonna wrong. Plz restart the server!!");
      }

      try = 0;  // init to 0 for later
    } else if (choice == 2) { // WRITE
      // receive a value from a client
      recv(new_socket, &sparm.value, sizeof(sparm.value), 0);
redo_writer:
      if (pthread_create(&wthread[idx++], NULL,
                         writer, &sparm)
          != 0) {
          printf("[[Creation Error]] Failed to create writer thread\n");
          if (try++ < MAX_TRY_THREAD_CREATE)
            goto redo_writer;
          else if (try == MAX_TRY_THREAD_CREATE)
            error_handling(new_socket, "Something gonna wrong. Plz restart the server!!");
      }

      try = 0;  // init to 0 for later
    }

    // Keep the number of threads
    if (idx >= 50) {
      idx = 0;

      while (idx < 50) {
        // Suspend execution of
        // the calling thread
        // until the target thread
        // teminates
        pthread_join(wthread[idx++], NULL);
        pthread_join(rthread[idx++], NULL);
      }

      idx = 0;
    }
  } while(1);

  exit_server(server);

  return 0;
}

int main()
{
  struct sock_server *server = (struct sock_server*)malloc(sizeof(struct sock_server));
  start(server);
}
