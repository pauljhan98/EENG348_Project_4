# EENG348_Project_4
Project 4 of EENG348 Concurrency 

PART 1

Part 1 implements concurrency without locks. The objective of this part is to simply allow multiple processes to run concurrently without sharing any sort of resources or variables. Concurrency is accomplished using a queue of processes. When the system initializes, we first build a queue of processes using process_create(). Process create initializes each appropriate struct in memory for a process, and then attaches it to a queue. Additional processes in the queue are linked to the process before it, i.e, we implement a linked list. We then schedule the processes in a "round robin" implementation. Each process is only allowed to run for a set amount of time. A timer interrupt sets the time slice. process_select monitiors current_process, which points to the current process that is executing. When current_process is interrupted, but not finished, it is linked to the end of the queue, and the head of the queue then becomes the current_process. If current_process terminates, it is not added back into the queue. process_select, more importantly, returns the stack pointer of the next current process, so that it can resume executing.

To test concurrency, several different processes for blinking seperate LEDs was established. Each LED was controlled by an independent process. Each process worked concurrently.

PART 2 

The code for this part of the lab is quite verbose due to debugging / ensuring correct pointer assignment. While in many places it isn't required, I have left it for the sake of clarity. 

Part 2 implements concurrency with locks. The objective of this part is to allow concurrency with the use of shared resources. A lock structure is implemented for each shared resource in the system. The lock contains a pointer to the process that currently has the lock, and a pointer to the head of the waiting lock queue. When a process aquires a lock, it is allowed to execute, and remain in the ready queue. If a process does not have a lock, the process is sent to a unique lock queue (each lock has its own queue) that is implemented using a linked list. When a process is sent to the appropriate lock waiting queue, yield is called to preform a context switch to another process in the ready queue. A "blocking" variable is raised such that the process just sent to the waiting queue does not also enter the ready queue - the ready queue and lock waiting queue are indepedent.

When a lock is let go, the first process in the lock waiting queue acquires it, and is sent to the end of the ready queue. 

To test concurrency, several locks were established for pins. Each lock controlled access to a specific pin. Several processes for each pin were then created. Access to each pin requires a lock, and no more than one process can hold a lock. Several other processes were created which did not require a lock, to ensure that seperate lock wait and ready queues work correctly. 
