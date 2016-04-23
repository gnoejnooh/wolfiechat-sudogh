#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Constant.h"
#include "Wrapper.h"

int main(int argc, char** argv) {
	int unixfd;
	int runflag = TRUE;
	char buf[MAX_LEN];
	unixfd = atoi(argv[1]);
  
	while(runflag) {
		printf(">");
		scanf("%s", buf);
		if(!strcmp(buf, "close")) {
			runflag = FALSE;
		}
		send(unixfd, buf, sizeof(buf), 0);
		recv(unixfd, buf, sizeof(buf), 0);
		printf("%s\n", buf);
		fflush(stdout);
	}
	printf("TEST EXIT");
	fflush(stdout);
	return 0;
}