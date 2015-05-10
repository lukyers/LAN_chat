#ifndef _MSG_COM_H_
#define _MSG_COM_H_

#include "net_head.h"

//#define NAME_LEN 16 
//#define COM_BUFSZ 20
//#define MSG_BUFSZ 512
//#define MAX_CLINUM 100

//#define SOCK_UDP_PORT 15976
//#define SOCK_TCP_PORT 35714 
//#define SRV_IPADDR "192.168.7.124"
//#define MC_IPADDR "235.147.123.100"
//#define DOWNPATH "./download"

struct msg_node {
	long mtype;
	char mname[NAME_LEN];
//	int m_id;
	char hname[NAME_LEN];
	struct sockaddr_in cli_addr;
//	int h_id;
	char text[MSG_BUFSZ];
};

struct msg_info {
	struct msg_node msg_node;
	struct msg_info* next;
};

enum IMSG_INFO {
	IMSG_MIN = 0,
	IMSG_LOGIN,				//--login		登录
	IMSG_LOGOUT,			//--logout		退出
	IMSG_USER_LIST,			//--list		查看在线用户
	IMSG_PRIVATE,			//--talk		私聊
	IMSG_GROUP,				//--toall		群聊
	IMSG_UPLOAD,			//--upload		上传文件
	IMSG_DOWNLOAD,			//--download	下载文件
	IMSG_DOWNLOAD_LIST,		//--download 	后获取文件列表

	//用户信息列表
	IMSG_USER_LIST_A,		//表示一次性发送完毕
	IMSG_USER_LIST_F,		//表示第一次发的不完全的内容
	IMSG_USER_LIST_N,		//表示后面还有内容
	IMSG_USER_LIST_D,		//表示已经全部发送完毕

	//服务器文件列表
	IMSG_DOWNLOAD_LIST_A,	//表示一次性发送完毕
	IMSG_DOWNLOAD_LIST_F,	//表示第一次发的不完全的内容
	IMSG_DOWNLOAD_LIST_N,	//表示后面还有内容
	IMSG_DOWNLOAD_LIST_D,	//表示已经全部发完
	IMSG_DOWNLOAD_LIST_NONE,//表示没有任何文件

	IMSG_OFFLINE,			//用户不在线
	IMSG_SRVOFF,			//服务器关闭
	IMSG_ONLINE,			//用户已经在线
	IMSG_CLS,
	IMSG_HELP,
	IMSG_ERR0,		
	IMSG_ERR1,
	//......
	IMSG_MAX
};

/*以下为客户端的函数申明*/
#ifdef _CLI_MSG_COM_C_

void snd_login(const char *user_name);
void snd_logout(const char *user_name);
void snd_private(const char *mname, const char *hname, const char *msg_text);
void snd_group(const char *mname, const char *msg_text);
void snd_get_user_list(const char *user_name);
void snd_get_file_list(const char *user_name);

int analy_cmd(const char *pstr_in);

#endif


/*以下为服务端的函数申明*/
#ifdef _SRV_MSG_COM_C_

void init_msg_list(struct msg_info **list_head);
void add_msg_list(struct msg_info *list_head, struct msg_node *data_node);
int del_msg_node(struct msg_info *list_head);
void destroy_msg_list(struct msg_info *list_head);
int get_msg_node(struct msg_info *list_head, struct msg_info *ret_node);

#endif

#endif


