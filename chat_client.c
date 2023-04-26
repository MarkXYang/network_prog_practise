#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <pthread.h>

void *socket_rx(void *fd)
{
	char buf[BUFSIZ];
		
	while(1) {
		int r= recv(*(int*)fd, buf, BUFSIZ,0);
		if( r<1 )
		{
			puts("Connection closed by peer");
			exit(-1);
		}
		buf[r] = '\0';
		printf(">%s\n", buf);
	}
}
int main(int argc, char *argv[])
{
	const char *port = "49152";
	char *server;
	struct addrinfo hints,*host;
	int r,sockfd;
	char buffer[BUFSIZ];

	if( argc<2 )
	{
		fprintf(stderr,"Format: client hostname\n");
		exit(1);
	}
	server = argv[1];

	/* obtain and convert server name and port */
	printf("Looking for server on %s...",server);
	memset( &hints, 0, sizeof(hints) );		/* use memset_s() */
	hints.ai_family = AF_INET;				/* IPv4 */
	hints.ai_socktype = SOCK_STREAM;		/* TCP */
	r = getaddrinfo( server, port, &hints, &host );
	if( r!=0 )
	{
		perror("failed");
		exit(1);
	}
	puts("found");

	/* create a socket */
	sockfd = socket(host->ai_family,host->ai_socktype,host->ai_protocol);
	if( sockfd==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* connect to the socket */
	r = connect(sockfd,host->ai_addr,host->ai_addrlen);
	if( r==-1 )
	{
		perror("failed");
		exit(1);
	}

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, socket_rx, &sockfd);
	/* loop to interact with the server */
	while(1)
	{
		/* local input  */
		fgets(buffer,BUFSIZ,stdin);
		if( strncmp(buffer,"close",5)==0 )
		{
			puts("Closing connection");
			pthread_exit(NULL);
			break;
		}
		send(sockfd,buffer,strlen(buffer),0);
	}	/* end while loop */

	/* all done, clean-up */
	freeaddrinfo(host);
	close(sockfd);
	puts("Disconnected");

	return(0);
}
