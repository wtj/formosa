/*
 * written by lthuang@cc.nsysu.edu.tw
 */

#include "bbs.h"
#include "tsbbs.h"

/* TODO: ╰参щ布, 琌Τ穝щ布, 秨布笆眎禟挡狦 */

int Tickets = 0;
int num_of_Box_vbits;

char Title[STRLEN] = "";
BOOL show_info = 0;


struct ticket
{
	char userid[IDLEN];
	unsigned long vbits;
}
Box;


struct newvote
{
	char userid[IDLEN];
	time_t rtime;
};

struct newvote nv;


typedef struct _cand
{
	char item[STRLEN];
}
CAND;


typedef struct _vote
{
	char filename[STRLEN];
	char title[STRLEN];
	char owner[IDLEN];
	time_t start_time;
	time_t end_time;
	int tickets;
	char allow_ip[40];
	unsigned int userlevel;
	char ident;
	time_t firstlogin;
	unsigned int numlogins;
	unsigned int numposts;
}
VOTEHEADER;


static void
setvotefile2(fname, direct, vf, f)
char *fname, *direct, *vf, *f;
{
	setdotfile(fname, direct, vf);
	strcat(fname, f);
}


static void
set_timestr(s, tp)
char *s;
time_t *tp;
{
	struct tm *tm;

	tm = localtime(tp);
	sprintf(s, "%02d.%02d.%02d",
		tm->tm_year - 11, tm->tm_mon + 1, tm->tm_mday);
}		


static int
count_box(path, vinfo)
char *path;
VOTEHEADER *vinfo;
{
	FILE *fp;
	CAND cand;
	char stime[40], etime[40], rtime[40];
	int fd, i, num[32], n_users = 0, n_tickets = 0;
	unsigned long j;


	memset(num, 0, sizeof(num));
	/* 布絚い计眔布计 */
	sprintf(genbuf, "%s/box", path);
	if ((fd = open(genbuf, O_RDONLY)) > 0)
	{
		while (read(fd, &Box, sizeof(Box)) == sizeof(Box))
		{
			n_users++;

			for (j = 1, i = 0; j != 0; j <<= 1, i++)
			{
				if (Box.vbits & j)
				{
					num[i] += 1;
					n_tickets += 1;
				}
			}
		}
		close(fd);
	}

	sprintf(genbuf, "%s/.CAND", path);
	if ((fd = open(genbuf, O_RDONLY)) < 0)
		return -1;

	sprintf(genbuf, "%s/result", path);
	if ((fp = fopen(genbuf, "w")) == NULL)
	{
		close(fd);
		return -1;
	}

	set_timestr(stime, &(vinfo->start_time));
	set_timestr(etime, &(vinfo->end_time));

	fprintf(fp, "[1;33;44m %-30.30s [1;37m%12s %8s - %8s %12s [m\n",
		"匡羭嘿", "羭快", "щ布秨﹍", "秨布丁", "–щ布计");
	fprintf(fp, " %-30.30s %12s %8s - %8s %12d\n",
		vinfo->title, vinfo->owner, stime, etime,
		vinfo->tickets);

	fprintf(fp, "\n щ布                     %15s %4s %6s %6s %8s\n",
		"ㄓ方", "单", "计", "眎禟计", "爹丁");

	if (vinfo->firstlogin)
		set_timestr(rtime, &(vinfo->firstlogin));
	else
		strcpy(rtime, "ぃ");

	fprintf(fp, "                                  %15s %4d %6d %6d %8s\n",
		*(vinfo->allow_ip) ? vinfo->allow_ip : "ぃ",
		vinfo->userlevel, vinfo->numlogins, vinfo->numposts, rtime);

	fprintf(fp, "\n[1;37;42m      %4s  %6s    %-55s [m\n",
		"絪腹", "布计", "嘿");
	/* щ布计挡狦 */
	for (i = 0; read(fd, &cand, sizeof(cand)) == sizeof(cand); i++)
		fprintf(fp, "      %4d%6d    %s\n", i + 1, num[i], cand.item);

	fprintf(fp, "    羆布计%6d\n", n_tickets);
	fprintf(fp, "    羆计%6d\n", n_users);

	close(fd);	
	fclose(fp);
}


