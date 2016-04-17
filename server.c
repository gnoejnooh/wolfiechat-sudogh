#include "server.h"

int main(int argc, char **argv) {

	int listenfd;
	int epollfd;
  int connfd;
	int VB = 0;
	int rc;
	char portno[65535];
	pthread_t tid;
  char motd[MAX_LEN];
	char buffer[MAX_LEN];
	param_t param;
	struct epoll_event event;

	struct sigaction sa;
  sa.sa_handler = &sigHandler;
  sa.sa_flags = SA_RESTART;
  Sigaction(SIGINT, &sa, NULL);
  Signal(SIGINT, sigHandler);

	VB = parseOption(argc, argv, portno, motd);
	listenfd = create_and_bind(portno);
  
	/* Listen on the socket for connection */
	rc = listen(listenfd, 10);
	if(rc < 0) {
		printError("error in listen");
	}
	/* Print before loop */
	printf("Currently listening on port %s\n", portno);
	/* Create epoll */
	epollfd = epoll_create1(0);
	if(rc < 0) {
		printError("error while creating epoll");
	}
	/* Add stdin */
	event.data.fd = fileno(stdin);
	event.events = EPOLLIN | EPOLLET;
	rc = epoll_ctl(epollfd, EPOLL_CTL_ADD, fileno(stdin), &event);
	if(rc < 0) {
		printError("epoll_ctl error");
	}
	/* Add socket */
	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	rc = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);
	if(rc < 0) {
		printError("epoll_ctl error");
	}

	while(TRUE) {
		int nfds;
		nfds = epoll_wait(epollfd, &event, 1, -1);
		if(nfds < 0) {
			printError("epoll_wait failed");
		}
		if(event.data.fd == fileno(stdin)) {
			printf("server> ");
      fflush(stdout);
			bzero(buffer, MAX_LEN);
			fgets(buffer, MAX_LEN, stdin);
			if(!strcmp(buffer, "/users\n")) {
				printf("List of users should be printed\n");
			} else if(!strcmp(buffer, "/help\n")) {
				printUsage();
			} else if(!strcmp(buffer, "/shutdown\n")) {
				break;
			} else {
				fprintf(stderr, "\x1B[1;31mERROR: command does not exist\x1B[0m\n");
			}
		} else if(event.data.fd == listenfd) {
			struct sockaddr_in cli_addr;
			socklen_t clilen = sizeof(cli_addr);
			connfd = accept(listenfd, (struct sockaddr*) &cli_addr, &clilen);
			param.VB = VB;
			param.connfd = connfd;
			param.motd = motd;
			pthread_create(&tid, 0, (void*)&login_handler, (void*) &param);
		} else {
			printError("fd error");
		}
	}
	printf("Shutting down...\n");
	close(epollfd);
	close(listenfd);

	return 0;
}

int create_and_bind(char* port) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int rc, listenfd;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	rc = getaddrinfo(NULL, port, &hints, &result);
	if(rc < 0) {
		printError("getaddrinfo error");
	}
	for(rp = result; rp != NULL; rp = rp->ai_next) {
		listenfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(listenfd < 0) continue;
		rc = bind(listenfd, rp->ai_addr, rp->ai_addrlen);
		if(rc == 0) break;
		close(listenfd);
	}
	if(rp == NULL) {
		printError("bind error");
	}
	freeaddrinfo(result);
	return listenfd;
}

void login_handler(void* incoming) {
	param_t *login_param = (param_t*) incoming;
	int connfd = login_param->connfd;
	int VB = login_param->VB;
	char buf_in[MAX_LEN];
	char buf_out[MAX_LEN];
	char* name = NULL;
	pthread_t tid;

  wolfieProtocol(connfd, VB);
  bzero(buf_in, sizeof(buf_in));
	read(connfd, buf_in, sizeof(buf_in));
	strip_crnl(buf_in);
	if(VB) {
		printVerbose(buf_in, INCOMING);
	}
	if(!strcmp(strtok(buf_in, " "), "IAM")) {
		name = strtok(NULL, " ");
	}
	//TODO: verify user name; reject if used
  sprintf(buf_out, "HI %s\r\n\r\n", name);
  write(connfd, buf_out, sizeof(buf_out));
  if(VB) {
		printVerbose(buf_out, OUTGOING);
	}
  bzero(buf_out, sizeof(buf_out));
  sprintf(buf_out, "MOTD %s\r\n\r\n", login_param->motd);
  write(connfd, buf_out, sizeof(buf_out));
  if(VB) {
		printVerbose(buf_out, OUTGOING);
	}
	pthread_create(&tid, 0, (void*)&handler, (void*) &login_param);
}

