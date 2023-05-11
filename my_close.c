#include<stdio.h>
#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>

int my_close(const ARGP arg,FileSystemInfop fileSystemInfop){
	char name[12];
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
			if(nameCheckChange(arg->argv[0],name)==ERROR){
                strcpy(error.msg,"文件名过长或存在非法字符\n\x00");
                printf("文件名过长或存在非法字符\n");
                return ERROR;
            }
            for(int i=0;i<11;i++){
                name[i]=toupper(name[i]);
            }
            name[11]='\0';
            DEBUG("|%s|\n",name);
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
	u32 cut;
	FAT_DS_BLOCK4K fat_ds;
	Opendfilep opendf;
	do{
		do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
		for(cut=0;cut<SPCSIZE/32;cut++){
			char lin[12];
			my_strcpy(lin,fat_ds.fat[cut].name,11);
			lin[11]='\0';
			if(fat_ds.fat[cut].name[0]=='\xe5'){
				//被删除的
				continue;
			}
			if((fat_ds.fat[cut].DIR_Attr&ATTR_ARCHIVE)&&strcmp(lin,name)==0){ 
                //文件
				for(int i=0;i<OPENFILESIZE;i++){
                    opendf = &(fileSystemInfop->Opendf[i]);
                    if(pathNum == opendf->Dir_Clus && opendf->flag==TRUE && strcmp(opendf->File_name,name)==0){
						close_in(i,fileSystemInfop);
                        return SUCCESS;
                    }
                }
                printf("文件未打开\n");
                return SUCCESS;
			}
		}
		pathNum=getNext(fileSystemInfop,pathNum);
	}while(pathNum!=FAT_FREE && pathNum!=FAT_END);

    return SUCCESS;

}

int close_in(int fnum,FileSystemInfop fileSystemInfop){
    /* 文件描述符非法 */
    if(fnum<0&&fnum>=OPENFILESIZE){
        return ERROR;
    }
    Opendfilep opendf = &(fileSystemInfop->Opendf[fnum]);
	opendf->flag=FALSE;
	return SUCCESS;
}