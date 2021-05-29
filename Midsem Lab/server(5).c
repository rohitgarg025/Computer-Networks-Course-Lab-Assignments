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
// #define SERVER_PORT_NUMBER atoi(argv[1])
// #define IP_ADDR "127.0.0.1"
#define MAX_PENDING 5
#define MAX_MESSAGE_LENGTH 1024
int client_socket;
int server_socket;

void sigintHandler(int sig_num) 
{ 
    signal(SIGINT, sigintHandler);
    char text[MAX_MESSAGE_LENGTH];
    strcpy(text, "exitServer");
    int bytes_sent = send(client_socket, text,strlen(text), 0);
        // printf("%d %ld\n", bytes_sent, strlen(text));
    // if (bytes_sent < 0){
    //         printf("Error while sending message to client\n");
    //         exit(0);
    // }
    printf("\nServer Terminated\n");
    close(client_socket);
    close(server_socket);
    fflush(stdout); 
    exit(0);
    
} 

int main(int argc, char const *argv[])
{
    // Handles Ctrl+C interrupt to exit server and eventually client
    signal(SIGINT, sigintHandler); 
    
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    char filename[MAX_MESSAGE_LENGTH]; //Filename is the string passed by client to server
    int port_number; //SERVER_PORT_NUMBER
    
    //inputs from user
    // printf("Enter port number\n"); //Give port number above 1024
    if(argc!=2){
        printf("ERROR: Please provide valid command-line arguments!\n");
        exit(1);
    }
    port_number = atoi(argv[1]);
    // scanf("%d", &port_number);
    // fflush (stdin);

    // socket creation phase
    //server's socket fd
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == -1)
    {
        printf("Server side socket creation failed!\n");
        return 0;
    }
    else
    {
        printf("Socket created successfully\n");
    }
    //printf("42\n"); //debug

    
    bzero(&serveraddr, sizeof(serveraddr)); //initialise socket
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port_number);
    // printf("49\n"); //debug

    //bind()
    int bindsocket = bind(server_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (bindsocket < 0)
    {
        perror("Binding Error: ");
        exit(0);
    }
    else
    {
        printf("Bind Successful...\n");
    }

    // listen()
    //Moves server from closed state to listen state
    int listenval = listen(server_socket, MAX_PENDING);
    if (listenval < 0)
    {
        perror("Listening Error: ");
        exit(0);
    }
    else
    {
        printf("Successfully listening....\n");
    }

    // printf("73"); //debug
    while (1)
    {
        printf("Waiting for a new client...\n");
        int clientaddrlen = sizeof(clientaddr);
        client_socket = accept(server_socket, (struct sockaddr *)&clientaddr, &clientaddrlen);
        if (!clientaddrlen)
        {
            perror("Accept Client Error: \n");
            exit(0);
        }
        printf("Handling Client @ %s\n", inet_ntoa(clientaddr.sin_addr));

        
        
        // read()
        int bytes_recv = recv(client_socket, filename, MAX_MESSAGE_LENGTH, 0);
        if (bytes_recv < 0)
        {
            perror("Error: ");
            exit(0);
        }
        filename[bytes_recv] = '\0';
        
        if (strcmp(filename, "exitClient") == 0)
            {
                printf("Client Abrupty Terminated...\n");
                printf("Server Exiting...\n");
                close(client_socket);
                continue;
            }
            
        //PROCESS/PROCEDURE TO BE PERFORMED
        
        // prints the received line in the reverser order 
        for(int i=strlen(filename)-1;i>=0;i--){
            printf("%c",filename[i]);
        }
        printf("\n");

        // reads a line from the standard input and sends it to the client. 
        char text[MAX_MESSAGE_LENGTH];
        // scanf("%[^\n]%*c", text); 
        // scanf("%s",text);
        // fgets(text, MAX_MESSAGE_LENGTH, stdin); 
        // bzero(text, sizeof(text)); 
        // int n=0;
        // while ((text[n++] = getchar()) != '\n');
        // fflush (stdin);
        // // printf("%s\n",text);

        scanf(" %[^\n]",text);
		getchar();
		text[strlen(text)]='\0';
    
        int bytes_sent = send(client_socket, text, strlen(text), 0);
        // printf("%d %ld\n", bytes_sent, strlen(text));
        if (bytes_sent < 0)
        {
            printf("Error while sending message to client");
            exit(0);
        }
        
        // printf("%s %d\n", text, bytes_sent); //debug
            
        
        
        
    }
    close(server_socket);

    return 0;
}

