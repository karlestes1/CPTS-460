// wait.c file
extern PROC * running;

int kexit(int exitValue)
{
  printf("proc %d in kexit\n", running->pid);

  running->exitCode = exitValue;
  running->status = ZOMBIE;

  // give up children to P1
  // wakeup parent;
  
  tswitch();
}

int ksleep(int event)
{
  printf("ksleep : YOU implement it\n");
}

int kwakeup(int event)
{
   printf("kakeup : YOU implement it\n");
}

int kwait()
{
   printf("kwait : YOU implement it\n");
}
