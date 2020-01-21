/*******************************************************
*                  @t.c file                          *
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

//Every variable is global because that's a great idea
u16 NSEC = 2;
char buf1[BLK], buf2[BLK];
u16 i, iblk;
char *cp;
char temp[64];
u32 *up, iblk2;

int prints(char *s)
{
  while (*s)
  {
    putc(*s);
    s++;
  }
}

// int gets(char *s)
// {
//   while ((*s = getc()) != '\r')
//   {
//     putc(*s);
//     (s++);
//     *s = 0;
//   }
//   *(s++) = 0;
// }

u16 getblk(u16 blk, char *buf)
{
  readfd((2 * blk) / CYL, ((2 * blk) % CYL) / TRK, ((2 * blk) % CYL) % TRK, buf);

  // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *name)
{
  for (i = 0; i < 12; i++)
  {
    getblk((u16)ip->i_block[i], buf2);
    dp = (DIR *)buf2;
    cp = buf2;

    if (!(ip->i_block[i]))
    {
      // 4. print file names in the root directory /
      while (cp < buf2 + BLK) //Loop through all entries in directory
      {
        //Copy over name
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = '\0';

        //Check if name of file matches search paramter
        if(strcmp(temp, name) == 0) //Found a match
        {
          return (u16)dp->inode - 1;
        }

        //Move to next record
        cp += dp->rec_len;
        dp = (DIR *)cp;
        memset(temp, 0, 64);
      }
    }
  } 

  return -1;
}

getInode()
{
  getblk((i/8) + iblk, buf1);
  ip = ((INODE*)buf1 + (i%8));
}

main()
{

  // 1. Write YOUR C code to get the INODE of /boot/mtx

  //Get the root inode
  getblk((u16)2, buf1);
  gp = (GD *)buf1; //Cast to group descriptor
  iblk = (u16)(gp->bg_inode_table);
  getblk(iblk, buf1); //Read in the first inode block
  ip = (INODE *)buf1 + 1; //ip points to the first inode

  //Search for boot
  i = search(ip, "boot");

  if(i == -1) error();

  getInode();

  i = search(ip, "mtx");

  if(i == -1) error();
  
  //if INODE has indirect blocks: get i_block[12] int buf2[  ]
  getInode();
  i = ip->i_block[12];
  getblk(i, buf2);
  iblk2 = buf2;

  // 2. setes(0x1000); // MTX loading segment = 0x1000
  setes(0x1000);



  // 3. load 12 DIRECT blocks of INODE into memory
  for (i = 0; i < 12; i++)
  {
    getblk((u16)ip->i_block[i], 0);
    putc('*');
    inces();
  }

  // // 4. load INDIRECT blocks, if any, into memory
  if (ip->i_block[12])
  {
    up = (u32 *)buf2;
    while (*up)
    {
      getblk((u16)*up, 0);
      putc('.');
      inces();
      up++;
    }
  }
  prints("go?");
  getc();
}
