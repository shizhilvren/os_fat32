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
                memset(name,0,ARGLEN);
                my_strcpy(name,arg->argv[0],strlen(arg->argv[0]));

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
    while(TRUE){
        do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
        for(cut=0;cut<SPCSIZE/32;cut++){
            if(fat_ds.fat[cut].DIR_Attr==0){
                break;
            }
        }
        if(cut==SPCSIZE){
            u32 lin=pathNum;
            pathNum=getNext(fileSystemInfop,pathNum);
            if(pathNum==FAT_END){
                //全都没有分配一个
                pathNum=newfree(fileSystemInfop,lin);
                if(pathNum==FAT_FREE){
                    strcpy(error.msg,"磁盘空间不足\n\x00");
                    printf("磁盘空间不足\n");
                    return ERROR;
                }
            }
        }else{
            //找到了空的
            //取得. 与..
            u32 pathnumd=newfree(fileSystemInfop,0);
            u32 pathnumdd=newfree(fileSystemInfop,0);
            FAT_DS_BLOCK4K fat_ds_d;
            FAT_DS_BLOCK4K fat_ds_dd;
            memset(&fat_ds_d,0,SPCSIZE);
            memset(&fat_ds_dd,0,SPCSIZE);
            my_strcpy(fat_ds_d.fat[0].name,".           ",11);
            my_strcpy(fat_ds_dd.fat[0].name,"..          ",11);
            fat_ds_d.fat[0].DIR_Attr=


            
            memset(&fat_ds.fat[cut],0,sizeof(FAT_DS));
            my_strcpy(fat_ds.fat[cut].name,name,11);
            fat_ds.fat[cut].DIR_Attr=ATTR_DIRECTORY;
            
            //写入新建文件
            do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
            break;
        }
    }
    
    


    



    return SUCCESS;
}