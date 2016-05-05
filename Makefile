all: server client chat

sfwrite: sfwrite.c
	gcc -Wall -Werror sfwrite.c

Database.o: Database.c sfwrite.c
	gcc -Wall -Werror -c Database.c sfwrite.c

User.o: User.c sfwrite.c
	gcc -Wall -Werror -c User.c sfwrite.c

Wrapper.o: Wrapper.c
	gcc -Wall -Werror -c Wrapper.c

chat: Chat.c Wrapper.o
	gcc -Wall -Werror -o chat Chat.c Wrapper.o

server: Server.c Database.o User.o Wrapper.o sfwrite.c
	gcc -Wall -Werror -o server Server.c Database.o User.o Wrapper.o sfwrite.c -pthread -lssl -lcrypto -lsqlite3 

client: Client.c User.o Wrapper.o sfwrite.c
	gcc -Wall -Werror -o client Client.c User.o Wrapper.o sfwrite.c -pthread 

clean:
	rm -f *~ *.o server client chat