#include "fs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int do_write_block4k(FILE* fp, BLOCK4K* block4k, int offset)
{
    int ret = -1;
    if (offset == -1) {
        ret = fwrite(block4k, sizeof(BLOCK4K), 1, fp);
    } else {
        fseek(fp, offset * SPCSIZE, SEEK_SET);
        ret = fwrite(block4k, sizeof(BLOCK4K), 1, fp);
    }
    return ret;
}
int do_write_block(FILE* fp, BLOCK* block, int offset, int num)
{
    int ret = -1;
    if (offset == -1) {
        ret = fwrite(block, sizeof(BLOCK), 1, fp);
    } else {
        // DEBUG("write %x-%x\n", offset * SPCSIZE + num * BLOCKSIZE, offset * SPCSIZE + num * BLOCKSIZE + sizeof(BLOCK));
        fseek(fp, offset * SPCSIZE + num * BLOCKSIZE, SEEK_SET);
        ret = fwrite(block, sizeof(BLOCK), 1, fp);
    }
    return ret;
}

int do_read_block4k(FILE* fp, BLOCK4K* block4k, int offset)
{

    int ret = -1;
    if (offset == -1) {
        ret = fread(block4k, sizeof(BLOCK4K), 1, fp);
    } else {
        fseek(fp, offset * SPCSIZE, SEEK_SET);
        ret = fread(block4k, sizeof(BLOCK4K), 1, fp);
    }
    return ret;
}
int do_read_block(FILE* fp, BLOCK* block, int offset, int num)
{
    
    int ret = -1;
    if (offset == -1) {
        ret = fread(block, sizeof(BLOCK), 1, fp);
    } else {
        DEBUG("read %x-%x\n", offset * SPCSIZE + num * BLOCKSIZE, offset * SPCSIZE + num * BLOCKSIZE + sizeof(BLOCK));
        fseek(fp, offset * SPCSIZE + num * BLOCKSIZE, SEEK_SET);
        ret = fread(block, sizeof(BLOCK), 1, fp);
    }
    return ret;
}

u32 L2R(FileSystemInfop fsip, u32 num)
{
    // DEBUG("lj %u->%u\n",num,(fsip->MBR_start+
    // fsip->BPB_RsvdSecCnt+
    // fsip->BPB_FATSz32*fsip->BPB_NumFATs)/fsip->BPB_SecPerClus+num-2);
    return (fsip->MBR_start + fsip->BPB_RsvdSecCnt + fsip->BPB_FATSz32 * fsip->BPB_NumFATs) / fsip->BPB_SecPerClus + num - 2;
}

u32 getNext(FileSystemInfop fsip, u32 num)
{
    if (num / (512 / 4) > fsip->BPB_FATSz32) {
        return 0;
    }
    u32 cuNum = num / (512 / 4);
    u32 index = num % (512 / 4);
    FAT fat;
    do_read_block(fsip->fp, (BLOCK*)&fat, (fsip->FAT[0] + cuNum) / 8, (fsip->FAT[0] + cuNum) % 8);
    return fat.fat[index];
}

