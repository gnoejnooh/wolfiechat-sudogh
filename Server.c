#include "Server.h"

int main(int argc, char **argv) {
  
  int listenfd;
  int epollfd;
  int *connfd = NULL;

  char port[MAX_PORT_LEN];
  char motd[MAX_LEN];
  char accountsFile[MAX_FILE_LEN];

  struct sockaddr_in *connAddr = malloc(sizeof(struct sockaddr_in));
  socklen_t connLen;

  struct epoll_event event;

  pthread_t tid;

  LoginThreadParam *loginThreadParam = NULL;

  signal(SIGINT, sigintHandler);

  db = NULL;

  verboseFlag = FALSE;
  runFlag = TRUE;

  parseOption(argc, argv, port, motd, accountsFile);

  openDatabase(&db, accountsFile);
  initializeUserList(&userList);

  listenfd = openListenFd(port);

  epollfd = epoll_create1(0);

  event.data.fd = STDIN;
  event.events = EPOLLIN | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fileno(stdin), &event);

  event.data.fd = listenfd;
  event.events = EPOLLIN | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);

  printf("Currently listening on port %s\n", port);

  while(runFlag) {

    epoll_wait(epollfd, &event, 1, -1);

    if(event.data.fd == STDIN) {
      executeCommand();

      if(runFlag == FALSE) {
        break;
      }
    }

    if(event.data.fd == listenfd) {
      connLen = sizeof(struct sockaddr_in);
      loginThreadParam = malloc(sizeof(LoginThreadParam));
      connfd = malloc(sizeof(int));
    
      if((*connfd = accept(listenfd, (struct sockaddr *)connAddr, &connLen)) != -1) {
        loginThreadParam->connfd = connfd;
        strcpy(loginThreadParam->motd, motd);
        pthread_create(&tid, NULL, loginThread, loginThreadParam);
      } else {
        free(connfd);
      }
    }
  }

  sqlite3_close(db);
  close(listenfd);
  free(connAddr);
  freeUserList(&userList);
  return EXIT_SUCCESS;
}

void parseOption(int argc, char **argv, char *port, char *motd, char *accountsFile) {

  int opt;

  memset(port, 0, MAX_PORT_LEN);
  memset(motd, 0, MAX_LEN);
  memset(accountsFile, 0, MAX_FILE_LEN);

  while((opt = getopt(argc, argv, "hv")) != -1) {
    switch(opt) {
    case 'h':
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      verboseFlag = TRUE;
      break;
    case '?':
    default:
      printUsage();
      exit(EXIT_FAILURE);
      break;
    }
  }

  if(optind < argc && (argc - optind) == 2) {
    strcpy(port, argv[optind++]);
    strcpy(motd, argv[optind++]);
  } else if(optind < argc && (argc - optind) == 3) {
    strcpy(port, argv[optind++]);
    strcpy(motd, argv[optind++]);
    strcpy(accountsFile, argv[optind++]);
  } else {
    if(argc - optind < 2) {
      printError("Missing arguments\n");
    } else if(argc - optind > 3) {
      printError("Too many arguments\n");
    }

    printUsage();
    exit(EXIT_FAILURE);
  }
}

int openListenFd(char *port) {

  struct addrinfo hints;
  struct addrinfo *list;
  struct addrinfo *cur;

  int listenfd;
  int opt = 1;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;
  getaddrinfo(NULL, port, &hints, &list);

  for(cur=list; cur!=NULL; cur=cur->ai_next) {
    if((listenfd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol)) < 0) {
      continue;
    }

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(int));

    if(bind(listenfd, cur->ai_addr, cur->ai_addrlen) == 0) {
      break;
    }

    close(listenfd);
  }

  freeaddrinfo(list);

  if(cur == NULL) {
    return -1;
  }

  if(listen(listenfd, BACKLOG) < 0) {
    close(listenfd);
    return -1;
  }

  return listenfd;
}

void executeCommand() {
  
  char buf[MAX_LEN];
  fgets(buf, MAX_LEN, stdin);

  if(strcmp(buf, "/users\n") == 0) {
    printAllUserInfo(userList);
  } else if(strcmp(buf, "/help\n") == 0) {
    printUsage();
  } else if(strcmp(buf, "/accts\n") == 0) {
    printAllAccountsInfo(&db);
  } else if(strcmp(buf, "/shutdown\n") == 0) {
    shutdownCommand();
  } else {
    printError("Command does not exist\n");
  }
}

void shutdownCommand() {
  User *cur = userList.head;
  int connfd;

  while(cur != NULL) {
    connfd = cur->connfd;
    Send(connfd, "BYE \r\n\r\n", sizeof("BYE \r\n\r\n"), 0);
    cur = cur->next;
  }

  runFlag = FALSE;
}

