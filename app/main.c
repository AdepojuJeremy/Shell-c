#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    char input[100];

    while (1) {
        // Print the prompt
        printf("$ ");
        fflush(stdout);

        // Wait for user input
        if (fgets(input, 100, stdin) == NULL) {
            break; // Exit if fgets fails (e.g., EOF)
        }

        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;

        // Check if the command is "exit 0"
        if (strcmp(input, "exit 0") == 0) {
            exit(0); // Exit the program with status 0
        }

        // Check if the command starts with "echo"
        if (strncmp(input, "echo ", 5) == 0) {
            // Print the rest of the input after "echo "
            printf("%s\n", input + 5);
            continue; // Skip the rest of the loop and prompt again
        }

        // Check if the command starts with "type"
        if (strncmp(input, "type ", 5) == 0) {
            char *command = input + 5;

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

        // Check if the command exists
        if (access(input, F_OK) == -1) {
            // Command not found
            printf("%s: command not found\n", input);
        }
    }

    return 0;
}
