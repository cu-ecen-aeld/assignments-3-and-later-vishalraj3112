/***********************************************************************************************************************
* File Name    : aesdsocket.c
* Project      : AESD Assignment 5
* Version      : 1.0
* Description  : Contains all the function implementation code for socket server application.
* Author       : Vishal Raj
* Creation Date: 2.18.22
* References   : https://lloydrochester.com/post/c/unix-daemon-example/, Linux Man pages,
*                https://www.qnx.com/developers/docs/6.5.0SP1.update/com.qnx.doc.neutrino_lib_ref/i/inet_ntop.html,
*                https://www.binarytides.com/socket-programming-c-linux-tutorial/.
***********************************************************************************************************************/
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
#include <stdbool.h>
#include <signal.h>
#include <arpa/inet.h>

#define LISTEN_BACKLOG	10
#define BUFF_SIZE	100

char *port = "9000";
char *file_path = "/var/tmp/aesdsocketdata";
char *op_buffer = NULL;

//defining socket file descriptor
int sfd = 0;
int accept_fd = 0;

//Function prototypes
void socket_open(void);

/***********************************************************************************************
* Name          : sighandler
* Description   : used to catch the particular signal and perform graceful shutdown.
* Parameters    : sig_no - the signal received to be handled.
* RETURN        : None
***********************************************************************************************/
void sighandler(int sig_no){

	//handle the particular signal
	switch(sig_no)
	{
		case SIGINT:
		case SIGTERM:
		case SIGKILL:
			printf("SIGINT/SIGTEM/SIGKILL detected!\n");
			syslog(LOG_DEBUG,"Caught signal, exiting");
			unlink(file_path); //delete the file
			free(op_buffer);
			close(accept_fd);
			close(sfd);
			break;

	}

	exit(EXIT_SUCCESS);
}

/***********************************************************************************************
* Name          : main
* Description   : used to intialize syslog, signal and call other functions.
* Parameters    : argc- command line argument count
*                 argv[] - command line argument content.
* RETURN        : exit status of program
***********************************************************************************************/
int main(int argc, char *argv[])
{

	//open the log file
	openlog("aesdsocket-a5",LOG_PID,LOG_USER);

	//Write a test string to log
	syslog(LOG_DEBUG,"Syslog opened");

	/*register for signal*/
	signal(SIGINT,sighandler);
	signal(SIGTERM,sighandler);
	signal(SIGKILL,sighandler);

	//Check the actual value or argv here:
	if((argc > 1) && (!strcmp("-d",(char*)argv[1]))){
	
	//if(argc > 1){
		printf("Entering daemon mode!\n");
		syslog(LOG_DEBUG,"aesdsocket entering daemon mode");
		
		//Enter daemon mode
		int d_ret = daemon(0,0);
		if(d_ret == -1){
			printf("Entering daemon mode failed!\n");
			syslog(LOG_DEBUG,"Entering daemon mode failed!");
			printf("daemon error: %s\n",strerror(errno));
		}
	}

	socket_open();

	//closing syslog
	closelog();

	return 0;
}