void printPrompt() {
  printf("server> ");
  fflush(stdout);
}

void printUsage() {
  fprintf(stderr, "USAGE: ./server [-h|-v] PORT_NUMBER MOTD\n");
  fprintf(stderr, "-h            Displays help menu & returns EXIT_SUCCESS.\n");
  fprintf(stderr, "-v            Verbose print all incoming and outgoing protocol verbs & content.\n");
  fprintf(stderr, "PORT_NUMBER   Port number to listen on.\n");
  fprintf(stderr, "MOTD          Message to display to the client when they connect.\n");
}

void * loginThread(void *argv) {
  LoginThreadParam *param = (LoginThreadParam *)argv;
  int connfd = *(param->connfd);
  char motd[MAX_LEN];

  pthread_t tid;

  char buf[MAX_LEN];
  char userName[MAX_NAME_LEN];

  strcpy(motd, param->motd);

  pthread_detach(pthread_self());
  free(param->connfd);
  free(argv);

  Recv(connfd, buf, MAX_LEN, 0);

  if(strcmp(buf, "WOLFIE \r\n\r\n") == 0) {
    
    Send(connfd, "EIFLOW \r\n\r\n", strlen("EIFLOW \r\n\r\n"), 0);

    if(authenticateUser(connfd, userName) == TRUE) {
      if(promptPassword(connfd, userName) == TRUE) {
        sprintf(buf, "HI %s \r\n\r\n", userName);
        Send(connfd, buf, strlen(buf), 0);
        insertUser(&userList, userName, connfd);

        sprintf(buf, "MOTD %s \r\n\r\n", motd);
        Send(connfd, buf, strlen(buf), 0);

        CommunicationThreadParam *communicationThreadParam = malloc(sizeof(CommunicationThreadParam));
        communicationThreadParam->connfd = &connfd;
        strcpy(communicationThreadParam->userName, userName);
        pthread_create(&tid, NULL, communicationThread, communicationThreadParam);
      }
    }
  }

  return NULL;
}

int authenticateUser(int connfd, char *userName) {

  char buf[MAX_LEN];
  
  Recv(connfd, buf, MAX_LEN, 0);

  if(strncmp(buf, "IAMNEW ", 7) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
    
    sscanf(buf, "IAMNEW %s \r\n\r\n", userName);

    if(isAccountExist(&db, userName) == TRUE) {
      sprintf(buf, "ERR 00 USER NAME TAKEN %s \r\n\r\n", userName);
      Send(connfd, buf, strlen(buf), 0);
      Send(connfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0);

      return FALSE;
    } else {
      sprintf(buf, "HINEW %s \r\n\r\n", userName);
      Send(connfd, buf, strlen(buf), 0);

      return TRUE;
    }
  } else if (strncmp(buf, "IAM ", 4) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
    
    sscanf(buf, "IAM %s \r\n\r\n", userName);
    
    if(isAccountExist(&db, userName) == FALSE) {
      Send(connfd, "ERR 01 USER NOT AVAILABLE \r\n\r\n", strlen("ERR 01 USER NOT AVAILABLE \r\n\r\n"), 0);
      Send(connfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0);

      return FALSE;
    } else if(isUserExist(userList, userName) == FALSE) {
      sprintf(buf, "AUTH %s \r\n\r\n", userName);
      Send(connfd, buf, strlen(buf), 0);

      return TRUE;
    } else {
      Send(connfd, "ERR 00 USER NAME TAKEN \r\n\r\n", strlen("ERR 00 USER NAME TAKEN \r\n\r\n"), 0);
      Send(connfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0);

      return FALSE;
    }
  }

  return FALSE;
}

int promptPassword(int connfd, char *userName) {

  char buf[MAX_LEN];
  char password[MAX_PASSWORD_LEN];
  
  Recv(connfd, buf, MAX_LEN, 0);

  if(strncmp(buf, "NEWPASS ", 8) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
    sscanf(buf, "NEWPASS %s \r\n\r\n", password);

    if(verifyPasswordCriteria(password) == TRUE) {
      char salt[MAX_SALT_LEN*2+1];
      char hash[SHA256_DIGEST_LENGTH*2+1];
      
	    Send(connfd, "SSAPWEN \r\n\r\n", strlen("SSAPWEN \r\n\r\n"), 0);
      getSalt(salt);
      getHash(hash, password, salt);
	    insertAccount(&db, userName, hash, salt);
	    return TRUE;
    } else {
    	Send(connfd, "ERR 02 BAD PASSWORD \r\n\r\n", strlen("ERR 00 BAD PASSWORD \r\n\r\n"), 0);
    	Send(connfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0);
      return FALSE;
    }
  } else if (strncmp(buf, "PASS ", 5) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
    sscanf(buf, "PASS %s \r\n\r\n", password);
    
    if(verifyPassword(&db, userName, password) == TRUE) {
      Send(connfd, "SSAP \r\n\r\n", strlen("SSAP \r\n\r\n"), 0);
      return TRUE;
    } else {
      Send(connfd, "ERR 02 BAD PASSWORD \r\n\r\n", strlen("ERR 02 BAD PASSWORD \r\n\r\n"), 0);
      Send(connfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0);
      return FALSE;
    }
  }

  return FALSE;
}

