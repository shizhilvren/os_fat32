#include<stdio.h>
#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>

int my_close(const ARGP arg,FileSystemInfop fileSystemInfop){
	const char helpstr[]=
"\
功能        关闭当前目录的某个文件\n\
语法格式    close name\n\
		   \n";
 	// FAT_DS_BLOCK4K fat_ds;
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
				DEBUG("|%s|\n",arg->argv[0]);
				break;
    		} 
    	case 0:
    		break;
    	default:
    	error:;
            strcpy(error.msg,"参数数量错误\n\x00");
            printf("参数数量错误\n");
            return ERROR;
    }
    
    
	u32 pathNum=fileSystemInfop->pathNum;
    Opendfilep opendf;

    // for(int i=0;i<OPENFILESIZE;i++){
		int num=ctoi(arg->argv[0]);
		if(num>=0&&num<OPENFILESIZE){
			opendf = &(fileSystemInfop->Opendf[num]);
			if(pathNum == opendf->Dir_Clus && opendf->flag==TRUE && strcmp(opendf->File_name,name)){
				opendf->flag=FALSE;
				return SUCCESS;
			}
		}
		printf("文件未打开\n");
    // }
    return SUCCESS;

}