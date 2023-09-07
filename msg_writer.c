#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

#define PATH_NAME "/home/rocky/code/msgQueue"
#define PRJ_ID   30

typedef struct {     
     long mtype;     //内核消息队列里的类型
     char data[128]; //用户发送的最大数据
}msgType;

int main() {
    //1.生成key
    key_t k1;
    int msg_id;
    int ret;
    char buf[128] = {0};
    msgType msg;
    k1 = ftok(PATH_NAME, PRJ_ID);
    if (k1 == -1) {
        perror("token");
        return -1;
    }

    //2.产生msgQueue的数据结构，最终在用户空间得到了一个id号
    msg_id = msgget(k1, IPC_CREAT | 0640);
    if (msg_id == -1) {
        perror("msg get");
        return -1;
    }

    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return -1;
    } else if(pid == 0) {
        //消息发送
        while(fgets(buf, sizeof(buf), stdin)) {
            buf[strlen(buf) - 1] = 0; 
            msg.mtype = 100;
            memset(msg.data, 0, sizeof(msg.data));
            memcpy(msg.data, buf, strlen(buf));
            ret = msgsnd(msg_id, &msg, strlen(msg.data), 0);
            if (ret == -1){
                perror("msg snd");
                return -1;
            }
            memset(buf, 0, sizeof(buf));
       }
    } else {
        //消息读取
        while(1) {
            memset(msg.data, 0, sizeof(msg.data));
            ret = msgrcv(msg_id, &msg, sizeof(msg.data), 200, 0);
            if (ret == -1){
                perror("msg rcv");
                return -1;
            }
            msg.data[ret] = 0;
            printf("read msg: %s\n", msg.data);
        }

    }
     return 0;
}
