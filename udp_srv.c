/*
 * =====================================================================================
 *
 *       Filename:  udp_srv.c
 *
 *    Description: An UDP server, which will receive any string, and revert the string,
 *    						 then send it back to client
 *
 *        Version:  1.0
 *        Created:  04/21/2023 11:52:45 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mark Yang
 *        Company:  Missisauga, ON. Canada
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

void str_reverse(char* buf)
{
	int len = strlen(buf);
	char c;
	int i;

	for(i = 0; i < (len/2); ++i) {
		buf[i] = buf[i] ^ buf[len - i - 1];
		buf[len - i - 1] = buf[i] ^ buf[len - i - 1];
		buf[i] = buf[i] ^ buf[len - i - 1];
	}
}

int main()
{
	struct addrinfo hints, *server;
	struct sockaddr client;
	socklen_t client_size;
	int r, sockfd;
	const char *buf = "Hello from UDP Server!\n";
	const int size = 256;
	char input[size];

	/* Configure the server */
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET; /* IPv4 */
	hints.ai_socktype = SOCK_DGRAM; /* UDP, datagram */
	hints.ai_flags = AI_PASSIVE; /* accept any connection */
	r = getaddrinfo(0, "49152", &hints, &server);
	if ( r != 0 ) {
		perror("Failed to call getaddrinfo");
		exit(1);
	}

	/* create socket for UDP server */
	sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
	if(sockfd < 0) {
		perror("Failed to create socket");
		exit(-1);
	}

	/* bind the server to a socket */
	r = bind(sockfd, server->ai_addr, server->ai_addrlen);
	if (r != 0) {
		perror("failed");
		exit(-1);
	}
	puts("UDP server is listening ...\n");

	client_size = sizeof(struct sockaddr);
	r = recvfrom(sockfd, input, size, 0, &client, &client_size);
	if (r < 0 ) {
		perror("Failed to receive from client");
		exit(-1);
	}

	str_reverse(input);

	r = sendto(sockfd, input, strlen(input), 0, &client, client_size);
	if (r < 0) {
		perror("Failed to send to client");
		exit(-1);
	}

	/* clean up */
	freeaddrinfo(server);
	close(sockfd);

	return(0);
}


