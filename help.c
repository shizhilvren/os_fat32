#include<stdio.h>
#include"fs.h"
const char help_str[]="文件系统\n\
符合fat32标注文件可以作为虚拟磁盘挂载\n\
命令      作用\n\
format    格式化文件系统\n\
load      加载文件系统\n\
mkdir     创建文件夹\n\
cd        进入文件夹\n\
create    创建文件\n\
dir       列出当前目录下文件\n\
rm        移除文件\n\
rmdir     移除空文件夹\n\
open      打开文件\n\
close     关闭文件\n\
write     写文件\n\
read      读文件\n\
help      显示提示\n\
\n";

int my_help(){
    printf("%s",help_str);
    return SUCCESS;
}