#ifndef __USER_H__
#define __USER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "Constant.h"
#include "sfwrite.h"

extern pthread_rwlock_t RW_lock;

typedef struct user {
	char userName[MAX_NAME_LEN];
	int connfd;

	struct user *prev;
	struct user *next;
} User;

typedef struct userList {
	User *head;
	User *tail;

	int count;
} UserList;

void initializeUserList(UserList *userList);
void insertUser(UserList *userList, char *userName, int connfd);
void deleteUser(UserList *userList, char *userName);
void printAllUserInfo(UserList userList);
int isUserExist(UserList userList, char *userName);
User * findUser(UserList userList, char *userName);
void matchUser(UserList userList, char *userName, int connfd);
void freeUserList(UserList *userList);

#endif