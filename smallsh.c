#include "smallsh.h"


// int childIsForeground = 1;
int foregroundOnly = 0;

struct sigaction SIGINT_action = {0};
struct sigaction SIGTSTP_action = {0};


/* 
 *
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
 **/
void userInput() {
    char input[2048] = "";

    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);

    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    while (strcmp(input, "exit") != 0) {
        fflush(stdout);
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
    char *inputFile = NULL;
    char *outputFile = NULL;
    char *arguments[512];
    int background = 0;
    int length = 0;
    int hasArguments = 0;

    token = strtok_r(userInput, " ", &currPosition);

    while ((token) != NULL && userInput[0] != '#') {
        if (!strcmp(token, "&")) {
            background = 1;
            // childIsForeground = 0;
        } else if (!strcmp(token, "<")) {
            token = strtok_r(NULL, " ", &currPosition);
            inputFile = token;
        } else if (!strcmp(token, ">")) {
            token = strtok_r(NULL, " ", &currPosition);
            outputFile = token;
        } else if (strstr(token, "$$") != NULL) {
            arguments[length] = expandVariable(token);
            length++;
            hasArguments = 1;
        } else {
            arguments[length] = token;
            length++;
            hasArguments = 1;
        }

        token = strtok_r(NULL, " ", &currPosition);
    }

    if (hasArguments) {
        readArguments(arguments, length, inputFile, outputFile, background);
    }
}


/*
 *
 **/
void readArguments(char *arguments[], int length, char *inputFile, char *outputFile, int background) {
    char *expandedVar;
    static int status = 0;

    // if (strstr(arguments[0], "$$") != NULL) {
    //     expandedVar = expandVariable(arguments[0]);
    //     printf("%s\n", expandedVar);
    //     fflush(stdout);
    // } else 
    if (strcmp(arguments[0], "cd") == 0) {
        changeDirectory(arguments[1], length);
    } else if (strcmp(arguments[0], "status") == 0) {
        findStatus(status);
    } else if (strcmp(arguments[0], "exit") == 0) {
        printf("\n");
        fflush(stdout);
    } else {
        status = executeOtherCommand(arguments, length, status, inputFile, outputFile, background);
        fflush(stdout);
    }
}


/*
 *
 **/
char *expandVariable(char *variable) {
    int length = strlen(variable);
    char *expandedVar = malloc(sizeof(char) * 2048);

    for (int i = 0; i < length; i++) {
        if (variable[i] == '$' && i < length - 1 && variable[i + 1] == '$') {
            sprintf(expandedVar, "%s%d", expandedVar, getpid());
            fflush(stdout);
            i++;
        } else {
            sprintf(expandedVar, "%s%c", expandedVar, variable[i]);
            fflush(stdout);
        }
    }

    return expandedVar;
}


/*
 *
 * Source: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
 **/
void changeDirectory(char *path, int numArguments) {
    char *directory;
    char cwd[PATH_MAX];
    char *expandedVar;

    if (numArguments == 1) {
        directory = getenv("HOME");
        chdir(directory);
        getcwd(cwd, sizeof(cwd));
        // printf("%s\n", cwd);
        // fflush(stdout);
    } else if (strstr(path, "$$") != NULL) {
        expandedVar = expandVariable(path);
        printf("%s\n", expandedVar);
        fflush(stdout);
        chdir(path);
        getcwd(cwd, sizeof(cwd));
        // printf("%s\n", cwd);
        // fflush(stdout);
    } else {
        chdir(path);
        getcwd(cwd, sizeof(cwd));
        // printf("%s\n", cwd);
        // fflush(stdout);
    }
}


/*
 *
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-monitoring-child-processes?module_item_id=20163874
 *         https://stackoverflow.com/questions/3659616/why-does-wait-set-status-to-256-instead-of-the-1-exit-status-of-the-forked-pr
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
int executeOtherCommand(char *arguments[], int length, int status, char *inputFile, char *outputFile, int background) {
    pid_t spawnPid = -5;
    pid_t childPid;
    int result;
    int targetFD;
    int sourceFD;
    char *commandArgs[length];

    for (int i = 0; i < length; i++) {
        commandArgs[i] = arguments[i];
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
        if (background == 0) {
            // struct sigaction SIGINT_action = {0};
            SIGINT_action.sa_handler = handle_SIGINT;
            sigfillset(&SIGINT_action.sa_mask);
            SIGINT_action.sa_flags = 0;
            sigaction(SIGINT, &SIGINT_action, NULL);
        }

        // If input file argument
        if (inputFile != NULL) {
            sourceFD = open(inputFile, O_RDONLY);
            if (sourceFD == -1) { 
                printf("cannot open %s for input\n", inputFile);
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
                printf("cannot open %s for output\n", outputFile);
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

        if (execvp(commandArgs[0], commandArgs)) {
            printf("%s: no such file or directory\n", arguments[0]);
            fflush(stdout);
            exit(1);
        }

        break;
    
    default:
        if (background == 1 && foregroundOnly == 0) {
            childPid = waitpid(spawnPid, &status, WNOHANG);
            printf("background pid is %d\n", spawnPid);
            fflush(stdout);
        } else {
            childPid = waitpid(spawnPid, &status, 0);
            fflush(stdout);
        }

        if(WIFSIGNALED(status)) {
            findStatus(status);
        }
    }

    return status;
}


/*
 *
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
 **/
void handle_SIGINT(int signal) {
    // char *message = NULL;
    // snprintf(message, "terminated by signal %d\n", WTERMSIG(signal));
    // write(STDOUT_FILENO, message, 23);
    // fflush(stdout);
    if(WIFSIGNALED(signal)) {
        findStatus(signal);
    }
}


/*
 *
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
 **/
void handle_SIGTSTP(int signal) {
    char *message = NULL;

    if (foregroundOnly == 0) {
        foregroundOnly = 1;
        message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 50);
        fflush(stdout);
    } else {
        foregroundOnly = 0;
        message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 30);
        fflush(stdout);
    }
}