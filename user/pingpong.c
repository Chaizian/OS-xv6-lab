#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc, char *argv[])
{
    int ptc[2];//父进程到子进程的管道文件描述符数组
    int ctp[2];
    char temp[8];//存数据
    //创建管道
    pipe(ptc);
    pipe(ctp);

    if(fork()!=0){
        //父进程
        write(ptc[1],"ping",4);//发送数据
        wait(0);//等待子进程结束
        read(ctp[0],temp,4);//读数据
        printf("%d: received %s\n",getpid(),temp);
    }
    else{
        //子进程
        read(ptc[0],temp,4);
        printf("%d: received %s\n",getpid(),temp);
        write(ctp[1],"pong",4);
    }

    exit(0);
}