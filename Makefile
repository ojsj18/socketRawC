all: client server
client: 
		gcc -g -w cliente.c -o cliente.out -lncurses

server:
		gcc -g -w servidor.c -o servidor.out -lncurses