#if 0
int
UnSeenVote(bname)
char *bname;
{
	int fd;
	VOTEHEADER vh;
	time_t rtime = 0;
	int total;


	setvotefile(genbuf, bname, VOTE_REC);
	total = get_num_records(genbuf, sizeof(vh));
	if (total == 0)
		return 0;

	get_record(genbuf, &vh, sizeof(vh), total);

	rtime = 0;
	setboardfile(genbuf, bname, "vote/men");
	if ((fd = open(genbuf, O_RDONLY)) > 0)
	{
		while (read(fd, &nv, sizeof(nv)) == sizeof(nv))
		{
			if (!strcmp(nv.userid, curuser.userid))
			{
				rtime = nv.rtime;
				break;
			}
		}
		close(fd);
	}

	return (atol(vh.filename + 2) > rtime);
}
#endif


void
DisplayNewVoteMesg()
{
	more("vote/NEWMSG", TRUE);
}


void
CheckNewSysVote()
{
	static BOOL sysvoting = FALSE;

	if (!sysvoting)	/* ? */
	{
		if (is_new_vote("sysop", curuser.lastlogin))
			sysvoting = TRUE;
	}
	if (sysvoting)
		DisplayNewVoteMesg();
}


static void
vote_title()
{
	title_func(BBSTITLE, "щ布跋");
	outs(_msg_vote_17);
	if (show_info)
		prints("[7m  %4s %-14.14s %12s %15s %4s %6s %6s %8s [m",
		       "絪腹", "щ布嘿", "羭快", "ㄓ方", "单",
		       "计", "眎禟计", "爹丁");
	else
		prints("[7m  %4s %-38.38s %4s %8s   %8s %8s[m",
		       "絪腹", "щ布嘿", "粄靡", "щ布秨﹍", "箇璸秨布", "–布计");
}


static void
vote_entry(x, ent, idx, top, last, rows)
int x;
VOTEHEADER ent[];
int idx;
int top, last, rows;
{
	int i, num;
	char stime[40], etime[40], rtime[40];

	for (num = top; num <= last; num++)
	{
		i = num - top;
		if (i >= rows)
			break;

		i = num - idx;

		if (show_info)
		{
/*              
 * if (HAS_PERM(PERM_SYSOP))
 * prints("ヘ魁 : %s\n\n", vinfo->filename);
 */
			if (ent[i].firstlogin)
				set_timestr(rtime, &(ent[i].firstlogin));
			else
				strcpy(rtime, "ぃ");

			prints("   %3d %-14.14s %12s %15s %4d %6d %6d %8s\n",
			       num, ent[i].title, ent[i].owner,
			       *(ent[i].allow_ip) ? ent[i].allow_ip : "ぃ",
			       ent[i].userlevel, ent[i].numlogins, ent[i].numposts,
			       rtime);
		}
		else
		{
			set_timestr(stime, &(ent[i].start_time));
			set_timestr(etime, &(ent[i].end_time));

			prints("   %3d %-39.39s %s %s - %s   %2d\n",
			       num, ent[i].title, (ent[i].ident == 7) ? _str_marker : "  ",
			       stime, etime, ent[i].tickets);
		}
	}
}


static void
cand_title()
{
	title_func("щ布跋", Title);
	outs("\n(△)(Enter)匡﹚щ布兜ヘ (■)(q)瞒秨\n\
(a)糤匡兜ヘ� (d)埃匡兜ヘ (E)э匡兜ヘ\n\
[7m  絪腹 伴匡 匡兜ヘ                                                          [m");
}


