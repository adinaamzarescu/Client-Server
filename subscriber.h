#ifndef _SUBSCRIBER_H
#define _SUBSCRIBER_H 1

#include "helpers.h"
#include "structures.h"

#define TOPIC_LENGTH 51
#define TYPE_LENGTH 11

int check_command (char buffer[MAX], Packet pack, int sockfd);
void print_tcp_pack (struct tcp_struct *pack_send);
void disable_neagle(int *sockfd);

#endif