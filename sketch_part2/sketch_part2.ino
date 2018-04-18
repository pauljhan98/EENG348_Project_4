
#include "concurrency.h"
// Pins for controlling the LEDs
int led1 = 9;
int led2 = 11;

lock_t pinlock;
lock_t pinlock2;

// p1, p2, p3 all share a single pin
// p4 does something else and doesn't need a lock



void p1 (void)
{
  /* process 1 here */
  // Process One turns on a single LED 
  while (1) {
  lock_acquire(&pinlock);
  digitalWrite(led1, HIGH);
  delay(100);
  digitalWrite(led1, LOW);
  delay(100);
  digitalWrite(led1, HIGH);
  delay(100);
  digitalWrite(led1, LOW);
  delay(100);
  lock_release(&pinlock);
  }
  
  return;
}

void p2 (void)
{
  /* process 2 here */
  // Process Two turns on a single LED
  while(1) {
  lock_acquire(&pinlock);
  digitalWrite(led1, HIGH);
  delay(500);
  digitalWrite(led1, LOW);
  delay(500);
  digitalWrite(led1, HIGH);
  delay(500);
  digitalWrite(led1, LOW);
  delay(500);
  digitalWrite(led1, HIGH);
  delay(500);
  digitalWrite(led1, LOW);
  delay(500);
  lock_release(&pinlock);
  
  }
  
 return;
}

void p3 (void)
{
  /* process 3 here */
  // Process three turns on a single LED
  while(1) {
  lock_acquire(&pinlock);
  digitalWrite(led1, HIGH);
  delay(1000);
  digitalWrite(led1, LOW);
  delay(1000);
  digitalWrite(led1, HIGH);
  delay(1000);
  digitalWrite(led1, LOW);
  delay(1000);
  digitalWrite(led1, HIGH);
  delay(1000);
  digitalWrite(led1, LOW);
  delay(1000);
  digitalWrite(led1, HIGH);
  delay(1000);
  digitalWrite(led1, LOW);
  delay(1000);
  lock_release(&pinlock);
  }
  
 return;
}

void p4 (void)
{
  /* process 4 here */
  // Process four turns on another single LED
  while(1) {
  digitalWrite(10, HIGH);
  delay(200);
  digitalWrite(10, LOW);
  delay(200);
  }
  
 return;
}

void p5 (void)
{
  /* process 1 here */
  // Process One turns on a single LED 
  while (1) {
  lock_acquire(&pinlock2);
  digitalWrite(led2, HIGH);
  delay(100);
  digitalWrite(led2, LOW);
  delay(100);
  digitalWrite(led2, HIGH);
  delay(100);
  digitalWrite(led2, LOW);
  delay(100);
  lock_release(&pinlock2);
  }
  
  return;
}

void p6 (void)
{
  /* process 3 here */
  // Process three turns on a single LED
  while(1) {
  lock_acquire(&pinlock2);
  digitalWrite(led2, HIGH);
  delay(1000);
  digitalWrite(led2, LOW);
  delay(1000);
  digitalWrite(led2, HIGH);
  delay(1000);
  digitalWrite(led2, LOW);
  delay(1000);
  digitalWrite(led2, HIGH);
  delay(1000);
  digitalWrite(led2, LOW);
  delay(1000);
  digitalWrite(led2, HIGH);
  delay(1000);
  digitalWrite(led2, LOW);
  delay(1000);
  lock_release(&pinlock2);
  }
  
 return;
}

void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  //pinMode(led2, OUTPUT); 
  lock_init(&pinlock); 
  lock_init(&pinlock2);
  
  if (process_create (p1, 32) < 0) {
    return;
  }
  if (process_create (p2, 32) < 0) {
    return;
  }
  if (process_create (p3, 32) < 0) {
    return;
  } 
  if (process_create (p4, 32) < 0) {
    return;
  }
 
  if (process_create (p5, 32) < 0) {
    return;
  }
  if (process_create (p6, 32) < 0) {
    return;
  } 


}

void loop()
{
  process_start();
  /* if you get here, then all processes are either finished or
     there is deadlock */
} 
