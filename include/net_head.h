#ifndef _NET_HEAD_H_
#define _NET_HEAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <dirent.h>
#include <time.h>

#include <signal.h>

#include <semaphore.h>

//#include <libgen.h>

#define NAME_LEN 16
#define FILENAME_LEN 100
#define SEND_MAXSZ 1024
#define READ_BUFSZ 1024
#define MAX_TCP_CLINUM 20
#define MAX_UDP_CLINUM 100

#define COM_BUFSZ 20
#define MSG_BUFSZ 512

#define SOCK_UDP_PORT 9999
#define SOCK_TCP_PORT 35714 
#define SRV_IPADDR "192.168.1.108"
#define MC_IPADDR "235.147.123.100"
#define DOWNPATH "./download"

//#define DOWNLOAD_LIST_MAX MAX_UDP_CLINUM 

#endif
