#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctype.h>

// Define a structure to pass data to threads
typedef struct {
    double *vector1;
    double *vector2;
    double *result;
    int start;
    int end;
} ThreadData;

// Function to perform vector addition in a thread
void *addVectorSegment(void *data) {
    ThreadData *threadData = (ThreadData *)data;
    for (int i = threadData->start; i < threadData->end; i++) {
        threadData->result[i] = threadData->vector1[i] + threadData->vector2[i];
    }
    pthread_exit(NULL);
}
void *subVectorSegment(void *data) {
    ThreadData *threadData = (ThreadData *)data;
    for (int i = threadData->start; i < threadData->end; i++) {
        threadData->result[i] = threadData->vector1[i] - threadData->vector2[i];
    }
    pthread_exit(NULL);
}
void *dotVectorSegment(void *data) {
    ThreadData *threadData = (ThreadData *)data;
    for (int i = threadData->start; i < threadData->end; i++) {
        threadData->result[i] = threadData->vector1[i] * threadData->vector2[i];
    }
    pthread_exit(NULL);
}
// Function to read vector data from a file and store it in an array
int readVectorFromFile(const char *filename, double *vector, int maxElements) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; // Error opening the file
    }

    int vectorLength = 0;
    while (fscanf(file, "%lf", &vector[vectorLength]) != EOF) {
        vectorLength++;
        if (vectorLength >= maxElements) {
            break; // Stop reading if the array is full
        }
    }

    fclose(file);
    //printf("%d", vectorLength);
    return vectorLength; // Return the number of elements read
}

// Compare the lengths of two vectors
bool vectorsHaveSameLength(const double *vector1, const double *vector2, int length1, int length2) {
    return length1 == length2;
}


void runTextEditor(const char *filename);

// for ls& and multiline replaced len - 2 by len -1

bool Exit = false;
char command[100];
bool multi = false;
char *input;


