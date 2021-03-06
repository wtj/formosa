/**
 ** This file was derived from MapleBBS's, powerful talk menu.
 ** Last updated: 05/20, lthuang@cc.nsysu.edu.tw
 **
 ** Re-write online user list menu, by using cursor menu function
 ** 09/14/98, lthuang@cc.nsysu.edu.tw
 **/

#include "bbs.h"
#include "tsbbs.h"
#include <stdlib.h>


struct pickup
{
	USER_INFO *ui;
	char friend;
#ifdef USE_OVERRIDE_IN_LIST
	char override;
//	char blacklist;
#endif	
};

struct pickup *pklist = NULL;

USER_INFO *uentp;

int friends_number = 0;
int num_users;
int u_ccur = 0;

BOOL list_friend;

#define PICKUP_WAYS     (5)

short pickup_way = 1;		/* 預設排序: 線上好友 */

char *msg_pickup_way[PICKUP_WAYS] =	/* lang.h */
{
	"任意排列",
	"線上好友",
	"英文代名",
	"線上狀態",
	"上站地點"
/*      
 * _msg_pickup_way_1,
 * _msg_pickup_way_2,
 * _msg_pickup_way_3,
 * _msg_pickup_way_4,
 * _msg_pickup_way_5
 */
};


static int
pickup_cmp(i, j)
struct pickup *i, *j;
{
	switch (pickup_way)
	{
	case 1:
	default:
#ifdef USE_OVERRIDE_IN_LIST		
		{
			int k;
			
			k = (j->friend - i->friend);
			if (k ==0 && j->friend == 0)
				return (j->override - i->override);
			return k;
		}
#else
		return (j->friend - i->friend);
#endif				
	case 2:
		return strcasecmp(i->ui->userid, j->ui->userid);
	case 3:
		return (i->ui->mode - j->ui->mode);
	case 4:
		return strcasecmp(i->ui->from, j->ui->from);
	}
}


static int
malloc_ulist(uentp)
USER_INFO *uentp;
{	
	int tmp;

	
	if (num_users >= MAXACTIVE)		/* debug */
		return -1;
		
	if (!HAS_PERM(PERM_CLOAK) && uentp->invisible)
		return -1;

	tmp = cmp_array(&friend_cache, uentp->userid);
	if (tmp == 1)
		friends_number++;
	else if (tmp <= 0 && list_friend)
		return -1;

	pklist[num_users].friend = tmp;
	pklist[num_users].ui = uentp;
#ifdef USE_OVERRIDE_IN_LIST	
/* TODO */
	if (uentp->pager)
	{
		pklist[num_users].override = can_override(uentp->userid, curuser.userid);
	}
	else
		pklist[num_users].override = 0;
//	pklist[num_users].blacklist = in_blacklist(uentp->userid, curuser.userid);
#endif	
	num_users++;

	return 0;
}


time_t pk_mtime;

static int
ulist_max()
{
	if (!pklist)		/* lthuang */
		pklist = (struct pickup *) malloc(sizeof(struct pickup) * MAXACTIVE);

	friends_number = 0;
	num_users = 0;
	apply_ulist(malloc_ulist);
	qsort(pklist, num_users, sizeof(struct pickup), pickup_cmp);
	time(&pk_mtime);	
	return num_users;
}


#ifdef USE_OVERRIDE_IN_LIST
static char
pagerchar(pkent, ident)
struct pickup *pkent;
char ident;
{
	USER_INFO *uentp = pkent->ui;

	//kmwang:20000610:壞人名單
//	if (pkent->blacklist)	// 若在壞人名單中.呼喚鈴必為 '*'
//		return '*';
	if (in_blacklist(uentp->userid, curuser.userid))
		return '*';
	//if (uentp->pager == PAGER_QUIET)
	if (uentp->pager & PAGER_QUIET) /* sarek:03242001:broadcast pager */
		return '*';
	else if (uentp->pager & PAGER_FRIEND)
	{
		if (pkent->override)
			return 'O';
	}
	else if (uentp->pager & PAGER_FIDENT)
	{
		if (pkent->override)
			return 'O';
		if (ident == 7)
			return ' ';
	}
	else
	{
/*	
		if (pkent->override)
			return 'O';
*/			
		return ' ';	
	}
	return '*';
}
#endif

