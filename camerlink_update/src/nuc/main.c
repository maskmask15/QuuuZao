#include "main.h"
#include "shareOpConfig.h"
#include "file_op.h"
//#include "../xdm/dma_DeNU_2.h"
#include "../xdm/dma.h"
#include "../platform.h"

#define IMG_PATH "fp.raw"
#define VEDIO_PATH "2nn.raw"
#define OUTPUT_PATH ""

int main() {
	//��ʼ��
	//init_platform();

    int src_H = 512;
    int src_W = 640;

    int dst_H = 511;
    int dst_W = 640;

    char file_fp_path[] = IMG_PATH;

    char file_video_path[] = VEDIO_PATH;

    char file_txt_path[] = OUTPUT_PATH;

    int FrameRate = 25;
    int NUIntensity = 2;
    int ShadingIntensity = 1;
    int NoiseIntensity = 1;

    printf("cpu0 start Inframe Update Parameter! 1hdaehd\n");
    //��ʼ����ַ
    //init_share_addr();
    Xil_DCacheFlushRange((UINTPTR)  NUC_BASE, 640 * 512 * 4);
//    printf("init_share_addr done\n");
    //��ʼ���ļ�
    Init();
    printf("Init done\n");
    //��ʼ��DMA
    DMA_Init(DeNU2_DMA_DEV_ID);
    printf("DeNU2_DMA_DEV_ID done\n");


    DMA_Init(DeStrip_DMA_DEV_ID);
    printf("DeStrip_DMA_DEV_ID done\n");
    //��ʼ����־λ
    init_flag();
    printf("init_flag done\n");
    //����cpu1
    //StartCpu1();

    //printf("StartCpu1 done\n");
    infrared_NUC(src_H, src_W, dst_H, dst_W, file_fp_path, file_video_path, file_txt_path,
                 FrameRate, NUIntensity, ShadingIntensity, NoiseIntensity);

    return 0;
}