#ifndef __FS__
#define __FS__

#include<stdio.h>

#define ARGNUM      10   //最大参数数量
#define ARGLEN      1024 //单一参数最大长度
#define SUCCESS     0    //成功返回值
#define ERROR       -1   //失败返回值
#define TRUE        1
#define FALSE       0
#define INF         (1<<30)
#define BLOCKSIZE   512
#define SecPerClus  8
#define SPCSIZE     4096
#define K           1024

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20
#define ATTR_LONG_NAME  (ATTR_READ_ONLY|\
                            ATTR_HIDDEN|\
                            ATTR_SYSTEM|\
                            ATTR_VOLUME_ID)
#define FAT_SAVE    0x0ffffff0
#define FAT_END     0x0fffffff
#define FAT_BAD     0x0ffffff7


typedef unsigned int uint;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
#pragma pack(1)
typedef struct __FAT_DS{
    char name[8];
    char named[3];
    u8 DIR_Attr;
    u8 DIR_NTRes;
    u8 DIR_CrtTimeTeenth;
    u16 DIR_CrtTime;
    u16 DIR_CrtDate;
    u16 DIR_LastAccDate;
    u16 DIR_FstClusHI;
    u16 DIR_WriTime;
    u16 DIR_WrtDate;
    u16 DIR_WriteLO;
    u16 DIR_FileSize;
    char recover[2];
}FAT_DS,*FAT_DSp;

typedef struct __MBR_in{
    u8 flag;
    u8 start;
    u16 starts_c;//起始扇区磁头号
    u8 FSflag;//0x0B
    u8 end_c;//结束磁头号
    u16 end_sector;
    u32 strart_chan;
    u32 all;
}MBR_in,*MBR_inp;

typedef struct __MBR{
    char recoverd[512-66-6];
    u32 sign;
    char recoved[2];
    MBR_in mbr_in[4];
    u32 end;
}MBR,*MBRp;

typedef struct __BS_BPB{
    char BS_jmpBoot[3];
    char BS_OEMName[8];
    u16 BPB_BytsPerSec;
    u8 BPB_SecPerClus;
    u16 BPB_RsvdSecCnt;//保留扇区数
    u8 BPB_NumFATs;//FAT数量
    u16 BPB_RootEntCnt;
    u16 BPB_TotSec16;
    u8 BPB_Media;
    u16 BPB_FATSz16;
    u16 BPB_SecPerTrk;
    u16 BPB_NumHeads;
    u32 BPB_HiddSec;
    u32 BPB_TotSec32;

    u32 BPB_FATSz32;//FAT扇区数
    u16 BPB_ExtFlags;
    u16 BPB_FSVer;
    u32 BPB_RootClis;//根目录簇号
    u16 BPB_FSInfo;
    u16 BPB_BkBootSec;
    char BPB_Reserved[12];
    u8 BS_DrvNum;
    u8 BS_Reserved1;
    u8 BS_BootSig;
    char BS_VolID[4];
    char BS_VolLab[11];
    char BS_FilSysType[8];
    char recover[420];
    // MBR mbr[4];
    u16 end;
}BS_BPB,*BS_BPBp;

typedef struct __FSInfo{
    u32 FSI_LeadSig;
    char recoved[480];
    u32 FSI_StrucSig;
    u32 FSI_Free_Count;
    u32 FSI_Nxt_free;
    char FSI_Reserved2[12];
    u32 end;
}FSInfo,*FSInfop;

//512FAT表
typedef struct __FAT{
    u32 fat[BLOCKSIZE/4];
}FAT,FATp;

typedef struct __FAT4K{
    u32 fat[SPCSIZE/4];
}FAT4K,FAT4Kp;

#pragma pack()


#define __DEBUG__
#ifdef __DEBUG__
    #define DEBUG printf
#elif
    #define DEBUG debug
    void debug(char *,...){
        return;
    }
#endif //__DEBUG__

//全局错误结构体
typedef struct __ERROR{
    char msg[ARGLEN];
}ERR;
ERR error;

//块操纵员素
typedef struct __BLOCK{
    char data[512];
}BLOCK;

//4K块
typedef struct __4KBLOCK{
    BLOCK block[8];
}BLOCK4K;


//参数结构体
typedef struct __ARGV{
    int len;   //参数数量
    char argv[ARGNUM][ARGLEN];  //参数数组
}ARG,*ARGP;

//vhd结构体
typedef struct __hd_ftr { 
  char   cookie[8];       /* Identifies original creator of the disk      */ 
  u32    features;        /* Feature Support -- see below                 */ 
  u32    ff_version;      /* (major,minor) version of disk file           */ 
  u64    data_offset;     /* Abs. offset from SOF to next structure       */ 
  u32    timestamp;       /* Creation time.  secs since 1/1/2000GMT       */ 
  char   crtr_app[4];     /* Creator application                          */ 
  u32    crtr_ver;        /* Creator version (major,minor)                */ 
  u32    crtr_os;         /* Creator host OS                              */ 
  u64    orig_size;       /* Size at creation (bytes)                     */ 
  u64    curr_size;       /* Current size of disk (bytes)                 */ 
  u32    geometry;        /* Disk geometry                                */ 
  u32    type;            /* Disk type                                    */ 
  u32    checksum;        /* 1's comp sum of this struct.                 */ 
  char   uuid[16];        /* Unique disk ID, used for naming parents      */ 
  char   saved;           /* one-bit -- is this disk/VM in a saved state? */ 
  char   hidden;          /* tapdisk-specific field: is this vdi hidden?  */ 
  char   reserved[426];   /* padding                                      */ 
}HD_FTR,*HD_FTRp; 


void startsys();    

/*成功返回SUCCESS 失败返回ERROR*/
//命令行可调用
int my_format(const ARGP arg);
int my_cd(const ARGP arg);
int my_mkdir(const ARGP arg);
int my_rmdir(const ARGP arg);
int my_dir(const ARGP arg);
int my_create(const ARGP arg);
int my_rm(const ARGP arg);

int my_open();
int my_close();
int my_write();
int my_read();


#endif //__FS__
