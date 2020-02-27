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

// queue.c file
//extern PROC *freeList;
extern PROC *readyQueue;
extern int ps();

int enqueue(PROC **queue, PROC *p)
{
  PROC *q = *queue;
  if (q == 0 || p->priority > q->priority)
  {
    *queue = p;
    p->next = q;
    return 0;
  }
  while (q->next && p->priority <= q->next->priority)
  {
    q = q->next;
  }
  p->next = q->next;
  q->next = p;
}

PROC *dequeue(PROC **queue)
{

  PROC *p = *queue;
  if (p)
    *queue = p->next;
  return p;
}

int printList(char *name, PROC *p)
{
  printf("%s=", name);
  while (p)
  {
    printf("[%d%d]->", p->pid, p->priority);
    p = p->next;
  }
  printf("NULL\n");
  return 0;
}

int printsleepList(PROC *p)
{
  printf("sleepList=");
  while (p)
  {
    printf("[%devent=%x]->", p->pid, p->event);
    p = p->next;
  }
  printf("NULL\n");
  return 0;
}

int enqueueT(PROC **queue, PROC *p, int seconds)
{
  PROC *q = *queue, *pPrev = 0;

  //queue is empty
  if (q == 0)
  {
    *queue = p;
    p->next = 0;
    p->pause = seconds;
    //ksleep(p);
    return 0;
  }

  //need to insert at front of list
  if (seconds < q->pause)
  {
    p->next = q;
    p->pause = seconds;
    q->pause = q->pause - seconds;
    *queue = p;
    //ksleep(p);
    return 0;
  }

  int global_seconds = q->pause;

  printf("General insert");

  // Locate PROC before point of insertion
  while (q->next != 0 && seconds < q->pause)
    q = q->next;

  p->next = q->next;
  q->next = p;

  p->pause = seconds - global_seconds;

  if (p->next)
    p->next->pause = p->next->pause - p->pause;

  //ksleep(p);
  return 0;
}

PROC *dequeueT(PROC **queue)
{
  PROC *p = *queue;

  int SR = int_off(); // disable IRQ and return CPSR

  if (p)
  {
    printf("wakeup %d\n", p->pid);
    p->status = READY;
    *queue = p->next;
    enqueue(&readyQueue, p);
  }
  int_on(SR); // restore original CPSR

  ps();
  printList("readyQueue", readyQueue);

  return p;
}

int printTimerQueue(PROC *p)
{
  printf("timerqueue=");
  while (p)
  {
    printf("[%d %d]->", p->pid, p->pause);
    p = p->next;
  }
  printf("NULL\n");
  return 0;
}
