#include"fs.h"
#include"tool.h"
#include<stdio.h>
#include<stdlib.h>
const char WXML[]="无效命令\n";
int main(){
    printf("欢迎使用文件系统\n");
    my_help();
    char cmd[ARGLEN];
    ARG argv;
    FileSystemInfo fileSysInfo;
    fileSysInfo.flag=FALSE;
    int flag;//执行状态标志位
    while(TRUE){
        if(fileSysInfo.flag){
            printf("%s > ",fileSysInfo.path);
        }else{
            printf("> ");
        }
        scanf("%s",cmd);
        if(getargv(&argv)==ERROR){
            printf(WXML);
        }
        if(strcmp(cmd,"format")==0){
            my_format(&argv);
        }else if(strcmp(cmd,"load")==0){
            my_load(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"mkdir")==0){
            my_mkdir(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"cd")==0){
            my_cd(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"create")==0){
            my_create(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"dir")==0||strcmp(cmd,"ls")==0){
            my_dir(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"rm")==0){
            my_rm(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"rmdir")==0){
            my_rmdir(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"open")==0){
            my_open(&argv,&fileSysInfo);
            DEBUG("open\n");
        }else if(strcmp(cmd,"close")==0){
            my_close(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"write")==0){
            my_write(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"read")==0){
            my_read(&argv,&fileSysInfo);
        }else if(strcmp(cmd,"exit")==0){
            flag=my_exitsys(&argv,&fileSysInfo);
            if(flag==SUCCESS){
                break;
            }
        }else if(strcmp(cmd,"help")==0){
            my_help();
        }else{
            printf(WXML);
        }
    }

    return 0;
}