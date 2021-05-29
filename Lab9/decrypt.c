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

    char *private_key = argv[1];
    char *ipfile = argv[2];
    char *decfile = argv[3];

    // unsigned char key[KEY_SIZE];
    // memset(key,0,sizeof(key));

    FILE *fkey;
    if ((fkey = fopen(private_key, "rb")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    RSA *rsa = RSA_new();
    rsa = PEM_read_RSAPrivateKey(fkey, &rsa, NULL, NULL);

    FILE *fdata;
    char data[2 * KEY_SIZE];
    memset(data, 0, sizeof(data));

    int size = 0;

    /*
            FILE IO FOR READING INPUT ENCRYPTED FILE
        */

    // READ FILE TO DECRYPT AND GET SIZE
    {
        // Open the file to decrypt
        fdata = fopen(ipfile, "r");

        // Go to end of file.
        fseek(fdata, 0, SEEK_END);

        // Determine size, go back to start
        size = ftell(fdata);
        fseek(fdata, 0, SEEK_SET);

        // Read file and close
        fread(data, size, 1, fdata);
        fclose(fdata);
    }

    /*
            FILE IO FOR READING INPUT ENCRYPTED FILE
        */

    char decrypted[MAX_FILE_SIZE];
    memset(decrypted, 0, sizeof(decrypted));

    int dec_len = RSA_private_decrypt(RSA_size(rsa), data, decrypted, rsa, RSA_PKCS1_PADDING);
    if (dec_len == -1)
    {
        int error = ERR_get_error();
        fprintf(stderr, "RSA Error =%d\n", error);
        fprintf(stderr, "----%s\n", ERR_error_string(error, NULL));
        exit(1);
    }

    FILE *fdecrypted = fopen(decfile, "wb");

    fwrite(decrypted, dec_len, 1, fdecrypted);

    fclose(fdecrypted);

    return 0;
}
