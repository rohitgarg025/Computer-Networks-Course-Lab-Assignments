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
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// #define SERVER_PORT_NUMBER atoi(argv[1])
// #define IP_ADDR "127.0.0.1"

#define MAX_PENDING 0
#define MAX_MESSAGE_LENGTH 1024

int client_socket; //connection socket file descriptor
int server_socket;


void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    char text[MAX_MESSAGE_LENGTH];
    strcpy(text, "exitServer");
    int bytes_sent = send(client_socket, text, strlen(text), 0);
    printf("\nServer Terminated\n");
    close(client_socket);
    close(server_socket);

    exit(0);
    fflush(stdout);
}

int main(int argc, char const *argv[])
{
    // Handles Ctrl+C interrupt to exit server and eventually client
    signal(SIGINT, sigintHandler);

    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    char message[MAX_MESSAGE_LENGTH]; //message is the string passed by client to server
    int port_number;                  //SERVER_PORT_NUMBER
    int shmid = shmget(IPC_PRIVATE, 1 * sizeof(int), 0777 | IPC_CREAT); //shared memory for maintaining number of clients

    //inputs from user
    // printf("Enter port number\n"); //Give port number above 1024
    if (argc != 2)
    {
        printf("ERROR: Please provide valid command-line arguments!\n");
        exit(1);
    }
    port_number = atoi(argv[1]);

    // socket creation phase
    //server's socket fd
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == -1)
    {
        printf("[+]Server side socket creation failed!\n");
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
    // printf("49\n"); //debug

    //bind phase
    int bindsocket = bind(server_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (bindsocket < 0)
    {
        perror("[+]Binding Error: ");
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

    int *num_of_clients = (int *)shmat(shmid, 0, 0); //main process attaching to shared_memory to keep track of number of clients
    pid_t x1;
    while (1)
    {

        printf("[+]Number of connected clients: %d\n", num_of_clients[0]);

        printf("[+]Waiting for a new client...\n");
        int clientaddrlen = sizeof(clientaddr);
        client_socket = accept(server_socket, (struct sockaddr *)&clientaddr, &clientaddrlen);
        num_of_clients[0]++; //increment count after connection
        char status[50]; //string denoting the status of new connection

        if (num_of_clients[0] <= 4)
        {
            //if the counter <=4, proceed with the incoming connection
            strcpy(status, "accept");
            int bytes_sent = send(client_socket, status, strlen(status), 0);
            if (!clientaddrlen)
            {
                perror("[+]Accept Client Error: \n");
                exit(0);
                printf("Client limit reached! Additional client connections will be refused.");
            }
        }
        else
        {//if new connection makes the counter exceed 4, then refuse to connect
            strcpy(status, "decline");
            int bytes_sent = send(client_socket, status, strlen(status), 0);
            printf("[+]Maximum Client Limit Reached...\n");
            printf("[+]Refusing Connection to new client...\n");
            num_of_clients[0]--;
            continue;
            
        }

        printf("[+]Handling Client %d @ %s:%d\n", num_of_clients[0], inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
        // ************************************************************************************
        x1 = fork();
        if (x1 == 0)
        {
            close(server_socket);
            int *noc = (int *)shmat(shmid, 0, 0);
            while (1)
            {

                // read()
                int bytes_recv = recv(client_socket, message, MAX_MESSAGE_LENGTH, 0);
                if (bytes_recv < 0)
                {
                    perror("Error: ");
                    exit(0);
                }
                message[bytes_recv] = '\0';

                if (strcmp(message, "exit") == 0)
                {
                    printf("[+]Client Abrupty Terminated...\n");
                    noc[0]--;
                    printf("[+]%d clients still connected...\n", noc[0]);
                    shmdt(noc);
                    close(client_socket);
                    exit(0);
                }

                //PROCESS/PROCEDURE TO BE PERFORMED
                printf("[+]New Message from Client!:\n");
                // prints the received line in the reverser order
                for (int i = strlen(message) - 1; i >= 0; i--)
                {
                    printf("%c", message[i]);
                }
                printf("\n");

                printf("[+]Enter Message:\n");

                // reads a line from the standard input and sends it to the client.
                char text[MAX_MESSAGE_LENGTH];
                scanf(" %[^\n]", text);
                getchar();
                text[strlen(text)] = '\0';
                
                int bytes_sent = send(client_socket, text, strlen(text), 0);
                if (bytes_sent < 0)
                {
                    printf("Error while sending message to client");
                    exit(0);
                }
            }
        }
    }
    close(server_socket);

    return 0;
}
