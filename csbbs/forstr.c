
#include "bbs.h"
#include "csbbs.h"

#define KEYWORD_DELIMITER	" \t\r\n"
#define KEYWORD_SEPARATE	"\t\r\n"

struct Paradata			
{
	char *parameter;
	struct Paradata *next;
};


struct Paradata *first_node = NULL;

char *
strupr(str)
char *str;
{
	int i;
	char c;

	for (i = 0; (c = str[i]) != '\0'; i++)
	{
		if (c >= 'a' && c <= 'z')
			str[i] = c - 'a' + 'A';
	}

	return str;
}

char *
GetToken(str, token, maxlen)
char *str;
char *token;
int maxlen;
{
	int i = 0, j;
	char *tmp;

	while (strchr(KEYWORD_DELIMITER, str[i]))	/* 去掉前面的空白 */
	{
		if (str[i] == '\0')
		{
			token[0] = '\0';
			return &str[i];
		}
		i++;
	}

	tmp = &str[i];
	j = 1;
	while (!strchr(KEYWORD_SEPARATE, tmp[j]))	/* 以tab為分隔，找出參數 */
		j++;

	if (j >= maxlen)
	{			/* token too large */
		token[0] = '\0';
		return &tmp[j];
	}

	strncpy(token, tmp, j);
	token[j] = '\0';	/* important */

	return &tmp[j];
}

/**********************************************************
*       將linklist中的資料全部清�
*
***********************************************************/
static void
free_node(node)
struct Paradata *node;
{
	struct Paradata *next_node;

	if (node == NULL)
		return;

	next_node = node->next;
	free(node);
	node = next_node;

	while (node != NULL)
	{
		next_node = node->next;
		free(node->parameter);
		free(node);
		node = next_node;
	}
}

/***********************************************************
*       將送進來的資料依照tab鍵將其區分開來
*       以linklist的方式存放
************************************************************/
void
SetParameter(data)
char *data;
{
	char temp[MAX_PARAMETER] = "", *data_ptr;
	int para_len;
	struct Paradata *last_node, *new_node;

	free_node(first_node);
	first_node = malloc(sizeof(struct Paradata));
	bzero(first_node, sizeof(struct Paradata));
	last_node = first_node;
	do
	{
		data = GetToken(data, temp, MAX_PARAMETER);

		para_len = strlen(temp);
		data_ptr = malloc(para_len * sizeof(char) + 1);
		bzero(data_ptr, para_len * sizeof(char) + 1);
		strcpy(data_ptr, temp);

		new_node = malloc(sizeof(struct Paradata));
		last_node->next = new_node;
		new_node->parameter = data_ptr;
		new_node->next = NULL;
		last_node = new_node;
	}
	while (*data != '\0');
}

/***************************************************
*       取得參數中的字串
*       num 為第num個參數
*       如果參數不合法，傳回NULL
*       傳回指向此字串的指標
****************************************************/
char *
Get_para_string(num)
int num;
{
	struct Paradata *node;
	int i;

	if (num < 1)
		return NULL;

	node = first_node;
	for (i = 0; i < num; i++)
	{
		if (node->next == NULL)
			return NULL;
		node = node->next;
	}
	return node->parameter;
}

/***************************************************
*       取得參數中的數字
*       num 為第num個參數
*       如果參數不合法，傳回0
*       傳回所求的參數數字
****************************************************/
int
Get_para_number(num)
int num;
{
	struct Paradata *node;
	int i;

	if (num < 1)
		return -1;

	node = first_node;
	for (i = 0; i < num; i++)
	{
		if (node->next == NULL)
			return -1;
		node = node->next;
	}

	return atoi(node->parameter);
}

/**************************************************
*       取得現在linklist中參數的個數
*
***************************************************/
int
Get_paras(void)
{
	int num = 0;
	struct Paradata *node;

	node = first_node;
	while (node->next != NULL)
	{
		num++;
		node = node->next;
	}
	return num;
}

StrDelR(str)
char *str;
{
	int i;

	if (str[0] != '\0')
	{
		i = strlen(str) - 1;
		while (i >= 0 && strchr(" \t\r\n", str[i]))
			i--;
		str[i + 1] = '\0';
	}
}

GetString(src, det, maxlen)
char *src;
char *det;
int maxlen;
{
	int i, j, len;

	maxlen--;
	det[0] = '\0';
	len = strlen(src);
	if (len == 0)
		return;
	i = 0;
	while (i < len && src[i] != '\"')
		i++;
	if (i == len)
		return;
	j = len - 1;
	while (j > 0 && src[j] != '\"')
		j--;
	len = j - i - 1;
	if (len > 0)
	{
		if (len > maxlen)
			len = maxlen;
		strncpy(det, &src[i + 1], len);
		det[len] = '\0';
	}
}


void
chk_str2(cs)
char *cs;
{
	if (cs)
	{
		int i, j;
		char *ct = cs;
		
		for (i = 0, j = 0; cs[i] != '\0'; i++)
		{
			if (cs[i] == 0x1b)	/* 0x1b is ESC */
				continue;
			ct[j++] = cs[i];
		}
		ct[j] = '\0';		
	}
}
