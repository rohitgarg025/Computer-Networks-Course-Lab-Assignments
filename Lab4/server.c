// C program to implement one side of FIFO
// This side reads first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	//make sfifo for reading messages sent by client
	mkfifo("sfifo", 0666);
	pid_t pid = fork();

	if (pid == 0)
	{
		// child process
		// read from sfifo
		while (1)
		{
			
			int sfile = open("sfifo", O_RDWR);
			char str1[512];
			read(sfile, str1, sizeof(str1));
			if (strcmp(str1, "exit") == 0)
			{
				close(sfile);
				unlink("sfifo");
				break;
			}
			printf("CLIENT: %s\n", str1);
			close(sfile);
		}
	}
	else
	{
		// parent process
		// write to cfifo
		while (1)
		{

			
			
			//take message from user
			char message[512];
			fgets(message, sizeof(message), stdin);
			message[strlen(message)-1] = '\0';

			if(strcmp(message,"exit") == 0){
				int temp = open("sfifo",O_RDWR);
				write(temp , message, sizeof(message));
				close(temp);
				break;
			}



			//write to cfifo
			int cfile = open("cfifo",O_RDWR);
			if(cfile==-1){
				printf("---Client Exited---\n");
			}
			write(cfile,message,sizeof(message));
			close(cfile);
		}
	}

	return 0;
}
