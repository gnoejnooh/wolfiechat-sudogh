#include "Database.h"

void openDatabase(sqlite3 **db, char *accountsFile) {
  int rc;
  char *sql = NULL;
  char *errMsg = NULL;

  if(strlen(accountsFile) != 0) {
    rc = sqlite3_open(accountsFile, db);
  } else {
    rc = sqlite3_open("data.db", db);
  }

  if(rc != SQLITE_OK) {
    printError("Can't open database\n");
    sqlite3_close(*db);
    exit(EXIT_FAILURE);
  }

  sql = "CREATE TABLE IF NOT EXISTS USERS("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name VARCHAR(16) UNIQUE NOT NULL,"
        "password VARCHAR(255) NOT NULL);";
        
  rc = sqlite3_exec(*db, sql, NULL, NULL, &errMsg);

  if(rc != SQLITE_OK) {
    printError("Can't create table\n");
    sqlite3_close(*db);
    exit(EXIT_FAILURE);
  }
}

int isAccountExist(sqlite3 **db, char *userName) {
	int rc;
  char *sql = NULL;
  char *errMsg;

	int accountExist = FALSE;

  sql = sqlite3_mprintf("SELECT COUNT(*) FROM USERS WHERE name = %Q;", userName);
  
  rc = sqlite3_exec(*db, sql, isAccountExistCallback, &accountExist, &errMsg);
  sqlite3_free(sql);

  if(rc != SQLITE_OK) {
    printError("Can't execute query\n");
    sqlite3_close(*db);
    exit(EXIT_FAILURE);
  }

  return accountExist;
}

void insertAccount(sqlite3 **db, char *userName, char *password) {
  int rc;
  char *sql = NULL;
  char *errMsg;

  if(isAccountExist(db, userName) == TRUE) {
  	printError("User Already Exist\n");
  	return;
  }

  sql = sqlite3_mprintf("INSERT INTO USERS VALUES(NULL, %Q, %Q);", userName, password);
  
  rc = sqlite3_exec(*db, sql, NULL, NULL, &errMsg);
  sqlite3_free(sql);

  if(rc != SQLITE_OK) {
    printError("Can't insert entry\n");
    return;
  }
}

void printAllAccountsInfo(sqlite3 **db) {
	int rc;
  char *sql = NULL;
  char *errMsg;

  sql = "SELECT * FROM USERS";
  
  rc = sqlite3_exec(*db, sql, printAllAccountsInfoCallback, NULL, &errMsg);

  if(rc != SQLITE_OK) {
    printError("Can't insert entry\n");
    return;
  }
}

int isAccountExistCallback(void *data, int argc, char **argv, char **azColName) {
	if(strcmp(argv[0], "0") == 0) {
		*(int *)data = FALSE;
	} else {
		*(int *)data = TRUE;
	}

	return 0;
}

int printAllAccountsInfoCallback(void *data, int argc, char **argv, char **azColName) {
	int i;
	for(i=0; i<argc; i++) {
		printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}