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

#include "defines.h"
#include "uart.c"
#include "vid.c"


typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

int color;
//char *tab = "0123456789ABCDEF";

// typedef struct uart{
//   char *base;
//   int  n;
// }UART;

// UART uart[4];

extern char _binary_wsu_bmp_start;

int color;
int SIZE;
UART *up;

int main()
{
  int x;

   char c, *p;
   int mode;
   uart_init();
   up = &uart[0];

   mode = 0;
   fbuf_init(mode);

   SIZE = 1;
   p = &_binary_wsu_bmp_start;
   show_bmp(p, 0, 0);

   while(1){
     SIZE = 2;
     uprintf(up, "enter a key from this UART : ");
     ugetc(up);
     p = &_binary_wsu_bmp_start;
     for (x=0; x<640*480; x++)
      fb[x] = 0x00000000;    // clean screen; all pixels are BLACK
     show_bmp(p, 0, 0);

     SIZE = 3;
     uprintf(up, "enter a key from this UART : ");
     ugetc(up);
     p = &_binary_wsu_bmp_start;
     for (x=0; x<640*480; x++)
      fb[x] = 0x00000000;    // clean screen; all pixels are BLACK
     show_bmp(p, 0, 0);

     SIZE = 4;
     uprintf(up, "enter a key from this UART : ");
     ugetc(up);
     p = &_binary_wsu_bmp_start;
     for (x=0; x<640*480; x++)
      fb[x] = 0x00000000;    // clean screen; all pixels are BLACK
     show_bmp(p, 0, 0);

     SIZE = 1;
     uprintf(up, "enter a key from this UART : ");
     ugetc(up);
     p = &_binary_wsu_bmp_start;
     for (x=0; x<640*480; x++)
      fb[x] = 0x00000000;    // clean screen; all pixels are BLACK
     show_bmp(p, 0, 0);
   }
   while(1);   // loop here  
}
