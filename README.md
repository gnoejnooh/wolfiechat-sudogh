# WolfieChat: Server & Client Model Chat Program (a chat service much like Google's Hangouts)
### Launch with following command
#### Server
```
./server [-hv] [-t THREAD_COUNT] PORT_NUMBER MOTD [ACCOUNTS_FILE]
-h Displays help menu & returns EXIT_SUCCESS.
-t THREAD_COUNT The number of threads used for the login queue.
-v Verbose print all incoming and outgoing protocol verbs & content.
PORT_NUMBER Port number to listen on.
MOTD Message to display to the client when they connect.
ACCOUNTS_FILE File containing username and password data to be loaded upon execution.
```
#### Client
```
./client [-hcv] [-a FILE] NAME SERVER_IP SERVER_PORT
-a FILE Path to the audit log file.
-h Displays this help menu, and returns EXIT_SUCCESS.
-c Requests to server to create a new user
-v Verbose print all incoming and outgoing protocol verbs & content.
NAME The username to display when chatting.
SERVER_IP The ipaddress of the server to connect to.
SERVER_PORT The port to connect to.
```
#### Chat
```
./chat UNIX_SOCKET_FD AUDIT_FILE_FD
UNIX_SOCKET_FD The Unix Domain file descriptor number.
AUDIT_FILE_FD The file descriptor of the audit.log created in the client program.
```
