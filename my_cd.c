#include<stdio.h>
#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>

int my_cd(const ARGP arg,FileSystemInfop fileSystemInfop){
	char name[12]; 
	const char helpstr[]=
"\
功能        进入文件夹\n\
语法格式    cd name\n\
name       进入文件夹的名字\n\
备注       文件名强制转为大写，文件名最长不超过8位\n";
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
    			//DEBUG("hear\n");
    			memset(name,' ',12);
    			my_strcpy(name,arg->argv[0],strlen(arg->argv[0]));
    			name[11]='\0';
    			for(int i=0;i<11;i++){
    				name[i]=toupper(name[i]);
    			}
    			//DEBUG("%s|\n",name);
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
//	while(TRUE){
	do{
		do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,pathNum));
		for(cut=0;cut<SPCSIZE/32;cut++){
			char lin[12];
			my_strcpy(lin,fat_ds.fat[cut].name,11);//是获取文件的名称？？？
			lin[11]='\0';
			
//			if(fat_ds.fat[cut].DIR_Attr==0){//是什么特殊的属性吗 
//   				break;
//			}else 
		//	DEBUG("in1%s|\n",lin);
			//DEBUG("in2%s|\n",name);
			if((fat_ds.fat[cut].DIR_Attr&ATTR_DIRECTORY) 
					&& strcmp(name,lin)==0 ){//前半部分什么意思，介绍下read的具体步骤 
				//路径快修改
				fileSystemInfop->pathNum=(u32)( (((u32)fat_ds.fat[cut].DIR_FstClusHI)<<16)
																	| (u32)fat_ds.fat[cut].DIR_FstClusLO );
				//DEBUG("%u\n",fileSystemInfop->pathNum);
				//DEBUG("%u\n",fat_ds.fat[cut].DIR_FstClusHI);
				//DEBUG("%u\n",fat_ds.fat[cut].DIR_FstClusLO);
				//路径字符串修改
				if(strcmp(lin,DIR_d)==0){

				}else if (strcmp(lin,DIR_dd)==0){
					int lin=0;
					for(int i=strlen(fileSystemInfop->path)-1;i>=0;i--){
						if(fileSystemInfop->path[i]=='/'){
							lin++;
							// fileSystemInfop->path[i]=0x00;
							if(lin==2){
								fileSystemInfop->path[i+1]=0x00;
								break;
							}
							
						}
					}
				}else{
					strcat(fileSystemInfop->path,lin);
					for(int i=strlen(fileSystemInfop->path)-1;i>=0;i--){
						if(fileSystemInfop->path[i]==' '){
							fileSystemInfop->path[i]=0x00;
						}else{
							break;
						}
					}
					strcat(fileSystemInfop->path,"/");
					fileSystemInfop->path[strlen(fileSystemInfop->path)]='\x0';
				}
				
				// printf("成功\n");
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
