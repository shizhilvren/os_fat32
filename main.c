#include"fs.h"
#include"tool.h"
#include<stdio.h>
#include<stdlib.h>
const char WXML[]="无效命令\n";
int main(){
    printf("欢迎使用文件系统\n");
    char cmd[ARGLEN];
    ARG argv;
    FileSystemInfo fileSysInfo;
    fileSysInfo.flag=FALSE;
    int flag;//执行状态标志位
    while(TRUE){
        scanf("%s",cmd);
        if(getargv(&argv)==ERROR){
            printf(WXML);
        }
        if(strcmp(cmd,"format")==0){
            my_format(&argv);
        }else if(strcmp(cmd,"load")==0){
            my_load(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"exit")==0){
            flag=my_exitsys(&argv,&fileSysInfo);
            if(flag==SUCCESS){
                break;
            }
        }else{
            printf(WXML);
        }
    }

    return 0;
}