#include "Wrapper.h"

void Send(int socket, const void *buffer, size_t length, int flags) {
	char msg[MAX_LEN];
	int n;
	
	if((n = send(socket, buffer, length, flags)) == 0) {
		return;
	}

	if(verboseFlag == TRUE) {
		strcpy(msg, buffer);
		if(strcmp(&msg[strlen(msg)-4], "\r\n\r\n") == 0) {
			msg[strlen(msg)-5] = '\0';
		}
		printf("\x1B[1;34mOutgoing: %s\x1B[0m\n", msg);
	}
}

void Recv(int socket, void *buffer, size_t length, int flags) {
	char msg[MAX_LEN];
	char temp[MAX_BUF_LEN];
	char *ch;
	int n;

	memset(buffer, 0, length);

	if((ch = strstr(stream, "\r\n\r\n")) != NULL) {
		strcpy(temp, &ch[4]);
		ch[4] = '\0';
		strcpy(buffer, stream);
		strcpy(stream, temp);
	} else {
		if((n = recv(socket, buffer, length, flags)) == 0) {
			return;
		}

		strcat(stream, (char *)buffer);
		ch = strstr(stream, "\r\n\r\n");
		strcpy(temp, &ch[4]);
		ch[4] = '\0';
		strcpy(buffer, stream);
		strcpy(stream, temp);
	}

	if(verboseFlag == TRUE) {
		strcpy(msg, buffer);
		if(strcmp(&msg[strlen(msg)-4], "\r\n\r\n") == 0) {
			msg[strlen(msg)-5] = '\0';
		}
		printf("\x1B[1;34mIncoming: %s\x1B[0m\n", msg);
	}
}

void printError(char *msg) {
  fprintf(stderr, "\x1B[1;31mERROR: ");
  fprintf(stderr, "%s", msg);
  fprintf(stderr, "\x1B[0m");
}