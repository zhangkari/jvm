/*******************************
 * file name:   linklist.h
 * description: linklist API 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#ifndef __LINKLIST__H__
#define __LINKLIST__H__

typedef struct linklist_s linklist_s;
typedef struct linklist_s linklist_t;

struct linklist_s
{
    void        *elem;
    linklist_s  *next;
};

/**
 * init linklist
 */
linklist_s* init_linklist();

/**
 * add node in list
 */
int add_list_elem(linklist_s *list, void *elem);

/**
 * free linklist
 */
void free_linklist(linklist_s *list);

#endif
