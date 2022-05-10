#ifndef _STRUCTURES_H
#define _STRUCTURES_H 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <limits.h>

#define TYPE_EXIT 'e'
#define TYPE_SUBSCRIBE 's'
#define TYPE_UNSUBSCRIBE 'u'

#define MAX_TOPIC 55
#define MAX_IP 16
#define MAX_TYPE 25
#define MAX_NAME 55
#define BUFLEN 1560
#define MAX_ID 10
#define MAX 100

typedef struct Packet {
	char topic[MAX_TOPIC];
	char data_type;
	char payload[BUFLEN];
	char ip[MAX_IP];
	uint16_t port;
	int sf;
	char type;
} Packet;


// TCP
typedef struct tcp_struct {
	char ip[MAX_IP];
	uint16_t port;
	char type[11];
	char topic[MAX_TOPIC];
	char payload[BUFLEN];
} msg_tcp;

// UDP
typedef struct udp_struct {
	char topic[50];
	uint8_t type;
	char payload[BUFLEN];
} msg_udp;

// Topic
typedef struct topic {
	char name[MAX_NAME];
	int sf;
} topic;

// Client
typedef struct client {
	char id[MAX_ID];
	int socket;
	int dim_topics;
	int dim_notsent;
	struct tcp_struct notsent[MAX];
	struct topic topics[MAX];
	int connected;
} client;


#endif