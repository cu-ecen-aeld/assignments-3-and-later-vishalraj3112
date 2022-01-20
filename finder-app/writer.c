#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char const *argv[])
{
	int fd,nr;

	char const *cmd_input[argc];
	char const *writefile = NULL;
	char const *writestr = NULL;

	for(int i = 0; i < argc; i++){
		*(cmd_input+i) = argv[i];
	}

	//open the log file
	openlog("writer-a2",LOG_PID,LOG_USER);

	//Test for seeing the arguments
	// for(int i = 0; i < argc; i++){
	// 	printf("%s\n",*(cmd_input+i));
	// }
	//Remove in final code

	if(argc < 3){
		printf("Error: Wrong arguments!\n");
		exit(1);
	}

	writefile = *(cmd_input+1);
	writestr = *(cmd_input+2);

	// printf("Writefile:%s\n",writefile);
	// printf("writestr:%s\n",writestr);
	// printf("size of writestr:%ld\n",strlen(writestr));

	//Create file
	fd = creat(writefile, 0644);
	if(fd == -1){
		printf("Error: File could not be created!");
		syslog(LOG_ERR,"Error: File could not be created");
		exit(1);
	}

	//Write file
	nr = write(fd,writestr,strlen(writestr));
	if(nr == -1){
		printf("Error: File could not be written!");
		syslog(LOG_ERR,"Error: File could not be written");
		exit(1);
	}else if(nr != strlen(writestr)){
		printf("Error: File partially written! ");
		exit(1);
	}
	syslog(LOG_DEBUG,"Writing %s to %s",writestr,writefile);
	closelog();

	return 0;
}