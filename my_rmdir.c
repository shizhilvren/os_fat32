#include<stdio.h>
#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>


int isEmpty(FileSystemInfop fileSystemInfop,u32 pathNum){
	u32 cut;
	FAT_DS_BLOCK4K fat_ds;
	// u32 delfileNum;
    do{
		do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
		for(cut=0;cut<SPCSIZE/32;cut++){
			if(fat_ds.fat[cut].name[0]=='\xe5' || fat_ds.fat[cut].name[0]=='\x00'){
				//被删除的 与空
				continue;
			}else{
                char name[12];
                my_strcpy(name,fat_ds.fat[cut].name,11);
                name[11]='\0';
                DEBUG("|%s|\n",name);
                DEBUG("%s\n",DIR_d);
                DEBUG("%s\n",DIR_dd);
                if(strcmp(name,DIR_d)==0 || strcmp(name,DIR_dd)==0){
                    continue;
                }else{
                    return FALSE;
                }
            }
		}
		pathNum=getNext(fileSystemInfop,pathNum);
	}while(pathNum!=FAT_FREE && pathNum!=FAT_END);
	return TRUE;
}



//暂时设定为只能删除当前目录下的文件,不包含非空目录
int my_rmdir(const ARGP arg,FileSystemInfop fileSystemInfop){
	char delname[12];
	const char helpstr[]=
"\
功能		删除文件夹\n\
格式		rm name\n\
name	  想要删除的文件夹名\n";

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
				if(nameCheckChange(arg->argv[0],delname)==ERROR){
                    strcpy(error.msg,"文件夹名过长或存在非法字符\n\x00");
                    printf("文件夹名过长或存在非法字符\n");
                    return ERROR;
                }
                for(int i=0;i<11;i++){
                    delname[i]=toupper(delname[i]);
                }
                delname[11]='\0';
				DEBUG("|%s|\n",delname);
                if(strcmp(arg->argv[0],"..")==0||strcmp(arg->argv[0],".")==0){
                    printf("文件夹不存在\n");
	                return SUCCESS;
                }
				break;
			}
		case 0:
			DEBUG("未输入文件名\n");
			return SUCCESS;
		default:
		error:;
			strcpy(error.msg,"参数数量错误\n\x00");
			printf("参数数量错误\n");
			return ERROR;
	}
	u32 pathNum=fileSystemInfop->pathNum;
	u32 cut;
	FAT_DS_BLOCK4K fat_ds;
	u32 delfileNum;

	do{
		do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
		for(cut=0;cut<SPCSIZE/32;cut++){
			char name[12];
			my_strcpy(name,fat_ds.fat[cut].name,11);
			name[11]='\0';
			if(fat_ds.fat[cut].name[0]=='\xe5'){
				//被删除的
				continue;
			}
			DEBUG("|%s|\n|%s|\n",delname,name);
			if( (fat_ds.fat[cut].DIR_Attr&ATTR_DIRECTORY) && strcmp(delname,name)==0 ){
				//找到了要删的目录
                delfileNum=(u32)( (((u32)fat_ds.fat[cut].DIR_FstClusHI)<<16) |(u32)fat_ds.fat[cut].DIR_FstClusLO );
                if(isEmpty(fileSystemInfop,delfileNum)==FALSE){
                    //空判断
                    strcpy(error.msg,"文件夹非空\n\x00");
                    printf("文件夹非空\n");
                    return ERROR;
                }
                while(delfileNum!=FAT_END && delfileNum!=FAT_FREE){
					delfileNum=delfree(fileSystemInfop,delfileNum);
				}
				fat_ds.fat[cut].name[0]='\xe5';
				do_write_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
				return SUCCESS;    	
			}else{
				continue;
			}
		}
		pathNum=getNext(fileSystemInfop,pathNum);
	}while(pathNum!=FAT_FREE && pathNum!=FAT_END);
	printf("文件夹不存在\n");
	return SUCCESS;
}