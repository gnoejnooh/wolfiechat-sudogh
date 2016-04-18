#include "Client.h"

int main(int argc, char* argv[]) {

  int clientfd;
  int verboseFlag = 0;
  int userFlag = 0;

  char name[MAX_NAME_LEN];
  char hostname[MAX_HOSTNAME_LEN];
  char port[MAX_PORT_LEN];
  char buf[MAX_LEN];

  fd_set readSet;
  fd_set readySet;

  parseOption(argc, argv, name, hostname, port, &verboseFlag, &userFlag);
  
  if((clientfd = openClientFd(hostname, port)) == -1) {
    printError("Failed to connect on server\n");
    exit(EXIT_FAILURE);
  }

  if(login(clientfd, name, verboseFlag, userFlag) == TRUE) {
    FD_ZERO(&readSet);
    FD_SET(clientfd, &readSet);
    FD_SET(fileno(stdin), &readSet);

    while(TRUE) {
      readySet = readSet;
      select(clientfd+1, &readySet, 0, 0, 0);
      if(FD_ISSET(STDIN, &readySet)) {
        executeCommand(clientfd, verboseFlag);
      } else {
        Recv(clientfd, buf, sizeof(buf), 0, verboseFlag);

        if(!strlen(buf)) {
          continue;
        }
        char* verb;
        verb = strtok(buf, " ");
        if(!strcmp(verb, "BYE")) {
          break;
        } else if(!strcmp(verb, "EMIT")) {
          unsigned long t = atoi(strtok(NULL, " "));
          printf("Connected for %d hour(s) %d minute(s), and %d second(s)\n", (int)t/3600, (int)(t%3600)/60, (int)(t%3600)%60);
        }
      }
    }
  } else {
    printError("Failed to login\n");
  }

  close(clientfd);

  return 0;
}

int openClientFd(char *hostname, char *port) {
  struct addrinfo hints;
  struct addrinfo *list;
  struct addrinfo *cur;

  int clientfd;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
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

int login(int clientfd, char *name, int verboseFlag, int userFlag) {
  char buf[MAX_LEN];
  char motd[MAX_LEN];

  int loginSucceed = FALSE;

  Send(clientfd, "WOLFIE \r\n\r\n", strlen("WOLFIE \r\n\r\n"), 0, verboseFlag);
  memset(buf, 0, MAX_LEN);
  Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);
  if(strcmp(buf, "EIFLOW \r\n\r\n") == 0) {
    memset(buf, 0, MAX_LEN);
    sprintf(buf, "IAM %s \r\n\r\n", name);
    Send(clientfd, buf, strlen(buf), 0, verboseFlag);

    memset(buf, 0, MAX_LEN);
    Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

    if(!strncmp(buf, "HI ", 3) && !strcmp(&buf[strlen(buf)-5], " \r\n\r\n")) {
      memset(buf, 0, MAX_LEN);
      Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

      if(!strncmp(buf, "MOTD ", 5) && strcmp(&buf[strlen(buf)-5], " \r\n\r\n")) {
        strcpy(motd, &buf[5]);
        motd[strlen(motd)-5] = '\0';
        printf("%s\n", motd);

        loginSucceed = TRUE;
      }
    } else if(!strcmp(buf, "ERR 00 USER NAME TAKEN \r\n\r\n")) {
      memset(buf, 0, MAX_LEN);
      Recv(clientfd, buf, MAX_LEN, 0, verboseFlag);

      Send(clientfd, "BYE \r\n\r\n", sizeof("BYE \r\n\r\n"), 0, verboseFlag);
    }
  }

  return loginSucceed;
}

void executeCommand(int clientfd, int verboseFlag) {
  char buf[MAX_LEN];
  fgets(buf, MAX_LEN, stdin);

  if(!strcmp(buf, "/time\n")) {
    Send(clientfd, "TIME \r\n\r\n", sizeof("TIME \r\n\r\n"), 0, verboseFlag);
  } else if(!strcmp(buf, "/help\n")) {
    printUsage();
  } else if(!strcmp(buf, "/logout\n")) {
    Send(clientfd, "BYE \r\n\r\n", sizeof("BYE \r\n\r\n"), 0, verboseFlag);
  } else if(!strcmp(buf, "/listu\n")) {
    Send(clientfd, "LISTU \r\n\r\n", sizeof("LISTU \r\n\r\n"), 0, verboseFlag);
  } else {
    printError("Command does not exist\n");
  }
}

void parseOption(int argc, char **argv, char *name, char *hostname, char *port, int *verboseFlag, int *userFlag) {
  
  int opt;

  while((opt = getopt(argc, argv, "hcv")) != -1) {
    switch(opt) {
    case 'h':
      /* The help menu was selected */
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case 'c':
      *userFlag = 1;
      break;
    case 'v':
      *verboseFlag = 1;
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

void printError(char *msg) {
  fprintf(stderr, "\x1B[1;31mERROR: ");
  fprintf(stderr, "%s", msg);
  fprintf(stderr, "\x1B[0m");
}

void printUsage() {
  fprintf(stderr, "\nUSAGE: [-hcv] NAME SERVER_IP SERVER_PORT\n");
  fprintf(stderr, "-h           Displays help menu & returns EXIT_SUCCESS.\n");
  fprintf(stderr, "-c           Request to server to create a new user\n");
  fprintf(stderr, "-v           Verbose print all incoming and outgoing protocol verbs & content.\n");
  fprintf(stderr, "NAME         This is the username to display when chatting\n");
  fprintf(stderr, "SERVER_IP    The ipaddress of the server to connect to\n");
  fprintf(stderr, "SERVER_PORT  The port to connect to\n");
}