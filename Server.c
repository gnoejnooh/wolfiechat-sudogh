#include "Server.h"

int main(int argc, char **argv) {
  
  int listenfd;
  int epollfd;
  int *connfd = NULL;

  char port[MAX_PORT_LEN];
  char motd[MAX_LEN];

  struct sockaddr_in *connAddr = NULL;
  socklen_t connLen;

  struct epoll_event event;

  pthread_t tid;

  UserList *userList = NULL; 

  LoginThreadParam *loginThreadParam = NULL;

  int verboseFlag = FALSE;
  int runFlag = TRUE;

  parseOption(argc, argv, port, motd, &verboseFlag);

  connAddr = malloc(sizeof(struct sockaddr_in));
  userList = malloc(sizeof(UserList));
  loginThreadParam = malloc(sizeof(LoginThreadParam));

  initializeUserList(userList);

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
      if((runFlag = executeCommand(userList)) == FALSE) {
        break;
      }
    }

    if(event.data.fd == listenfd) {
      connLen = sizeof(struct sockaddr_in);
      connfd = malloc(sizeof(int));
    
      if((*connfd = accept(listenfd, (struct sockaddr *)connAddr, &connLen)) != -1) {
        loginThreadParam->tid = tid;
        loginThreadParam->connfd = connfd;
        loginThreadParam->motd = motd;
        loginThreadParam->userList = userList;
        loginThreadParam->verboseFlag = verboseFlag;
        pthread_create(&tid, NULL, loginThread, loginThreadParam);
      } else {
        free(connfd);
      }
    }
  }

  close(listenfd);
  free(connAddr);
  freeUserList(userList);
  free(userList);
  free(loginThreadParam);
  return EXIT_SUCCESS;
}

void parseOption(int argc, char **argv, char *port, char *motd, int *verboseFlag) {

  int opt;

  while((opt = getopt(argc, argv, "hv")) != -1) {
    switch(opt) {
    case 'h':
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      *verboseFlag = TRUE;
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

int executeCommand(UserList *userList) {

  int runFlag = TRUE;
  
  char buf[MAX_LEN];
  fgets(buf, MAX_LEN, stdin);

  if(strcmp(buf, "/users\n") == 0) {
    printAllUserInfo(userList);
  } else if(strcmp(buf, "/help\n") == 0) {
    printUsage();
  } else if(strcmp(buf, "/shutdown\n") == 0) {
    runFlag = FALSE;
  } else {
    printError("Command doesn't exist\n");
  }

  return runFlag;
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
  pthread_t tid = param->tid;
  int connfd = *(param->connfd);
  char *motd = param->motd;
  UserList *userList = param->userList;
  int verboseFlag = param->verboseFlag;

  char buf[MAX_LEN];
  char userName[MAX_NAME_LEN];

  pthread_detach(pthread_self());
  free(argv);

  memset(buf, 0, MAX_LEN);
  Recv(connfd, buf, MAX_LEN, 0, verboseFlag);

  if(strcmp(buf, "WOLFIE \r\n\r\n") == 0) {
    Send(connfd, "EIFLOW \r\n\r\n", strlen("EIFLOW \r\n\r\n"), 0, verboseFlag);
    memset(buf, 0, MAX_LEN);
    Recv(connfd, buf, MAX_LEN, 0, verboseFlag);

    if(strncmp(buf, "IAM ", 4) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
      strcpy(userName, &buf[4]);
      userName[strlen(userName)-5] = '\0';
      
      if(!isUserExist(userList, userName)) {

        CommunicationThreadParam *communicationThreadParam = malloc(sizeof(CommunicationThreadParam));
        
        memset(buf, 0, MAX_LEN);
        strcat(buf, "HI ");
        strcat(buf, userName);
        strcat(buf, " \r\n\r\n");
        Send(connfd, buf, strlen(buf), 0, verboseFlag);
        insertUser(userList, userName);
        
        memset(buf, 0, MAX_LEN);
        strcat(buf, "MOTD ");
        strcat(buf, motd);
        strcat(buf, " \r\n\r\n");
        Send(connfd, buf, strlen(buf), 0, verboseFlag);

        communicationThreadParam->connfd = &connfd;
        communicationThreadParam->userList = userList;
        communicationThreadParam->verboseFlag = verboseFlag;
        pthread_create(&tid, NULL, communicationThread, communicationThreadParam);

      } else {

        Send(connfd, "ERR 00 USER NAME TAKEN \r\n\r\n", strlen("ERR 00 USER NAME TAKEN \r\n\r\n"), 0, verboseFlag);
        Send(connfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0, verboseFlag);

        memset(buf, 0, MAX_LEN);
        Recv(connfd, buf, MAX_LEN, 0, verboseFlag);

        printf("%s\n", buf);
      }
    }
  }
  
  close(connfd);
  return NULL;
}

void * communicationThread(void *argv) {

  CommunicationThreadParam *param = (CommunicationThreadParam *)argv;
  int connfd = *(param->connfd);
  //UserList *userList = param->userList;
  int verboseFlag = param->verboseFlag;

  time_t begin = time(NULL);

  char buf[MAX_LEN];

  pthread_detach(pthread_self());
  free(argv);

  memset(buf, 0, MAX_LEN);
  Recv(connfd, buf, MAX_LEN, 0, verboseFlag);

  if(strcmp(buf, "TIME \r\n\r\n") == 0) {
    time_t current = time(NULL);
    memset(buf, 0, MAX_LEN);
    sprintf(buf, "EMIT %ld\r\n\r\n", begin-current);
    Send(connfd, buf, sizeof(buf), 0, verboseFlag);
  }

  close(connfd);

  return NULL;
}