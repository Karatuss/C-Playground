#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>  // inet_addr
//#include <netdb.h>      // hostent - gethostbyname
#include "my_socket.h"

int cnt_connection_hit;

void *connection_handler(void *client_desc)
{
  // Get the socket descriptor
  int sock = *(int *)client_desc;
  int read_size = 0;
  char message[100] = {0};
  char client_message[2000] = {0};

  // Send messages to client
  sprintf(message, "[%d] Hello client!!\n", sock);
  write(sock, message, strlen(message));

  sprintf(message, "[%d] total visit count: %d\n", sock, ++cnt_connection_hit);
  write(sock, message, strlen(message));

  write(sock, "Your Message: ", 14);
  while ((read_size = recv(sock, client_message, 2000, 0)) > 0) {
    // React to client from server-side
    sprintf(message, "[%d] Sever echo: %s\n", sock, client_message);
    write(sock, message, strlen(message));

    write(sock, "Your Message: ", 14);
    memset(client_message, 0, 2000);
  }

  if (read_size == 0) {
    puts("Client disconnected");
    fflush(stdout);
  } else if (read_size < 0) {
    perror("recv failed");
  }

  // Free the socket pointer
  free(client_desc);

  return 0;
}

void error_handling(char *msg)
{
  printf("[Error] %s\n", msg);
  exit(1);
}

int get_ipaddr_from_dns(struct hostent **he, char *hostname)
{
  if (!(*he = gethostbyname(hostname))) {
    herror("gethostbyname");
    exit(1);
  }
  return 0;
}

