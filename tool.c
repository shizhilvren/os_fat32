#include"fs.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


//字符串转换成整 错误返回INF
int ctoi(const char* ch){
    int ret=0;
    for(uint i=0;i<strlen(ch);i++){
        ret*=10;
        if('0'<=ch[i]&&ch[i]<='9'){
            ret+=ch[i]-'0';
        }else{
            return INF;
        }
    }
    return ret;
}

//取得参数 
void getargv(ARGP argp){
    char cmd[ARGLEN*10];
    if(gets(cmd)==NULL){
        argp->len=0;
        return;
    }
    int num=0;
    int len=0;
    int flag=0;
    for(uint i=0;i<strlen(cmd);i++){
        if(cmd[i]==' ' ||cmd[i]=='\t' ||cmd[i]=='\r'){
            flag=0;
            continue;
        }
        if(flag==0){
            num++;
            len=0;
        }
        (argp->argv)[num-1][len]=cmd[i];
        len++;
        flag=1;
    }
    argp->len=num;
}

int do_write_block4k(FILE*fp,BLOCK4K* block4k,int offset){
    if(offset==-1){
        return fwrite(block4k,sizeof(BLOCK4K),1,fp);
    }else{
        fseek(fp,offset*SPCSIZE,SEEK_SET);
        return fwrite(block4k,sizeof(BLOCK4K),1,fp);
    }
}
int do_write_block(FILE*fp,BLOCK* block,int offset,int num){
    if(offset==-1){
        return fwrite(block,sizeof(BLOCK),1,fp);
    }else{
        fseek(fp,offset*SPCSIZE+num*BLOCKSIZE,SEEK_SET);
        return fwrite(block,sizeof(BLOCK),1,fp);
    }
}

char* my_strcpy(char *to,const char*from,int size ){
    for(int i=0;i<size;i++){
        to[i]=from[i];
    }
    return to;
}
