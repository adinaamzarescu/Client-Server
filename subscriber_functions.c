#include "subscriber.h"

int check_command (char buff[MAX], Packet pack, int sockfd) {
	int res;
	char *tk = strtok(buff, " "); 
	if (!strncmp(buff, EXIT, strlen(EXIT))) {
		pack.type = TYPE_EXIT;
		res = send(sockfd, &pack, PACKLEN, 0);
		DIE (res < 0, "[subscriber] ERROR: Could not send");
		return -1;
	} else if (!strncmp(buff, SUBSCRIBE, strlen(SUBSCRIBE))) {
		pack.type = TYPE_SUBSCRIBE;
		tk = strtok(NULL, " ");
		strcpy(pack.topic, tk);
		tk = strtok(NULL, " ");
		pack.data_type = tk[0] - '0';

		res = send(sockfd, &pack, PACKLEN, 0);
		DIE (res < 0, "[subscriber] ERROR: Could not send");
		return 1;
	} else if (!strncmp(buff, UNSUBSCRIBE, strlen(UNSUBSCRIBE))) {
		// strcpy(&pack.type, "u");
		pack.type = TYPE_UNSUBSCRIBE;
		tk = strtok(NULL, " ");
		strcpy(pack.topic, tk);
		tk = strtok(NULL, " ");
		pack.data_type = tk[0];

		res = send(sockfd, &pack, PACKLEN, 0);
		DIE (res < 0, "[subscriber] ERROR: Could not send");
		return 2;
	}
	return 0;
}

void print_tcp_pack (struct tcp_struct *pack_send) {

	char ip[MAX_IP];
	strcpy (ip, pack_send->ip);
	uint16_t port = pack_send->port;
	char type[TYPE_LENGTH];
	strcpy (type, pack_send->type);
	char topic[TOPIC_LENGTH];
	strcpy (topic, pack_send->topic);
	char payload[BUFLEN];
	strcpy (payload, pack_send->payload);

	printf("%s:%u - %s - %s - %s\n", ip, port, topic, type, payload);
		
}

void disable_neagle(int *sockfd) {
    int neagle = 1;
    int sock = *sockfd;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&neagle, sizeof(int));
}