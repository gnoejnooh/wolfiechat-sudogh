#include "Client.h"

int main(int argc, char **argv) {

  char name[MAX_NAME_LEN];
  char hostname[MAX_HOSTNAME_LEN];
  char port[MAX_PORT_LEN];

  fd_set readSet;
  fd_set readySet;

  runFlag = TRUE;
  verboseFlag = FALSE;

  signal(SIGINT, sigintHandler);

  parseOption(argc, argv, name, hostname, port);

  if((clientfd = openClientFd(hostname, port)) == -1) {
    printError("Failed to connect on server\n");
    exit(EXIT_FAILURE);
  }

  if(login(name) == TRUE) {
    
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
        
      }
    }
  } else {
    printError("Failed to login\n");
  }

  close(clientfd);

  return 0;
}

void parseOption(int argc, char **argv, char *name, char *hostname, char *port) {

  int opt;

  while((opt = getopt(argc, argv, "hcv")) != -1) {
    switch(opt) {
    case 'h':
      /* The help menu was selected */
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case 'c':
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

int login(char *name) {
  char buf[MAX_LEN];
  char motd[MAX_LEN];

  int loginSucceed = FALSE;
  
  Send(clientfd, "WOLFIE \r\n\r\n", strlen("WOLFIE \r\n\r\n"), 0, verboseFlag);
  Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

  if(strcmp(buf, "EIFLOW \r\n\r\n") == 0) {
    sprintf(buf, "IAM %s \r\n\r\n", name);

    Send(clientfd, buf, strlen(buf), 0, verboseFlag);
    Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

    if(strncmp(buf, "HI ", 3) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
      Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

      if(strncmp(buf, "MOTD ", 5) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
        sscanf(buf, "MOTD %s \r\n\r\n", motd);
        printf("%s\n", motd); 

        loginSucceed = TRUE;
      }
    } else if(strcmp(buf, "ERR 00 USER NAME TAKEN \r\n\r\n") == 0) {
      Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);
      Send(clientfd, "BYE \r\n\r\n", sizeof("BYE \r\n\r\n"), 0, verboseFlag);
    }
  }

  return loginSucceed;
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
  } else {
    printError("Command does not exist\n");
  }
}

void timeCommand() {
  char buf[MAX_LEN];
  long int duration = 0;

  Send(clientfd, "TIME \r\n\r\n", strlen("TIME \r\n\r\n"), 0, verboseFlag);
  Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

  sscanf(buf, "EMIT %ld \r\n\r\n", &duration);
  printf("Connected for %d hour(s) %d minute(s), and %d second(s)\n",
    (int)duration/3600, ((int)duration%3600)/60, ((int)duration%3600)%60);
}

void logoutCommand() {
  char buf[MAX_LEN];

  Send(clientfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0, verboseFlag);
  Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

  runFlag = FALSE;
}

void listuCommand() {
  char buf[MAX_LISTU_LEN];
  char *token;

  Send(clientfd, "LISTU \r\n\r\n", strlen("LISTU \r\n\r\n"), 0, verboseFlag);
  Recv(clientfd, buf, MAX_LISTU_LEN, 0, verboseFlag);

  if(strncmp(buf, "UTSIL ", 6) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
    token = strtok(buf, " \r\n");
    token = strtok(NULL, " \r\n");
    while(token != NULL) {
      token = strtok(NULL, " \r\n");
    }
  }
}

void printUsage() {
  fprintf(stderr, "USAGE: ./client [-hcv] NAME SERVER_IP SERVER_PORT\n");
  fprintf(stderr, "-h           Displays help menu & returns EXIT_SUCCESS.\n");
  fprintf(stderr, "-c           Request to server to create a new user\n");
  fprintf(stderr, "-v           Verbose print all incoming and outgoing protocol verbs & content.\n");
  fprintf(stderr, "NAME         This is the username to display when chatting\n");
  fprintf(stderr, "SERVER_IP    The ipaddress of the server to connect to\n");
  fprintf(stderr, "SERVER_PORT  The port to connect to\n");
}

void printError(char *msg) {
  fprintf(stderr, "\x1B[1;31mERROR: ");
  fprintf(stderr, "%s", msg);
  fprintf(stderr, "\x1B[0m");
}

void sigintHandler(int signum) {
  logoutCommand();
  exit(signum);
}