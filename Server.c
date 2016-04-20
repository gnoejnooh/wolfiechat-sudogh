#include "Server.h"

int main(int argc, char **argv) {
  
  int listenfd;
  int epollfd;
  int *connfd = NULL;

  char port[MAX_PORT_LEN];
  char motd[MAX_LEN];

  struct sockaddr_in *connAddr = malloc(sizeof(struct sockaddr_in));
  socklen_t connLen;

  struct epoll_event event;

  pthread_t tid; 

  LoginThreadParam *loginThreadParam = NULL;

  signal(SIGINT, sigintHandler);

  verboseFlag = FALSE;
  runFlag = TRUE;

  parseOption(argc, argv, port, motd);

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

  close(listenfd);
  free(connAddr);
  freeUserList(&userList);
  return EXIT_SUCCESS;
}

void parseOption(int argc, char **argv, char *port, char *motd) {

  int opt;

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
  } else {
    if(argc - optind < 2) {
      printError("Missing arguments\n");
    } else {
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
  } else if(strcmp(buf, "/shutdown\n") == 0) {
    shutdownCommand();
  } else {
    printError("Command doesn't exist\n");
  }
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

void printError(char *msg) {
  fprintf(stderr, "\x1B[1;31mERROR: ");
  fprintf(stderr, "%s", msg);
  fprintf(stderr, "\x1B[0m");
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
    Recv(connfd, buf, MAX_LEN, 0);

    if(strncmp(buf, "IAM ", 4) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
      strcpy(userName, &buf[4]);
      userName[strlen(userName)-5] = '\0';
      
      if(!isUserExist(userList, userName)) {

        CommunicationThreadParam *communicationThreadParam = malloc(sizeof(CommunicationThreadParam));
        
        memset(buf, 0, MAX_LEN);
        sprintf(buf, "HI %s \r\n\r\n", userName);
        Send(connfd, buf, strlen(buf), 0);
        insertUser(&userList, userName, connfd);
        
        memset(buf, 0, MAX_LEN);
        sprintf(buf, "MOTD %s \r\n\r\n", motd);
        Send(connfd, buf, strlen(buf), 0);

        communicationThreadParam->connfd = &connfd;
        strcpy(communicationThreadParam->userName, userName);
        pthread_create(&tid, NULL, communicationThread, communicationThreadParam);

      } else {

        Send(connfd, "ERR 00 USER NAME TAKEN \r\n\r\n", strlen("ERR 00 USER NAME TAKEN \r\n\r\n"), 0);
        Send(connfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0);

        Recv(connfd, buf, MAX_LEN, 0);

        printf("%s\n", buf);
        close(connfd);
      }
    }
  }

  return NULL;
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
      timeCommand(connfd, begin);
    } else if(strcmp(buf, "LISTU \r\n\r\n") == 0) {
      listuCommand(connfd);
    } else if(strcmp(buf, "BYE \r\n\r\n") == 0) {
      Send(connfd, "BYE \r\n\r\n", sizeof("BYE \r\n\r\n"), 0);
      printf("%s\n", userName);
      deleteUser(&userList, userName);
      break;
    }
  }

  close(connfd);

  return NULL;
}

void timeCommand(int connfd, time_t begin) {
  char buf[MAX_LEN];
  time_t current = time(NULL);

  sprintf(buf, "EMIT %ld \r\n\r\n", current-begin);
  Send(connfd, buf, sizeof(buf), 0);
}

void listuCommand(int connfd) {
  char buf[MAX_LISTU_LEN];

  User *cur = userList.head;
  memset(buf, 0, MAX_LISTU_LEN);

  sprintf(buf, "UTSIL");

  while(cur != NULL) {
    sprintf(buf+strlen(buf), " %s \r\n", cur->userName);
    cur = cur->next;
  }

  sprintf(buf+strlen(buf), "\r\n");
  Send(connfd, buf, sizeof(buf), 0);
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

void sigintHandler(int signum) {
  shutdownCommand();
  exit(signum);
}