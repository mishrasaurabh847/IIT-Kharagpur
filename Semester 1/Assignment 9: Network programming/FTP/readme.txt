READ ME

Introduction
This repository contains two files, `server.c` and `client.c`, which implement a simple FTP server and client. 

_____________________Server Implementation_____________________

Running the Server
To run the FTP server, follow these steps:

1. Compile the server code using the following command:
   
   gcc -o server server.c

2. Start the server by executing the compiled binary:

   ./server


3. The server will prompt you to provide the port number to bind to. Enter the desired port number.

_____________________Client Implementation_____________________

Running the Client
To run the FTP client, follow these steps:

1. Compile the client code using the following command:

   gcc -o client client.c

2. Start the client by executing the compiled binary:

   ./client


3. The client will ask you to provide the following information in the specified order:
   
	- Server's IP address IMPORTANT This must be the ACTUAL IP ADDRESS
   
	- Port number
   	
	- FTP commands (e.g., ls, cd, get, put, close)



_____________________EXTRA EXPLANATION_____________________

### Server working
The FTP server has the following properties:

1. The server can bind to a specific port to listen for incoming FTP commands.
2. The server is able to handle multiple concurrent connections.


### Client Commands
The FTP client supports the following commands:

1. `put <file_name>`: Upload a file from the client to the server in the current directory.
2. `get <file_name>`: Download a file from the server to the client in the current directory.
3. `close`: Close the connection with the server.
4. `cd <directory_name>`: Change the current directory on the server.
5. `ls`: List all the contents of the current directory on the server available to the client.