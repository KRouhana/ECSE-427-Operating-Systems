CC=gcc
CFLAGS=

mysh: shell.c interpreter.c shellmemory.c pcb.c queue.c scheduler.c
	$(CC) $(CFLAGS) -c shell.c interpreter.c shellmemory.c pcb.c queue.c scheduler.c
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o pcb.o queue.o scheduler.o

clean: 
	rm mysh; rm *.o