static void
ulist_entry(x, ent, idx, top, last, rows)
int x;
struct pickup ent[];
int idx;
int top, last, rows;
{
	register struct pickup *pkp;
	register int num;
	BOOL update = FALSE;	/* buggy and TODO when page-changed :) */
	int tmp;

	pkp = &(ent[top - idx]);
	for (num = top; num <= last && (num - top) < rows; num++, pkp++)
	{
		uentp = pkp->ui;
		if (uentp->userid[0] && uentp->pid > 2)
		{
			if (pkp->friend && uentp->login_time > pk_mtime)
			{
				if (!HAS_PERM(PERM_CLOAK) && uentp->invisible)
					continue;
				tmp = cmp_array(&friend_cache, uentp->userid);
				if (tmp <= 0)
				{
					friends_number--;
					if (list_friend)
						continue;
					pkp->friend = 0;
					update = TRUE;
				}
			}
			prints("   %4d %s%-12s %-20.20s[m %-15.15s %c%c %-15.15s",
			       num,
			       pkp->friend ? "[1;36m" : "",
/* kmwang:20000628:使用者名單顯示的是fakeid */
#ifndef IGNORE_CASE
                               uentp->userid,
#else
                               strcasecmp(uentp->fakeuserid,uentp->userid)? uentp->userid :
                               uentp->fakeuserid,
#endif
/* sarek:12/30/2000 濾除ANSI控制碼 */
#ifndef KHBBS 
			       (uentp->ident != 7) ? "中山遊客" : (strip_ansi ? esc_filter(uentp->username) : uentp->username),
#else
			       (uentp->ident != 7) ? "高資遊客" : (strip_ansi ? esc_filter(uentp->username) : uentp->username),
#endif

/* TODO 
			       uentp->home,
*/			       
			       uentp->from,
			       (uentp->invisible ? 'C' : ' '),
#ifdef USE_OVERRIDE_IN_LIST			       
			       pagerchar(pkp, curuser.ident),
#else
			       (uentp->pager & 0x00FF) ? '*' : ' ',
#endif			       
			       modestring(uentp, 1));
			if (uentp->idle_time)
				prints(" %2d", (uentp->idle_time > 100) ?
				       99 : uentp->idle_time);
			outs("\n");
		}
		else
			prints("  %5d %s\n", num, _msg_list_19);
	}
	if (update)
		time(&pk_mtime);
}


static void
ulist_title()
{
	title_func((list_friend ? _msg_list_12 : _msg_list_13), BBSTITLE);
	prints(_msg_list_5, _msg_list_4, msg_pickup_way[pickup_way], 
#ifndef IGNORE_CASE
        curuser.userid,
#else
        curuser.fakeuserid,
#endif
	num_users, friends_number); 
	prints(_msg_list_14,
       _msg_list_16, _msg_list_17, HAS_PERM(PERM_CLOAK) ? 'C' : ' ',
       _msg_list_18);
}


static void
ulist_btitle()
{
	prints(_msg_list_20, pagerstring(&uinfo));
}


static int
ulist_get(direct, s, size, top)
char *direct;
void *s;
int size;
int top;
{
	int n = (num_users > ROWSIZE) ? ROWSIZE : num_users - top + 1;

	memcpy(s, &(pklist[top - 1]), n * size);
	return n;
}


static int
ucmd_sort()
{
	int i;
#if 0
	static time_t last_utime = 0;
	static time_t now;
	static int cnt = 0;

	++cnt;
	if (cnt > 4)
		time(&now);
	if (cnt > 5)
	{	
		if (now - last_utime < 3)
		{
			msg("系統忙碌中, 請梢後...");		
			return C_NONE;
		}
		cnt = 0;
	}
	last_utime = now;
#endif

	move(b_line, 0);
	clrtoeol();
	outs(_msg_list_4);
	for (i = 1; i < PICKUP_WAYS; i++)
		prints(" (%d)%s", i, msg_pickup_way[i]);
/*	refresh();*/
	if (getdata(b_line, 58, " ?", genbuf, 2, ECHONOSP, NULL))
	{
		i = atoi(genbuf);
		if (i > 0 && i < PICKUP_WAYS)
		{
			pickup_way = i;
			u_ccur = 0;
			return C_INIT;
		}
	}
	return C_FOOT;
}


