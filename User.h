#ifndef __USER_H__
#define __USER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constant.h"

typedef struct user {
	char userName[MAX_NAME_LEN];

	struct user *prev;
	struct user *next;
} User;

typedef struct userList {
	User *head;
	User *tail;

	int count;
} UserList;

void initializeUserList(UserList *userList);
void insertUser(UserList *userList, char *userName);
void deleteUser(UserList *userList, char *userName);
void printAllUserInfo(UserList *userList);
int isUserExist(UserList *userList, char *userName);
void freeUserList(UserList *userList);

#endif