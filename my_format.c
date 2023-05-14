#include "fs.h"
#include "tool.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
/* 存在随机根目录dir乱码 原因不明 */
/*
    接受一个参数 不得小于256MB 不得大于2097152MB(2TB)
    默认一个磁盘块512B 一簇4KB 8个块
*/
#define MIN 256
#define MAX 2097152
// 单位字节
void vhdset(HD_FTRp vhd, u64 size)
{
    strcpy(vhd->cookie, "conectix");
    vhd->features = BigtoLittle32(0x00000002);
    vhd->ff_version = BigtoLittle32(0x00010000);
    vhd->data_offset = BigtoLittle64(0xFFFFFFFFFFFFFFFF);
    vhd->timestamp = 0x00000000;
    strcpy(vhd->crtr_app, "myfs");
    vhd->crtr_ver = BigtoLittle32(0x00060001);
    // strcpy((char*)vhd->crtr_os,"Wi2k");
    vhd->crtr_os = BigtoLittle32(0x5769326b);
    vhd->orig_size = BigtoLittle64(size);
    vhd->curr_size = BigtoLittle64(size);
    vhd->geometry = BigtoLittle32(0x03eb0c11);
    vhd->type = BigtoLittle32(2); // 类型
    vhd->checksum = 0;
    // strcmp(vhd->uuid,"                ");
    memset(vhd->uuid, 0xff, sizeof(vhd->uuid));
    memset(vhd->reserved, 0, sizeof(vhd->reserved));
    u32 chksum = 0;
    u8* p = (u8*)vhd;
    for (int i = 0; i < 512; i++) {
        chksum += p[i];
    }
    vhd->checksum = BigtoLittle32(~chksum);
}
// size为字节数
void MBRset(MBRp mbr, int size)
{
    mbr->sign = 0x12345678;
    (mbr->mbr_in[0]).flag = 0;
    (mbr->mbr_in[0]).FSflag = 0x0b;
    mbr->mbr_in[0].strart_chan = 0x00000008;
    mbr->mbr_in[0].all = size / BLOCKSIZE - mbr->mbr_in[0].strart_chan;
    mbr->end = 0xAA55;
}
void FSInfoset(FSInfop fsi)
{
    memset(fsi, 0, BLOCKSIZE);
    fsi->FSI_LeadSig = 0x41615252;
    fsi->FSI_StrucSig = 0x61417272;
    fsi->FSI_Free_Count = 0xff;
    fsi->FSI_Nxt_free = 0xFFFFFFFF;
    fsi->end = 0xAA550000;
}

// size 为字节数
void BS_BPSset(BS_BPBp bs_bpb, int size, int hiden)
{
    my_strcpy(bs_bpb->BS_jmpBoot, "\xEB\x58\x90", sizeof(bs_bpb->BS_jmpBoot));
    my_strcpy(bs_bpb->BS_OEMName, "MSDOS5.0", sizeof(bs_bpb->BS_OEMName));
    bs_bpb->BPB_BytsPerSec = 512;
    bs_bpb->BPB_SecPerClus = 8; //
    bs_bpb->BPB_RsvdSecCnt = 32;
    bs_bpb->BPB_NumFATs = 2;
    bs_bpb->BPB_RootEntCnt = 0;
    bs_bpb->BPB_TotSec16 = 0;
    bs_bpb->BPB_Media = 0xf8;
    bs_bpb->BPB_FATSz16 = 0;
    bs_bpb->BPB_SecPerTrk = 0x3f;
    bs_bpb->BPB_NumHeads = 0xff;
    bs_bpb->BPB_HiddSec = hiden;
    bs_bpb->BPB_TotSec32 = size / BLOCKSIZE;

    int x = size / SPCSIZE / (512 / 4);
    bs_bpb->BPB_FATSz32 = x + (8 - x % 8); // 强制4k对齐
    bs_bpb->BPB_ExtFlags = 0;
    bs_bpb->BPB_FSVer = 0;
    bs_bpb->BPB_RootClis = 2;
    bs_bpb->BPB_FSInfo = 1;
    bs_bpb->BPB_BkBootSec = 6;
    memset(bs_bpb->BPB_Reserved, 0, sizeof(bs_bpb->BPB_Reserved));
    bs_bpb->BS_DrvNum = 0x80;
    bs_bpb->BS_Reserved1 = 0;
    bs_bpb->BS_BootSig = 0x29;
    // char BS_VolID[4];
    my_strcpy(bs_bpb->BS_VolLab, "NO NAME    ", 11);
    my_strcpy(bs_bpb->BS_FilSysType, "FAT32    ", 8);
    bs_bpb->end = 0xAA55;
}

