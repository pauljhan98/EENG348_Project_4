#include <stdio.h>
#include <avr/io.h>
#include "concurrency.h"

__attribute__((used)) unsigned char _orig_sp_hi, _orig_sp_lo;

__attribute__((used)) void process_begin ()
{
  asm volatile (
		"cli \n\t"
		"in r24,__SP_L__ \n\t"
		"sts _orig_sp_lo, r24\n\t"
		"in r25,__SP_H__ \n\t"
		"sts _orig_sp_hi, r25\n\t"
		"ldi r24, 0\n\t"
		"ldi r25, 0\n\t"
		"rjmp .dead_proc_entry\n\t"
		);
}

__attribute__((used)) void process_terminated ()
{
  asm volatile (
		"cli\n\t"
		"lds r25, _orig_sp_hi\n\t"
		"out __SP_H__, r25\n\t"
		"lds r24, _orig_sp_lo\n\t"
		"out __SP_L__, r24\n\t"
		"ldi r24, lo8(0)\n\t"
		"ldi r25, hi8(0)\n\t"
		"rjmp .dead_proc_entry"
		);
}

void process_timer_interrupt ();

__attribute__((used)) void yield ()
{
  if (!current_process) return;
  asm volatile ("cli\n\t");
  asm volatile ("rjmp process_timer_interrupt\n\t");
}

__attribute__((used)) void process_timer_interrupt()
{
  asm volatile (
		"push r31\n\t"
		"push r30\n\t"
		"push r29\n\t"
		"push r28\n\t"
		"push r27\n\t"
		"push r26\n\t"
		"push r25\n\t"
		"push r24\n\t"
		"push r23\n\t"
		"push r22\n\t"
		"push r21\n\t"
		"push r20\n\t"
		"push r19\n\t"
		"push r18\n\t"
		"push r17\n\t"
		"push r16\n\t"
		"push r15\n\t"
		"push r14\n\t"
		"push r13\n\t"
		"push r12\n\t"
		"push r11\n\t"
		"push r10\n\t"
		"push r9\n\t"
		"push r8\n\t"
		"push r7\n\t"
		"push r6\n\t"
		"push r5\n\t"
		"push r4\n\t"
		"push r3\n\t"
		"push r2\n\t"
		"push r1\n\t"
		"push r0\n\t"
		"in r24, __SREG__\n\t"
		"push r24\n\t"
		"in r24, __SP_L__\n\t"
		"in r25, __SP_H__\n\t"
		".dead_proc_entry:\n\t"
		"rcall process_select\n\t"
		"eor r18,r18\n\t"
		"or r18,r24\n\t"
		"or r18,r25\n\t"
		"brne .label_resume\n\t"
		"lds r25, _orig_sp_hi\n\t"
		"out __SP_H__, r25\n\t"
		"lds r24, _orig_sp_lo\n\t"
		"out __SP_L__, r24\n\t"
		"ret\n\t"
		".label_resume:\n\t"
		"out __SP_L__, r24\n\t"
		"out __SP_H__, r25\n\t"
		"pop r0\n\t"
		"out  __SREG__, r0\n\t"
		"pop r0\n\t"
		"pop r1\n\t"
		"pop r2\n\t"
		"pop r3\n\t"
		"pop r4\n\t"
		"pop r5\n\t"
		"pop r6\n\t"
		"pop r7\n\t"
		"pop r8\n\t"
		"pop r9\n\t"
		"pop r10\n\t"
		"pop r11\n\t"
		"pop r12\n\t"
		"pop r13\n\t"
		"pop r14\n\t"
		"pop r15\n\t"
		"pop r16\n\t"
		"pop r17\n\t"
		"pop r18\n\t"
		"pop r19\n\t"
		"pop r20\n\t"
		"pop r21\n\t"
		"pop r22\n\t"
		"pop r23\n\t"
		"pop r24\n\t"
		"pop r25\n\t"
		"pop r26\n\t"
		"pop r27\n\t"
		"pop r28\n\t"
		"pop r29\n\t"
		"pop r30\n\t"
		"pop r31\n\t"
		"reti\n\t");
}


/*
 * Stack: save 32 regs, +2 for entry point +2 for ret address
 */
