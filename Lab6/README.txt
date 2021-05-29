CS F303
Computer Networks
ROHIT GARG 2018A7PS0193G

1. Compile sever.c (gcc -o s server.c).
2. Compile client.c (gcc -o c client.c).
3. Run server executable and pass port number as argument (./s 2345) [ Here 2345 is used as port].
4. In a separate terminal window, run client executable and pass IP address of server followed by server port as arguments (./c 127.0.0.1 2345) [ As per the arguments passed in server.c, the server is made on localhost 127.0.0.1 and the port number specified]
------Note: Users can run up to 4 clients. Any further clients would be rejected.------
For each client:
5. Pass message from client to server using the standard input on client terminal.
6. Pass message from server to client using standard input on server terminal.
7. Repeat from step 5 till exit is entered or Ctrl+C is pressed on client end.

After this the client will exit and the server will wait for another client to connect.

Note: 
compile client1 as gcc (gcc -o c1 client.c).
client2 as (gcc -o c2 client.c) and so on... for better readility of output
Anyways, both approaches of naming object files work.