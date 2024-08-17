#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    char input[100];

    while (1) {
        // Print the prompt
        printf("$ ");
        fflush(stdout);

        // Wait for user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; // Exit if fgets fails (e.g., EOF)
        }

        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;

        // Tokenize the input string to separate command and arguments
        char *args[10];
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < 10) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Check if the command is "exit 0"
        if (strcmp(args[0], "exit") == 0 && strcmp(args[1], "0") == 0) {
            exit(0); // Exit the program with status 0
        }

        // Check if the command is "echo"
        if (strcmp(args[0], "echo") == 0) {
            // Print the arguments after "echo"
            for (int j = 1; args[j] != NULL; j++) {
                if (j > 1) printf(" ");
                printf("%s", args[j]);
            }
            printf("\n");
            continue; // Skip the rest of the loop and prompt again
        }

        // Check if the command is "type"
        if (strcmp(args[0], "type") == 0) {
            char *command = args[1];

            // Check if the command is a known builtin
            if (strcmp(command, "echo") == 0 || strcmp(command, "exit") == 0 || strcmp(command, "type") == 0) {
                printf("%s is a shell builtin\n", command);
            } else {
                // Check if the command is an executable file in PATH
                char *path = getenv("PATH");
                if (path != NULL) {
                    char pathCopy[1024];
                    strncpy(pathCopy, path, sizeof(pathCopy));
                    pathCopy[sizeof(pathCopy) - 1] = '\0'; // Ensure null termination

                    char *dir = strtok(pathCopy, ":");
                    int found = 0;

                    while (dir != NULL) {
                        char fullPath[150];
                        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, command);
                        if (access(fullPath, X_OK) == 0) { // Check if the file is executable
                            printf("%s is %s\n", command, fullPath);
                            found = 1;
                            break;
                        }
                        dir = strtok(NULL, ":");
                    }

                    if (!found) {
                        printf("%s: not found\n", command);
                    }
                } else {
                    printf("%s: not found\n", command);
                }
            }
            continue; // Skip the rest of the loop and prompt again
        }

        // If the command is not built-in, attempt to execute it
        // First, check if the command exists in PATH
        char *command = args[0];
        char *path = getenv("PATH");
        int command_found = 0;
        char fullPath[150];
        if (path != NULL) {
            char pathCopy[1024];
            strncpy(pathCopy, path, sizeof(pathCopy));
            pathCopy[sizeof(pathCopy) - 1] = '\0'; // Ensure null termination

            char *dir = strtok(pathCopy, ":");

            while (dir != NULL) {
                snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, command);
                if (access(fullPath, X_OK) == 0) { // Check if the file is executable
                    command_found = 1;
                    break;
                }
                dir = strtok(NULL, ":");
            }
        }

        // If the command is not found, print an error message
        if (!command_found) {
            printf("%s: command not found\n", command);
            continue; // Skip the execution attempt
        }

        // Command was found, so now execute it
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            execvp(fullPath, args);

            // If execvp fails, print an error message and exit
            perror("execvp");
            exit(1);
        } else if (pid > 0) {
            // Parent process: wait for the child to complete
            int status;
            waitpid(pid, &status, 0);
        } else {
            // Fork failed
            perror("fork");
        }
    }

    return 0;
}
