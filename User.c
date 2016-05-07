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
	pthread_rwlock_wrlock(&RW_lock);
	for(i=0; i<src->count; i++) {
		next = cur->next;
		insertUser(dst, cur->userName, cur->begin, cur->connfd);
		cur = next;
	}
	pthread_rwlock_unlock(&RW_lock);
}

void insertUser(UserList *userList, char *userName, int connfd, time_t begin) {
	User *user;

	if(isUserExist(*userList, userName) == TRUE) {
		return;
	}

	pthread_rwlock_wrlock(&RW_lock);
	user = malloc(sizeof(User));

	strcpy(user->userName, userName);
	user->connfd = connfd;
	user->begin = begin;

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
	pthread_rwlock_unlock(&RW_lock);
}

void deleteUser(UserList *userList, char *userName) {
	User *cur = userList->head;
	User *next = NULL;

	int i;
	pthread_rwlock_wrlock(&RW_lock);

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
			break;
		}
		cur = next;
	}
	pthread_rwlock_unlock(&RW_lock);
}

void printAllUserInfo(UserList userList) {
	User *cur = userList.head;
	User *next = NULL;

	int i;
	pthread_mutex_t Q_lock = PTHREAD_MUTEX_INITIALIZER;
	sfwrite(&Q_lock, stdout, "There is %d user(s) exist.\n", userList.count);

	pthread_rwlock_rdlock(&RW_lock);
	for(i=0; i<userList.count; i++) {
		next = cur->next;
		sfwrite(&Q_lock, stdout, "USERNAME: %-20s\n", cur->userName);
		cur = next;
	}
	pthread_rwlock_unlock(&RW_lock);
}

int isUserExist(UserList userList, char *userName) {
	User *cur = userList.head;
	User *next = NULL;

	int i;
	int userExist = FALSE;
	pthread_rwlock_rdlock(&RW_lock);
	for(i=0; i<userList.count; i++) {
		next = cur->next;
		if(strcmp(cur->userName, userName) == 0) {
			userExist = TRUE;
		}
		cur = next;
	}
	pthread_rwlock_unlock(&RW_lock);
	return userExist;
}

User * findUser(UserList userList, char *userName) {
	User *cur = userList.head;
	User *next = NULL;
	User *user = NULL;

	int i;
	pthread_rwlock_rdlock(&RW_lock);
	for(i=0; i<userList.count; i++) {
		next = cur->next;
		if(strcmp(cur->userName, userName) == 0) {
			user = cur;
			break;
		}
		cur = next;
	}
	pthread_rwlock_unlock(&RW_lock);
	return user;
}

void matchUser(UserList userList, char *userName, int connfd) {
	User *cur = userList.head;
	User *next = NULL;

	int i;
	pthread_rwlock_rdlock(&RW_lock);
	for(i=0; i<userList.count; i++) {
		next = cur->next;
		if(cur->connfd == connfd) {
			strcpy(userName, cur->userName);
		}
		cur = next;
	}
	pthread_rwlock_unlock(&RW_lock);
}

time_t matchBegin(UserList userList, int connfd) {
	User *cur = userList.head;
	User *next = NULL;
	time_t begin;

	int i;
	pthread_rwlock_rdlock(&RW_lock);
	for(i=0; i<userList.count; i++) {
		next = cur->next;
		if(cur->connfd == connfd) {
			begin = cur->begin;
			break;
		}
		cur = next;
	}
	pthread_rwlock_unlock(&RW_lock);
	return begin;
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