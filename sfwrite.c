#include "sfwrite.h"

void sfwrite(pthread_mutex_t *lock, FILE *stream, char *fmt, ...) {
	
	va_list vl;
	
	pthread_mutex_lock(lock);
	va_start(vl, fmt);
	vfprintf(stream, fmt, vl);
	va_end(vl);
	pthread_mutex_unlock(lock);
}