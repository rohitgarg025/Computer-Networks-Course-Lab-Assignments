#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#define KEY_SIZE 16000
#define MAX_FILE_SIZE 1024

int padding = RSA_PKCS1_PADDING;

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Insufficient cmd-line arguments\n");
    }
    char *public_key = argv[1];
    char *ipfile = argv[2];
    char *encfile = argv[3];

    //**********************************************************

    FILE *fkey;
    if ((fkey = fopen(public_key, "rb")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    RSA *rsa = RSA_new();
    rsa = PEM_read_RSA_PUBKEY(fkey, &rsa, NULL, NULL);
    
    //**********************************************************

    //**********************************************************
    unsigned char data[MAX_FILE_SIZE];
    int length = 0;
    memset(data, 0, sizeof(data));

    FILE *fdata;
    if ((fdata = fopen(ipfile, "rb")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    fseek(fdata, 0, SEEK_END);
    length = ftell(fdata);
    fseek(fdata, 0, SEEK_SET);
    fread(data, 1, length, fdata);
    //**********************************************************

    //**********************************************************
    //encrypted file is twice as big as size of key
    unsigned char encrypted[2 * KEY_SIZE];
    memset(encrypted, 0, sizeof(encrypted));

    int enc_length = RSA_public_encrypt(length, data, encrypted, rsa, RSA_PKCS1_PADDING);
    if (enc_length == -1)
    {
        int error = ERR_get_error();
        fprintf(stderr, "RSA Error =%d\n", error);
        fprintf(stderr, "----%s\n", ERR_error_string(error, NULL));
        exit(1);
    }

    FILE *fencrypted = fopen(encfile, "wb");
    if (fencrypted == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    fwrite(encrypted, sizeof(encrypted), 1, fencrypted);
    //**********************************************************

    fclose(fencrypted);
    fclose(fdata);

    return 0;
}