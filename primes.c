#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int readpipe);

int main(int argc, char *argv[]) 
{
    int p[2];
    pipe(p);  // 创建初始管道

    if (fork() == 0) // 创建第一个子进程 
    { 
        close(p[1]); // 子进程关闭写端
        prime(p[0]); // 子进程调用 prime 函数
    } 
    else 
    {
        close(p[0]); // 父进程关闭读端
        for (int i = 2; i <= 35; i++) 
        {
            write(p[1], &i, sizeof(i)); // 父进程写入 2-35 之间的数字到管道
        }
        close(p[1]); // 写入完成后关闭写端
        wait(0); // 等待子进程结束
    }
    exit(0);
}

void prime(int readpipe) 
{
    int prime_num;
    // 读取第一个数字
    if (read(readpipe, &prime_num, sizeof(prime_num)) == 0) 
    {
        exit(0); // 如果没有读取到任何数据，退出
    }

    // 输出当前的质数
    printf("prime %d\n", prime_num);

    int num;
    int p[2];
    pipe(p); // 创建新管道

    // 筛选数字，传递给下一个进程
    while (read(readpipe, &num, sizeof(num)) > 0) 
    {
        if (num % prime_num != 0) { // 如果不能被当前质数整除
            write(p[1], &num, sizeof(num)); // 写入子进程的管道
        }
    }

    close(readpipe); // 关闭父进程的读管道
    close(p[1]);     // 关闭新管道的写端

    if (fork() == 0) // 创建新子进程
    { 
        close(p[1]); // 子进程关闭写端
        prime(p[0]); // 递归调用 prime 函数处理新数据
    } 
    else 
    {
        close(p[0]); // 父进程关闭读端
        wait(0); // 等待子进程结束
    }
    exit(0);
}
