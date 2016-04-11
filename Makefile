CC = gcc
CFLAG = -Wall -Werror

all: server

server: server.c
	$(CC) $(CFLAG) server.c -o server

clean:
	rm *.o server