/**
 ** written by lthuang@cc.nsysu.edu.tw
 **/

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "struct.h"
#include "libproto.h"

#ifndef LOCK_EX
# define LOCK_EX               F_LOCK     /* exclusive lock */
# define LOCK_UN               F_ULOCK    /* unlock */
#endif


#if 1
void
bbslog(const char *mode, const char *fmt, ...)
{
	va_list args;
	time_t now;
	char msgbuf[128], buf[128];
	char timestr[20];

	va_start(args, fmt);
#if !HAVE_VSNPRINTF
	vsprintf(msgbuf, fmt, args);
#else	
	vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
#endif	
	va_end(args);

	time(&now);
	strftime(timestr, sizeof(timestr), "%m/%d/%Y %X", localtime(&now));

#if !HAVE_SNPRINTF
	sprintf(buf, "%s %.8s: %s\n", timestr, mode, msgbuf);
#else
	snprintf(buf, sizeof(buf), "%s %.8s: %s\n", timestr, mode, msgbuf);
#endif	
	append_record(PATH_BBSLOG, buf, strlen(buf));
}
#endif

void
sethomefile(buf, userid, filename)
register char *buf;
#ifndef IGNORE_CASE                     // clean warnings
const char *userid, *filename;
#else
char *userid;
const char *filename;
#endif
{
	register unsigned char c = *userid;

	if (isupper(c))
		c = tolower(c);
	else if (!islower(c))
		c = '0';

#ifdef IGNORE_CASE /* sarek:03/12/2001:高市資教希望可以申請大小寫有別的ID */
        //strtolow(userid);
#endif
	if (filename)
		sprintf(buf, "home/%c/%s/%s", c, userid, filename);
	else
		sprintf(buf, "home/%c/%s", c, userid);
}


void
setuserfile(buf, userid, filename)
register char *buf;
#ifndef IGNORE_CASE
const char *userid, *filename;
#else
char *userid;
const char *filename;
#endif
{
#ifdef IGNORE_CASE
        strtolow(userid);
#endif
	sprintf(buf, "%s/%s", filename, userid);
}


void
setboardfile(buf, bname, filename)
register char *buf;
const char *bname, *filename;
{
	if (filename)
		sprintf(buf, "%s/%s/%s", BBSPATH_BOARDS, bname, filename);
	else
		sprintf(buf, "%s/%s", BBSPATH_BOARDS, bname);
}


void
setvotefile(buf, bname, filename)
register char *buf;
const char *bname, *filename;
{
	if (filename)
		sprintf(buf, "%s/%s/vote/%s", BBSPATH_BOARDS, bname, filename);
	else
		sprintf(buf, "%s/%s/vote", BBSPATH_BOARDS, bname);
}


void
settreafile(buf, bname, filename)
register char *buf;
const char *bname, *filename;
{
	if (filename)
		sprintf(buf, "%s/%s/%s", BBSPATH_TREASURE, bname, filename);
	else
		sprintf(buf, "%s/%s", BBSPATH_TREASURE, bname);
}


void
setmailfile(buf, userid, filename)
char *buf;
#ifndef IGNORE_CASE
const char *userid, *filename;
#else
char *userid;
const char *filename;
#endif
{
	register unsigned char c = *userid;

	if (isupper(c))
		c = tolower(c);
	else if (!islower(c))
		c = '0';

#ifdef IGNORE_CASE
        strtolow(userid);
#endif	

	if (filename)
		sprintf(buf, "%s/%c/%s/%s", UFNAME_MAIL, c, userid, filename);	
	else
		sprintf(buf, "%s/%c/%s", UFNAME_MAIL, c, userid);
}


void
setdotfile(buf, dotfile, fname)
register char *buf;
const char *dotfile, *fname;
{
	register char *ptr;

	strcpy(buf, dotfile);
	if ((ptr = strrchr(buf, '/')) == NULL)
	{
		bbslog("ERR", "setdotfile: [%s] [%s]", dotfile, fname);
		exit(-1);
		/* NOT REACHED */
	}
	while (ptr > buf && *(ptr-1) == '/')
		*ptr-- = '\0';
	ptr++;
	if (fname)
		strcpy(ptr, (*fname == '/') ? fname + 1 : fname);	
	else
		*ptr = '\0';
}


