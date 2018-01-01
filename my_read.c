#include<stdio.h>
#include"fs.h"
#include"tool.h"
#include<memory.h>
#include<ctype.h>
//读取文件最大长度为4098字节
#define MAXLEN 4098

int do_read(int fd,int len,char *text){

	char buffer[1024];
	Opendfilep opendf = &(fileSystemInfop->Opendf[fd]);
	u32 pathNum=opendf->Dir_Clus;
	int offset = L2R(fileSystemInfop,pathNum)
	int num = 0;
	int readBytes = 0;
	int remainBytes = len;

	do{

		if(remainBytes<1024){
			fseek(fp,offset*SPCSIZE+num*BLOCKSIZE,SEEK_SET);
			fread(buffer,1,remainBytes,fp);
			for(int i=0;i<remainBytes;i++){
				text[readBytes] = buffer[i];
			}
			readBytes += readBytes;
			remainBytes = 0;
			break;
		}else{

			fseek(fp,offset*SPCSIZE+num*BLOCKSIZE,SEEK_SET);
			fread(buffer,sizeof(buffer),1,fp);
			for(int i=0;i<sizeof(buffer);i++){
				text[readBytes] = buffer[i];
			}
			remainBytes -= sizeof(buffer);//sizeof(buffer)理论上应为1024
			readBytes += sizeof(buffer);
		}
		//4098 = 1024 * 4
		num += 1;
		if( num ==4 )num=0;
		//最大读取限制
		if(readBytes>((MAXLEN/1024)-1)*1024)break;

		pathNum=getNext(fileSystemInfop,pathNum);
		
	}while(pathNum!=FAT_FREE && pathNum!=FAT_END);

	return readBytes;

}

int my_read(int fd,int len,FileSystemInfop fileSystemInfop){

	char text[MAXLEN];
	int readBytes;
	Opendfilep opendf;
	
	if( fd >9 || fd < 0 ){
		printf("%s\n","文件描述符不合法");
		return ERROR;
	}
	opendf = &(fileSystemInfop->Opendf[fd]);

	if( opendf->flag == FALSE ){
		printf("%s\n","给定文件描述符无效");
		return ERROR;
	}else{
		u32 pathNum=opendf->Dir_Clus;
		FAT_DS_BLOCK4K fat_ds;
		readBytes = do_read(fd,len,text);
		//text[readBytes] = '\0';
		//将结果输出到屏幕
		for(int i=0;text[i]!='\0';i++){
			printf("%s",text[i]);
		}

	}



}
