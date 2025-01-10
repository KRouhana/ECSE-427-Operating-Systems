#include "queue.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"
#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include <unistd.h>
#include <stdbool.h>
#include "LRU_queue.h"

#define FRAME_SIZE 3

pthread_t threads[2];
pthread_mutex_t queuelock;
bool exit_flag = false;
struct LRUQueue lru_queue;


int runinstructionNew(struct PCB* pcb, char *scriptName, int PCNumber);
void *worker(void *arg);
void runPCBJobRR(struct PCB pcb, int timeSlice);
void go_to_line_in_file(FILE *file, int line);
void runPCB(struct PCB *pcb, int timeSlice);
int runinstruction(struct PCB* pcb);
static bool load_page_from_file(FILE *file, struct PCB *pcb, int frame_num, int current_line);
static void handle_page_fault(struct PCB *pcb, int current_instruction, int frame_num);
static void print_victim_contents(int frame_num);

//Scheduler used for FCFS and SJF
void scheduler() {
    //Get the first PCB in the ready queue
    while(!emptyQueue()) {

        struct PCB pcb = dequeue();

        //Run the instruction
        int newPC = -1;

        while(newPC != pcb.length) {
            newPC = runinstruction(&pcb);
            //Update the PCB
            updatePCB(&pcb, newPC);
        }

        //Remove the PCB from the ready queue
        freePCB(&pcb);

        //Free the shell memory
        mem_remove_value(pcb.scriptName, pcb.length);
    }

}

/**
 * A3-2024
 * Runs the instruction in the PCB
 */
int runinstructionNew(struct PCB* pcb, char *scriptName, int PCNumber) {
    // Calculate virtual address
    int virtualAddress = pcb->pagetable[PCNumber / FRAME_SIZE] * FRAME_SIZE + (PCNumber % FRAME_SIZE);
 
    // Check if page is in memory   
    if (pcb->pagetable[PCNumber / FRAME_SIZE] != -1) {
        // Update LRU queue
        move_page_to_back(pcb->pagetable[PCNumber / FRAME_SIZE], &lru_queue);

        // Get and execute instruction
        char* line = mem_get_value_at_index(virtualAddress);
        parseInput(line);
        return PCNumber + 1;
    }

    // Page fault
    return -1;
}

/**
 * A3-2024
 * Runs the PCB for the given time slice
 */
void runPCB(struct PCB *pcb, int timeSlice) {
    int current_instruction;  // Tracks current instruction being executed
    bool isFinished = false;  // Flag to indicate if process has completed

    // Execute instructions within time slice limit
    for (int i = 0; i < timeSlice && !isFinished; i++) {
        current_instruction = pcb->current_instruction;
        // Execute next instruction and get next program counter value
        int nextPC = runinstructionNew(pcb, pcb->scriptName, current_instruction);

        // Check if process has completed all instructions
        if (nextPC == pcb->length) {
            isFinished = true;
            break;
        }

        // Handle page fault
        if (nextPC == -1) {
            // Try to find an available frame
            int nextPage = getNextAvailablePage();
            
            if (nextPage != -1) {

                printf("Page fault!\n");
                handle_page_fault(pcb, current_instruction + 1, nextPage);
            } else {
                int evictedPage;
                if (dequeue_page(&lru_queue, &evictedPage)) {
                    print_victim_contents(evictedPage);
                    handle_page_fault(pcb, current_instruction + 1, evictedPage);
                }
            }
            // Re-queue PCB after page fault handling
            enqueue(*pcb);
            return;
        }

        // Update PCB with next instruction to execute
        updatePCB(pcb, nextPC);
    }
    
    // If process didn't finish in its time slice, put it back in ready queue
    if (!isFinished) {
        enqueue(*pcb);
    }
}

void schedulerRR(int timeSlice, bool multithreading) {
    if (!multithreading) {
        while (!emptyQueue()) {
            struct PCB pcb = dequeue();
            runPCB(&pcb, timeSlice);
        }
    } else {
        pthread_mutex_init(&queuelock, NULL);
        for (int i = 0; i < 2; i++) {
            pthread_create(&threads[i], NULL, worker, NULL);
        }
        for (int i = 0; i < 2; i++) {
            pthread_join(threads[i], NULL);
        }
        pthread_mutex_destroy(&queuelock);
    }
}

