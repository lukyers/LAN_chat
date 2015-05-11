#ifndef _CLI_MSG_COM_C_
#define _CLI_MSG_COM_C_
#include "./include/msg_com.h"
#endif

void catch_signal(void);
void exec_func(int);
void clear_func(void);

void *recvfrom_kb(void *arg);
void *recvfrom_ser(void *arg);

void show_help(void);

int udp_cli_fd = -1;
int tcp_cli_fd = -1;
static pthread_t p_tid[3];
struct sockaddr_in udp_selfaddr;
struct sockaddr_in udp_srvaddr;
char user_name[NAME_LEN] = {'\0'};
bool login_in = true;

int main(int argc, char const *argv[])
{
	if (argc != 2)	
	{
		fprintf(stderr, "usage: commad <user_name>\n");
		exit(0);
	}

	int ret_val = -1;

	catch_signal();
	udp_cli_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_cli_fd < 0)
	{
		perror("socket error");
		exit(1); 
	}

	int on  = 1;
	ret_val = setsockopt(udp_cli_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret_val < 0)
	{
		printf("setsockopt SO_REUSEADDR error\n");
		exit(1);
	}

	bzero(&udp_selfaddr, sizeof(udp_selfaddr));
	bzero(&udp_srvaddr, sizeof(udp_srvaddr));

	udp_selfaddr.sin_family = AF_INET;
	udp_selfaddr.sin_port = htons(SOCK_UDP_PORT);
	udp_selfaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	udp_srvaddr.sin_family = AF_INET;
	udp_srvaddr.sin_port = htons(SOCK_UDP_PORT);
	udp_srvaddr.sin_addr.s_addr = inet_addr(SRV_IPADDR);

#ifdef NDEBUG
	ret_val = bind(udp_cli_fd, (const struct sockaddr *) &udp_selfaddr, sizeof(udp_selfaddr));
	if (ret_val < 0)
	{
		printf("bind error\n");
		exit(1);
	}

	struct ip_mreq group_mr;
	group_mr.imr_multiaddr.s_addr = inet_addr(MC_IPADDR);
	group_mr.imr_interface.s_addr = htonl(INADDR_ANY);
	ret_val = setsockopt(udp_cli_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group_mr, sizeof(group_mr));
	if (ret_val < 0)
	{
		perror("setsockopt IP_ADD_MEMBERSHIP error");
		exit(1);
	}
#endif
	strcpy(user_name, argv[1]);
	snd_login(user_name);
	bzero(p_tid,sizeof(p_tid));

	ret_val = pthread_create(&p_tid[1], NULL, recvfrom_ser, NULL);
	if (ret_val < 0)
	{
		perror("pthread_create error");
		exit(1);
	}

	pthread_detach(p_tid[1]);

	printf("\033[H\033[2J");

	puts("Loging......");

	int time_num = 0;
	do
	{
		usleep(100000);
		time_num++;
	} while (time_num < 50 && login_in);

	if (login_in)
	{
		puts("Login time out");
		clear_func();
	}

	ret_val = pthread_create(&p_tid[0], NULL, recvfrom_kb, NULL);
	if (ret_val < 0)
	{
		perror("pthread_create error");
		exit(1);
	}

	pthread_detach(p_tid[0]);
	pthread_detach(p_tid[1]);

	while(1)
		sleep(10);
}

void *recvfrom_kb(void *arg)
{
	char put_in[MSG_BUFSZ];
	char strbuf[MSG_BUFSZ];
	bool change_p = true;
	bool group_ing = false;
	char old_talk[NAME_LEN];

	show_help();
	while(1){
		bzero(put_in, sizeof(put_in));
		bzero(strbuf, sizeof(strbuf));

		fgets(put_in, sizeof(put_in), stdin);
		put_in[sizeof(put_in) - 1] = '\0';
		put_in[strlen(put_in) - 1] = '\0';

BEGIN_INPUT:

		switch(analy_cmd(put_in)){
			case IMSG_HELP:
				show_help();
				break;

			case IMSG_LOGOUT:
				snd_logout(user_name);
				clear_func();
				break;

			case IMSG_PRIVATE:
				change_p = true;
				group_ing = false;

INPUT_MSG:
				puts("Please input user_name you want to talk:");
				bzero(put_in, sizeof(put_in));
				fgets(put_in, sizeof(put_in), stdin);
				put_in[sizeof(put_in) - 1] = '\0';
				put_in[strlen(put_in) - 1] = '\0';
				if (analy_cmd(put_in) != -1)
				{
					goto BEGIN_INPUT;
				}
				if (0 == strcmp(user_name, put_in))
				{
					puts("Tip: Don't send msg to yourself!");
					goto INPUT_MSG;
					break;
				}
				change_p = false;
				memset(old_talk, '\0', NAME_LEN);
				strncpy(old_talk, put_in, NAME_LEN);
				old_talk[NAME_LEN - 1] = '\0';

				puts("MSG: ");
				fgets(strbuf, sizeof(strbuf), stdin);
				strbuf[sizeof(strbuf) - 1] = '\0';
				strbuf[strlen(strbuf) - 1] = '\0';
				snd_private(user_name, old_talk, strbuf);
				break;

			case IMSG_GROUP:
				group_ing = true;
				change_p = true;
				puts("group MSG: ");
				fgets(strbuf, sizeof(strbuf), stdin);
				strbuf[sizeof(strbuf) - 1] = '\0';
				strbuf[strlen(strbuf) - 1] = '\0';

				snd_group(user_name, strbuf);
				break;

			default:
				if (!change_p)
				{
					printf("---send msg to [%s]\n", old_talk);
					snd_private(user_name, old_talk, put_in);
				}else if(group_ing){
					puts("---send msg to group ");
					snd_group(user_name, put_in);
				}else {
					puts("Please input correct order!");
					show_help();
				}
				break;
		}
	}
}

