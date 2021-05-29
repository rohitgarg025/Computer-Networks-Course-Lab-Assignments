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
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY_SIZE 16000
#define MAX_MESSAGE_LENGTH 32000

int padding = RSA_PKCS1_PADDING;

int main(int argc, char **argv)
{
    //*******************************************************************
    // Read cmd arguments
    if (argc != 5)
    {
        printf("ERROR: Please provide valid command-line arguments!\n");
        exit(1);
    }

    int client_socket; //client's socket
    struct sockaddr_in serveraddr;

    char *server_ip_addr = argv[1];
    int server_port = atoi(argv[2]);
    char *private_key = argv[3];
    char *public_key = argv[4];

    //*******************************************************************
    // fopen public key for encryption

    FILE *fenc;
    if ((fenc = fopen(public_key, "rb")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }
    //create rsa structure
    RSA *rsa_enc = RSA_new();
    rsa_enc = PEM_read_RSA_PUBKEY(fenc, &rsa_enc, NULL, NULL);

    //*******************************************************************
    // fopen private key for decryption

    FILE *fdec;
    if ((fdec = fopen(private_key, "rb")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }
    RSA *rsa_dec = RSA_new();
    rsa_dec = PEM_read_RSAPrivateKey(fdec, &rsa_dec, NULL, NULL);

    //*******************************************************************

    

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
    int connection = connect(client_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (connection < 0)
    {
        if (errno == EHOSTUNREACH || errno == ENETUNREACH)
        {
            printf("[-]Error Code: %d Error: Server is unreachable\n", errno);
        }
        else
        {
            perror("[-]Connection Error: ");
            printf("[-]Error Code: %d Error: %s\n", errno, strerror(errno));
        }
        close(client_socket);
        exit(0);
    }
    else
    {
        printf("[+]Connection Established Successfully\n");
    }
    int child1,child2;

    //*******************************************************************
    child1 = fork();
    if (!child1)
    {
        //sends encrypted message
        while (1)
        {
            char message[MAX_MESSAGE_LENGTH]; //Filename is the string passed by client to server
            memset(message, 0 ,sizeof(message));

            printf("[Send Message<]\n");
            fgets(message, sizeof(message),stdin);
            message[strlen(message)-1] = '\0';

            //encrypted file is twice as big as size of key
            unsigned char encrypted[2 * KEY_SIZE];
            memset(encrypted, 0, sizeof(encrypted));

            int enc_length = RSA_public_encrypt(strlen(message), message, encrypted, rsa_enc, RSA_PKCS1_PADDING);
            if (enc_length == -1)
            {
                int error = ERR_get_error();
                fprintf(stderr, "RSA Error =%d\n", error);
                fprintf(stderr, "----%s\n", ERR_error_string(error, NULL));
                exit(1);
            }

            int bytesSent = send(client_socket, encrypted, enc_length, 0);
            if (bytesSent != enc_length)
            {
                perror("[+]Sender Error: ");
    
                break;
            }

            if(strcmp(message, "exit") == 0){
                printf("exit command issued by user...client exiting...\n");
                break;

            }
        }
        exit(0);
    }
    child2 = fork();
    if(!child2)
    {
        //decryptes received message
        while (1)
        {
            unsigned char encrypted[2 * KEY_SIZE];
            memset(encrypted, 0, sizeof(encrypted));
            int bytes_recv = recv(client_socket, encrypted,sizeof(encrypted), 0);
            if (bytes_recv == 0)
            {
                break;
            }            
            if (bytes_recv < 0)
            {
                perror("[-]Error: ");
                break;
            }
            if(strlen(encrypted) <= 0){
                continue;
            }

            encrypted[bytes_recv] = '\0';

            char decrypted[MAX_MESSAGE_LENGTH];
            memset(decrypted, 0, sizeof(decrypted));

            int dec_len = RSA_private_decrypt(RSA_size(rsa_dec), encrypted, decrypted, rsa_dec, RSA_PKCS1_PADDING);
            if (dec_len == -1)
            {
                int error = ERR_get_error();
                fprintf(stderr, "[-]RSA Error =%d\n", error);
                fprintf(stderr, "[-]----%s\n", ERR_error_string(error, NULL));
                break;
            }


            if(strcmp(decrypted, "exit") == 0){
                printf("exit command issued by user...client exiting...\n");
                break;

            }

            //display encrypted message
            printf("[Encrypted Message>]\n");
            printf("%s\n",encrypted);

            //display decrypted message
            printf("[Decrypted Message>]\n");
            printf("%s\n",decrypted);
            printf("[Send Message<]\n");
        }
        exit(0);
    }

    wait(NULL);
    kill(child1, SIGKILL);
    kill(child2, SIGKILL);
    close(client_socket);
    return 0;
}
