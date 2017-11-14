#ifndef __NET_H__
#define __NET_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <netdb.h>
#include <sys/un.h>
#include <sys/stat.h>

#include <errno.h>

int multicast_add_membership(int sockfd, char *multi_ip);
int multicast_drop_membership(int sockfd, char *multi_ip);
int local_udp_socket_bind(int port);
int sockaddr_convert(struct sockaddr_in *si, char *ip, int port);

#define SOCKET_PIPE_SUPPORT
#ifdef SOCKET_PIPE_SUPPORT
int socket_server_pipe_create(const char *name);
int socket_server_pipe_accept_client(int server);
int socket_client_pipe_connect(const char *name);
int socket_client_pipe_read(int p, uint8_t *buf, int len);
int socket_client_pipe_write(int p, const uint8_t *buf, int len);
int socket_pipe_close(int spipe);
#endif

#endif
