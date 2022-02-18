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

#define LISTEN_BACKLOG	10
#define BUFF_SIZE	100

char *port = "9000";
char *file_path = "/var/tmp/aesdsocketdata";
char *op_buffer = NULL;

void socket_open(void);

void sighandler(int sig_no){

	if(sig_no == SIGINT){
		printf("SIGINT detected!\n");
		unlink(file_path); //delete the file
		free(op_buffer);
	}else if(sig_no == SIGTERM){
		printf("SIGTERM detected!\n");
		unlink(file_path); //delete the file
		free(op_buffer);
	}

	exit(EXIT_SUCCESS);
}

int main()
{

	//open the log file
	openlog("aesdsocket-a5",LOG_PID,LOG_USER);

	//Write a test string to log
	syslog(LOG_DEBUG,"Syslog opened");

	/*register for signal*/
	signal(SIGINT,sighandler);
	signal(SIGTERM,sighandler);

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
	char buff[BUFF_SIZE] = {0};
	//char output_buff[BUFF_SIZE] = {0};

	memset(buff,0,BUFF_SIZE);

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

	int packet_size = 0;
	//char *op_buffer = NULL;

	//Create file
	int fd = creat(file_path, 0644);
	if(fd == -1){
		printf("Error: File could not be created!\n");
		syslog(LOG_ERR,"Error: File could not be created!");
		exit(1);
	}

	op_buffer = (char *) malloc(sizeof(char)*BUFF_SIZE);
	if(op_buffer == NULL){
		printf("Malloc failed!\n");
		exit(1);
	}

	memset(op_buffer,0,BUFF_SIZE);

	//close fd after creating
	close(fd);

	//free after use
	freeaddrinfo(results);

	while(1){

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

		bool packet_comp = false;
		int i;
		int recv_ret = 0;

		while(packet_comp == false){

			printf("Receiving data from descriptor:%d.\n",sfd);

			recv_ret = recv(accept_fd, buff, BUFF_SIZE ,0); //**!check the flag
			if(recv_ret < 0){
				printf("Error: Receive failed\n");
				printf("recv error: %s\n",strerror(errno));
				syslog(LOG_ERR,"Error: Receive failed");
				exit(1);
			}else if(recv_ret == 0){
				break;
			}

			//Following line giving valgrind error
			//printf("Client data:%s bytes received:%d\n",buff,recv_ret);

			for(i = 0;i < BUFF_SIZE;i++){

				if(buff[i] == 10){
					packet_comp = true;
					i++;
					break;
				}

			}
			packet_size += i;

			//reallocate to a larger buffer now
			op_buffer = (char *) realloc(op_buffer,(packet_size+1));
			if(op_buffer == NULL){
				printf("Realloc failed\n");
				exit(1);
			}

			strncat(op_buffer,buff,i);

			memset(buff,0,BUFF_SIZE);
		}

		//Write this data to the file first
		//Open in append mode first
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

		//Open file for reading for sending to client
		char *read_data_buf = NULL;//dont need to malloc this, is inefficient
		read_data_buf = (char *) malloc(sizeof(char) * strlen(op_buffer));

		fd = open(file_path,O_RDONLY);
		if(fd == -1){
			printf("File open error for reading\n");
			exit(1);
		}

		//read the file data in a buffer
		int rd = read(fd, read_data_buf, strlen(op_buffer));
		if(rd == -1){
			printf("Reading from file failed!\n");
			printf("file read error: %s\n",strerror(errno));
			exit(1);
		}

		//8. Send to client data of the read file buffer
		printf("Writing data: %s to :%d\n\n",op_buffer,accept_fd);

		int send_ret = send(accept_fd,op_buffer,strlen(op_buffer),0);
		
		if(send_ret == -1){
			printf("Error: Data could not be sent\n");
			syslog(LOG_ERR,"Error: Data could not be sent");
			exit(1);
		}

		close(fd);
		free(read_data_buf);
		//free(op_buffer);
		//close(accept_fd);
	}
	
	//free malloced data
	free(op_buffer);

	//9. Close sfd, accept_fd
	freeaddrinfo(results);
	//close(accept_fd);
	close(sfd);

}

// void write_to_file(){

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

// }

//receive and send byte by byte

// while(1){

	// 	printf("Receiving data from descriptor:%d.\n",accept_fd);
	// 	int recv_ret = recv(accept_fd, &input, 1 ,0); //**!check the flag
	// 	if(recv_ret < 0){
	// 		printf("Error: Receive failed\n");
	// 		printf("recv error: %s\n",strerror(errno));
	// 		syslog(LOG_ERR,"Error: Receive failed");
	// 		exit(1);
	// 	}else if(recv_ret == 0){
	// 		break;
	// 	}

	// 	printf("Client data:%d bytes received:%d\n\n",input,recv_ret);

	// 	strcat(buff,&input);

	// 	//8. Send data to client after receiving packet
	// 	if(input == 10){//line feed received

	// 		printf("buff_count:%ld\n",strlen(buff));
	// 		printf("Writing data %s to :%d\n",buff,accept_fd);
	// 		int send_ret = send(accept_fd,buff,strlen(buff),0);
	// 		if(send_ret == -1){
	// 			printf("Error: Data could not be sent\n");
	// 			syslog(LOG_ERR,"Error: Data could not be sent");
	// 			exit(1);
	// 		}
	// 		printf("send length:%d\n",send_ret);
	// 	}

	// 	//memset(buff,0,BUFF_SIZE);

	// }