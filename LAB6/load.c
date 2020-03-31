
char *cp;
GD *gp;
INODE *ip;
DIR *dp;

u16 inode_start;

u16 ino;
u16 blknum; //= (ino-1) / 8 + inode_start;
u16 disp;   // = (ino-1) % 8;
u32 *up;
char   c, temp[64];


#define BLK 1024
char buf1[BLK], buf2[BLK];

u16 search(INODE *mip, char *name)
{
    for (int i = 0; i < 12; i++)
    { // search DIR direct blocks only
        if (mip->i_block[i] == 0)
        {
            printf("end of iblock");
            return 0;
        }

        getblock((u16)mip->i_block[i], buf1);
        dp = (DIR *)buf1;
        cp = buf1;
        while (cp < buf1 + BLK)
        {
            //memset(temp,0, dp->name_len);
            strcpy(temp, dp->name);
            temp[dp->name_len] = 0;
            if (strcmp(name, temp) == 0)
            {
                //printf("FOUND INODE\n\r");
                memset(temp, 0, dp->name_len);
                return dp->inode;
            }
            if (dp->rec_len == 0)
            {
                break;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
            memset(temp, 0, dp->name_len);
        }
    }
    return 0;
}

void getInode()
{
    blknum = (ino - 1) / 8 + inode_start;
    disp = (ino - 1) % 8;
    getblock(blknum, buf1);
    ip = (INODE *)buf1 + disp;
}

// must load filename to Umode image area at 8MB+(pid-1)*1MB
int load(char *filename, PROC *p)
{

    getblock(2, buf1);

    // 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
    gp = (GD*)buf1;
    inode_start = (u16)gp->bg_inode_table;

    ino = 2;
    //get INODE of ino to buf
    getInode();

    //ip for boot
    ino = search(ip, "bin");
    getInode();

    //ip for mtx
    ino = search(ip, filename);
    getInode();

    getblock((u16)ip->i_block[12], buf2);

    for (int i = 0; i < 12; i++)
    {
        
        if(ip->i_block[i]){
            getblock((u16)ip->i_block[i], (char *)(0x800000 + (p->pid-1)*0x100000 + i*BLK));
            
            printf("*");
        }
    }



    if (ip->i_block[12])
    {
        up = (u32 *)buf2;
        while (*up)
        {
            getblock((u32)*up, 0);
            printf(".");
            up++;
        }
    }
    
    return 1;
}