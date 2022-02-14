#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define LISTEN_BACKLOG	10
#define BUFF_SIZE	10

char *port = "9000";
char *file_path = "/var/tmp/aesdsocketdata";

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
	//int fd,nr,cl;

	struct addrinfo hints;
	struct addrinfo *results;
	struct sockaddr client_addr;
	socklen_t client_addr_size;
	char buff[BUFF_SIZE];

	//1. set the sockaddr using getaddrinfo
	//clear the hints first
	memset(&hints,0,sizeof(hints));
	//set all the hint parameters then
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;


	int getaddr_ret = getaddrinfo(NULL,port,&hints,&results);
	if(getaddr_ret != 0){
		printf("Error: getaddrinfo failed\n");
		syslog(LOG_ERR,"Error: getaddrinfo failed");
		exit(1);
	}

	//2. First open the socket
	printf("Opening socket.\n");
	sfd = socket(PF_INET6, SOCK_STREAM, 0);//IPv4,TCP,Any protocol
	if(sfd == -1){
		printf("Error: socket() failed\n");
		syslog(LOG_ERR,"Error: socket() failed");
		freeaddrinfo(results);
		exit(1);
	}


	//3. Bind / assign name to a socket
	printf("Binding the socket descriptor:%d.\n",sfd);
	int bind_ret = bind(sfd, results->ai_addr, results->ai_addrlen);
	if(bind_ret == -1){
		printf("Error: Bind failed\n");
		syslog(LOG_ERR,"Error: Bind failed");
		printf("bind error: %s\n",strerror(errno));
		freeaddrinfo(results);
		exit(1);
	}

	//4. Listen on the socket
	printf("Listening on socket.\n");
	int list_ret = listen(sfd, LISTEN_BACKLOG);
	if(list_ret == -1){
		printf("Error: Listen failed\n");
		syslog(LOG_ERR,"Error: Listen failed");
		freeaddrinfo(results);
		exit(1);
	}

	//5. accept the socket
	printf("Accepting connection.\n");
	int accept_fd = accept(sfd,(struct sockaddr *)&client_addr, &client_addr_size);
	if(accept_fd == -1){
		printf("Error: accept failed\n");
		printf("accept error: %s\n",strerror(errno));
		syslog(LOG_ERR,"Error: accept failed");
		freeaddrinfo(results);
		exit(1);
	}
	syslog(LOG_DEBUG,"Accepting connection from %s",client_addr.sa_data);
	printf("Accepting connection from %s\n",client_addr.sa_data);

	//6. Receive from socket

	// while(1){

		printf("Receiving data from descriptor:%d.\n",sfd);
		int recv_ret = recv(accept_fd, buff, BUFF_SIZE ,0); //**!check the flag
		if(recv_ret < 0){
			printf("Error: Receive failed\n");
			printf("recv error: %s\n",strerror(errno));
			syslog(LOG_ERR,"Error: Receive failed");
			exit(1);
		}

		printf("Client data:%s",buff);

		//7. Add data to the file after receiving data from client
		
		// //Create file
		// fd = creat(file_path, 0644);
		// if(fd == -1){
		// 	printf("Error: File could not be created!\n");
		// 	syslog(LOG_ERR,"Error: File could not be created!");
		// 	exit(1);
		// }

		// //Write file
		// nr = write(fd,buff,BUFF_SIZE);
		// if(nr == -1){
		// 	printf("Error: File could not be written!\n");
		// 	syslog(LOG_ERR,"Error: File could not be written!");
		// 	exit(1);
		// }else if(nr != BUFF_SIZE){
		// 	printf("Error: File partially written!\n");
		// 	syslog(LOG_ERR,"Error: File partially written!");
		// 	exit(1);
		// }
		// syslog(LOG_DEBUG,"Writing received data from client to file");

		// cl = close(fd);
		// if(cl == -1){
		// 	printf("Error: File could not be Closed!\n");
		// 	syslog(LOG_ERR,"Error: File could not be Closed!");
		// 	exit(1);
		// }

		//8. Send data to client
		int send_ret = send(accept_fd,buff,BUFF_SIZE,0);
		if(send_ret == -1){
			printf("Error: Data could not be sent\n");
			syslog(LOG_ERR,"Error: Data could not be sent");
			exit(1);
		}

		memset(buff,0,BUFF_SIZE);

	// }

	//9. Close sfd, accept_fd
	freeaddrinfo(results);
	//close(accept_fd);
	//close(sfd);

}
