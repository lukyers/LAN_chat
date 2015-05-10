/*
   该文件主要完成链表的相关操作
   初始化、添加、查找、删除、清空 等
PS:采用内核链表实现的各类操作
 */
#ifndef _LIST_COM_C_

#define _LIST_COM_C_

#endif

#include "list_com.h"

//初始化
void init_list(struct user_info **list_head)
{
	(*list_head) = (struct user_info *)malloc(sizeof(struct user_info));
	if(NULL == (*list_head)) {
		perror("malloc");
		exit(1);
	}
	INIT_LIST_HEAD(&(*list_head)->user_list);
}

//添加结点 前端添加 新结点
void add_list(struct user_info *list_head, \
	   		  struct user_info *data_node) 
{
	struct user_info *tmp = (struct user_info *)malloc(sizeof(struct user_info));
	if(NULL == tmp) {
		perror("add_list malloc");
		exit(1);
	}

	memcpy(tmp, data_node, sizeof(struct user_info));

	list_add(&(tmp->user_list), &(list_head->user_list));
}

//删除结点  根据名字删除
//注意:需要删除内部 list_head 结点和外部结点
int del_node(struct user_info *list_head, \
			 const char *name) 
{
	struct user_info *tmp = NULL;
	struct list_head *pos = NULL, *q = NULL;

	list_for_each_safe(pos, q, &(list_head->user_list)) {

		tmp = list_entry(pos, struct user_info, user_list);
		if(name != NULL) {

			if(0 == strcmp(name, tmp->user_name)) {
				list_del(pos);
				free(tmp);
				return 1;
			}

		} else {
			list_del(pos);
			free(tmp);	
		}
	}

	return 0;

}

//查找  根据名字查找
struct user_info *search_list(struct user_info *list_head, \
							  const char *name)
{
	struct user_info *tmp = NULL;

	list_for_each_entry(tmp, &(list_head->user_list), user_list)
		if(0 == strcmp(name, tmp->user_name)) return tmp;
	return NULL;
}

//获取链表所有结点名称信息 储存到 二维数组 list_str 中
void get_all_user(struct user_info *list_head, char (*list_str)[70], int *num)
{
	struct user_info *tmp = NULL;
	*num = 0;

	list_for_each_entry(tmp, &(list_head->user_list), user_list) {
		memset(list_str[*num], '\0', sizeof(list_str[*num]));
		//链接 名称 IP:端口
		sprintf(list_str[*num], "# %-15s\t [%s:%d] #\n", tmp->user_name, inet_ntoa(tmp->cli_addr.sin_addr), ntohs(tmp->cli_addr.sin_port));
		(*num)++;
	}
}

//清空链表  一般进程结束时 调用
void destroy_list(struct user_info *list_head)
{
	del_node(list_head, NULL);
}
