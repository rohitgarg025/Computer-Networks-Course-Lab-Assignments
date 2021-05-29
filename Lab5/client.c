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
// #define PORT_NUMBER 8080
// #define SERVER_IP_ADDR "127.0.0.1"
#define FILE_NAME_LEN 50

// The client sends a query to server requesting data from <filename> to the server.
// The client will accept sever's IP address, server' port number, and the requested <filename> as input parameters.

// On receiving the data, the client creates the file <filename> and stores the data. (5 marks). A sample file is attached.

// Each client connection causes the server to fork a new process just for that client.
int main(int argc, char const *argv[])
{
    int client_socket; //client's socket
    struct sockaddr_in serveraddr;
    char server_ip_addr[50];
    int server_port;
    char filename[FILE_NAME_LEN];

    // input to client
    printf("Enter Server IP Address: \n");
    scanf("%s", server_ip_addr);
    printf("Enter Server Port Number: \n");
    scanf("%d", &server_port);

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
    int connection = connect(client_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
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
        exit(0);
    }
    else
    {
        printf("Connection Established Successfully\n");
    }

    while (1)
    {

        printf("\nEnter Filename: \n");
        scanf("%s", filename);

        //write()
        int bytesSent = send(client_socket, filename, strlen(filename), 0);

        if (bytesSent != strlen(filename))
        {
            perror("Sender Error: ");
            exit(0);
        }
        else
        {
            printf("Filename sent successfully\n");
        }

        //exit condition
        if (strcmp(filename, "exitClient") == 0)
        {
            printf("Client Exiting...");
            break;
        }

        if (strcmp(filename, "exitServer") == 0)
        {
            printf("Server Terminated...\n");
            printf("Client Exiting..\n");
            break;
        }

        //read()
        char text[50];
        int bytes_recv = recv(client_socket, text, 50, 0);
        text[bytes_recv] = '\0';
        if (bytes_recv < 0)
        {
            perror("Error: ");
            exit(0);
        }
        else
        {
            if (strcmp(text, "exit") == 0)
            {
                printf("Server Terminated...\n");
                printf("Client Exiting...\n");
                break;
            }

            if(strcmp(text, "\0") == 0){
                printf("File not found at server side\n");
                printf("Server returned empty string\n");
            }
            
            printf("Request data received successfully from the server\n");
        }

        // printf("%s %d\n", text, bytes_recv);

        //create a new file and write values in it;
        FILE *frecv = fopen(filename, "w+");
        // if (frecv == NULL)
        // {
        //     perror("Error while File Creation at Client Side: ");
        //     exit(0);
        // }
        fputs(text, frecv);
        fclose(frecv);
    }
    // close()
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