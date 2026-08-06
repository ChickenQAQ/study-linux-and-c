///////////////////////////////////////////////////////////
//
//  Copyright(C), 2013-2021, GEC Tech. Co., Ltd.
//
//  文件: lab/tree/headers/drawtree.h
//  日期: 2017-9
//  描述: 使用C语言写一页webpage展示二叉树
//  作者: vincent040
//
///////////////////////////////////////////////////////////

#ifndef _DRAWTREE_H_
#define _DRAWTREE_H_

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 公共头文件 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <strings.h>

#include <sys/stat.h>
#include <sys/types.h>

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ 公共头文件 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

#define MAX(a, b) ({ \
		typeof(a) _a = a; \
		typeof(b) _b = b; \
		(void)(&_a == &_b);\
		_a > _b? _a : _b; \
		})


/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 默认二叉树节点 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
typedef struct _tree_node
{
	int data;
	struct _tree_node *lchild;
	struct _tree_node *rchild;

#ifdef AVL
	int height;
#endif

#ifdef RB
	struct _tree_node *parent;
	int color;
#endif
}_treenode, *_linktree;
/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ 默认二叉树节点 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */


/* ↓↓↓↓↓ 用户指定二叉树节点 ↓↓↓↓↓ */
#ifndef TREENODE
#define TREENODE _treenode
#endif
/* ↑↑↑↑↑ 用户指定二叉树节点 ↑↑↑↑↑ */


/* ↓↓↓↓↓↓↓↓↓↓↓↓↓ 画网页相关算法代码 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

#ifndef QUEUE_TREENODE_DATATYPE
#define QUEUE_TREENODE_DATATYPE TREENODE *
#endif

typedef QUEUE_TREENODE_DATATYPE qn_datatype;

struct _queue_node
{
	qn_datatype data;
	struct _queue_node *next;

};

typedef struct
{
	struct _queue_node *front;
	struct _queue_node *rear;
#ifdef __queue_size
	int size;
#endif
}_queuenode, *_linkqueue;


static _linkqueue __init_queue(void)
{
    _linkqueue q = (_linkqueue)malloc(sizeof(_queuenode));
	q->front = q->rear =
		(struct _queue_node *)malloc(sizeof(struct _queue_node));
	q->rear->next = NULL;

	return q;
}

static bool __is_empty_q(_linkqueue q)
{
	return (q->front == q->rear);
}

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓ 画网页相关算法代码 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
static bool __out_queue(_linkqueue q, qn_datatype *pdata)
{
	if(__is_empty_q(q))
		return false;

	struct _queue_node *p = q->front;

	q->front = q->front->next;
	free(p);
	*pdata = q->front->data;

	return true;
}

static bool __en_queue(_linkqueue q, qn_datatype data)
{
	struct _queue_node *pnew;
	pnew = (struct _queue_node *)malloc(sizeof(struct _queue_node));
	if(pnew == NULL)
		return false;

	pnew->data = data;
	pnew->next = NULL;

	q->rear->next = pnew;
	q->rear = pnew;

	return true;
}

#ifdef __queue_size
int __queue_size(_linkqueue *q)
{
	return q->size;
}
#endif
/* ↓↓↓↓↓↓↓↓↓↓↓↓↓ 画网页相关算法代码 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */


static void __pre_travel(TREENODE *root, void (*handler)(TREENODE *))
{
	if(root == NULL)
		return;
	
	handler(root);
	__pre_travel(root->lchild, handler);
	__pre_travel(root->rchild, handler);
}

static void __mid_travel(TREENODE *root, void (*handler)(TREENODE *))
{
	if(root == NULL)
		return;
	
	__mid_travel(root->lchild, handler);
	handler(root);
	__mid_travel(root->rchild, handler);
}

