#include "Chat.h"

int main(int argc, char** argv) {
	
	int unixfd = atoi(argv[1]);
	char buf[MAX_LEN];

	memset(buf, 0, MAX_LEN);
  
	while(TRUE) {
		fgets(buf, MAX_LEN, stdin);
		buf[strlen(buf)-1] = '\0';

		if(strcmp(buf, "close") == 0) {
			break;
		}

		send(unixfd, buf, sizeof(buf), 0);
		printf("%s\n", buf);
	}

	return 0;
}