void *recvfrom_ser(void *arg)
{
	struct msg_node msgbuf;

	while(1){
		bzero(&msgbuf, sizeof(msgbuf));
		recvfrom(udp_cli_fd, (void *) &msgbuf, sizeof(msgbuf), 0, NULL, NULL);

		switch(msgbuf.mtype){
			case IMSG_LOGIN:
				if (0 == strcmp(msgbuf.mname, user_name))
				{
					login_in = false;
					puts("Login!");
				}else {
					printf("Sys: [%s] online\n", msgbuf.mname);
				}
				break;

			case IMSG_LOGOUT:
				if (strcmp(msgbuf.mname, user_name))
				{
					printf("系统提示:[%s]下线了!\n", msgbuf.mname);
				}
				break;

			case IMSG_PRIVATE:							//如果是私聊信息
				if(strcmp(msgbuf.mname, user_name))		//屏蔽掉给自己发送的消息(从键盘接受用户名时已经做了判断,这行可省略)
					printf("[%s]私人信息:\n%s\n", msgbuf.mname, msgbuf.text);
				break;

			case IMSG_GROUP:							//如果是群消息
				if(strcmp(msgbuf.mname, user_name))		//防止把信息发给自己
					printf("[%s]群消息:\n%s\n", msgbuf.mname, msgbuf.text);
				break;

			case IMSG_OFFLINE:		//如果是 不限在的信号 说明输入的私聊对象不在线  简单提示下
				printf("系统提示:[%s]不在线!\n", msgbuf.hname);
				break;

				//以下为显示所有在线用户信息过程,处理了多种可能出现的情况
			case IMSG_USER_LIST_A:		//一次性发完的
				puts("\n  昵称                   IP地址和端口信息");
				puts("================================================");
				printf("%s", msgbuf.text);
				puts("================================================\n");
				break;

			case IMSG_USER_LIST_F:		//首次发送且还有后序内容,此时要注意打印方式
				puts("\n  昵称                   IP地址和端口信息");
				puts("================================================");
				printf("%s", msgbuf.text);
				break;

			case IMSG_USER_LIST_N:		//中间部分的发送,还有后序内容
				printf("%s", msgbuf.text);
				break;

			case IMSG_USER_LIST_D:		//最后一次发送
				printf("%s", msgbuf.text);
				puts("================================================\n");
				break;
				//以上为显示所有在线用户信息过程,处理多种可能的各种情况				
			
			case IMSG_ONLINE:		//如果是重复登录的消息
				puts("系统提示:你已经在线了!");
				clear_func();		//清空关闭必要数据并结束进程
				break;

			case IMSG_SRVOFF:		//服务器关闭是,通知各个客户端的消息
				puts("系统提示:服务器关闭!");
				clear_func();		//服务器都关闭了,客户端紧跟着自动结束
				break;

			case IMSG_TEST:
				printf("[%s]\n", msgbuf.text);
				break;

			default :
				break;

		}
	}
}

void show_help(void)
{
	printf("\n======================命令======================\n");
	printf("==   --help    	帮助   ==   --logout 退出     ==\n");
	printf("==   --talk     私聊   ==   --toall  群聊     ==\n");
	printf("==   --list     查询   ==   --cls    清屏     ==\n");
	printf("================================================\n");
}

void catch_signal(void)
{
	signal(SIGINT, exec_func);
	signal(SIGSTOP, exec_func);
	signal(SIGQUIT, exec_func);
}

void exec_func(int signo)
{
	snd_logout(user_name);

	printf("\b\b");				//打印两个退格 例如按 Ctrl+C 会出现 ^C 打印两个退格可以把这个删掉
	pthread_cancel(p_tid[0]);	//结束各个线程
	pthread_cancel(p_tid[1]);
	clear_func();

}

void clear_func(void)
{
	close(udp_cli_fd);	//关UDP套接字
	puts("结束进程!");
	usleep(500000); 
	exit(0);
}