#include "smallsh.h"


/* 
 *
 **/
void userInput() {
    char input[2048] = "";

    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = handle_SIGINT;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);

    struct sigaction SIGTSTP_action = {0};
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    while (strcmp(input, "exit") != 0) {
        printf(": ");
        fgets(input, 256, stdin);
        fflush(stdin);
        fflush(stdout);

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
    // char *argument;
    char *inputFile = NULL;
    char *outputFile = NULL;
    char *arguments[512];
    int length = 0;

    // inputFile = malloc(sizeof(char) * 256);
    // outputFile = malloc(sizeof(char) * 256);
    // inputFile = NULL;
    // outputFile = NULL;
    token = strtok_r(userInput, " ", &currPosition);
    // argument = calloc(strlen(token) + 1, sizeof(char));
    // strcpy(argument, token);

    // printf("%s\n", token);

    // free(argument);

    while ((token) != NULL && userInput[0] != '#') {
        // argument = calloc(strlen(token) + 1, sizeof(char));
        // strcpy(argument, token);

        if (!strcmp(token, "<")) {
            token = strtok_r(NULL, " ", &currPosition);
            inputFile = token;
        } else if (!strcmp(token, ">")) {
            token = strtok_r(NULL, " ", &currPosition);
            outputFile = token;
        } else {
            arguments[length] = token;

            // if (length > 0 && !strcmp(arguments[length - 1], "<")) {
            //     strcpy(inputFile, argument);
            //     printf("%s", inputFile);
            // } else if (length > 0 && !strcmp(arguments[length - 1], ">")) {
            //     strcpy(outputFile, argument);
            //     printf("%s", outputFile);
            // }

            // free(argument);
            length++;
        }

        token = strtok_r(NULL, " ", &currPosition);
    }

    // for (int i = 0; i < length; i++) {
    //     printf("%s", arguments[i]);
    // }

    // for (int i = 0; i < length; i++) {
    //     if (!strcmp(arguments[i], "<")) {
    //         strcpy(inputFile, arguments[i + 1]);
    //         printf("%s", inputFile);
    //     } else if (!strcmp(arguments[i], ">")) {
    //         strcpy(outputFile, arguments[i + 1]);
    //         printf("%s", outputFile);
    //     }
    // }

    // printf("%s", inputFile);
    // printf("%s", outputFile);

    readArguments(arguments, length, inputFile, outputFile);
    
    // free(inputFile);
    // free(outputFile);
    // for (int i = 0; i < length; i++) {
    //     arguments[i] = NULL;
    // }
}


/*
 *
 **/
void readArguments(char *arguments[], int length, char *inputFile, char *outputFile) {
    // if (strstr(command, "$$") != NULL) {
    //     expandVariable(command);
    // }

    // printf("%d", length);
    char *expandedVar;
    int status = 0;
    // int builtIn = 0;
    // char *inputFile = malloc(sizeof(char) * 256);
    // char *outputFile = malloc(sizeof(char) * 256);

    // strcpy(inputFile, "");
    // strcpy(outputFile, "");

    // for (int i = 0; i < length; i++) {
    //     if (!strcmp(arguments[i], "<")) {
    //         strcpy(inputFile, arguments[i + 1]);
    //         // printf("%s", inputFile);
    //     } else if (!strcmp(arguments[i], ">")) {
    //         strcpy(outputFile, arguments[i + 1]);
    //         // printf("%s", outputFile);
    //     }
    // }

    // for (int i = 0; i < length; i++) {
    //     if (strstr(arguments[i], "$$") != NULL) {
    //         expandedVar = expandVariable(arguments[i]);
    //         builtIn = 1;
    //         printf("%s\n", expandedVar);
    //     } else if (strcmp(arguments[i], "cd") == 0) {
    //         // printf("%s %s\n", arguments[i], arguments[i + 1]);
    //         changeDirectory(arguments[i + 1], length);
    //         builtIn = 1;
    //     } else if (strcmp(arguments[i], "status") == 0) {
    //         findStatus(status);
    //         builtIn = 1;
    //     } else if (strcmp(arguments[i], "exit") == 0) {
    //         builtIn = 1;
    //     }
    // }

    // if (builtIn == 0) {
    //     executeOtherCommand(arguments, length, status);
    // }

    if (strstr(arguments[0], "$$") != NULL) {
        expandedVar = expandVariable(arguments[0]);
        // builtIn = 1;
        printf("%s\n", expandedVar);
        fflush(stdout);
    } else if (strcmp(arguments[0], "cd") == 0) {
        // printf("%s %s\n", arguments[i], arguments[i + 1]);
        changeDirectory(arguments[1], length);
        // builtIn = 1;
    } else if (strcmp(arguments[0], "status") == 0) {
        findStatus(status);
        // builtIn = 1;
    } else if (strcmp(arguments[0], "exit") == 0) {
        // builtIn = 1;
        printf("\n");
        fflush(stdout);
    } else {
        // char *commandArgs[length];

        // for (int i = 0; i < length; i++) {
        //     commandArgs[i] = arguments[i];
        //     // printf("%s hi", arguments[i]);
        // }

        // for (int i = 0; i < length; i++) {
        //     printf("%s %d", arguments[i], length);
        // }

        executeOtherCommand(arguments, length, status, inputFile, outputFile);
        printf("\n");
        fflush(stdout);

        // for (int i = 0; i < length; i++) {
        //     commandArgs[i] = NULL;
        // }
    }

    // free(inputFile);
    // free(outputFile);
}


