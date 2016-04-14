#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#define MAX_INPUT	1024

#define USAGE(void) do {\
    fprintf(stderr,\
        "\nUSAGE: [-hcv] NAME SERVER_IP SERVER_PORT\n"\
        "-h           Displays help menu & returns EXIT_SUCCESS.\n"\
        "-c           Request to server to create a new user\n"\
        "-v           Verbose print all incoming and outgoing protocol verbs & content.\n"\
        "NAME         This is the username to display when chatting\n"\
        "SERVER_IP	  The ipaddress of the server to connect to\n"\
        "SERVER_PORT  The port to connect to"\
    );\
} while(0)