static void
cand_entry(x, ent, idx, top, last, rows)
int x;
CAND ent[];
int idx;
int top, last, rows;
{
	int i, num;
	unsigned long j;

	for (num = top; num <= last; num++)
	{
		if (num - top >= rows)
			break;
		i = num - idx;

		j = 1 << (num - 1);
		prints("   %3d  %s  %s\n",
		       num, (Box.vbits & j) ? "》" : "  ", ent[i].item);
	}
}


static int
ccmd_enter(ent, cinfo, direct)
int ent;
CAND *cinfo;
char *direct;
{
	/* ぃщ罢 */
	if (num_of_Box_vbits >= Tickets)
	{
		msg(_msg_vote_7, Tickets);
		getkey();
		return C_FULL;
	}

	/* 盢布щ既絚 */
	if (!(Box.vbits & (1 << (ent - 1))))
	{
		Box.vbits |= 1 << (ent - 1);
		strcpy(Box.userid, curuser.userid);
		num_of_Box_vbits++;
	}

	/* 恶糶щ布種ǎ */
	move(10, 0);
	clrtobot();
	if (getdata(10, 0, "眤癸セΩщ布Τㄤウ種ǎ盾? [n]: ", genbuf, 2,
		    ECHONOSP, NULL) && genbuf[0] == 'y')
	{
		char buf[PATHLEN];

		if (getdata(11, 0, "", buf, 60, DOECHO, NULL))
		{
			sprintf(genbuf, "\n%s 弧%s", curuser.userid, buf);
			setdotfile(buf, direct, "mess");
			append_record(buf, genbuf, strlen(genbuf));
		}
	}
	return C_FULL;
}


static int
ccmd_add(ent, cinfo, direct)
int ent;
CAND *cinfo;
char *direct;
{
	CAND ch;

	if (!hasBMPerm)
		return C_NONE;

	if (get_num_records(direct, sizeof(*cinfo)) > sizeof(Box.vbits) * 8)
	{
		msg("匡兜ヘ程 %d 兜", sizeof(Box.vbits) * 8);
		getkey();
		return C_FOOT;
	}

	memset(&ch, 0, sizeof(ch));
	if (getdata(b_line, 0, "匡兜ヘ: ", ch.item, sizeof(ch.item), DOECHO,
		    NULL))
	{
		append_record(direct, &ch, sizeof(ch));
		return C_INIT;
	}
	return C_FOOT;
}


static int
ccmd_edit(ent, cinfo, direct)
int ent;
CAND *cinfo;
char *direct;
{
	if (!hasBMPerm)
		return C_NONE;

	if (getdata(b_line, 0, "匡兜ヘ: ", cinfo->item, sizeof(cinfo->item),
		    DOECHO, cinfo->item))
	{
		substitute_record(direct, cinfo, sizeof(*cinfo), ent);
		return C_INIT;
	}
	return C_FOOT;
}


static int
ccmd_delete(ent, cinfo, direct)
int ent;
CAND *cinfo;
char *direct;
{
	if (!hasBMPerm)
		return C_NONE;

	msg("Are you sure ? [n]: ");
	if (igetkey() == 'y')
	{
		/* 埃匡兜ヘ */
		delete_record(direct, sizeof(*cinfo), ent);
		return C_INIT;
	}
	return C_FOOT;
}


struct one_key cand_comms[] =
{
	'a', ccmd_add,
	'r', ccmd_enter,
	'd', ccmd_delete,
	'E', ccmd_edit,
	0, NULL
};


static int
vcmd_desc(ent, vinfo, direct)
int ent;
VOTEHEADER *vinfo;
char *direct;
{
	/* 匡羭弧 */
	setvotefile2(genbuf, direct, vinfo->filename, "/desc");
	more(genbuf, TRUE);
	return C_FULL;
}


