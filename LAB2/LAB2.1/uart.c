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
#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef volatile struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
  while (*(up->base + FR) & RXFE);
  return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + FR) & TXFF);
  *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}

//Taken from 360 Lab 1
void myrpu(UART *up, u32 x, int BASE) {
  char c;
  if(x) {
    c = tab[x % BASE];
    myrpu(up, x/BASE, BASE);
    uputc(up, c);
  }
}

int ufprintf(UART *up, char *fmt, ...)
{
  //Most of this code was taken from my personal 360 LAB 1 code
  char* ip = &fmt;
  int x;
  ip++;

  for(char*cp = fmt; *cp != '\0'; cp++)
  {
    if(*cp == '%') //Chracter to format
    {
      cp++;
      switch(*cp)
      {
        case 'c':
          uputc(up,*ip);
          break;

        case 's':
          uprints(up, *ip);
          break;

        case 'd':
          x = *ip;
          if(x < 0) {
            x = (~x) + 0b1;
            uputc(up, '-');
          }
          (x==0)? uputc(up, '0') : myrpu(up, x, 10);
          uputc(up, ' ');
          break;

        case 'x':
          x = *ip;
          uprints(up, "0x");
          (x == 0) ? uputc(up, '0') : myrpu(up, x, 16);
          uputc(up, ' ');
          break;
      }
      ip++;
    }
    else if(*cp == '\n')
    {
      uputc(up, *cp);
      uputc(up, '\r');
    }
    else
    {
      uputc(up, *cp);
    } 
  } 
}
