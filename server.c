#ifndef _SRV_MSG_COM_C_
#define _SRV_MSG_COM_C_
#include "./include/msg_com.h"
#endif 

#ifndef _LIST_COM_C_
#define _LIST_COM_C_
#include "./include/list_com.h"
#endif

void catch_signal(void);
void exec_func(int);

void *recvfrom_cli(void *arg);
void *process_msg(void *arg);

static int udp_serv_fd = -1;
static struct sockaddr_in udp_servaddr;
pthread_t p_tid[3];
static struct msg_info *msg_head = NULL;
static struct user_info *user_head = NULL;

static sem_t msg_sem;

int main(int argc, char const *argv[])
{
	int ret_val = -1;

	catch_signal();

	udp_serv_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_serv_fd < 0)
	{
		perror("socket error");
		exit(1);
	}

	int on = 1;
	ret_val = setsockopt(udp_serv_fd, SOL_SOCKET, 
						SO_REUSEADDR, &on, sizeof(on));
	if (ret_val < 0)
	{
		perror("setsockopt error");
		exit(1);
	}

	bzero(&udp_servaddr, sizeof(udp_servaddr));
	udp_servaddr.sin_family = AF_INET;
	udp_servaddr.sin_port = htons(SOCK_UDP_PORT);
	udp_servaddr.sin_addr.s_addr = inet_addr(SRV_IPADDR);

	ret_val = bind(udp_serv_fd, (const struct sockaddr *) &udp_servaddr, 
					sizeof(udp_servaddr));
	if (ret_val < 0)
	{
		perror("bind error");
		exit(1);
	}

	init_list(&user_head);
	init_msg_list(&msg_head);
	printf("\033[H\033[2J");	//清屏  很神奇的方式
	puts("服务器开启完毕!");		//
	printf("IP地址:%s,端口号:%d\n", SRV_IPADDR, SOCK_UDP_PORT);	//

	sem_init(&msg_sem, 0, 1);

	ret_val = pthread_create(&p_tid[0], NULL, recvfrom_cli, NULL);
	if (ret_val < 0)
	{
		perror("pthread_create error");
		exit(1);
	}

	ret_val = pthread_create(&p_tid[1], NULL, process_msg, NULL);
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

void catch_signal(void)
{
	signal(SIGINT, exec_func);
	signal(SIGSTOP, exec_func);
	signal(SIGQUIT, exec_func);
}

void exec_func(int signo)
{
	pthread_cancel(p_tid[0]);
	pthread_cancel(p_tid[1]);

	sem_close(&msg_sem);
	close(udp_serv_fd);
	destroy_list(user_head);
	destroy_msg_list(msg_head);
	puts("\b\bStop server ");
	usleep(500000);
	exit(0);
}

void *recvfrom_cli(void *arg)
{
	struct msg_node cli_msgbuf;
	struct sockaddr_in peer;
	int ret_val = -1;
	socklen_t peer_len = sizeof(peer);

	while(1){
		bzero(&peer, sizeof(peer));
		bzero(&cli_msgbuf, sizeof(cli_msgbuf));
		ret_val = recvfrom(udp_serv_fd, (void *) &cli_msgbuf, sizeof(cli_msgbuf), 0, (struct sockaddr *) &peer, &peer_len);
		if (ret_val < 0)
		{
			perror("recvfrom error");
		}

		memcpy(&(cli_msgbuf.cli_addr), &peer, sizeof(peer));
		sem_wait(&msg_sem);
		add_msg_list(msg_head, &cli_msgbuf);
		sem_post(&msg_sem);
	}
}

