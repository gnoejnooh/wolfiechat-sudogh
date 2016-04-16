#include "client.h"

int main(int argc, char* argv[]) {

  int sockfd;
  char portno[MAX_LEN];
  char addr[MAX_LEN];
  char name[MAX_LEN];
  char buffer[MAX_LEN];

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
      
  struct addrinfo hints;
  struct addrinfo *servinfo;

  parseOption(argc, argv, name, addr, portno);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
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
  //server = gethostbyname(servinfo->ai_canonname);
  //bzero((char*) &serv_addr, sizeof(serv_addr));
  //serv_addr.sin_family = AF_INET;
  //bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
  //serv_addr.sin_port = htons(atoi(portno));
  /* Establish a connection */
  //connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  /* Read welcome message from the server */
  read(sockfd, buffer, MAX_LEN);
  printf("%s\n", buffer);
  /* Send message to the server */
  printf("%s: ", name);
  bzero(buffer, MAX_LEN);
  fgets(buffer, MAX_LEN, stdin);
  write(sockfd, buffer, strlen(buffer));
  /* Free the linked list */
  freeaddrinfo(servinfo);
  return 0;
}

void printUsage() {
  fprintf(stderr, "\nUSAGE: [-hcv] NAME SERVER_IP SERVER_PORT\n");
  fprintf(stderr, "-h           Displays help menu & returns EXIT_SUCCESS.\n");
  fprintf(stderr, "-c           Request to server to create a new user\n");
  fprintf(stderr, "-v           Verbose print all incoming and outgoing protocol verbs & content.\n");
  fprintf(stderr, "NAME         This is the username to display when chatting\n");
  fprintf(stderr, "SERVER_IP    The ipaddress of the server to connect to\n");
  fprintf(stderr, "SERVER_PORT  The port to connect to");
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