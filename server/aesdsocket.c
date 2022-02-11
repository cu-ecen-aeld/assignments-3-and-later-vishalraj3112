#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netdb.h>

#define LISTEN_BACKLOG	50
#define BUFF_SIZE	500

char *port = "9000";

void socket_open(void);

int main()
{

	//open the log file
	openlog("aesdsocket-a5",LOG_PID,LOG_USER);

	//Write a test string to log
	syslog(LOG_DEBUG,"Syslog opened");

	socket_open();

	//closing syslog
	closelog();

	
	return 0;
}

void socket_open()
{
	//defining socket file descriptor
	int sfd = 0;

	struct addrinfo hints;
	struct addrinfo *results;
	struct sockaddr client_addr;
	socklen_t client_addr_size;
	char buff[BUFF_SIZE];

	//1. First open the socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);//IPv4,TCP,Any protocol
	if(sfd == -1){
		printf("Error: socket() failed\n");
		syslog(LOG_ERR,"Error: socket() failed");
		exit(1);
	}

	//2. set the sockaddr using getaddrinfo
	//clear the hints first
	memset(&hints,0,sizeof(struct addrinfo));
	//set all the hint parameters then
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;

	int getaddr_ret = getaddrinfo(NULL,port,&hints,&results);
	if(getaddr_ret != 0){
		printf("Error: getaddrinfo failed\n");
		syslog(LOG_ERR,"Error: getaddrinfo failed");
		exit(1);
	}

	//3. Bind / assign name to a socket
	int bind_ret = bind(sfd, results->ai_addr, results->ai_addrlen);
	if(bind_ret == -1){
		printf("Error: Bind failed\n");
		syslog(LOG_ERR,"Error: Bind failed");
		exit(1);
	}

	//addrinfo no longer needed
	free(results);


	//4. Listen on the socket
	int list_ret = listen(sfd, LISTEN_BACKLOG);
	if(list_ret == -1){
		printf("Error: Listen failed\n");
		syslog(LOG_ERR,"Error: Listen failed");
		exit(1);
	}

	//5. accept the socket
	int accept_ret = accept(sfd,(struct sockaddr *) &client_addr, &client_addr_size);
	if(accept_ret == -1){
		printf("Error: accept failed\n");
		syslog(LOG_ERR,"Error: accept failed");
		exit(1);
	}

	//6. Receive from socket
	ssize_t recv_ret = recv(sfd, buff, BUFF_SIZE ,MSG_CMSG_CLOEXEC); //**!check the flag
	if(recv_ret == -1){
		printf("Error: Receive failed\n");
		syslog(LOG_ERR,"Error: Receive failed");
		exit(1);
	}

	//7. Close sfd

}