int my_format(const ARGP arg)
{
    char fatName[8] = "WLt    ";
    char fileName[ARGLEN] = "fs.vhd";
    const char helpstr[] = "\
功能         格式化文件系统\n\
语法格式     format size [name [namefile]]\n\
szie        磁盘大小 单位MB\n\
name        卷标名  默认 WTL\n\
namefile    虚拟磁盘文件路径（当前目录下开始） 默认 fs.vhd\n";
    DEBUG("%d", sizeof(BS_BPB));
    BLOCK4K block4k;
    BLOCK block;
    FILE* fp = NULL;
    switch (arg->len) {
    case 3:
        my_strcpy(fileName, arg->argv[2], ARGLEN);
    case 2:
        my_strcpy(fatName, arg->argv[1], 8);
    case 1:
        if (strcmp(arg->argv[0], "/?") == 0 && arg->len == 1) {
            printf(helpstr);
            return SUCCESS;
        } else {
            break;
        }
        break;
    default:
        strcpy(error.msg, "参数数量错误\n\x00");
        printf("参数数量错误\n");
        return ERROR;
    }
    // if(arg->len>3){
    //     strcpy(error.msg,"参数数量错误\n\x00");
    //     printf("参数数量错误\n");
    //     return ERROR;
    // }

    DEBUG("%s %d", arg->argv[0], ctoi(arg->argv[0]));
    int size = ctoi(arg->argv[0]);
    if (size < MIN || size > MAX) {
        strcpy(error.msg, "磁盘容量量错误\n\x00");
        printf("磁盘容量量错误\n");
        return ERROR;
    }
    int cut = size * K * K / SPCSIZE;
    DEBUG("%d %d\n", sizeof(BLOCK), sizeof(BLOCK4K));
    fp = fopen(fileName, "wb");
    if (fp == NULL) {
        strcpy(error.msg, "文件打开错误\n\x00");
        printf("文件打开量错误\n");
        return ERROR;
    }

    int offset = 0;
    int num = 0;
    // 生成文件本体
    memset(&block4k, 0, SPCSIZE);
    memset(&block, 0, BLOCKSIZE);
    for (int i = 0; i < cut; i++) {
        do_write_block4k(fp, &block4k, -1);
    }
    do_write_block(fp, (BLOCK*)&block, -1, 0); // vhd 附加块
    DEBUG("4K块数量 %d\n", cut);
    fclose(fp);
    fp = fopen(fileName, "rb+");
    // 处理vhd格式
    HD_FTR vhd;
    do_read_block(fp, (BLOCK*)&vhd, cut, 0);
    vhdset(&vhd, size * K * K);
    do_write_block(fp, (BLOCK*)&vhd, cut, 0);

    // 磁盘分区
    MBR mbr;
    do_read_block(fp, (BLOCK*)&mbr, 0, 0);
    MBRset(&mbr, size * K * K);
    do_write_block(fp, (BLOCK*)&mbr, 0, 0);

    // fatBPB
    offset = mbr.mbr_in[0].strart_chan * BLOCKSIZE / SPCSIZE;
    num = (mbr.mbr_in[0].strart_chan * BLOCKSIZE % SPCSIZE) / BLOCKSIZE;
    BS_BPB bs_pbp;
    do_read_block(fp, (BLOCK*)&bs_pbp, offset, num);
    BS_BPSset(&bs_pbp,
        size * K * K - mbr.mbr_in[0].strart_chan * BLOCKSIZE,
        mbr.mbr_in[0].strart_chan);
    do_write_block(fp, (BLOCK*)&bs_pbp, offset, num);

    // 初始化根目录
    int start = bs_pbp.BPB_FATSz32 * bs_pbp.BPB_NumFATs + bs_pbp.BPB_RsvdSecCnt + mbr.mbr_in[0].strart_chan + (bs_pbp.BPB_RootClis - 2) * bs_pbp.BPB_SecPerClus;
    DEBUG("开始扇区 %d\n", start);

    offset = start / 8;
    num = start % 8;
    FAT_DSp fatdsp = (FAT_DSp)&block;
    do_read_block(fp, &block, offset, num);
    memset(fatdsp, 0, sizeof(FAT_DS));
    my_strcpy(fatdsp->name, fatName, 8);
    my_strcpy(fatdsp->named, "   ", 3);
    fatdsp->DIR_Attr = ATTR_VOLUME_ID;
    do_write_block(fp, &block, offset, num);

    //         for (int i = 0; i < cut; i++) {
    //     for (int j = 0; j < 8; j++) {
    //         int readlean = do_read_block(fp, &block, i, j);
    //         printf("read len %d\n",readlean);
    //         char* chr = &block;
    //         for (int k = 0; k < SPCSIZE; k++) {
    //             if ((*chr)) {
    //                 printf("%02X\n \"%c\"", i * SPCSIZE + j * BLOCKSIZE,*chr);
    //             }
    //             // assert(!(*chr));
    //             chr++;
    //         }
    //     }
    // }

    FSInfo fsi;
    offset = mbr.mbr_in[0].strart_chan * BLOCKSIZE / SPCSIZE;
    num = (mbr.mbr_in[0].strart_chan * BLOCKSIZE % SPCSIZE) / BLOCKSIZE + 1;
    do_read_block(fp, (BLOCK*)&fsi, offset, num);
    FSInfoset(&fsi);
    // do_write_block(fp,(BLOCK*)&bs_pbp,mbr.mbr_in[0].strart_chan*BLOCKSIZE/SPCSIZE,(mbr.mbr_in[0].strart_chan*BLOCKSIZE%SPCSIZE)/BLOCKSIZE);
    do_write_block(fp, (BLOCK*)&fsi, offset, num);

    // fat设置 (根目录初始化)

    const int str_fat1 = bs_pbp.BPB_RsvdSecCnt + mbr.mbr_in[0].strart_chan;
    const int str_fat2 = bs_pbp.BPB_RsvdSecCnt + bs_pbp.BPB_FATSz32 + mbr.mbr_in[0].strart_chan;
    const int str_fat_array[] = { str_fat1, str_fat2 };
    DEBUG("%d %d\n", str_fat1, str_fat2);

    for (int k = 0; k < 2; k++) {
        FAT fat;
        char* chr = &fat;
        chr[0] = 1;
        offset = str_fat_array[k] / 8;
        num = str_fat_array[k] % 8;
        do_read_block(fp, (BLOCK*)&fat, offset, num);
        // memset(&fat, 0, BLOCKSIZE);
        for (int i = 0; i < 512; i++) {
            // printf("%x %02x %02x\n", &fat, i, *chr);
            assert(!(*chr));
            chr++;
        }
        fat.fat[0] = 0x0ffffff8;
        for (u32 i = 1; i <= bs_pbp.BPB_RootClis; i++) {
            fat.fat[i] = FAT_END;
        }
        // fat.fat[bs_pbp.BPB_RootEntCnt]=
        do_write_block(fp, (BLOCK*)&fat, offset, num);
    }

    fclose(fp);
    DEBUG("磁盘申请成功\n");
    // if()
    return SUCCESS;
}
