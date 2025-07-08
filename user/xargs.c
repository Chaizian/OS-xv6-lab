#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// 读取一行输入到 buf，直到 '\n' 或 EOF，返回读取的字节数
int readline(char *buf, int max) {
  int i = 0;
  char ch;
  while (i < max - 1) {
    int n = read(0, &ch, 1);
    if (n == 0) break;     // EOF
    if (ch == '\n') break; // 行结束
    buf[i++] = ch;
  }
  buf[i] = '\0'; // C字符串结尾
  return i;
}

int main(int argc, char *argv[]) {
  char *exec_argv[MAXARG];   // 存储最终执行命令的参数
  int i;

  // 先将 xargs 后面命令部分（比如 "echo bye"）复制到 exec_argv 里
  for (i = 1; i < argc; i++) {
    exec_argv[i - 1] = argv[i];
  }

  char buf[512];
  while (readline(buf, sizeof(buf))) {
    int pid;
    char *p = buf;
    int j = i - 1; // 从 exec_argv 的末尾开始添加参数

    // 按空格切割用户输入的这一行
    while (*p) {
      // 跳过前导空格
      while (*p == ' ') p++;
      if (*p == '\0') break;

      exec_argv[j++] = p; // 添加一个新参数
      // 找到当前参数末尾，并加 '\0'
      while (*p != ' ' && *p != '\0') p++;
      if (*p == ' ') {
        *p = '\0';
        p++;
      }
    }

    exec_argv[j] = 0; // 以 NULL 结尾

    // 创建子进程执行命令
    pid = fork();
    if (pid == 0) {
      exec(exec_argv[0], exec_argv);
      fprintf(2, "exec failed\n");
      exit(1);
    } else {
      wait(0);
    }
  }

  exit(0);
}
