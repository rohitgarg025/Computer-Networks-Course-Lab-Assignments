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
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <libgen.h>

#include <malloc.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

// #define PORT_NUMBER 8080
// #define SERVER_IP_ADDR "127.0.0.1"

#define FAIL -1

int OpenConnection(const char *hostname, int port)
{
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;

    if ((host = gethostbyname(hostname)) == NULL)
    {
        perror(hostname);
        abort();
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *)(host->h_addr);
    if (connect(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}

SSL_CTX *InitCTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();     /* Load cryptos, et.al. */
    SSL_load_error_strings();         /* Bring in and register error messages */
    method = TLSv1_2_client_method(); /* Create new client-method instance */
    ctx = SSL_CTX_new(method);        /* Create new context */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line); /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);      /* free the malloc'ed string */
        X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

#define MAX_MESSAGE_LENGTH 5000
int client_socket; //client's socket
int connection;    //connection sockfd
void sigintHandler(int sig_num)
{

    signal(SIGINT, sigintHandler);
    char text[MAX_MESSAGE_LENGTH];
    strcpy(text, "exit");
    int bytes_sent = send(client_socket, text, strlen(text), 0);
    // printf("%d %ld\n", bytes_sent, strlen(text));
    if (bytes_sent < 0)
    {
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

int ReadHttpStatus(int sock)
{
    char c;
    char buff[1024] = "", *ptr = buff + 1;
    int bytes_received, status;
    printf("Begin Response ..\n");
    while (bytes_received = recv(sock, ptr, 1, 0))
    {
        if (bytes_received == -1)
        {
            perror("ReadHttpStatus");
            exit(1);
        }

        if ((ptr[-1] == '\r') && (*ptr == '\n'))
            break;
        ptr++;
    }
    *ptr = 0;
    ptr = buff + 1;

    sscanf(ptr, "%*s %d ", &status);

    printf("%s\n", ptr);
    printf("status=%d\n", status);
    char statstr[200];

    sprintf(statstr, "%d", status);
    // char str[42];
    printf("%s\n", statstr);
    if (statstr[0] == '2')
    {
        printf("[+] 2xx Status, proceeding to write file.\n");
    }
    else
    {
        printf("!!!!!!!!Error in getting file from URL.\n");
        exit(1);
    }

    printf("End Response ..\n");
    return (bytes_received > 0) ? status : 0;
}

int ReadHttpsStatus(SSL *ssl)
{
    char c;
    char buff[2048] = "", *ptr = buff + 1;
    int bytes_received, status;
    // printf("Begin Response ..\n");
    while (bytes_received = SSL_read(ssl, ptr, 1))
    {
        if (bytes_received == -1)
        {
            perror("ReadHttpStatus");
            exit(1);
        }

        if ((ptr[-1] == '\r') && (*ptr == '\n'))
            break;
        ptr++;
    }
    *ptr = 0;
    ptr = buff + 1;

    sscanf(ptr, "%*s %d ", &status);

    return (bytes_received > 0) ? status : 0;
}
//the only filed that it parsed is 'Content-Length'
int ParseHeader(int sock)
{
    char c;
    char buff[1024] = "", *ptr = buff + 4;
    int bytes_received, status;
    printf("Begin HEADER ..\n");
    while (bytes_received = recv(sock, ptr, 1, 0))
    {
        if (bytes_received == -1)
        {
            perror("Parse Header");
            exit(1);
        }

        if (
            (ptr[-3] == '\r') && (ptr[-2] == '\n') &&
            (ptr[-1] == '\r') && (*ptr == '\n'))
            break;
        ptr++;
    }

    *ptr = 0;
    ptr = buff + 4;
    //printf("%s",ptr);

    if (bytes_received)
    {
        ptr = strstr(ptr, "Content-Length:");
        if (ptr)
        {
            sscanf(ptr, "%*s %d", &bytes_received);
        }
        else
            bytes_received = -1; //unknown size

        printf("Content-Length: %d\n", bytes_received);
    }
    printf("End HEADER ..\n");
    return bytes_received;
}

int ParseHeaderHttps(SSL *ssl)
{
    char c;
    char buff[1024] = "", *ptr = buff + 4;
    int bytes_received, status;
    // printf("Begin HEADER ..\n");
    while (bytes_received = SSL_read(ssl, ptr, 1))
    {
        if (bytes_received == -1)
        {
            perror("Parse Header");
            exit(1);
        }

        if (
            (ptr[-3] == '\r') && (ptr[-2] == '\n') &&
            (ptr[-1] == '\r') && (*ptr == '\n'))
            break;
        ptr++;
    }

    *ptr = 0;
    ptr = buff + 4;
    // printf("%s",ptr);

    if (bytes_received)
    {
        ptr = strstr(ptr, "Content-Length:");
        if (ptr)
        {
            sscanf(ptr, "%*s %d", &bytes_received);
        }
        else
            bytes_received = -1; //unknown size

        // printf("Content-Length: %d\n", bytes_received);
    }
    // printf("End HEADER ..\n");
    return bytes_received;
}

int main(int argc, char **argv)
{
    
    signal(SIGINT, sigintHandler);

    struct sockaddr_in serveraddr;

    // char message[MAX_MESSAGE_LENGTH]; //Filename is the string passed by client to server

    if (argc != 2)
    {
        printf("ERROR: Please provide valid command-line arguments!\n");
        exit(1);
    }
    
    char *url = argv[1];
    printf("\nURL:%s\n", url);
    char *protocol = strtok(url, "://");
    char *domain = strtok(NULL, "/");
    char *path = strtok(NULL, "\n");
    char *filename = NULL;
    if (path != NULL)
    {
        filename = basename(path);
        // if(filename == NULL){filename = "/";}
        printf("%s\n", filename);
    }
    char *server_ip_addr;
    
    printf("%s %s %s\n", protocol, domain, path); //debug


    int server_port;
    if (strcmp(protocol, "http") == 0)
    {
        server_port = 80;

        // socket creation
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

        // retrive server_ip_addr from hostname
        struct hostent *he;
        he = gethostbyname(domain);
        if (he == NULL)
        {
            herror("gethostbyname");
            exit(1);
        }

        // CONSTRUCT SERVER ADDRESS STRUCT
        bzero(&serveraddr, sizeof(serveraddr)); //init struct with all zeros
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(server_port); //input port number
        serveraddr.sin_addr = *((struct in_addr *)he->h_addr);
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
            printf("[+]Connection Established Successfully\n");
        }

        // write/send GET request to HTTP/HTTPS server
        char request[MAX_MESSAGE_LENGTH];

        //write to formatted string to request str
        //EX: message = "GET /download/pdfurl-guide.pdf HTTP/1.1\r\nHost: www.axmag.com\r\n\r\n Connection: keep-alive\r\n\r\n Keep-Alive: 300\r\n";
        if (path == NULL)
        {
            snprintf(request, sizeof(request), "GET / HTTP/1.0\r\nHost: %s\r\n\r\n", domain);
        }
        else
        {
            // if path exits then..
            snprintf(request, sizeof(request), "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n", path, domain);
        }

        //send request str to server

        int bytesSent = send(client_socket, request, strlen(request), 0);

        if (bytesSent != strlen(request))
        {
            perror("[+]Sender Error: ");
            close(connection);
            close(client_socket);
            exit(1);
        }
        else
        {
            printf("[+]Request sent successfully\n");
        }
        int contentlengh;
        remove(filename);
        

        if (ReadHttpStatus(client_socket) && (contentlengh = ParseHeader(client_socket)))
        {
            FILE *file = fopen(filename, "ab");

            if (file == NULL)
            {
                printf("File could not opened");
            }

            
            int recvFileLength = 0;

            // Write out content to file
            char response[MAX_MESSAGE_LENGTH];
            while (1)
            {

                int bytes_recv = recv(client_socket, response, MAX_MESSAGE_LENGTH, 0);
                if (bytes_recv < 0)
                {
                    puts("recv failed");
                    break;
                }

                if (bytes_recv == 0)
                {
                    // puts("recv failed");
                    break;
                }

                response[bytes_recv] = '\0';

                recvFileLength = recvFileLength + bytes_recv;

                fwrite(response, bytes_recv, 1, file);

                // printf("\nReceived byte size = %d\nTotal lenght = %d", bytes_recv, recvFileLength);
                if (bytes_recv == contentlengh)
                    break;
            }

            // puts("Reply received\n");
            fclose(file);
        }
        
        close(client_socket);
    }
    else if (strcmp(protocol, "https") == 0)
    {

        // printf("442");
        server_port = 443;
        SSL_CTX *ctx;
        int server;
        SSL *ssl;
        char buf[1024];
        int bytes;

        char *hostname;
        int portnum = server_port;
        hostname = domain;
        // strcpy(hostname, domain);
        
        // printf("454");
        SSL_library_init();
        ctx = InitCTX();
        server = OpenConnection(hostname,portnum);
        ssl = SSL_new(ctx);           /* create new SSL connection state */
        SSL_set_fd(ssl, server);      /* attach the socket descriptor */
        if (SSL_connect(ssl) == FAIL) /* perform the connection */
            ERR_print_errors_fp(stderr);
        else
        {
            char send_data[3098], recv_data[2048], *p;
            
            snprintf(send_data, sizeof(send_data),
                     "GET /%s HTTP/1.0\r\nHOST: %s\r\n\r\n",
                     path, domain);
            printf("%s",send_data);
            printf("[+]Connected with %s encryption\n", SSL_get_cipher(ssl));
            ShowCerts(ssl);                               /* get any certs */
            SSL_write(ssl, send_data, strlen(send_data)); /* encrypt & send message */
            remove(filename);
            
            int bytes_recieved;
            int recvFileLength = 0;
            int contentlengh;

            if ((ReadHttpsStatus(ssl) / 100 == 2) && (contentlengh = ParseHeaderHttps(ssl)))
            {
                FILE *fp = fopen(filename, "wb");
                memset(recv_data, 0, sizeof(recv_data));
                while (bytes_recieved = SSL_read(ssl, recv_data, sizeof(recv_data)))
                {
                    if (bytes_recieved < 0)
                    {
                        puts("recv failed");
                        break;
                    }

                    if (bytes_recieved == 0)
                    {
                        break;
                    }

                    recv_data[bytes_recieved] = '\0';

                    recvFileLength = recvFileLength + bytes_recieved;

                    fwrite(recv_data, bytes_recieved, 1, fp);

                    if (bytes_recieved == contentlengh)
                        break;
                }
                fclose(fp);
            }
            else
            {
                printf("!!!!!!!!Error in getting file from URL.\n");
                remove(filename);
                exit(1);
            }
            printf("[+] Data recieved.\n");
            
            SSL_free(ssl);
            //     bytes = SSL_read(ssl, buf, sizeof(buf)); /* get reply & decrypt */
            //     buf[bytes] = 0;
            //     printf("Received: \"%s\"\n", buf);
            //     SSL_free(ssl); /* release connection state */
            // }
            close(server);     /* close socket */
            SSL_CTX_free(ctx); /* release context */
        }

        
    }
    return 0;
}


/*
NOTES:
https://jakash3.wordpress.com/2011/03/04/http-file-downloader-in-c/
https://stackoverflow.com/questions/33960385/how-to-download-a-file-from-http-using-c
https://stackoverflow.com/questions/3382173/download-http-thru-sockets-c

https://stackoverflow.com/questions/19207745/htons-function-in-socket-programing
http://www.martinbroadhurst.com/downloading-a-web-page-in-c-using-a-socket.html

*/
