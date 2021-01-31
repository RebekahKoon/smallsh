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
    strcmp(arguments[1], "");
}


/*
 *
 **/
void readArguments(char arguments[512][2048], int length) {
    // if (strstr(command, "$$") != NULL) {
    //     expandVariable(command);
    // }

    printf("%d", length);

    for (int i = 0; i < length; i++) {
        if (strstr(arguments[i], "$$") != NULL) {
            expandVariable(arguments[i]);
        } else if (strcmp(arguments[i], "cd") == 0) {
            // printf("%s %s\n", arguments[i], arguments[i + 1]);
            changeDirectory(arguments[i + 1], length);
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


/*
 *
 **/
void changeDirectory(char *path, int numArguments) {
    // if (path != NULL) {
    //     printf("Hi\n");
    // } else {
    //     printf("%s\n", path);
    // }
    char *directory;

    if (numArguments == 1) {
        directory = getenv("HOME");
        chdir(directory);
        printf("1\n");
    } else {
        printf("2\n");
        chdir(path);
    }
}