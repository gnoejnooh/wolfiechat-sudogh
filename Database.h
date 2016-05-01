#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "Constant.h"
#include "Wrapper.h"

typedef struct passwordInfo {
	char hash[SHA256_DIGEST_LENGTH*2+1];
	char salt[MAX_SALT_LEN*2+1];
} PasswordInfo;

void openDatabase(sqlite3 **db, char *accountsFile);
int isAccountExist(sqlite3 **db, char *userName);
int verifyPassword(sqlite3 **db, char *userName, char *password);
void insertAccount(sqlite3 **db, char *userName, char *hash, char *salt);
void printAllAccountsInfo(sqlite3 **db);

void getSalt(char *salt);
void getHash(char *hash, char *password, char *salt);

int isAccountExistCallback(void *data, int argc, char **argv, char **azColName);
int verifyPasswordCallback(void *data, int argc, char **argv, char **azColName);
int printAllAccountsInfoCallback(void *data, int argc, char **argv, char **azColName);

#endif