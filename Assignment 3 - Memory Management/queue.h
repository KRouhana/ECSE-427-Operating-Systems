#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>

#include "pcb.h"  // Include the PCB structure definition

// Pointer to track the head of the ready queue
struct node {
    struct PCB process;
    struct node *next;
};

// Adds a new PCB to the end of the ready queue
void enqueue(struct PCB new_pcb);

// Removes and returns the PCB at the front of the ready queue
struct PCB dequeue();

// Checks if the ready queue is empty
int emptyQueue();

struct node *getHead();

void ageJobsInQueue();
void setHead(struct node* newHead);
void promote_jobs(struct PCB currentPCB, bool isFinished);

// For debugging purposes
void printQueue();

#endif 