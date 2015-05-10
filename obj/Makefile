CC = gcc
CFLAGS = -g -Wall -lpthread
all:server client

server:list_com.o srv_msg_com.o server.o
	$(CC)  $^ -o $@ $(CFLAGS)

client:list_com.o cli_msg_com.o client.o
	$(CC)  $^ -o $@ $(CFLAGS)