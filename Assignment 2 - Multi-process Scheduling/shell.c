#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"

int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {
    printf("Shell version 1.3 created September 2024\n");
    help();

    char prompt = '$';  				// Shell prompt
    char userInput[MAX_USER_INPUT];		// user's input stored here
    int errorCode = 0;					// zero means no error, default

    //init user input
    for (int i = 0; i < MAX_USER_INPUT; i++) {
        userInput[i] = '\0';
    }
    
    //init shell memory
    mem_init();
    while(1) {							
        //If input from file is over, switch to keyboard input
        if(feof(stdin)){
            freopen("/dev/tty", "r", stdin);
        } 

        //If input does not come from a file, print prompt						
        if(isatty(STDIN_FILENO)){
            printf("%c ",prompt);
        }
        
        // here you should check the unistd library 
        // so that you can find a way to not display $ in the batch mode
        fgets(userInput, MAX_USER_INPUT-1, stdin);
        errorCode = parseInput(userInput);
        if (errorCode == -1) exit(99);	// ignore all other errors
        memset(userInput, 0, sizeof(userInput));
    }

    return 0;
}


int wordEnding(char c) {
    // You may want to add ';' to this at some point,
    // or you may want to find a different way to implement chains.
    return c == '\0' || c == '\n' || c == ' ' || c == ';';
}

int parseInput(char inp[]) {
    
    char *commands[10]; // Array to hold up to 10 commands
    int numCommands = 0;
    char *token = strtok(inp, ";"); // Split input by semicolons
    int errorCode = 0;

    while (token != NULL && numCommands < 10) {
        commands[numCommands] = strdup(token); // Store each command
        numCommands++;
        token = strtok(NULL, ";");
    }
    
    for (int i = 0; i < numCommands; i++) {
        char *command = commands[i];
        char tmp[200], *words[100];                            
        int ix = 0, w = 0;
        int wordlen;

        for (ix = 0; command[ix] == ' ' && ix < 1000; ix++); // skip white spaces
        
        while (command[ix] != '\n' && command[ix] != '\0' && ix < 1000) {
            // extract a word
            for (wordlen = 0; !wordEnding(command[ix]) && ix < 1000; ix++, wordlen++) {
                tmp[wordlen] = command[ix];                        
            }
            tmp[wordlen] = '\0';
            words[w] = strdup(tmp);
            w++;
            if (command[ix] == '\0') break;
            ix++; 
        }
    
        errorCode = interpreter(words, w);
        if (errorCode != 0) {
            return errorCode;
        }
    }
    
    return errorCode;
}
