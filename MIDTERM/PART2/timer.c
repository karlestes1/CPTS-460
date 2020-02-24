// Timer file for PART 2 of the CPTS 460 take home midterm
// Basic structure for a majority of this code was taken from
//"Embedded and Real-TIme Operating Systems" by K.C. Wang

#include "type.h"
#include "vid.c"
#include "kernel.c"

// timer register u32 offsets from base address
#define TLOAD 0x0
#define TVALUE 0x1
#define TCNTL 0x2
#define TINTCLR 0x3
#define TRIS 0x4
#define TMIS 0x5
#define TBGLOAD 0x6

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
    int i;
    TIMER *tp;
    printf("timer_init()\n");
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

void timer_stop(int n)
{
    // stop a timer
    TIMER *tp = &timer[n];
    *(tp->base + TCNTL) &= 0x7F; // clear enable bit 7
}
