/***********************************************************************************************************************
* File Name    : writer.c
* Project      : AESD Assignment 2
* Version      : 1.0
* Description  : Contains all the function implementation code for writer application.
* Author       : Vishal Raj
* Creation Date: 1.19.22
***********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

//declaring prototype
void write_file(char const *writefile, char const *writestr);

/***********************************************************************************************
* Name          : main
* Description   : used to call other sub-functions and read command line arguments.
* Parameters    : argc-no of command line arguments, argv-pointer to the command line 
*                 arguments.
* RETURN        : success exit code
***********************************************************************************************/
int main(int argc, char const *argv[])
{
	char const *cmd_input[argc];

	for(int i = 0; i < argc; i++){
		*(cmd_input+i) = argv[i];
	}

	//open the log file
	openlog("writer-a2",LOG_PID,LOG_USER);

	if(argc < 3){
		printf("Error: Wrong arguments!\n");
		syslog(LOG_ERR,"Error: Wrong arguments!");
		exit(1);
	}

	write_file(*(cmd_input+1),*(cmd_input+2));


	return 0;
}

/***********************************************************************************************
* Name          : write_file
* Description   : used to create and write to file, handle error conditions and also perform
*                 syslog functions.
* Parameters    : argc-no of command line arguments, argv-pointer to the command line 
*                 arguments.
* RETURN        : success exit code
***********************************************************************************************/
void write_file(char const *writefile, char const *writestr)
{
	int fd,nr,cl;

	//Create file
	fd = creat(writefile, 0644);
	if(fd == -1){
		printf("Error: File could not be created!\n");
		syslog(LOG_ERR,"Error: File could not be created!");
		exit(1);
	}

	//Write file
	nr = write(fd,writestr,strlen(writestr));
	if(nr == -1){
		printf("Error: File could not be written!\n");
		syslog(LOG_ERR,"Error: File could not be written!");
		exit(1);
	}else if(nr != strlen(writestr)){
		printf("Error: File partially written!\n");
		syslog(LOG_ERR,"Error: File partially written!");
		exit(1);
	}
	syslog(LOG_DEBUG,"Writing %s to %s",writestr,writefile);

	cl = close(fd);
	if(cl == -1){
		printf("Error: File could not be Closed!\n");
		syslog(LOG_ERR,"Error: File could not be Closed!");
		exit(1);
	}

	closelog();

}

//[EOF]