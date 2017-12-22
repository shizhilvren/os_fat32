#include"fs.h"
#include"tool.h"
#include<memory.h>

int nameCheck(const char* name){

}

int my_mkdir(const ARGP arg,FileSystemInfop fileSystemInfop){
    const char helpstr[]=
"\
功能        创建文件夹\n\
语法格式    mkdir name\n\
name       创建文件夹的名字\n";
    char name;
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

    



    return SUCCESS;
}