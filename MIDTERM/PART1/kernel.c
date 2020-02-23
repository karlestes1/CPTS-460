// kernel.c file

#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue;
PROC *sleepList;
int procsize = sizeof(PROC);
int body();
char *status[] = {"FREE", "READY", "SLEEP", "ZOMBIE"};

int init()
{
  int i, j;
  PROC *p;
  kprintf("kernel_init()\n");
  for (i = 0; i < NPROC; i++) //Initialization of the PROC lis
  {
    p = &proc[i];
    p->pid = i;
    p->status = FREE;
    p->next = p + 1;
  }
  proc[NPROC - 1].next = 0; // circular proc list

  freeList = &proc[0]; //Put all the PROCS in the free list
  readyQueue = 0;
  sleepList = 0;

  p = running = dequeue(&freeList); //Start with PROC[0]
  p->status = READY;
  p->priority = 0;
  p->ppid = 0; // P0 is its own parent

  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

int kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p == 0)
  {
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;

  // set kstack for new proc to resume to func()
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i = 1; i < 15; i++)
    p->kstack[SSIZE - i] = 0;
  p->kstack[SSIZE - 1] = (int)func; // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE - 14]);
  enqueue(&readyQueue, p);

  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p->pid;
}

int scheduler()
{
  //  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  //kprintf("next running = %d\n", running->pid);
  if (running->pid)
  {
    color = running->pid;
  }
}

// Copied from my own CPTS 360 Prelab 3
int ps()
{
  int i;
  PROC *p;
  printf("\nPID  PPID  status\n");
  printf("---  ----  ------\n ");
  for (i = 0; i < NPROC; i++)
  {
    p = &proc[i];
    printf(" %d    %d    ", p->pid, p->ppid);
    if (p == running)
      printf("RUNNING\n");
    else
      printf("%s\n", status[p->status]);
  }
}

int body()
{
  char c, cmd[64];

  kprintf("proc %d resume to body()\n", running->pid);
  while (1)
  {
    printf("-------- proc %d running -----------\n", running->pid);

    printList("freeList  ", freeList);
    printList("readyQueue", readyQueue);
    printsleepList(sleepList);

    printf("Enter a command [ps|switch|kfork|ksleep|kwakeup|exit] : ",
           running->pid);
    kgets(cmd);

    if (strcmp(cmd, "ps") == 0)
      ps();
    else if (strcmp(cmd, "switch") == 0)
      tswitch();
    else if (strcmp(cmd, "kfork") == 0)
      kfork((int)body, 1);
    else if (strcmp(cmd, "exit") == 0)
      kexit();
    else if (strcmp(cmd, "ksleep") == 0)
      do_sleep();
    else if (strcmp(cmd, "kwakeup") == 0)
      do_wakeup();
  }
}
