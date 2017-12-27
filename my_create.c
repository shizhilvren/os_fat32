#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>

int nameCheckChange(const char name[ARGLEN],char name38[12]){
    if(strlen(name)>12||strlen(name)<=0){
        return ERROR;
    }
    int point=-1;
    for(u32 i=0;i<strlen(name);i++){
        if(name[i]=='.'){
            point=i;
            continue;
        }
        if(!(isalnum(name[i]) || isalpha(name[i]) || isspace(name[i]) ||
                 name[i]=='$' || name[i]=='%' || name[i]=='\'' || name[i]=='-' ||
                  name[i]=='_' || name[i]=='@' || name[i]=='~' || name[i]=='`' || 
                  name[i]=='!' || name[i]=='(' || name[i]==')' || name[i]=='{' || 
                  name[i]=='}' || name[i]=='^' || name[i]=='#' || name[i]=='&')){
            return ERROR;
        }
    }
    if( ((point!=-1) && (point<=8 && strlen(name)-point-1<=3 )) 
                || (point==-1 && strlen(name)<=8 )){
        memset(name38,' ',11);
        name38[11]='\0';
        if(point==0){
            return ERROR;
        }else if(point!=-1){
            for(int i=0;i<point;i++){
                name38[i]=name[i];
            }
            for(int i=point+1;i<(int)strlen(name);i++){
                name38[i-point+8-1]=name[i];
            }
        }else{
            for(int i=0;i<(int)strlen(name);i++){
                name38[i]=name[i];
            }
        }
        return SUCCESS;
    }
    return ERROR;;
}

int my_create(const ARGP arg,FileSystemInfop fileSystemInfop){
    const char helpstr[]=
"\
功能        创建文件\n\
语法格式    create name\n\
name       创建文件夹的名字\n\
备注       文件名强制转为大写，文件名要符合三八命名方式\n";
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
                memset(name,' ',ARGLEN);
                my_strcpy(name,arg->argv[0],strlen(arg->argv[0]));
                name[11]='\0';
                if(nameCheckChange(arg->argv[0],name)==ERROR){
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
            if(fat_ds.fat[cut].DIR_Attr==0){
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
            memset(&fat_ds.fat[cut],0,sizeof(FAT_DS));
            my_strcpy(fat_ds.fat[cut].name,name,11);
            fat_ds.fat[cut].DIR_Attr=ATTR_ARCHIVE;
            
            //写入新建文件
            do_write_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
            DEBUG("创建成功\n");
            break;
        }
    }
    return SUCCESS;
}