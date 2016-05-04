#include "Client.h"

int main(int argc, char **argv) {

  char hostname[MAX_HOSTNAME_LEN];
  char port[MAX_PORT_LEN];

  fd_set readSet;
  fd_set readySet;

  runFlag = TRUE;
  verboseFlag = FALSE;
  createUserFlag = FALSE;

  signal(SIGINT, sigintHandler);

  parseOption(argc, argv, hostname, port);

  initializeUserList(&userList);

  if((clientfd = openClientFd(hostname, port)) == -1) {
    printError("Failed to connect on server\n");
    exit(EXIT_FAILURE);
  }

  if(login() == TRUE) {
    
    FD_ZERO(&readSet);
    FD_SET(STDIN, &readSet);
    FD_SET(clientfd, &readSet);

    while(runFlag) {
      readySet  = readSet;
      select(clientfd+1, &readySet, NULL, NULL, NULL);

      if(FD_ISSET(STDIN, &readySet)) {
        executeCommand();
      }

      if(FD_ISSET(clientfd, &readySet)) {
        receiveMessage();
      }
    }
  } else {
    printError("Failed to login\n");
  }

  close(clientfd);

  return 0;
}

void parseOption(int argc, char **argv, char *hostname, char *port) {

  int opt;

  while((opt = getopt(argc, argv, "hcv")) != -1) {
    switch(opt) {
    case 'h':
      /* The help menu was selected */
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case 'c':
      createUserFlag = TRUE;
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

  /* Get position arguments */
  if(optind < argc && (argc - optind) == 3) {
    strcpy(name, argv[optind++]);
    strcpy(hostname, argv[optind++]);
    strcpy(port, argv[optind++]);
  } else {
    if((argc - optind) < 3) {
      printError("Missing arguments\n");
    } else {
      printError("Too many arguments\n");
    }
    printUsage();
    exit(EXIT_FAILURE);
  }
}

int openClientFd(char *hostname, char *port) {
  struct addrinfo hints;
  struct addrinfo *list;
  struct addrinfo *cur;

  int fd;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
  getaddrinfo(hostname, port, &hints, &list);

  for(cur=list; cur!=NULL; cur=cur->ai_next) {
    if((fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol)) < 0) {
      continue;
    }

    if(connect(fd, cur->ai_addr, cur->ai_addrlen) != -1) {
      break;
    }

    close(fd);
  }

  freeaddrinfo(list);
  if(cur == NULL) {
    return -1;
  } else {
    return fd;
  }
}

int login() {
  char buf[MAX_LEN];
  char motd[MAX_LEN];

  int loginSucceed = FALSE;
  
  Send(clientfd, "WOLFIE \r\n\r\n", strlen("WOLFIE \r\n\r\n"), 0);
  Recv(clientfd, buf, MAX_LEN, 0);

  if(strcmp(buf, "EIFLOW \r\n\r\n") == 0) {
    if(authenticateUser() == TRUE) {
      if(promptPassword() == TRUE) {
        Recv(clientfd, buf, MAX_LEN, 0);

        if(strncmp(buf, "HI ", 3) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
          Recv(clientfd, buf, MAX_LEN, 0);

          if(strncmp(buf, "MOTD ", 5) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
            sscanf(buf, "MOTD %s \r\n\r\n", motd);
            printf("%s\n", motd); 

            loginSucceed = TRUE;
          }
        }
      }
    }
  }

  return loginSucceed;
}

int authenticateUser() {
  char buf[MAX_LEN];

  if(createUserFlag) {
    sprintf(buf, "IAMNEW %s \r\n\r\n", name);
  } else {
    sprintf(buf, "IAM %s \r\n\r\n", name);
  }

  Send(clientfd, buf, strlen(buf), 0);
  Recv(clientfd, buf, MAX_LEN, 0);

  if(strncmp(buf, "HINEW ", 6) == 0 || strncmp(buf, "AUTH ", 5) == 0) {
    if(strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
      return TRUE;
    }
  } else if(strncmp(buf, "ERR 00", 5) == 0) {
    printError("User name taken\n");
    Recv(clientfd, buf, MAX_LEN, 0);
  } else if(strncmp(buf, "ERR 01", 5) == 0) {
    printError("User not available\n");
    Recv(clientfd, buf, MAX_LEN, 0);
  }

  return FALSE;
}

int promptPassword() {
  char *pass;
  char buf[MAX_LEN];
  
  pass = getpass("Enter Password: ");
  
  if(createUserFlag) {
    sprintf(buf, "NEWPASS %s \r\n\r\n", pass);
  } else {
    sprintf(buf, "PASS %s \r\n\r\n", pass);
  }

  Send(clientfd, buf, strlen(buf), 0);
  Recv(clientfd, buf, MAX_LEN, 0);

  if(strncmp(buf, "SSAPWEN ", 8) == 0 || strncmp(buf, "SSAP ", 5) == 0) {
    if(strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
      return TRUE;
    }
  } else {
    Recv(clientfd, buf, MAX_LEN, 0);
  }
  return FALSE;
}

void executeCommand() {
  
  char buf[MAX_LEN];
  
  fgets(buf, MAX_LEN, stdin);

  if(strcmp(buf, "/time\n") == 0) {
    timeCommand();
  } else if(strcmp(buf, "/help\n") == 0) {
    printUsage();
  } else if(strcmp(buf, "/logout\n") == 0) {
    logoutCommand();
  } else if(strcmp(buf, "/listu\n") == 0) {
    listuCommand();
  } else if(strncmp(buf, "/chat", 5) == 0) {
  	chatCommand(buf);
  } else {
    printError("Command does not exist\n");
  }
}

void receiveMessage() {
  char buf[MAX_LEN];

  Recv(clientfd, buf, MAX_LEN, 0);

  if(strncmp(buf, "MSG", 3) == 0) {
    receiveChatMessage(buf);
  } else if(strncmp(buf, "ERR 01", 5) == 0) {
    printError("User not avaiable\n");
  }
}

void receiveChatMessage(char *line) {
  char to[MAX_NAME_LEN];
  char from[MAX_NAME_LEN];
  char msg[MAX_LEN];

  sscanf(line, "MSG %s %s %1024[^\n]", to, from, msg);

  processChatMessage(to, from, msg);
}

void processChatMessage(char *to, char *from, char *msg) {

  int socketfd[2];
  int pid;
  char buf[MAX_LEN];

  char *cmd[MAX_NAME_LEN] =
    {"/usr/bin/xterm", "-geometry", "45x35", "-background", "gray15",
    "-fa", "Monospace", "-fs", "12"};
  char fd[MAX_FD_LEN];

  char userName[MAX_NAME_LEN];

  CommunicationThreadParam *communicationThreadParam;
  pthread_t tid;

  if(strcmp(name, to) == 0) {
    strcpy(userName, from);
  } else if(strcmp(name, from) == 0) {
    strcpy(userName, to);
  }

  if(isUserExist(userList, userName) == FALSE) {
    socketpair(AF_UNIX, SOCK_STREAM, 0, socketfd);
    insertUser(&userList, userName, socketfd[0]);

    sprintf(buf, "WOLFIE CHAT with %s \n", userName);
    sprintf(fd, "%d", socketfd[1]);
    cmd[9] = "-T";
    cmd[10] = buf;
    cmd[11] = "-e";
    cmd[12] = "./chat";
    cmd[13] = fd;
    cmd[14] = (void *)NULL;

    if((pid = fork()) == 0) {

      close(socketfd[0]);
      execv(cmd[0], cmd);
      exit(EXIT_SUCCESS);
    } else {
      close(socketfd[1]);

      if(strcmp(name, to) == 0) {
        sprintf(buf, "\e[93m[ %s ]\e[0m %s", userName, msg);
      } else {
        sprintf(buf, "\e[94m[ %s ]\e[0m %s", name, msg);
      }

      Send(socketfd[0], buf, strlen(buf), 0);
    }

    communicationThreadParam = malloc(sizeof(CommunicationThreadParam));
    communicationThreadParam->connfd = socketfd[0];
    strcpy(communicationThreadParam->userName, userName);
    pthread_create(&tid, NULL, communicationThread, communicationThreadParam);
    
  } else {
    User *user = findUser(userList, userName);
    int connfd = user->connfd;

    if(strcmp(name, to) == 0) {
      sprintf(buf, "\e[93m[ %s ]\e[0m %s", userName, msg);
    } else {
      sprintf(buf, "\e[94m[ %s ]\e[0m %s", name, msg);
    }

    Send(connfd, buf, strlen(buf), 0);
  }
}

void timeCommand() {
  char buf[MAX_LEN];
  long int duration = 0;

  Send(clientfd, "TIME \r\n\r\n", strlen("TIME \r\n\r\n"), 0);
  Recv(clientfd, buf, MAX_LEN, 0);

  sscanf(buf, "EMIT %ld \r\n\r\n", &duration);
  printf("Connected for %d hour(s) %d minute(s), and %d second(s)\n",
    (int)duration/3600, ((int)duration%3600)/60, ((int)duration%3600)%60);
}

void logoutCommand() {
  char buf[MAX_LEN];

  Send(clientfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0);
  Recv(clientfd, buf, MAX_LEN, 0);

  runFlag = FALSE;
}

void listuCommand() {
  char buf[MAX_LISTU_LEN];
  char *token;

  Send(clientfd, "LISTU \r\n\r\n", strlen("LISTU \r\n\r\n"), 0);
  Recv(clientfd, buf, MAX_LISTU_LEN, 0);

  if(strncmp(buf, "UTSIL ", 6) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
    token = strtok(buf, " \r\n");
    token = strtok(NULL, " \r\n");
    while(token != NULL) {
      printf("USERNAME: %s\n", token);
      token = strtok(NULL, " \r\n");
    }
  }
}

void chatCommand(char *line) {
	char buf[MAX_LEN];
  char to[MAX_NAME_LEN];
  char msg[MAX_LEN];

  memset(buf, 0, MAX_LEN);
  memset(to, 0, MAX_NAME_LEN);
  memset(msg, 0, MAX_LEN);

	if(verifyChatCommand(line, to, msg) == FALSE) {
    printError("Invalid format\n");
    return;
  }

	sprintf(buf, "MSG %s %s %s \r\n\r\n", to, name, msg);
  Send(clientfd, buf, strlen(buf), 0);
}

int verifyChatCommand(char *line, char *to, char *msg) {
  int verified = FALSE;

  line[strlen(line)-1] = '\0';
  sscanf(line, "/chat %s %1024[^\n]", to, msg);

  if(strlen(to) != 0 && strlen(msg) != 0) {
    verified = TRUE;
  }

  return verified;
}

void printUsage() {
  fprintf(stderr, "USAGE: ./client [-hcv] NAME SERVER_IP SERVER_PORT\n");
  fprintf(stderr, "-h                 Displays help menu & returns EXIT_SUCCESS.\n");
  fprintf(stderr, "-c                 Request to server to create a new user\n");
  fprintf(stderr, "-v                 Verbose print all incoming and outgoing protocol verbs & content.\n");
  fprintf(stderr, "NAME               This is the username to display when chatting\n");
  fprintf(stderr, "SERVER_IP          The ipaddress of the server to connect to\n");
  fprintf(stderr, "SERVER_PORT        The port to connect to\n");
  fprintf(stderr, "\nClient Commands\n");
  fprintf(stderr, "/time              Display the time client has been connected to the server\n");
  fprintf(stderr, "/help              Display usage statement\n");
  fprintf(stderr, "/logout            Logout from the server\n");
  fprintf(stderr, "/listu             Display a list of active user in current server\n");
  fprintf(stderr, "/chat <to> <msg>   Send message to indicated user\n");
}

void * communicationThread(void *argv) {
  CommunicationThreadParam *param = (CommunicationThreadParam *)argv;
  int connfd = param->connfd;
  char userName[MAX_NAME_LEN];

  char buf[MAX_LEN];
  char msg[MAX_LEN];

  strcpy(userName, param->userName);

  pthread_detach(pthread_self());
  free(argv);

  while(TRUE) {
    RecvChat(connfd, msg, MAX_LEN, 0);

    if(strcmp(msg, "/close") == 0) {
      deleteUser(&userList, userName);
      break;
    }

    if(strlen(msg) != 0) {
      sprintf(buf, "MSG %s %s %s \r\n\r\n", userName, name, msg);
      Send(clientfd, buf, strlen(buf), 0);
    }
  }

  close(connfd);

  return NULL;
}

void sigintHandler(int signum) {
  logoutCommand();
  exit(signum);
}