#include "server.h"

int main(int argc, char** argv) {

	setvbuf(stdout, NULL, _IONBF, BUFSIZ);

	DIE(argc < 2, "[server] ERROR: Wrong number of arguments");

	int udp_sock;
	int tcp_sock;

	// Initialise sockets
	init_sockets(argv[1], &udp_sock, &tcp_sock);

	// Manage server
	fd_set file_descr;
	manage_server (&tcp_sock, &udp_sock, &file_descr);

	int max = maxim(tcp_sock, udp_sock);

	stop_server(max, &file_descr);

	close(udp_sock);
	close(tcp_sock);

	return 0;
}
