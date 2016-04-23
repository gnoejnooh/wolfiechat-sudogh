#include "Chat.h"

int main(int argc, char** argv) {
	
	int unixfd = atoi(argv[1]);
	int msgflag = FALSE;
	char buf[MAX_LEN];

	fd_set readSet;
	fd_set readySet;

	memset(buf, 0, MAX_LEN);
  
	FD_ZERO(&readSet);
	FD_SET(STDIN, &readSet);
	FD_SET(unixfd, &readSet);

	while(TRUE) {
		readySet = readSet;
		select(unixfd+1, &readySet, NULL, NULL, NULL);

		if(FD_ISSET(STDIN, &readySet)) {
			fgets(buf, MAX_LEN, stdin);
			buf[strlen(buf)-1] = '\0';

			if(strcmp(buf, "/close") == 0) {
				break;
			}
			send(unixfd, buf, sizeof(buf), 0);
			msgflag = TRUE;
		}
		if(FD_ISSET(unixfd, &readySet)) {
			memset(buf, 0, MAX_LEN);
			recv(unixfd, buf, MAX_LEN, 0);
			if(msgflag) {
				printf("> ");
			} else {
				printf("< ");
			}
			printf("%s\n", buf);
			msgflag = FALSE;
		}
	}

	return 0;
}