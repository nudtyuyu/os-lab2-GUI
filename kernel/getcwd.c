/*
 *  linux/lib/getcwd.c
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
#include <linux/fs.h>
#include <fcntl.h>
#include <const.h>
#include <errno.h>
#include <linux/tty.h>
#define NAMEMAX 50
#define ACC_MODE(x) ("\004\002\006\377"[(x)&O_ACCMODE])

/*
 * comment out this line if you want names > NAME_LEN chars to be
 * truncated. Else they will be disallowed.
 */
/* #define NO_TRUNCATE */

#define MAY_EXEC 1
#define MAY_WRITE 2
#define MAY_READ 4
#define ENTRY_LENGTH 16
#define INODE_LENGTH 2
#define LONGLEN sizeof(long)
#define OFFSETLEN sizeof(off_t)
#define SHORTLEN  sizeof(unsigned short)

static struct m_inode * get_dir(const char * pathname);
static struct m_inode * dir_namei(const char * pathname,int * namelen, const char ** name);
static int permission(struct m_inode * inode,int mask);
static struct buffer_head * find_entry(struct m_inode ** dir,const char * name, int namelen, struct dir_entry ** res_dir);
static int match(int len,const char * name,struct dir_entry * de);
static struct m_inode * my_namei(const char * pathname);
static int my_getdents(struct m_inode *dir, struct linux_dirent *dirp, unsigned int count);
static struct m_inode * find_inodep(short id);
static struct buffer_head * add_entry(struct m_inode * dir,const char * name, int namelen, struct dir_entry ** res_dir);
static int my_open(const char * filename,int flag,int mode);
static int my_open_namei(const char * pathname, int flag, int mode,struct m_inode ** res_inode);
static int my_chdir(const char * filename);



long sys_getcwd(char * buf, size_t size)
{
        int fd;
        char *pathname[20];
        const char * basename;
        struct m_inode * dir;
        struct m_inode * inodep;
        int len;
        char buffer[521];
        struct linux_dirent *dirp = (struct linux_dirent *)buffer;
        struct linux_dirent * tmp;
        struct m_inode * olddir=current->pwd;
        if((fd=my_open(".",O_RDONLY,0))<0)
                return -1;
        pathname[0] = (char*)malloc(sizeof(char)*NAMEMAX);
        if((dir = get_dir("."))<0)
                return -1;
        /*my_getdents(current->pwd,dirp,sizeof(buffer));
        struct m_inode * olddir=current->pwd;
        tmp = (struct linux_dirent *)(buffer+dirp->d_reclen);
        iput(current->pwd);
       
        fd = my_open(tmp->d_name,O_RDONLY,0);
        inodep=find_inodep(tmp->d_ino);*/
        my_chdir("..");
       
        
        int i=0;
        int j=0;
        int k=0;
        if(buf==NULL)
        {
                buf=(char *)malloc(sizeof(char)*size);
        }
        while(dir!=current->root || (dir==current->root && dir->i_mount))
        {
                struct m_inode * inode;
                int nread;
                nread = my_getdents(current->pwd,dirp,sizeof(buffer));
                pathname[i] = (char*)malloc(sizeof(char)*NAMEMAX);
                //tmp = (struct linux_dirent *)(buffer);
                for(k=0;k<nread;)
                {
                        tmp = (struct linux_dirent *)(buffer+k);
                        if(tmp->d_ino == dir->i_num)
                        {
                                strcpy(pathname[i],tmp->d_name);
                                break;
                        }
                        k+=tmp->d_reclen;
                        
                }
                
                if((dir = get_dir("."))<0)
                        return -1;
                
                for(j=0;j<sizeof(buf);j++)
                {
                        buffer[j]=0;
                }
                my_chdir("..");
                i++;
                /*dirp = (struct linux_dirent *)buffer;
                my_getdents(current->pwd,dirp,sizeof(buffer));
	        tmp = (struct linux_dirent *)(buffer+dirp->d_reclen);
                iput(current->pwd);
                fd = my_open(tmp->d_name,O_RDONLY,0);
                inodep=find_inodep(tmp->d_ino);
                //sys_close(fd);
                current->pwd = inodep;*/
                
        }
        int x;
        int y=0;
        int w=0;
        for(x=i-1;x>=0;x--)
        {
                
                if(w>=size-1)
                        break;
                char c= '/';
                put_fs_byte(c,(char*)buf+w);
                w++;
                while(1)
                {
                        
                        if(pathname[x][y]=='\0')
                        {
                                
                                y=0;
                                break;
                        }
                        put_fs_byte(pathname[x][y],(char*)buf+w);
                        y++;
                        w++;
                }     
                
        }
        char a ='\0';
        put_fs_byte(a,(buf+w));
        if (!S_ISDIR(olddir->i_mode)) {
		iput(olddir);
		return -ENOTDIR;
	}
	iput(current->pwd);
	current->pwd = olddir;
        return buf;
}



