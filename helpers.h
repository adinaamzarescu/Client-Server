#ifndef _HELPERS_H
#define _HELPERS_H 1

#define TRUE	1
#define FALSE	0

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#define maxim(x,y) ((x>y)?x:y)

#define PACKLEN sizeof(struct Packet)
#define MAX_MESSAGE sizeof(struct tcp_struct)

#define EXIT	"exit"
#define SUBSCRIBE	"subscribe"
#define UNSUBSCRIBE	"unsubscribe"

#endif
