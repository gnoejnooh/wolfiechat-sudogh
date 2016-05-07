#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#include <pthread.h>

#define TRUE				1
#define	FALSE				0

#define STDIN				0
#define STDOUT				1
#define STDERR				2

#define MAX_LEN				1024
#define MAX_PORT_LEN		8
#define MAX_NAME_LEN		16
#define MAX_DUMMY_LEN		128
#define MAX_ARGS_LEN		32
#define MAX_PASSWORD_LEN	256
#define MAX_SALT_LEN		64
#define MAX_FILE_LEN		256
#define MAX_HOSTNAME_LEN	256
#define MAX_FD_LEN			8
#define MAX_EVENT_LEN		16
#define MAX_TIME_LEN		64
#define MAX_LISTU_LEN		65536
#define MAX_BUF_LEN			65536

#define AUDIT_FILE_NAME		"audit.log"

#define BACKLOG				128

pthread_rwlock_t RW_lock;
pthread_mutex_t Q_lock;

#endif