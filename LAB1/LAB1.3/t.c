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
char temp[64];
u32 *up;

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

//Searches the current global INODE PTR for a specific file name
//Returns a 1 if the things is found and a 0 otherwise
u16 search(char *name)
{
  char *cp;

  for (i = 0; i < 12; i++) //Assume DIR has at most 12 direct blocks
  {
    if(ip->i_block[i] == 0) //No data to read in
      break;

    getblk((u16)ip->i_block[i], buf1);
    dp = (DIR*)buf1;
    cp = buf1;

    while(cp < buf1 + BLK) //Loop through all entries in the directory
    {
      //Copy over the name
      strncpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;

      if(!strcmp(temp, name))
      {
        i = dp->inode;
        getblk(((i-1)/8 + iblk), buf1);
        ip = (INODE*)buf1 + ((i-1)%8);
        return 1;
      }

      //Move to next record
      cp += dp->rec_len;
      dp = (DIR *)cp;
      memset(temp, 0, 64);
    }
  }
  return 0; 
}



main()
{

  //Get block where inodes start
  getblk((u16)2, buf1);
  iblk = (((GD *)buf1)->bg_inode_table);
  getblk((u16)iblk, buf1);

  //Get root inode
  ip = (INODE *)buf1 + 1;

  // 1. Write YOUR C code to get the INODE of /boot/mtx
  //Look for /boot/mtx

  if(!search("boot"))
    error();

  if(!search("mtx"))
    error();

  prints("Found /boot/mtx\n\r");

  //if INODE has indirect blocks: get i_block[12] int buf2[  ]
  getblk((u16)ip->i_block[12], buf2);

  // 2. setes(0x1000); // MTX loading segment = 0x1000
  setes(0x1000);

  // 3. load 12 DIRECT blocks of INODE into memory
  for (i = 0; i < 12; i++)
  {
    getblk((u16)ip->i_block[i], 0); //Read in the block
    putc('*');
    inces(); //Increment the ES Register
  }
  prints("\n\r");

  // 4. load INDIRECT blocks, if any, into memory
  if (ip->i_block[12])
  {
    up = (u32 *)buf2; //Start at beginning of indirect block list
    while (*up) //Loop only while there are blocks to read in
    {
      getblk((u16)*up, 0); //Get the block of data
      putc('.');
      inces(); //Increment the ES Register
      (u16)up++; //Move to next block
    }
  }
  prints("\n\rgo?");
  getc();
}
