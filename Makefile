CC = gcc
CFLAG = -Wall -Werror -g

all: server client

server: server.c
	$(CC) $(CFLAG) server.c -o server -pthread -lsqlite3

client: client.c
	$(CC) $(CFLAG) client.c -o client

clean:
	rm -f *~ *.o server client