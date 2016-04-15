/* fprintf() */
#include <stdio.h>
/* exit() */
#include <stdlib.h>
/* getopt() */
#include <unistd.h>
/* struct sockaddr_in */
#include <netinet/in.h>
/* bzero() */
#include <string.h>
/* signal() */
#include <signal.h>

#define MAX_INPUT   1024

#define TRUE    1
#define FALSE   0

/* Print out the program usage string */
#define USAGE(void) do {\
    fprintf(stderr,\
        "USAGE: [-h|-v] PORT_NUMBER MOTD\n"\
        "-h           Displays help menu & returns EXIT_SUCCESS.\n"\
        "-v           Verbose print all incoming and outgoing protocol verbs & content.\n"\
        "PORT_NUMBER  Port number to listen on.\n"\
        "MOTD         Message to display to the client when they connect.\n"\
    );\
} while(0)
