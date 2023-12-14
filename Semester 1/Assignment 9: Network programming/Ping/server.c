#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <pthread.h>


#define LOG_FILE "ping_logs.txt"
int started=0;

// Function to handle incoming ping requests in a thread
void* handlePing(void* clientSocket) {
    int client_sock = *(int*)clientSocket;
    char buffer[1024];
    char message [1024];
    ssize_t bytesReceived;
    // Implement packet loss or timeout handling mechanism
    int retransmissions = 0;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    double rtt;

    while (retransmissions < 3)
    {
        // Wait for an acknowledgment within the specified timeout
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
        // Receive data from the client

        bytesReceived = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            gettimeofday(&end, NULL);
            rtt = (double)(end.tv_usec - start.tv_usec) / 1000.0; // Calculate RTT in milliseconds
            printf("RTT: %.2f ms\n", rtt);
            break;  // Acknowledgment received, exit the retransmission loop
        } else {
            // Retransmit acknowledgment
            send(client_sock, buffer, bytesReceived, 0);
            retransmissions++;
            printf("Retransmission %d...\n", retransmissions);
        }
    }
    

    if (bytesReceived <= 0) {
        perror("Error receiving data");
    } else {
        // Log the incoming ping request, including source IP and port
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        getpeername(client_sock, (struct sockaddr*)&clientAddr, &addrLen);

        printf("Received ping from IP: %s port: %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("Client: %s\n", buffer);

        strcpy(message, "Reply from sender to, ");
        strcat(message, " ");
        strcat(message, buffer);
        bzero(buffer, 1024);
        strcpy(buffer, message);
        bzero(message, 1024);
        printf("Server: %s\n", buffer);
        send(client_sock, buffer, strlen(buffer), 0);
        
        // Log the incoming request, source IP address, port, and RTT
        struct timeval start, end;
        gettimeofday(&start, NULL);
        double rtt;

        // Implement packet loss or timeout handling mechanism
        // Wait for an acknowledgment within the specified timeout
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
        
        
        gettimeofday(&end, NULL);
        rtt = (double)(end.tv_usec - start.tv_usec) / 1000.0; // Calculate RTT in milliseconds
        printf("RTT: %.6f ms\n", rtt);
        close(client_sock);
        printf("[+]Client disconnected.\n\n");
        if(started==1)
        {
            printf("---------------------------------\n");
            printf("Listening...\n");
            started=0;
        }

        // Log the incoming request, source IP address, port, and RTT
        FILE* logFile = fopen(LOG_FILE, "a");
        if (logFile) {
            time_t current_time;
            char time_string[50];
            time(&current_time);
            strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
            fprintf(logFile, "[%s] Source IP: %s, Port: %d, RTT: %.2f ms\n", time_string, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), rtt);
            fclose(logFile);
        }
    }

    close(client_sock);
    free(clientSocket);
    return NULL;
}

int main() 
{
    //char *ip = "10.146.255.155";
    in_addr_t ip=0;
    int port = 5566;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t client_thread;
    printf("Listening at %u\n", ip);
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    //server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_addr.s_addr = ip;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    listen(server_sock, 5);
    printf("Listening...\n");

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        printf("\n[+]Client connected.\n");
        if(client_sock>=0)
        {
            started = 1;
        }
        // Create a thread to handle the incoming ping request
        int* clientSocket = (int*)malloc(sizeof(int));
        *clientSocket = client_sock;
        if (pthread_create(&client_thread, NULL, handlePing, (void*)clientSocket) != 0) {
            perror("Error creating thread");
        }
    }

    close(server_sock);
    return 0;
}
