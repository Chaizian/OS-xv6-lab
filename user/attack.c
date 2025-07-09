#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"


int
main(int argc, char *argv[])
{
  char *p = sbrk(PGSIZE * 17);
  if(p == (char *) -1){
    fprintf(2, "sbrk failed\n");
    exit(1);
  }
  p=p+16*PGSIZE;
  // 偏移32字节读取8字节秘密
  char *secret = p + 32;

  // 写到文件描述符2
  write(2, secret, 8);

  exit(0);
}



