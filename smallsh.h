#ifndef SMALLSH_H
#define SMALLSH_H

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


// Constant values
#define CD "cd"
#define STATUS "status"
#define EXIT "exit"


// Struct for an inputted command
struct command {
    char *arguments[513];
    char *inputFile;
    char *outputFile;
    int background;
    int numArguments;
};


char *userInput(char *input);
struct command *createTokens(char *userInput);
void readArguments(struct command *userCommand);
char *expandVariable(char *variable);
void changeDirectory(char *path, int numArguments);
void findStatus(int status);
int executeOtherCommand(struct command *userCommand, int status);
void handle_SIGTSTP(int signal);
void redirectInput(char *inputFile);
void redirectOutput(char *outputFile);
void terminateProcesses();
void ignore_SIGINT();
void redefine_SIGTSTP();


#endif