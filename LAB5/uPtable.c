#include "type.h"

int uPtable(PROC *p)
{
  int i;
  
  //Same format as mkptable from t.c
  //u32 *ut = (u32 *)0x400000; // at 4MB
  p->pgdir = (u32 *)(0x400000 + ((p->pid) * 0x4000));
  
  u32 entry = 0 | 0x412;     // 0x412;// AP=01 (Kmode R|W; Umode NO) domaian=0

  for (i = 0; i < 4096; i++) // clear 4k entries of table
    p->pgdir[i] = 0;

  // 128 MB RAM
  for (i = 0; i < 128; i++)
  {
    p->pgdir[i] = entry;
    entry += 0x100000;
  }
  // 2MB I/O space at 256 MB
  entry = 256 * 0x100000 | 0x412;
  for (i = 256; i < 258; i++)
  {
    p->pgdir[i] = entry;
    entry += 0x100000;
  }
  
  //User Mode code
  u32 addr = (char *)(0x800000 + (p->pid-1)*0x100000);
  p->pgdir[2048] = addr | 0xC32;
  p->usp = (int *)VA(0x100000); //High end of 1MB VA
  p->kstack[SSIZE-1] = VA(0); //Starting VA=0
}