//Scheduler used for AGING
void schedulerAging() {
    while (!emptyQueue()) {
        // Dequeue the current PCB and mark it as unfinished initially
        struct PCB pcb = dequeue();
        bool isFinished = false;
        
        // Run one instruction and update the program counter
        int newPC = runinstruction(&pcb);
        updatePCB(&pcb, newPC);

        // Check if the process is completed
        if (newPC >= pcb.length) {
            // Free PCB's associated memory and mark it as finished
            freePCB(&pcb);
            mem_remove_value(pcb.scriptName, pcb.length);
            isFinished = true;
        }

        // Age jobs in the ready queue
        ageJobsInQueue();

        // Promote jobs based on their updated length scores
        // Promote jobs also checks if the current job is finished and will requeue the pcb in order
        promote_jobs(pcb, isFinished);

    
    }
}
//runs the instruction in the PCB
int runinstruction(struct PCB* pcb) {
    
    int PCNumber = pcb->current_instruction;
    char key[MAX_USER_INPUT + 10];

    //Get the current instruction from the PCB
    sprintf(key, "%s%d", pcb->scriptName, PCNumber);

    char* line = mem_get_value(key);
    //Run the instruction
    parseInput(line);

    return PCNumber + 1;
}

//Worker thread function for multithreading
 void *worker(void *arg) {
    //printf("Worker thread started\n");
    while (!exit_flag) {
        pthread_mutex_lock(&queuelock);
        if (!emptyQueue()) {
            struct PCB pcb = dequeue();
            pthread_mutex_unlock(&queuelock);

            // Process the PCB
            runPCBJobRR(pcb, 30);
        } else {
            pthread_mutex_unlock(&queuelock);
            usleep(1000);
            //printf("Worker thread exiting\n");
            return NULL;
        }
    }
    return NULL;
}

//Runs the PCB job for RR and RR30 (used for multithreading)
void runPCBJobRR(struct PCB pcb, int timeSlice) {
    // Run timeSlice instructions
    int newPC = pcb.current_instruction;

    for (int i = 0; i < timeSlice && newPC < pcb.length; i++) {
        newPC = runinstruction(&pcb);
        // Update the PCBcd
        updatePCB(&pcb, newPC);
    }

    // Lock the queue to modify it
    pthread_mutex_lock(&queuelock);

    // If the PCB has more instructions, re-enqueue it
    if (newPC < pcb.length) {
        enqueue(pcb);
    } else {
        // Free the PCB and shell memory if done
        freePCB(&pcb);
        mem_remove_value(pcb.scriptName, pcb.length);
    }

    pthread_mutex_unlock(&queuelock);
}

/**
 * A3-2024
 * Helper function to go to a specific line in a file
 */
void go_to_line_in_file(FILE *file, int line) {
    if (line <= 0) return;
    
    fseek(file, 0, SEEK_SET);
    int current_line = 1;
    char c;
    while (current_line < line && (c = fgetc(file)) != EOF) {
        if (c == '\n') current_line++;
    }
    
    if (current_line < line) {
        fseek(file, 0, SEEK_SET);
    }
    return;
}

/**
 * A3-2024
 * Helper function to load a page from a file into a frame
 */
static bool load_page_from_file(FILE *file, struct PCB *pcb, int frame_num, int current_line) {

    if (!file) return false;
    
    // Position file pointer at the correct line
    go_to_line_in_file(file, current_line);
    
    // Track if any lines were successfully loaded
    bool loaded = false;

    // Load FRAME_SIZE lines into memory frame
    for (int k = 0; k < FRAME_SIZE; k++) {
        char line[MAX_USER_INPUT];
        // Read line from file, break if EOF reached
        if (fgets(line, MAX_USER_INPUT-1, file) == NULL) {
            break;
        }
        // Store line in shell memory at correct frame offset
        mem_load_line(FRAME_SIZE * frame_num + k, pcb->scriptName, line);
        loaded = true;
    }

    return loaded;
}

/**
 * A3-2024
 * Helper function to handle a page fault
 */
static void handle_page_fault(struct PCB *pcb, int current_instruction, int frame_num) {
    // Construct path to process's backing store file
    char fileName[MAX_USER_INPUT];
    sprintf(fileName, "backing_store/%d", pcb->PID);
    FILE *file = fopen(fileName, "rt");
    
    // If file can't be opened, requeue PCB and return
    if (!file) {
        enqueue(*pcb);
        return;
    }

    // Try to load page from backing store file
    if (load_page_from_file(file, pcb, frame_num, current_instruction)) {
        // Update page table with new frame location
        pcb->pagetable[current_instruction / FRAME_SIZE] = frame_num;
        // Add frame to LRU queue for replacement tracking
        add_page_to_queue(frame_num, &lru_queue);
    }
    
    fclose(file);
}

/**
 * A3-2024
 * Helper function to print the contents of a victim page
 */
static void print_victim_contents(int frame_num) {
    printf("Page fault! Victim page contents:\n\n");
    for (int i = 0; i < FRAME_SIZE; i++) {
        char* content = mem_get_value_at_index(FRAME_SIZE * frame_num + i);
        if (content) printf("%s", content);
    }
    printf("\nEnd of victim page contents.\n");
}