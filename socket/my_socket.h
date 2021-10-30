#ifndef _MY_SOCKET_H
#define _MY_SOCKET_H

#include <netdb.h>  // hostent - gethostbyname

extern int cnt_connection_hit;

void *connection_handler(void *client_desc);
void error_handling(char *msg);
int get_ipaddr_from_dns(struct hostent **he, char *hostname);

#endif  /* _MY_SOCKET_H */