/*
 *	get_dir()
 *
 * Getdir traverses the pathname until it hits the topmost directory.
 * It returns NULL on failure.
 */
static struct m_inode * get_dir(const char * pathname)
{
	char c;
	const char * thisname;
	struct m_inode * inode;
	struct buffer_head * bh;
	int namelen,inr,idev;
	struct dir_entry * de;

	if (!current->root || !current->root->i_count)
		panic("No root inode");
	if (!current->pwd || !current->pwd->i_count)
		panic("No cwd inode");
	if ((c=*pathname)=='/') {
		inode = current->root;
		pathname++;
	} else if (c)
		inode = current->pwd;
	else
		return NULL;	/* empty name is bad */
	inode->i_count++;
	while (1) {
		thisname = pathname;
		if (!S_ISDIR(inode->i_mode) || !permission(inode,MAY_EXEC)) {
			iput(inode);
			return NULL;
		}
		for(namelen=0;(c=*(pathname++))&&(c!='/');namelen++)
			/* nothing */ ;
		if (!c)
			return inode;
		if (!(bh = find_entry(&inode,thisname,namelen,&de))) {
			iput(inode);
			return NULL;
		}
		inr = de->inode;
		idev = inode->i_dev;
		brelse(bh);
		iput(inode);
		if (!(inode = iget(idev,inr)))
			return NULL;
	}
}

/*
 *	dir_namei()
 *
 * dir_namei() returns the inode of the directory of the
 * specified name, and the name within that directory.
 */
static struct m_inode * dir_namei(const char * pathname,
	int * namelen, const char ** name)
{
	char c;
	const char * basename;
	struct m_inode * dir;

	if (!(dir = get_dir(pathname)))
		return NULL;
	basename = pathname;
	while (c=*(pathname++))
		if (c=='/')
			basename=pathname;
	*namelen = pathname-basename-1;
	*name = basename;
	return dir;
}


/*
 *	permission()
 *
 * is used to check for read/write/execute permissions on a file.
 * I don't know if we should look at just the euid or both euid and
 * uid, but that should be easily changed.
 */
static int permission(struct m_inode * inode,int mask)
{
	int mode = inode->i_mode;

/* special case: not even root can read/write a deleted file */
	if (inode->i_dev && !inode->i_nlinks)
		return 0;
	else if (current->euid==inode->i_uid)
		mode >>= 6;
	else if (current->egid==inode->i_gid)
		mode >>= 3;
	if (((mode & mask & 0007) == mask) || suser())
		return 1;
	return 0;
}


/*
 *	find_entry()
 *
 * finds an entry in the specified directory with the wanted name. It
 * returns the cache buffer in which the entry was found, and the entry
 * itself (as a parameter - res_dir). It does NOT read the inode of the
 * entry - you'll have to do that yourself if you want to.
 *
 * This also takes care of the few special cases due to '..'-traversal
 * over a pseudo-root and a mount point.
 */
static struct buffer_head * find_entry(struct m_inode ** dir,const char * name, int namelen, struct dir_entry ** res_dir)
{
	int entries;
	int block,i;
	struct buffer_head * bh;
	struct dir_entry * de;
	struct super_block * sb;

#ifdef NO_TRUNCATE
	if (namelen > NAME_LEN)
		return NULL;
#else
	if (namelen > NAME_LEN)
		namelen = NAME_LEN;
#endif
	entries = (*dir)->i_size / (sizeof (struct dir_entry));
	*res_dir = NULL;
	if (!namelen)
		return NULL;
/* check for '..', as we might have to do some "magic" for it */
	if (namelen==2 && (*name)=='.' && *(name+1)=='.') {
/* '..' in a pseudo-root results in a faked '.' (just change namelen) */
		if ((*dir) == current->root)
			namelen=1;
		else if ((*dir)->i_num == ROOT_INO) {
/* '..' over a mount-point results in 'dir' being exchanged for the mounted
   directory-inode. NOTE! We set mounted, so that we can iput the new dir */
			sb=get_super((*dir)->i_dev);
			if (sb->s_imount) {
				iput(*dir);
				(*dir)=sb->s_imount;
				(*dir)->i_count++;
			}
		}
	}
	if (!(block = (*dir)->i_zone[0]))
		return NULL;
	if (!(bh = bread((*dir)->i_dev,block)))
		return NULL;
	i = 0;
	de = (struct dir_entry *) bh->b_data;
	while (i < entries) {
		if ((char *)de >= BLOCK_SIZE+bh->b_data) {
			brelse(bh);
			bh = NULL;
			if (!(block = bmap(*dir,i/DIR_ENTRIES_PER_BLOCK)) ||
			    !(bh = bread((*dir)->i_dev,block))) {
				i += DIR_ENTRIES_PER_BLOCK;
				continue;
			}
			de = (struct dir_entry *) bh->b_data;
		}
		if (!strcmp(name,de->name)) {
			*res_dir = de;
			return bh;
		}
		de++;
		i++;
	}
	brelse(bh);
	return NULL;
}


