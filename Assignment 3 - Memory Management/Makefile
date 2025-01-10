CC=gcc
CFLAGS=

mysh: shell.c interpreter.c shellmemory.c pcb.c queue.c scheduler.c LRU_queue.c
	$(CC) $(CFLAGS) -D FRAME_MEM_SIZE=$(framesize) -D VAR_MEM_SIZE=$(varmemsize) -c shell.c interpreter.c shellmemory.c pcb.c queue.c scheduler.c LRU_queue.c
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o pcb.o queue.o scheduler.o LRU_queue.o

clean: 
	rm mysh; rm *.o
