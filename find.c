#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 获取路径中的文件名部分
char* fmtname(char *path) {
  static char buf[DIRSIZ+1];
  char *p;

  // 找到路径中的最后一个斜杠
  for(p = path + strlen(path); p >= path && *p != '/'; p--) 
    ;
  p++; 

  // 返回文件名或空格填充的名称
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p) + 1); // 复制文件名，包含结束符
  return buf;
}

// 递归查找并匹配文件名
void find(char *path, char *target) {
  char buf[512], *p; 
  int fd;            
  struct dirent de;  
  struct stat st;    

  // 打开路径
  if((fd = open(path, 0)) < 0) return;

  // 获取文件状态信息
  if(fstat(fd, &st) < 0) {
    close(fd);
    return;
  }

  switch(st.type) {
  case T_FILE:  
  case T_DEVICE:  
    if(strcmp(fmtname(path), target) == 0) // 名称匹配
      printf("%s\n", path);
    break;

  case T_DIR:  
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/'; // 添加目录分隔符

    // 遍历目录内容
    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
      if(de.inum == 0) continue; // 跳过无效项
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue; // 跳过 . 和 ..

      memmove(p, de.name, DIRSIZ);
      p[strlen(de.name)] = 0; // 确保名称结束

      find(buf, target); // 递归查找
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    fprintf(2, "Usage: find <path> <name>\n");
    exit(1);
  }
  find(argv[1], argv[2]); // 查找匹配的文件或目录
  exit(0);
}