static int
ucmd_pager(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	toggle_pager();
	return C_FOOT;
}

/* sarek:03/20/2001:for preventing broadcast */
static int
ucmd_bpager(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	toggle_bpager();
	return C_FOOT;
}


static int
ucmd_help(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	clear();
	outs(_msg_list_6);
	if (curuser.userlevel > PERM_PAGE || curuser.ident == 7)
		outs(_msg_list_7);
	//outs("  [CTRL-D]         切換廣播呼喚鈴\n");	/* lang.h */
#ifndef NSYSUBBS		
	if (HAS_PERM(PERM_SYSOP))
		outs("\n 站長專用鍵\n\
   [i]          把壞蛋踢出去   [E]                  修改網友資料\n");	/* lang.h */
#endif		
#ifdef USE_OVERRIDE_IN_LIST
	outs("\n P欄位說明  *: 拒絕接受呼叫 O: 可接受強力呼叫");	/* lang.h */
#endif	
	pressreturn();
	return C_FULL;
}


static int
ucmd_query(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	uentp = pkent->ui;
	if (uentp->userid[0])
	{
		QueryUser(uentp->userid, uentp);
		return C_FULL;
	}
	return C_NONE;
}


static int
ucmd_mail(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	if (curuser.userlevel)	/* guest 不能 mail */
	{
		char to[STRLEN], title[TTLEN];

		uentp = pkent->ui;
		if (uentp->userid[0])
		{
			clear();
			strcpy(to, uentp->userid);
			title[0] = '\0';
			/* fn_src: NULL, postpath: NULL */
			PrepareMail(NULL, to, title);
			pressreturn();

			return C_FULL;
		}
	}
	return C_NONE;
}


static int
ucmd_review(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	t_review();	/* lthuang */
	return C_FULL;
}


static int
ucmd_refresh(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
#if 0
	static time_t last_utime = 0, now;
	static int cnt = 0;

	++cnt;
	if (cnt > 2)
		time(&now);
	if (cnt > 3)
	{	
		if (now - last_utime < 5)
		{
			msg("請稍後再試...");		
			return C_NONE;
		}
		cnt = 0;
	}
	last_utime = now;
#endif
	return C_INIT;
}


static int
ucmd_addfriend(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	if (curuser.userlevel)	/* guest 不能 add friend */
	{
		uentp = pkent->ui;
		if (uentp->userid[0] && !pkent->friend)
		{
			char uident[IDLEN];
			
			xstrncpy(uident, uentp->userid, sizeof(uident));
			friendAdd(uident, 'F');
			/** 
			 ** friendAdd() will automatically call free_array() to
			 ** to release friend cache
			 **/
			friends_number++;
/*			free_array(&friend_cache);*/
			malloc_array(&friend_cache, ufile_overrides);
			pkent->friend = 1;
			return C_FULL;
		}
	}
	return C_NONE;
}


static int
ucmd_delfriend(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	if (curuser.userlevel)	/* guest 不能 del friend */
	{
		uentp = pkent->ui;
		if (uentp->userid[0] && pkent->friend)
		{
			char uident[IDLEN];
			
			xstrncpy(uident, uentp->userid, sizeof(uident));
			friendDelete(uident, 'F');
			/** 
			 ** friendDelete() will automatically call free_array() to
			 ** to release friend cache
			 **/
			friends_number--;
/*			free_friend(&friend_cache);*/
			malloc_array(&friend_cache, ufile_overrides);			
			pkent->friend = 0;
			return C_FULL;
		}
	}
	return C_NONE;
}


