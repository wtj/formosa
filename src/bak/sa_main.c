/* 
 * Li-te Huang, lthuang@cc.nsysu.edu.tw, 03/09/98
 * Last updated: 06/02/98
 */

#include "bbs.h"
#include "tsbbs.h"
#include <sys/socket.h>


char myfromhost[HOSTLEN];
char myuserid[IDLEN];
char mypasswd[PASSLEN];

/* multi-login */
int multi_logins = 1;

extern pid_t child_pid;
extern BOOL show_ansi;
extern MSQ allmsqs[];
extern int msq_first, msq_last;

extern char *genpasswd();


/**
 ** Idle Timeout
 **/
void
saybyebye(s)
int s;
{
	int fd = getdtablesize();

	while (fd)
		close(--fd);
	shutdown(0, 2);
	close(0);
	exit(0);
}


void
abort_bbs(s)
int s;
{
	if (child_pid > 2)
		kill(child_pid, SIGKILL);

	user_logout(cutmp, &curuser);
	shutdown(0, 2);
	exit(0);
}


static void
warn_bell()
{
	unsigned char bell[4] = {7,7,7,0};
	fputs(bell, stderr);
}


static void
talk_request(s)
int s;
{
#if	defined(LINUX) || defined(SOLARIS)
	/*
	 * Reset signal handler for SIGUSR1, when signal received, 
	 * some OS set the handler to default, SIG_DFL. The SIG_DFL
	 * usually is terminating the process. So, when user was paged
	 * twice, he will be terminated.
	 */
	signal(SIGUSR1, talk_request);
#endif
	talkrequest = TRUE;
	warn_bell();
}


static void
msq_request(s)
int s;
{
	static char bigbuf[1024];
	static int len;
	static MSQ tmp;
	
	len = 0;
	while (msq_rcv(cutmp, &tmp) == 0)
	{
		/* overwrite the previous when queue is full */
		if (msq_last != -1 && (msq_last + 1) % LOCAL_MAX_MSQ == msq_first)
			msq_first = (msq_first + 1) % LOCAL_MAX_MSQ;		
		
		msq_last = (msq_last + 1) % LOCAL_MAX_MSQ;
		memcpy(&(allmsqs[msq_last]), &tmp, sizeof(tmp));
	
		msq_tostr(&(allmsqs[msq_last]), genbuf);
		strcpy(bigbuf + len, genbuf);
		len += strlen(genbuf);
		strcpy(bigbuf + len, "\n");
		len += 1;
	}
	if (len > 0)
	{
		int fd;
		
		if ((fd = open(ufile_write, O_WRONLY, O_CREAT | O_APPEND)) > 0)
		{
			write(fd, bigbuf, len);
			close(fd);
		}
/*
for speed-up, not use lock-file append
		append_record(ufile_write, bigbuf, len);
*/		
	}

#if	defined(LINUX) || defined(SOLARIS)
	signal(SIGUSR2, msq_request);
#endif
	warn_bell();
		
	msqrequest = TRUE;
}


BOOL IsRealSysop = FALSE;

