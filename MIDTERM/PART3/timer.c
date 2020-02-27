#include "type.h"
// timer register u32 offsets from base address
#define TLOAD 0x0
#define TVALUE 0x1
#define TCNTL 0x2
#define TINTCLR 0x3
#define TRIS 0x4
#define TMIS 0x5
#define TBGLOAD 0x660 //3 Interrupts and Exceptions Processing typedef volatile struct

//#include "vid.c"

typedef unsigned int u32;

PROC *pauseList;

extern int color; // define this in your t.c file
extern char *tab; // define this in your t.c file: char *tab = "0123456789ABCDEF";

void memcpy(void *dest, void *src, int n)
{
    // Typecast src and dest addresses to (char *)
    char *csrc = (char *)src;
    char *cdest = (char *)dest;

    // Copy contents of src[] to dest[]
    for (int i = 0; i < n; i++)
        cdest[i] = csrc[i];
}

typedef volatile struct timer
{
    u32 *base;
    // timer's base address; as u32 pointer
    int tick, hh, mm, ss; // per timer data area
    char clock[16];
} TIMER;
volatile TIMER timer[4]; //4 timers; 2 per unit; at 0x00 and 0x20

void timer_init()
{
    int i;
    TIMER *tp;
    kprintf("timer_init()\n");
    for (i = 0; i < 4; i++)
    {
        tp = &timer[i];
        if (i == 0)
            tp->base = (u32 *)0x101E2000;
        if (i == 1)
            tp->base = (u32 *)0x101E2020;
        if (i == 2)
            tp->base = (u32 *)0x101E3000;
        if (i == 3)
            tp->base = (u32 *)0x101E3020;
        *(tp->base + TLOAD) = 0x0;
        // reset
        *(tp->base + TVALUE) = 0xFFFFFFFF;
        *(tp->base + TRIS) = 0x0;
        *(tp->base + TMIS) = 0x0;
        *(tp->base + TLOAD) = 0x100;
        // CntlReg=011-0010=|En|Pe|IntE|-|scal=01|32bit|0=wrap|=0x66
        *(tp->base + TCNTL) = 0x66;
        *(tp->base + TBGLOAD) = 0x1C00;          // timer counter value
        tp->tick = tp->hh = tp->mm = tp->ss = 0; // initialize wall clock
        strcpy((char *)tp->clock, "00:00:00");
    }
}

int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    *(tp->base + TINTCLR) = 0xFFFFFFFF;
}

void timer_handler(int n) // n=timer unit
{

    int i;
    TIMER *t = &timer[n];
    t->tick++;

    if (t->tick == 120)
    {
        t->tick = 0;
        t->ss++;
        if (t->ss == 60)
        {
            t->ss = 0;
            t->mm++;
            if (t->mm == 60)
            {
                t->mm = 0;
                t->hh++;
            }
        }
        if (n == 0)
        { // timer0: display wall-clock directly
            //printf("test");
            for (i = 0; i < 8; i++)
            {
                // clear old clock area
                //unkpchar(t->clock[i], n, 70 + i);
            }
            t->clock[7] = '0' + (t->ss % 10);
            t->clock[6] = '0' + (t->ss / 10);
            t->clock[4] = '0' + (t->mm % 10);
            t->clock[3] = '0' + (t->mm / 10);
            t->clock[1] = '0' + (t->hh % 10);
            t->clock[0] = '0' + (t->hh / 10);
            for (i = 0; i < 8; i++)
            {
                // display new wall clock
                kpchar(t->clock[i], n, 70 + i);
            }
        }

    }
    timer_clearInterrupt(n);
}

void timer_start(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    kprintf("timer_start %d base=%x\n", n, tp->base);
    *(tp->base + TCNTL) |= 0x80; // set enable bit 7
}

void timer_stop(int n)
{
    // stop a timer
    TIMER *tp = &timer[n];
    *(tp->base + TCNTL) &= 0x7F; // clear enable bit 7
}

// void t_cmd()
// {
//     kprintf("proc %d resume to body()\n", running->pid);
//     printList("readyQueue", readyQueue);
//     printf("\nEnter time to sleep:");
//     char timestr[5];
//     kgets(timestr);
//     int time = atoi(timestr);
//     pause(time);
// }

// int pause(int t)
// {
//     //printf("\nIn Pause\n");
//     lock();
//     // disable IRQ interrupts
//     running->pause = t;
//     running->status = PAUSE;

//     enqueueT(&pauseList, running, t);
//     tswitch();
//     unlock();
//     // enable IRQ interrupts
// }

// int tsleep(int event)
// {
//     int SR = int_off();
//     running->event = event;
//     running->status = SLEEP;
//     tswitch();
//     int_on(SR);
// }