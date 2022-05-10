#include "subscriber.h"

int main(int argc, char** argv) {

	int res;

	setvbuf(stdout, NULL, _IONBF, BUFSIZ);

	DIE(argc < 4, "[subscriber] ERROR: Usage.\n");

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "[subscriber] ERROR: Could not open socket\n");

	short port = atoi(argv[3]);
	DIE(port <= 0, "[subscriber] ERROR: Invalid port");

	// Fill the sockaddr struct
	struct sockaddr_in server_data;
	server_data.sin_family = AF_INET;
	server_data.sin_port = htons(port);
	res = inet_aton(argv[2], &server_data.sin_addr);
	DIE(!res, "[subscriber] ERROR: Error in function 'inet_aton'");

	// Connect to server
	res = connect(sockfd, (struct sockaddr *)&server_data, sizeof(server_data));
	DIE(res < 0, "[subscriber] ERROR: Connect");

	// Send message to the server
	char buff[BUFLEN] = {0};
	sprintf(buff, "connect %s", argv[1]);

	res = send(sockfd, argv[1], strlen(buff), 0);
	DIE(res < 0, "[subscriber] ERROR: Send");

	fd_set read_file;
	fd_set tmp_file;
	FD_ZERO(&read_file);
	FD_ZERO(&tmp_file);

	FD_SET(sockfd, &read_file);
	FD_SET(STDIN_FILENO, &read_file);

	struct Packet pack;
	// Disable neagle
	disable_neagle(&sockfd);

	while(TRUE) {
		// Clone the set
		tmp_file = read_file;

		int sel = select(sockfd + 1, &tmp_file, NULL, NULL, NULL);
		DIE(sel < 0, "[subscriber] ERROR: Could not select message\n");
		
		memset(buff, 0, BUFLEN);

		// Comanda la stdin
		if (FD_ISSET(0, &tmp_file)) {
			char buff[MAX];
			memset(buff, 0, MAX);
			fgets(buff, MAX, stdin);
			memset(&pack, 0, PACKLEN);

			int check = check_command (buff, pack, sockfd);
			if (!check) {
				printf("Invalid cmd.\n");
			} else if (check == 1) {
				printf("Subscribed to topic.\n");
			} else if  (check == 2) {
				printf("Unsubscribed to topic.\n");
			} else if (check < 0) {
				break;
			}

		}
		// Server message
		if(FD_ISSET(sockfd, &tmp_file)) {
			char buff[MAX_MESSAGE] = {0};

			res = recv(sockfd, buff, MAX_MESSAGE, 0);
			DIE(res < 0, "[subscriber] ERROR: Error while receiving message.");

			// The server is down
			if (!res)
				break;
			print_tcp_pack ((struct tcp_struct *)buff);
		}
	}
	
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	
	return 0;
}

