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

/* Print out the program usage string */
#define USAGE(void) do {\
    fprintf(stderr,\
        "\nUSAGE: [-h|-v] PORT_NUMBER MOTD\n"\
        "-h           Displays help menu & returns EXIT_SUCCESS."\
        "-v           Verbose print all incoming and outgoing protocol verbs & content."\
        "PORT_NUMBER  Port number to listen on."\
        "MOTD         Message to display to the client when they connect."\
    );\
} while(0)
