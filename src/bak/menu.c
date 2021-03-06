
#include "bbs.h"
#include "tsbbs.h"


extern struct commands MainMenu[];


/*******************************************************************
 * Admin Menu
 *******************************************************************/
extern int adminMaintainUser(), adminCreateBoard(), adminMaintainBoard(), 
	adminEditConf(), adminListUsers(), adminBroadcast(), adminMailBm();

#ifdef USE_DELUSER
extern int adminDeleteUser();
#endif
#ifdef NSYSUBBS1
extern int adminCancelUser();
#endif
#ifndef NSYSUBBS
extern int adminSyscheck();
#endif

#if 0
extern int adminKickUser();
#endif

struct commands AdminMenu[] =
{
	'i', PERM_SYSOP, NULL, adminMaintainUser, ADMIN, "User Info", "更改使用者資料",
#ifdef USE_DELUSER
	'd', PERM_SYSOP, NULL, adminDeleteUser, ADMIN, "Delete User", "刪除一位使用者",
#endif
	'n', PERM_SYSOP, NULL, adminCreateBoard, ADMIN, "New Board", "建立新看板",
	'm', PERM_SYSOP, NULL, adminMaintainBoard, ADMIN, "Modify Board", "更改/刪除/整理看板設定",
	'e', PERM_SYSOP, NULL, adminEditConf, ADMIN, "Edit Config", "編輯設定檔案",
    'u', PERM_SYSOP, NULL, adminListUsers, LAUSERS, "List All Users", "列出所有使用者",
#if 0    
	'k', PERM_SYSOP, NULL, adminKickUser, ADMIN, "Kick User", "將線上使用者斷線",
#endif	
	'a', PERM_SYSOP, NULL, adminBroadcast, SENDMSG, "BroadCast", "全站廣播",
	'b', PERM_SYSOP, NULL, adminMailBm, SMAIL, "Mail to all BMs", "寄信給所有板主",
#ifdef NSYSUBBS1
	'c', PERM_SYSOP, NULL, adminCancelUser, ADMIN, "Cancel User", "停用使用者帳號",
#endif	
#ifndef NSYSUBBS
	's', PERM_SYSOP, NULL, adminSyscheck, ADMIN, "Manually Check Ident", "手動身份認證",
#endif	
#ifdef ANIMEBBS
	'a', PERM_SYSOP, MainMenu, NULL, ADMIN, "Admin Menu", "神官密殿",
#else
	'a', PERM_SYSOP, MainMenu, NULL, ADMIN, "Admin Menu", "管理者辦公室",
#endif	
	0, PERM_SYSOP, NULL, NULL, 0, NULL, NULL
};


/*******************************************************************
 * Xyz Menu
 *******************************************************************/
extern int x_info(), x_override(), x_signature(), x_plan(), x_date(), x_viewnote(),
    x_uflag();

#ifdef USE_IDENT
extern int x_idcheck();
#endif


struct commands XyzMenu[] =
{
	'd', 1, NULL, x_info, 0, "Personal Data", "顯示與修改個人基本資料",
	'u', 0, NULL, x_uflag, 0, "User Option", "切換個人使用設定",
	'o', 1, NULL, x_override, OVERRIDE, "Override Edit", "編輯好朋友名單",
	's', 1, NULL, x_signature, EDITSIG, "Signature Edit", "編輯簽名檔",
	'n', 1, NULL, x_plan, EDITPLAN, "Plan Edit", "編輯名片檔",
	't', 0, NULL, x_date, 0, "Time Now", "顯示現在系統時間",
	'v', 0, NULL, x_viewnote, 0, "View Note", "查閱\留言",
#ifdef USE_IDENT
	'c', 1, NULL, x_idcheck, 0, "ID Check", "進行身份確認",
#endif
#ifdef ANIMEBBS
	'x', 0, MainMenu, NULL, XMENU, "Xyz Menu", "工具街",
#else
	'x', 0, MainMenu, NULL, XMENU, "Xyz Menu", "個人資料維護工具箱",
#endif	
	0, 0, NULL, NULL, 0, NULL, NULL
};


