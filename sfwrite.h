#ifndef __SFWRITE_H__
#define __SFWRITE_H__

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

void sfwrite(pthread_mutex_t *lock, FILE *stream, char *fmt, ...);

#endif