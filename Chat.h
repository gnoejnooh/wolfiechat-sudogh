#ifndef __CHAT_H__
#define __CHAT_H__

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Constant.h"
#include "Wrapper.h"

int chatfd;

void sigintHandler(int signum);
void sighupHandler(int signum);

#endif