int main()
{
    int i = 0, n = 0;
    bool ampersand = false;
    char *token;
    char line[100];
    char *argv[100];
    char *found = NULL;

    
    while (!Exit)
    {
        i = 0;
        ampersand = false;
        Exit = false;
        multi = false;
        
        command[0] = '\0';
        /*printf("shell> ");
        fflush(stdout);
        
        
        
        
        
        fgets(command, 100, stdin);
        */
        
        input = readline("shell>");
        // Check for EOF.
        if (!input)
            continue;
        // Add input to history.
        strcpy(command, input);
        //puts(command);
        int len = strlen(command);
        
        strcpy(line, command);
        
        if(line[len - 1] == '\\')
        {
            command[0]='\0';
            if (len > 0 && line[len - 1] == '\n')
            {
                line[len - 1] = '\0'; // Remove trailing newline
            }
        }
        while (line[len - 1] == '\\')
        {
            multi = true;
            //printf("%s", command);
            line[len - 1] = ' '; // Replace '\' with a space
            strcat(command, line);
            //printf("> ");
            /*fflush(stdout);

            fgets(line, sizeof(line), stdin);
            
            */
            input = readline(">");
            // Check for EOF.
            if (!input)
                    continue;
                    
            strcpy(line, input);
            len = strlen(line);

            if (len > 0 && line[len - 1] == '\n')
            {
                line[len - 1] = '\0'; // Remove trailing newline
            }
        }
        if(multi == true)
        {
            
            strcat(command, line);
            //printf("%s", command);
            /*for(int j=0;j<100;j++)
            {
                printf("%c", command[j]);
            }*/
            
        }
        len = strlen(command);
        add_history (command);
        // handling cases line ls|pwd by replacing ls|pwd by ls | pwds
        // Find the first occurrence of '|'
        found = strstr(command, "|");

        while (found != NULL)
        {
            // Calculate the position of found '|' in the command string
            char *output = command + (found - command);
            
            // Move the remaining part of the string to make space for " | "
            memmove(output + 3, output + 1, strlen(output));
            
            // Replace '|' with " | "
            strncpy(output, " | ", 3);
            
            // Find the next occurrence of '|'
            found = strstr(output + 3, "|");
            
        }
        
        if (len > 0 && command[len - 1] == '&')
        {
            ampersand = true;
            command[len - 1] = '\0'; // Remove the '&' and the newline character
        }

        // Tokenize the input command and build the argument array
        token = strtok(command, " \t\n");
        while (token != NULL)
        {
            argv[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        argv[i] = NULL; // Ensure the argument array is NULL-terminated

        n = i;

        if (n == 0)
        {
            continue; // Empty command, prompt again
        }

        // Check for background execution
        if (strcmp(argv[n - 1], "&") == 0)
        {
            ampersand = true;
            argv[n - 1] = NULL;
            n--;
        }

        if (n == 1 && strcmp(argv[0], "help") == 0)
        {
            // Help command
            printf("Available commands:\n");
            printf("1. pwd\n");
            printf("2. cd <directory_name>\n");
            printf("3. mkdir <directory_name>\n");
            printf("4. ls [flags]\n");
            printf("5. exit\n");
            printf("6. help\n");
            printf("7. Pipe | for multiple commands\n");
            printf("8. \\ for multiline commands\n");
            printf("9. Execute any other command\n");
            
            continue;
        }
        // Check for the "addvec" command
        if ((n >=3 && n<=4) && strcmp(argv[0], "addvec") == 0)
        {
            char *file1 = argv[1];
            char *file2 = argv[2];
            char *inputString;
            if(n>3)
            {
                inputString = argv[3];
                
                // Loop through each character in the string
                for (int l = 0; inputString[l] != '\0'; l++)
                {
                    if (inputString[l] == '-')
                    {
                        inputString[l] = ' ';
                    }
                }
                
            }
            argv[3]=inputString;
            
            int numThreads = (n == 4) ? atoi(argv[3]) : 3; // Default to 3 threads
            
            printf("Number of threads: %d\n", numThreads);
            
            int vectorLength = 100;
            // Set the vector length (adjust as needed)
            double *vector1 = (double *)malloc(vectorLength * sizeof(double));
            double *vector2 = (double *)malloc(vectorLength * sizeof(double));
            double *result = (double *)malloc(vectorLength * sizeof(double));

            int length1 = readVectorFromFile(file1, vector1, vectorLength);
            int length2 = readVectorFromFile(file2, vector2, vectorLength);

            if (length1 < 0 || length2 < 0)
            {
                perror("Error reading vector data\n");
                continue;
            }
            else if (!vectorsHaveSameLength(vector1, vector2, length1, length2))
            {
                perror("Vector lengths do not match.\n");
                continue;
            }
            else
            {
                // Split the work among threads
                int elementsPerThread = vectorLength / numThreads;
                pthread_t threads[numThreads];
                ThreadData threadData[numThreads];

                for (int i = 0; i < numThreads; i++) {
                    int start = i * elementsPerThread;
                    int end = (i == numThreads - 1) ? vectorLength : start + elementsPerThread;

                    threadData[i].vector1 = vector1;
                    threadData[i].vector2 = vector2;
                    threadData[i].result = result;
                    threadData[i].start = start;
                    threadData[i].end = end;

                    // Create threads to perform vector addition
                    pthread_create(&threads[i], NULL, addVectorSegment, &threadData[i]);
                }

                // Wait for threads to finish
                for (int i = 0; i < numThreads; i++) {
                    pthread_join(threads[i], NULL);
                }
                printf("\nAddition: \n");
                // Output the result
                for (int i = 0; i < length1; i++) {
                    printf("%.2lf ", result[i]);
                }
                printf("\n");
            }

            free(vector1);
            free(vector2);
            free(result);
            continue;
        }
        if ((n >=3 && n<=4) && strcmp(argv[0], "subvec") == 0)
        {
            char *file1 = argv[1];
            char *file2 = argv[2];
            char *inputString;
            if(n>3)
            {
                inputString = argv[3];
                
                // Loop through each character in the string
                for (int l = 0; inputString[l] != '\0'; l++)
                {
                    if (inputString[l] == '-')
                    {
                        inputString[l] = ' ';
                    }
                }
                
            }
            argv[3]=inputString;
            
            int numThreads = (n == 4) ? atoi(argv[3]) : 3; // Default to 3 threads
            
            printf("Number of threads: %d\n", numThreads);
            
            int vectorLength = 100;
            // Set the vector length (adjust as needed)
            double *vector1 = (double *)malloc(vectorLength * sizeof(double));
            double *vector2 = (double *)malloc(vectorLength * sizeof(double));
            double *result = (double *)malloc(vectorLength * sizeof(double));

            int length1 = readVectorFromFile(file1, vector1, vectorLength);
            int length2 = readVectorFromFile(file2, vector2, vectorLength);

            if (length1 < 0 || length2 < 0)
            {
                perror("Error reading vector data\n");
                continue;
            }
            else if (!vectorsHaveSameLength(vector1, vector2, length1, length2))
            {
                perror("Vector lengths do not match.\n");
                continue;
            }
            else
            {
                // Split the work among threads
                int elementsPerThread = vectorLength / numThreads;
                pthread_t threads[numThreads];
                ThreadData threadData[numThreads];

                for (int i = 0; i < numThreads; i++) {
                    int start = i * elementsPerThread;
                    int end = (i == numThreads - 1) ? vectorLength : start + elementsPerThread;

                    threadData[i].vector1 = vector1;
                    threadData[i].vector2 = vector2;
                    threadData[i].result = result;
                    threadData[i].start = start;
                    threadData[i].end = end;

                    // Create threads to perform vector addition
                    pthread_create(&threads[i], NULL, subVectorSegment, &threadData[i]);
                }

                // Wait for threads to finish
                for (int i = 0; i < numThreads; i++) {
                    pthread_join(threads[i], NULL);
                }
                printf("\nSubtraction: \n");
                // Output the result
                for (int i = 0; i < length1; i++) {
                    printf("%.2lf ", result[i]);
                }
                printf("\n");
            }

            free(vector1);
            free(vector2);
            free(result);
            continue;
        }
        if ((n >=3 && n<=4) && strcmp(argv[0], "dotprod") == 0)
        {
            char *file1 = argv[1];
            char *file2 = argv[2];
            char *inputString;
            if(n>3)
            {
                inputString = argv[3];
                
                // Loop through each character in the string
                for (int l = 0; inputString[l] != '\0'; l++)
                {
                    if (inputString[l] == '-')
                    {
                        inputString[l] = ' ';
                    }
                }
                
            }
            argv[3]=inputString;
            
            int numThreads = (n == 4) ? atoi(argv[3]) : 3; // Default to 3 threads
            
            printf("Number of threads: %d\n", numThreads);
            
            int vectorLength = 100;
            // Set the vector length (adjust as needed)
            double *vector1 = (double *)malloc(vectorLength * sizeof(double));
            double *vector2 = (double *)malloc(vectorLength * sizeof(double));
            double *result = (double *)malloc(vectorLength * sizeof(double));

            int length1 = readVectorFromFile(file1, vector1, vectorLength);
            int length2 = readVectorFromFile(file2, vector2, vectorLength);

            if (length1 < 0 || length2 < 0)
            {
                perror("Error reading vector data\n");
                continue;
            }
            else if (!vectorsHaveSameLength(vector1, vector2, length1, length2))
            {
                perror("Vector lengths do not match.\n");
                continue;
            }
            else
            {
                // Split the work among threads
                int elementsPerThread = vectorLength / numThreads;
                pthread_t threads[numThreads];
                ThreadData threadData[numThreads];

                for (int i = 0; i < numThreads; i++) {
                    int start = i * elementsPerThread;
                    int end = (i == numThreads - 1) ? vectorLength : start + elementsPerThread;

                    threadData[i].vector1 = vector1;
                    threadData[i].vector2 = vector2;
                    threadData[i].result = result;
                    threadData[i].start = start;
                    threadData[i].end = end;

                    // Create threads to perform vector addition
                    pthread_create(&threads[i], NULL, dotVectorSegment, &threadData[i]);
                }

                // Wait for threads to finish
                for (int i = 0; i < numThreads; i++) {
                    pthread_join(threads[i], NULL);
                }
                double sum=0;
                printf("\nDot Product: \n");
                // Output the result
                for (int i = 0; i < length1; i++) 
                {
                    printf("%.2lf ", result[i]);
                    sum+=result[i];
                }
                printf("\n");
                printf("sum is %.2lf ", sum);
                printf("\n");
            }

            free(vector1);
            free(vector2);
            free(result);
            continue;
        }
        if (n == 1 && strcmp(argv[0], "exit") == 0)
        {
            // Exit command
            Exit = true;
            break;
        }
    int num_pipes = 0;
    int pipe_positions[10];
        // Find pipe characters and their positions
        for (int j = 0; j < n; j++)
    {
        if (strcmp(argv[j], "|") == 0)
        {
            pipe_positions[num_pipes] = j;
            num_pipes++;
        }
    }
        if (num_pipes==0 && strcmp(argv[0], "cd") == 0)
        {
            if (n == 2)
            {
                if (chdir(argv[1]) == -1)
                {
                    perror("cd: failed to change directory\n");
                    continue;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                perror("cd: missing argument or more than two arguments\n");
                continue;
            }
        }
        else
        {
            // Execute other commands
            int pipe_positions[10]; // Store positions of pipe characters
            int num_pipes = 0;

            // Find pipe characters and their positions
            for (int j = 0; j < n; j++)
            {
                if (strcmp(argv[j], "|") == 0)
                {
                    pipe_positions[num_pipes] = j;
                    num_pipes++;
                }
            }

            if (num_pipes > 0)
            {
                // If there are pipes, set up and execute the pipeline
                int pipe_fd[2];
                int prev_pipe_read = -1;

                for (int pipe_num = 0; pipe_num <= num_pipes; pipe_num++)
                {
                    if (pipe(pipe_fd) == -1)
                    {
                        perror("Pipe creation failed\n");
                        exit(1);
                    }

                    pid_t pipe_child_pid = fork();
                    if (pipe_child_pid == -1)
                    {
                        perror("Fork failed\n");
                        exit(1);
                    }

                    if (pipe_child_pid == 0)
                    {
                        // Child process for the command in the pipeline
                        if (pipe_num > 0)
                        {
                            // Redirect input from the previous pipe
                            dup2(prev_pipe_read, STDIN_FILENO);
                            close(prev_pipe_read);
                        }

                        if (pipe_num < num_pipes)
                        {
                            // Redirect output to the next pipe
                            dup2(pipe_fd[1], STDOUT_FILENO);
                            close(pipe_fd[1]);
                        }

                        // Set up the command arguments
                        int start = (pipe_num == 0) ? 0 : (pipe_positions[pipe_num - 1] + 1);
                        int end = (pipe_num < num_pipes) ? pipe_positions[pipe_num] : n;
                        int command_argc = end - start;
                        char *command_argv[command_argc + 1];

                        for (int k = start; k < end; k++)
                        {
                            command_argv[k - start] = argv[k];
                        }
                        command_argv[command_argc] = NULL;
                        if (num_pipes>0 && command_argc == 1 && strcmp(command_argv[0], "help") == 0)
                        {
                            // Help command
                            printf("Available commands:\n");
                            printf("1. pwd\n");
                            printf("2. cd <directory_name>\n");
                            printf("3. mkdir <directory_name>\n");
                            printf("4. ls [flags]\n");
                            printf("5. exit\n");
                            printf("6. help\n");
                            printf("7. Pipe | for multiple commands\n");
                            printf("8. \\ for multiline commands\n");
                            printf("9. Execute any other command\n");
                            
                            exit(0);
                        }
                    if (num_pipes>0 && strcmp(command_argv[0], "vi") == 0)
                    {
                        // 'vi' command, launch the text editor
                        if (command_argc == 2)
                        {
                            endwin();
                            clear();
                            // Terminate the NCurses interface
                            runTextEditor(command_argv[1]);
                            //initscr();
                            exit(0);
                        }
                        else
                        {
                            printw("Usage: vi <filename>\n");
                            refresh();
                            exit(1);
                        }
                        exit(0);
                        printf("Out of editor\n");
                    }
                    if (num_pipes>0 && strcmp(command_argv[0], "exit") == 0)
                    {
                        // 'vi' command, launch the text editor
                        if (command_argc == 1)
                        {
                            // Exit command
                            Exit = true;
                            printf("exit true");
                        }
                        else
                        {
                            perror("exit has more than 0 arguments\n");
                            exit(1);
                        }
                        break;
                        //printf("Out of editor\n");
                    }
                    // Check for the "addvec" command
                    if (num_pipes > 0 && command_argc >= 3 && command_argc <= 4 && strcmp(command_argv[0], "addvec") == 0)
                    {
                        char *file1 = command_argv[1];
                        char *file2 = command_argv[2];
                        char *inputString;
                        if (command_argc > 3)
                        {
                            inputString = command_argv[3];

                            // Loop through each character in the string
                            for (int l = 0; inputString[l] != '\0'; l++)
                            {
                                if (inputString[l] == '-')
                                {
                                    inputString[l] = ' ';
                                }
                            }
                        }
                        command_argv[3] = inputString;

                        int numThreads = (command_argc == 4) ? atoi(command_argv[3]) : 3; // Default to 3 threads

                        printf("Number of threads: %d\n", numThreads);

                        int vectorLength = 100;
                        // Set the vector length (adjust as needed)
                        double *vector1 = (double *)malloc(vectorLength * sizeof(double));
                        double *vector2 = (double *)malloc(vectorLength * sizeof(double));
                        double *result = (double *)malloc(vectorLength * sizeof(double));

                        int length1 = readVectorFromFile(file1, vector1, vectorLength);
                        int length2 = readVectorFromFile(file2, vector2, vectorLength);

                        if (length1 < 0 || length2 < 0)
                        {
                            perror("Error reading vector data\n");
                            exit(1);
                        }
                        else if (!vectorsHaveSameLength(vector1, vector2, length1, length2))
                        {
                            perror("Vector lengths do not match.\n");
                            exit(1);
                        }
                        else
                        {
                            // Split the work among threads
                            int elementsPerThread = vectorLength / numThreads;
                            pthread_t threads[numThreads];
                            ThreadData threadData[numThreads];

                            for (int i = 0; i < numThreads; i++)
                            {
                                int start = i * elementsPerThread;
                                int end = (i == numThreads - 1) ? vectorLength : start + elementsPerThread;

                                threadData[i].vector1 = vector1;
                                threadData[i].vector2 = vector2;
                                threadData[i].result = result;
                                threadData[i].start = start;
                                threadData[i].end = end;

                                // Create threads to perform vector addition
                                pthread_create(&threads[i], NULL, addVectorSegment, &threadData[i]);
                            }

                            // Wait for threads to finish
                            for (int i = 0; i < numThreads; i++)
                            {
                                pthread_join(threads[i], NULL);
                            }
                            printf("\nAddition: \n");
                            // Output the result
                            for (int i = 0; i < length1; i++)
                            {
                                printf("%.2lf ", result[i]);
                            }
                            printf("\n");
                        }

                        free(vector1);
                        free(vector2);
                        free(result);
                        exit(0);
                    }
                    // Check for the "subvec" command
                    if (num_pipes > 0 && command_argc >= 3 && command_argc <= 4 && strcmp(command_argv[0], "subvec") == 0)
                    {
                        char *file1 = command_argv[1];
                        char *file2 = command_argv[2];
                        char *inputString;
                        if (command_argc > 3)
                        {
                            inputString = command_argv[3];

                            // Loop through each character in the string
                            for (int l = 0; inputString[l] != '\0'; l++)
                            {
                                if (inputString[l] == '-')
                                {
                                    inputString[l] = ' ';
                                }
                            }
                        }
                        command_argv[3] = inputString;

                        int numThreads = (command_argc == 4) ? atoi(command_argv[3]) : 3; // Default to 3 threads

                        printf("Number of threads: %d\n", numThreads);

                        int vectorLength = 100;
                        // Set the vector length (adjust as needed)
                        double *vector1 = (double *)malloc(vectorLength * sizeof(double));
                        double *vector2 = (double *)malloc(vectorLength * sizeof(double));
                        double *result = (double *)malloc(vectorLength * sizeof(double));

                        int length1 = readVectorFromFile(file1, vector1, vectorLength);
                        int length2 = readVectorFromFile(file2, vector2, vectorLength);

                        if (length1 < 0 || length2 < 0)
                        {
                            perror("Error reading vector data\n");
                            exit(1);
                        }
                        else if (!vectorsHaveSameLength(vector1, vector2, length1, length2))
                        {
                            perror("Vector lengths do not match.\n");
                            exit(1);
                        }
                        else
                        {
                            // Split the work among threads
                            int elementsPerThread = vectorLength / numThreads;
                            pthread_t threads[numThreads];
                            ThreadData threadData[numThreads];

                            for (int i = 0; i < numThreads; i++)
                            {
                                int start = i * elementsPerThread;
                                int end = (i == numThreads - 1) ? vectorLength : start + elementsPerThread;

                                threadData[i].vector1 = vector1;
                                threadData[i].vector2 = vector2;
                                threadData[i].result = result;
                                threadData[i].start = start;
                                threadData[i].end = end;

                                // Create threads to perform vector subtraction
                                pthread_create(&threads[i], NULL, subVectorSegment, &threadData[i]);
                            }

                            // Wait for threads to finish
                            for (int i = 0; i < numThreads; i++)
                            {
                                pthread_join(threads[i], NULL);
                            }
                            printf("\nSubtraction: \n");
                            // Output the result
                            for (int i = 0; i < length1; i++)
                            {
                                printf("%.2lf ", result[i]);
                            }
                            printf("\n");
                        }

                        free(vector1);
                        free(vector2);
                        free(result);
                        exit(0);
                    }
                    // Check for the "dotprod" command
                    if (num_pipes > 0 && command_argc >= 3 && command_argc <= 4 && strcmp(command_argv[0], "dotprod") == 0)
                    {
                        char *file1 = command_argv[1];
                        char *file2 = command_argv[2];
                        char *inputString;
                        if (command_argc > 3)
                        {
                            inputString = command_argv[3];

                            // Loop through each character in the string
                            for (int l = 0; inputString[l] != '\0'; l++)
                            {
                                if (inputString[l] == '-')
                                {
                                    inputString[l] = ' ';
                                }
                            }
                        }
                        command_argv[3] = inputString;

                        int numThreads = (command_argc == 4) ? atoi(command_argv[3]) : 3; // Default to 3 threads

                        printf("Number of threads: %d\n", numThreads);

                        int vectorLength = 100;
                        // Set the vector length (adjust as needed)
                        double *vector1 = (double *)malloc(vectorLength * sizeof(double));
                        double *vector2 = (double *)malloc(vectorLength * sizeof(double));
                        double *result = (double *)malloc(vectorLength * sizeof(double));

                        int length1 = readVectorFromFile(file1, vector1, vectorLength);
                        int length2 = readVectorFromFile(file2, vector2, vectorLength);

                        if (length1 < 0 || length2 < 0)
                        {
                            perror("Error reading vector data\n");
                            exit(1);
                        }
                        else if (!vectorsHaveSameLength(vector1, vector2, length1, length2))
                        {
                            perror("Vector lengths do not match.\n");
                            exit(1);
                        }
                        else
                        {
                            // Split the work among threads
                            int elementsPerThread = vectorLength / numThreads;
                            pthread_t threads[numThreads];
                            ThreadData threadData[numThreads];

                            for (int i = 0; i < numThreads; i++)
                            {
                                int start = i * elementsPerThread;
                                int end = (i == numThreads - 1) ? vectorLength : start + elementsPerThread;

                                threadData[i].vector1 = vector1;
                                threadData[i].vector2 = vector2;
                                threadData[i].result = result;
                                threadData[i].start = start;
                                threadData[i].end = end;

                                // Create threads to perform dot product
                                pthread_create(&threads[i], NULL, dotVectorSegment, &threadData[i]);
                            }

                            // Wait for threads to finish
                            for (int i = 0; i < numThreads; i++)
                            {
                                pthread_join(threads[i], NULL);
                            }
                            double sum=0;
                            printf("\nDot Product: \n");
				// Output the result
				for (int i = 0; i < length1; i++) 
				{
				    printf("%.2lf ", result[i]);
				    sum+=result[i];
				}
				printf("\n");
				printf("sum is %.2lf ", sum);
				printf("\n");
                        }

                        free(vector1);
                        free(vector2);
                        free(result);
                        exit(0);
                    }

                    if(num_pipes>0 && strcmp(command_argv[0], "cd") == 0)
                     {
                    //printf("%d %d %d\n", start, end, command_argc);
                    
                    
                    // Execute the command
                        if (command_argc == 2)
                        {
                        //printf("%s", command_argv[1]);
                            if (chdir(command_argv[1]) == -1)
                            {
                                perror("cd: failed to change directory\n");
                                exit(1);
                            }

                        }
                        else
                        {
                            perror("cd: missing argument or more than two arguments\n");
                            exit(1);
                        }
                        exit(0);
                     }
                    else
                    {
                        // Execute the command
                        execvp(command_argv[0], command_argv);
                        perror("execvp: command not found\n");
                        exit(1);
                        }
                    }
                    else
                    {
                        // Parent process
                        if (prev_pipe_read != -1)
                        {
                            close(prev_pipe_read);
                        }

                        close(pipe_fd[1]); // Close the write end of the current pipe
                        prev_pipe_read = pipe_fd[0]; // Store the read end of the current pipe

                        // Wait for the last child process to complete
                        if (pipe_num == num_pipes)
                        {
                            if (!ampersand)
                            {
                                int status;
                                waitpid(pipe_child_pid, &status, 0);
                                if (WIFEXITED(status))
                                {
                                    printf("Last child process with PID %d exited\n", pipe_child_pid);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                // If no pipe character found, execute the command as usual
                pid_t child_pid = fork();
                if (child_pid == -1)
                {
                    printf("Fork failed\n");
                    exit(1);
                }

                if (child_pid == 0)
                {
                    // Child process
                    if (strcmp(argv[0], "vi") == 0)
                    {
                        // 'vi' command, launch the text editor
                        if (n == 2)
                        {
                            endwin(); // Terminate the NCurses interface
                            runTextEditor(argv[1]);
                            //initscr();
                            exit(0);
                        }
                        else
                        {
                            printw("Usage: vi <filename>\n");
                            refresh();
                            exit(1);
                        }
                        exit(0);
                        printf("Out of editor\n");
                    }
                    if (execvp(argv[0], argv) == -1)
                    {
                        perror("execvp: command not found\n");
                        exit(1);
                    }
                    exit(0);
                }
                else if (ampersand)
                {
                    // Parent process in background execution
                    printf("Child process with PID %d started in the background.\n", child_pid);
                }
                else
                {
                    // Parent process in foreground execution
                    int status;
                    waitpid(child_pid, &status, 0);
                    if (WIFEXITED(status))
                    {
                        printf("Child process with PID %d exited\n", child_pid);
                    }
                }
            }
        }
        
    }
    endwin();
    return 0;
}

void runTextEditor(const char *filename) {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    raw();
    //nodelay(stdscr, TRUE);
    //keypad(stdscr, TRUE);
    curs_set(1);

    // Open the specified file for editing
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        file = fopen(filename, "w+");
    }

    if (file == NULL) {
        endwin();
        perror("Unable to open or create the file");
        return;
    }

    // Read the file into a buffer
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t bytes_read;
    int x = 0, y = 0;
    
    while ((bytes_read = getline(&buffer, &buffer_size, file)) != -1) {
        if(buffer[strlen(buffer)-1]=='\n')
            buffer[strlen(buffer)-1]='\0';
        printw("%s", buffer);
        y++;
    }
    
    if(y>0)
        y--;
    refresh();

    int ch;
    int lines = 0, words = 0, characters = 0;

    int in_word = 0;  // Flag to track if we're inside a word
    int prev_ch = '\n';  // Track the previous character for line counting

    while (1) {
        ch = getch();
        switch (ch) {
            case KEY_UP:
                if (y > 0) {
                    move(y - 1, x);
                    y--;
                }
                break;
            case KEY_DOWN:
                if (y < LINES - 1) {
                    move(y + 1, x);
                    y++;
                }
                break;
            case KEY_LEFT:
                if (x > 0) {
                    move(y, x - 1);
                    x--;
                }
                break;
            case KEY_RIGHT:
                if (x < COLS - 1) {
                    move(y, x + 1);
                    x++;
                }
                break;
            case 27: // ESC key
                goto exit_editor;
                break;
            case 127: // DELETE key
                if(isalnum(getyx(stdscr, y, x)) && characters > 0)characters--;
                if (x > 0) {
                    move(y, x - 1);
                    delch();
                    x--;
                } else if (y > 0) {
                    move(y - 1, COLS - 1);
                    delch();
                    x = COLS - 1;
                    y--;
                }
                break;
            case 24: // Ctrl+X (EXIT)
                goto exit_editor;
                break;
            case 3: // Ctrl+C (EXIT)
                goto exit_editor;
                break;
            case 19: // Ctrl+S (SAVE)
                // Save the contents to the file
                        fseek(file, 0, SEEK_SET);
                        for (int i = 0; i < LINES; i++) {
                            char line[COLS + 1];
                            mvinnstr(i, 0, line, COLS);
                            line[COLS] = '\0';
                            fputs(line, file);
                            //if (i < LINES - 1)
                                fputc('\n', file);
                        }
                        fflush(file); // Flush the file
                        break;
            
            case 10: // Enter key (newline)
                            //insch('\n');
                             // Move to the beginning of the next line
                            x = 0;
                            y++;
                            move(y, x);
                            in_word = 0;
                            // Update character count and lines
                            if (isalnum(ch))
                            {
                                
                                // Update line/word/character counts
                                characters++;
                            }
                            lines++;
                            break;
            case KEY_ENTER: // Enter key (newline)
                            //insch('\n');
                            move(y, x); // Move to the beginning of the next line
                            x = 0;
                            y++;
                            // Update character count and lines
                            if (isalnum(ch))
                            {
                                
                                // Update line/word/character counts
                                characters++;
                            };
                            lines++;
                            break;
            default:
                insch(ch);
                move(y, x + 1);
                x++;
                if (isalnum(ch))
                {
                    
                    // Update line/word/character counts
                    characters++;
                }
                // Check if the character is part of a word
                if (ch != ' ' && ch != '\n' && ch != '\t') {
                    if (!in_word) {
                        in_word = 1;
                        words++;
                    }
                } else {
                    in_word = 0;
                }

                // Count lines when a newline is encountered
                if (ch == '\n' && prev_ch != '\n') {
                    lines++;
                }
                prev_ch = ch;
        }
    }

exit_editor:
    endwin();
    fclose(file);
    free(buffer);
    
    printf("\nNumber of lines: %d\n", lines+1);
    printf("Number of words: %d\n", words);
    printf("Number of characters: %d\n", characters);
    //refresh();
}