static int
vcmd_enter(ent, vinfo, direct)
int ent;
VOTEHEADER *vinfo;
char *direct;
{
	char tmpdir[PATHLEN];
	int ca_ccur = 0;
	int fd;
	unsigned long j;

	memset(&Box, 0, sizeof(Box));

	/* 琌щ筁布 */
	setvotefile2(genbuf, direct, vinfo->filename, "/box");
	if ((fd = open(genbuf, O_RDONLY)) > 0)
	{
		while (read(fd, &Box, sizeof(Box)) == sizeof(Box))
		{
			if (!strcmp(Box.userid, curuser.userid))
				break;
			memset(&Box, 0, sizeof(Box));
		}
		close(fd);
	}

	/* 璝瞒秨, userid ぃ 0, ボㄏノΤщ布絚笆祇ネ */
	Box.userid[0] = '\0';
	/* 计眤щ碭布 */
	for (j = 1, num_of_Box_vbits = 0; j != 0; j <<= 1)
	{
		if (Box.vbits & j)
			num_of_Box_vbits++;
	}

	if (time(0) > vinfo->end_time)
	{
		showmsg("щ布笆Ν挡!");
		return C_FULL;
	}

	if (curuser.userlevel < vinfo->userlevel
	    || strncmp(uinfo.from, vinfo->allow_ip, strlen(vinfo->allow_ip))
	    || curuser.ident < vinfo->ident
	    || curuser.numlogins < vinfo->numlogins
	    || curuser.numposts < vinfo->numposts
	    || (vinfo->firstlogin && curuser.firstlogin > vinfo->firstlogin))
	{
		showmsg("╆簆, Ωщ布笆Τ癸禜, ォ眤礚猭把!");
		return C_FULL;
	}

	/* 把щ布玡, 陪ボΩщ布弧 */
	vcmd_desc(ent, vinfo, direct);

	Tickets = vinfo->tickets;
	strcpy(Title, vinfo->title);
	setvotefile2(tmpdir, direct, vinfo->filename, "/.CAND");

	cursor_menu(4, 0, tmpdir, cand_comms, sizeof(CAND), &ca_ccur,
	    cand_title, NULL /* cm_btitle */ , cand_entry, read_get, read_max,
		    NULL, 1, TRUE, SCREEN_SIZE-4);

	/* ищ筁布舘! */
	if (Box.userid[0])
	{
		/* 糶布絚 */
		setdotfile(genbuf, tmpdir, "/box");
		append_record(genbuf, &Box, sizeof(Box));
	}

	if (curbe->voting)
	{
		setboardfile(genbuf, CurBList->filename, "vote/men");
		if ((fd = open(genbuf, O_RDWR | O_CREAT)) > 0)
		{
			while (read(fd, &nv, sizeof(nv)) == sizeof(nv))
			{
				if (!strcmp(nv.userid, curuser.userid))
				{
					lseek(fd, -sizeof(nv), SEEK_CUR);
					break;
				}
			}
			strcpy(nv.userid, curuser.userid);
			time(&nv.rtime);
			write(fd, &nv, sizeof(nv));
			close(fd);
	#if 1
		chmod(genbuf, 0600);
	#endif
		}
		curbe->voting = 0;
	}

	return C_LOAD;
}


static time_t
get_time(str)
char *str;
{
	struct tm tm;

	memset(&tm, 0, sizeof(tm));
	sscanf(str, "%02d.%02d.%02d",
	       &(tm.tm_year), &(tm.tm_mon), &(tm.tm_mday));
	tm.tm_year += 11;
	tm.tm_mon -= 1;
	return mktime(&tm);
}


