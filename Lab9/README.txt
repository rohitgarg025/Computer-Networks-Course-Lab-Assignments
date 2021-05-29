ROHIT GARG
2018A7PS0193G

# ZIP CONTENTS
* encrypt.c - Encryption program
* decrypt.c - Decryption program
* private.pem - file containing private key
* public.pem - file containing corresponding public key
* input.txt -  a text file 
* ciphertext.txt - its encryption using the submitted public key 
* README.txt - readme


# PREREQ(You must have this installed on your PC)
openssl library (sudo apt-get install libssl-dev)

# COMPILATION
gcc -o en encrypt.c -lssl -lcrypto
gcc -o de decrypt.c -lssl -lcrypto

# EXECUTION
./en public.pem input.txt ciphertext.txt
./de private.pem ciphertext.txt result.txt

# Note:
#1
* In command one, replace input.txt with text file name to be encrypted and ciphertext.txt with the name of the encoded file you want.
* In command two, replace ciphertext.txt with the name you gave as last parameter in command one and result.txt with the decoded file name you want.

#2
* The code is working properly.
* result.txt is the final decoded file.
* If you want to generate your own public key and private key apart from ones provided in folder, please keep key size according to RSA file size-key size relation.
Hence to encrypt 1024 byte file, generate key of size atleast (1024+42)*8 = 8528 bits ~ 9Kbits. Hence 10K bits size key has been provided to be on the safer side since public-private split also takes place according to documentation of openssl-rsa.

For Ex:
Use the below command to generate RSA private and public keys with length of 10000
a) openssl genrsa -out private.pem 10000
b) openssl rsa -in private.pem -outform PEM -pubout -out public.pem

These commands will generate private.pem and public.pem respectively. Only generate if needed, otherwise use keys given in zip folder.