static void
user_init()
{
	setmailfile(ufile_mail, curuser.userid, DIR_REC);
	sethomefile(ufile_overrides, curuser.userid, UFNAME_OVERRIDES);
	sethomefile(ufile_blacklist, curuser.userid, UFNAME_BLACKLIST);
	setuserfile(ufile_write, curuser.userid, UFNAME_WRITE);

#ifdef GUEST
	if (!strcmp(curuser.userid, GUEST))
	{
		curuser.flags[0] &= ~PICTURE_FLAG;
		return;
	}
#endif

#ifdef NSYSUBBS
	/* ~bbs/conf/sysoplist 站長列表
	 * 本檔案中有列名者（一行一個ＩＤ）才能看到 Admin 選單，並擁
	 * 有 admin 使用權, 否則即使有 PERM_BM 等級的站長也看不到，，所以
	 * 新增站長請記得修改此檔案。
	 */
	IsRealSysop = seekstr_in_file("conf/sysoplist", curuser.userid);
#endif

	if (curuser.flags[0] & COLOR_FLAG)
		show_ansi = FALSE;
	else
		show_ansi = TRUE;

	/* sarek:01/02/2000 STRIP ANSI */
        if (curuser.flags[0] & STRIP_ANSI_FLAG)
                strip_ansi = TRUE;
        else
                strip_ansi = FALSE;
	/* sarek:01/02/2000 above */


	ReadRC_Expire();

	/* 
	 * If user multi-login, 
	 * we should not remove the exist message. by lthuang
	 */
	if (multi_logins < 2)
		unlink(ufile_write);
	/*
	 * Some user complain that there were some mail mark deleted in 
	 * their mail box for a long time. In fact, they do not logout
	 * in proper way, so result in this situation. We do this checking
	 * for force-packing their mail box. by lthuang 
	 */
	if ((curuser.numlogins % 7) == 0)
		pack_article(ufile_mail);

#if 0
	/* lasehu: for updating usrec flags from FormosaBBS 1.1.x to 1.2.x */
	if (!curuser.pager)
	{
		/* old PAGER_FLAG is [0]:0x01, old CHKONLY_FLAG is [0]:0x10 */
		if (curuser.flags[0] & 0x11)
			curuser.pager = PAGER_FIDENT;
		else if (curuser.flags[0] & 0x01)
			curuser.pager = PAGER_FRIEND;
	}

	/* old PICTURE_FLAG is [1]:0x01 */ 
	if (curuser.flags[1] & 0x01)
	{
		curuser.flags[0] |= PICTURE_FLAG;
		curuser.flags[1] &= ~0x01;
	}
	/* old NOTE_FLAG is [1]:0x02 */
	if (curuser.flags[1] & 0x02)
	{
		curuser.flags[0] |= NOTE_FLAG;
		curuser.flags[1] &= ~0x02;
	}
#endif

#ifdef STRIP_ANSI_USERNAME
	{ char *p;
	/* strip the ansi codes in username */
	if ((p = strchr(curuser.username, 0x1b)) != NULL)
	{
		if (!strstr(p+1, "[0m") && !strstr(p+1, "[m"))
		{
			clear();
			move(10, 0);
			outs("抱歉, 由於您的暱稱含有彩色控制碼, 且未加上還原碼\n\
為了使得出現在信件、佈告、使用者列表中彩色控制碼的佈置發生畫面亂掉的問題,\n\
所以暫時取消您的暱稱!\n\
\n\
請您至主選單 -> (x)個人工具箱 -> (f)去除暱稱彩色控制碼\n\
便可恢復您原有的暱稱, 但彩色控制碼將會消失!\n");
			pressreturn();
			uinfo.username[0] = '\0';
		}
	}
	}
#endif

#ifdef IGNORE_CASE
        /* kmwang:20000628:檢查 Fake ID 是否相等於 Real ID */
        if (strcasecmp(curuser.userid, curuser.fakeuserid))
        {
                strcpy(curuser.fakeuserid, curuser.userid);
        }
#endif
	
#if 0
	{
		FILE *fp;
		char *ip, *home;

		if ((fp = fopen("conf/hostlist", "r")) != NULL)
		{
			while (fgets(genbuf, sizeof(genbuf), fp))
			{
				if (ip = strtok(genbuf, " \t\n"))
				{
					if (*ip == '#')
						continue;
					if (!strncmp(ip, myfromhost, strlen(ip))
					&& (home = strtok(NULL, "\n")) != NULL)
					{
						xstrncpy(uinfo.from, home, sizeof(uinfo.from));
						break;
					}
				}
			}
			fclose(fp);
		}
	}
#endif

	/* initialize */
	if (PERM_SYSOP == curuser.userlevel)
		maxkeepmail = 100 * SPEC_MAX_KEEP_MAIL;	/* lthuang */
/* By kmwang:20000529:For KHBBS 等級在100(含)以上者的信箱上限皆設為 200 */
#ifdef KHBBS
	else if (curuser.userlevel >= 100)
		maxkeepmail = SPEC_MAX_KEEP_MAIL;
#else
	else if (PERM_BM == curuser.userlevel)
		maxkeepmail = SPEC_MAX_KEEP_MAIL;
#endif
	else
		maxkeepmail = MAX_KEEP_MAIL;
}