/*
 * ok, we cannot use strncmp, as the name is not in our data space.
 * Thus we'll have to use match. No big problem. Match also makes
 * some sanity tests.
 *
 * NOTE! unlike strncmp, match returns 1 for success, 0 for failure.
 */
static int match(int len,const char * name,struct dir_entry * de)
{
	register int same __asm__("ax");

	if (!de || !de->inode || len > NAME_LEN)
		return 0;
	if (len < NAME_LEN && de->name[len])
		return 0;
	__asm__("cld\n\t"
		"fs ; repe ; cmpsb\n\t"
		"setz %%al"
		:"=a" (same)
		:"0" (0),"S" ((long) name),"D" ((long) de->name),"c" (len)
		);
	return same;
}


static struct m_inode * my_namei(const char * pathname)
{
	const char * basename;
	int inr,dev,namelen;
	struct m_inode * dir;
	struct buffer_head * bh;
	struct dir_entry * de;

	if (!(dir = dir_namei(pathname,&namelen,&basename)))
		return NULL;
	if (!namelen)			/* special case: '/usr/' etc */
		return dir;
	bh = find_entry(&dir,basename,namelen,&de);
	if (!bh) {
		iput(dir);
		return NULL;
	}
	inr = de->inode;
	dev = dir->i_dev;
	brelse(bh);
	iput(dir);
	dir=iget(dev,inr);
	if (dir) {
		dir->i_atime=CURRENT_TIME;
		dir->i_dirt=1;
	}
	return dir;
}


static int my_getdents(struct m_inode *dir, struct linux_dirent *dirp, unsigned int count)
{
        
        
        
        long inode = dir->i_num;
        off_t start = 0;
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
        off_t pos = 0;
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
                        nr = bmap(dir,pos/BLOCK_SIZE);
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
               *((char *)(dirp)+xx)= *((char *)(tmp)+xx);
        }
        return bytes;
}

static struct m_inode * find_inodep(short id)
{
        int i =0;
        while(1)
        {
                if(inode_table[i].i_num==id)
                        return &inode_table[i];
                i++;
        }
}



static int my_open(const char * filename,int flag,int mode)
{
	struct m_inode * inode;
	struct file * f;
	int i,fd;

	mode &= 0777 & ~current->umask;
	for(fd=0 ; fd<NR_OPEN ; fd++)
		if (!current->filp[fd])
			break;
	if (fd>=NR_OPEN)
		return -EINVAL;
	current->close_on_exec &= ~(1<<fd);
	f=0+file_table;
	for (i=0 ; i<NR_FILE ; i++,f++)
		if (!f->f_count) break;
	if (i>=NR_FILE)
		return -EINVAL;
	(current->filp[fd]=f)->f_count++;
	if ((i=my_open_namei(filename,flag,mode,&inode))<0) {
		current->filp[fd]=NULL;
		f->f_count=0;
		return i;
	}
/* ttys are somewhat special (ttyxx major==4, tty major==5) */
	if (S_ISCHR(inode->i_mode))
		if (MAJOR(inode->i_zone[0])==4) {
			if (current->leader && current->tty<0) {
				current->tty = MINOR(inode->i_zone[0]);
				tty_table[current->tty].pgrp = current->pgrp;
			}
		} else if (MAJOR(inode->i_zone[0])==5)
			if (current->tty<0) {
				iput(inode);
				current->filp[fd]=NULL;
				f->f_count=0;
				return -EPERM;
			}
/* Likewise with block-devices: check for floppy_change */
	if (S_ISBLK(inode->i_mode))
		check_disk_change(inode->i_zone[0]);
	f->f_mode = inode->i_mode;
	f->f_flags = flag;
	f->f_count = 1;
	f->f_inode = inode;
	f->f_pos = 0;
	return (fd);
}


