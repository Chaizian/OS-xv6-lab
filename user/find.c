#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 拼接路径，将 base + "/" + name 拼成新路径 path
void join(char *path, const char *base, const char *name) {
  strcpy(path, base);  // 先复制 base 到 path
  int len = strlen(path);
  // 如果路径末尾不是 '/'，则添加 '/'
  if (len > 0 && path[len - 1] != '/')
    path[len++] = '/';
  // 追加文件名
  strcpy(path + len, name);
}

// 递归查找函数，path 是当前目录，target 是要查找的文件名
void find(const char *path, const char *target) {
  char buf[512], fullpath[512];         // buf 用于 stat 错误信息，fullpath 存完整路径
  int fd;
  struct dirent de;                     // dirent 表示目录项
  struct stat st;                       // stat 结构用于判断文件类型等信息

  fd = open(path, 0);                   // 以只读方式打开当前路径
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {             // 获取文件状态
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE:  // 如果是普通文件，判断文件名是否匹配目标名
      if (strcmp(path + strlen(path) - strlen(target), target) == 0)
        printf("%s\n", path);
      break;

    case T_DIR:  // 如果是目录
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("find: path too long\n");
        break;
      }

      // 遍历目录下的每一个目录项（文件或子目录）
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)  // 无效目录项
          continue;

        // 跳过当前目录 "." 和上级目录 ".."
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
          continue;

        // 拼接子路径：当前路径 + '/' + 当前目录项名称
        join(fullpath, path, de.name);

        // 获取子路径的文件状态
        if (stat(fullpath, &st) < 0) {
          printf("find: cannot stat %s\n", fullpath);
          continue;
        }

        // 如果是普通文件，判断名字是否匹配
        if (st.type == T_FILE) {
          if (strcmp(de.name, target) == 0)
            printf("%s\n", fullpath);
        }
        // 如果是目录，递归进入继续查找
        else if (st.type == T_DIR) {
          find(fullpath, target);
        }
      }
      break;
  }

  close(fd);  // 关闭当前目录文件描述符
}

// 主函数，命令行参数：find <路径> <目标文件名>
int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <path> <target>\n");
    exit(1);
  }

  find(argv[1], argv[2]);  // 调用递归查找函数
  exit(0);
}
