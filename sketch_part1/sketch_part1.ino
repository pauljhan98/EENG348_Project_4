#include "concurrency.h"
// Pins for controlling the LEDs
int led1 = 9;
int led2 = 10;
int led3 = 11;

void p1 (void)
{
  /* process 1 here */
  // Process One turns on a single LED 
  while (1) {
  digitalWrite(led1, HIGH);
  delay(200);
  digitalWrite(led1, LOW);
  delay(200);
  }
  
  return;
}

void p2 (void)
{
  /* process 2 here */
  // Process Two turns on another, single LED
  while(1) {
  digitalWrite(led2, HIGH);
  delay(500);
  digitalWrite(led2, LOW);
  delay(500);
  }
  
 return;
}

void p3 (void)
{
  /* process 3 here */
  // Process three turns on another, single LED
  while(1) {
  digitalWrite(led3, HIGH);
  delay(100);
  digitalWrite(led3, LOW);
  delay(100);
  }
  
 return;
}

void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT); 
  pinMode(led3, OUTPUT); 
  
  if (process_create (p1, 32) < 0) {
    return;
  }
  if (process_create (p2, 32) < 0) {
    return;
  }
  if (process_create (p3, 32) < 0) {
    return;
  }
}

void loop()
{
  process_start();
  /* if you get here, then all processes are either finished or
     there is deadlock */
}