/*******************************************************************
 * Mail Menu
 *******************************************************************/
extern int m_new(), m_read(), m_send(), m_group();

struct commands MailMenu[] =
{
	'n', 1, NULL, m_new, RMAIL, "New Mails", "只讀新的信",
	'r', 1, NULL, m_read, RMAIL, "Read Mails", "檢視所有信件",
	's', 1, NULL, m_send, SMAIL, "Send Mail", "寄信",
	'g', 1, NULL, m_group, SMAIL, "Mail to Group", "寄信給多人",
#ifdef ANIMEBBS
	'm', 0, MainMenu, NULL, MAIL, "Mail Menu", "魚雁往返亭",
#else
	'm', 0, MainMenu, NULL, MAIL, "Mail Menu", "個人信箱",
#endif	
	0, 0, NULL, NULL, 0, NULL, NULL
};


/*******************************************************************
 * Talk Menu
 *******************************************************************/
extern int t_query(), t_talk(), t_chat(), x_override(), t_list(), t_friends(),
    t_message(), t_fsendmsg(), t_review(), t_pager();


struct commands TalkMenu[] =
{
     'u', 0, NULL, t_list, LUSERS, "Online Users", "選單列出正在線上的使用者",
  'f', 1, NULL, t_friends, LFRIENDS, "Friends Online", "列出正在線上的老朋友",
	'q', 0, NULL, t_query, QUERY, "Query User", "查詢使用者個人資料細項",
	'p', 1, NULL, t_pager, PAGE, "Pager Switch", "切換聊天呼喚鈴",
	't', 1, NULL, t_talk, PAGE, "Talk", "雙人談心雅室",
	'w', 1, NULL, t_message, SENDMSG, "Send Message", "線上送訊息",
	'b', 1, NULL, t_fsendmsg, SENDMSG, "BroadCast", "送訊息給好友",
	'r', 1, NULL, t_review, SENDMSG, "Review Message", "回顧線上訊息",
	'c', 3, NULL, t_chat, CHATROOM, "BBS Chat Room", "站內聊天廣場",
	't', 0, MainMenu, NULL, TMENU, "Talk Menu", "休閒聊天園地",
	0, 0, NULL, NULL, 0, NULL, NULL
};


/*******************************************************************
 * Main Menu
 *******************************************************************/
extern int Select(), Goodbye(), Boards(), Class(), Announce(), MainRead(),
	x_lang();

#ifdef ANIMEBBS
struct commands MainMenu[] =
{
	'c', 0, NULL, Class, CLASS_MENU, "Class", "伊莉琴斯分區圖",
	'n', 0, NULL, Announce, READING, "Announce", "神殿發言人",
	'0', 0, NULL, Boards, BOARDS_MENU, "Boards", "伊莉琴斯鳥瞰圖",
	's', 0, NULL, Select, SELECT, "Select", "伊莉琴斯任意門",
	'r', 0, NULL, MainRead, READING, "Read", "讀取佈告",
	't', 0, &(TalkMenu[0]), NULL, TMENU, "Talk Menu", "有緣千里相逢廳",
	'm', 1, &(MailMenu[0]), NULL, MAIL, "Mail Menu", "魚雁往返亭",
	'x', 0, &(XyzMenu[0]), NULL, XMENU, "Xyz Utilities", "工具街",
	'l', 0, NULL, x_lang, 0, "Language switch", "切換語言版本",
	'g', 0, NULL, Goodbye, 0, "Goodbye", "小別伊莉琴斯",
	'a', PERM_SYSOP, &(AdminMenu[0]), NULL, ADMIN, "Admin Menu", "神官密殿",
	'0', 0, NULL, NULL, MMENU, "Main Menu", "主選單",
	0, 0, NULL, NULL, 0, NULL, NULL
};
#else
struct commands MainMenu[] =
{
	'c', 0, NULL, Class, CLASS_MENU, "Class", "分類式佈告欄選擇",
	'n', 0, NULL, Announce, READING, "Announce", "進站公告",
	'0', 0, NULL, Boards, BOARDS_MENU, "Boards", "游標式佈告欄選單",
	's', 0, NULL, Select, SELECT, "Select", "輸入式佈告欄選擇",
	'r', 0, NULL, MainRead, READING, "Read", "讀取佈告",
	't', 0, &(TalkMenu[0]), NULL, TMENU, "Talk Menu", "休閒聊天園地",
	'm', 1, &(MailMenu[0]), NULL, MAIL, "Mail Menu", "個人郵件信箱",
    'x', 0, &(XyzMenu[0]), NULL, XMENU, "Xyz Utilities", "個人資料維護工具箱",
	'l', 0, NULL, x_lang, 0, "Language switch", "切換語言版本",
	'g', 0, NULL, Goodbye, 0, "Goodbye", "再見，我的朋友",
	'a', PERM_SYSOP, &(AdminMenu[0]), NULL, ADMIN, "Admin Menu", "管理者辦公室",
	'0', 0, NULL, NULL, MMENU, "Main Menu", "主選單",
	0, 0, NULL, NULL, 0, NULL, NULL
};
#endif


