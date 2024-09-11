#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
 

int 
main(int argc, char *argv[])
{
    int parentport[2];
    int childport[2];//父进程、子进程读写端口
    pipe(parentport[2]);
    pipe(childport[2]);//创建管道
    int pid = fork();
    char buffer[64];
    if(pid == 0)//pid在子节点处为0
    { 
        read(parentport[0], buffer, 4); //从管道读取ping
        printf("%d: received %s\n", getpid(), buffer);
        write(childport[1], "pong", strlen('pong'));//写入
    }
    else
    {
        write(parentport[1], "ping", strlen("ping"));
        read(childport[0], buffer, 4);
        printf("%d: received %s\n", getpid(), buffer);
    }
    close(parentport[0]);
    close(parentport[1]);
    close(childport[0]);
    close(childport[1]);
    exit(0);
}