static int
set_vote(direct, vinfo)
char *direct;
VOTEHEADER *vinfo;
{
	char filename[PATHLEN];

	move(1, 0);
	clrtobot();

	outs("<叫猔種> 砞﹚い璝赣兜ぃ叫 Enter 菠筁, ┮Τ丁チ瓣");

	if (!getdata(3, 0, "щ布嘿 : ", genbuf, sizeof(vinfo->title), DOECHO, vinfo->title))
		return -1;
	strcpy(vinfo->title, genbuf);

	if (vinfo->start_time == 0)
		vinfo->start_time = time(0);
	set_timestr(genbuf, &(vinfo->start_time));
	if (getdata(4, 0, "щ布秨﹍(yy.mm.dd) : ", genbuf, 9, ECHONOSP, genbuf))
		vinfo->start_time = get_time(genbuf);

	if (vinfo->end_time == 0)
		vinfo->end_time = time(0) + 86400;
	set_timestr(genbuf, &(vinfo->end_time));
	if (getdata(5, 0, "箇璸秨布(yy.mm.dd) : ", genbuf, 9, ECHONOSP, genbuf))
		vinfo->end_time = get_time(genbuf);

	if (vinfo->tickets == 0)
		vinfo->tickets = 1;
	sprintf(genbuf, "%d", vinfo->tickets);
	if (getdata(6, 0, "–布计 : ", genbuf, 3, ECHONOSP, genbuf))
		vinfo->tickets = atoi(genbuf);
	if (vinfo->tickets <= 0 || vinfo->tickets > sizeof(Box.vbits) * 8)
		return -1;

	if (getdata(7, 0, "ㄓ方(ㄒ140.117) : ", genbuf, sizeof(vinfo->allow_ip), ECHONOSP,
		    vinfo->allow_ip))
	{
		strcpy(vinfo->allow_ip, genbuf);
	}

	sprintf(genbuf, "%d", vinfo->userlevel);
	if (getdata(8, 0, "ㄏノ单(ㄒ50) : ", genbuf, 4, ECHONOSP, genbuf))
		vinfo->userlevel = atoi(genbuf);

	strcpy(genbuf, "n");
	getdata(9, 0, "ō粄靡щ(y/n): ", genbuf, 2, ECHONOSP, genbuf);
	if (genbuf[0] == 'y')
		vinfo->ident = 7;
	else
		vinfo->ident = 0;

	if (vinfo->firstlogin != 0)
		set_timestr(genbuf, &(vinfo->firstlogin));
	else
		genbuf[0] = '\0';
	if (getdata(10, 0, "爹ら戳(yy.mm.dd)", genbuf, 9, ECHONOSP, genbuf))
		vinfo->firstlogin = get_time(genbuf);

	sprintf(genbuf, "%d", vinfo->numlogins);
	if (getdata(11, 0, "Ω计 : ", genbuf, 7, ECHONOSP, genbuf))
		vinfo->numlogins = atoi(genbuf);

	sprintf(genbuf, "%d", vinfo->numposts);
	if (getdata(12, 0, "眎禟Ω计 : ", genbuf, 7, ECHONOSP, genbuf))
		vinfo->numposts = atoi(genbuf);

	setvotefile2(filename, direct, vinfo->filename, "/desc");
	vedit(filename, NULL, NULL);
}


static int
vcmd_edit(ent, vinfo, direct)
int ent;
VOTEHEADER *vinfo;
char *direct;
{
	if (!hasBMPerm)
		return C_NONE;

	if (set_vote(direct, vinfo) == -1)
		return C_FULL;

	if (substitute_record(direct, vinfo, sizeof(*vinfo), ent) == 0)
		return C_INIT;
/*		
	return C_FULL;
*/
	return C_LOAD;	/* becuase ReplyLastCall had destroyed hdrs */	
}


static int
vcmd_add(ent, vinfo, direct)
int ent;
VOTEHEADER *vinfo;
char *direct;
{
	VOTEHEADER vh_new;
	char path[PATHLEN], *p;

	if (!hasBMPerm)
		return C_NONE;

	getdata(b_line, 0, "絋﹚盾(y/n)? [n]: ", genbuf, 2, ECHONOSP | LOWCASE,
		NULL);
	if (genbuf[0] != 'y')
		return C_FOOT;

	memset(&vh_new, 0, sizeof(vh_new));

	strcpy(vh_new.owner, curuser.userid);

	/* ミ匡羭ヘ魁 */
	strcpy(path, direct);
	p = strrchr(path, '/') + 1;
	do
	{
		sprintf(p, "V.%d.A", time(0));
	}
	while (mkdir(path, 0700) != 0);
	strcpy(vh_new.filename, p);

	if (set_vote(direct, &vh_new) == -1)
	{
		rmdir(path);
		return C_FULL;
	}

	append_record(direct, &vh_new, sizeof(vh_new));

	set_brdt_vote_mtime(CurBList->filename);
	curbe->voting = TRUE;

	/* 笆秈赣匡羭ぇ匡虫: ノ猭Τ岿? */
	vcmd_enter(ent + 1, &vh_new, direct);
	return C_INIT;
}