static int
ucmd_switch(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	if (!curuser.userlevel)	/* guest 不能 show friend */
		return C_NONE;

	list_friend ^= 1;
	u_ccur = 0;
	return C_INIT;
}


static int
ucmd_mbox(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	if (!curuser.userlevel)	/* guest 不能 read mail */
		return C_NONE;

	m_read();
	return C_LOAD;
}


static int
ucmd_talk(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	uentp = pkent->ui;
	if (uentp->userid[0])
	{
		if (strcmp(uentp->userid, curuser.userid))
		{
			talk_user(uentp);
			return C_FULL;
		}
	}
	return C_NONE;
}


static int
ucmd_msq(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	uentp = pkent->ui;
	if (uentp->userid[0])
	{
		char userid[IDLEN];
		
		xstrncpy(userid, uentp->userid, sizeof(userid));
		SendMesgToSomeone(userid);
		return C_FOOT;
	}
	return C_NONE;
}


static int
ucmd_fmsq(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	t_fmsq();
	return C_FOOT;
}


#ifndef NSYSUBBS
static int
ucmd_edituser(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	if (!HAS_PERM(PERM_SYSOP))
		return C_NONE;
	set_user_info(pkent->ui->userid);
	return C_FULL;
}


static int
ucmd_kick(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	if (HAS_PERM(PERM_SYSOP))
	{
		uentp = pkent->ui;
		if (uentp->userid[0]
		    && uentp->pid > 2 && kill(uentp->pid, 0) != -1)
		{
			if (uentp->userid[0] != '\0'
			    && strcmp(uentp->userid, curuser.userid))
			{
				if (uentp->pid > 2)
					kill(uentp->pid, SIGKILL);
				/* do not write back user data */
				bbsd_log_write("KICK", "%s", uentp->userid);
				purge_ulist(uentp);
				u_ccur = 0;
				return C_INIT;
			}
		}
	}
	return C_NONE;
}
#endif


static int
ucmd_find(ent, pkent, direct)
int ent;
struct pickup *pkent;
char *direct;
{
	if (getdata(b_line, 0, "尋找 : ", genbuf, 20, XECHO, NULL))	/* lang.h */
	{
		int j = ent % num_users;	
		
		while (j != ent - 1)
		{
			if (strstr(pklist[j].ui->userid, genbuf)
			    || strstr(pklist[j].ui->from, genbuf))
			{
				u_ccur = j + 1;
				return C_MOVE;
			}
			if (++j >= num_users)
				j = 0;
		}
	}
	msg("沒找到!");	/* lang.h */
	getkey();
	return C_FOOT;
}


struct one_key ulist_comms[] =
{
	{'f', ucmd_switch},
	{'s', ucmd_refresh},
	{'l', ucmd_review},
	{'h', ucmd_help},
	{CTRL('P'), ucmd_pager},
	{CTRL('D'), ucmd_bpager}, /* sarek:03/20/2001:for preventing broadcast */
	{'r', ucmd_query},
	{'x', ucmd_mbox},
#ifndef NSYSUBBS	
	{'E', ucmd_edituser},
	{'i', ucmd_kick},
#endif	
	{'u', ucmd_query},
	{'a', ucmd_addfriend},
	{'d', ucmd_delfriend},
	{'\t', ucmd_sort},
	{'m', ucmd_mail},
	{'t', ucmd_talk},
	{'w', ucmd_msq},
	{'b', ucmd_fmsq},
	{'/', ucmd_find},
	{0, NULL}
};


static void
pickup_user()
{
	malloc_array(&friend_cache, ufile_overrides);	
	
	cursor_menu(4, 0, NULL, ulist_comms, sizeof(struct pickup), &u_ccur,
		    ulist_title, ulist_btitle, ulist_entry,
		    ulist_get, ulist_max, NULL, 0, TRUE, SCREEN_SIZE-4);
}


int
t_list()
{
	list_friend = FALSE;
	pickup_user();
	return C_LOAD;
}


int
t_friends()
{
	list_friend = TRUE;
	pickup_user();
	return C_LOAD;
}
