Lab 10[Compre Lab]

ROHIT GARG
2018A7PS0193G


# PREREQUISITE
openssl library (sudo apt-get install libssl-dev)

# ZIP CONTENTS
* client.c - client program
* server.c - server program
* private1.pem - file containing private key for client 1
* public1.pem - file containing corresponding public key for client 1
* private2.pem - file containing private key for client 2
* public2.pem - file containing corresponding public key for client 2
* README.txt - readme

# COMPILATION
gcc -o c client.c -lssl -lcrypto
gcc -o s server.c

# EXECUTION
[1] Run Server (./s 8080) - [Replace 8080 with port number]
[2] In a seperate terminal, Run first client (./c 127.0.0.1 8080 private1.pem public2.pem) - [Replace 8080 with port number given to server. Replace private1.pem and public2.pem with keys for private key 1 and public key 2 respectively]
[3] In a third terminal, Run second client (./c 127.0.0.1 8080 private2.pem public1.pem) - [Replace 8080 with port number given to server. Replace private2.pem and public1.pem with keys for private key 2 and public key 1 respectively]

# SAMPLE RUN
<<Terminal Window 1>> gcc -o s server.c
<<Terminal Window 1>> ./s 8080

<<Terminal Window 2>> gcc -o c client.c -lssl -lcrypto
<<Terminal Window 2>> ./c 127.0.0.1 8080 private1.pem public2.pem
<<Terminal Window 3>> ./c 127.0.0.1 8080 private2.pem public1.pem

# send message from client 1's terminal:-

<<Terminal Window 2>> I am client 1

# print encrypted and corresponding decrypted message at client 2's end:-

<<Terminal Window 3>> [Encrypted Message>]
<<Terminal Window 3>> :<�*���:�o�#�v
              (�ȞR)!�2���b��ur�S�X5�X�l��XU�B��Relf����X���ٴ�2Β��;>�RsY��:�
jdt�o[+�[]���{��*B-�褳,���;�x��:���1��c�t�@�R��t�Ć�͛�������iU<|��Z�+�B�7���L<�A�d鍎�ULn���D
          ���/K��,�;���O��q��_^C�-cz�D84v���NA��%)���C!�Ix
                                                          ò�y�1Z�휿!�)P��v�g)��&���l\H���c��=��Jy���8t    }�:��&N��X�OX��
<<Terminal Window 3>> [Decrypted Message>]
<<Terminal Window 3>> I am client 1

# issue exit command at any terminal to exit.

<<Terminal Window 3>> exit
exit command issued by user...client exiting...
exit command issued by user...client exiting...


# NOTE
* The code is working properly.
* If you want to generate your own public key and private key apart from ones provided in folder, please keep key size according to RSA file size-key size relation.
Hence to encrypt 1024 byte file, generate key of size atleast (1024+42)*8 = 8528 bits ~ 9Kbits. Hence 10K bits size key has been provided to be on the safer side since public-private split also takes place according to documentation of openssl-rsa.

For Ex:
Use the below command to generate RSA private and public keys with length of 16000
a) openssl genrsa -out private.pem 16000
b) openssl rsa -in private.pem -outform PEM -pubout -out public.pem

These commands will generate private.pem and public.pem respectively. Only generate if needed, otherwise use keys given in zip folder.
