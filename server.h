#ifndef _SERVER_H
#define _SERVER_H 1

#include "helpers.h"
#include "structures.h"

#define SOCKADDR_SIZE sizeof(struct sockaddr)
#define TCP_STRUCT_SIZE sizeof(struct tcp_struct)

#define INT "INT"
#define SHORT_REAL "SHORT_REAL"
#define FLOAT "FLOAT"
#define STRING "STRING"

#define TCP_SIZE sizeof(struct tcp_struct)

void init_sockets(char *port_char, int *udp_sock, int *tcp_sock);

void manage_server(int *tcp, int *udp, fd_set *file);

void manage_tcp (int *tcp, struct sockaddr_in *new, int *max_val, struct client *clients, fd_set *file);

void manage_udp(int *udp, struct sockaddr_in *addr, int *max_val, struct client *clients, fd_set *file);

void stop_server(int max, fd_set *file_descr);

#endif
