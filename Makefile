CFLAGS = -Wall -g -lm
PORT = 12363
IP_SERVER = 127.0.0.1

ID = C1

all: server subscriber

# Compileaza server.c
server: server.c server_functions.c

# Compileaza subscriber.c
subscriber: subscriber.c subscriber_functions.c

.PHONY: clean run_server run_subscriber

# Ruleaza serverul
run_server:
	./server ${PORT}

# Ruleaza clientul
run_subscriber:
	./subscriber $(ID) ${IP_SERVER} ${PORT}

clean:
	rm -f server subscriber subscriber_functions server_functions

