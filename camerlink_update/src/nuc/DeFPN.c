//
// Created by ozgaga on 2022/11/5.
//

#include <math.h>
#include "InitParam.h"
#include "../mat/matCal_s32.h"
#include "../mat/matCal_s64.h"

#include <stdio.h>

extern struct GlobalNUC globalNuc;
extern struct GlobalVar globalVar;
extern struct pthread_nuc pthread_DeFPN;

static mat_s32 *FP_DeTrnd;   // 绉讳綅鏀惧ぇFP_DeTrnd_Shift鍊�
static int64_t SumFPDeTrnd;

#define fpSumShift 4

void InitDeFPN(mat_s64 *FP_S64) {
    int H = FP_S64->rows, W = FP_S64->cols;

    mat_s64 *FP_BLUR = createMat_s64(H - 200, W - 200);
    matMeanBlur_201_s64(FP_S64, FP_BLUR);

    mat_s64 *FP_DeTrnd_S64 = createMat_s64(H - 200, W - 200);
    matGetRect_s64(FP_S64, FP_DeTrnd_S64, 100, 100);

    matSub_s64(FP_DeTrnd_S64, FP_BLUR, FP_DeTrnd_S64);
    matSubS_s64(FP_DeTrnd_S64, matAvg_s64(FP_DeTrnd_S64), FP_DeTrnd_S64);

    /******** Init Var *********/
    FP_DeTrnd = createMat_s32(H - 200, W - 200);
    for (int i = 0; i < FP_DeTrnd_S64->rows * FP_DeTrnd_S64->cols; i++) {
        FP_DeTrnd->data[i] = (int) (FP_DeTrnd_S64->data[i] >> (dr - fpShift));
    }

    matPow2_s64(FP_DeTrnd_S64, FP_DeTrnd_S64);
    SumFPDeTrnd = matSum_s64(FP_DeTrnd_S64) >> (dr + dr - fpSumShift);
    /******** Init Var *********/

    matRelease_s64(&FP_DeTrnd_S64);
    matRelease_s64(&FP_BLUR);
}


void *DeFPN_s32() {
    /**
     @param:Y   鍘熷鍥惧儚
     @param:FP  鍧囧寑闈㈠浘鍍�
     @param:alphaFP  闇�瑕佹洿鏂扮殑鍙傛暟
     **/
    static int k = 0;
    int H = globalNuc.NUC_DeShading->rows;
    int W = globalNuc.NUC_DeShading->cols;

    mat_s32 *Y_DeTrnd = createMat_s32(H - 200, W - 200);
    matGetRect_s32(globalNuc.NUC_DeShading, Y_DeTrnd, 100, 100);

    mat_s32 *Y_Blur = createMat_s32(H - 200, W - 200);
    matMeanBlur_201_s32(globalNuc.NUC_DeShading, Y_Blur);

    matSub_s32(Y_DeTrnd, Y_Blur, Y_DeTrnd);
    // 姝ゅ姹傚潎鍊兼帴杩�0锛屽彲蹇界暐
//    matSubS_s32(Y_DeTrnd, matAvg_s32(Y_DeTrnd), Y_DeTrnd);

    matRelease_s32(&Y_Blur);

    // 绠椾箻娉曠殑鍚屾椂姹傚拰锛�
    int64_t sum_Y_FP_DeTrnd = 0;
    for (int i = 0; i < H - 200; i++) {
        for (int j = 0; j < W - 200; j++) {
            sum_Y_FP_DeTrnd += (Y_DeTrnd->data[i * (W - 200) + j] * FP_DeTrnd->data[i * (W - 200) + j]);
//            sum_Y_FP_DeTrnd += ((int64_t) Y_DeTrnd->data[i * (W - 200) + j] * (int64_t) FP_DeTrnd->data[i * (W - 200) + j]);
        }
    }
    matRelease_s32(&Y_DeTrnd);
//    printf("sum_Y_FP_DeTrnd: %f\n", sum_Y_FP_DeTrnd);

    if (k == 0) {
        globalVar.alphaFP = (int) ((sum_Y_FP_DeTrnd / SumFPDeTrnd) >> (fpShift - fpSumShift));
        k = 1;
    } else {
        globalVar.alphaFP = (8 * globalVar.alphaFP +
                             2 * (int) ((sum_Y_FP_DeTrnd / SumFPDeTrnd) >> (fpShift - fpSumShift))) / 10;
    }
//    printf("globalVar.alphaFP: %d\n", globalVar.alphaFP);

    //pthread_DeFPN.running = false;

    return NULL;
}

