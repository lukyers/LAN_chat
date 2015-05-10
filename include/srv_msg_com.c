/*
   消息链表操作文件
   采用普通单链表实现
   包括 初始化、添加、删除、取结点、清空整个链表
 */
#ifndef _SRV_MSG_COM_C_

#define _SRV_MSG_COM_C_

#endif

#include "./msg_com.h"

//初始化链表头
//链表头不存储数据
void init_msg_list(struct msg_info **list_head)
{
	(*list_head) = (struct msg_info *)malloc(sizeof(struct msg_info));
	if(NULL == (*list_head)) {
		perror("malloc");
		exit(1);
	}
	(*list_head)->next = NULL;
}

//添加结点 采取尾部链接
void add_msg_list(struct msg_info *list_head, \
	   		  struct msg_node *data_node) 
{
	struct msg_info *new_node = (struct msg_info *)malloc(sizeof(struct msg_info));
	if(NULL == new_node) {
		perror("malloc");
		exit(1);
	}
	//初始化新结点
	new_node->next = NULL;
	memcpy(&(new_node->msg_node), data_node, sizeof(struct msg_node));

	struct msg_info *tmp = list_head;
	//链接到尾部
	while(tmp->next) {
		tmp = tmp->next;
	}

	tmp->next = new_node;
}

//删除结点,头部删除 每次删除第一个结点
//没有数据结点时返回0
int del_msg_node(struct msg_info *list_head) 
{
	struct msg_info *tmp = list_head->next;
	
	if(NULL == tmp) return 0;

	list_head->next = tmp->next;
	free(tmp);
	return 1;
}

//获取首个数据结点
//无数据结点时返回0
int get_msg_node(struct msg_info *list_head, struct msg_info *ret_node)
{
	struct msg_info *tmp = list_head->next;

	if(NULL == tmp) return 0;

	*ret_node = *tmp;
	return 1;
}

//清空链表 一般进程结束时调用
void destroy_msg_list(struct msg_info *list_head)
{
	struct msg_info *tmp = NULL;
	struct msg_info *tmp_n = list_head->next;
	while(tmp_n) {
		tmp = tmp_n->next;
		free(tmp_n);
		tmp_n = tmp;
	}

	free(list_head);
}

