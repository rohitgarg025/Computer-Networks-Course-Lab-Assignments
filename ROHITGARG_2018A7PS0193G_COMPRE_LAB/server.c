#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_PENDING 0
#define MAX_MESSAGE_LENGTH 32000

int main(int argc, char **argv)
{
    /* code */
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;

    //**************************************************************
    if (argc != 2)
    {
        printf("ERROR: Please provide valid command-line arguments!\n");
        exit(1);
    }
    int port_number = atoi(argv[1]);

    //**************************************************************
    int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server_socket == -1)
    {
        printf("[-]Server side socket creation failed!\n");
        return 0;
    }
    else
    {
        printf("[+]Socket created successfully\n");
    }

    bzero(&serveraddr, sizeof(serveraddr)); //initialise socket
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port_number);

    int bindsocket = bind(server_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (bindsocket < 0)
    {
        perror("[-]Binding Error: ");
        exit(0);
    }
    else
    {
        printf("[+]Bind Successful...\n");
    }

    // listen()
    //Moves server from closed state to listen state
    //server now passively listens and server socket is open to incoming connections
    int listenval = listen(server_socket, MAX_PENDING);
    if (listenval < 0)
    {
        perror("[+]Listening Error: ");
        exit(0);
    }
    else
    {
        printf("[+]Successfully listening....\n\n");
    }
    //**************************************************************

    while (1)
    {
        printf("[+]Initiating chat session....\n");
        int clientaddrlen = sizeof(clientaddr);
        int client_socket1 = accept(server_socket, (struct sockaddr *)&clientaddr, &clientaddrlen);
        printf("[+]Connected to client1 @ %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        int client_socket2 = accept(server_socket, (struct sockaddr *)&clientaddr, &clientaddrlen);
        printf("[+]Connected to client2 @ %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // ************************************************************************************

        if (!fork())
        {
            while (1)
            {
                //receive message from client 1
                // read the message from client and copy it in buffer
                char message[MAX_MESSAGE_LENGTH];
                int bytes_recv = recv(client_socket1, message, sizeof(message), 0);
                if (bytes_recv == 0){
                    printf("client1 exited\n");
                    exit(0);
                }
                if (bytes_recv < 0)
                {
                    perror("[-]Error: ");
                    break;
                }
                message[bytes_recv] = '\0';

                int bytes_sent = send(client_socket2, message, bytes_recv, 0);
                if (bytes_sent < 0)
                {
                    printf("[-]Error while sending message to client");
                    break;
                }
                
            }

            exit(0);
        }
        else
        {

            while (1)
            {
                //receive message from client 2
                // read the message from client and copy it in buffer
                char message[MAX_MESSAGE_LENGTH];
                int bytes_recv = recv(client_socket2, message, sizeof(message), 0);
                if (bytes_recv == 0){
                    printf("client2 exited\n");
                    break;
                }
                if (bytes_recv < 0)
                {
                    perror("[-]Error: ");
                    break;
                }
                message[bytes_recv] = '\0';

                int bytes_sent = send(client_socket1, message, bytes_recv, 0);
                if (bytes_sent < 0)
                {
                    printf("[-]Error while sending message to client");
                    break;
                }
                
            }

            // exit(0);
        }

        printf("[+]Chat session finished....clients exited successfully\n");
        close(client_socket1);
        close(client_socket2);
    }

    return 0;
}
