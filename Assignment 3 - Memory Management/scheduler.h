#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"
#include <stdbool.h>
#include <pthread.h>

void scheduler();
void schedulerRR(int timeSlice, bool multithreading);
void schedulerAging();
int runinstructionNew(struct PCB* pcb, char *scriptName, int PCNumber);
void go_to_line_in_file(FILE *file, int line);

extern pthread_t threads[2];
extern bool exit_flag;


#endif