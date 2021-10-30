#include <stdio.h>
#include <stdlib.h>       // exit
#include <string.h>       // strcpy
#include <unistd.h>       // socklen_t, write, read
#include <pthread.h>      // threading
#include <sys/socket.h>
#include <arpa/inet.h>

#include "my_socket.h"

// #define TEST
#define CLIENT_MAX 3

int cnt_connection_hit;

int main(int argc, char *argv[])
{
  int server_desc;
  int client_desc;
  socklen_t client_addr_size;
  int *new_socket;

  struct sockaddr_in server;
  struct sockaddr_in client;
  char *message, server_reply[2000];

  char *hostname = "www.google.com";
  char ip[100];
  struct hostent *he;
  struct in_addr **addr_list;

  int option = 1;   // for setsockopt - make SO_REUSEADDR 1

  if (argc != 2)
    error_handling("plz specify the opened port number");

  // Create socket
  server_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (server_desc == -1)
    error_handling("Could not create socket");
  
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(atoi(argv[1]));
  setsockopt(server_desc, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // this is for disable socket binding time-wait state.

  // Bind 
  if (bind(server_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
    error_handling("bind failed");
  puts("bind done");

  // Listen
  if (listen(server_desc, CLIENT_MAX) < 0) 
    error_handling("listen failed");

  // If client asks to connect, accept it.
  client_addr_size = sizeof(client);
  while ((client_desc = accept(server_desc, (struct sockaddr *)&client, (socklen_t *)&client_addr_size))) {
    puts("Connection accepted");

    // Reply to the client
    // unless we make this while context, the connection between
    // server and client would be disconnected immediatly.
    message = "Welcome to my little socket server!\n";
    write(client_desc, message, strlen(message));

    pthread_t sniffer_thread;
    new_socket = (int *)malloc(sizeof(int));
    *new_socket = client_desc;

    if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_socket) < 0)
      error_handling("could not create thread");
  }
  if (client_desc < 0) 
    error_handling("accept error");



#ifdef TEST
  // Get IP address from hostname
  get_ipaddr_from_dns(&he, hostname);  // if the allocation is failed, program is terminated.
  addr_list = (struct in_addr **)he->h_addr_list;

  for (int i = 0; addr_list[i]; i++)
    strcpy(ip, inet_ntoa(*addr_list[i]));
  printf("%s resolved to : %s\n", hostname, ip);
  
  // Connect to remote server
  if (connect(server_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    puts("connect error");
    return 1;
  } 

  puts("Connected\n");

  // Send data
  message = "GET / HTTP/1.1\r\n\r\n";
  if (send(server_desc, message, strlen(message), 0) < 0) {
    puts("Send failed");
    return 1;
  }
  puts("Data Send\n");

  // Receive a reply from the server
  if (recv(server_desc, server_reply, 2000, 0) < 0)
    puts("recv failed");
  puts("Reply received\n");
  puts(server_reply);
#endif  /* end of TEST */
}
