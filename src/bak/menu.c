
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
	'i', PERM_SYSOP, NULL, adminMaintainUser, ADMIN, "User Info", "���ϥΪ̸��",
#ifdef USE_DELUSER
	'd', PERM_SYSOP, NULL, adminDeleteUser, ADMIN, "Delete User", "�R���@��ϥΪ�",
#endif
	'n', PERM_SYSOP, NULL, adminCreateBoard, ADMIN, "New Board", "�إ߷s�ݪO",
	'm', PERM_SYSOP, NULL, adminMaintainBoard, ADMIN, "Modify Board", "���/�R��/��z�ݪO�]�w",
	'e', PERM_SYSOP, NULL, adminEditConf, ADMIN, "Edit Config", "�s��]�w�ɮ�",
    'u', PERM_SYSOP, NULL, adminListUsers, LAUSERS, "List All Users", "�C�X�Ҧ��ϥΪ�",
#if 0    
	'k', PERM_SYSOP, NULL, adminKickUser, ADMIN, "Kick User", "�N�u�W�ϥΪ��_�u",
#endif	
	'a', PERM_SYSOP, NULL, adminBroadcast, SENDMSG, "BroadCast", "�����s��",
	'b', PERM_SYSOP, NULL, adminMailBm, SMAIL, "Mail to all BMs", "�H�H���Ҧ��O�D",
#ifdef NSYSUBBS1
	'c', PERM_SYSOP, NULL, adminCancelUser, ADMIN, "Cancel User", "���ΨϥΪ̱b��",
#endif	
#ifndef NSYSUBBS
	's', PERM_SYSOP, NULL, adminSyscheck, ADMIN, "Manually Check Ident", "��ʨ����{��",
#endif	
#ifdef ANIMEBBS
	'a', PERM_SYSOP, MainMenu, NULL, ADMIN, "Admin Menu", "���x�K��",
#else
	'a', PERM_SYSOP, MainMenu, NULL, ADMIN, "Admin Menu", "�޲z�̿줽��",
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
	'd', 1, NULL, x_info, 0, "Personal Data", "��ܻP�ק�ӤH�򥻸��",
	'u', 0, NULL, x_uflag, 0, "User Option", "�����ӤH�ϥγ]�w",
	'o', 1, NULL, x_override, OVERRIDE, "Override Edit", "�s��n�B�ͦW��",
	's', 1, NULL, x_signature, EDITSIG, "Signature Edit", "�s��ñ�W��",
	'n', 1, NULL, x_plan, EDITPLAN, "Plan Edit", "�s��W����",
	't', 0, NULL, x_date, 0, "Time Now", "��ܲ{�b�t�ήɶ�",
	'v', 0, NULL, x_viewnote, 0, "View Note", "�d�\\�d��",
#ifdef USE_IDENT
	'c', 1, NULL, x_idcheck, 0, "ID Check", "�i�樭���T�{",
#endif
#ifdef ANIMEBBS
	'x', 0, MainMenu, NULL, XMENU, "Xyz Menu", "�u���",
#else
	'x', 0, MainMenu, NULL, XMENU, "Xyz Menu", "�ӤH��ƺ��@�u��c",
#endif	
	0, 0, NULL, NULL, 0, NULL, NULL
};


/*******************************************************************
 * Mail Menu
 *******************************************************************/
extern int m_new(), m_read(), m_send(), m_group();

