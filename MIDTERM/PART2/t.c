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
int color;

#include "type.h"
#include "string.c"
#include "queue.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "kernel.c"
#include "wait.c"
#include "timer_queue.c"
#include "timer.c"


TIMER *tp[4];

void timer0_handler(){
   timer_handler(0);
}

int vectorInt_init() // Use vectored interrupts of PL190 VIC
{
   printf("vectorInterrupt_init()\n");
   /*********** set up vectored interrupts *****************
(1). write to vectoraddr0 (0x100) with ISR of timer0
vectoraddr1 (0x104) with ISR of UART0
vectoraddr2 (0x108) with ISR of UART1
vectoraddr3 (0x10C) with ISR of KBD
*********************************************************/
   *((int *)(VIC_BASE_ADDR + 0x100)) = (int)timer0_handler;
   //*((int *)(VIC_BASE_ADDR + 0x104)) = (int)uart0_handler;
   //*((int *)(VIC_BASE_ADDR + 0x108)) = (int)uart1_handler;
   *((int *)(VIC_BASE_ADDR + 0x10C)) = (int)kbd_handler;
   // (2). write to intControlRegs = E = 1 | IRQ # = 1xxxxx * 
   *((int *)(VIC_BASE_ADDR + 0x200)) = 0x24; //100100 at IRQ 4
   *((int *)(VIC_BASE_ADDR + 0x204)) = 0x2C;                           //101100 at IRQ 12
   *((int *)(VIC_BASE_ADDR + 0x208)) = 0x2D;                           //101101 at IRQ 13
   *((int *)(VIC_BASE_ADDR + 0x20C)) = 0x3F;                           //111111 at IRQ 31
   // (3). write 0's to IntSelectReg to generate IRQ interrupts
   //(any bit=1 generates FIQ interrupt)
   *((int *)(VIC_BASE_ADDR + 0x0C)) = 0;
}

void copy_vectors(void)
{
   extern u32 vectors_start;
   extern u32 vectors_end;
   u32 *vectors_src = &vectors_start;
   u32 *vectors_dst = (u32 *)0;

   while (vectors_src < &vectors_end)
      *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);

// void IRQ_handler(){
//    void *(*f)( );
//    // f as a function pointer
//    int status = *(int *)(VIC_BASE_ADDR+0x30);
//    f =(void *)*((int *)(VIC_BASE_ADDR+0x30));
//    f();
//    // call the ISR function
//    *((int *)(VIC_BASE_ADDR+0x30)) = 1; // write to vectorAddr as EOI
// }

// void IRQ_handler()
// {
//    int vicstatus, sicstatus;
//    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
//    sicstatus = SIC_STATUS;

//    if (vicstatus & (1 << 31))
//    {
//       if (sicstatus & (1 << 3))
//       {
//          kbd_handler();
//       }
//    }

//    // read VIC status registers to determine interrupt source
//    // VIC status BITs: timer0,1=4, uart0=13, uart1=14
//    if (vicstatus & (1 << 4))
//    {
//       // bit4=1:timer0,1
//       if (*(tp[0]->base + TVALUE) == 0) // timer 0
//          timer_handler(0);
//       if (*(tp[1]->base + TVALUE) == 0) // timer 1
//          timer_handler(1);
//    }
//    if (vicstatus & (1 << 5))
//    {
//       // bit5=1:timer2,3
//       if (*(tp[2]->base + TVALUE) == 0)
//          // timer 2
//          timer_handler(2);
//       if (*(tp[3]->base + TVALUE) == 0)
//          // timer 3
//          timer_handler(3);
//    }
// }

void IRQ_handler()
{
   int vicstatus, sicstatus;
   // read VIC SIV status registers to find out which interrupt
   vicstatus = VIC_STATUS;
   sicstatus = SIC_STATUS;
   if (vicstatus & (1 << 4)) // bit4: timer0
      timer_handler(0);
   if (vicstatus & (1 << 5)) // bit5: timer2
      timer_handler(2);
   // if (vicstatus & (1 << 12))
   //    // Bit12: uart0
   //    uart_handler(&uart[0]);
   // if (vicstatus & (1 << 13))
   //    // bit13: uart1
   //    uart_handler(&uart[1]);
   if (vicstatus & (1 << 31))
   { // SIC interrupts=bit_31=>KBD at bit 3
      if (sicstatus & (1 << 3))
         kbd_handler();
   }
}

int body();
int main()
{
   int i;
   char line[128];
   //   u8 kbdstatus, key, scode;

   color = WHITE;
   row = col = 0;
   
   fbuf_init();
   timer_init(); // timer2,3 at VIC.bit5
   kbd_init();
   vectorInt_init();
   kprintf("Welcome to WANIX in Arm\n");


   
   // timer_start(1);
   // timer_start(2);
   // timer_start(3);


   VIC_INTENABLE = 0;
   // allow KBD interrupts
   VIC_INTENABLE |= (1 << 31); // allow VIC IRQ31

   // enable KBD IRQ
   SIC_ENSET = 1 << 3;    // KBD int=3 on SIC
   SIC_PICENSET = 1 << 3; // KBD int=3 on SIC


   
   //VIC_INTENABLE = 0;
   VIC_INTENABLE |= (1 << 4);  // timer0,1 at bit4
   VIC_INTENABLE |= (1 << 12); // UART0 at bit12
   VIC_INTENABLE |= (1 << 13); // UART1 at bit13
   VIC_INTENABLE |= (1 << 31); // SIC to VIC's IRQ31
   

   timer_start(0);
   // timer_start(1);
   // timer_start(2);
   // timer_start(3);
   init();
   printf("P0 switch to P1\n");
   for(int i = 0; i < 4; i++){
      kfork((int)t_cmd, 1);
   }
   
   printList("readyQueue", readyQueue);
   
   while (1)
   {
      if (readyQueue)
          tswitch();
   }
}
