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
    strcpy(text, "exitClient");
    int bytes_sent = send(client_socket, text,strlen(text), 0);
    // printf("%d %ld\n", bytes_sent, strlen(text));
    if (bytes_sent < 0){
            printf("Error while sending message to client\n");
            exit(0);
    }
    printf("\nClient Terminated\n");
    close(client_socket);
    close(connection);
    fflush(stdout); 
    exit(0);
    
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
    char filename[MAX_MESSAGE_LENGTH]; //Filename is the string passed by client to server

    if(argc != 3){
        printf("ERROR: Please provide valid command-line arguments!\n");
        exit(1);
    }

    const char* server_ip_addr = argv[1];
    server_port = atoi(argv[2]);
    // input to client
    // printf("Enter Server IP Address: \n");
    // scanf("%s", server_ip_addr);
    
    // printf("Enter Server Port Number: \n");
    // scanf("%d", &server_port);
    

    // strcpy(server_ip_addr,"127.0.0.1");
    // server_port = 12345;
    // strcpy(filename,"file4.txt");

    // printf("%s\n%d\n%s\n",server_ip_addr,server_port, filename);

    //socket creation
    client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == -1)
    {
        perror("Socket Creation Error: \n");
        return 0;
    }
    else
    {
        printf("Socket created successfully\n");
    }

    // CONSTRUCT SERVER ADDRESS STRUCT

    bzero(&serveraddr, sizeof(serveraddr)); //init struct with all zeros
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port); //input port number
    serveraddr.sin_addr.s_addr = inet_addr(server_ip_addr);
    printf("Address assigned\n");

    // ESTABLISH CONNECTION
    //connect() phase
    // The connect function is used by a TCP client to establish a connection with a TCP server.
    connection = connect(client_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (connection < 0)
    {
        if (errno == EHOSTUNREACH || errno == ENETUNREACH)
        {
            printf("Error Code: %d Error: Server is unreachable\n", errno);
        }
        else
        {
            perror("Connection Error: ");
            printf("Error Code: %d Error: %s\n", errno, strerror(errno));
        }
        close(connection);
        close(client_socket);
        exit(0);
    }
    else
    {
        printf("Connection Established Successfully\n");
    }

    
        // printf("Enter String:");
        // gets(filename);
        // scanf("%[^\n]%*c", filename); 
        // scanf("%s",filename);
        // printf("91");

        // bzero(filename, sizeof(filename)); 
        // int n = 0;
        // while ((filename[n++] = getchar()) != '\n');
        // printf("")

        
        scanf(" %[^\n]",filename);
		getchar();
		filename[strlen(filename)]='\0';

    
        int bytesSent = send(client_socket, filename, strlen(filename), 0);

        if (bytesSent != strlen(filename))
        {
            perror("Sender Error: ");
            close(connection);
            close(client_socket);
            exit(0);
        }
        
        //exit condition
        // if (strcmp(filename, "exitClient") == 0)
        // {
        //     printf("Client Exiting...");
        //     exit(0);
        // }

        // if (strcmp(filename, "exitServer") == 0)
        // {
        //     printf("Server Terminated...\n");
        //     printf("Client Exiting..\n");
        //     close(client_socket);
        //     exit(0);
        // }

        //read()
        char text[MAX_MESSAGE_LENGTH];
        int bytes_recv = recv(client_socket, text,MAX_MESSAGE_LENGTH, 0);
        text[bytes_recv] = '\0';

        if (bytes_recv < 0)
        {
            perror("Error: ");
            close(connection);
            close(client_socket);
            exit(0);
        }
        else
        {
            if (strcmp(text, "exitServer") == 0)
            {
                printf("Server Terminated...\n");
                printf("Client Exiting...\n");
                close(connection);
                close(client_socket);
                exit(0);
            }
            
        }
        
        // The client prints the received line in the reverse order and exits. 
        for(int i=strlen(text)-1;i>=0;i--){
            printf("%c",text[i]);
        }
        printf("\n");

    close(client_socket);
    return 0;
}

/* Create a new socket of type TYPE in domain DOMAIN, using protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
Returns a file descriptor for the new socket, or -1 for errors.  */

/*
1) Client and server can be single run programs
2) create parent-child write-read processes for server and client
3) create sequential using while loop
*/

/*
Some Notes
The "AF_" prefix stands for "address family" and the "PF_" prefix stands for "protocol family." Historically, the intent was that a single protocol family might support multiple address families and that the PF_ value was used to create the socket and the AF_ value was used in socket address structures.
*/