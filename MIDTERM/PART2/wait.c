// wait.c file

extern PROC *running;
extern PROC *sleepList;
extern PROC *readyQueue;

extern int int_on();
extern int int_off();
int kexit() // SIMPLE kexit() for process to terminate
{
  printf("proc %d exit\n", running->pid);
  running->status = ZOMBIE;
  tswitch();
}

int ksleep(int event)
{
  int SR = int_off();
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList,running);
  
  tswitch();
  int_on(SR);
}

int kwakeup(int event)
{

  int SR = int_off();
  PROC *p;
  PROC *temp = 0;
  
  while (p = dequeue(&sleepList))
  {
    if (p->status == SLEEP && p->event == event)
    {
      p->status = READY;
      enqueue(&readyQueue, p);
    }
    else{
      enqueue(&temp,p );
    }
  }
  sleepList = temp;
  int_on(SR);
}