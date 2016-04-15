#include "server.h"

void error(const char* msg) {
	fprintf(stderr, "\x1B[1;31mError: %s\n", msg);
	exit(1);
}

void sig_handler(int signal) {
	if(signal == SIGINT) {
		printf(" Handling SIGINT...\n");
		exit(0);
	}
}

int main(int argc, char *argv[]) {
	int run = TRUE;
	int opt;
	int sockfd;
	int cli_sockfd;
	int portno;
	fd_set input;
	socklen_t clilen;
	char* motd = NULL;
	char buffer[MAX_INPUT];
	struct sockaddr_in serv_addr, cli_addr;

	struct sigaction sa;
	sa.sa_handler = &sig_handler;
	sa.sa_flags = SA_RESTART;

	if(sigaction(SIGINT, &sa, NULL) == -1)
		error("cannot handle SIGINT\n");

	while((opt = getopt(argc, argv, "hv:")) != -1) {
        switch(opt) {
            case 'h':
                /* The help menu was selected */
                USAGE();
                exit(EXIT_SUCCESS);
                break;
            case 'v':
                break;
            case '?':
                /* Let this case fall down to default;
                 * handled during bad option.
                 */
            default:
                /* A bad option was provided. */
                USAGE();
                exit(EXIT_FAILURE);
                break;
        }
    }
    /* Get position arguments */
    if(optind < argc && (argc - optind) == 2) {
        portno = atoi(argv[optind++]);
        motd = argv[optind++];
    } else {
        if((argc - optind) <= 0) {
            error("Missing PORT_NUMBER and MOTD.");
        } else if((argc - optind) == 1) {
            error("Missing MOTD.");
        } else {
            error("Too many arguments provided.");
        }
        USAGE();
        exit(EXIT_FAILURE);
    }
    /* Create new socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("unable to open a socket");
    if(signal(SIGINT, sig_handler) == SIG_ERR) error("signal");
    /* Set buffer to zero */
    bzero((char*) &serv_addr, sizeof(serv_addr));
    /*
	 * struct sockaddr_in {
	 *	short		sin_family;
	 *	u_short		sin_port;
	 *	struct		in_addr sin_addr;
	 *	char		sin_zero[8];
	 * };
	 */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	/* Bind socket to an address */
	if(bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		error("unable to bind");
	/* Listen on the socket for connection */
	listen(sockfd, 5);
	/* Get client address length */
	clilen = sizeof(cli_addr);
	/* Print before loop */
	printf("Currently listening on port %d\n", portno);
	printf("server> ");
	fflush(stdout);
	
	/* Implement I/O multiplexing for input */
	while(run) {
		/* Initialize fdset */
		FD_ZERO(&input);
		FD_SET(sockfd, &input);
		FD_SET(fileno(stdin), &input);
		/* Use select() to determine which fd is used */
		if(select(sockfd+1, &input, 0, 0, 0) < 0)
			error("something went wrong with select");
		/* Handle user command */
		if(FD_ISSET(fileno(stdin), &input)) {
			bzero(buffer, MAX_INPUT);
			fgets(buffer, MAX_INPUT, stdin);
			if(!strcmp(buffer, "/users\n")) {
				printf("List of users should be printed\n");
			} else if(!strcmp(buffer, "/help\n")) {
				USAGE();
			} else if(!strcmp(buffer, "/shutdown\n")) {
				run = FALSE;
			} else {
				fprintf(stderr, "\x1B[1;31mError: command does not exist\x1B[0m\n");
			}
			/*
			switch(buffer[0]) {
				case '/':
					
				default:
					fprintf(stderr, "\x1B[1;31mError: command does not exist\x1B[0m\n");
			}*/
			if(run == TRUE) {
				printf("server> ");
				fflush(stdout);
			}
		}
		/* Accept connection and spawn login thread */
		if(FD_ISSET(sockfd, &input)) {
			/* Block process until a client connects to the server- */
			cli_sockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
			if(cli_sockfd < 0)
				error("unable to accept connection");
			/* Write message of the day to client */
			if(write(cli_sockfd, motd, strlen(motd)) < 0)
				error("unable to write message of the day");
			/* Initialize buffer and echo client's msg */
			bzero(buffer, MAX_INPUT);
			if(read(cli_sockfd, buffer, MAX_INPUT) < 0)
				error("unable to read from client socket");
			if(write(cli_sockfd, buffer, MAX_INPUT) < 0)
				error("unable to write to client socket");
			//TODO: Spawn a new thread to verify user name
			close(cli_sockfd);
		}
	}

	printf("Shutting down...\n");
	close(sockfd);

	return 0;
}