#define EXTRA_SPACE 37
#define EXTRA_PAD 4




unsigned int process_init (void (*f) (void), int n)
{
  unsigned long stk;
  int i;
  unsigned char *stkspace;

  /* Create a new process */
  n += EXTRA_SPACE + EXTRA_PAD;
  stkspace = (unsigned char *) malloc (n);

  if (stkspace == NULL) {
    /* failed! */
    return 0;
  }

  /* Create the "standard" stack, including entry point */
  for (i=0; i < n; i++) {
      stkspace[i] = 0;
  }

  n -= EXTRA_PAD;

  stkspace[n-1] = ( (unsigned int) process_terminated ) & 0xff;
  stkspace[n-2] = ( (unsigned int) process_terminated ) >> 8;
  stkspace[n-3] = ( (unsigned int) f ) & 0xff;
  stkspace[n-4] = ( (unsigned int) f ) >> 8;

  /* SREG */
  stkspace[n-EXTRA_SPACE] = SREG;

  stk = (unsigned int)stkspace + n - EXTRA_SPACE - 1;

  return stk;
}

/************************** Part 2 Code *****************************************************/

process_t *queue;
process_t *current_process;

struct process_state
{
  // Value of the stack pointer
  unsigned int sp;
  struct process_state *next;
  int block;                        // 1 if blocks, 0 if not blocked
};



                                   // Updated process_select with checks for blocking / locks
                                   // If a process is blocked, it means it tried acquiring the lock, but it couldn't get it
                                   // It was thus sent to the lock queue. A blocked current process thus cannot be sent to
                                   // the end of the ready queue.
__attribute__((used)) unsigned int process_select (unsigned int cursp)
{
/* Called by the runtime system to select another process.
   "cursp" = the stack pointer for the currently running process
*/ 

 if (current_process->block == 1)
 {
  current_process->sp = cursp;      // Process is blocked, so we know it's waiting to acquire a lock in a lock queue
  current_process = queue;          // Set sp to cursp of the process, then switch to the next process in the ready queue
  queue = queue->next;
  current_process->next = NULL;
  return current_process->sp;
 }

 if (cursp == 0) {
  if (queue == NULL) {
                                    // No process has been created, or all processes have terminated
    return 0;
  }
  else {                           
                                    // We need to schedule the first process
                                    // Set the current process to the first value in the queue
                                    // Set queue to whatever is next in the queue
    current_process = queue;
    queue = queue->next;
    current_process->next = NULL;
    return current_process->sp;
  }
 }

 if (cursp != 0) {
  if (queue == NULL)
  {
                                    // We have a current process, but no queue
                                    // If this is the case, allow it to keep running
    return cursp;
  }
 }
 
                                    // Here, we know we have a queue, and current process. We want to find the 
                                    // end of the queue, and attach the current process to the end of it
                                    // and then we want to move the process 
                                    // that queue is pointing to to current process
                                    // and then have the tail of the queue point to
                                    // the current process. Set the new tail to point to null.
 process_t *process_track = queue;

  int flag = 1;
  
  while (flag) {
   if (process_track->next == NULL) {
                                        // We know that next is null, so process_track  
                                        // is pointing to the head
                                        // Set flag to 0, end while-loop
    flag = 0;
   }
   else {
    process_track = process_track->next;
   }   
  }
                                        // process_track holds the end of the queue
  
  current_process->sp = cursp;
  current_process->next = NULL;
  
  process_track->next = current_process;
  
  current_process = queue;
  queue = queue->next;
  //comment this out to get one led blinking
  return current_process->sp;
 
}

void process_start (void)
{
/* Starts up the concurrent execution */
 process_begin();
}

