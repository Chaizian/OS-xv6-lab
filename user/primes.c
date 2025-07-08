#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
void primes(int) __attribute__((noreturn));
// 筛子进程函数：读取输入 pipe，过滤某个质数的倍数，并递归创建下一级进程
void primes(int p_in_read_fd) {
    int p;

    // 尝试读取第一个数（当前要筛的质数）
    if (read(p_in_read_fd, &p, sizeof(p)) != sizeof(p)) {
        // 无数据，说明筛选完毕
        close(p_in_read_fd);
        exit(0);
    }

    // 打印质数
    printf("prime %d\n", p);

    // 创建新的 pipe 给下一级
    int p_out[2];
    pipe(p_out);

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }
    else if (pid == 0) {
        // 子进程递归处理下一级筛子
        close(p_out[1]);      // 子进程不写
        close(p_in_read_fd);  // 不再用父 pipe
        primes(p_out[0]);
        // 不返回（递归退出）
    } 
    else {
        // 父进程：继续读、过滤、转发
        int num;
        while (read(p_in_read_fd, &num, sizeof(num)) == sizeof(num)) {
            if (num % p != 0) {
                write(p_out[1], &num, sizeof(num)); // 非 p 的倍数才传下去
            }
        }
        // 关闭所有用完的 pipe
        close(p_in_read_fd);
        close(p_out[1]);

        // 等待子进程退出
        wait(0);
        exit(0);
    }
}

int main() {
  int p[2];
  pipe(p);

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  } else if (pid == 0) {
    // 子进程执行 primes 递归函数
    close(p[1]);    // 不再写
    primes(p[0]);   // 从 p[0] 中读
  } else {
    // 父进程写入 2~280 到管道
    close(p[0]);  // 不读
    for (int i = 2; i <= 280; i++) {
      write(p[1], &i, sizeof(i));
    }
    close(p[1]);  // 写完关闭写端

    // 等待 primes 全部执行完
    wait(0);
  }

  exit(0);
}
