2018A7PS0193G
Rohit Garg
Lab 7

COMPILATION

gcc -w -o client client.c -lssl -lcrypto

EXECUTION

./client [url]

Note: url is expected to be of the form: 
http(s)://www.websitename.com/.../.../.../filename.extension


1. On running the code with correct url format, the terminal will show if data was recieved or if there was an error in reaching the file.
2. Upon downloading the file, the client will exit.
3. File will be downloaded in the local folder.
4. Please make sure that you pass on valid path with domain name otherwise file will not be downloaded.
