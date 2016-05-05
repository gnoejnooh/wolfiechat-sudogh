#include "Chat.h"

int main(int argc, char** argv) {
	
	char buf[MAX_LEN];

	fd_set readSet;
	fd_set readySet;

	chatfd = atoi(argv[1]);

	memset(buf, 0, MAX_LEN);

	signal(SIGINT, sigintHandler);
	signal(SIGHUP, sighupHandler);
  
	FD_ZERO(&readSet);
	FD_SET(STDIN, &readSet);
	FD_SET(chatfd, &readSet);

	while(TRUE) {
		readySet = readSet;
		select(chatfd+1, &readySet, NULL, NULL, NULL);

		if(FD_ISSET(STDIN, &readySet)) {
			fgets(buf, MAX_LEN, stdin);
			buf[strlen(buf)-1] = '\0';

			send(chatfd, buf, sizeof(buf), 0);

			if(strcmp(buf, "/close") == 0) {
				break;
			}
		}

		if(FD_ISSET(chatfd, &readySet)) {
			memset(buf, 0, MAX_LEN);
			recv(chatfd, buf, MAX_LEN, 0);
			printf("%s\n", buf);
		}
	}

	close(chatfd);
	return 0;
}

void sigintHandler(int signum) {
  send(chatfd, "/close", sizeof("/close"), 0);
  exit(EXIT_FAILURE);
}

void sighupHandler(int signum) {
  send(chatfd, "/close", sizeof("/close"), 0);
  exit(EXIT_FAILURE);
}