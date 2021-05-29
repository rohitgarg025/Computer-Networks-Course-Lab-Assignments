// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	//make cfifo for reading messages sent by client
	mkfifo("cfifo", 0666);
	

	pid_t pid = fork();
	if (pid == 0)
	{
		// child process
		// write to sfifo
		while (1)
		{
			//take message from user
			
			//take message from user
			char message[512];
			fgets(message, sizeof(message), stdin);
			message[strlen(message)-1] = '\0';

			//write to sfifo
			if (strcmp(message, "exit") == 0)
			{
				//if server sent exit then open your fifo
				//write exit in your fifo
				int temp = open("cfifo", O_RDWR);
				write(temp, message, sizeof(message));
				close(temp);
				break;
			}

			int sfile = open("sfifo", O_RDWR);
			if (sfile == -1)
			{
				printf("---Server Exited ---\n");
			}
			write(sfile, message, sizeof(message));
			close(sfile);
		}
	}
	else
	{
		// parent process
		// read from cfifo
		while (1)
		{
			char str1[512]; //to read from client's fifo

			int cfile = open("cfifo", O_RDWR);
			read(cfile, str1, sizeof(str1));
			if (strcmp(str1, "exit") == 0)
			{
				//if exit message in your fifo..means exit and unlink your own file
				close(cfile);
				unlink("cfifo");
				break;
			}

			printf("SERVER: %s\n", str1);
			close(cfile);
		}
	}
	return 0;
}