/*******************************************************************
 * Menu Operation
 *******************************************************************/

int menu_depth = 1;
static int n_cmenus = 0;
struct commands *cmenus = MainMenu;
short redraw;


#ifdef USE_MENUSHOW
int pict_no = 0;
struct MenuShowShm *msshm;
#endif

static void
menu_title()
{
	title_func(cmenus[n_cmenus - 1].chelp, BBSTITLE);

#ifdef USE_MENUSHOW
	if (redraw && !(curuser.flags[0] & PICTURE_FLAG))
	{
		if (msshm == NULL)
			msshm = (struct MenuShowShm *) attach_shm(MENUSHOW_KEY, sizeof(struct MenuShowShm));
		if (msshm && msshm->number)
		{
			int j;
			char *p1, *p2;
			unsigned char chs;
			static long randomseed = 1;

			/*  
			 * This is a simple linear congruential random number
			 * generator.  Hence, it is a bad random number
			 * generator, but good enough for most randomized
			 * geometric algorithms. modify by lthuang
			 */
			randomseed = (randomseed * 1366l + 150889l) % 714025l;
			pict_no = randomseed / (714025l / msshm->number + 1);

/*
 * j = n_cmenus + 2;
 * if (j < 13)
 * j = 13;
 */
			move(10, 69 - strlen(msshm->list[pict_no].owner));
			prints("《提供》%s ", msshm->list[pict_no].owner);
			j = 2;
			move(j, 0);
/*                      
 * prints("[7m《作者》%35.35s  《標題》%27.27s[m",
 * msshm->list[pict_no].owner, msshm->list[pict_no].title);
 */
			p1 = msshm->list[pict_no].body;

/*
 * while (j++ < b_line)
 */
			while (++j < 11)
			{
				if (p2 = strchr(p1, '\n'))
					p1 = ++p2;
				else
					break;
			}
			/* 
			 * Direct output the content in shared memory,
			 * for better performance. by lthuang
			 */
			j = p1 - msshm->list[pict_no].body;
			p1 = msshm->list[pict_no].body;
			while (j-- > 0 && (chs = *p1++) != '\0')
				outc(chs);
			outs("[m");

			redraw = FALSE;
		}
/*              
 * move(b_line, 0); 
 */
	}
#endif /* USE_MENUSHOW */
}


static void
menu_btitle()
{
	prints(_msg_menu_2, pagerstring(&uinfo));
}


static void
menu_entry(x, ent, idx, top, last, rows)
int x;
struct commands ent[];
int idx;
int top, last, rows;
{
	int i, num;

	for (num = top; num <= last; num++)
	{
		i = num - top;
		if (i >= rows)
			break;

		i = x;
		while (i-- > 0)
			outc(' ');

		i = num - idx;
		prints("   ([1;3%cm%c[m) %-23s %s\n",
		       (menu_depth == 1) ? '2' : '6',
		       ent[i].key, ent[i].ehelp, ent[i].chelp);
	}
}