int process_create (void (*f)(void), int n)
{
  /* Create a new process */

  
  unsigned int stack_pointer = process_init(f, n);

  if (stack_pointer == 0) {                  // Check to see if there is memory for stack and malloc
    return -1;
  }

  process_t *new_process = malloc(sizeof(process_t));

  if (new_process == NULL) { // or, !new_process
    return -1;
  }
  
 new_process->sp = stack_pointer;
 new_process->next = NULL;                    // New process doesn't point to anything
 new_process->block = 0;                      // New process starts unblocked



 if (queue == NULL) {
                                              // Make the current process the head
                                              // Since there is currently nothing else in the queue
  queue = new_process;
 }
 else {
                                              // We know there is a queue
                                              // Find the last process to execute in the queue and 
                                              // link the new process to it
  process_t *process_track = queue;
  int flag = 1;
  
  while (flag) {   
   if (process_track->next == NULL) {
                                              // We know that next is null, so process_track  
                                              // is pointing to the head
                                              // Set flag to 0, end while-loop
    flag = 0;
   }
   else {
    process_track = process_track->next;
   }  
  }
                                              // Set the process at the end of the queue to point to the new process
    process_track->next = new_process;
 }
}

/************************** Lock related functions and code ****************************/

lock_init(lock_t *l) {
 l->current = NULL;                          // Pointer to current process that holds the lock
 l->locked = NULL;                           // Pointer to queue of locked processes
}




lock_acquire(lock_t *l) {

  asm volatile ("cli\n\t");                  // disable interrupts before checking lock aspects
  
  if (l->current == NULL) {                  // No process currently holds the lock
                                             // So allow the current process requesting the lock to aquire the lock
    l->current = current_process;
    l->current->block = 0;                   // Not nessecary in this logic implementation 
  }
  else if (l->current != current_process) {
                                            // Block the current process if it doesn't have the lock
                                            // and throw it on the lock waiting queue
     if (l->locked == NULL) {
                                            // Nothing is waiting for the lock
                                            // Add to the lock queue, make it the head
                                            // Make it point to null since there is nothing else in the queue
      current_process->block = 1;
      current_process->next = NULL;
      l->locked = current_process;
      
     }
     else {
                                            // Otherwise, there is a queue for the lock
                                            // Find the tail of the lock queue, and link the current process to it
      int flag = 1;
      process_t *process_track = l->locked;

      while (flag) {
        if (process_track->next == NULL) {
          flag = 0;                             // We found the tail; process_track holds the tail
        }
        else {
        process_track = process_track->next;    // Point to the next waiting process in the lock queue
        }
      }
                                                // Add the current process to the lock queue of that particular lock
      current_process->next = NULL; 
      process_track->next = current_process;
                                                // make current process to point to null
     }
                                                // Block the current process, and call yield() to go to next function
     current_process->block = 1;
   
     yield();    
  }
  asm volatile ("sei\n\t"); // re-enable interrupts after you yield

}

lock_release(lock_t *l) {
  asm volatile ("cli\n\t");                  // disable interrupts before checking lock aspects

 
  if (l->locked == NULL) {                   // No process is waiting for the lock, so
    l->current = NULL;                       // release the lock, set it to null and return
    l->current->block = 0;
    return;
  }

                                             // Otherwise, there is something in the lock queue
                                             // Let the first thing in the lock queue acquire the lock
                                             // and then send it to the end of the ready queue
  if (l->locked != NULL) {                    
    l->current = l->locked;
    l->current->block = 0;                   // unblock the process
    l->locked = l->locked->next;             // locked now points to whatever is next in the lock queue, or null
  }
                                             // l->current points to the process that now holds the lock
                                             // Find the end of the ready queue, and put the new process with the lock on the ready queue
                                             // If the ready queue is empty, make the process popping off the lock queue the head of the ready queue

  if (queue == NULL) {
                                              // Make the current process the head
                                              // Since there is currently nothing else in the queue
   queue = l->current;
   queue->block = 0;
   queue->next = NULL;
  }
 else {
                                              // If there is something in the queue, find the tail and attach the process there.
 process_t *process_track = queue;

  int flag = 1;
  
  while (flag) {
   if (process_track->next == NULL) {
                                        // We know that next is null, so process_track  
                                        // is pointing to the tail
                                        // Set flag to 0, end while-loop
    flag = 0;
   }
   else {
    process_track = process_track->next;
   }   
  }
                                        // process_track holds the end of the queue
                                        // set new end of the queue to be the process that
                                        // holds the lock, and make sure the new tail points to null
                                        // and that the process is unblocked
  l->current->block = 0;
  l->current->next = NULL;                                    
  process_track->next = l->current;
 }
 
  asm volatile ("sei\n\t");             // re-enable interrupts
}


