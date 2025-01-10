#include "queue.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"
#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include <unistd.h>

pthread_t threads[2];
pthread_mutex_t queuelock;
bool exit_flag = false;

int runinstruction(struct PCB* pcb);
void *worker(void *arg);
void runPCBJobRR(struct PCB pcb, int timeSlice);

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

//Scheduler used for RR and RR30 and MT
void schedulerRR(int timeSlice, bool multithreading) {
    
    if(!multithreading){
            // Get the first PCB in the ready queue
        while (!emptyQueue()) {
            
            struct PCB pcb = dequeue();

            // Run timeslice instructions
            int newPC = pcb.current_instruction;

            for (int i = 0; i < timeSlice && newPC < pcb.length; i++) {
                newPC = runinstruction(&pcb);
                // Update the PCB
                updatePCB(&pcb, newPC);
            }

            // If the PCB has more instructions, re-enqueue it
            if (newPC < pcb.length) {
                enqueue(pcb);
            } else {
                // Free the PCB and shell memory if done
                freePCB(&pcb);
                mem_remove_value(pcb.scriptName, pcb.length);
            }
        }
    
    //We activate multithreading
    }else {
        // Initialize the mutex before creating threads
        pthread_mutex_init(&queuelock, NULL);

        // Create the worker threads
        for (int i = 0; i < 2; i++) {
            if (pthread_create(&threads[i], NULL, worker, NULL) != 0) {
                //fprintf(stderr, "Error creating thread %d\n", i);
            }
        }

        // Wait for all worker threads to finish
        for (int i = 0; i < 2; i++) {
            pthread_join(threads[i], NULL);
        }

        // Destroy the mutex after threads have completed
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

