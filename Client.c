#include "Client.h"

int main(int argc, char **argv) {
  
  int clientfd;

  char name[MAX_NAME_LEN];
  char hostname[MAX_HOSTNAME_LEN];
  char port[MAX_PORT_LEN];

  fd_set readSet;
  fd_set readySet;

  int runFlag = TRUE;
  int verboseFlag = FALSE;

  parseOption(argc, argv, name, hostname, port, &verboseFlag);

  if((clientfd = openClientFd(hostname, port)) == -1) {
    printError("Failed to connect on server\n");
    exit(EXIT_FAILURE);
  }

  if(login(clientfd, name, verboseFlag) == TRUE) {
    
    FD_ZERO(&readSet);
    FD_SET(STDIN, &readSet);
    FD_SET(clientfd, &readSet);

    while(runFlag) {
      readySet  = readSet;
      select(clientfd+1, &readySet, NULL, NULL, NULL);

      if(FD_ISSET(STDIN, &readySet)) {
        if((runFlag = executeCommand(clientfd, verboseFlag)) == FALSE) {
          break;
        }
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

void parseOption(int argc, char **argv, char *name, char *hostname, char *port, int *verboseFlag) {

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
      *verboseFlag = TRUE;
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
    strcpy(name, argv[1]);
    strcpy(hostname, argv[2]);
    strcpy(port, argv[3]);
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

  int clientfd;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
  getaddrinfo(hostname, port, &hints, &list);

  for(cur=list; cur!=NULL; cur=cur->ai_next) {
    if((clientfd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol)) < 0) {
      continue;
    }

    if(connect(clientfd, cur->ai_addr, cur->ai_addrlen) != -1) {
      break;
    }

    close(clientfd);
  }

  freeaddrinfo(list);
  if(cur == NULL) {
    return -1;
  } else {
    return clientfd;
  }
}

int login(int clientfd, char *name, int verboseFlag) {
  char buf[MAX_LEN];
  char motd[MAX_LEN];

  int loginSucceed = FALSE;
  
  Send(clientfd, "WOLFIE \r\n\r\n", strlen("WOLFIE \r\n\r\n"), 0, verboseFlag);
  memset(buf, 0, MAX_LEN);
  Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

  if(strcmp(buf, "EIFLOW \r\n\r\n") == 0) {
    memset(buf, 0, MAX_LEN);
    strcat(buf, "IAM ");
    strcat(buf, name);
    strcat(buf, " \r\n\r\n");

    Send(clientfd, buf, strlen(buf), 0, verboseFlag);
    memset(buf, 0, MAX_LEN);
    Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

    if(strncmp(buf, "HI ", 3) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
      memset(buf, 0, MAX_LEN);
      Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

      if(strncmp(buf, "MOTD ", 5) == 0 && strcmp(&buf[strlen(buf)-5], " \r\n\r\n") == 0) {
        strcpy(motd, &buf[5]);
        motd[strlen(motd)-5] = '\0';
        printf("%s\n", motd); 

        loginSucceed = TRUE;
      }
    } else if(strcmp(buf, "ERR 00 USER NAME TAKEN \r\n\r\n") == 0) {
      memset(buf, 0, MAX_LEN);
      Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

      Send(clientfd, "BYE \r\n\r\n", sizeof("BYE \r\n\r\n"), 0, verboseFlag);
    }
  }

  return loginSucceed;
}

int executeCommand(int clientfd, int verboseFlag) {

  int runFlag = TRUE;
  
  char buf[MAX_LEN];
  fgets(buf, MAX_LEN, stdin);

  if(strcmp(buf, "/time\n") == 0) {
    printError("Not implemented yet\n");
  } else if(strcmp(buf, "/help\n") == 0) {
    printUsage();
  } else if(strcmp(buf, "/logout\n") == 0) {
    logout(clientfd, verboseFlag);
    runFlag = FALSE;
  } else if(strcmp(buf, "/listu\n") == 0) {
    printError("Not implemented yet\n");
  } else {
    printError("Command does not exist\n");
  }

  return runFlag;
}

void logout(int clientfd, int verboseFlag) {
  char buf[MAX_LEN];

  Send(clientfd, "BYE \r\n\r\n", strlen("BYE \r\n\r\n"), 0, verboseFlag);
  Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);
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