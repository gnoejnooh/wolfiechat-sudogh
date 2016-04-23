all: server client chat

User.o: User.c
	gcc -Wall -Werror -c User.c

Wrapper.o: Wrapper.c
	gcc -Wall -Werror -c Wrapper.c

chat: Chat.c Wrapper.o
	gcc -Wall -Werror -o chat Chat.c Wrapper.o

server: Server.c User.o Wrapper.o
	gcc -Wall -Werror -pthread -o server Server.c User.o Wrapper.o

client: Client.c User.o Wrapper.o
	gcc -Wall -Werror -pthread -o client Client.c User.o Wrapper.o

clean:
	rm -f *~ *.o server client chat