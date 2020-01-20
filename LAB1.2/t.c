/*******************************************************
*                      t.c file                        *
*******************************************************/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;
GD *gp;
INODE *ip;
DIR *dp;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

main()
{
  u16 i, iblk;
  char temp[64];
  char *cp;

  prints("read block# 2 (GD)\n\r");
  getblk((u16)2, buf1);

  // 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  gp = (GD *)buf1; //Cast to group descriptor
  iblk = (u16)(gp->bg_inode_table);

  prints("inode_block=");
  putc(iblk + '0');
  prints("\n\r");

  // 2. WRITE YOUR CODE to get root inode
  getblk(iblk, buf1); //Read in the first inode block

  prints("read inodes begin block to get root inode\n\r");
  ip = (INODE *)buf1 + 1; //ip points to the first inode

  // 3. WRITE YOUR CODE to step through the data block of root inode
  prints("read data block of root DIR\n\r");

  for (i = 0; i < 12; i++)
  {
    getblk((u16)ip->i_block[i], buf2);
    dp = (DIR *)buf2;
    cp = buf2;

    if (ip->i_block[i] != 0)
    {
      // 4. print file names in the root directory /
      while (cp < buf2 + BLK) //Loop through all entries in directory
      {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = '\0';

        if (temp[0] != NULL || temp == NULL) //Only print when there is a file name
        {
          prints(temp);
          prints("\n\r");
        }

        //Move to next record
        cp += dp->rec_len;
        dp = (DIR *)cp;
        memset(temp, 0, 64);
      }
    }
  }
}

int prints(char *s)
{
  while (*s)
  {
    putc(*s);
    s++;
  }
}

int gets(char *s)
{
  while ((*s = getc()) != '\r')
  {
    putc(*s);
    (s++);
    *s = 0;
  }
  *(s++) = 0;
}

int getblk(u16 blk, char *buf)
{
  // readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  readfd(blk / 18, ((blk) % 18) / 9, (((blk) % 18) % 9) << 1, buf);
}
