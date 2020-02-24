// wait.c file

extern PROC *running;
extern PROC *sleepList;
extern PROC *readyQueue;

int kexit(int exitValue) // kexit() for process to terminate
{
  printf("\n\n***RUNNING KEXIT***\n");
  if (running->pid == 1) //Cannot kill P1
  {
    printf("proc 1 cannot be killed\n");
    return;
  }

  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  running->exitCode = exitValue;
  running->status = ZOMBIE;

  //Give away children to P1
  if (running->pid != 1)
    giveAwayChildren();

  //Wakeup parent proc using parent proc address
  kwakeup(running->parent);

  tswitch();
}

int ksleep(int event)
{
  printf("\n\n***RUNNING KSLEEP***\n");
  printf("proc %d going to sleep on event=%d\n", running->pid, event);

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
  printf("\n\n***RUNNING KWAKEUP***\n");
  PROC *temp, *p;
  temp = 0;

  int SR = int_off(); // disable IRQ and return CPSR

  while (p = dequeue(&sleepList))
  {
    if (p->status == SLEEP && p->event == event)
    {
      printf("wakeup %d\n", p->pid);
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

int kwait(int *status)
{
  printf("\n\n***RUNNING WAIT***\n");
  PROC* pCur = running->child;
  PROC* pPrev = 0;

  //If no child, return -1 as an ERROR
  if(running->child == 0)
    return -1;

  while(1)
  {

      for(pCur = running->child, pPrev = 0; pCur; pPrev = pCur, pCur = pCur->sibling)
      {
      if(pCur->status == 3)
      {
        *status = pCur->exitCode;

        //Adjust child and sibling pointers before freeing the process
        if(pPrev == 0) //Zombie is child pointer
          running->child = pCur->sibling;
        else
          pPrev->sibling = pCur->sibling;

        pCur->status = 0; //Change status to FREE

        enqueue(&freeList, pCur); //Add the process back to the free list

        return pCur->pid;

      }
    }

    ksleep(running);

  }
}

int do_wait()
{
  int status;
  int pid;
  pid =  kwait(&status);
  printf("proc %d buried a ZOMBIE Child %d with exit value %d\n", running->pid, pid, status);
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

// Gives away children of running process to P1
//Copied and adapted from my CPTS 360 Prelab 3
void giveAwayChildren()
{
  PROC *p1 = running;
  PROC *pCur = 0;

  while(p1->pid != 1) //Find pointer to p1 process
    p1 = p1->parent;

  if(running->child != 0) //There are children to give to p1
  {
    if(p1->child == 0) //p1 has no children
    {
      running->child = p1->child;
      pCur = p1->child;
    }
    else //p1 has children
    {
      pCur = p1->child;
      
      while(pCur->sibling != 0)
      {
        pCur = p1->sibling;
      }

      pCur->sibling = running->child;
      pCur = pCur->sibling;
    }

    while(pCur != 0)
    {
      pCur->parent = p1;
      pCur->ppid = p1->pid;
      pCur = pCur->sibling;
    }
  }
}