static void
new_register(nu)
USEREC *nu;
{
	int attempt = 0;

	int fd;
		

	if ((fd = open(NEWID_HELP, O_RDONLY)) > 0)
	{
		while (read(fd, genbuf, sizeof(genbuf)))
			outs(genbuf);
		close(fd);
	}

	memset(nu, 0, sizeof(USEREC));

	/* determine what userid used for newuser */
	outs(_msg_formosa_1);
	while (1)
	{
		if (attempt++ >= 3)
		{
			outs(_msg_formosa_4);
			exit(0);	/* abort bbs */
		}

		getdata(0, 0, _msg_formosa_2, nu->userid, IDLEN, ECHONOSP, NULL);
#ifdef IGNORE_CASE
                strcpy(nu->fakeuserid, nu->userid);
                /* kmwang:20000629:將 nu->userid 全轉為小寫 */
                strtolow(nu->userid);
#endif
		if (invalid_new_userid(nu->userid))
		{
			prints(_msg_formosa_3, LEAST_IDLEN);
			continue;
		}

		if (get_passwd(NULL, nu->userid) > 0
/*		    || is_duplicate_userid(nu->userid)*/)		/* check duplicate userid */
		{
			outs(_msg_formosa_5);
			continue;
		}
		strcpy(myuserid, nu->userid);
		break;
	}

	/* enter new user date, password, etc. */
	while (1)
	{
		getdata(0, 0, _msg_formosa_6, mypasswd, sizeof(mypasswd), XNOECHO, 
		        NULL);
		if (strlen(mypasswd) < 4)
		{
			outs(_msg_formosa_7);
			continue;
		}
		if (!strcmp(mypasswd, nu->userid))
		{
			outs(_msg_formosa_8);
			continue;
		}
		getdata(0, 0, _msg_formosa_9, genbuf, sizeof(mypasswd), XNOECHO, NULL);
		if (strcmp(genbuf, mypasswd))
		{
			outs(_msg_formosa_10);
			continue;
		}
		break;
	}

	getdata(0, 0, _msg_formosa_11, nu->username, sizeof(nu->username),
		XECHO, NULL);
	getdata(0, 0, _msg_formosa_12, nu->email, sizeof(nu->email),
		ECHONOSP, NULL);

	nu->firstlogin = time(0);	/* lthuang */
	nu->lastlogin = nu->firstlogin;
	uinfo.login_time = nu->firstlogin;	/* lthuang */
	xstrncpy(uinfo.from, myfromhost, HOSTLEN);
	xstrncpy(nu->lasthost, myfromhost, HOSTLEN);
	xstrncpy(nu->passwd, genpasswd(mypasswd), PASSLEN);
	nu->userlevel = PERM_DEFAULT;
	nu->numlogins = 1;	/* lthuang */
	/* write data to password file */
	if ((nu->uid = new_user(nu, FALSE)) <= 0)
	{
		outs(_msg_formosa_13);
		oflush();
		exit(0);
	}
}