void *process_msg(void *arg)
{
	struct msg_info msgbuf;
	struct user_info data_node;
	struct user_info *search_node = NULL;

	int ret_val = -1;
	int num = -1;
	char list_str[MAX_UDP_CLINUM][70];

	int max_num = 0, i = 0;

	while(1){
		ret_val = get_msg_node(msg_head, &msgbuf);
		if (ret_val)
		{
			sem_wait(&msg_sem);
			del_msg_node(msg_head);
			sem_post(&msg_sem);
			switch(msgbuf.msg_node.mtype){
				case IMSG_LOGIN:
					search_node = NULL;
					search_node = search_list(user_head, msgbuf.msg_node.mname);
					if (search_node != NULL)
					{
						msgbuf.msg_node.mtype = IMSG_ONLINE;
						num = sendto(udp_serv_fd, (void *) &msgbuf.msg_node, \
									sizeof(msgbuf.msg_node), 0, \
									(const struct sockaddr *) &msgbuf.msg_node.cli_addr, \
									sizeof(struct sockaddr_in));
						printf("[%s] Already login \n", msgbuf.msg_node.mname);
						if (num < 0) perror("login sendto error");
						break;
					}
					bzero(&data_node, sizeof(data_node));
					strcpy(data_node.user_name, msgbuf.msg_node.mname);
					memcpy(&(data_node.cli_addr), &(msgbuf.msg_node.cli_addr), sizeof(data_node.cli_addr));
					add_list(user_head, &data_node);
					printf("[%s] login \n", data_node.user_name);
					break;
				
				case IMSG_LOGOUT:
					del_node(user_head, msgbuf.msg_node.mname);
					printf("[%s] logout \n", msgbuf.msg_node.mname);
					break;

				case IMSG_PRIVATE:
					printf("[%s] sendto [%s]\n", msgbuf.msg_node.mname, msgbuf.msg_node.hname);
					search_node = NULL;
					search_node = search_list(user_head, msgbuf.msg_node.hname);
					if (NULL == search_node)
					{
						 printf("====[%s] is not online! \n", msgbuf.msg_node.hname);
						 msgbuf.msg_node.mtype = IMSG_OFFLINE;
						 num = sendto(udp_serv_fd, (void *) &msgbuf.msg_node, \
						 				sizeof(msgbuf.msg_node), 0, \
						 				(const struct sockaddr *) &msgbuf.msg_node.cli_addr, \
						 				sizeof(struct sockaddr_in));
					} else { 
						printf("\n");
						num = sendto(udp_serv_fd, (void *) &msgbuf.msg_node, \
										sizeof(msgbuf.msg_node), 0, \
										(const struct sockaddr *) &(search_node->cli_addr), \
										sizeof(struct sockaddr_in));
					}
					if (num < 0)
					{
						perror("private sendto error");
					}
					break;

				case IMSG_USER_LIST:
					printf("[%s] Seek user info \n", msgbuf.msg_node.mname);
					get_all_user(user_head, list_str, &max_num);
					memset(msgbuf.msg_node.text, '\0', MSG_BUFSZ);
					for (i = 1; i <= max_num; ++i)
					{
						strncat(msgbuf.msg_node.text, list_str[i-1], strlen(list_str[i-1]));
						if (0 == i % 10 || i == max_num){
							if(max_num <= 10)
								msgbuf.msg_node.mtype = IMSG_USER_LIST_A;
							else if(i == 10)
								msgbuf.msg_node.mtype = IMSG_USER_LIST_F;
							else if(i != max_num)
								msgbuf.msg_node.mtype = IMSG_USER_LIST_N;
							else
								msgbuf.msg_node.mtype = IMSG_USER_LIST_D;

							num = sendto(udp_serv_fd, (void *) &(msgbuf.msg_node), \
											sizeof(msgbuf.msg_node), 0, \
											(const struct sockaddr *) &msgbuf.msg_node.cli_addr, \
											sizeof(struct sockaddr_in));
							if (num < 0)
							{
								perror("list sendto error");
								break;
							}
							memset(msgbuf.msg_node.text, '\0', MSG_BUFSZ);
						
						}
					}
					break;

				default:
					break; 		

			}
		}

	}
}