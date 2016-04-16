#include "client.h"

int main(int argc, char* argv[]) {

  int rc;
  int listenfd;
  char portno[MAX_LEN];
  char addr[MAX_LEN];
  char name[MAX_LEN];
  char buffer[MAX_LEN];
  char buf_in[MAX_LEN];
  struct addrinfo hints;
  struct addrinfo *servinfo;
  fd_set input;
  //struct sockaddr_in serv_addr;
  //struct hostent *server;
  /*
      struct  hostent {
      char    *h_name;        //official name of host
      char    **h_aliases;    // alias list
      int     h_addrtype;     // host address type
      int     h_length;       // length of address
      char    **h_addr_list;  // list of addresses from name server
      #define h_addr  h_addr_list[0]  // address, for backward compatiblity
      };
  */
  parseOption(argc, argv, name, addr, portno);

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if(listenfd < 0) {
    printError("cannot open socket");
    exit(-1);
  }
  
  /* Modify addrinfo hint struct */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(addr, portno, &hints, &servinfo);
  /*
  struct addrinfo {
    int     ai_flags;          // AI_PASSIVE, AI_CANONNAME, ...
    int     ai_family;         // AF_xxx
    int     ai_socktype;       // SOCK_xxx
    int     ai_protocol;       // 0 (auto) or IPPROTO_TCP, IPPROTO_UDP 

    socklen_t  ai_addrlen;     // length of ai_addr
    char   *ai_canonname;      // canonical name for nodename
    struct sockaddr  *ai_addr; // binary address
    struct addrinfo  *ai_next; // next structure in linked list
  };
  */
  /* Establish a connection */
  rc = connect(listenfd, servinfo->ai_addr, servinfo->ai_addrlen);
  if(rc < 0) {
    printError("cannot connect");
    exit(1);
  }

  wolfieProtocol(listenfd);
  sprintf(buffer, "IAM %s\r\n\r\n", name);
  rc = write(listenfd, buffer, sizeof(buffer));
  if(rc < 0) {
    printError("unable to write login protocol");
  }
  bzero(buffer, sizeof(buffer));
  rc = read(listenfd, buffer, sizeof(buffer));
  if(rc < 0) {
    printError("unable to read login protocol");
  }
  strip_crnl(buffer);
  printf("%s\n", buffer);
  
  while(TRUE) {
    FD_ZERO(&input);
    FD_SET(listenfd, &input);
    FD_SET(fileno(stdin), &input);

    printf("%s> ", name);
    fflush(stdout);

    if(select(listenfd+1, &input, 0, 0, 0) < 0) {
      printError("something went wrong with select");
    }

    if(FD_ISSET(fileno(stdin), &input)) {
      bzero(buffer, MAX_LEN);
      fgets(buffer, MAX_LEN, stdin);
      if(!strcmp(buffer, "/time\n")) {
        rc = write(listenfd, "TIME\r\n\r\n", 8);
        if(rc < 0) {
          printError("unable to write TIME");
        }
      } else if(!strcmp(buffer, "/help\n")) {
        printUsage();
      } else if(!strcmp(buffer, "/logout\n")) {
        rc = write(listenfd, "BYE\r\n\r\n", 8);
        if(rc < 0) {
          printError("unable to write BYE");
        }
      } else if(!strcmp(buffer, "/listu\n")) {
        printf("Should send request to server\n");
      } else {
        fprintf(stderr, "\x1B[1;31mError: command does not exist\x1B[0m\n");
      }
    }
    else {
    //if(FD_ISSET(listenfd, &input)) {
      rc = read(listenfd, buf_in, sizeof(buf_in));
      strip_crnl(buf_in);

      if(!strlen(buf_in)) {
        continue;
      }
      char* verb;
      //char* arg;
      verb = strtok(buf_in, " ");

      if(!strcmp(verb, "BYE")) {
        break;
      } else if(!strcmp(verb, "MOTD")) {
        printf("%s\n", strtok(NULL, " "));
      } else if(!strcmp(verb, "EMIT")) {
        unsigned long t = atoi(strtok(NULL, " "));
        printf("Connected for %d hour(s) %d minute(s), and %d second(s)\n", (int)t/3600, (int)(t%3600)/60, (int)(t%3600)%60);
      }
    }
  }

  /* Free the linked list */
  freeaddrinfo(servinfo);
  printf("Logged out successfully...\n");
  return 0;
}

void strip_crnl(char* str) {
  while(*str != '\0') {
    if(*str == '\r' || *str == '\n') {
      *str = '\0';
    }
    str++;
  }
}

void wolfieProtocol(int listenfd) {
  int rc;
  char buffer[10];
  rc = write(listenfd, "WOLFIE\r\n\r\n", 10);
  if(rc < 0) {
    printError("unable to write WOLFIE protocol");
  }
  bzero(buffer, sizeof(buffer));
  rc = read(listenfd, buffer, sizeof(buffer));
  if(rc < 0) {
    printError("unable to read EIFLOW protocol");
  }
  if(strcmp(buffer, "EIFLOW\r\n\r\n")) {
    printError("protocol does not match");
  }
}

void printError(const char *msg) {
  fprintf(stderr, "\x1B[1;31mError: %s\n", msg);
  exit(1);
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

void parseOption(int argc, char **argv, char *name, char *addr, char *portno) {
  
  int opt;

  while((opt = getopt(argc, argv, "hcv:")) != -1) {
    switch(opt) {
    case 'h':
      /* The help menu was selected */
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case 'c':
      break;
    case 'v':
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
  if(argc == 4) {
    strcpy(name, argv[1]);
    strcpy(addr, argv[2]);
    strcpy(portno, argv[3]);
  } else {
    if(argc == 0) {
        fprintf(stderr, "\x1B[1;31mMissing required arguments.\n");
    } else if(argc == 1) {
        fprintf(stderr, "\x1B[1;31mMissing SERVER_IP and SERVER_PORT.\n");
    } else if(argc == 2) {
        fprintf(stderr, "\x1B[1;31mMissing SERVER_PORT.\n");
    } else {
        fprintf(stderr, "\x1B[1;31mToo many arguments provided.\n");
    }
    printUsage();
    exit(EXIT_FAILURE);
  }
}