static void
login_query()
{
	int act, attempt = 0, n;
	FILE *fp;


	if ((fp = fopen(BBSSRV_WELCOME, "r")) != NULL)
	{
		while (fgets(genbuf, sizeof(genbuf), fp))
			outs(genbuf);
		fclose(fp);
	}

#ifdef ACTFILE
	if ((fp = fopen(ACTFILE, "r")) != NULL)
	{
		while (fgets(genbuf, sizeof(genbuf), fp))
			outs(genbuf);
		fclose(fp);
	}
#endif

	num_ulist(&act, NULL, NULL);
	prints(_msg_formosa_14, BBSTITLE, act, MAXACTIVE);

#if 0
	prints("\n[1m%s[m\n", FORMOSA_BBS_SERVER_VERSION);
#endif

#ifdef SHOW_UPTIME
	if ((fp = fopen(SHOW_UPTIME, "r")) != NULL)
	{
		if (fgets(genbuf, sizeof(genbuf), fp))
		{
			char *ptr;

			if ((ptr = strrchr(genbuf, ':')) != NULL)
			{
				char *p;
				
				if ((p = strrchr(genbuf, '\n')) != NULL)
					*p = '\0';
				prints(_msg_formosa_15, ++ptr);
			}
		}
		fclose(fp);
	}
#endif

	if (act > MAXACTIVE)
	{
		prints(_msg_formosa_16, MAXACTIVE);
		oflush();
		shutdown(0, 2);
		exit(0);
	}

	for (;;)
	{
		if (attempt++ >= 3)	/* too many times, fail login */
		{
			prints(_msg_formosa_17, 3);
			oflush();
			shutdown(0, 2);
			exit(0);
		}

#if	defined(LOGINASNEW)
		outs(_msg_formosa_18);
#ifdef GUEST
		prints(_msg_formosa_19, GUEST);
#endif /* GUEST */
#else
#ifdef GUEST
		prints(_msg_formosa_20, GUEST);
#endif
#endif /* !LOGINASNEW */

		if (!getdata(0, 0, _msg_formosa_21, myuserid, sizeof(myuserid),
			     ECHONOSP, NULL))
		{
			outs(_msg_err_userid);
			continue;
		}

#ifdef IGNORE_CASE
                strtolow(myuserid);     /* 拿掉大小寫限制 */
#endif

		if (!strcmp(myuserid, "new"))
		{
#ifndef LOGINASNEW
  #ifdef GUEST
			printf(_msg_formosa_23);
			oflush();
			shutdown(0, 2);
			exit(0);
  #endif
#else /* !LOGINASNEW */
			alarm(300);	/* when new user registering, set timeout to 300 sec */
			new_register(&curuser);
			goto login;
#endif /* LOGINASNEW */
		}
#ifdef GUEST
		else if (!strcmp(myuserid, GUEST))
		{
			getdata(0, 0, _msg_formosa_25, mypasswd, sizeof(mypasswd),
				XNOECHO, NULL);
		}
#endif
		else
		{
			getdata(0, 0, _msg_formosa_26, mypasswd, sizeof(mypasswd),
				XNOECHO, NULL);
		}

	      login:
		n = user_login(&cutmp, &curuser, CTYPE_TSBBS, myuserid, mypasswd,
			       myfromhost);
		if (n == ULOGIN_OK)
		{
			memcpy(&uinfo, cutmp, sizeof(USER_INFO));
			break;
		}
		else if (n == ULOGIN_PASSFAIL)
		{
			outs(_msg_formosa_27);
			continue;
		}
		outs(_msg_formosa_44);
	}
}


int
Announce()
{
	more(WELCOME, TRUE);
	return C_FULL;
}


/*
 * check multi-logins for current user
 */
BOOL bCountGuest = FALSE;

