#include "smallsh.h"


/* 
 *
 **/
void userInput() {
    char input[2048] = "";

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
    char *argument;
    char arguments[512][2048];
    int length = 0;

    token = strtok_r(userInput, " ", &currPosition);
    // argument = calloc(strlen(token) + 1, sizeof(char));
    // strcpy(argument, token);

    // printf("%s\n", token);

    // free(argument);

    while ((token) != NULL && userInput[0] != '#') {
        argument = calloc(strlen(token) + 1, sizeof(char));
        strcpy(argument, token);
        strcpy(arguments[length], argument);
        length++;
        free(argument);

        token = strtok_r(NULL, " ", &currPosition);
    }

    readArguments(arguments, length);
}


/*
 *
 **/
void readArguments(char arguments[512][2048], int length) {
    // if (strstr(command, "$$") != NULL) {
    //     expandVariable(command);
    // }

    for (int i = 0; i < length; i++) {
        if (strstr(arguments[i], "$$") != NULL) {
            expandVariable(arguments[i]);
        }
    }
}


/*
 *
 **/
void expandVariable(char *variable) {
    int length = strlen(variable);

    for (int i = 0; i < length; i++) {
        if (variable[i] == '$' && i < length - 1 && variable[i + 1] == '$') {
            printf("%d", getpid());
            i++;
        } else {
            printf("%c", variable[i]);
        }
    }

    printf("\n");
}