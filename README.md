## CLIENT - SERVER
### Copyright Adina-Amzarescu, 321CA

This project simulates a newsletter application. The main implementation
was the server.

* Server

The server makes the connection between a sender (UDP) and clients (TCP).
The clients are the subscribers.

It starts by opening the 2 sockets (for which I used FD) udp and tcp.
init_sockets will initialise them. You can find this function in server.functions.c.
There I checked the port, then filled the sockaddr structure. After this I applied
the bind function and set the TCP socket as the listening one.

manage_server has a while that will run until we exit manually. Inside this infinite
loop I checked for the type of socket. If it is a TCP socket this means a new TCP
client was received. If it is an udp socket then it is a new message and else it's a
client that is already connected. Here I test for exit, if i == 0 then the while will stop,
quit becomes TRUE.

* Subscriber
 
 They will connect to the server, sends their id when they connect and
 waits for stdin commands. The 3 possible commands are:
  * subscribe
  * unsubscribe
  * exit