void handler(void* incoming) {
	param_t *comm_param = (param_t*) incoming;
	int n;
	int connfd = comm_param->connfd;
	int VB = comm_param->VB;
	time_t t = time(NULL);
	char buf_in[MAX_LEN];
	char buf_out[MAX_LEN];


	while((n = read(connfd, buf_in, sizeof(buf_in))) > 0) {
		buf_in[n] = '\0';
		buf_out[0] = '\0';

		if(!strlen(buf_in)) {
			continue;
		}
		if(VB) {
  		printVerbose(buf_in, INCOMING);
  	}
		if(!strcmp(buf_in, "BYE\r\n\r\n")) {
			if(write(connfd, "BYE\r\n\r\n", 7) < 0) {
				printError("unable to write BYE protocol");
  		}
			break;
		} else if(!strcmp(buf_in, "TIME\r\n\r\n")) {
			time_t current = time(NULL);
			current = current - t;
			sprintf(buf_out, "EMIT %ld\r\n\r\n", current);
			if(write(connfd, buf_out, sizeof(buf_out)) < 0) {
				printError("unable to write EMIT protocol");
  		}
		}
	}
	close(connfd);	
}

void strip_crnl(char* str) {
  while(*str != '\0') {
    if(*str == '\r' || *str == '\n') {
      *str = ' ';
    }
    str++;
  }
}

void wolfieProtocol(int connfd, int VB) {
	char buffer[MAX_LEN];
	char wolfie[10] = {'W', 'O', 'L', 'F', 'I', 'E', '\r', '\n', '\r', '\n'};
	char eiflow[10] = {'E', 'I', 'F', 'L', 'O', 'W', '\r', '\n', '\r', '\n'};
	/* Read WOLFIE protocol */
	read(connfd, buffer, sizeof(buffer));
  if(strcmp(buffer, wolfie)) {
  	printError("protocol does not match");
  }
  if(VB) {
  	printVerbose(buffer, INCOMING);
	}
  write(connfd, eiflow, sizeof(eiflow));
  if(VB) {
  	printVerbose(buffer, OUTGOING);
	}
}

void initializeDatabase(sqlite3 *db, sqlite3_stmt *res, char *err_msg) {
  Sqlite3_open("usrinfo.db", &db);
  Sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);
  sqlite3_step(res);
}

void Sqlite3_open(const char *filename, sqlite3 **ppDb) {
  int rc = sqlite3_open(filename, ppDb);
  if(rc != SQLITE_OK) {
    printError("ERROR: Cannot open database");
    sqlite3_close(*ppDb);
  }
}

void Sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail) {
  int rc = sqlite3_prepare_v2(db, zSql, nByte, ppStmt, pzTail);
  if(rc != SQLITE_OK) {
    printError("ERROR: Failed to fetch data");
    sqlite3_close(db);
  }
}

void printUsage() {
  fprintf(stderr, "USAGE: [-h|-v] PORT_NUMBER MOTD\n");
  fprintf(stderr, "-h           Displays help menu & returns EXIT_SUCCESS.\n");
  fprintf(stderr, "-v           Verbose print all incoming and outgoing protocol verbs & content.\n");
  fprintf(stderr, "PORT_NUMBER  Port number to listen on.\n");
  fprintf(stderr, "MOTD         Message to display to the client when they connect.\n");
}

void printError(const char *msg) {
  fprintf(stderr, "\x1B[1;31mError: %s\n", msg);
  exit(1);
}

void printVerbose(char *msg, int flag) {
	char buffer[MAX_LEN];
	char* end;
	strip_crnl(msg);
	end = msg + strlen(msg) - 1;
	while(end > msg && isspace(*end)) end--;
	*(end+1) = 0;
	if(!flag) {
		sprintf(buffer, "Incoming: %s", msg);
	} else {
		sprintf(buffer, "Outgoing: %s", msg);
	}
	fprintf(stdout, "\x1B[1;34m%s\x1B[0m\n", buffer);
}

void Signal(int sig, void (*func)(int)) {
  if(signal(sig, func) == SIG_ERR) {
    printError("ERROR: Error on signal handler");
  }
}

void Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
  if(sigaction(signum, act, oldact) == -1) {
    printError("ERROR: Cannot handle SIGINT");
  }
}

void sigHandler(int signal) {
  if(signal == SIGINT) {
    printf("\nHandling SIGINT...\n");
    exit(0);
  }
}

int parseOption(int argc, char **argv, char *portno, char *motd) {

  int opt;
  int verbose = 0;

  while((opt = getopt(argc, argv, "hv:")) != -1) {
    switch(opt) {
    case 'h':
      /* The help menu was selected */
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case 'v':
    	verbose = 1;
      break;
    case '?':
      /* Let this case fall down to default;
       * handled during bad option.
       */
    default:
      /* A bad option was provided. */
      printUsage();
      exit(EXIT_FAILURE);
      break;
    }
  }
  
  /* Get position arguments */
  if(argc == 3) {
    strcpy(portno, argv[1]);
    strcpy(motd, argv[2]);
  } else {
    if(argc == 1) {
      printError("Missing PORT_NUMBER and MOTD.");
    } else if(argc == 2) {
      printError("Missing MOTD.");
    } else {
      printError("Too many arguments provided.");
    }
    printUsage();
    exit(EXIT_FAILURE);
  }
  return verbose;
}