/*
 * =====================================================================================
 *
 *       Filename:  udp_client.c
 *
 *    Description: UDP client to send a string to server, and receive the feedback from
 *    						 server.
 *
 *        Version:  1.0
 *        Created:  04/21/2023 12:24:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mark Yang
 *        Company:  Ontario, Canada
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>

void strip_str(char* str) {
	char* end;

	/* Trim leading whitespace */
	while(isspace(*str)) {
		str++;
	}

	/* Trim trailing whitespace */
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) {
		end--;
	}

	*(end + 1) = '\0';
}

int main(int argc, char* argv[])
{
	struct addrinfo hints, *host;
	int r, sockfd;
	const int size = 256;
	char buf[size];

	if (argc < 2) {
		printf("Usage: %s <input_str>\n", argv[0]);
		exit(-1);
	}
	else {
		strip_str(argv[1]);
	}

	/* configuration the remote address */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	r = getaddrinfo(0, "49152", &hints, &host);
	if (r != 0) {
		perror("Failed of getaddrinfo");
		exit(-1);
	}

	/* create the socket */
	sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
	if (sockfd < 0) {
		perror("Failed to create socket");
		exit(-1);
	}

	/* Send data to server */
	r = sendto(sockfd, argv[1], strlen(argv[1]), 0, host->ai_addr, host->ai_addrlen);

	r = recvfrom(sockfd, buf, size, 0, host->ai_addr, &host->ai_addrlen);

	if (r < 0) {
		perror("Faile to receive from server");
		exit(-1);
	}
	else {
		buf[r] = '\0';
		printf("%s\n", buf);
	}

	/* clean up */
	freeaddrinfo(host);
	close(sockfd);
}

