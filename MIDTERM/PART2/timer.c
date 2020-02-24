// Timer file for PART 2 of the CPTS 460 take home midterm
// Basic structure for a majority of this code was taken from
//"Embedded and Real-TIme Operating Systems" by K.C. Wang

#include "type.h"
#include "vid.c"
#include "kernel.c"

PROC *pauseList; // a list of pausing processes

typedef struct timer
{
    u32 *base;            // timer's base address;
    int tick, hh, mm, ss; // per timer data area
    char clock[16];
} TIMER;

TIMER timer[4]; // 4 timers;

//Intialization of the timers
void timer_init()
{
    printf("timer_init()\n");
    pauseList = 0;
    // pauseList initially 0
    // initialize all 4 timers
}

void timer_handler(int n) // n=timer unit
{
    int i;
    TIMER *t = &timer[n];
    t->tick++;
    t->ss = t->tick;
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

    if (n == 0) // timer0: display wall-clock directly
    {
        for (i = 0; i < 8; i++) // clear old clock area
            unkpchar(t->clock[i], n, 70 + i);

        t->clock[7] = '0' + (t->ss % 10);
        t->clock[6] = '0' + (t->ss / 10);
        t->clock[4] = '0' + (t->mm % 10);
        t->clock[3] = '0' + (t->mm / 10);
        t->clock[1] = '0' + (t->hh % 10);
        t->clock[0] = '0' + (t->hh / 10);

        for (i = 0; i < 8; i++) // display new wall clock
            kpchar(t->clock[i], n, 70 + i);
    }

    if (n == 2) // timer2: process PAUSed PROCs in pauseList
    { 
        PROC *p, *tempList = 0;
        while (p = dequeue(&pauseList))
        {
            p->pause--;
            if (p->pause == 0)
            { // pause time expired
                p->status = READY;
                enqueue(&readyQueue, p);
            }
            else
                enqueue(&tempList, p);
        }
        pauseList = tempList;
        // updated pauseList
    }
    timer_clearInterrupt(n);
}
int timer_start(int n)
{
    TIMER *tp = &timer[n];
    kprintf("timer_start %d base=%x\n", n, tp->base);
    *(tp->base + TCNTL) |= 0x80;
    // set enable bit 7
}
int timer_clearInterrupt(int n)
{
    TIMER *tp = &timer[n];
    *(tp->base + TINTCLR) = 0xFFFFFFFF;
}
