#include "server.h"

void init_sockets(char *port_char, int *udp_sock, int *tcp_sock) {

	int enable = 1;
	// Creating copies of teh sockets
	int udp = *udp_sock;
	int tcp = *tcp_sock;
	// Open UDP socket
	udp = socket(PF_INET, SOCK_DGRAM, 0);
	DIE(udp < 0, "[server] ERROR: Could not open UDP socket\n");

	// Open TCP socket
	tcp = socket(AF_INET, SOCK_STREAM, 0);
	DIE(tcp < 0, "[server] ERROR: Could not open TCP socket\n");

	// Check port
	short port = atoi(port_char);
	DIE(port <= 0, "[subscriber] ERROR: Invalid port");

	// Fill the sockaddr struct
	struct sockaddr_in server_address;
	memset((char *)&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	struct sockaddr *cast_address = (struct sockaddr *) &server_address;

	// Bind TCP socket
	int tcp_bind  = bind(tcp, cast_address, SOCKADDR_SIZE);
	DIE(tcp_bind < 0, "[server] ERROR: Could not bind TCP socket\n");

	// Bind UDP socket
	struct sockaddr_in udp_address;
	udp_address.sin_family = AF_INET;
	udp_address.sin_port = htons(port);
	udp_address.sin_addr.s_addr = htonl(INADDR_ANY);

	struct sockaddr *udp_cast_address = (struct sockaddr *) &udp_address;

	int udp_bnd = bind(udp, udp_cast_address, SOCKADDR_SIZE);
	DIE(udp_bnd < 0, "[server] ERROR: Could not bind TCP socket\n");

	setsockopt(tcp, IPPROTO_TCP, TCP_NODELAY, &enable, 4);

	// set the TCP socket as listening socket
	int tcp_lsn = listen(tcp, INT_MAX);
	DIE(tcp_lsn < 0, "[server] ERROR: Could not listen on socket\n");

	// Sending back the sockets
	*udp_sock = udp;
	*tcp_sock = tcp;
}

void manage_tcp (int *tcp, struct sockaddr_in *new, int *max_val, struct client *clients, fd_set *file) {
    socklen_t len_udp = sizeof(struct sockaddr);
    fd_set file_ds = *file;
    int max = *max_val;
    int res;
    char buff[PACKLEN];
    memset(buff, 0, PACKLEN);
    int crawler = -1;
    int connected = 0;
    // Creating a copy of tcp
    int tcp_sock = *tcp;
    struct sockaddr_in new_tcp = *new;

    // Accepting
    int socket = accept(tcp_sock, (struct sockaddr *) &new_tcp, &len_udp);
    DIE(socket < 0, "[server] ERROR: Could not accept.\n");
    // Receiving
    res = recv(socket, buff, 10, 0);
    DIE(res < 0, "[server] ERROR: Could not receive.\n");

    // Searching for the client's id and check if it is connected
    for(int i = 5; i <= max; ++i) {
        if(!strcmp(clients[i].id, buff)) {
            crawler = i;
            connected = clients[i].connected;
            break;
        }
    }
    // New client
    if (crawler == -1) {
        FD_SET(socket, &file_ds);
        // Checking if the socket is greater than max
        max = maxim (socket, max);
        strcpy(clients[max].id, buff);
        clients[max].socket = socket;
        clients[max].connected = 1;
        int tcp_sin_port = ntohs(new_tcp.sin_port);
        printf("New client %s connected from ", clients[max].id);
        printf("%s:%d\n", inet_ntoa(new_tcp.sin_addr), tcp_sin_port);

    } else if (crawler && !connected) {
        FD_SET(socket, &file_ds);
        clients[crawler].socket = socket;
        clients[crawler].connected = TRUE;
        int tcp_sin_port = ntohs(new_tcp.sin_port);
        printf("New client %s connected from ", clients[crawler].id);
        printf("%s:%d.\n", inet_ntoa(new_tcp.sin_addr), tcp_sin_port);
        for(int k = 0; k < clients[crawler].dim_notsent; k++){
            int res = send(clients[crawler].socket, &clients[crawler].notsent[k], TCP_SIZE, 0);
            DIE(res < 0, "[server] ERROR: Could not send.\n");
        }
        clients[crawler].dim_notsent = 0;
    } else {
        close(socket);
        printf("Client %s already connected.\n", clients[crawler].id);
    }

    // Sending back the sockets
	*tcp = tcp_sock;
    *new = new_tcp; 
    *file = file_ds;
    *max_val = max;
}

void manage_udp(int *udp, struct sockaddr_in *addr, int *max_val, struct client *clients, fd_set *file) {
    socklen_t len_udp = sizeof(struct sockaddr);
    fd_set file_ds = *file;
    int max = *max_val;
    int res;
    char buff[PACKLEN];
    uint32_t nr;
    double real;
    int n = 1;
    char sign = 0;

    memset(buff, 0, PACKLEN);
    // Creating a copy of udp
    int udp_sock = *udp;
    struct sockaddr_in udp_address = *addr;
    // Receive
    res = recvfrom(udp_sock, buff, BUFLEN, 0, (struct sockaddr *)&udp_address, &len_udp);
    DIE(res < 0, "[server] ERROR: Could not receive.\n");
    // Send to TCP
    struct tcp_struct tcp_send;
    memset(&tcp_send, 0, sizeof(struct tcp_struct));
    // Send to UDP
    struct udp_struct *udp_send;

    tcp_send.port = htons(udp_address.sin_port);
    strcpy(tcp_send.ip, inet_ntoa(udp_address.sin_addr));

    udp_send = (struct udp_struct *)buff;

    strcpy(tcp_send.topic, udp_send->topic);
    tcp_send.topic[MAX_TOPIC] = 0;

    switch (udp_send->type) {
        case 0:
            // INT
            nr = ntohl(*(uint32_t *)(udp_send->payload + 1));
            sign = udp_send->payload[0];
            if(sign) {
                // Signed
                nr = nr * (-1);
                sprintf(tcp_send.payload, "%d", nr);
            } else {
                // Unsigned
                sprintf(tcp_send.payload, "%d", nr);
            }
            strcpy(tcp_send.type, INT);

            break;
        case 1:
            // SHORT_REAL
            nr = ntohs(*(uint16_t *)(udp_send->payload));
            real = ((float)(nr)) / 100;

            sprintf(tcp_send.payload, "%.2f", real);
            strcpy(tcp_send.type, SHORT_REAL);

            break;
        case 2:
            // FLOAT
            nr = ntohl(*(uint32_t *)(udp_send->payload + 1));

            for(int j = 0; j < udp_send->payload[5]; j++)
                n = n * 10;

            real = ((float) (nr)) / n;
            sign = udp_send->payload[0];
            if(sign) {
                // Signed
                real = real * (-1);
                sprintf(tcp_send.payload, "%lf", real);
            } else {
                // Unsigned
                sprintf(tcp_send.payload, "%lf", real);
            }

            strcpy(tcp_send.type, FLOAT);

            break;
        case 3:
            // STRING
            strcpy(tcp_send.payload, udp_send->payload);
            strcpy(tcp_send.type, STRING);

            break;
    }

    for(int i = 5; i <= max; ++i) {
    	for(int j = 0; j < clients[i].dim_topics; ++j) {
    		if (!strcmp(clients[i].topics[j].name, tcp_send.topic)) {
    			if(clients[i].connected){
                    // Sends the message to the clients
    				int res = send(clients[i].socket, &tcp_send, TCP_SIZE, 0);
    				DIE(res < 0, "[server] ERROR: Error while sending message\n");
    			} else {
                    // Adds the message at the end for clients with SF = 1
    				if(clients[i].topics[j].sf) {
    					clients[i].notsent[clients[i].dim_notsent++] = tcp_send;
    				}
    			}
    			break;
    		}
    	}
    }

    // Sending back
	*udp = udp_sock;
    *addr = udp_address; 
    *file = file_ds;
    *max_val = max;

}

void manage_server(int *tcp, int *udp, fd_set *file) {
	fd_set file_ds = *file, tmp_ds;
	int quit = 0;
	int res;
    // Index of the topic
    int idx;
	struct client *clients = calloc(MAX * 10, sizeof(struct client));
	struct sockaddr_in new_tcp, udp_address;

	// Creating copies of the sockets
	int udp_sock = *udp;
	int tcp_sock = *tcp;

	// Initialise descriptor sets
	FD_ZERO(&file_ds);
	FD_ZERO(&tmp_ds);

	FD_SET(tcp_sock, &file_ds);
	FD_SET(udp_sock, &file_ds);
	FD_SET(0, &file_ds);

	// Highest numbered file descriptor
	int max = maxim(tcp_sock, udp_sock);

	// Run until quit = 1
	while (!quit) {
		// Clone the set
		tmp_ds = file_ds;

		// Select the file
		res = select(max + 1, &tmp_ds, NULL, NULL, NULL);
		DIE(res < 0, "[server] ERROR: Could not select message\n");

		char buff[PACKLEN];

		// Find selected socket

		for (int i = 0; i <= max; ++i) {
			if(FD_ISSET(i, &tmp_ds)) {
				memset(buff, 0, PACKLEN);
				if (tcp_sock == i) {
                    // New TCP client
                    manage_tcp(&tcp_sock, &new_tcp, &max, clients, &file_ds);

				} else if (udp_sock == i) {
                    // New UDP message
                    manage_udp(&udp_sock, &udp_address, &max, clients, &file_ds);
				} else if (i == STDIN_FILENO) {
					fgets(buff, MAX, stdin);

					if(!strncmp(buff, EXIT, strlen(EXIT))) {
						quit = 1;
						break;
					}
				} else {
                    // The client that is already connected
                    memset(buff, 0, PACKLEN);
					int res = recv(i, buff, sizeof(buff), 0);
					DIE(res < 0, "[server] ERROR: Could not receive message\n");
					if(res) {
						struct Packet *in_pack = (struct Packet *) buff;
						client* c = NULL;

                        // Finding the client
						for(int j = 5; j <= max; j++) {
							if (i == clients[j].socket) {
								c = &clients[j];
								break;
							}
						}
                        // Testing for type
                        if (in_pack->type == TYPE_EXIT) {
							for (int j = 5; j <= max; j++) {
								if(clients[j].socket == i) {
									printf("Client %s disconnected.\n", clients[j].id);
									clients[j].connected = 0;
									clients[j].socket = -1;
									FD_CLR(i, &file_ds);
									close(i);
									break;
								}
							}
                        } else if (in_pack->type == TYPE_SUBSCRIBE) {
							idx = -1;
                            // Finding the index
							for(int j = 0; j < c->dim_topics; j++) {
								if (!strcmp(c->topics[j].name, in_pack->topic)) {
									idx = j;
									break;
								}
							}
                            // If the index wasn't found
							if(idx < 0) {
								strcpy(c->topics[c->dim_topics].name, in_pack->topic);
								c->topics[c->dim_topics].sf = in_pack->data_type;
								c->dim_topics++;
							}

						} else if(in_pack->type == TYPE_UNSUBSCRIBE) {
							idx = -1;
                            // Finding the index
							for(int j = 0; j < c->dim_topics; j++) {
								if(strcmp(c->topics[j].name, in_pack->topic) == 0) {
									idx = j;
									break;
								}
                            }
                            // If found
							if (idx >= 0) {
								for(int k = idx; k < c->dim_topics; k++)
									c->topics[k] = c->topics[k+1];
								c->dim_topics--;
							}
						}
					} else if (!res) {
						for (int j = 5; j <= max; j++) {
                            // Disconnecting the clients
							if(clients[j].socket == i) {
								printf("Client %s disconnected.\n", clients[j].id);
								clients[j].connected = 0;
								clients[j].socket = -1;
								FD_CLR(i, &file_ds);
								close(i);
								break;
							}
						}
					}
				}
			}

		}


	}

	// Sending back the sockets
	*udp = udp_sock;
	*tcp = tcp_sock;
	*file = file_ds;
}

void stop_server(int max, fd_set *file_ds) {
	for(int i = 3; i <= max; i++) {
		if(FD_ISSET(i, file_ds))
			close(i);
	}
}