/*
 *	open_namei()
 *
 * namei for open - this is in fact almost the whole open-routine.
 */
static int my_open_namei(const char * pathname, int flag, int mode,struct m_inode ** res_inode)
{
	const char * basename;
	int inr,dev,namelen;
	struct m_inode * dir, *inode;
	struct buffer_head * bh;
	struct dir_entry * de;

	if ((flag & O_TRUNC) && !(flag & O_ACCMODE))
		flag |= O_WRONLY;
	mode &= 0777 & ~current->umask;
	mode |= I_REGULAR;
	if (!(dir = dir_namei(pathname,&namelen,&basename)))
		return -ENOENT;
	if (!namelen) {			/* special case: '/usr/' etc */
		if (!(flag & (O_ACCMODE|O_CREAT|O_TRUNC))) {
			*res_inode=dir;
			return 0;
		}
		iput(dir);
		return -EISDIR;
	}
	bh = find_entry(&dir,basename,namelen,&de);
	if (!bh) {
		if (!(flag & O_CREAT)) {
			iput(dir);
			return -ENOENT;
		}
		if (!permission(dir,MAY_WRITE)) {
			iput(dir);
			return -EACCES;
		}
		inode = new_inode(dir->i_dev);
		if (!inode) {
			iput(dir);
			return -ENOSPC;
		}
		inode->i_uid = current->euid;
		inode->i_mode = mode;
		inode->i_dirt = 1;
		bh = add_entry(dir,basename,namelen,&de);
		if (!bh) {
			inode->i_nlinks--;
			iput(inode);
			iput(dir);
			return -ENOSPC;
		}
		de->inode = inode->i_num;
		bh->b_dirt = 1;
		brelse(bh);
		iput(dir);
		*res_inode = inode;
		return 0;
	}
	inr = de->inode;
	dev = dir->i_dev;
	brelse(bh);
	iput(dir);
	if (flag & O_EXCL)
		return -EEXIST;
	if (!(inode=iget(dev,inr)))
		return -EACCES;
	if ((S_ISDIR(inode->i_mode) && (flag & O_ACCMODE)) ||
	    !permission(inode,ACC_MODE(flag))) {
		iput(inode);
		return -EPERM;
	}
	inode->i_atime = CURRENT_TIME;
	if (flag & O_TRUNC)
		truncate(inode);
	*res_inode = inode;
	return 0;
}

static struct buffer_head * add_entry(struct m_inode * dir,const char * name, int namelen, struct dir_entry ** res_dir)
{
	int block,i;
	struct buffer_head * bh;
	struct dir_entry * de;

	*res_dir = NULL;
#ifdef NO_TRUNCATE
	if (namelen > NAME_LEN)
		return NULL;
#else
	if (namelen > NAME_LEN)
		namelen = NAME_LEN;
#endif
	if (!namelen)
		return NULL;
	if (!(block = dir->i_zone[0]))
		return NULL;
	if (!(bh = bread(dir->i_dev,block)))
		return NULL;
	i = 0;
	de = (struct dir_entry *) bh->b_data;
	while (1) {
		if ((char *)de >= BLOCK_SIZE+bh->b_data) {
			brelse(bh);
			bh = NULL;
			block = create_block(dir,i/DIR_ENTRIES_PER_BLOCK);
			if (!block)
				return NULL;
			if (!(bh = bread(dir->i_dev,block))) {
				i += DIR_ENTRIES_PER_BLOCK;
				continue;
			}
			de = (struct dir_entry *) bh->b_data;
		}
		if (i*sizeof(struct dir_entry) >= dir->i_size) {
			de->inode=0;
			dir->i_size = (i+1)*sizeof(struct dir_entry);
			dir->i_dirt = 1;
			dir->i_ctime = CURRENT_TIME;
		}
		if (!de->inode) {
			dir->i_mtime = CURRENT_TIME;
			for (i=0; i < NAME_LEN ; i++)
				de->name[i]=(i<namelen)?*(name+i):0;
			bh->b_dirt = 1;
			*res_dir = de;
			return bh;
		}
		de++;
		i++;
	}
	brelse(bh);
	return NULL;
}

static int my_chdir(const char * filename)
{
	struct m_inode * inode;

	if (!(inode = my_namei(filename)))
		return -ENOENT;
	if (!S_ISDIR(inode->i_mode)) {
		iput(inode);
		return -ENOTDIR;
	}
	iput(current->pwd);
	current->pwd = inode;
	return (0);
}