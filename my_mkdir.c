#include"fs.h"
#include"tool.h"
#include<memory.h>

int nameCheck(const char* name){
    return SUCCESS;
}

int my_mkdir(const ARGP arg,FileSystemInfop fileSystemInfop){
    const char helpstr[]=
"\
功能        创建文件夹\n\
语法格式    mkdir name\n\
name       创建文件夹的名字\n";
    char name[ARGLEN];
    FAT_DS_BLOCK4K fat_ds;
    switch(arg->len){
        case 1:
            if(strcmp(arg->argv[0],"/?")==0){
                printf(helpstr);
                return SUCCESS;
            }else{
                strcpy(name,arg->argv[0]);
            }
            break;
        case 0:
            break;
        default:
        error:;
            strcpy(error.msg,"参数数量错误\n\x00");
            printf("参数数量错误\n");
            return ERROR;
    }
    u32 pathNum=fileSystemInfop->pathNum;
    u32 cut;
    while(pathNum!=FAT_END)
    do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,pathNum);
    
    for(cut=0;cut<SPCSIZE/32;cut++){
        if(fat_ds.fat[cut].DIR_Attr==0){
            break;
        }
    }
    if(cut==SPCSIZE){
        pathNum=getNext(fileSystemInfop,pathNum);
    }


    



    return SUCCESS;
}