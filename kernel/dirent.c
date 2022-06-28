/*
 *  linux/lib/dirent.c
 *
 *  (C) 2022  Healer
 */

#define __LIBRARY__

#include <unistd.h>
#include <linux/sched.h>
#include <sys/stat.h>
#include <string.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#define ENTRY_LENGTH 16
#define INODE_LENGTH 2
#define LONGLEN sizeof(long)
#define OFFSETLEN sizeof(off_t)
#define SHORTLEN  sizeof(unsigned short)
//#define FILENAME_LENGTH 14



//_syscall1(unsigned int,sleep,unsigned int,seconds);


//int sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count);

int sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
        
        struct m_inode * dir = current->filp[fd]->f_inode;
        if(current->filp[fd]->f_pos==dir->i_size)       
                return 0;
        long inode = dir->i_num;
        off_t start = current->filp[fd]->f_pos;
        ///unsigned int readnr = (count>(dir->i_size-start))?dir->i_size-start:count;
        //readnr = (readnr/16)*16;
        char *buf=(char *)malloc(sizeof(char)*count);
        struct linux_dirent *tmp = (struct linux_dirent *)buf;
        int min = LONGLEN+SHORTLEN+OFFSETLEN+1;
        int i=0;
        unsigned int rn=0;
        off_t offset=0;
        //int entryNum= count/sizeof(struct linux_dirent);
        int bytes=0;
        struct buffer_head * bh;
        off_t pos = current->filp[fd]->f_pos;
        int block=-1;
        while((char*)tmp-buf<=count-min)
        {
                
                if(pos>=dir->i_size)       
                        break;
                //char *buf = (char*)malloc(sizeof(char)*(ENTRY_LENGTH+1));
                
                int num=0;
                int x,y=0;
                int lnr = pos/BLOCK_SIZE;
                int nr;
                if(pos/1024!=block)
                {
                        /*nr = (lnr<7)?dir->i_zone[lnr]:0;
                        if(lnr >=7 && lnr <512)
                        {
                                lnr-=7;
                                bh = bread(dir->i_dev,dir->i_zone[7]);
                                nr = ((unsigned short *) (bh->b_data))[lnr];
                        }
                        else if(lnr>=512)
                        {
                                lnr-=512;
                                bh = bread(dir->i_dev,dir->i_zone[8]);
                                nr = ((unsigned short *)bh->b_data)[lnr>>9];
                        }*/
                        nr = bmap(current->filp[fd]->f_inode,pos/BLOCK_SIZE);
                        bh = bread(dir->i_dev,nr);
                        block = pos/1024;
                }       
                y=0;
                char * data = bh->b_data+ENTRY_LENGTH*i;
                if(*(unsigned short *)data==0)
                {
                        pos+=ENTRY_LENGTH;
                        i++;
                        continue;
                }        
                tmp->d_ino = *(unsigned short *)data;
                for(x=2;x<ENTRY_LENGTH;x++)
                {
                        if(data[x]==0)
                                break;
                        *((char *)(tmp)+min-1+y) = data[x];
                        y++;
                }
                *((char *)(tmp)+min-1+y) = '\0';
                tmp->d_reclen=y+min;
                tmp->d_off = min+y+(tmp-dirp);
                bytes += tmp->d_reclen;
                pos+=ENTRY_LENGTH;
                tmp=(struct linux_dirent *)((char *)tmp+tmp->d_reclen);
                i++;      
        }
        int len=(char*)tmp-buf;
        tmp = (struct linux_dirent*)buf;
        int xx=0;
        
        for(xx=0;xx<bytes;xx++)
        {
                put_fs_byte(*((char *)(tmp)+xx),(char *)(dirp)+xx);
        }
        return bytes;
}





