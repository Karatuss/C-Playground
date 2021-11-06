#ifndef _CLIENT_H
#define _CLIENT_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>   // close(), ...
#include <fcntl.h>    // open()

#include <pthread.h>
#include <errno.h>

struct client_param {
  int socket_fd;
  int menu;    // 0x01 ~ 0x02
  int input;
};

void *client_handler(void *param);
void init_client(int *socket_fd, struct sockaddr_in *sock_client);

#endif  /* _CLIENT_H */
