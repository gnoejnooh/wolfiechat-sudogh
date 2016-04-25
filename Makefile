all: server client chat

Database.o: Database.c
	gcc -Wall -Werror -c Database.c

User.o: User.c
	gcc -Wall -Werror -c User.c

Wrapper.o: Wrapper.c
	gcc -Wall -Werror -c Wrapper.c

chat: Chat.c Wrapper.o
	gcc -Wall -Werror -o chat Chat.c Wrapper.o

server: Server.c Database.o User.o Wrapper.o
	gcc -Wall -Werror -o server Server.c Database.o User.o Wrapper.o -pthread -lssl -lcrypto -lsqlite3 

client: Client.c User.o Wrapper.o
	gcc -Wall -Werror -o client Client.c User.o Wrapper.o -pthread 

clean:
	rm -f *~ *.o server client chat