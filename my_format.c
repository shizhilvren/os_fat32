#include"fs.h"
#include"tool.h"
#include<stdlib.h>
#include<string.h>
/*
    接受一个参数 不得小于256MB 不得大于2097152MB(2TB)
    默认一个磁盘块512B 一簇4KB 8个块
*/
#define MIN 256
#define MAX 2097152
//单位字节
void vhdset(HD_FTRp vhd,u64 size){
    strcpy(vhd->cookie,"conectix");
    vhd->features=BigtoLittle32(0x00000002);
    vhd->ff_version=BigtoLittle32(0x00010000);
    vhd->data_offset=BigtoLittle64(0xFFFFFFFFFFFFFFFF);
    vhd->timestamp=0x00000000;
    strcpy(vhd->crtr_app,"myfs");
    vhd->crtr_ver=BigtoLittle32(0x00060001);
    // strcpy((char*)vhd->crtr_os,"Wi2k");
    vhd->crtr_os=BigtoLittle32(0x5769326b);
    vhd->orig_size=BigtoLittle64(size);
    vhd->curr_size=BigtoLittle64(size);
    vhd->geometry=BigtoLittle32(0x03eb0c11);
    vhd->type=BigtoLittle32(2); //类型
    vhd->checksum=0;
    // strcmp(vhd->uuid,"                ");
    memset(vhd->uuid,0xff,sizeof(vhd->uuid));
    memset(vhd->reserved,0,sizeof(vhd->reserved));
    u32 chksum=0;
    u8* p=(u8*)vhd;
    for(int i=0;i<512;i++){
        chksum+=p[i];
    }
    vhd->checksum=BigtoLittle32(~chksum);
}
//size为字节数
void MBRset(MBRp mbr,int size){
    (mbr->mbr_in[0]).flag=0;
    (mbr->mbr_in[0]).FSflag=0x0b;
    mbr->mbr_in[0].strart_chan=0x00000008;
    mbr->mbr_in[0].all=size/BLOCKSIZE-mbr->mbr_in[0].strart_chan;
    mbr->end=0xAA55;
}

//size 为字节数
void BS_BPSset(BS_BPBp bs_bpb,int size,int hiden){
    my_strcpy(bs_bpb->BS_jmpBoot,"\0xEB\0x00\0x00",sizeof(bs_bpb->BS_jmpBoot));
    my_strcpy(bs_bpb->BS_OEMName,"MSWIN4.1",sizeof(bs_bpb->BS_OEMName));
    bs_bpb->BPB_BytsPerSec=512;
    bs_bpb->BPB_SecPerClus=8;//
    bs_bpb->BPB_RsvdSecCnt=32;
    bs_bpb->BPB_NumFATs=2;
    bs_bpb->BPB_RootEntCnt=0;
    bs_bpb->BPB_TotSec16=0;
    bs_bpb->BPB_Media=0xf8;
    bs_bpb->BPB_FATSz16=0;
    bs_bpb->BPB_SecPerTrk=0;
    bs_bpb->BPB_NumHeads=0;
    bs_bpb->BPB_HiddSec=hiden;
    bs_bpb->BPB_TotSec32=size/BLOCKSIZE;

    bs_bpb->BPB_FATSz32=size/BLOCKSIZE+1;
    bs_bpb->BPB_ExtFlags=0;
    bs_bpb->BPB_FSVer=0;
    bs_bpb->BPB_RootClis=2;
    bs_bpb->BPB_FSInfo=1;
    bs_bpb->BPB_BkBootSec=6;
    memset(bs_bpb->BPB_Reserved,0,sizeof(bs_bpb->BPB_Reserved));
    bs_bpb->BS_DrvNum=0;
    bs_bpb->BS_Reserved1=0;
    bs_bpb->BS_BootSig=0x29;
    // char BS_VolID[4];
    // char BS_VolLab[11];
    my_strcpy(bs_bpb->BS_FilSysType,"FAT32    ",8);
    bs_bpb->end=0xAA55;
}

int my_format(const ARGP arg){
    DEBUG("%d",sizeof(BS_BPB));
    BLOCK4K block4k;
    FILE *fp=NULL;
    if(arg->len!=1){
        strcpy(error.msg,"参数数量错误\n\x00");
        printf("参数数量错误\n");
        return ERROR;
    }
    DEBUG("%s %d",arg->argv[0],ctoi(arg->argv[0]));
    int size=ctoi(arg->argv[0]);
    if(size<MIN||size>MAX){
        strcpy(error.msg,"磁盘容量量错误\n\x00");
        printf("磁盘容量量错误\n");
        return ERROR;
    }
    int cut=size*K*K/SPCSIZE;
    DEBUG("%d %d\n",sizeof(BLOCK),sizeof(BLOCK4K));
    fp=fopen("fs.vhd","wb");
    if(fp==NULL){
        strcpy(error.msg,"文件打开错误\n\x00");
        printf("文件打开量错误\n");
        return ERROR;
    }
    for(int i=0;i<cut;i++){
        do_write_block4k(fp,&block4k,-1);
    }
    //处理vhd格式
    HD_FTR vhd;
    vhdset(&vhd,size*K*K);
    do_write_block(fp,(BLOCK*)&vhd,-1,0);
    
    MBR mbr;
    MBRset(&mbr,size*K*K);
    do_write_block(fp,(BLOCK*)&mbr,0,0);

    BS_BPB bs_pbp;
    BS_BPSset(&bs_pbp,
        size*K*K-mbr.mbr_in[0].strart_chan*BLOCKSIZE,
        mbr.mbr_in[0].strart_chan);
    do_write_block(fp,(BLOCK*)&bs_pbp,mbr.mbr_in[0].strart_chan*BLOCKSIZE/SPCSIZE,(mbr.mbr_in[0].strart_chan*BLOCKSIZE%SPCSIZE)/BLOCKSIZE);
    
    fclose(fp);
    DEBUG("磁盘申请成功\n");
    // if()
}