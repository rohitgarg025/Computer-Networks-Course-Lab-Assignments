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
#define FILE_NAME_LEN 50
int client_socket;
int server_socket;

void sigintHandler(int sig_num) 
{ 
    signal(SIGINT, sigintHandler);
    char text[50];
    strcpy(text, "exit");
    int bytes_sent = send(client_socket, text, 10, 0);
        // printf("%d %ld\n", bytes_sent, strlen(text));
    // if (bytes_sent < 0){
    //         printf("Error while sending message to client\n");
    //         exit(0);
    // }
    printf("\nServer Terminated\n");
    close(client_socket);
    close(server_socket);
    exit(0);
    fflush(stdout); 
} 

int main(int argc, char const *argv[])
{
    signal(SIGINT, sigintHandler); 

    
    
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    char filename[FILE_NAME_LEN];
    int port_number;
    //inputs from user
    printf("Enter port number\n"); //Give port number above 1024
    scanf("%d", &port_number);

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

        int clientaddrlen = sizeof(clientaddr);
        client_socket = accept(server_socket, (struct sockaddr *)&clientaddr, &clientaddrlen);
        if (!clientaddrlen)
        {
            perror("Accept Client Error: \n");
            exit(0);
        }
        printf("Handling Client @ %s\n", inet_ntoa(clientaddr.sin_addr));

        while (1)
        { // for the connected client
        // read()
        int bytes_recv = recv(client_socket, filename, FILE_NAME_LEN, 0);
        filename[bytes_recv] = '\0';
        
        // printf("%s\n", filename); //debug
        // printf("%d\n", strcmp(filename, "exit") == 0); //debug
        //if client passes 'exit' as filename then server exits
        

        if(strcmp(filename, "exitClient") == 0){
            close(client_socket);
            break;
        }

        if(strcmp(filename, "exitServer") == 0){
            printf("Server Exiting...");
            close(client_socket);
            close(server_socket);
            exit(0);
        }

        if (bytes_recv < 0)
        {
            perror("Error: ");
            exit(0);
        }

        
        // printf("%s %ld\n", filename, strlen(filename)); //debug

        //process()
        FILE *fp;
        fp = fopen(filename, "r");
        char text[50];
        if (fp == NULL)
        {
            //If File DNE
            printf("Requested File does not exist at server side\n");
            printf("Sending empty string to client...\n");
            text[0] = '\0'; // empty string
        }
        else
        {
            //File Exists
            // Read only the first 10 bytes
            
            int bytes_read = fread(text, 1, 10, fp);
            text[10] = '\0';
            // printf("%s\n", text);
            if (bytes_read < 0)
            {

                perror("File Read Error:");
                printf("Actual Bytes Read: %d\n", bytes_read);
                fclose(fp);
                exit(0);
            }
            // printf("%s %d\n", text, bytes_read); //debug
        }

        int bytes_sent = send(client_socket, text, 10, 0);
        // printf("%d %ld\n", bytes_sent, strlen(text));
        if (bytes_sent < 0)
        {
            printf("Error while sending message to client");
            exit(0);
        }
        else{
            printf("Requested data sent to client successfully\n\n");
        }

        // printf("%s %d\n", text, bytes_sent); //debug
            
        }
        
        
    }
    close(server_socket);

    return 0;
}

// Notes
// When a socket is created with socket(2), it exists in a name space (ad‐
//    dress family) but has no address assigned to it.   bind()  assigns  the
//    address  specified  by  addr  to the socket referred to by the file de‐
//    scriptor sockfd.  addrlen specifies the size, in bytes, of the  address
//    structure  pointed to by addr.  Traditionally, this operation is called
//    “assigning a name to a socket”.

// int socket (int family, int type, int protocol);
// /* Returns: non-negative descriptor if OK, -1 on error */

// Develop a TCP-based server and client using C. The single client requests data from a file on the server. The server opens the file and sends data back to the client.
// Given: The server has few text files files in its present working directory. The client and server will work in separate directories.

// The server will accept its port number as the input parameter.

//Output:
//  The server parses the query and returns the first 10 bytes of the file <filename>.
