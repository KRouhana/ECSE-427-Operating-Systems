#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "shellmemory.h"
#include "shell.h"
#include <dirent.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "scheduler.h"
#include "pcb.h"
#include "queue.h"
#include <stdbool.h>
#include <pthread.h>
int MAX_ARGS_SIZE = 7;
bool multithreading = false;

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

int badcommandExec(){
    printf("Bad command: exec\n");
    return 1;
}

int badcommandSameProgExec(){
    printf("Bad command: exec, same programs detected\n");
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
int load_script_to_memory(char *script);
int exec(char* command_args[], int args_size);
int load_script_to_memory(char *script);
int pcb_length_comparator(const void *a, const void *b);

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
    
    } else if (strcmp(command_args[0], "exec") == 0) {        
        if (args_size < 3 || args_size > MAX_ARGS_SIZE) return badcommandExec();
        //Check if the same program is being executed
        if(args_size > 3){
            if(strcmp(command_args[1], command_args[2]) == 0 || strcmp(command_args[1], command_args[3]) == 0 || strcmp(command_args[2], command_args[3]) == 0) {
                return badcommandSameProgExec();
            } 
        }

        return exec(command_args, args_size);
    
    }
    
    else return badcommand();
}

int help() {

char help_string[] = "";
    // note the literal tab characters here for alignment
// 	char help_string[] = "COMMAND			DESCRIPTION\n \
// help			Displays all the commands\n \
// quit			Exits / terminates the shell with “Bye!”\n \
// set VAR STRING		Assigns a value to shell memory\n \
// print VAR		Displays the STRING assigned to VAR\n \
// run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
    printf("%s\n", help_string);
    return 0;
}

int quit() {
    printf("Bye!\n");
    //tried to have a flag to exit the threads but it was not working
    //exit_flag = true;

    //wait for the threads to finish
    if(multithreading){
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
    }
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
    
    int length = load_script_to_memory(script);
    struct PCB pcb = createPCB(script, length, 0);
    enqueue(pcb);

    // Run the scheduler
    scheduler();

    return 0;
}

int exec(char* command_args[], int args_size) {
    

    if(strcmp(command_args[args_size - 1], "MT") == 0){
        multithreading = true;
        args_size = args_size - 1;
    }

    if(strcmp(command_args[args_size - 1], "#") == 0){
        char line[MAX_USER_INPUT];
        char key[MAX_USER_INPUT + 10];
        args_size = args_size - 1;


        // Saves each line at key = Filename{i}, with i the line
        int i = 0;
        // Read each line from the script input
        while (!feof(stdin)) {
            
            fgets(line, MAX_USER_INPUT - 1, stdin);
            // Create a unique key for each line in the format "script_name{line_number}"
            sprintf(key, "%s%d", "#", i);
            // Store the line in shell memory with the created key
            mem_set_value(key, line);
            i++;

        }
        struct PCB pcb = createPCB("#", i, 0);
        enqueue(pcb);

    }




    if (strcmp(command_args[args_size - 1], "FCFS") == 0) {
        for (int i = 1; i < args_size - 1; i++) {
            
            int length = load_script_to_memory(command_args[i]);
            struct PCB pcb = createPCB(command_args[i], length, 0);
            enqueue(pcb);
        }
        // Run the scheduler
        scheduler();
    
    //SJF
    } else if (strcmp(command_args[args_size - 1], "SJF") == 0) {
        struct PCB pcb_list[args_size - 2];
        int pcb_count = 0;

        for (int i = 1; i < args_size - 1; i++) {
            int length = load_script_to_memory(command_args[i]);

            // Create PCB for script (name, length, start_position)
            struct PCB pcb = createPCB(command_args[i], length, 0);
            pcb_list[pcb_count++] = pcb;
        }

        // Sort the PCBs by the length of the script (Shortest Job First)
        qsort(pcb_list, pcb_count, sizeof(struct PCB), pcb_length_comparator);

        // Enqueue the sorted PCBs
        for (int i = 0; i < pcb_count; i++) {
            enqueue(pcb_list[i]);
        }

        // Run the scheduler
        scheduler();
    
    //RR
    } else if(strcmp(command_args[args_size - 1], "RR") == 0){

        for (int i = 1; i < args_size - 1; i++) {
            int length = load_script_to_memory(command_args[i]);
            struct PCB pcb = createPCB(command_args[i], length, 0);
            enqueue(pcb);
        }
        // Run the scheduler
        schedulerRR(2, multithreading);

    //AGING
    } else if(strcmp(command_args[args_size - 1], "AGING") == 0){
        struct PCB pcb_list[args_size - 2];
        int pcb_count = 0;

        for (int i = 1; i < args_size - 1; i++) {
            int length = load_script_to_memory(command_args[i]);

            // Create PCB for script (name, length, start_position)
            struct PCB pcb = createPCB(command_args[i], length, 0);
            pcb_list[pcb_count++] = pcb;
        }

        // Sort the PCBs by the length of the script (Shortest Job First)
        qsort(pcb_list, pcb_count, sizeof(struct PCB), pcb_length_comparator);

        // Enqueue the sorted PCBs
        for (int i = 0; i < pcb_count; i++) {
            enqueue(pcb_list[i]);
        }

        // Run the scheduler
        schedulerAging();
    
    
    //RR30
    } else if(strcmp(command_args[args_size - 1], "RR30") == 0){

        for (int i = 1; i < args_size - 1; i++) {
            int length = load_script_to_memory(command_args[i]);
            struct PCB pcb = createPCB(command_args[i], length, 0);
            enqueue(pcb);
        }
        // Run the scheduler
        schedulerRR(30, multithreading);

    }



    return 0;
}


//Function to load the script into shell memory
int load_script_to_memory(char *script) {
    
    
    char line[MAX_USER_INPUT];
    char key[MAX_USER_INPUT + 10];
    long script_length;

    FILE *p = fopen(script, "rt");  // the program is in a file

    if (p == NULL) {
        return badcommandFileDoesNotExist();
    }

    // Get the size of the file
    fseek(p, 0, SEEK_END);
    script_length = ftell(p);
    fseek(p, 0, SEEK_SET);

    // Saves each line at key = Filename{i}, with i the line
    int i = 0;
    // Read each line from the script file
    while (fgets(line, MAX_USER_INPUT - 1, p) != NULL) {
        // Create a unique key for each line in the format "script_name{line_number}"
        sprintf(key, "%s%d", script, i);
        // Store the line in shell memory with the created key
        mem_set_value(key, line);
        i++;
    }
    
    fclose(p);

    //return the length of the script
    return i;
}


// Comparator function to sort PCBs by script length, will be used by qsort
int pcb_length_comparator(const void *a, const void *b) {
    struct PCB *pcb_a = (struct PCB *)a;
    struct PCB *pcb_b = (struct PCB *)b;
    return pcb_a->length - pcb_b->length;
}

