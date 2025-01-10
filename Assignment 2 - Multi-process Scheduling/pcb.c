#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"

// Static variable for generating unique process IDs
static int next_pid = 1;
// Function to create a new PCB
struct PCB createPCB( char* scriptName, int length, int start_position) {
    struct PCB pcb;

    pcb.PID = next_pid++;
    pcb.start_position = start_position;
    pcb.length = length;
    pcb.current_instruction = 0;
    pcb.scriptName = scriptName;
    pcb.score = length;
    return pcb;
}

// Function to update the current instruction in the PCB
void updatePCB(struct PCB *pcb, int current_instruction) {
    if (pcb != NULL) {
        pcb->current_instruction = current_instruction;
    }
}

void updatePCBscore(struct PCB *pcb, int score) {
    if (pcb != NULL) {
        pcb->score = score;
    }
}

// Function to free the allocated memory for scriptName
void freePCB(struct PCB *pcb) {
    if (pcb != NULL && pcb->scriptName != NULL) {
        free(pcb->scriptName);
        pcb->scriptName = NULL;
    }
}