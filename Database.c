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
        "name VARCHAR(32) UNIQUE NOT NULL,"
        "hash VARCHAR(512) NOT NULL,"
        "salt VARCHAR(128) NOT NULL)";
        
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
  pthread_rwlock_rdlock(&RW_lock);  
  rc = sqlite3_exec(*db, sql, isAccountExistCallback, &accountExist, &errMsg);
  pthread_rwlock_unlock(&RW_lock);
  sqlite3_free(sql);

  if(rc != SQLITE_OK) {
    printError("Can't execute query\n");
    sqlite3_close(*db);
    exit(EXIT_FAILURE);
  }

  return accountExist;
}

int verifyPassword(sqlite3 **db, char *userName, char *password) {
	int rc;
	char *sql = NULL;
	char *errMsg;

	char hash[SHA256_DIGEST_LENGTH*2+1];
	PasswordInfo *passwordInfo = malloc(sizeof(PasswordInfo));

	int verified = FALSE;

	memset(passwordInfo, 0, sizeof(PasswordInfo));

	sql = sqlite3_mprintf("SELECT * FROM USERS WHERE name = %Q;", userName);

  pthread_rwlock_rdlock(&RW_lock);
	rc = sqlite3_exec(*db, sql, verifyPasswordCallback, passwordInfo, &errMsg);
  pthread_rwlock_unlock(&RW_lock);

	getHash(hash, password, passwordInfo->salt);

	if(strcmp(hash, passwordInfo->hash) == 0) {
		verified = TRUE;
	}

  sqlite3_free(sql);

  if(rc != SQLITE_OK) {
    printError("Can't execute query\n");
    sqlite3_close(*db);
    exit(EXIT_FAILURE);
  }

  free(passwordInfo);
  return verified;
}

void insertAccount(sqlite3 **db, char *userName, char *hash, char *salt) {
  int rc;
  char *sql = NULL;
  char *errMsg;

  sql = sqlite3_mprintf("INSERT INTO USERS VALUES(NULL, %Q, %Q, %Q);", userName, hash, salt);
  
  pthread_rwlock_rdlock(&RW_lock);
  rc = sqlite3_exec(*db, sql, NULL, NULL, &errMsg);
  pthread_rwlock_unlock(&RW_lock);
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

  pthread_rwlock_rdlock(&RW_lock);
  rc = sqlite3_exec(*db, sql, printAllAccountsInfoCallback, NULL, &errMsg);
  pthread_rwlock_unlock(&RW_lock);

  if(rc != SQLITE_OK) {
    printError("Can't insert entry\n");
    return;
  }
}

void getSalt(char *salt) {
	unsigned char digest[MAX_SALT_LEN];
	int i;

	RAND_bytes(digest, MAX_SALT_LEN);

	for(i=0; i<MAX_SALT_LEN; i++) {
    sprintf(&salt[i*2], "%02x", (unsigned int)digest[i]);
  }
}

void getHash(char *hash, char *password, char *salt) {
	SHA256_CTX c;
	unsigned char digest[SHA256_DIGEST_LENGTH];
	int i;

	for(i=0; i<strlen(salt); i++) {
  	password[i] = password[i] & salt[i];
  }

	SHA256_Init(&c);
	SHA256_Update(&c, password, strlen(password));
	SHA256_Final(digest, &c);

  for (i=0; i<SHA256_DIGEST_LENGTH; i++) {
    sprintf(&hash[i*2], "%02x", (unsigned int)digest[i]);
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

int verifyPasswordCallback(void *data, int argc, char **argv, char **azColName) {
	PasswordInfo *passwordInfo = (PasswordInfo *)data;
	
	strcpy(passwordInfo->hash, argv[2]);
	strcpy(passwordInfo->salt, argv[3]);

	return 0;
}

int printAllAccountsInfoCallback(void *data, int argc, char **argv, char **azColName) {
	printf("USERNAME: %s\nPASSWORD: %-20s\n\n", argv[1], argv[2]);
	return 0;
}