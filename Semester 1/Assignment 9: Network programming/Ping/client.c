#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

int main()
{
 
    char ip[100] = "127.0.0.1";
    int port = 5566;
    int nr, I;
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    printf("Enter the address:\n");
    fgets(ip, 100, stdin);
    printf("Enter the port number:\n");
    scanf("%d", &port);
    printf("Enter the number of requests:\n");
    scanf("%d", &nr);
    printf("Enter the Interval in ms:\n");
    scanf("%d", &I);
    for(int i =0; i<nr; i++)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0){
            perror("[-]Socket error");
            exit(1);
        }
        printf("[+]TCP server socket created.\n");
        
        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = port;
        addr.sin_addr.s_addr = inet_addr(ip);
        
        connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        struct timeval start, end;
        gettimeofday(&start, NULL);
        printf("Connected to the server.\n");
        
        bzero(buffer, 1024);
        sprintf(buffer, "Ping %d from CLIENT to IP: %s Port: %d", i+1, ip, port);
        puts(buffer);
        printf("Client: %s\n", buffer);
        send(sock, buffer, strlen(buffer), 0);
        sleep(1);
        bzero(buffer, 1024);
        recv(sock, buffer, sizeof(buffer), 0);
        printf("Server: %s\n", buffer);
        
        
        // ... Process the client request ...
        
        // Get the current time again
        gettimeofday(&end, NULL);
        
        // Calculate the time taken
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        double elapsed = seconds + microseconds / 1e6;
        
        printf("Time taken: %.6f seconds\n", elapsed);
        close(sock);
        printf("Disconnected from the server.\n");
        printf("---------------------------------\n");
        if (i < nr - 1)
        {
            
            sleep(I/1000);
        }
    }
  return 0;
}
