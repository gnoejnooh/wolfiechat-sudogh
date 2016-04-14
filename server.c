#include "server.h"

int main(int argc, char *argv[]) {
	int opt;
	int sockfd;
	int cli_sockfd;
	int portno;
	socklen_t clilen;
	char* motd = NULL;
	char buffer[MAX_INPUT];
	struct sockaddr_in serv_addr, cli_addr;

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
            fprintf(stderr, "Missing PORT_NUMBER and MOTD.\n");
        } else if((argc - optind) == 1) {
            fprintf(stderr, "Missing MOTD.\n");
        } else {
            fprintf(stderr, "Too many arguments provided.\n");
        }
        USAGE();
        exit(EXIT_FAILURE);
    }
    /* Create new socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
	bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	/* Listen on the socket for connection */
	listen(sockfd, 5);
	//TODO: implement i/o multiplexing to watch multiple client
	/* Block process until a client connects to the server- */
	clilen = sizeof(cli_addr);
	cli_sockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
	/* Write welcome message to client */
	write(cli_sockfd, motd, strlen(motd));
	/* Initialize buffer and read from it */
	bzero(buffer, MAX_INPUT);
	read(cli_sockfd, buffer, MAX_INPUT);
	printf("%s\n", buffer);

	return 0;
}