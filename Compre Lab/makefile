all: compile
compile: s c

s: server.c 
	gcc -o s server.c

c: client.c
	gcc -o c client.c -lssl -lcrypto
