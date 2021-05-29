CS F303
Computer Networks

ROHIT GARG[2018A7PS0193G]

1. Compile sever.c (gcc -o server server.c).
2. Compile client.c (gcc -o client client.c).
3. Run server executable and pass port number as argument (./server 2345) [ Here 2345 is used as server port number].
4. In a separate terminal window, run client executable and pass IP address of server followed by server port as arguments (./client 127.0.0.1 2345) [ Due to cmd arguments passed in server.c, the server is listening on localhost 127.0.0.1 and the port number specified, which here is 2345]
5. Pass message from client to server using the standard input on client terminal.
6. Pass message from server to client using standard input on server terminal.

After this the client will exit and the server will wait for another client to connect. 

Note:
Both the server and client are capable of handling abrupt termination done by SIGINT [Ctrl+C] keys.

Wireshark packets can be captured using the loopback interface. The results can be filtered using the commands given in the PDF.

Open rohit.pcap to access captured packets on my machine via Wireshark.

