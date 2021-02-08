#include "smallsh.h"

int foregroundOnly = 0;

struct sigaction SIGINT_action = {{0}};
struct sigaction SIGTSTP_action = {{0}};


/* 
 * Asks for user input. Additionally turns off the default actions of ctrl-c and ctrl-z.
 **/
char *userInput(char *input) {
    fflush(stdout);
    printf(": ");
    fgets(input, 2049, stdin);
    fflush(stdin);
    fflush(stdout);

    // If newline is in the input, removes it
    if (strlen(input) > 0 && (input[strlen(input) - 1] == '\n')) {
        input[strlen(input) - 1] = '\0';
    }

    return input;
}


/*
 * Creates tokens from user input separated by spaces. Puts the tokens
 * in a command struct in order to read the command. 
 * 
 * @params char *userInput: input of command from the user
 **/
struct command *createTokens(char *userInput) {
    char *token;
    char *currPosition;
    char *tempPosition;

    // Initializing struct that will store the user's command
    struct command *userCommand = malloc(sizeof(struct command));
    userCommand->numArguments = 0;
    userCommand->background = 0;
    userCommand->inputFile = NULL;
    userCommand->outputFile = NULL;

    token = strtok_r(userInput, " ", &currPosition);

    // Finding each argument of the command if line is not a comment
    while ((token) != NULL && userInput[0] != '#') {
        if (!strcmp(token, "&") && ((token = strtok_r(NULL, " ", &tempPosition)) == NULL)) {
            if (foregroundOnly == 0) {
                // Process will run in the background
                userCommand->background = 1;
            }
        } else if (!strcmp(token, "<")) {
            // Redirection to an input file exists
            token = strtok_r(NULL, " ", &currPosition);
            userCommand->inputFile = token;
        } else if (!strcmp(token, ">")) {
            // Redirection to an output file exists
            token = strtok_r(NULL, " ", &currPosition);
            userCommand->outputFile = token;
        } else if (strstr(token, "$$") != NULL) {
            // Variable needs to be expanded
            userCommand->arguments[userCommand->numArguments] = expandVariable(token);
            userCommand->numArguments++;
        } else {
            userCommand->arguments[userCommand->numArguments] = token;
            userCommand->numArguments++;
        }

        token = strtok_r(NULL, " ", &currPosition);
        tempPosition = currPosition;
    }

    return userCommand;
}


/*
 * Reads arguments from the command. The built-in commands are changing
 * the directory, printing the status of the last foreground process, or
 * exiting the program. If none of these were inputted, then a non-built-in
 * command has been inputted and will be read.
 * 
 * @params struct command *userCommand: struct containing the user's
 *         command information
 **/
void readArguments(struct command *userCommand) {
    static int status = 0;

    if (strcmp(userCommand->arguments[0], "cd") == 0) {
        // Change to another directory
        changeDirectory(userCommand->arguments[1], userCommand->numArguments);
    } else if (strcmp(userCommand->arguments[0], "status") == 0) {
        // Finding status of the last non-built-in foreground process
        findStatus(status);
    } else if (strcmp(userCommand->arguments[0], "exit") == 0) {
        // Program will exit
        printf("\n");
        fflush(stdout);
    } else {
        // Non-built-in command
        status = executeOtherCommand(userCommand, status);
        fflush(stdout);
    }
}


/*
 * Expands all instances of "$$" in a variable with the process ID 
 * of smallsh.
 * 
 * @params char *variable: variable that will be expanded
 * @returns char *expandedVar: the expanded variable
 **/
char *expandVariable(char *variable) {
    int length = strlen(variable);
    char *expandedVar = malloc(sizeof(char) * 2048);

    // Finding all instances of "$$" in the variable
    for (int i = 0; i < length; i++) {
        if (variable[i] == '$' && i < length - 1 && variable[i + 1] == '$') {
            // Expanding the variable
            sprintf(expandedVar, "%s%d", expandedVar, getpid());
            fflush(stdout);
            i++;
        } else {
            sprintf(expandedVar, "%s%c", expandedVar, variable[i]);
            fflush(stdout);
        }
    }

    return expandedVar;

    free(expandedVar);
}


/*
 * Changes the current directory. If no other argument was inputted, the
 * directory will be changed to the home directory. If there is an
 * argument for a directory, then it will be changed to that directory.
 * 
 * @params char *path: path to the directory
 * @params int numArguments: number of arguments in the command
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
    } else if (strstr(path, "$$") != NULL) {
        expandedVar = expandVariable(path);
        printf("%s\n", expandedVar);
        fflush(stdout);
        chdir(path);
        getcwd(cwd, sizeof(cwd));
    } else {
        chdir(path);
        getcwd(cwd, sizeof(cwd));
    }
}


/*
 * Finds and prints the exit status or the terminating signal of the last
 * foreground process run by smallsh.
 * 
 * @params int status: status number of the last foreground process
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
 * Executes non-built-in commands by first forking a child off from the parent
 * process and then using execvp() to execute the command. If an input or 
 * output file was inputted, then first redirects to either or both of those 
 * files. The process will be run either in the foreground or background
 * depending on the user's command. A message will be printed if any 
 * background processes have been terminated.
 * 
 * @params struct command *userCommand: contains data of the user's command
 * @params int status: stores the status of the command
 * @returns int status: returns the status of the command
 * 
 * Sources: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-creating-and-terminating-processes?module_item_id=20163873
 *          https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-monitoring-child-processes?module_item_id=20163874
 *          https://canvas.oregonstate.edu/courses/1798831/pages/exploration-process-api-executing-a-new-program?module_item_id=20163875
 **/
