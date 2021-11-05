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
  /*
  if (access("client", F_OK) != 0) {
    printf("[-]launcher: client exec file doesn't exist.\n");
    exit(-1);
  }

  int clients = 0;
  printf("How many client do u want to make?\n");
  printf("> ");
  scanf("%d", &clients);

  do {
  system("build/client");
  } while(clients--);
  */
  return 0;
}
