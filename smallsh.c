#include "smallsh.h"


/* 
 *
 **/
void userInput() {
    char input[2048] = "";

    while (strcmp(input, "exit") != 0) {
        printf(": ");
        fgets(input, 256, stdin);
        fflush(stdin);

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

    // printf("%d", length);
    char *expandedVar;
    int status = 0;

    for (int i = 0; i < length; i++) {
        if (strstr(arguments[i], "$$") != NULL) {
            expandedVar = expandVariable(arguments[i]);
            printf("%s\n", expandedVar);
        } else if (strcmp(arguments[i], "cd") == 0) {
            // printf("%s %s\n", arguments[i], arguments[i + 1]);
            changeDirectory(arguments[i + 1], length);
        } else if (strcmp(arguments[i], "status") == 0) {
            findStatus(status);
        }
    }
}


/*
 *
 **/
char *expandVariable(char *variable) {
    int length = strlen(variable);
    char *expandedVar = malloc(sizeof(char) * 2048);
    char strPointer = 0;

    // for (int i = 0; i < length; i++) {
    //     if (variable[i] == '$' && i < length - 1 && variable[i + 1] == '$') {
    //         printf("%d", getpid());
    //         i++;
    //     } else {
    //         printf("%c", variable[i]);
    //     }
    // }

    for (int i = 0; i < length; i++) {
        if (variable[i] == '$' && i < length - 1 && variable[i + 1] == '$') {
            sprintf(expandedVar, "%s%d", expandedVar, getpid());
            // strPointer = strlen(expandedVar) - 1;
            i++;
        } else {
            // expandedVar[strPointer] = variable[strPointer];
            sprintf(expandedVar, "%s%c", expandedVar, variable[i]);
        }
    }

    // printf("%s\n", expandedVar);

    return expandedVar;
}


/*
 *
 * Source: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
 **/
void changeDirectory(char *path, int numArguments) {
    // if (path != NULL) {
    //     printf("Hi\n");
    // } else {
    //     printf("%s\n", path);
    // }
    char *directory;
    char cwd[PATH_MAX];
    char *expandedVar;

    if (numArguments == 1) {
        directory = getenv("HOME");
        chdir(directory);
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        // printf("1\n");
    } else if (strstr(path, "$$") != NULL) {
        expandedVar = expandVariable(path);
        printf("%s\n", expandedVar);
        chdir(path);
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
    } else {
        chdir(path);
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        // printf("2\n");
    }
}


/*
 *
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-monitoring-child-processes?module_item_id=20163874
 **/
void findStatus(int status) {
    if (WIFEXITED(status)) {
        printf("exit value %d\n", WEXITSTATUS(status));
    } else {
        printf("terminated by signal %d\n", WTERMSIG(status));
    }
}