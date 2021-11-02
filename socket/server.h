#ifndef _SERVER_H

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <fcntl.h>       /* for O_* constants */
#include <sys/stat.h>    /* for mode constants */
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#define MAX_CLIENT  50
#define THREAD_SIZE  100

#define MAX_TRY_THREAD_CREATE 10

struct sock_server {
  int socket_fd;
  struct sockaddr_in addr;
  
  char *hostname;
  char ip[100];
  struct hostent *host_ent;
  struct in_addr **addr_list;

  /* 
   * semaphores for Read & Write Algorithm focusing on FAIRNESS
   * they are init to 0
   *
   * sem_t
   *  resource     : resource lock
   *  rmutex       : reader lock for syncing changes to shared variable 'readcount'
   *  service_queue: FAIRNESS - preserves ordering of requests (FIFO)
   */
  sem_t *resource, *rmutex, *service_queue;
  int readcount;
  // shared resources
  int server_hit;
  int shared_data;
};

struct server_param {
  struct sock_server *server;
  int value;
};

void init_server(struct sock_server *server, int domain, int type, int protocol, int port);
void exit_server(struct sock_server *server);
void error_handling(int socket_fd, char *error);

int start(struct sock_server *server);  /* ENTRY POINT */

#endif  /* _SERVER_H */