int newfree(FileSystemInfop fsip, u32 num)
{
    FAT fat;
    u32 cuNum = num / (512 / 4);
    u32 index = num % (512 / 4);
    for (u32 i = 0; i < fsip->BPB_FATSz32; i++) {
        do_read_block(fsip->fp, (BLOCK*)&fat, (fsip->FAT[0] + i) / 8, (fsip->FAT[0] + i) % 8);
        for (int j = 0; j < 512 / 4; j++) {
            if (fat.fat[j] == FAT_FREE) {
                fat.fat[j] = FAT_END;
                for (int i = 0; i < fsip->BPB_NumFATs; i++) {
                    do_write_block(fsip->fp, (BLOCK*)&fat, (fsip->FAT[i] + i) / 8, (fsip->FAT[i] + i) % 8);
                }
                if (num != 0) {
                    do_read_block(fsip->fp, (BLOCK*)&fat, (fsip->FAT[0] + cuNum) / 8, (fsip->FAT[0] + cuNum) % 8);
                    fat.fat[index] = i * (512 / 4) + j;
                    for (int i = 0; i < fsip->BPB_NumFATs; i++) {
                        do_write_block(fsip->fp, (BLOCK*)&fat, (fsip->FAT[i] + cuNum) / 8, (fsip->FAT[i] + cuNum) % 8);
                    }
                }
                BLOCK4K block4k;
                memset(&block4k, 0, SPCSIZE);
                do_write_block4k(fsip->fp, &block4k, L2R(fsip, i * (512 / 4) + j));
                return i * (512 / 4) + j;
            }
        }
    }
    return 0;
}
int delfree(FileSystemInfop fsip, u32 num)
{
    FAT fat;
    u32 cuNum = num / (512 / 4);
    u32 index = num % (512 / 4);
    do_read_block(fsip->fp, (BLOCK*)&fat, (fsip->FAT[0] + cuNum) / 8, (fsip->FAT[0] + cuNum) % 8);
    u32 ret = fat.fat[index];
    fat.fat[index] = FAT_FREE;
    for (int i = 0; i < fsip->BPB_NumFATs; i++) {
        do_write_block(fsip->fp, (BLOCK*)&fat, (fsip->FAT[i] + cuNum) / 8, (fsip->FAT[i] + cuNum) % 8);
    }
    return ret;
}

char* my_strcpy(char* to, const char* from, int size)
{
    for (int i = 0; i < size; i++) {
        to[i] = from[i];
    }
    return to;
}

// 取得命令参数
int getargv(ARGP argp)
{
    char cmd[ARGLEN * 10];
    if (gets(cmd) == NULL) {
        argp->len = 0;
        return SUCCESS;
    }
    int num = 0;
    int len = 0;
    int flag = 0;
    for (u32 i = 0; i < strlen(cmd); i++) {
        if (len >= ARGLEN) {
            return ERROR;
        }
        if (num >= ARGNUM) {
            return ERROR;
        }
        if (cmd[i] == ' ' || cmd[i] == '\t' || cmd[i] == '\r') {
            flag = 0;
            continue;
        }
        if (flag == 0) {
            num++;
            len = 0;
        }
        (argp->argv)[num - 1][len] = cmd[i];
        (argp->argv)[num - 1][len + 1] = 0;
        len++;
        flag = 1;
    }
    argp->len = num;
    return SUCCESS;
}

// 数字字符串转换为int类型数字,如果非法返回INF，不考虑溢出
int ctoi(const char* ch)
{
    int ret = 0;
    for (u32 i = 0; i < strlen(ch); i++) {
        ret *= 10;
        if ('0' <= ch[i] && ch[i] <= '9') {
            ret += ch[i] - '0';
        } else {
            return INF;
        }
    }
    return ret;
}

int nameCheckChange(const char name[ARGLEN], char name38[12])
{
    if (strlen(name) > 12 || strlen(name) <= 0) {
        return ERROR;
    }
    int point = -1;
    for (u32 i = 0; i < strlen(name); i++) {
        if (name[i] == '.') {
            point = i;
            continue;
        }
        if (!(isalnum(name[i]) || isalpha(name[i]) || isspace(name[i]) || name[i] == '$' || name[i] == '%' || name[i] == '\'' || name[i] == '-' || name[i] == '_' || name[i] == '@' || name[i] == '~' || name[i] == '`' || name[i] == '!' || name[i] == '(' || name[i] == ')' || name[i] == '{' || name[i] == '}' || name[i] == '^' || name[i] == '#' || name[i] == '&')) {
            return ERROR;
        }
    }
    if (((point != -1) && (point <= 8 && strlen(name) - point - 1 <= 3))
        || (point == -1 && strlen(name) <= 8)) {
        memset(name38, ' ', 11);
        name38[11] = '\0';
        if (point == 0) {
            return ERROR;
        } else if (point != -1) {
            for (int i = 0; i < point; i++) {
                name38[i] = name[i];
            }
            for (int i = point + 1; i < (int)strlen(name); i++) {
                name38[i - point + 8 - 1] = name[i];
            }
        } else {
            for (int i = 0; i < (int)strlen(name); i++) {
                name38[i] = name[i];
            }
        }
        return SUCCESS;
    }
    return ERROR;
    ;
}

int debug_in(char* format, ...)
{
    return 0;
}