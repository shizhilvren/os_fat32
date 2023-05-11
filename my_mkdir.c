#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>

int nameCheck(char name[ARGLEN]){
    if(strlen(name)>11||strlen(name)<=0){
        return ERROR;
    }
    for(u32 i=0;i<strlen(name);i++){
        if(!(isalnum(name[i]) || isalpha(name[i]) || isspace(name[i]) ||
                 name[i]=='$' || name[i]=='%' || name[i]=='\'' || name[i]=='-' ||
                  name[i]=='_' || name[i]=='@' || name[i]=='~' || name[i]=='`' || 
                  name[i]=='!' || name[i]=='(' || name[i]==')' || name[i]=='{' || 
                  name[i]=='}' || name[i]=='^' || name[i]=='#' || name[i]=='&')){
            return ERROR;
        }
    }
    for(int i=strlen(name);i<11;i++){
        name[i]=' ';
    }
    name[11]='\x0';
    return SUCCESS;
}

int my_mkdir(const ARGP arg,FileSystemInfop fileSystemInfop){
    const char helpstr[]=
"\
功能        创建文件夹\n\
语法格式    mkdir name\n\
name       创建文件夹的名字\n\
备注       文件名强制转为大写，文件名最长不超过8位\n";
    char name[ARGLEN];
    FAT_DS_BLOCK4K fat_ds;
    if(fileSystemInfop->flag==FALSE){
        strcpy(error.msg,"未指定文件系统\n\x00");
        printf("未指定文件系统\n");
        return ERROR;
    }
    switch(arg->len){
        case 1:
            if(strcmp(arg->argv[0],"/?")==0){
                printf(helpstr);
                return SUCCESS;
            }else{
                // memset(name,' ',ARGLEN);
                strcpy(name,arg->argv[0]);
                // name[11]='\0';
                if(nameCheck(name)==ERROR){
                    strcpy(error.msg,"文件名过长或存在非法字符\n\x00");
                    printf("文件名过长或存在非法字符\n");
                    return ERROR;
                }
                for(int i=0;i<11;i++){
                    name[i]=toupper(name[i]);
                }
                name[11]='\0';
            }
            break;
        case 0:
            DEBUG("文件名空\n");
            return SUCCESS;
        default:
        error:;
            strcpy(error.msg,"参数数量错误\n\x00");
            printf("参数数量错误\n");
            return ERROR;
    }

    u32 pathNum=fileSystemInfop->pathNum;
    u32 cut;
    while(TRUE){
        //检查这一页的目录项
        do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
        for(cut=0;cut<SPCSIZE/32;cut++){
            char lin[12];
            my_strcpy(lin,fat_ds.fat[cut].name,11);
            lin[11]='\0';
            if(
                // fat_ds.fat[cut].name[0]=='\xE5' 
            // || 
            fat_ds.fat[cut].name[0]=='\x00'){
                break;
            }else if((fat_ds.fat[cut].DIR_Attr) && 
                            strcmp(name,lin)==0 ){
                strcpy(error.msg,"文件已存在\n\x00");
                printf("文件已存在\n");
                return ERROR;
            }
        }
        //取得下一个目录项
        if(cut==SPCSIZE/32){
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
            FAT_DS_BLOCK4K fat_ds_d;
            memset(&fat_ds_d,0,SPCSIZE);
            my_strcpy(fat_ds_d.fat[0].name,DIR_d,11);
            my_strcpy(fat_ds_d.fat[1].name,DIR_dd,11);
            fat_ds_d.fat[0].DIR_Attr=ATTR_DIRECTORY;
            fat_ds_d.fat[1].DIR_Attr=ATTR_DIRECTORY;
            fat_ds_d.fat[0].DIR_FstClusHI=(u16)(pathnumd>>16);
            fat_ds_d.fat[0].DIR_FstClusLO=(u16)(pathnumd&0x0000ffff);
            fat_ds_d.fat[1].DIR_FstClusHI=(u16)(pathNum>>16);
            fat_ds_d.fat[1].DIR_FstClusLO=(u16)(pathNum&0x0000ffff);
            do_write_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds_d,L2R(fileSystemInfop,pathnumd));
            
            memset(&fat_ds.fat[cut],0,sizeof(FAT_DS));
            my_strcpy(fat_ds.fat[cut].name,name,11);
            fat_ds.fat[cut].DIR_FstClusHI=(u16)(pathnumd>>16);
            fat_ds.fat[cut].DIR_FstClusLO=(u16)(pathnumd&0x0000ffff);
            fat_ds.fat[cut].DIR_Attr=ATTR_DIRECTORY;
            
            //写入新建文件
            do_write_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
            DEBUG("创建成功\n");
            break;
        }
    }
    return SUCCESS;
}