/*
 *
 **/
char *expandVariable(char *variable) {
    int length = strlen(variable);
    char *expandedVar = malloc(sizeof(char) * 2048);
    // char strPointer = 0;

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
            fflush(stdout);
            // strPointer = strlen(expandedVar) - 1;
            i++;
        } else {
            // expandedVar[strPointer] = variable[strPointer];
            sprintf(expandedVar, "%s%c", expandedVar, variable[i]);
            fflush(stdout);
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
        fflush(stdout);
        // printf("1\n");
    } else if (strstr(path, "$$") != NULL) {
        expandedVar = expandVariable(path);
        printf("%s\n", expandedVar);
        fflush(stdout);
        chdir(path);
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        fflush(stdout);
    } else {
        chdir(path);
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        fflush(stdout);
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
        fflush(stdout);
    } else {
        printf("terminated by signal %d\n", WTERMSIG(status));
        fflush(stdout);
    }
}


/* 
 *
 * Sources: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-creating-and-terminating-processes?module_item_id=20163873
 *          https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-monitoring-child-processes?module_item_id=20163874
 *          https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-executing-a-new-program?module_item_id=20163875
 *          https://canvas.oregonstate.edu/courses/1798831/pages/exploration-processes-and-i-slash-o?module_item_id=20163883
 **/
int executeOtherCommand(char *arguments[], int length, int status, char *inputFile, char *outputFile) {
    pid_t spawnPid = -5;
    pid_t childPid;
    int result;
    int targetFD;
    int sourceFD;
    char *commandArgs[length];

    for (int i = 0; i < length; i++) {
        commandArgs[i] = arguments[i];
        // printf("%s %s", commandArgs[i], arguments[i]);
    }

    spawnPid = fork();
    switch (spawnPid)
    {
    case -1:
        perror("fork() failed!\n");
        fflush(stdout);
        exit(1);
        break;

    case 0:
        // for (int i = 0; i < length; i++) {
        //     if (strcpy(commandArgs[i], "<")) {
        //         printf("input %s\n", commandArgs[i + 1]);
        //     } else if (strcpy(commandArgs[i], ">")) {
        //         printf("output %s\n", commandArgs[i + 1]);
        //     }
        // }

        // If input file argument
        if (inputFile != NULL) {
            sourceFD = open(inputFile, O_RDONLY);
            if (sourceFD == -1) { 
                perror("source open()");
                fflush(stdout); 
                exit(1); 
            }

            // Redirect stdin to source file
            result = dup2(sourceFD, 0);
            if (result == -1) { 
                perror("source dup2()");
                fflush(stdout);
                exit(2); 
            }

            fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
        }

        // If output file argument
        if (outputFile != NULL) {
            // Open target file
            targetFD = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (targetFD == -1) { 
                perror("target open()");
                fflush(stdout);
                exit(1); 
            }

            // Redirect stdout to target file
            result = dup2(targetFD, 1);
            if (result == -1) { 
                perror("target dup2()");
                fflush(stdout);
                exit(2); 
            }

            fcntl(targetFD, F_SETFD, FD_CLOEXEC);
        }

        // if (outputFile == NULL && inputFile == NULL) {
        //     char *fileCommand[1];
        //     fileCommand[0] = "ls";
        //     printf("hi");

        //     if (execvp("ls", fileCommand)) {
        //         printf("%s: no such file or directory\n", arguments[0]);
        //         fflush(stdout);
        //         exit(1);
        //     }
        // }

        if (execvp(commandArgs[0], commandArgs)) {
            printf("%s: no such file or directory\n", arguments[0]);
            fflush(stdout);
            exit(1);
        }

        // execlp(arguments[0], arguments[0], arguments[1], NULL);
        // perror("execve");
        exit(1);
        break;
    
    default:
        fflush(stdout);
        childPid = waitpid(spawnPid, &status, 0);
        // findStatus(status);
        fflush(stdout);
        break;
    }

    return status;
}


/*
 *
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
 **/
void handle_SIGINT(int signal) {
	char* message = "Caught SIGINT, sleeping for 1 second\n";
	write(STDOUT_FILENO, message, 39);
    fflush(stdout);
	sleep(1);
}


/*
 *
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
 **/
void handle_SIGTSTP(int signal) {
	char* message = "Caught SIGTSTP, sleeping for 1 second\n";
	write(STDOUT_FILENO, message, 39);
    fflush(stdout);
	sleep(1);
}