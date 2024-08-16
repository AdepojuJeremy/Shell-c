#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    // Print the prompt
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    // Remove the newline character from the input
    input[strcspn(input, "\n")] = 0;

    // Check if the command exists
    if (access(input, F_OK) == -1) {
        // Command not found
        printf("%s: command not found\n", input);
    }

    return 0;
}