int verifyPasswordCriteria(char *password) {
	
	int i;
	int upper = 0;
	int lower = 0;
	int symbol = 0;
	int number = 0;
	
	for(i = 0; i < strlen(password); i++) {
		if(password[i] > 47 && password[i] < 58) {
			number++;
		} else if(password[i] > 64 && password[i] < 91) {
			upper++;
		} else if(password[i] > 96 && password[i] < 123) {
			lower++;
		} else {
			symbol++;
		}
	}

	if(strlen(password) >= 5 && upper && symbol && number) {
		return TRUE;
	}

	return FALSE;
}

void * communicationThread(void *argv) {

  CommunicationThreadParam *param = (CommunicationThreadParam *)argv;
  int connfd = *(param->connfd);
  char userName[MAX_NAME_LEN];

  char buf[MAX_LEN];
  time_t begin = time(NULL);

  strcpy(userName, param->userName);

  pthread_detach(pthread_self());
  free(argv);

  while(TRUE) {
    Recv(connfd, buf, MAX_LEN, 0);

    if(strcmp(buf, "TIME \r\n\r\n") == 0) {
      receiveTimeMessage(connfd, begin);
    } else if(strcmp(buf, "LISTU \r\n\r\n") == 0) {
      receiveListuMessage(connfd);
    } else if(strncmp(buf, "MSG", 3) == 0) {
      receiveChatMessage(connfd, buf);
    } else if(strcmp(buf, "BYE \r\n\r\n") == 0) {
      receiveByeMessage(connfd, userName);
      break;
    }
  }

  close(connfd);

  return NULL;
}

void receiveTimeMessage(int connfd, time_t begin) {
  char buf[MAX_LEN];
  time_t current = time(NULL);

  sprintf(buf, "EMIT %ld \r\n\r\n", current-begin);
  Send(connfd, buf, sizeof(buf), 0);
}

void receiveListuMessage(int connfd) {
  char buf[MAX_LISTU_LEN];

  User *cur = userList.head;

  sprintf(buf, "UTSIL");

  while(cur != NULL) {
    sprintf(buf+strlen(buf), " %s \r\n", cur->userName);
    cur = cur->next;
  }

  sprintf(buf+strlen(buf), "\r\n");
  Send(connfd, buf, sizeof(buf), 0);
}

void receiveChatMessage(int connfd, char *line) {
  char to[MAX_NAME_LEN];
  char from[MAX_NAME_LEN];

  User *user;
  int userConnfd;

  sscanf(line, "MSG %s %s", to, from);

  if(isUserExist(userList, to) == FALSE || isUserExist(userList, from) == FALSE) {
    if((user = findUser(userList, to)) != NULL) {
      userConnfd = user->connfd;
      Send(userConnfd, "ERR 01 USER NOT AVAILABLE \r\n\r\n", sizeof("ERR 01 USER NOT AVAILABLE"), 0);
    }

    if((user = findUser(userList, from)) != NULL) {
      userConnfd = user->connfd;
      Send(userConnfd, "ERR 01 USER NOT AVAILABLE \r\n\r\n", sizeof("ERR 01 USER NOT AVAILABLE"), 0);
    }
  } else {
    if((user = findUser(userList, to)) != NULL) {
      userConnfd = user->connfd;
      Send(userConnfd, line, MAX_LEN, 0);
    }

    if((user = findUser(userList, from)) != NULL) {
      userConnfd = user->connfd;
      Send(userConnfd, line, MAX_LEN, 0);
    }
  }
}

void receiveByeMessage(int connfd, char *userName) {
  char buf[MAX_LEN];

  User *cur;

  sprintf(buf, "UOFF %s \r\n\r\n", userName);

  Send(connfd, "BYE \r\n\r\n", sizeof("BYE \r\n\r\n"), 0);
  deleteUser(&userList, userName);

  cur = userList.head;

  while(cur != NULL) {
    Send(cur->connfd, buf, MAX_LEN, 0);
    cur = cur->next;
  }
}

void sigintHandler(int signum) {
  shutdownCommand();
  exit(signum);
}