int executeOtherCommand(struct command *userCommand, int status) {
    pid_t spawnPid = -5;
    pid_t childPid;
    char *commandArgs[userCommand->numArguments];

    // Put commands in another array that has length of number of arguments
    for (int i = 0; i < userCommand->numArguments; i++) {
        commandArgs[i] = userCommand->arguments[i];
    }

    spawnPid = fork();
    switch (spawnPid)
    {
    // Fork failure
    case -1:
        perror("fork() failed!\n");
        fflush(stdout);
        exit(1);
        break;

    // Child process
    case 0:
        // ctrl-c can cancel foreground process
        if (userCommand->background == 0) {
            SIGINT_action.sa_handler = SIG_DFL;
            sigfillset(&SIGINT_action.sa_mask);
            SIGINT_action.sa_flags = 0;
            sigaction(SIGINT, &SIGINT_action, NULL);
        }

        if (userCommand->background == 1) {
            // If input redirection not specified
            if (userCommand->inputFile == NULL) {
                userCommand->inputFile = "/dev/null";
                redirectInput(userCommand->inputFile);
            }

            // If output redirection not specified
            if (userCommand->outputFile == NULL) {
                userCommand->outputFile = "/dev/null";
                redirectOutput(userCommand->outputFile);
            }
        }

        // If input file argument
        if (userCommand->inputFile != NULL) {
            redirectInput(userCommand->inputFile);
        }

        // If output file argument
        if (userCommand->outputFile != NULL) {
            redirectOutput(userCommand->outputFile);
        }

        // Executes command if valid
        if (execvp(commandArgs[0], commandArgs)) {
            printf("%s: no such file or directory\n", userCommand->arguments[0]);
            fflush(stdout);
            exit(1);
        }

        break;
    
    default:
        if (userCommand->background == 1 && foregroundOnly == 0) {
            // Running process in the background
            childPid = waitpid(spawnPid, &status, WNOHANG);
            printf("background pid is %d\n", spawnPid);
            fflush(stdout);
        } else {
            // Running process in the foreground
            childPid = waitpid(spawnPid, &status, 0);
            fflush(stdout);

            // Finding status if ctrl-c inputted
            if(WIFSIGNALED(status)) {
                findStatus(status);
            }
        }
    }

    // Looking for any finished background processes
    while ((childPid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("background pid %d is done: ", childPid);
        findStatus(status);
    }

    return status;
}


/*
 * If ctrl-z is inputted, changes to foreground-only mode. If ctrl-z
 * is inputted again, exits foreground-only mode.
 * 
 * @params int signal: signal of the inputted ctrl-z
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


/* 
 * Redirects input to the desired file if the file is found.
 * 
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-processes-and-i-slash-o?module_item_id=20163883
 **/
void redirectInput(char *inputFile) {
    int sourceFD;
    int result;

    sourceFD = open(inputFile, O_RDONLY);
    // Can't open file
    if (sourceFD == -1) { 
        printf("cannot open %s for input\n", inputFile);
        fflush(stdout); 
        exit(1); 
    }

    result = dup2(sourceFD, 0);
    // Failed redirect
    if (result == -1) { 
        perror("source dup2()");
        fflush(stdout);
        exit(2); 
    }

    // Closing file
    fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
}


/* 
 * Redirects output to the desired file if the file is found.
 * 
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-processes-and-i-slash-o?module_item_id=20163883
 **/
void redirectOutput(char *outputFile) {
    int targetFD;
    int result;

    // Open target file
    targetFD = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    // Can't open file
    if (targetFD == -1) { 
        printf("cannot open %s for output\n", outputFile);
        fflush(stdout);
        exit(1); 
    }

    result = dup2(targetFD, 1);
    // Failed redirect
    if (result == -1) { 
        perror("target dup2()");
        fflush(stdout);
        exit(2);
    }

    // Closing file
    fcntl(targetFD, F_SETFD, FD_CLOEXEC);   
}


/* 
 * Kills all currently running processes.
 * 
 * Sources: https://stackoverflow.com/questions/14558068/c-kill-all-processes
 **/
void terminateProcesses() {
    // Killing processes
    printf("Killing any running processes...\n");
    fflush(stdout);
    kill(0, SIGKILL);
}


/*
 * Ignores when user inputs ctrl-c.
 * 
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
 **/
void ignore_SIGINT() {
    // Ignoring ctrl-c
    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);
}


/*
 * Ignores when user inputs ctrl-z.
 * 
 * Source: https://canvas.oregonstate.edu/courses/1798831/pages/exploration-signal-handling-api?module_item_id=20163882
 **/
void redefine_SIGTSTP() {
    // Handling ctrl-z
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
}