#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "Constant.h"
#include "Wrapper.h"

void openDatabase(sqlite3 **db, char *accountsFile);
int isAccountExist(sqlite3 **db, char *userName);
int verifyPassword(sqlite3 **db, char *userName, char *password);
void insertAccount(sqlite3 **db, char *userName, char *password);
void printAllAccountsInfo(sqlite3 **db);

int isAccountExistCallback(void *data, int argc, char **argv, char **azColName);
int verifyPasswordCallback(void *data, int argc, char **argv, char **azColName);
int printAllAccountsInfoCallback(void *data, int argc, char **argv, char **azColName);

#endif