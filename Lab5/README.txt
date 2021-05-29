Computer Networks
Lab 5

Note: 
Server's directory refers to files, server.c file belonging to server;
Client's directory refers to files, client.c files belonging to client;
 
 Steps for compiling and running
 
1. Place server.c and client.c in separate individual directories.
2. Open two terminal windows. One in Server' directory and another in Client's directory.
3. In the server terminal, compile server.c {gcc -o server server.c }.
3. Type ./server to run the server code.
4. Enter server port number.
5. Now, in the client terminal,compile client.c {gcc -o client client.c}.
6. Type ./client to run the client code.
7. Enter server IP address.(localhost IP: 127.0.0.1)
8. Enter Server port number in client terminal.
9. After the connection is made,enter filename as input to client's terminal.
8. To exit the client, type exitClient as filename on server terminal.
9. To exit the server, type exitServer as filename on client terminal.

Note: server can also be exited using Ctrl + c. In this case, client will be notified when it queries the server next time for any file data. 
