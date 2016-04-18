#include "Wrapper.h"

void Send(int socket, const void *buffer, size_t length, int flags, int verboseFlag) {
	char msg[MAX_LEN];

	send(socket, buffer, length, flags);
	if(verboseFlag == TRUE) {
		strcpy(msg, buffer);
		msg[strlen(msg)-5] = '\0';
		printf("\x1B[1;34mOutgoing: %s\x1B[0m\n", msg);
	}
}

void Recv(int socket, void *buffer, size_t length, int flags, int verboseFlag) {
	char msg[MAX_LEN];

	recv(socket, buffer, length, flags);
	if(verboseFlag == TRUE) {
		strcpy(msg, buffer);
		msg[strlen(msg)-5] = '\0';
		printf("\x1B[1;34mIncoming: %s\x1B[0m\n", msg);
	}
}