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


void userInput();
void createTokens(char *userInput);
void readArguments(char arguments[512][2048], int length, char *inputFile, char *outputFile);
char *expandVariable(char *variable);
void changeDirectory(char *path, int numArguments);
void findStatus(int status);
int executeOtherCommand(char arguments[512][2048], int length, int status, char *inputFile, char *outputFile);
void handle_SIGINT(int signal);
void handle_SIGTSTP(int signal);