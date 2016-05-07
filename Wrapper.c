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
		sfwrite(&Q_lock, stdout, "\x1B[1;34mOutgoing: %s\x1B[0m\n", msg);
	}
}

void Recv(int socket, void *buffer, size_t length, int flags) {
	char msg[MAX_LEN];
	char temp[MAX_BUF_LEN];
	char *ch;
	int n;

	memset(buffer, 0, length);

	do {
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
	} while(strncmp(buffer, "UOFF ", 5) == 0);

	if(verboseFlag == TRUE) {
		strcpy(msg, buffer);
		if(strcmp(&msg[strlen(msg)-4], "\r\n\r\n") == 0) {
			msg[strlen(msg)-5] = '\0';
		}
		sfwrite(&Q_lock, stdout, "\x1B[1;34mIncoming: %s\x1B[0m\n", msg);
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
		sfwrite(&Q_lock, stdout, "\x1B[1;34mIncoming: %s\x1B[0m\n", msg);
	}
}

void printLoginLog(int fd, char *userName, char *ip, char *port, int isSucceed, char *msg) {
	char buf[MAX_LEN];
	char timeStamp[MAX_TIME_LEN];

	getTimeStamp(timeStamp);

	if(isSucceed == TRUE) {
		sprintf(buf, "%s, %s, LOGIN, %s:%s, success, %s\n", timeStamp, userName, ip, port, msg);
	} else {
		sprintf(buf, "%s, %s, LOGIN, %s:%s, fail, %s\n", timeStamp, userName, ip, port, msg);
	}

	flock(fd, LOCK_EX);
	write(fd, buf, strlen(buf));
	flock(fd, LOCK_UN);
}

void printCmdLog(int fd, char *userName, char *command, int isSucceed, char *origin) {
	char buf[MAX_LEN];
	char timeStamp[MAX_TIME_LEN];

	getTimeStamp(timeStamp);

	if(isSucceed == TRUE) {
		sprintf(buf, "%s, %s, CMD, %s, success, %s\n", timeStamp, userName, command, origin);
	} else {
		sprintf(buf, "%s, %s, CMD, %s, fail, %s\n", timeStamp, userName, command, origin);
	}
	
	flock(fd, LOCK_EX);
	write(fd, buf, strlen(buf));
	flock(fd, LOCK_UN);
}

void printMsgLog(int fd, char *userName, char *origin, char *partner, char *msg) {
	char buf[MAX_LEN];
	char timeStamp[MAX_TIME_LEN];

	getTimeStamp(timeStamp);

	sprintf(buf, "%s, %s, MSG, %s, %s, %s\n", timeStamp, userName, origin, partner, msg);

	flock(fd, LOCK_EX);
	write(fd, buf, strlen(buf));
	flock(fd, LOCK_UN);
}

void printLogoutLog(int fd, char *userName, int isIntentional) {
	char buf[MAX_LEN];
	char timeStamp[MAX_TIME_LEN];

	getTimeStamp(timeStamp);

	if(isIntentional == TRUE) {
		sprintf(buf, "%s, %s, LOGOUT, intentional\n", timeStamp, userName);
	} else {
		sprintf(buf, "%s, %s, LOGOUT, error\n", timeStamp, userName);
	}

	flock(fd, LOCK_EX);
	write(fd, buf, strlen(buf));
	flock(fd, LOCK_UN);
}

void printErrLog(int fd, char *userName, char *msg) {
	char buf[MAX_LEN];
	char timeStamp[MAX_TIME_LEN];

	getTimeStamp(timeStamp);

	sprintf(buf, "%s, %s, ERR, %s\n", timeStamp, userName, msg);

	flock(fd, LOCK_EX);
	write(fd, buf, strlen(buf));
	flock(fd, LOCK_UN);
}

void getTimeStamp(char *timeStamp) {
	time_t rawTime;
	struct tm *timeInfo;

	time(&rawTime);
	timeInfo = localtime(&rawTime);

	strftime(timeStamp, MAX_TIME_LEN, "%D-%I:%M%p", timeInfo);
}

void printError(char *msg) {
  sfwrite(&Q_lock, stderr, "\x1B[1;31mERROR: ");
  sfwrite(&Q_lock, stderr, "%s", msg);
  sfwrite(&Q_lock, stderr, "\x1B[0m");
}