static int
vcmd_delete(ent, vinfo, direct)
int ent;
VOTEHEADER *vinfo;
char *direct;
{
	if (!hasBMPerm)
		return C_NONE;

	msg(_msg_not_sure);
	if (igetkey() == 'y')
	{
		/* 埃匡羭戈 */
		delete_record(direct, sizeof(*vinfo), ent);

		/* 埃匡羭ヘ魁 */
		setdotfile(genbuf, direct, vinfo->filename);
		myunlink(genbuf);
		return C_INIT;
	}
	return C_FULL;
}


static int
vcmd_result(ent, vinfo, direct)
int ent;
VOTEHEADER *vinfo;
char *direct;
{
	char fn_result[PATHLEN], fname[PATHLEN];
	FILE *fpr, *fpw;

	/* 狾敖核щ布挡狦 */
	if (!hasBMPerm && time(0) < vinfo->end_time)
	{
		showmsg("临⊿秨布ら翅!! 单!");
		return C_FOOT;
	}

	/* 参璸布絚 */
	setdotfile(fname, direct, vinfo->filename);
	count_box(fname, vinfo);

	/* ㄖ種ǎ匡羭挡狦 */
	setvotefile2(fn_result, direct, vinfo->filename, "/result");
	setvotefile2(fname, direct, vinfo->filename, "/mess");
	if ((fpr = fopen(fname, "r")) != NULL)
	{
		if ((fpw = fopen(fn_result, "a")) != NULL)
		{
			fprintf(fpw, "\n[7m 琌呼ね種ǎ                                                           [m\n\n");
			while (fgets(genbuf, sizeof(genbuf), fpr))
				fputs(genbuf, fpw);
			fclose(fpw);
		}
		fclose(fpr);
	}
	more(fn_result, TRUE);

	return C_FULL;
}


static int
vcmd_info(ent, vinfo, direct)
int ent;
VOTEHEADER *vinfo;
char *direct;
{
	show_info ^= 1;
	return C_FULL;
}


static int
vcmd_help(ent, vinfo, direct)
int ent;
VOTEHEADER *vinfo;
char *direct;
{
	more("doc/VOTE_HELP", TRUE);
	return C_FULL;
}


struct one_key vote_comms[] =
{
	'a', vcmd_add,
	'h', vcmd_help,
	'r', vcmd_enter,
	'd', vcmd_delete,
	'E', vcmd_edit,
	'o', vcmd_result,
	'i', vcmd_info,
	'c', vcmd_desc,
	0, NULL
};



static int
IsMultiLogin()
{
	if (multi > 1)
	{
		clear();
		outs(_msg_vote_1);
		pressreturn();
		return 1;
	}
	return 0;
}


int
v_board()
{
	char tmpdir[PATHLEN];
	int v_ccur = 0;

#ifdef GUEST
	if (!strcmp(curuser.userid, GUEST))
		return C_NONE;
#endif

	if (IsMultiLogin())
		return C_LOAD;

	setvotefile(tmpdir, CurBList->filename, NULL);
	/* ミ狾щ布ヘ魁 */
	mkdir(tmpdir, 0755);
	setvotefile(tmpdir, CurBList->filename, VOTE_REC);

	cursor_menu(4, 0, tmpdir, vote_comms, sizeof(VOTEHEADER), &v_ccur,
	  vote_title, NULL /* vote_btitle */ , vote_entry, read_get, read_max,
		    NULL, 1, TRUE, SCREEN_SIZE-4);
	return C_LOAD;
}