static int
menu_max(direct, size)
char *direct;
int size;
{
	int i, j;

	for (i = 0, j = 0; cmenus[i].key; i++)
	{
#ifdef USE_IDENT
		if (cmenus[i].cfunc == x_idcheck && curuser.ident == 7)
			continue;
#endif
		if (curuser.userlevel < cmenus[i].level)
			continue;
#ifdef NSYSUBBS
		if (cmenus[i].level == PERM_SYSOP)
		{
			extern BOOL IsRealSysop;

			if (!IsRealSysop)
				continue;
			if (strncmp(uinfo.from, "140.117.12.", 11)
#if defined(NSYSUBBS2)
			    && strncmp(uinfo.from, "140.117.11.14", 13)
#endif
			    && strncmp(uinfo.from, "127.0.0.1", 9)
			    && strncmp(uinfo.from, "140.117.11.1", 12)
			    && strncmp(uinfo.from, "140.117.18.61", 13))
			{
				continue;
			}
		}
#endif
		if (j != i)
			memcpy(&(cmenus[j]), &(cmenus[i]), sizeof(struct commands));
		j++;
	}
	if (j != i)
		memcpy(&(cmenus[j]), &(cmenus[i]), sizeof(struct commands));
	update_umode(cmenus[j - 1].mode);

#if 0
	if (uinfo.mode == ADMIN && !HAS_PERM(PERM_SYSOP))	/* debug */
	{
		bbsd_log_write("ERR", "not sysop, but enter (A)dmin");
		exit(0);
	}
#endif

	n_cmenus = j;
	return j - 1;
}


static int
menu_get(direct, s, size, top)
char *direct;
void *s;
int size;
int top;
{
	int n = n_cmenus - top /*+ 1*/;
	extern int autoch;

	/* if new mail available, key move to the function automatically */
	if (menu_depth == 1 && CheckNewmail(curuser.userid, FALSE))
		autoch = 'm';

	if (n > ROWSIZE)
		n = ROWSIZE;
	memcpy(s, cmenus, n * size);
	return n;
}


static int
menu_findkey(nbuf, ent, start, total)
char *nbuf;
struct commands ent[];
{
	register int i;

	for (i = start; i < total; i++)
	{
		if (ent[i].key == *nbuf)
		{
			nbuf[0] = '\0';
			return (i + 1);
		}
	}
	for (i = 0; i < start; i++)
	{
		if (ent[i].key == *nbuf)
		{
			nbuf[0] = '\0';
			return (i + 1);
		}
	}
	nbuf[0] = '\0';
	return -1;
}


static int
mcmd_menushow(ent, cent, direct)
int ent;
struct commands *cent;
char *direct;
{
	if (!(curuser.flags[0] & PICTURE_FLAG))
	{
		more(msshm->list[pict_no].filename, TRUE);
		redraw = TRUE;
		return C_FULL;
	}
	return C_NONE;
}


static int
mcmd_enter(ent, cent, direct)
int ent;
struct commands *cent;
char *direct;
{
	if (cent->comm)
	{
		cmenus = cent->comm;	/* 進入子選單 */
		menu_depth++;
		return C_REDO;
	}
	update_umode(cent->mode);
	return (*(cent->cfunc)) ();
}



struct one_key menu_comms[] =
{
	'\t', mcmd_menushow,
	'r', mcmd_enter,
	0, NULL
};


void
domenu()
{
	int m_ccur[4];

#ifdef USE_VOTE
	CheckNewSysVote();
#endif

	memset(m_ccur, 0, sizeof(m_ccur));

	for (;;)
	{
		redraw = TRUE;

		if (cursor_menu(11, 13, NULL, menu_comms, sizeof(struct commands),
				&(m_ccur[menu_depth - 1]),
				menu_title, menu_btitle, menu_entry, menu_get, menu_max,
				menu_findkey, 0, TRUE, SCREEN_SIZE-4) == 0)
		{
			if (menu_depth == 1)
			{
				Goodbye();
			}
			else
			{
				menu_depth--;	/* 退回上層選單 */
				m_ccur[menu_depth] = 0;
				cmenus = cmenus[n_cmenus - 1].comm;
			}
		}
	}
}
