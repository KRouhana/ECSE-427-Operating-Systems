#ifndef PCB_H
#define PCB_H

// Define the PCB structure
struct PCB {
    int PID;                     // Unique process ID
    int start_position;          // Start position of the script in shell memory
    int length;                  // Length of the script (number of characters or lines)
    int current_instruction;     // Current instruction being executed (program counter)
    char* scriptName;            // Name of the script file
    int score;             // Score based on length
};

// Function prototypes
struct PCB createPCB( char* scriptName, int length, int start_position);               // Function to create a new PCB
void updatePCB(struct PCB *pcb, int current_instruction);    // Function to update the current instruction
void updatePCBscore(struct PCB *pcb, int score);    // Function to update the score
void freePCB(struct PCB *pcb);                              // Function to free allocated resources for the PCB

#endif // PCB_H