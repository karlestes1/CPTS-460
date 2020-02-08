/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf
 
typedef struct proc{
  struct proc *next;
  int    *ksp;

  int    pid;
  int    ppid;
  int    priority;
  int    status;
  int    event;
  int    exitCode;

  struct proc *parent;
  struct proc *child;
  struct proc *sibling;

  int    kstack[SSIZE];
}PROC;
***************************/
#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue;
PROC *sleepList;
int procsize = sizeof(PROC);
int body();

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;
  
  p = dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0;
  running = p;

  // P0 has no parent, no sibling
  p->parent = 0;
  p->sibling = 0;
  p->child = 0;  // P0 has no child YET

  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

  
PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;
  
  /********** TO DO **********
  implement the process family tree as a BINARY tree 
  using child, sibling pointers
  ***************************/
  // set kstack for new proc to resume to func()
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);
  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p;
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);

  if (color==WHITE)
    color = GREEN;
  else
    color = WHITE;
}  

int body()
{
  char c, cmd[64];

  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    printf("-------- proc %d running -----------\n", running->pid);
    
    printList("freeList  ", freeList);
    printList("readyQueue", readyQueue);
    printf("Enter a command [switch|kfork|exit|sleep|wakeup|wait] : ",
	   running->pid);

    kgets(cmd);
    printf("cmd = %s\n", cmd);

    if (strcmp(cmd, "switch")==0)
      tswitch();
    else if (strcmp(cmd, "kfork")==0)
      kfork((int)body, 1);

    else if (strcmp(cmd, "exit")==0){
      if (running->pid == 1){
	printf("P1 never die\n");
	continue;
      }
      printf("enter an exit value : ");
      int value = geti();	
      kexit(value);
    }
    else if (strcmp(cmd, "sleep")==0){
      if (running->pid == 1){
	printf("P1 never sleep\n");
	continue;
      }
      printf("enter a value to sleep : ");
      int value = geti();	
      ksleep(value);
    }
    else if (strcmp(cmd, "wakeup")==0){
      printf("enter a value to wakeup : ");
      int value = geti();	
      kwakeup(value);
    }
    else if (strcmp(cmd, "wait")==0){
      kwait();
    }
  }
}
