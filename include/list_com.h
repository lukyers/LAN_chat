#ifndef _LIST_COM_H_
#define _LIST_COM_H_

#include "dlist.h"
#include "net_head.h"

//#define NAME_LEN 16

struct user_info {

	struct list_head user_list;  //内核链表
	
	char user_name[NAME_LEN];
//	unsigned long id;
	struct sockaddr_in cli_addr;
};

#ifdef _LIST_COM_C_

void init_list(struct user_info **list_head);
void add_list(struct user_info *list_head,struct user_info *data_node);
int del_node(struct user_info *list_head,const char *name);
struct user_info *search_list(struct user_info *list_head,const char *name);
void get_all_user(struct user_info *list_head, char (*list_str)[70], int *num);
void destroy_list(struct user_info *list_head);

#endif

#endif
