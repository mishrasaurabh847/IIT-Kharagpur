#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void show_menu() 
{
    printf("ftp_client> ");
    fflush(stdout);
}
void display_server_response(int client_socket) 
{
    int size;
    recv(client_socket, &size, sizeof(int), 0);

    if (size > 0) 
    {
        char *server_data = (char *)malloc(size);
        recv(client_socket, server_data, size, 0);
        printf("Server response:\n%s\n", server_data);
        free(server_data);
    } 
    else
    {
        printf("No response from the server.\n");
    }
}

int main() 
{
    int client_socket;
    struct sockaddr_in server;
    int port;
    char ip[100];
    printf("Enter the address:\n");
    fgets(ip, 100, stdin);
    printf("Enter the Port Number: ");
    scanf("%d", &port);
    getchar();  // Consume the newline character left in the input buffer

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) 
    {
        perror("Socket creation failed");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    int connect_result = connect(client_socket, (struct sockaddr *)&server, sizeof(server));
    if (connect_result == -1) 
    {
        perror("Connect Error");
        exit(1);
    }

    show_menu();

    while (1) 
    {
        char command[100];
        char filename[100];
        int size;

        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';
        
        if (strcmp(command, "close") == 0) 
        {
            close(client_socket);
            break;
        }
        
        send(client_socket, command, sizeof(command), 0);
        
        if (strncmp(command, "get", 3) == 0) 
        {
            sscanf(command, "get %s", filename);
            char *lastPeriod = strrchr(filename, '.');
            
            if (lastPeriod != NULL)
            {
                // Calculate the position to insert "_ftp"
                size_t position = lastPeriod - filename;
                // Insert "_ftp" at the calculated position
                memmove(filename + position + 11, lastPeriod, strlen(lastPeriod) + 1);
                strncpy(filename + position, "_fromServer", 11);
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
        }
        else if (strncmp(command, "put", 3) == 0) 
        {
            
            sscanf(command, "put %s", filename);
            FILE *file = fopen(filename, "rb");

            if (file != NULL)
            {
                fseek(file, 0, SEEK_END);
                size = ftell(file);
                //fprintf("SIZE %d\n", size);
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
        else if (strcmp(command, "ls") == 0) 
        {
            recv(client_socket, &size, sizeof(int), 0);

            if (size > 0) 
            {
                char *file_list = (char *)malloc(size);
                recv(client_socket, file_list, size, 0);
                printf("Server directory listing:\n%s\n", file_list);
                free(file_list);
            } 
            else
            {
                printf("Server directory is empty.\n");
            }
        } 
        else if (strncmp(command, "cd", 2) == 0) 
        {
            // Send the specified directory to the server
            send(client_socket, command, sizeof(command), 0);
            recv(client_socket, &size, sizeof(int), 0);

            if (size == 0) 
            {
                printf("Directory changed successfully on the server.\n");
            } else
            {
                printf("Directory change failed on the server.\n");
            }
            recv(client_socket, &size, sizeof(int), 0);

            if (size > 0) 
            {
                char *cwd = (char *)malloc(size);
                recv(client_socket, cwd, size, 0);
                printf("Server current working directory: %s\n", cwd);
                free(cwd);
            } 
            else
            {
                printf("Failed to retrieve the current working directory from the server.\n");
            }
        } 
        else if (strcmp(command, "pwd") == 0)
        {
            // Request and receive the current working directory from the server
            send(client_socket, command, sizeof(command), 0);
            recv(client_socket, &size, sizeof(int), 0);

            if (size > 0) 
            {
                char *cwd = (char *)malloc(size);
                recv(client_socket, cwd, size, 0);
                printf("Server current working directory: %s\n", cwd);
                free(cwd);
            } 
            else
            {
                printf("Failed to retrieve the current working directory from the server.\n");
            }
        }
        show_menu();
    }

    return 0;
}
