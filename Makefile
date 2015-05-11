CC = gcc
OBJS_DIR = obj

C_LIST = ./obj/list_com.o
C_MSG_LIST = ./obj/cli_msg_com.o
S_MSG_LIST = ./obj/srv_msg_com.o
S_OBJS = ./obj/server.o
C_OBJS = ./obj/client.o

#CFLAGS = -g -Wall -lpthread
CFLAGS = -g -Wall -lpthread -DNDEBUG

all:$(C_LIST) $(C_MSG) $(C_MSG_LIST) $(S_MSG_LIST) $(S_OBJS) $(C_OBJS)

$(C_LIST):./include/list_com.c
	$(CC) -c $^ -o $@ $(CFLAGS)

$(C_MSG_LIST):./include/cli_msg_com.c
	$(CC) -c $^ -o $@ $(CFLAGS)

$(S_MSG_LIST):./include/srv_msg_com.c
	$(CC) -c $^ -o $@ $(CFLAGS)

$(S_OBJS):server.c
	$(CC) -c $^ -o $@ $(CFLAGS)

$(C_OBJS):client.c
	$(CC) -c $^ -o $@ $(CFLAGS)

clean:
	$(RM) $(OBJS_DIR)/*.o
	rm -rf $(BIN_DIR)

dist:clean
	tar zcf ../LAN_com.bak.tar.gz ../LAN_com