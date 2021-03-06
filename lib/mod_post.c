
#include "bbs.h"
#include <sys/stat.h>


/*
 * update the rewind_time for some board, 
 * user should update their readrc according this rewind_time
 */
static int
rewind_board(bname)
char *bname;
{
	int fd;
	BOARDHEADER sbh;

	if ((fd = open(BOARDS, O_RDWR)) > 0)
	{
		flock(fd, LOCK_EX);
		while (read(fd, &sbh, sizeof(sbh)) == sizeof(sbh))
		{
			if (!strcmp(sbh.filename, bname))
			{
				sbh.rewind_time = time(0);
				if (lseek(fd, -((off_t) sizeof(sbh)), SEEK_CUR) != -1)
				{
					if (write(fd, &sbh, sizeof(sbh)) == sizeof(sbh))
					{
						flock(fd, LOCK_UN);
						close(fd);
						return 0;
					}
				}
				break;
			}
		}
		flock(fd, LOCK_UN);
		close(fd);
	}
	return -1;
}


/**
 ** send article or cancel message to usenet
 **/
int
append_news(bname, fname, userid, username, title, opt)
char *bname, *fname, opt;
char *userid, *username, *title;
{
	FILE *fp;
	char nbuf[PATHLEN];
	extern int flock(int fd, int op);

/* TODO
	if (opt == 'D')
		sprintf(nbuf, "news/cancel.bntp");
	else
		sprintf(nbuf, "news/out.bntp");
	if ((fp = fopen(nbuf, "a+")) == NULL)
		return -1;
		
	flock(fileno(fp), LOCK_EX);
	fprintf(fp, "%s\t%s\t%s\t%s\t%s\n", 
	        bname, fname, userid, username, title);
	fclose(fp);	       
	return 0;
*/	
	sprintf(nbuf, "news/output/%s", bname);
	if ((fp = fopen(nbuf, "a+")) == NULL)
		return -1;
	flock(fileno(fp), LOCK_EX);
	if (opt == 'D')
		fprintf(fp, "-%s\n", fname);
	else
		fprintf(fp, "%s\n", fname);
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	if (opt == 'D')
	{
		/* for sending cancel message to usenet */
		char bbuf[PATHLEN];	
		
		setboardfile(bbuf, bname, fname);
		sprintf(nbuf, "news/cancel/%s.%s", bname, fname);
		mycp(bbuf, nbuf);
	}
	return 0;
}


/* 
 * fname    欲張的的佈告檔名
 * ident    張貼者的認證等級
 * tonews   是否送上 news
 * postpath 張貼目錄路徑 (若張貼在精華區)
 * mask		張貼佈告時順便設定 flag		add by asuka
 */
#ifdef USE_THREADING	/* syhu */
int
PublishPost(fname, userid, username, bname, title, ident, fromhost, tonews, postpath, flag, thrheadpos, thrpostidx)
char *fname, *userid, *username, *bname, *title, *fromhost, *postpath;
char ident;
short tonews;
unsigned char flag;
int thrheadpos; 							/* position in .THREADHEAD file */
int thrpostidx;								/* index in .THREADPOST file */	

#else
int
PublishPost(fname, userid, username, bname, title, ident, fromhost, tonews, postpath, flag)
char *fname, *userid, *username, *bname, *title, *fromhost, *postpath;
char ident;
short tonews;
unsigned char flag;
#endif
{
	char timestamp[15], tempfile[PATHLEN], pathTmp[PATHLEN];
	int artno;
	

	/* copy the post to a temp. location first, all the processing will
	   be done on this temp file until it's finished */	
	sprintf(tempfile, "tmp/bbs%05d", (int)getpid());
	if (mycp(fname, tempfile) < 0)
		return -1;
	
 	/* set up the correct destination path of this post */
	if (postpath)
		strcpy(pathTmp, postpath);
	else
		setboardfile(pathTmp, bname, NULL);

 	/* do the actual copying of the post in 'tempfile' to destination,
	   and also update the .DIR file */ 
#ifdef USE_THREADING	/* syhu */
	if ((artno = append_article(tempfile, pathTmp, userid, title,ident,/*syhu*/ 
	                            timestamp, (postpath) ? FALSE : TRUE, 
	                            flag, fromhost, thrheadpos, thrpostidx)) != -1)
#else
	if ((artno = append_article(tempfile, pathTmp, userid, title, ident,
                                timestamp, (postpath) ? FALSE : TRUE,
                                flag, fromhost)) != -1)
#endif
	{
		if (!postpath)
		{
 			/* if this is the first post, then record/update the rewind 
			   time for this board */ 
			if (artno == 1)
				rewind_board(bname);
			if (tonews
#if EMAIL_LIMIT
			    && ident == 7
#endif
				)
				append_news(bname, timestamp, userid, username, title, 'S');
 
			/* update the share memory info of no. of posts in this board */
			set_brdt_numposts(bname, FALSE);	/* lthuang: 99/08/20 */				
		}
		unlink(tempfile);
		
		return artno;
	}
	unlink(tempfile);
	return -1;
}


int
make_treasure_folder(direct, title, dirname)
char *direct;
char *title;
char *dirname;
{
	char path[PATHLEN], new[PATHLEN];
	char *stamp;
	int fd, fdnew, insert_ok;
	FILEHEADER fhbuf, th;
	int result;

	setdotfile(path, direct, NULL);		/* 取得 direct 的所在目錄 */
	stamp = path + strlen(path);
	do
	{
		sprintf(stamp, "M.%d.A", (int)time(0));	/* -ToDo- T.XXXXXXX.A */
	}
	while (mkdir(path, 0700) == -1);
	
	if (dirname)
		strcpy(dirname, path);

	memset(&th, 0, sizeof(th));
	strcpy(th.title, title);
	strcpy(th.filename, stamp);
	th.accessed |= FILE_TREA;

	if ((fd = open(direct, O_RDWR | O_CREAT, 0600)) < 0)
	{
		rmdir(path);
		return -1;
	}
	sprintf(new, "%s.new", direct);	
	if ((fdnew = open(new, O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0)
	{
		close(fd);
		rmdir(path);
		return -1;
	}

	result = 0;
	insert_ok = FALSE;
	flock(fd, LOCK_EX);
	while (read(fd, &fhbuf, FH_SIZE) == FH_SIZE)
	{
		if (!insert_ok && !(fhbuf.accessed & FILE_TREA))
		{
			if (write(fdnew, &th, sizeof(th)) != sizeof(th))
			{
				result = -1;
				break;
			}
			insert_ok = TRUE;
		}
		if (write(fdnew, &fhbuf, FH_SIZE) != FH_SIZE)
		{
			result = -1;
			break;
		}
	}
	if (result == 0 && !insert_ok)
	{
		if (write(fdnew, &th, sizeof(th)) != sizeof(th))
			result = -1;
	}
	close(fdnew);
	flock(fd, LOCK_UN);
	close(fd);
	if (result == 0)
	{
		if (rename(new, direct) == 0)
			return 0;
	}
	unlink(new);
	rmdir(path);
	return -1;
}