static int
count_multi_login(upent)
USER_INFO *upent;
{
	static int short i = 0;

	if (upent->pid <= 2 || uinfo.pid <= 2)	/* debug */
		return -1;

	if (!strcmp(upent->userid, uinfo.userid))	/* -ToDo- compare uid */
	{
		i++;
		if (upent->pid != uinfo.pid)
		{
			multi_logins++;
			if (bCountGuest)
			{
				if (multi_logins > MAX_GUEST_LOGINS)
				{
					outs(_msg_formosa_39);
					oflush();
					shutdown(0, 2);
					exit(0);
				}
				return 0;
			}
			/* prevent user multi-login many many times */
			if (multi_logins > MULTILOGINS + 1 && !HAS_PERM(PERM_SYSOP))
				goto force_leave;
			prints(_msg_formosa_40, i, upent->pid, upent->from);
			if (igetkey() == 'y')
			{
				if (upent->pid > 2)	/* debug */
					kill(upent->pid, SIGKILL);
				purge_ulist(upent);
				multi_logins--;
			}
			if (multi_logins > MULTILOGINS && !HAS_PERM(PERM_SYSOP))
			{
			      force_leave:
				prints(_msg_formosa_41, multi_logins);
				oflush();
				shutdown(0, 2);
				exit(0);
			}
		}
	}
	return 0;
}


static void
multi_user_check()
{
#ifdef GUEST
	if (!strcmp(curuser.userid, GUEST))
		bCountGuest = TRUE;
#endif /* GUEST */

	apply_ulist(count_multi_login);
}


/*
 * Main function of BBS
 */
int
main(argc, argv)
int argc;
char *argv[];
{
	char *host=NULL;
	unsigned ibuf[80];
	extern int utmp_semid;

	//host=getenv("REMOTEHOST");
	//printf("User comes from %s\n", host);

	init_bbsenv();

#if 0
	host_deny((char *) NULL);   /* init host deny table */
#endif  

	utmp_semid = sem_init(UTMPSEM_KEY);

	sprintf((char *) (ibuf + 1), "\r\n\r\n☆ %s ☆\r\n\r\r\n\r", BBSTITLE);
	write(1, ibuf, strlen((char *) (ibuf + 1)) + 1);
	fflush(stdout);

	
	signal(SIGHUP, abort_bbs);
	signal(SIGBUS, abort_bbs);
#ifdef SYSV
	signal(SIGSYS, abort_bbs);
#endif	
	signal(SIGTERM, abort_bbs);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGPIPE, abort_bbs);	/* lthuang */
	signal(SIGURG, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGUSR1, talk_request);
	signal(SIGUSR2, msq_request);

	if (setjmp(byebye))
	{
		shutdown(0, 2);
		exit(1);
	}

	/* initialize virtual terminal */
	init_vtty();

	/* start timeout alarm */
	signal(SIGALRM, saybyebye);
	alarm(120);

	/* default language is 'chinese' */	
	lang_init(LANG_CHINESE);	
	xstrncpy(myfromhost, (host ? host : "local"), sizeof(myfromhost));	
	login_query();
	/* multi-language message supported */
	lang_init(curuser.lang);
	
	/* stop timeout alarm */
	signal(SIGALRM, SIG_IGN);
	alarm(0);
	
#if 0		/* !!! TEST !!! */
	/* start normal alarm */
	init_alarm();
#endif	

	/* TODO: write 'bbsd: userid from' to /proc/<pid>/psinfo, argv */

	bbsd_log_open();

	term_init("vt100");	/* only support 'vt100' now */
	initscr();
	multi_user_check();
	user_init();
	/* bakfiletest by  wnlee */
#ifdef GUEST
	if (strcmp(curuser.userid, GUEST))
#endif
	{
		setuserfile(genbuf, curuser.userid, UFNAME_EDIT);
		if (isfile(genbuf))
		{
			clear();
			move(10, 0);
			outs(_msg_formosa_42);
			pressreturn();
		}
	}

	/* welcome banner */
	more(WELCOME0, TRUE);	
	/* Announce banner */
	Announce();

	if (!(curuser.flags[0] & NOTE_FLAG))	/* wnlee */
		x_viewnote();
		
	/* new user guide */
	if (curuser.userlevel <= 3)
		more(NEWGUIDE, TRUE);

	/* enter main menu */
	domenu();
	/* NOT REACHED */
}
