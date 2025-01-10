#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "queue.h"
#include <stdbool.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
void insert_in_order(struct node* newPCB);


// Pointer to the head of the ready queue
struct node *head = NULL;

// Function to add a PCB to the ready queue
void enqueue(struct PCB process) {
    struct node *temp = (struct node*)malloc(sizeof(struct node));
    temp->process = process;
    temp->next = NULL;
    if (head == NULL) {
        head = temp;
    } else {
        struct node *temp1 = head;
        while (temp1->next != NULL) {
            temp1 = temp1->next;
        }
        temp1->next = temp;
    }
}

// Function to remove and return the PCB at the head of the ready queue
struct PCB dequeue() {
    if (head == NULL) {
        // If the queue is empty
        struct PCB emptyPCB;
        // Initialize emptyPCB with default values if necessary
        return emptyPCB;
    }
    struct node *temp = head;
    struct PCB process = (head->process);
    head = head->next;
    free(temp);
    return process;
}

// Function to age jobs in the ready queue
void ageJobsInQueue() {
    
    struct node *temp = head;
    while (temp != NULL) {
        
        temp->process.score = max(0, temp->process.score - 1);
        temp = temp->next;
    }
}


struct node* getHead() {
    return head;
}

void setHead(struct node* newHead) {
    head = newHead;
}

int emptyQueue() {
    return head == NULL;
}


void promote_jobs(struct PCB currentPCB, bool isFinished){

    struct node *temp = head;
    struct node *prev = NULL;
    struct node *lowest_score_node = head;

    // Step 2: Find the job with the lowest score in the queue
    while(temp != NULL && temp->process.score < lowest_score_node->process.score) {
        lowest_score_node = temp;
            // Track previous node for removal later
        prev = head;
        while(prev->next != lowest_score_node) {
            prev = prev->next;
        }
        temp = temp->next;
    }

    // Step 3: Promote job with lowest score to head of the queue if it's not already head
    if(lowest_score_node != head) {
        
        //Place process at the head of the queue
        prev->next = lowest_score_node->next;
        lowest_score_node->next = head;
        head = lowest_score_node;
    }

    // Step 4: If the current job is not finished, reinsert it back into the queue in sorted order
    if (!isFinished) {
        struct node *newPCB = (struct node*)malloc(sizeof(struct node));

        newPCB->process = currentPCB;
        insert_in_order(newPCB);  // Insert job back in queue based on score
    }

}


// Helper function to insert node in queue based on lengthScore order
void insert_in_order(struct node* newPCB) {

    if (head == NULL) {
        // Insert at the head if queue is empty
        head = newPCB;
        newPCB->next = NULL;
        return;
    }

    struct node *temp = head, *prev = NULL;


    // Find correct position based on lengthScore
    while (temp != NULL && newPCB->process.score > temp->process.score) {
        prev = temp;
        temp = temp->next;

    }

    if (prev == NULL) {
        // Insert at the head if newPCB has the lowest score
        newPCB->next = head;
        head = newPCB;
    } else{
        // Insert between prev and temp
        prev->next = newPCB;
        newPCB->next = temp;
    }
}

//For debugging purposes
void printQueue() {
    struct node *temp = head;
    while (temp != NULL) {
        printf("%s ", temp->process.scriptName);
        temp = temp->next;
    }
    printf("\n");
}