# Ping and FTP Implementation in C

## Table of Contents
1. [Overview](#overview)
    - 1.1 [Part I: Ping Implementation](#part-i-ping-implementation)
        - 1.1.1 [Server Implementation](#server-implementation)
        - 1.1.2 [Client Implementation](#client-implementation)
    - 1.2 [Part II: FTP Implementation](#part-ii-ftp-implementation)
        - 1.2.1 [Server Implementation](#server-implementation-1)
        - 1.2.2 [Client Implementation](#client-implementation-1)
2. [Usage](#usage)
    - 2.1 [Ping](#ping)
    - 2.2 [FTP](#ftp)
3. [Compiling and Running](#compiling-and-running)
    - 3.1 [Ping Compilation](#ping-compilation)
    - 3.2 [FTP Compilation](#ftp-compilation)
4. [Important Note](#important-notes)

## Overview
This project involves the implementation of a simplified version of the ping command and a File Transfer Protocol (FTP) server and client in the C programming language. The ping implementation measures the round-trip time (RTT) between a client and a server, while the FTP implementation supports basic file transfer operations.

### Part I: Ping Implementation
#### Server Implementation
- The server handles multiple ping requests concurrently.
- Responds to each ping request with an acknowledgment containing the same payload.
- Implements a mechanism to handle packet loss or timeouts (e.g., retransmission of acknowledgment).
- Logs each incoming ping request, including source IP address, port, and RTT in a file.
- Protects the logfile to prevent overwriting by multiple ping handler processes.

#### Client Implementation
- Sends ping requests to the server and measures the round-trip time.
- Capable of sending multiple ping requests to the server.
- Implements a mechanism to send ping requests.
- Displays the acknowledgment from the server along with the RTT.
- Allows users to specify server IP, number of ping requests, interval between requests, and the port from the command line.

### Part II: FTP Implementation
#### Server Implementation
- The FTP server handles file transfer requests from multiple clients concurrently.
- Supports commands:
  - `put <file_name>`: Uploads a file from the client to the server in the current directory.
  - `get <file_name>`: Downloads a file from the server to the client in the current directory.
  - `close`: Closes the connection with the client.
  - `cd <directory_name>`: Changes the current directory on the server.
  - `ls`: Lists all contents of the current directory on the server available to the client.
- Binds to a specific port to listen for incoming FTP commands.
- Handles multiple concurrent connections.

#### Client Implementation
- The FTP client allows users to connect to the FTP server and perform file transfer operations.
- Supports commands:
  - `put <file_name>`: Uploads a file to the server.
  - `get <file_name>`: Downloads a file from the server.
  - `close`: Closes the connection with the server.
  - `cd <directory_name>`: Changes the current directory on the server.
  - `ls`: Lists all contents of the current directory on the server.
- Implements a mechanism to establish a connection with the server (using a known port).
- Provides a simple user interface prompt (`ftp_client>`) for entering FTP commands.
- Displays appropriate messages for the success or failure of file operations.

## Usage
### Ping
1. Compile the server and client programs separately.
   ```bash
   gcc server.c
   gcc client.c
   ```
2. Run the server executable with the desired port.
   ```bash
   ./a.out
   ```
3. Run the client executable with the server IP, port, number of ping requests, and interval.
   ```bash
   ./a.out
   ```

### FTP
1. Compile the server and client programs separately.
   ```bash
   gcc server.c
   gcc client.c
   ```
2. Run the server executable with the desired port.
   ```bash
   ./a.out
   ```
3. Run the client executable with the server IP, port, number of ping requests, and interval.
   ```bash
   ./a.out
   ```
4. Use the `ftp_client>` prompt to enter FTP commands.

## Important Note
The entire assignment is explained in "Assignment - 9 (Networks).pdf".
