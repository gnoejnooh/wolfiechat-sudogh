#include "server.h"

int main(int argc, char *argv[]) {
	int opt;
	int portno;
	char* motd = NULL;
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
}