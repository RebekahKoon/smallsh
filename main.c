#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "smallsh.h"

/* 
 * Starting point of the smallsh program. Asks for user input
 * and then executes the command based on the input.
 **/
int main() {
    char *input = malloc(sizeof(char) * 2049);
    struct command *userCommand;

    ignore_SIGINT();
    redefine_SIGTSTP();

    input = userInput(input);

    // Loops until user exits
    while (strcmp(input, EXIT) != 0) {
        // Creating tokens based on user input
        userCommand = createTokens(input);

        // Reading command if any arguments
        if (userCommand->numArguments != 0) {
            readArguments(userCommand);
        }

        free(userCommand);

        input = userInput(input);
    }

    free(input);
    terminateProcesses();

    return 0;
}