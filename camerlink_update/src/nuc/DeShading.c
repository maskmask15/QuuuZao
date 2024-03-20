//
// Created by ozgaga on 2022/11/3.
//

#include <stdio.h>
#include "InitParam.h"
#include "../mat/matCal_s32.h"
#include "../mat/mat.h"
#include "utils.h"

extern struct GlobalVar globalVar;
extern struct GlobalNUC globalNuc;
extern struct pthread_nuc pthread_DeShading;

static mat_s32 *resShading, *resShading4;
static int64_t sh_sh, sh_sh4, sh4_sh4;

// 濞夈劍鍓扮敮鎼佸櫤闂囷拷鐟曚礁鏋惌鈺呮█缁夎缍呮稉锟介懛锟�
#define varShift 10
//#define varShift 20
#define matShift 20
#define scaleTimes 10
#define scaleTimes2 100

void InitDeShading(int H, int W) {
    //sh_sh閿涳拷0.7606804296 * (1 << varShift) = 779
    //sh_sh4閿涳拷0.5223224804 * (1 << varShift) = 535
    //sh4_sh4閿涳拷0.4147914436 * (1 << varShift) = 425
    sh_sh = 779;
    sh_sh4 = 535;
    sh4_sh4 = 425;

    resShading = createMat_s32(H - 100, W - 101);
//    FILE *uniform_resShading = fopen("../raw_files/resShading.raw", "rb");
//    if (uniform_resShading == NULL) {
//        printf("Open file failure: %s\n", "resShading.raw");
//        return;
//    }
//    fread(resShading->data, 4, resShading->rows * resShading->cols, uniform_resShading);

    Read("resShading.raw",resShading->data,resShading->rows * resShading->cols * 4, 0);

    //fclose(uniform_resShading);

    resShading4 = createMat_s32(H - 100, W - 101);
//    FILE *uniform_resShading4 = fopen("../raw_files/resShading4.raw", "rb");
//    if (uniform_resShading4 == NULL) {
//        printf("Open file failure: %s\n", "resShading4.raw");
//        return;
//    }
//    fread(resShading4->data, 4, resShading4->rows * resShading4->cols, uniform_resShading4);

    Read("resShading4.raw",resShading4->data,resShading4->rows * resShading4->cols * 4, 0);

    //fclose(uniform_resShading4);
}

void *DeShading(void *value) {
    int scale = (int) *((int *) value);
    int H = globalNuc.ORG->rows;
    int W = globalNuc.ORG->cols;

    mat_s32 *resORG = createMat_s32(H - 100, W - 101);
    matFilter_Rect50_s32(globalNuc.ORG, resORG);

    mat_s32 *tmpSh = createMat_s32(H - 100, W - 101);

    matMul_s32(resShading4, resORG, tmpSh);
    int64_t sum_resShading4_resY = (matSum_s32_re64(tmpSh)) >> (matShift - varShift);
    matMul_s32(resShading, resORG, tmpSh);
    int64_t sum_resShading_resY = (matSum_s32_re64(tmpSh)) >> (matShift - varShift);

    int beta = (int) (((sh_sh * sum_resShading4_resY - sum_resShading_resY * sh_sh4)) *
                      scale / (sh_sh * sh4_sh4 - sh_sh4 * sh_sh4));


    int alpha = (int) (((sum_resShading_resY) * scaleTimes - beta * sh_sh4) * scale / sh_sh);


    globalVar.alphaShading = ((globalVar.alphaShading << 3) * scaleTimes2 + (alpha << 1)) / (10 * scaleTimes2);
    globalVar.alphaShading4 = ((globalVar.alphaShading4 << 3) * scaleTimes + (beta << 1)) / (10 * scaleTimes);



    matRelease_s32(&tmpSh);
    matRelease_s32(&resORG);
    //pthread_DeShading.running = false;

    return NULL;
}
