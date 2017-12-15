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
    int flag;
    while(TRUE){
        scanf("%s",cmd);
        getargv(&argv);
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