struct commands MailMenu[] =
{
	'n', 1, NULL, m_new, RMAIL, "New Mails", "�uŪ�s���H",
	'r', 1, NULL, m_read, RMAIL, "Read Mails", "�˵��Ҧ��H��",
	's', 1, NULL, m_send, SMAIL, "Send Mail", "�H�H",
	'g', 1, NULL, m_group, SMAIL, "Mail to Group", "�H�H���h�H",
#ifdef ANIMEBBS
	'm', 0, MainMenu, NULL, MAIL, "Mail Menu", "��������F",
#else
	'm', 0, MainMenu, NULL, MAIL, "Mail Menu", "�ӤH�H�c",
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
     'u', 0, NULL, t_list, LUSERS, "Online Users", "���C�X���b�u�W���ϥΪ�",
  'f', 1, NULL, t_friends, LFRIENDS, "Friends Online", "�C�X���b�u�W���ѪB��",
	'q', 0, NULL, t_query, QUERY, "Query User", "�d�ߨϥΪ̭ӤH��ƲӶ�",
	'p', 1, NULL, t_pager, PAGE, "Pager Switch", "������ѩI��a",
	't', 1, NULL, t_talk, PAGE, "Talk", "���H�ͤ߶���",
	'w', 1, NULL, t_message, SENDMSG, "Send Message", "�u�W�e�T��",
	'b', 1, NULL, t_fsendmsg, SENDMSG, "BroadCast", "�e�T�����n��",
	'r', 1, NULL, t_review, SENDMSG, "Review Message", "�^�U�u�W�T��",
	'c', 3, NULL, t_chat, CHATROOM, "BBS Chat Room", "������Ѽs��",
	't', 0, MainMenu, NULL, TMENU, "Talk Menu", "�𶢲�Ѷ�a",
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
	'c', 0, NULL, Class, CLASS_MENU, "Class", "����^�����Ϲ�",
	'n', 0, NULL, Announce, READING, "Announce", "�����o���H",
	'0', 0, NULL, Boards, BOARDS_MENU, "Boards", "����^��������",
	's', 0, NULL, Select, SELECT, "Select", "����^�����N��",
	'r', 0, NULL, MainRead, READING, "Read", "Ū���G�i",
	't', 0, &(TalkMenu[0]), NULL, TMENU, "Talk Menu", "���t�d���۳{�U",
	'm', 1, &(MailMenu[0]), NULL, MAIL, "Mail Menu", "��������F",
	'x', 0, &(XyzMenu[0]), NULL, XMENU, "Xyz Utilities", "�u���",
	'l', 0, NULL, x_lang, 0, "Language switch", "�����y������",
	'g', 0, NULL, Goodbye, 0, "Goodbye", "�p�O����^��",
	'a', PERM_SYSOP, &(AdminMenu[0]), NULL, ADMIN, "Admin Menu", "���x�K��",
	'0', 0, NULL, NULL, MMENU, "Main Menu", "�D���",
	0, 0, NULL, NULL, 0, NULL, NULL
};
#else
struct commands MainMenu[] =
{
	'c', 0, NULL, Class, CLASS_MENU, "Class", "�������G�i����",
	'n', 0, NULL, Announce, READING, "Announce", "�i�����i",
	'0', 0, NULL, Boards, BOARDS_MENU, "Boards", "��Ц��G�i����",
	's', 0, NULL, Select, SELECT, "Select", "��J���G�i����",
	'r', 0, NULL, MainRead, READING, "Read", "Ū���G�i",
	't', 0, &(TalkMenu[0]), NULL, TMENU, "Talk Menu", "�𶢲�Ѷ�a",
	'm', 1, &(MailMenu[0]), NULL, MAIL, "Mail Menu", "�ӤH�l��H�c",
    'x', 0, &(XyzMenu[0]), NULL, XMENU, "Xyz Utilities", "�ӤH��ƺ��@�u��c",
	'l', 0, NULL, x_lang, 0, "Language switch", "�����y������",
	'g', 0, NULL, Goodbye, 0, "Goodbye", "�A���A�ڪ��B��",
	'a', PERM_SYSOP, &(AdminMenu[0]), NULL, ADMIN, "Admin Menu", "�޲z�̿줽��",
	'0', 0, NULL, NULL, MMENU, "Main Menu", "�D���",
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
			prints("�m���ѡn%s ", msshm->list[pict_no].owner);
			j = 2;
			move(j, 0);
/*                      
 * prints("[7m�m�@�̡n%35.35s  �m���D�n%27.27s[m",
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
		cmenus = cent->comm;	/* �i�J�l��� */
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
				menu_depth--;	/* �h�^�W�h��� */
				m_ccur[menu_depth] = 0;
				cmenus = cmenus[n_cmenus - 1].comm;
			}
		}
	}
}