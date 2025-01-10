#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "shellmemory.h"
#include "shell.h"
#include <dirent.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int MAX_ARGS_SIZE = 7;

int badcommand(){
    printf("Unknown Command\n");
    return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
    printf("Bad command: File not found\n");
    return 3;
}

int badcommandTooManyTokens(){
    printf("Bad command: Too many tokens\n");
    return 1;
}

int badcommandMkdir(){
    printf("Bad command: my_mkdir\n");
    return 1;
}

int badcommandCd(){
    printf("Bad command: my_cd\n");
    return 1;
}

int help();
int quit();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();
int echo(char *var);
int my_ls();
int my_mkdir(char *dir);
int my_touch(char *filename);
int my_cd(char *dir);
int dir_entry_comparator(const void *a, const void *b);

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size) {
    int i;

    if (args_size < 1) {
        return badcommand();
    }

    if (args_size > MAX_ARGS_SIZE) {
        return badcommandTooManyTokens();
    }

    for (i = 0; i < args_size; i++) { // terminate args at newlines
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help") == 0){
        //help
        if (args_size != 1) return badcommand();
        return help();
    
    } else if (strcmp(command_args[0], "quit") == 0) {
        //quit
        if (args_size != 1) return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set") == 0) {
        
        // Calculate the maximum number of characters in the value string. Each token can have a max of 100 characters.
        int maxChars = MAX_TOKEN_SIZE * (args_size - 2) + 1;
        char value[maxChars];
        value[0] = '\0';
        for (int i = 2; i < args_size; i++){
            if(command_args[i] != NULL){
                strcat(value, command_args[i]);
                if (i < args_size - 1){
                    strcat(value, " ");
                }
            }
        }  
        // Variable is command_args[1], value is command_args[2-6]
        return set(command_args[1], value);
    
    } else if (strcmp(command_args[0], "echo") == 0) {
        if (args_size != 2) return badcommand();
        return echo(command_args[1]);
    
    } else if (strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1) return badcommand();
        return my_ls();
    
    } else if (strcmp(command_args[0], "my_mkdir") == 0) {
        if (args_size != 2) return badcommand();
        return my_mkdir(command_args[1]);

    } else if (strcmp(command_args[0], "my_touch") == 0) {
        if (args_size != 2) return badcommand();
        return my_touch(command_args[1]);

    } else if (strcmp(command_args[0], "my_cd") == 0) {
        if (args_size != 2) return badcommand();
        return my_cd(command_args[1]);

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2) return badcommand();
        return print(command_args[1]);
    
    } else if (strcmp(command_args[0], "run") == 0) {
        if (args_size != 2) return badcommand();
        return run(command_args[1]);
    
    } else return badcommand();
}

int help() {

    // note the literal tab characters here for alignment
	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
    printf("%s\n", help_string);
    return 0;
}

int quit() {
    printf("Bye!\n");
    exit(0);
}

int set(char *var, char *value) {

    char *link = "=";
    char buffer[MAX_USER_INPUT];
    strcpy(buffer, var);
    strcat(buffer, link);
    strcat(buffer, value);
    mem_set_value(var, value);

    return 0;
}

int echo(char *var) {

    char first_char = var[0];
    
    //User wants to check if variable exists
    if (first_char == '$') {
        
        char var_name[MAX_TOKEN_SIZE];
        strcpy(var_name, var+1);
        
        char *result = mem_get_value(var_name);
        
        //Variable does not exist
        if(strcmp(result, "Variable does not exist") == 0) {
            printf("\n");
        
        //Variable exists, print value
        } else {
            printf("%s\n", result);
        }
    } else {
        printf("%s\n", (var));
    }

    return 0;
}

int my_ls() {
    DIR *d;
    struct dirent *dir;
    int count = 0;

    d = opendir("."); // Open the current directory
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                count++;
            }
        }
    }
    closedir(d); // Close the directory
        
    char *entries[count];
    int i = 0;
    
    d = opendir("."); // Open the current directory
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                entries[i] = strdup(dir->d_name); // Copy the name
                i++;
            }
        }
    }
    closedir(d); // Close the directory

        // Sort the entries
    qsort(entries, count, sizeof(char *), dir_entry_comparator);

        // Print sorted entries
    for (int i = 0; i < count; i++) {
        printf("%s\n", entries[i]);
        free(entries[i]); // Free the duplicated string
    }
    return 0;
}

int my_mkdir(char *dir) {
    
    char first_char = dir[0];

    if (first_char == '$') {
        char dir_name[MAX_TOKEN_SIZE];
        strcpy(dir_name, dir+1);
        
        char *result = mem_get_value(dir_name);
        
        //Variable does not exist or has spaces
        if(strstr(result, " ") || strcmp(result, "Variable does not exist") == 0) {
            return badcommandMkdir();
        
        //Variable exists, create directory with read/write/execute permissions for all
        } else {
            mkdir(result,0777);
        }
    //Make directory with user inputted name
    } else {
        mkdir(dir,0777);
    }
    return 0;
}

int my_touch(char *filename) {
    // Create file with read/write permissions
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644); 
    close(fd); 
    return 0;
}

int my_cd(char *dir) {
    //Change directory to user inputted directory
    if (chdir(dir) != 0) {
        return badcommandCd();
    }
    return 0;
}

int dir_entry_comparator(const void *a, const void *b) {
    return strcmp(*(const char **) a, *(const char **) b);
}

int print(char *var) {
    printf("%s\n", mem_get_value(var)); 
    return 0;
}

int run(char *script) {
    int errCode = 0;
    char line[MAX_USER_INPUT];
    FILE *p = fopen(script, "rt");  // the program is in a file

    if (p == NULL) {
        return badcommandFileDoesNotExist();
    }

    fgets(line, MAX_USER_INPUT-1, p);
    while (1) {
        errCode = parseInput(line);	// which calls interpreter()
        memset(line, 0, sizeof(line));

        if (feof(p)) {
            break;
        }
        fgets(line, MAX_USER_INPUT-1, p);
    }

    fclose(p);

    return errCode;
}