static void __post_travel(TREENODE *root, void (*handler)(TREENODE *))
{
	if(root == NULL)
		return;

	__post_travel(root->lchild, handler);
	__post_travel(root->rchild, handler);
	handler(root);
}

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓ 画网页相关算法代码 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
static void __level_travel(TREENODE *root, void (*handler)(TREENODE *))
{
	if(root == NULL)
		return;

    _linkqueue q;
	q = __init_queue();

	__en_queue(q, root);

    TREENODE *tmp;
	while(1)
	{
		if(!__out_queue(q, &tmp))
			break;

		handler(tmp);

		if(tmp->lchild != NULL)
			__en_queue(q, tmp->lchild);
		if(tmp->rchild != NULL)
			__en_queue(q, tmp->rchild);
	}
}
/* ↓↓↓↓↓↓↓↓↓↓↓↓↓ 画网页相关算法代码 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
static char page_begin[] = "<html><head><title>tree map"
                           "</title></head><body>"
			   "<table border=0 cellspacing"
                           "=0 cellpadding=0>";
static char line_begin[] = "<tr>";
static char line_end  [] = "</tr>";
static char space     [] = "<td>&nbsp;</td>";
static char underline [] = "<td style=\"border-bottom:"
	 		   "1px solid #58CB64\">&nbsp;"
                           "</td>";

#ifdef RB
static char data_begin_red[] = "<td bgcolor=\"#FF0000\";style="
			       "\"border:1px sol"
                               "id #58CB64;background-colo"
                               "r:#DDF1D8;PADDING:2px;\" t"
                               "itle=\"level: 1\">";
static char data_begin_blk[] = "<td bgcolor=\"#000000\";style="
			       "\"border:1px sol"
                               "id #58CB64;background-colo"
                               "r:#DDF1D8;PADDING:2px;\" t"
                               "itle=\"level: 1\"><font color"
				"=\"#FFFFFF\">";
static char data_end_red[] = "</td>";
static char data_end_blk[] = "</font></td>";
#else
static char data_begin[] = "<td style=\"border:1px sol"
                           "id #58CB64;background-colo"
                           "r:#DDF1D8;PADDING:2px;\" t"
                           "itle=\"level: 1\">";
static char data_end  [] = "</td>";
#endif

static char page_end  [] = "</table></body></html>";

#define MAX_TREENODES_NUMBER 100
#define FILENAME 32

static int central_order[MAX_TREENODES_NUMBER];

static void putunderline(int fd, int num)
{
	int i;
	for(i=0; i<num; i++)
	{
		write(fd, underline, strlen(underline));
	}
}


static void putspace(int fd, int num)
{
	int i;
	for(i=0; i<num; i++)
	{
		write(fd, space, strlen(space));
	}
}
/* ↓↓↓↓↓↓↓↓↓↓↓↓↓ 画网页相关算法代码 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
#ifdef RB
static void putdata(int fd, TREENODE * p)
{
	char s[50];
	bzero(s, 50);

	snprintf(s, 50, "%d", p->data);

	switch(p->color)
	{
	case RED:
		write(fd, data_begin_red, strlen(data_begin_red));
		write(fd, s, strlen(s));
		write(fd, data_end_red, strlen(data_end_red));
		break;
	case BLACK:
		write(fd, data_begin_blk, strlen(data_begin_blk));
		write(fd, s, strlen(s));
		write(fd, data_end_blk, strlen(data_end_blk));
	}
}
#else
static void putdata(int fd, int data)
{
	char s[50];
	bzero(s, 50);

	snprintf(s, 50, "%d", data);
	write(fd, data_begin, strlen(data_begin));
	write(fd, s, strlen(s));
	write(fd, data_end, strlen(data_end));
}
#endif

static int Index = 0;
static void create_index(TREENODE * root)
{
	if(Index >= MAX_TREENODES_NUMBER-1)
		return;

	central_order[Index++] = root->data;
}


static int get_index(int data)
{
	int i;
	for(i=0; i<100; i++)
	{
		if(central_order[i] == data)
			return i;
	}
	return -1;
}

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓ 画网页相关算法代码 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
static void data_leftside(int fd, TREENODE * root, int spaces)
{
	if(root == NULL)
		return;

	int s_line=0;

	if(root->lchild != NULL)
	{
		s_line = get_index(root->data)-
			 get_index(root->lchild->data)-1;
	}
	putspace(fd, spaces-s_line);
	putunderline(fd, s_line);
}


static int data_rightside(int fd, TREENODE * root)
{
	if(root == NULL)
		return 0;

	int s_line=0;

	if(root->rchild != NULL)
	{
		s_line = get_index(root->rchild->data)-
			 get_index(root->data)-1;
	}

	putunderline(fd, s_line);
	return s_line;
}


static void start_page(int fd)
{
	write(fd, page_begin, strlen(page_begin));
}

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓ 画网页相关算法代码 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
static void end_page(int fd)
{
	write(fd, page_end, strlen(page_end));
}


static void draw(TREENODE * root)
{
	if(root == NULL)
		return;

	time_t t;
	time(&t);
	static char filename[FILENAME];
	bzero(filename, FILENAME);
	snprintf(filename, FILENAME, "%u.html", (unsigned)t);
	int fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0644);
	if(fd == -1)
	{
		perror("open() failed");
		exit(1);
	}

	Index = 0;
	__mid_travel(root, create_index);

    _linkqueue q = __init_queue();

	TREENODE * tmp = root;
	int ndata = 1;

	start_page(fd);
	while(1)
	{
		write(fd, line_begin, strlen(line_begin));

		int i, n = 0;
		int nextline = 0;
		for(i=0; i<ndata; i++)
		{
			int index = get_index(tmp->data);

			data_leftside(fd, tmp, index-n);

			#ifdef RB
			putdata(fd, tmp);
			#else
			putdata(fd, tmp->data);
			#endif
			int rightline = data_rightside(fd, tmp);

			if(tmp->lchild != NULL)
			{
				nextline++;
				__en_queue(q, tmp->lchild);
			}
			if(tmp->rchild != NULL)
			{
				nextline++;
				__en_queue(q, tmp->rchild);
			}
			if(!__out_queue(q, &tmp))
				return;

			n = index + rightline;
			n++;
		}
		write(fd, line_end, strlen(line_end));
		ndata = nextline;
	}
	end_page(fd);
	close(fd);
}
#endif
