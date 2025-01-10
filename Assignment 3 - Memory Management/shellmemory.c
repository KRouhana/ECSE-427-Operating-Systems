#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shellmemory.h"
#include "shell.h"
#include "pcb.h"

/**
 * The memory layout is now:
 * Frame store: 0 to getFrameSize()-1
 * Variable store: MEM_SIZE - getVarSize() to MEM_SIZE-1
 */


struct memory_struct {
    char *var;
    char *value;
};
int getNextAvailablePage();
void mem_load_line(int index, char *var_in, char *value_in);
void mem_remove_value(char *scriptName, int length);
char *mem_get_value_at_index(int index);
char *mem_get_value(char *var_in);

struct memory_struct shellmemory[MEM_SIZE];

// Helper functions
int match(char *model, char *var) {
    int i, len = strlen(var), matchCount = 0;
    for (i = 0; i < len; i++) {
        if (model[i] == var[i]) matchCount++;
    }
    if (matchCount == len) {
        return 1;
    } else return 0;
}

// Shell memory functions

void mem_init(){
    int i;
    for (i = 0; i < MEM_SIZE; i++){		
        shellmemory[i].var   = "none";
        shellmemory[i].value = "none";
    }
}

//OLD: Set key value pair
/**
 *  
void mem_set_value(char *var_in, char *value_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, var_in) == 0){
            shellmemory[i].value = strdup(value_in);
            return;
        } 
    }

    //Value does not exist, need to find a free spot.
    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, "none") == 0){
            shellmemory[i].var   = strdup(var_in);
            shellmemory[i].value = strdup(value_in);
            return;
        } 
    }

    return;
}
 */


//OLD: get value based on input key
/**
 * char *mem_get_value(char *var_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, var_in) == 0){
            return strdup(shellmemory[i].value);
        } 
    }
    return "Variable does not exist";
}
 */


// Modified to handle variable store at high addresses
void mem_set_value(char *var_in, char *value_in) {
    int i;
    // Search in variable store section for existing variable
    for (i = 1; i < getVarSize(); i++) {
        if (strcmp(shellmemory[MEM_SIZE - i].var, var_in) == 0) {
            shellmemory[MEM_SIZE - i].value = strdup(value_in);
            return;
        }
    }

    // Find free spot in variable store if not found
    for (i = 1; i < getVarSize(); i++) {
        if (strcmp(shellmemory[MEM_SIZE - i].var, "none") == 0) {
            shellmemory[MEM_SIZE - i].var = strdup(var_in);
            shellmemory[MEM_SIZE - i].value = strdup(value_in);
            return;
        }
    }
}

// Modified to search in variable store section
char *mem_get_value(char *var_in) {
    int i;
    for (i = 1; i < getVarSize(); i++) {
        if (strcmp(shellmemory[MEM_SIZE - i].var, var_in) == 0) {
            return strdup(shellmemory[MEM_SIZE - i].value);
        }
    }
    return NULL;
}

// Remove key value pair
void mem_remove_value(char *scriptName, int length) {
	char key[MAX_USER_INPUT + 10];
    for (int i = 0; i < length; i++){		
		sprintf(key, "%s%d", scriptName, i);
		mem_set_value(key, "none");
	}
}

// Frame store operations
char *mem_get_value_at_index(int index) {
    if (index < 0 || index >= MEM_SIZE) {
        return NULL;
    }
    return strdup(shellmemory[index].value);
}

void mem_load_line(int index, char *var_in, char *value_in) {
    shellmemory[index].var = strdup(var_in);
    shellmemory[index].value = strdup(value_in);
}

// Frame management functions
int getNextAvailablePage() {
    for (int i = 0; i < getFrameSize() / FRAME_SIZE; i++) {
        if (strcmp(shellmemory[FRAME_SIZE * i].var, "none") == 0) {
            return i;
        }
    }
    return -1;
}
