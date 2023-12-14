#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <arpa/inet.h>

int started = -1;
#define MAX_CLIENTS 10

struct ClientInfo 
{
    int socket;
    struct sockaddr_in address;
};

void *handle_client(void *arg) 
{
    struct ClientInfo *client_info = (struct ClientInfo *)arg;
    int client_socket = client_info->socket;
    struct sockaddr_in client_address = client_info->address;
    int put=0;
    char command[100];
    char filename[100];
    char buf[100];
    int size;

    ssize_t received_bytes;
    while (1) 
    {
        if (put == 0)
            received_bytes = recv(client_socket, command, sizeof(command), 0);

        if (received_bytes <= 0) 
        {
            // If recv returns 0 or a negative value, the client has closed the connection
            printf("Client has closed the connection.\n");
            printf("[+]Client disconnected.\n\n");
            printf("---------------------------------\n");
            printf("Listening...\n");
            break; // Exit the loop and close the client connection
        }
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        getpeername(client_socket, (struct sockaddr*)&clientAddr, &addrLen);
        
        if(put==0)
            printf("Received request from IP: %s port: %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        
        if (strncmp(command, "put", 3)==0)
            put=1;
        
        if(put==0)
            printf("Client: %s\n", command);
        
        if (strcmp(command, "close") == 0)
        {
            printf("closing connection");
            // Close the connection with the client
            close(client_socket);
            break;
        }
        if (strcmp(command, "ls") == 0)
        {
            DIR *dir;
            struct dirent *entry;
            char file_list[4096] = "";

            dir = opendir(".");
            if (dir != NULL) 
            {
                while ((entry = readdir(dir))) 
                {
                    strcat(file_list, entry->d_name);
                    strcat(file_list, "\n");
                }
                closedir(dir);
            }

            size = strlen(file_list);
            send(client_socket, &size, sizeof(int), 0);
            send(client_socket, file_list, size, 0);
        } 
        else if (strncmp(command, "get", 3) == 0) 
        {
            sscanf(command, "get %s", filename);
            FILE *file = fopen(filename, "rb");

            if (file != NULL)
            {
                fseek(file, 0, SEEK_END);
                size = ftell(file);
                //printf("SIZE %d\n", size);
                fseek(file, 0, SEEK_SET);
                
                char *file_data = (char *)malloc(size);
                char *line_buffer = NULL;
                size_t line_len = 0;

                while (getline(&line_buffer, &line_len, file) != -1)
                {
                    strcat(file_data, line_buffer);
                }
                //printf("FILE DATA: %s", file_data);
                send(client_socket, file_data, size, 0);

                fclose(file);
                free(file_data);
                free(line_buffer); // Don't forget to free the line_buffer
                const char* data = "END_OF_DATA";
                printf("sending...");
                sleep(7);
                send(client_socket, data, strlen(data), 0);

                printf("File uploaded successfully.\n");
            }
            else
            {
                size = 0;
                send(client_socket, &size, sizeof(int), 0);
                const char* data = "END_OF_DATA";
                send(client_socket, data, strlen(data), 0);
                printf("File not found on server.\n");
            }
        }
        else if (strncmp(command, "put", 3) == 0) 
        {
            sscanf(command, "put %s", filename);
            char *lastPeriod = strrchr(filename, '.');
            
            if (lastPeriod != NULL)
            {
                // Calculate the position to insert "_ftp"
                size_t position = lastPeriod - filename;
                // Insert "_ftp" at the calculated position
                memmove(filename + position + 11, lastPeriod, strlen(lastPeriod) + 1);
                strncpy(filename + position, "_fromClient", 11);
            }
            char file_data[10000];
            printf("Receiving file: %s\n", filename);
            FILE* file = fopen(filename, "wb");
            if (file == NULL)
            {
                perror("Error opening file");
            }
            else
            {
                while (1)
                {
                    bzero(file_data, 10000);
                    int bytesReceived = recv(client_socket, file_data, sizeof(file_data), 0);
                    if (strstr(file_data, "END_OF_DATA") != NULL)
                    {
                        fclose(file);
                        printf("File received!\n");
                        break; // Exit the loop when the marker is found
                    }
                    printf("%s", file_data);
                    printf("saving...");
                    //printf("DATA IS %d", bytesReceived);
                    if (bytesReceived <= 0)
                    {
                        printf("DATA is %d\n", bytesReceived);
                        fclose(file);
                        printf("File received: %s\n", filename);
                        break; // Error or end of file
                    }
                    fwrite(file_data, 1, bytesReceived, file);
                }
                fclose(file);
                printf("File received: %s\n", filename);
            }
            put=0;
        }
        else if (strncmp(command, "cd", 2) == 0)
        {
            // Change directory on the server
            recv(client_socket, filename, sizeof(filename), 0);
            char *spacePos = strchr(filename, ' ');
            int cd_result;
            if (spacePos != NULL)
            {
                // Calculate the position of the space character
                size_t position = spacePos - filename;
                
                // Extract the directory name after the space character
                char *directoryName = spacePos + 1;
                
                // Remove any leading or trailing spaces
                directoryName = strtok(directoryName, " \n\r\t");
                
                // Use the extracted directory name with chdir to change the working directory
                cd_result = chdir(directoryName);
                printf("Changed working directory to: %s\n", directoryName);
            }
            else 
            {
                printf("Invalid command in the filename.\n");
            }
            //int cd_result = chdir(filename);
            send(client_socket, &cd_result, sizeof(int), 0);
            system("pwd>temp.txt");
            int i = 0;
            FILE *f = fopen("temp.txt", "r");
            buf[0]='\0';
            while (!feof(f))
                buf[i++] = fgetc(f);
            buf[i - 1] = '\0';
            fclose(f);
            send(client_socket, buf, 100, 0);
            buf[0] = '\0';
            
        }
        else if (strcmp(command, "close") == 0)
        {
            printf("closing connection");
            // Close the connection with the client
            close(client_socket);
            break;
        }
        else if (strcmp(command, "quit") == 0) 
        {
            close(client_socket);
            break;
        }
        else if (strcmp(command, "pwd") == 0)
        {
            system("pwd>temp.txt");
            int i = 0;
            FILE*f = fopen("temp.txt","r");
            while(!feof(f))
                buf[i++] = fgetc(f);
            buf[i-1] = '\0';
            fclose(f);
            send(client_socket, buf, 100, 0);
        }
        if(put==0)
        {
            printf("[+]Client disconnected.\n\n");
            printf("---------------------------------\n");
            printf("Listening...\n");
        }
    }

    free(client_info);
    pthread_exit(NULL);
}

int main() 
{
    int server_socket;
    struct sockaddr_in server;
    int port;
    printf("Enter the Port Number:");
    scanf("%d", &port);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
    {
        perror("Socket creation failed");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");


    server.sin_family = AF_INET;  // Set the address family to IPv4
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int bind_result = bind(server_socket, (struct sockaddr *)&server, sizeof(server));
    if (bind_result == -1) 
    {
        perror("Binding error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);


    int listen_result = listen(server_socket, MAX_CLIENTS);
    if (listen_result == -1) 
    {
        perror("Listen failed");
        exit(1);
    }
    printf("Listening...\n");

    while (1) 
    {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);

        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket == -1) 
        {
            perror("Accept failed");
            exit(1);
        }
        printf("\n[+]Client connected.\n");
        if(client_socket>=0)
        {
            started = 1;
        }
        pthread_t client_thread;
        struct ClientInfo *client_info = (struct ClientInfo *)malloc(sizeof(struct ClientInfo));
        client_info->socket = client_socket;
        client_info->address = client_address;

        if (pthread_create(&client_thread, NULL, handle_client, (void *)client_info) != 0) 
        {
            perror("Thread creation failed");
            close(client_socket);
            free(client_info);
        }
    }

    close(server_socket);

    return 0;
}
