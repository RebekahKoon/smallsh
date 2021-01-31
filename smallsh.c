#include "smallsh.h"


/* 
 *
 **/
void userInput() {
    char input[256] = "";

    while (strcmp(input, "exit") != 0) {
        printf(": ");
        fgets(input, 256, stdin);

        if (strlen(input) > 0 && (input[strlen(input) - 1] == '\n')) {
            input[strlen(input) - 1] = '\0';
        }

        createTokens(input);
    }
}


/*
 *
 **/
void createTokens(char *userInput) {
    char *token;
    char *currPosition;
    char *command;

    token = strtok_r(userInput, " ", &currPosition);
    // command = calloc(strlen(token) + 1, sizeof(char));
    // strcpy(command, token);

    // printf("%s\n", token);

    // free(command);

    while ((token) != NULL && userInput[0] != '#') {
        command = calloc(strlen(token) + 1, sizeof(char));
        strcpy(command, token);
        free(command);

        token = strtok_r(NULL, " ", &currPosition);
    }
}