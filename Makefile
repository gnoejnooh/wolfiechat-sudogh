all: server client

User.o: User.c
	gcc -Wall -Werror -c User.c

Wrapper.o: Wrapper.c
	gcc -Wall -Werror -c Wrapper.c

server: Server.c User.o Wrapper.o
	gcc -Wall -Werror -pthread -o server Server.c User.o Wrapper.o

client: Client.c Wrapper.o
	gcc -Wall -Werror -o client Client.c Wrapper.o

clean:
	rm -f *~ *.o server client