//该文件内的函数主要负责客户端从键盘输入的命令的解析和操作

#ifndef _CLI_MSG_COM_C_

#define _CLI_MSG_COM_C_

#endif

#include "./msg_com.h"

//申明外部变量 这些是主程序内的全局变量,需要在本文件内使用
extern int udp_cli_fd,tcp_cli_fd;
extern struct sockaddr_in udp_selfaddr;
extern struct sockaddr_in udp_srvaddr;

//向服务器发送登录消息
void snd_login(const char *user_name)
{
	struct msg_node msg_login;
	int num = -1;

	bzero(&msg_login, sizeof(msg_login));

	msg_login.mtype = IMSG_LOGIN;			//设定消息类型为登录消息
	strcpy(msg_login.mname, user_name);		//设定登录的用户名
	msg_login.mname[NAME_LEN - 1] = '\0';
	//通过UDP发送消息到服务器
	num = sendto(udp_cli_fd, &msg_login, sizeof(msg_login) - MSG_BUFSZ, 0, 
					(const struct sockaddr *)&udp_srvaddr, sizeof(udp_srvaddr));
	if(num < 0) {
		perror("snd_login sendto");
		exit(1);
	}
}

//向服务器发送退出的消息
void snd_logout(const char *user_name)
{
	struct msg_node msg_logout;
	int num = -1;

	bzero(&msg_logout, sizeof(msg_logout));

	msg_logout.mtype = IMSG_LOGOUT;			//设定消息类型为 退出消息
	strcpy(msg_logout.mname, user_name);	//设定用户名
	msg_logout.mname[NAME_LEN - 1] = '\0';
	//用UDP发送该消息到服务器
	num = sendto(udp_cli_fd, &msg_logout, sizeof(msg_logout) - MSG_BUFSZ, 0, 
					(const struct sockaddr *)&udp_srvaddr, sizeof(udp_srvaddr));
	if(num < 0) {
		perror("snd_logout snedto");
		exit(1);
	}
}

//向服务器发送私聊消息
//mname为发送者用户名,hname为接受着用户名,msg_text为聊天信息的内容
void snd_private(const char *mname,const char *hname,const char *msg_text)
{
	struct msg_node msg_private;
	int num = -1;

	bzero(&msg_private, sizeof(msg_private));

	msg_private.mtype = IMSG_PRIVATE;		//设定 消息类型
	strcpy(msg_private.mname, mname);		//设定 发送方
	msg_private.mname[NAME_LEN - 1] = '\0';

	strcpy(msg_private.hname, hname);		//设定 接受方
	msg_private.hname[NAME_LEN - 1] = '\0';

	strcpy(msg_private.text, msg_text);		//设定 聊天内容
	msg_private.text[MSG_BUFSZ - 1] = '\0';
	//以UDP发送消息到服务器
	num = sendto(udp_cli_fd, &msg_private, sizeof(msg_private) - MSG_BUFSZ + strlen(msg_text) + 1, 
					0, (const struct sockaddr *)&udp_srvaddr, sizeof(udp_srvaddr));
	if(num < 0) {
		perror("snd_private sendto");
		exit(1);
	}

}

//向服务器发送群聊消息
//mname为发送者用户名,msg_text为群聊消息的内容
void snd_group(const char *mname,const char *msg_text)
{
	struct msg_node msg_group;
	int num = -1;

	bzero(&msg_group, sizeof(msg_group));

	msg_group.mtype = IMSG_GROUP;			//设定 消息类型
	strcpy(msg_group.mname, mname);			//设定 发送方
	msg_group.mname[NAME_LEN - 1] = '\0';

	strcpy(msg_group.text, msg_text);		//设定 聊天内容
	msg_group.text[MSG_BUFSZ - 1] = '\0';
	//以UDP发送消息到服务器
	num = sendto(udp_cli_fd, &msg_group, sizeof(msg_group) - MSG_BUFSZ + strlen(msg_text) + 1, 0, 
					(const struct sockaddr *)&udp_srvaddr, sizeof(udp_srvaddr));
	if(num < 0) {
		perror("snd_group sendto");
		exit(1);
	}

}

//向服务器发送获取在线用户列表的消息
void snd_get_user_list(const char *user_name)
{
	struct msg_node msg_list;
	int num = -1;

	bzero(&msg_list, sizeof(msg_list));

	msg_list.mtype = IMSG_USER_LIST;		//设定 消息类型
	strcpy(msg_list.mname, user_name);		//设定 发送方
	msg_list.mname[NAME_LEN - 1] = '\0';
	//以UDP发送消息到服务器
	num = sendto(udp_cli_fd, &msg_list, sizeof(msg_list) - MSG_BUFSZ, 0, 
					(const struct sockaddr *)&udp_srvaddr, sizeof(udp_srvaddr));
	if(num < 0) {
		perror("snd_get_user_list sendto");
		exit(1);
	}

}

//向服务器发送获取下载文件列表的消息
void snd_get_file_list(const char *user_name)
{
	struct msg_node msg_list;
	int num = -1;

	bzero(&msg_list, sizeof(msg_list));

	msg_list.mtype = IMSG_DOWNLOAD_LIST;	//设定 消息类型
	strcpy(msg_list.mname, user_name);		//设定 发送方
	msg_list.mname[NAME_LEN - 1] = '\0';
	//以UDP发送消到服务器
	num = sendto(udp_cli_fd, &msg_list, sizeof(msg_list) - MSG_BUFSZ, 0, 
					(const struct sockaddr *)&udp_srvaddr, sizeof(udp_srvaddr));
	if(num < 0) {
		perror("snd_get_file_list sendto");
		exit(1);
	}


}

//解析命令
//pstr_in 储存的是从键盘输入的字符串
int analy_cmd(const char *pstr_in)
{
	if(0 == strcmp(pstr_in, "--help")) return IMSG_HELP;
	if(0 == strcmp(pstr_in, "--logout")) return IMSG_LOGOUT;
	if(0 == strcmp(pstr_in, "--talk")) return IMSG_PRIVATE;
	if(0 == strcmp(pstr_in, "--toall")) return IMSG_GROUP;
	if(0 == strcmp(pstr_in, "--upload")) return IMSG_UPLOAD;
	if(0 == strcmp(pstr_in, "--download")) return IMSG_DOWNLOAD;
	if(0 == strcmp(pstr_in, "--list")) return IMSG_USER_LIST;
	if(0 == strcmp(pstr_in, "--cls")) return IMSG_CLS;

	return -1;
}


