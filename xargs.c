#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUF_SIZE 2048

int main(int argc, char *argv[]) 
{
    char *cmd_args[MAXARG]; // 子进程参数数组
    for (int i = 1; i < argc; i++)
        cmd_args[i - 1] = argv[i]; // 去掉xargs并存入缓冲区

    char buffer[BUF_SIZE]; // 缓冲区存储标准输入数据
    char c;
    int read_status = 1;

    // 循环读取标准输入
    while (read_status) 
    { 
        int buf_idx = 0; // 缓冲区指针
        int arg_start = 0; // 当前参数的起始位置
        int cmd_idx = argc - 1; // 初始命令参数数量

        // 逐个字符读取一行
        while (1) 
        { 
            read_status = read(0, &c, 1);
            if (read_status == 0) exit(0);
            
            if (c == ' ' || c == '\n') 
            { 
                buffer[buf_idx++] = 0; // 参数末尾标记
                cmd_args[cmd_idx++] = &buffer[arg_start];
                arg_start = buf_idx; // 更新下一个参数起点
                if (c == '\n') break; // 处理完一行
            } 
            else 
            {
                buffer[buf_idx++] = c; // 拼接字符
            }
        }

        cmd_args[cmd_idx] = 0; // 参数数组结束标记

        if (fork() == 0) 
        {
            exec(cmd_args[0], cmd_args); // 执行命令
        } 
        else 
        {
            wait(0); // 等待子进程结束
        }
    }
    exit(0);
}
