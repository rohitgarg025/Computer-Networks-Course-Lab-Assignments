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
// #define PORT_NUMBER 8080
// #define SERVER_IP_ADDR "127.0.0.1"
#define MAX_MESSAGE_LENGTH 1024
int client_socket; //client's socket
int connection; //connection sockfd
void sigintHandler(int sig_num) 
{ 
    
    signal(SIGINT, sigintHandler);
    char text[MAX_MESSAGE_LENGTH];
    strcpy(text, "exit");
    int bytes_sent = send(client_socket, text,strlen(text), 0);
    // printf("%d %ld\n", bytes_sent, strlen(text));
    if (bytes_sent < 0){
            printf("Error while sending message to client\n");
            exit(0);
    }
    printf("\nClient Terminated\n");
    close(client_socket);
    close(connection);
    exit(0);
    fflush(stdout); 
    
    
} 

// The client sends a message to server 

// The client will accept sever's IP address, server' port number, and the string as input parameters.

// On receiving the message, prints it in reverse the client exits

// Each client connection causes the server to fork a new process just for that client.
int main(int argc, char const *argv[])
{
    signal(SIGINT, sigintHandler);
    
    struct sockaddr_in serveraddr;
    int server_port;
    char message[MAX_MESSAGE_LENGTH]; //Filename is the string passed by client to server

    if(argc != 3){
        printf("ERROR: Please provide valid command-line arguments!\n");
        exit(1);
    }

    const char* server_ip_addr = argv[1];
    server_port = atoi(argv[2]);

    //socket creation
    client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == -1)
    {
        perror("[+]Socket Creation Error: \n");
        return 0;
    }
    else
    {
        printf("[+]Socket created successfully\n");
    }

    // CONSTRUCT SERVER ADDRESS STRUCT

    bzero(&serveraddr, sizeof(serveraddr)); //init struct with all zeros
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port); //input port number
    serveraddr.sin_addr.s_addr = inet_addr(server_ip_addr);
    printf("[+]Address assigned\n");

    // ESTABLISH CONNECTION
    //connect() phase
    // The connect function is used by a TCP client to establish a connection with a TCP server.
    connection = connect(client_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (connection < 0)
    {
        if (errno == EHOSTUNREACH || errno == ENETUNREACH)
        {
            printf("[+]Error Code: %d Error: Server is unreachable\n", errno);
        }
        else
        {
            perror("[+]Connection Error: ");
            printf("[+]Error Code: %d Error: %s\n", errno, strerror(errno));
        }
        close(connection);
        close(client_socket);
        exit(0);
    }
    else
    {
        // printf("[+]Connection Established Successfully\n");
    }

    char status[MAX_MESSAGE_LENGTH];
    int bytes_recv = recv(client_socket, status,MAX_MESSAGE_LENGTH, 0);
    status[bytes_recv] = '\0';
    
    // printf("%s",status);

    if(strcmp(status, "decline") == 0){
        printf("[+]Connection Declined\n");
        exit(0);
    }
    else if(strcmp(status, "accept") == 0){
        printf("[+]Connection Established Successfully\n");
        printf("==Status: Server accepted Client==\n");
    }

    // printf("%s",status);


    while(1){   
        printf("[+]Enter message:\n");
        scanf(" %[^\n]",message);
		getchar();
		message[strlen(message)]='\0';


        int bytesSent = send(client_socket, message, strlen(message), 0);

        if (bytesSent != strlen(message))
        {
            perror("[+]Sender Error: ");
            close(connection);
            close(client_socket);
            break;
        }
        
        // exit condition
        if (strcmp(message, "exit") == 0)
        {
            printf("[+]Client Exiting...");
            break;
        }


        //read()
        char text[MAX_MESSAGE_LENGTH];
        int bytes_recv = recv(client_socket, text,MAX_MESSAGE_LENGTH, 0);
        text[bytes_recv] = '\0';

        if (bytes_recv < 0)
        {
            perror("[+]Error: ");
            close(connection);
            close(client_socket);
            exit(0);
        }
        else
        {
            if (strcmp(text, "exitServer") == 0)
            {
                printf("[+]Server Terminated...\n");
                printf("[+]Client Exiting...\n");
                close(connection);
                close(client_socket);
                exit(0);
            }
            
        }
        printf("[+]Message from server:\n");
        // The client prints the received line in the reverse order and exits. 
        for(int i=strlen(text)-1;i>=0;i--){
            printf("%c",text[i]);
        }
        printf("\n");
    }
    close(client_socket);
    return 0;
}

