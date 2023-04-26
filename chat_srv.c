#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>

#define TRUE 1
#define FALSE 0

int buff_add_prefix(char* buf, size_t buf_size, char* prefix)
{
	int len_prefix = strlen(prefix);
	int len_buf = strlen(buf);

	if (len_prefix + len_buf >= buf_size)
		return -1;
	for(int i = len_buf; i >= 0; --i) {
		buf[i+len_prefix] = buf[i];
	}
	for(int i = 0; i<len_prefix; ++i) {
		buf[i] = prefix[i];
	}
	return 0;
}

int main()
{
	const char *port = "49152";		/* available port */
	const char *welcome_msg = "Type 'close' to disconnect; 'shutdown' to stop\n";
	const int sz_hostname = 32;
	char hostname[sz_hostname];
	char buffer[BUFSIZ];
	const int MAX_CONNECTS = 10;			/* also max connections */
	char connections[MAX_CONNECTS][sz_hostname];	/* storage for IPv4 connections */
	socklen_t address_len = sizeof(struct sockaddr);
	struct addrinfo hints, *server;
	struct sockaddr address;
	int r,fd,done;
	fd_set main_fds, read_fds;
	int serverfd,clientfd;

	/* Step0: configure the server */
	memset( &hints, 0, sizeof(struct addrinfo));	/* use memset_s() */
	hints.ai_family = AF_INET;			/* IPv4 */
	hints.ai_socktype = SOCK_STREAM;	/* TCP */
	hints.ai_flags = AI_PASSIVE;		/* accept any */
	r = getaddrinfo( 0, port, &hints, &server );
	if( r!=0 )
	{
		perror("failed");
		exit(1);
	}

	/* Step1: create a socket */
	serverfd = socket(server->ai_family,server->ai_socktype,server->ai_protocol);
	if( serverfd==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* Step2: bind to a port */
	r = bind(serverfd,server->ai_addr,server->ai_addrlen);
	if( r==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* Step3: listen for a connection*/
	puts("TCP Server is listening...");
	r = listen(serverfd,MAX_CONNECTS);
	if( r==-1 )
	{
		perror("failed");
		exit(1);
	}

	/* Step4: deal with multiple connections */
	FD_ZERO(&main_fds);			/* initialize file descriptor set */
	FD_SET(serverfd, &main_fds);	/* set the server's file descriptor */
	/* endless loop to process the connections */
	done = FALSE;
	while(!done)
	{
		/* backup the main set into a read set for processing */
		read_fds = main_fds;

		/* Step4.1: scan the connections for any activity */
		r = select(MAX_CONNECTS+1,&read_fds, NULL, NULL, 0);

		/* Step4.2 select() woke up. Identify the fd that has events */
		if( r < 0 )
		{
			perror("Select failed");
			exit(-1);
		}

		/* process any connections */
		for( fd=1; fd<=MAX_CONNECTS; fd++)
		{
			/* filter only active or new clients */
			if( FD_ISSET(fd, &read_fds) )	/* returns true for any fd waiting */
			{
				/* Step4.2.1 filter out the server, which indicates a new connection */
				if( fd==serverfd )
				{
					/* add the new client */
					clientfd = accept(serverfd,&address,&address_len);
					if( clientfd < 0 )
					{
						perror("Socket accept failed");
						exit(-1);
					}
					/* connection accepted, get name */
					r = getnameinfo(&address,address_len,hostname,sz_hostname,0,0,NI_NUMERICHOST);
					/* update array */
					strcpy(connections[clientfd],hostname);
					printf("New connection from %s\n",connections[clientfd]);

					/* add new client socket to the master list */
					FD_SET(clientfd, &main_fds);

					/* respond to the connection */
					strcpy(buffer,"Hello to ");
					strcat(buffer,connections[clientfd]);
					strcat(buffer,"!\n");
					strcat(buffer,welcome_msg);
					send(clientfd,buffer,strlen(buffer),0);
				} /* end if, add new client */
				/* the current fd has incoming info - deal with it */
				else
				{
					/* read the input buffer for the current fd */
					r = recv(fd,buffer,BUFSIZ,0);
					/* if nothing received, disconnect them */
					if( r<1 )
					{
						/* clear the fd and close the connection */
						FD_CLR(fd, &main_fds);	/* reset in the list */
                        connections[fd][0] = '\0'; /* clear hostname */
						close(fd);				/* disconnect */
						/* update the screen */
						printf("%s closed\n",connections[fd]);
					}
					/* something has been received */
					else
					{
						buffer[r] = '\0';		/* terminate the string */
						/* if 'shutdown' sent... */
						if( strcmp(buffer,"shutdown\n")==0 )
							done = TRUE;		/* terminate the loop */
						/* otherwise, echo back the text */
						else {
							char prefix[sz_hostname + 8];
                            strcpy(prefix, "From ");
                            strcat(prefix, connections[fd]);
                            strcat(prefix, ": ");
							int ret = buff_add_prefix(buffer,BUFSIZ,prefix);
							if (ret < 0) {
								perror("Size is too big, over BUFSIZ!");
								exit(-1);
							}
                            for (int fd=0; fd<MAX_CONNECTS; ++fd) {
                                if(strlen(connections[fd]) > 0)
                                    send(fd,buffer,strlen(buffer),0);
                            }
						}
					}
				} /* end else to send/recv from client(s) */
			} /* end if */
		} /* end for loop through connections */
	} /* end while */

	puts("TCP Server shutting down");
	close(serverfd);
	freeaddrinfo(server);
	return(0);
}
