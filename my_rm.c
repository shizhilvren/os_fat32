#include<stdio.h>
#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>
//暂时设定为只能删除当前目录下的文件,不包含非空目录
int my_rm(const ARGP arg,FileSystemInfop fileSystemInfop){
	char delname[12];
	const char helpstr[]=
"\
功能		删除文件或空目录\n\
格式		rm name\n\
name		name of the file want to delete\n  ";

	if(fileSystemInfop->flag==FALSE){
		strcpy(error.msg,"未指定文件系统\n\x00");
		printf("未指定文件系统\n");
		return ERROR;
	}

	switch(arg->len){
		case 1:
			memset(delname,' ',12);
			my_strcpy(delname,arg->argv[0],strlen(arg->argv[0]));
			delname[11]='\0';
				break;
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
			//判断是否是目录，是目录再判断是否空目录
			if( strcmp(delname,name)==0 ){
				if( !( fat_ds.fat[cut].DIR_Attr&ATTR_DIRECTORY )){

				}
				else{
					if(fat_ds.fat[cut].DIR_FileSize==32){
						//删除空目录
					}
					else{
						
						strcpy(error.msg,"目录非空\n\x00");
						printf("目录非空\n");
						return SUCCESS
					}
				}
				delfileNum=(u32)( (((u32)fat_ds.fat[cut].DIR_FstClusHI)<<16)
																	| (u32)fat_ds.fat[cut].DIR_FstClusLO );

				printf("成功\n");
				return SUCCESS;	    	
			}else{
				continue;
			}
		}
		pathNum=getNext(fileSystemInfop,pathNum);
	}while(pathNum!=FAT_FREE && pathNum!=FAT_END);
	printf("文件不存在\n");
	return SUCCESS;
}