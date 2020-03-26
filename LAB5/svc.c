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
#include "type.h"
#include "wait.c"

extern PROC proc[], *running;
extern int tswitch();
extern int kprintf(char *, ...);
extern int strcpy(char *dest, char *src);

int ktswitch()
{
  tswitch();
}

int kgetpid()
{
  return running->pid;
}

int kgetppid()
{
  return running->ppid;
}

char *pstatus[]={"FREE   ","READY  ","SLEEP  ","BLOCK  ","ZOMBIE", " RUN  "};
int kps()
{
  kprintf("\n\n***RUNNING PS***\n");
  int i;
  PROC *p;
  kprintf("\nPID  PPID  status       name\n");
  kprintf("---  ----  ------       ----\n");
  for (i = 0; i < NPROC; i++)
  {
    p = &proc[i];
    kprintf(" %d    %d    ", p->pid, p->ppid);
    if (p == running)
      kprintf("RUNNING    %s\n", p->name);
    else
      kprintf("%s    %s\n", pstatus[p->status], p->name);
  }
}

int kchname(char *s)
{ 
  if(!s)
    kprintf("No name provided\n");
  else
    strcpy(running->name, s);
}

int kgetPA()
{
  return (u32) ((char *)(0x800000 + (running->pid-1)*0x100000));
}


int svc_handler(int a, int b, int c, int d)
{
  int r;
  
  // printf("svc_handler: a=%d b=%d c=%d d=%d\n",a,b,c,d);

  switch(a){ // a is the call number
    
     case 0: r = kgetpid();          break;
     case 1: r = kgetppid();         break;
     case 2: r = kps();              break;
     case 3: r = kchname((char *)b); break;
     case 4: r = ktswitch();         break;
     case 5: r = ksleep(b);           break;
     case 6: r = kwakeup(b);          break;
     case 7: r = kfork((char*)b);      break;
     case 8: r = kexit(b);            break;
     case 9: r = kwait(b);            break;

     case 90: r = kgetc();           break;
     case 91: r = kputc(b);          break;
     case 92: r = kgetPA();          break;
     default: printf("invalid syscall %d\n", a);
  }

  return r; //return to goUmode in ts.s, which will replace r0 in Kstack with r
}

