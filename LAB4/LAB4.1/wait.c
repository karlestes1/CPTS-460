// wait.c file

extern PROC *running;
extern PROC *sleepList;
extern PROC *readyQueue;

int kexit() // SIMPLE kexit() for process to terminate
{
  printf("proc %d exit\n", running->pid);
  running->status = ZOMBIE;
  tswitch();
}

int ksleep(int event)
{
  int SR = int_off();

  // disable IRQ and return CPSR
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);

  tswitch();  // switch process
  int_on(SR); // restore original CPSR
}

int kwakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
  
  int SR = int_off(); // disable IRQ and return CPSR
  
  while (p = dequeue(&sleepList))
    {
      if (p->status == SLEEP && p->event == event)
      {
        p->status = READY;
        enqueue(&readyQueue, p);
      }
      else
      {
        enqueue(&temp, p);
      }
      
    }
    sleepList = temp;
  int_on(SR); // restore original CPSR
}

int do_sleep()
{
  int event;
  char input[16];
  
  //Get input value
  printf("\nEnter an event value to sleep on : ");
  kgets(input);
  printf("\n");
  
  //Convert to int
  event = atoi(input);
  
  //Call sleep function
  ksleep(event);
}

int do_wakeup()
{
  int event;
  char input[16];
  
  //Get input value
  printf("\nEnter an event value to wakeup with : ");
  kgets(input);
  printf("\n");
  
  //Convert to int
  event = atoi(input);
  
  //Call wakeup function
  kwakeup(event);
}