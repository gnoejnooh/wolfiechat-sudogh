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

	if((ch = strstr(localBuffer, "\r\n\r\n")) != NULL) {
	
		strcpy(temp, &ch[4]);
		ch[4] = '\0';
		strcpy(buffer, localBuffer);
		strcpy(localBuffer, temp);
	} else {

		if((n = recv(socket, buffer, length, flags)) == 0) {
			return;
		}

		strcat(localBuffer, (char *)buffer);
		ch = strstr(localBuffer, "\r\n\r\n");
		strcpy(temp, &ch[4]);
		ch[4] = '\0';
		strcpy(buffer, localBuffer);
		strcpy(localBuffer, temp);
	}

	if(verboseFlag == TRUE) {
		strcpy(msg, buffer);
		if(strcmp(&msg[strlen(msg)-4], "\r\n\r\n") == 0) {
			msg[strlen(msg)-5] = '\0';
		}
		printf("\x1B[1;34mIncoming: %s\x1B[0m\n", msg);
	}
}

void RecvChat(int socket, void *buffer, size_t length, int flags) {
	char msg[MAX_LEN];
	int n;

	memset(buffer, 0, length);

	if((n = recv(socket, buffer, length, flags)) == 0) {
		return;
	}

	if(verboseFlag == TRUE) {
		strcpy(msg, buffer);
		printf("\x1B[1;34mIncoming: %s\x1B[0m\n", msg);
	}
}

void printLog(int fd, char *userName, char *event, ...) {
	va_list ap;
	
	time_t rawTime;
	struct tm *info;

	char buf[MAX_LEN];

	time(&rawTime);
	info = localtime(&rawTime);
	strftime(buf, MAX_LEN, "%x-%I:%M%p, ", info);
	strcat(buf, userName);
	strcat(buf, ", ");
	strcat(buf, event);
	strcat(buf, ", ");
	va_start(ap, event);
	if(strcmp(event, "LOGIN") == 0) {
		strcat(buf, va_arg(ap, char *));
		strcat(buf, ":");
		strcat(buf, va_arg(ap, char *));
		strcat(buf, ", ");
		strcat(buf, va_arg(ap, char *));
		strcat(buf, ", ");
		strcat(buf, va_arg(ap, char *));
	} else if(strcmp(event, "CMD")) {
		strcat(buf, va_arg(ap, char*));
		strcat(buf, ", ");
		strcat(buf, va_arg(ap, char *));
		strcat(buf, ", ");
		strcat(buf, va_arg(ap, char *));
	} else if(strcmp(event, "MSG")) {
		strcat(buf, va_arg(ap, char*));
		strcat(buf, ", ");
		strcat(buf, va_arg(ap, char *));
		strcat(buf, ", ");
		strcat(buf, va_arg(ap, char *));
	} else if(strcmp(event, "LOGOUT")) {
		strcat(buf, va_arg(ap, char*));
	} else if(strcmp(event, "ERR")) {
		strcat(buf, va_arg(ap, char*));
	}
	strcat(buf, "\n");
	printf("TEST%s", buf);
	write(fd, &buf, sizeof(buf));
}

void printError(char *msg) {
  fprintf(stderr, "\x1B[1;31mERROR: ");
  fprintf(stderr, "%s", msg);
  fprintf(stderr, "\x1B[0m");
}