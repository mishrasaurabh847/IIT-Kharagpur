Read Me 
This repository contains a simple client-server application for sending ping requests from a client to a server. The server records the received pings in a log file named ping_logs.txt. To get started, please follow the instructions below.


Server
The server is implemented in server.c. To compile and run the server, open your terminal and navigate to the project directory. Then execute the following commands:

gcc -o server server.c
./server
The server will start listening for incoming ping requests.



Client
The client is implemented in client.c. To compile and run the client, open another terminal window and navigate to the project directory. Then execute the following commands:

gcc -o client client.c
./client
The client will prompt you to enter the following information:

IP Address: Enter the IP address of the server to which you want to send ping requests.

Port Number: Enter the port number on which the server is listening for requests.

Number of Requests: Specify how many ping requests you want to send.

Interval (in milliseconds): Set the time interval between each ping request in milliseconds.
After providing this information, the client will send the specified number of ping requests to the server at the specified intervals in milliseconds.


Log File
The server records all received ping requests in a log file named ping_logs.txt. You can inspect this log file to review the details of the ping requests and their timestamps.