void
init_bbsenv()
{
/*
	if (chdir(HOMEBBS) == -1)
	{
		fprintf(stderr, "\ncannot chdir: %s", HOMEBBS);
		fflush(stderr);
		exit(-2);
	}
*/
	chdir(HOMEBBS);
	
	if (getuid() != BBS_UID)
	{
#ifdef CHROOT_BBS	
		if (chroot(HOMEBBS) == -1 || chdir("/") == -1)
		{
			fprintf(stderr, "\ncannot chroot: %s\n", HOMEBBS);
			fflush(stderr);
			exit(-1);
		}
#endif		
		if (setgid(BBS_GID) == -1 || setuid(BBS_UID) == -1)
		{
			fprintf(stderr, "\nplease run this program in bbs\n");
			fflush(stderr);
			exit(-1);
		}
	}
	load_bbsconf();	
}


#if 1
/*ARGUSED*/
/*
   host_deny  - 檢查使用者來處, 傳回 0 表示 allow, 1 表示 deny
   by lmj
*/
struct HostDeny {
	char *host;
	short len;
};

static struct HostDeny *
host_deny_init(fname)
char *fname;
{
	FILE *fp;
	struct HostDeny *table = (struct HostDeny *) NULL;

	if((fp = fopen(fname, "r")) != NULL)
	{
		char genbuf[80], *buf, *p1, *p2, *hosts;
		short i = 0, num;

		p2 = buf = (char *) malloc(4096);
		p1 = genbuf;
		memset(buf, 0, 4096);
		memset(genbuf, 0, sizeof(genbuf));
		while(fgets(genbuf, sizeof(genbuf), fp))
		{
			if(*genbuf == '#' || *genbuf == '\n')
				continue;
			while(*p1 && (*p1 < '0' || *p1 > '9'))
				p1++;
			if(*p1)
			{
				i++;
				while((*p1 >= '0' && *p1 <= '9') || *p1 == '.' || *p1 == '*')
					*p2++ = *p1++;
				*p2++ = '\n';
			}
			memset(genbuf, 0, sizeof(genbuf));
			p1 = genbuf;
		}
		num = i;
		table = (struct HostDeny *) malloc((++i)*sizeof(struct HostDeny));
		memset(table, 0, i*sizeof(struct HostDeny));
		i = strlen(buf);
		hosts = (char *) malloc(++i);
		memset(hosts, 0, i);
		strcpy(hosts, buf);
		free(buf);
		for(i = 0, p1 = hosts; i < num; i++)
		{
			table[i].host = p1;
			p1 = strchr(p1, '\n');
			if(*(p1 - 1) == '*')
				table[i].len = p1 - table[i].host - 1;
			*p1++ = '\0';
		}
		fclose(fp);
	}
	return table;
}


static struct HostDeny *dhost = (struct HostDeny *)NULL;
static struct HostDeny *ahost = (struct HostDeny *)NULL;
static struct HostDeny hostnull[1];

int 
host_deny(host)
char *host;
{
	int deny_login = 0;

	if (ahost)
	{
		short i;

		if (ahost[0].host)
		{
		    /* 若 host.allow 存在，先 deny all */
			deny_login++;	/* deny all */
			for (i = 0; ahost[i].host; i++)
			{
				if (ahost[i].len)
				{
					if (!strncmp(ahost[i].host, host, ahost[i].len))
					{
						deny_login = 0;
						break;
					}
				}
				else
				{
					if(!strcmp(ahost[i].host, host))
					{
						deny_login = 0;
						break;
					}
				}
			}
		}
		if (dhost[0].host && !deny_login)
		{
			for (i = 0; dhost[i].host; i++)
			{
				if (dhost[i].len)
				{
					if (!strncmp(dhost[i].host, host, dhost[i].len))
					{
						deny_login++;
						break;
					}
				}
				else
				{
					if (!strcmp(dhost[i].host, host))
					{
						deny_login++;
						break;
					}
				}
			}
		}
	}
	else
	{
		memset(hostnull, 0, sizeof(hostnull));
		ahost = host_deny_init(HOST_ALLOW);
		if (!ahost)
			ahost = hostnull;
		dhost = host_deny_init(HOST_DENY);
		if (!dhost)
			dhost = hostnull;
	}
	return deny_login;
}
#endif
