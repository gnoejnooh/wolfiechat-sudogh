#include "User.h"

void initializeUserList(UserList *userList) {
	userList->head = NULL;
	userList->tail = NULL;
	userList->count = 0;
}

void copyUserList(UserList *dst, UserList *src) {
	User *cur = src->head;
	User *next = NULL;

	int i;

	for(i=0; i<src->count; i++) {
		next = cur->next;
		insertUser(dst, cur->userName);
		cur = next;
	}
}

void insertUser(UserList *userList, char *userName) {
	User *user;

	if(isUserExist(userList, userName) == TRUE) {
		return;
	}

	user = malloc(sizeof(User));

	strcpy(user->userName, userName);

	if(userList->count == 0) {
		user->prev = NULL;
		user->next = NULL;
		userList->head = user;
		userList->tail = user;
	} else {
		user->prev = userList->tail;
		user->next = NULL;
		userList->tail->next = user;
		userList->tail = user;
	}

	(userList->count)++;
}

void deleteUser(UserList *userList, char *userName) {
	User *cur = userList->head;
	User *next = NULL;

	int i;

	puts("TEST");

	for(i=0; i<userList->count; i++) {
		next = cur->next;
		if(strcmp(cur->userName, userName) == 0) {
			User *prev = cur->prev;
			if(prev != NULL) {
				prev->next = next;
			}
			
			if(next != NULL) {
				next->prev = prev;
			}

			if(cur == userList->head) {
				userList->head = next;
			}

			if(cur == userList->tail) {
				userList->tail = prev;
			}

			(userList->count)--;
			free(cur);
			return;
		}
		cur = next;
	}
}

void printAllUserInfo(UserList *userList) {
	User *cur = userList->head;
	User *next = NULL;

	int i;

	for(i=0; i<userList->count; i++) {
		next = cur->next;
		printf("USER[%d]: %s\n", i+1, cur->userName);
		cur = next;
	}
}

int isUserExist(UserList *userList, char *userName) {
	User *cur = userList->head;
	User *next = NULL;

	int i;

	for(i=0; i<userList->count; i++) {
		next = cur->next;
		if(strcmp(cur->userName, userName) == 0) {
			return TRUE;
		}
		cur = next;
	}

	return FALSE;
}

void freeUserList(UserList *userList) {
	User *cur = userList->head;
	User *next = NULL;

	int i;

	for(i=0; i<userList->count; i++) {
		next = cur->next;
		free(cur);
		cur = next;
	}
}