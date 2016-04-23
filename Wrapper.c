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
	int n;

	memset(buffer, 0, length);

	if((n = recv(socket, buffer, length, flags)) == 0) {
		return;
	}

	if(verboseFlag == TRUE) {
		strcpy(msg, buffer);
		if(strcmp(&msg[strlen(msg)-4], "\r\n\r\n") == 0) {
			msg[strlen(msg)-5] = '\0';
		}
		printf("\x1B[1;34mIncoming: %s\x1B[0m\n", msg);
	}
}