/***********************************************************************************************
* Name          : socket_open
* Description   : perform all the server socket configuration related steps, read packets from 
*                 client, write packet to the file and sends data back to client.
* Parameters    : None
* RETURN        : None
***********************************************************************************************/
void socket_open(void)
{
	//defining all socket related resources
	struct addrinfo hints;
	struct addrinfo *results;
	struct sockaddr client_addr;
	socklen_t client_addr_size;
	char buff[BUFF_SIZE] = {0};
	char ipv_4[INET_ADDRSTRLEN];

	memset(buff,0,BUFF_SIZE);

	//1. Set the sockaddr using getaddrinfo
	
	//clear the hints first
	memset(&hints,0,sizeof(hints));
	
	//set all the hint parameters then
	hints.ai_flags = AI_PASSIVE;

	//store the result 
	int getaddr_ret = getaddrinfo(NULL,port,&hints,&results);
	if(getaddr_ret != 0){
		printf("Error: getaddrinfo failed\n");
		syslog(LOG_ERR,"Error: getaddrinfo failed");
		exit(1);
	}

	//2. First open the socket
	printf("Opening socket.\n");
	sfd = socket(PF_INET, SOCK_STREAM, 0);//IPv4,TCP,Any protocol
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

	//variables required for packet detection
	int packet_size = 0;
	char c = 0;

	//Create file
	int fd = creat(file_path, 0644);
	if(fd == -1){
		printf("Error: File could not be created!\n");
		syslog(LOG_ERR,"Error: File could not be created!");
		exit(1);
	}

	//close fd after creating
	close(fd);

	//free after use
	freeaddrinfo(results);

	while(1){

		//make the buffer required for client input storage
		op_buffer = (char *) malloc(sizeof(char)*BUFF_SIZE);
		if(op_buffer == NULL){
			printf("Malloc failed!\n");
			exit(1);
		}

		memset(op_buffer,0,BUFF_SIZE);

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
		client_addr_size = sizeof(struct sockaddr);

		printf("Accepting connection.\n");
		accept_fd = accept(sfd,(struct sockaddr *)&client_addr, &client_addr_size);
		if(accept_fd == -1){
			printf("Error: accept failed\n");
			printf("accept error: %s\n",strerror(errno));
			syslog(LOG_ERR,"Error: accept failed");
			freeaddrinfo(results);
			exit(1);
		}

		//Below logic required to get ip address in readable format.
		struct sockaddr_in *addr = (struct sockaddr_in *)&client_addr;

		inet_ntop(AF_INET, &(addr->sin_addr),ipv_4,INET_ADDRSTRLEN);

		//Logging the client connection and address
		syslog(LOG_DEBUG,"Accepting connection from %s",ipv_4);
		printf("Accepting connection from %s\n",ipv_4);

		//Package storage related variables
		bool packet_comp = false;
		int i;
		int recv_ret = 0;

		/*Packet reception, detection and storage logic*/
		while(packet_comp == false){

			//printf("Receiving data from descriptor:%d.\n",sfd);

			recv_ret = recv(accept_fd, buff, BUFF_SIZE ,0); //**!check the flag
			if(recv_ret < 0){
				printf("Error: Receive failed\n");
				printf("recv error: %s\n",strerror(errno));
				syslog(LOG_ERR,"Error: Receive failed");
				exit(1);
			}else if(recv_ret == 0){
				break;
			}

			/*Detect '\n' or ASCII value 
			  10 in the packet.
			*/
			for(i = 0;i < BUFF_SIZE;i++){

				if(buff[i] == 10){
					packet_comp = true;
					i++;
					break;
				}

			}
			packet_size += i;

			/*reallocate to a larger buffer now as static buffer can
			  only accomodate upto fixed size*/
			op_buffer = (char *) realloc(op_buffer,(packet_size+1));
			if(op_buffer == NULL){
				printf("Realloc failed\n");
				exit(1);
			}

			strncat(op_buffer,buff,i);

			memset(buff,0,BUFF_SIZE);
		}

		/*Write the data received from client to the 
		file first & open in append mode*/
		fd = open(file_path,O_APPEND | O_WRONLY);
		if(fd == -1){
			printf("File open error for appending\n");
			exit(1);
		}

		int nr = write(fd,op_buffer,strlen(op_buffer));
		if(nr == -1){
			printf("Error: File could not be written!\n");
			syslog(LOG_ERR,"Error: File could not be written!");
			exit(1);
		}else if(nr != strlen(op_buffer)){
			printf("Error: File partially written!\n");
			syslog(LOG_ERR,"Error: File partially written!");
			exit(1);
		}

		close(fd);

		/*Below is the logic for reading the data from the
		  input file byte by byte and sending to client.
		*/
		memset(buff,0,BUFF_SIZE);

		fd = open(file_path,O_RDONLY);
		if(fd == -1){
			printf("File open error for reading\n");
			exit(1);
		}

		//sending data byte-by-byte
		for(int i=0;i<packet_size;i++){

			//read the file data in a buffer
			int rd = read(fd, &c, 1);
			if(rd == -1){
				printf("Reading from file failed!\n");
				printf("file read error: %s\n",strerror(errno));
				exit(1);
			}

			int send_ret = send(accept_fd,&c,1,0);
			if(send_ret == -1){
				printf("Error: Data could not be sent\n");
				syslog(LOG_ERR,"Error: Data could not be sent");
				exit(1);
			}
		}

		close(fd);

		//Free the allocated buffer
		free(op_buffer);

		syslog(LOG_DEBUG,"Closed connection from %s",ipv_4);
		printf("Closed connection from %s\n",ipv_4);
	}

	//9. Close sfd, accept_fd
	close(accept_fd);
	close(sfd);

}
//[EOF]