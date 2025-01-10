#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"
#include <stdbool.h>
#include <pthread.h>

void scheduler();
void schedulerRR(int timeSlice, bool multithreading);
void schedulerAging();
int runinstruction(struct PCB* pcb);
extern pthread_t threads[2];
extern bool exit_flag;


#endif