#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>


struct command {
    char *arguments[512];
    char *inputFile;
    char *outputFile;
    int background;
    int numArguments;
};


void userInput();
void createTokens(char *userInput);
void readArguments(struct command *userCommand);
char *expandVariable(char *variable);
void changeDirectory(char *path, int numArguments);
void findStatus(int status);
int executeOtherCommand(struct command *userCommand, int status);
void handle_SIGTSTP(int signal);