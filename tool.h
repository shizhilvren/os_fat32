#include"fs.h"
#ifndef __TOOL__
#define __TOOL__

//大小端转换
//int64
#define BigtoLittle64(A)   ((( (u64)(A) & 0xff00000000000000L ) >> 56) | \
                            (( (u64)(A) & 0x00ff000000000000L ) >> 40)   | \
                            (( (u64)(A) & 0x0000ff0000000000L ) >> 24)   | \
                            (( (u64)(A) & 0x000000ff00000000L ) >> 8)    |\
                            (( (u64)(A) & 0x00000000ff000000L ) << 8) | \
                            (( (u64)(A) & 0x0000000000ff0000L ) << 24)   | \
                            (( (u64)(A) & 0x000000000000ff00L ) << 40)   | \
                            (( (u64)(A) & 0x00000000000000ffL ) << 56))

//int32
#define BigtoLittle32(A)   ((( (u32)(A) & 0xff000000 ) >> 24) | \
                                               (( (u32)(A) & 0x00ff0000 ) >> 8)   | \
                                               (( (u32)(A) & 0x0000ff00 ) << 8)   | \
                                               (( (u32)(A) & 0x000000ff ) << 24))
//int32
#define BigtoLittle32(A)   ((( (u32)(A) & 0xff000000 ) >> 24) | \
                                               (( (u32)(A) & 0x00ff0000 ) >> 8)   | \
                                               (( (u32)(A) & 0x0000ff00 ) << 8)   | \
                                               (( (u32)(A) & 0x000000ff ) << 24))

//int16
#define BigtoLittle16(A)   (( ((u16)(A) & 0xff00) >> 8 )    | \
                            (( (u16)(A) & 0x00ff ) << 8))

//字符转数字 出错返回INF
int ctoi(const char* ch);
//取得参数
int getargv(ARGP argp);
//逻辑簇转实际簇号
u32 L2R(FileSystemInfop fsip,u32 num);

//实际写入函数 offext为偏移块数 为-1时保持文件默认指针 
int do_write_block4k(FILE*fp,BLOCK4K* block4k,int offset);
//实际写入函数 offext为偏移块数 为-1时保持文件默认指针 num 为第几块0-7
int do_write_block(FILE*fp,BLOCK* block,int offset,int num);
//实际读入函数 offext为偏移块数 为-1时保持文件默认指针 
int do_read_block4k(FILE*fp,BLOCK4K* block4k,int offset);
//实际读入函数 offext为偏移块数 为-1时保持文件默认指针 num 为第几块0-7
int do_read_block(FILE*fp,BLOCK* block,int offset,int num);

/* 取得一个空闲簇簇号 返回0失败，否则返回空闲簇好 num为要取得新的空间的逻辑簇号 
    若 num=0则不连接否则标记num下一块为新取得的
*/
u32 newfree(FileSystemInfop fsip,u32 num);
//释放一个已用簇 num簇号返回被删除的下一块
int delfree(FileSystemInfop fsip,u32 num);

/* 取得num簇的下一簇 返回下一簇编号 出错返回0*/
u32 getNext(FileSystemInfop fsip,u32 num);
/* 把输入的文件名转换为83明明格式的文件名 若文件名合法则返回SUCCESS否则返回ERROR */
int nameCheckChange(const char name[ARGLEN],char name38[12]);
//与系统的区别不检查\0x00
char* my_strcpy(char *to,const char*from,int size );
#endif 