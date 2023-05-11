#include<stdio.h>
#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>
int my_open(const ARGP arg,FileSystemInfop fileSystemInfop){
	char name[12]; 
	const char helpstr[]=
"\
功能        打开当前目录的文件\n\
语法格式    open name\n\
		   \n";
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
	u32 fileclus;
	Opendfilep opendf;
	do{
		do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
		for(cut=0;cut<SPCSIZE/32;cut++){
			char lin[12];
			my_strcpy(lin,fat_ds.fat[cut].name,11);
			lin[11]='\0';
            
            if(strcmp(lin,name)==0){
            	if(fat_ds.fat[cut].DIR_Attr&ATTR_DIRECTORY){
            		printf("不能打开文件夹\n");
            		strcpy(error.msg,"不能打开文件夹\n\x00");
            		return SUCCESS;
            	} 
            	else{
            		fileclus = (u32)( (((u32)fat_ds.fat[cut].DIR_FstClusHI)<<16) |(u32)fat_ds.fat[cut].DIR_FstClusLO );
            		for(int i =0;i<OPENFILESIZE;i++){
            			//只能打开一次文件
            			opendf = &(fileSystemInfop->Opendf[i]);
            			if(opendf->flag == TRUE){
            				if((opendf->Dir_Clus == pathNum)&&(opendf->File_Clus ==fileclus) && (strcmp(opendf->File_name,name)==0)){
            					printf("只能打开一次文件\n");
            					return SUCCESS;
            				}
            				else continue;
            			}else{
            				opendf->flag = TRUE;
            				opendf->Dir_Clus = pathNum;
            				opendf->File_Clus = fileclus;
							opendf->readp = 0;
							opendf->writep = 0;
							opendf->numID = cut;
            				strcpy(opendf->File_name,name);
	            			printf("%s%d\n","打开文件成功 文件描述符是",i);
	            			return SUCCESS;
            			}
            		}
            		printf("打开文件数已达到最大，打开失败\n");
            		return SUCCESS;


                }
            }else{
				continue;
			}
        }
		pathNum=getNext(fileSystemInfop,pathNum);
	}while(pathNum!=FAT_FREE && pathNum!=FAT_END);
	printf("未找到目标文件，打开失败!\n");
	return SUCCESS;
}