#include<stdio.h>
#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>
//读取文件最大长度为4098字节
#define MAXLEN 4098
int my_read(const ARGP arg,FileSystemInfop fileSystemInfop){
	char name[12]; 
	u32 start=0;
	/* 0为读取所有 */
	u32 len=0; 
	const char helpstr[]=
"\
功能        读取文件内容\n\
语法格式    read name [len[start]]\n\
name	   要读的文件名\n\
len			读取得文件长度 默认为所有\n\
start		读取的开始位置 默认为0\n";
 	FAT_DS_BLOCK4K fat_ds;
    if(fileSystemInfop->flag==FALSE){
        strcpy(error.msg,"未指定文件系统\n\x00");
        printf("未指定文件系统\n");
        return ERROR;
    }
    switch(arg->len){
		case 3:
			start=ctoi(arg->argv[2]);
			if(start==INF){
				goto error;
			}
		case 2:
			len=ctoi(arg->argv[1]);
			if(len==INF){
				goto error;
			}
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
            if((fat_ds.fat[cut].DIR_Attr&ATTR_ARCHIVE) && strcmp(name,lin)==0){ 
                //文件
				for(int i=0;i<OPENFILESIZE;i++){
                    opendf = &(fileSystemInfop->Opendf[i]);
                    if(pathNum == opendf->Dir_Clus && opendf->flag==TRUE && strcmp(opendf->File_name,name)==0){
                        char buf[ARGLEN*10];
						int readlen=0;
						if(len==0){
							len=fat_ds.fat[cut].DIR_FileSize;
						}
						while(len-readlen>ARGLEN*10){
							readlen+=read_real(i,start+readlen,ARGLEN*10,(void*)buf,fileSystemInfop);
							for(int i=0;i<ARGLEN*10;i++){
								printf("%c",buf[i]);
							}
						}
						int lin=read_real(i,start+readlen,len-readlen,(void*)buf,fileSystemInfop);
						for(int i=0;i<lin;i++){
							printf("%c",buf[i]);
						}
						printf("\n");
						return SUCCESS;
                    }
                }
				printf("文件未打开\n");
				return SUCCESS;
			}
		}
		pathNum=getNext(fileSystemInfop,pathNum);
	}while(pathNum!=FAT_FREE && pathNum!=FAT_END);
	printf("文件不存在\n");
	return SUCCESS;
}

int read_real(int fnum,u32 start,u32 size,void* buf,FileSystemInfop fileSystemInfop){
	FAT_DS_BLOCK4K fat_ds;
	BLOCK4K block4k;
    /* 文件描述符非法 */
    if(fnum<0&&fnum>=OPENFILESIZE){
        return -1;
    }
	Opendfilep opendf = &(fileSystemInfop->Opendf[fnum]);

	do_read_block4k(fileSystemInfop->fp,(BLOCK4K*)&fat_ds,L2R(fileSystemInfop,opendf->Dir_Clus));
	if(start>fat_ds.fat[opendf->numID].DIR_FileSize){
		return -1;
	}
	// if(size==0){
	// 	size=fat_ds.fat[opendf->numID].DIR_FileSize;
	// }
	if(start+size>fat_ds.fat[opendf->numID].DIR_FileSize){
		size=fat_ds.fat[opendf->numID].DIR_FileSize-start;
	}
	
	int where=SPCSIZE;
	u32 fileclus=opendf->File_Clus;
	u32 fileclusold=0;
	for(u32 i=0;i<start/SPCSIZE;i++){
		fileclus=getNext(fileSystemInfop,fileclus);
		where+=SPCSIZE;
	}
	u32 len=size;
	opendf->readp=start;
    /* 4k读入入补齐 */
	int readlen=0;
    if(opendf->readp%SPCSIZE!=0){
        do_read_block4k(fileSystemInfop->fp,&block4k,L2R(fileSystemInfop,fileclus));
        int lin;
        if(len-readlen<(SPCSIZE-opendf->readp%SPCSIZE)){
            lin=len-readlen;
        }else{
            lin=(SPCSIZE-opendf->readp%SPCSIZE);
        }
		readlen+=lin;
        my_strcpy((char*)buf,&(((char*)&block4k)[(opendf->readp%SPCSIZE)]),lin);
        opendf->readp+=lin;
        fileclusold=fileclus;
        fileclus=getNext(fileSystemInfop,fileclus);
    }
	while(len-readlen>0){
        do_read_block4k(fileSystemInfop->fp,&block4k,L2R(fileSystemInfop,fileclus));
        int lin;
        if(len-readlen<SPCSIZE){
            lin=len-readlen;
        }else{
            lin=SPCSIZE;
        }
        my_strcpy((char*)(&((char*)buf)[readlen]),((char*)(&block4k)),lin);
        readlen+=lin;
		opendf->readp+=lin;
        fileclusold=fileclus;
        fileclus=getNext(fileSystemInfop,fileclus);
    }
	return readlen;

}

// int my_read(int fd,int len,FileSystemInfop fileSystemInfop){

// 	char text[MAXLEN];
// 	int readBytes;
// 	Opendfilep opendf;
	
// 	if( fd >=OPENFILESIZE || fd < 0 ){
// 		printf("%s\n","文件描述符不合法");
// 		return ERROR;
// 	}
// 	opendf = &(fileSystemInfop->Opendf[fd]);

// 	if( opendf->flag == FALSE ){
// 		printf("%s\n","给定文件描述符无效");
// 		return ERROR;
// 	}else{
// 		u32 pathNum=opendf->Dir_Clus;
// 		FAT_DS_BLOCK4K fat_ds;
// 		readBytes = do_read(fd,len,text);
// 		//text[readBytes] = '\0';
// 		//将结果输出到屏幕
// 		for(int i=0;text[i]!='\0';i++){
// 			printf("%s",text[i]);
// 		}

// 	}



// }
