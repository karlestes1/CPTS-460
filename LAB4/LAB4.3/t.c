#include "type.h"
#include "defines.h"
#include "string.c"
#include "queue.c"
#include "vid.c"
#include "keymap"

#include "kbd.c"
#include "uart.c"
#include "pipe.c"
#include "exceptions.c"
#include "kernel.c"


#include "message.c"

PIPE *kpipe;

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

// IRQ interrupts handler entry point
void IRQ_handler()
{
    int vicstatus, sicstatus;

    // read VIC SIC status reg to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  

    if (vicstatus & (1<<31)){ // SIC interrupts=bit_31=>KBD at bit 3 
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}


int pipe_writer()
{
  char c, *cp; 
  struct uart *up = &uart[0];
  char line[128];
  int i;

  while(1){
    uprintf("Enter a line for task1 to get : ");
    ugets(up, line);
    uprints(up, "\r\n");
    printf("proc%d writes %d line=[%s] to pipe %d\n", running->pid, i, line);
    write_pipe(kpipe, line, strlen(line));
  }
  //kpipe->nwriter--;
  //uprintf("pipe writer proc %d exit\n", running->pid);
  //kexit();
}

int pipe_reader()
{
  char c, *cp; 
  char line[128];
  int  i, j, n;

  for (i=0; i<2; i++){
    printf("proc%d read from pipe %d\n", running->pid, i);
    n = read_pipe(kpipe, line, 20);
    printf("proc%d read n=%d bytes from pipe : [", running->pid, n);
    for (j=0; j<n; j++)
      kputc(line[j]);
    kprintf("]\n");
  }
  printf("pipe reader proc%d exit\n", running->pid);
  kpipe->nreader--;
  kexit();
}


int sender() // send task code
{
  struct uart *up = &uart[0];
  char line[128];
  while (1)
  {
    ugets(up, line);
    printf("task%d got a line=%s\n", running->pid, line);
    send(line, 4);
    printf("task%d send %s to pid=4\n", running->pid, line);
  }
}
int receiver() // receiver task code
{
  char line[128];
  int pid;
  while (1)
  {
    printf("task%d try to receive msg\n", running->pid);
    pid = recv(line);
    printf("task%d received: [%s] from task%d\n",
           running->pid, line, pid);
  }
}
int main()
{
  kprintf("DOES THIS WORK?");
  msg_init();
  kprintf("P0 kfork tasks\n");
  kfork((int)sender, 1);   // sender process
  tswitch();
  kfork((int)receiver, 1); // receiver process
  while (1)
  {
    if (readyQueue)
      tswitch();
  }
}