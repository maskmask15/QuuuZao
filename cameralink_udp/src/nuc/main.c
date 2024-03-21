#include "main.h"
#define IMG_PATH "fp.raw"
#define VEDIO_PATH "2nn.raw"
#define OUTPUT_PATH ""

int nuc_init() {
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


    printf("cpu0 start! NUC 12121 \n");

    Init();
    printf("cpu0 start! NUC 12121 init done\n");
    init_infrared_NUC(src_H, src_W, dst_H, dst_W, file_fp_path, file_video_path, file_txt_path,
    		FrameRate, NUIntensity, ShadingIntensity, NoiseIntensity);

    return 0;
}
