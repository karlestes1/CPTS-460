// Uart file for PART 2 of the CPTS 460 take home midterm
// Basic structure for a majority of this code was taken from
//"Embedded and Real-TIme Operating Systems" by K.C. Wang

#include "type.h"

#define UDR 0x00
#define UDS 0x04
#define UFR 0x18
#define CNTL 0x2C
#define IMSC 0x38
#define MIS 0x40
#define SBUFSIZE 12870


typedef volatile struct uart
{
    char *base; // base address; as char *
    int n;
    // uart number 0-3
    char inbuf[SBUFSIZE];
    int indata, inroom, inhead, intail;
    char outbuf[SBUFSIZE];
    int outdata, outroom, outhead, outtail;
    volatile int txon; // 1=TX interrupt is on
} UART;

extern int do_rx(UART*);
extern int do_tx(UART*);
extern void lock();
extern void unlock();

UART uart[4]; // 4 UART structures
int uart_init()
{
    int i;
    UART *up;
    for (i = 0; i < 4; i++)
    { // uart0 to uart2 are adjacent
        up = &uart[i];
        up->base = (char *)(0x101F1000 + i * 0x1000);
        *(up->base + CNTL) &= ~0x10; // disable UART FIFO
        *(up->base + IMSC) |= 0x30;
        up->n = i; //UART ID number
        up->indata = up->inhead = up->intail = 0;
        up->inroom = SBUFSIZE;
        up->outdata = up->outhead = up->outtail = 0;
        up->outroom = SBUFSIZE;
        up->txon = 0;
    }
    uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

void uart_handler(UART *up)
{
    u8 mis = *(up->base + MIS); // read MIS register
    if (mis & (1 << 4))         // MIS.bit4=RX interrupt
        do_rx(up);
    if (mis & (1 << 5)) // MIS.bit5=TX interrupt
        do_tx(up);
}
int do_rx(UART *up) // RX interrupt handler
{
    char c;
    c = *(up->base + UDR);
    printf("rx interrupt: %c\n", c);
    if (c == 0xD)
        printf("\n");
    up->inbuf[up->inhead++] = c;
    up->inhead %= SBUFSIZE;
    up->indata++;
    up->inroom--;
}
int do_tx(UART *up) // TX interrupt handler
{
    char c;
    printf("TX interrupt\n");
    if (up->outdata <= 0)
    {                              // if outbuf[ ] is empty
        *(up->base + IMSC) = 0x10; // disable TX interrupt
        up->txon = 0;              // turn off txon flag
        return 0;
    }
    c = up->outbuf[up->outtail++];
    up->outtail %= SBUFSIZE;
    *(up->base + UDR) = (int)c; // write c to DR
    up->outdata--;
    up->outroom++;
}

int ugetc(UART *up)
// return a char from UART
{
    char c;
    while (up->indata <= 0)
        ; // loop until up->data > 0 READONLY
    c = up->inbuf[up->intail++];
    up->intail %= SBUFSIZE;
    // updating variables: must disable interrupts
    lock();
    up->indata--;
    up->inroom++;
    unlock();
    return c;
}
int uputc(UART *up, char c)
{
    // output a char to UART
    kprintf("uputc %c ", c);
    if (up->txon)
    { //if TX is on, enter c into outbuf[]
        up->outbuf[up->outhead++] = c;
        up->outhead %= 128;
        lock();
        up->outdata++;
        up->outroom--;
        unlock();
        return 0;
    }
    // txon==0 means TX is off => output c & enable TX interrupt
    // PL011 TX is riggered only if write char, else no TX interrupt
    int i = *(up->base + UFR);
    // read FR
    while (*(up->base + UFR) & 0x20)
        ; // loop while FR=TXF
    *(up->base + UDR) = (int)c;
    // write c to DR
    *(up->base + IMSC) |= 0x30;
    up->txon = 1;
}
int ugets(UART *up, char *s)
// get a line from UART
{
    kprintf("%s", "in ugets: ");
    while ((*s = (char)ugetc(up)) != '\r')
    {
        uputc(up, *s++);
    }
    *s = 0;
}
int uprints(UART *up, char *s)
{
    while (*s)
        uputc(up, *s++);
}