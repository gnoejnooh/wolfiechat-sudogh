#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "Constant.h"

void Send(int socket, const void *buffer, size_t length, int flags, int verboseFlag);
void Recv(int socket, void *buffer, size_t length, int flags, int verboseFlag);

#endif