#include <stdio.h>
#include <stdlib.h>
#include "server.h"

void print_start_launcher()
{
  puts("-----------------------------------");
  puts("    Welcome to my socket lab       ");
  puts("    Let's do test the socket       ");
  puts("-----------------------------------");
}

int main()
{
  struct sock_server *server = malloc(sizeof(struct sock_server));
  if (!server) {
    printf("[-]launcher: server instance: malloc failed\n");
    exit(-1);
  }

  print_start_launcher();
  start(server);  // start server

  return 0;
}
