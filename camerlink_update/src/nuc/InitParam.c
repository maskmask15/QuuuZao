#include <stdlib.h>
#include <math.h>
#include "InitParam.h"
#include "../mat/matCal_s32.h"
#include "../mat/matCal_s64.h"
#include "file_op.h"
#include <stdio.h>

struct GlobalNUC globalNuc;
struct GlobalVar globalVar;

// Shading_S64閺�鎯с亣娴滐拷2^ShShift
// Shading4_S64閺�鎯с亣娴滐拷2^Sh4Shift
// FP_S64閺�鎯с亣娴滐拷2^dr

void preProcessShading(mat_s64 *Shading_S64, mat_s64 *Shading4_S64, mat_s64 *FP_S64) {
    int H = FP_S64->rows;
    int W = FP_S64->cols;

    // 娑撳閲滈弽鍥櫤閺�鎯с亣 2 ^ 10
    static int sumShift = 10;
    int64_t sum_Shading2_2 = 15567050;
    int64_t sum_Shading2_4 = 11981357;
    int64_t sum_Shading4_4 = 10135232;

    // 閺�鎯с亣娴滐拷 2^(dr+ShShift)=27
    int64_t sum_Shading4_FP = 0;
    for (int i = 0; i < H * W; i++) {
        sum_Shading4_FP += (Shading4_S64->data[i] * FP_S64->data[i]);
    }

    // 閺�鎯с亣娴滐拷 2^(dr+Sh4Shift)=27
    int64_t sum_Shading2_FP = 0;
    for (int i = 0; i < H * W; i++) {
        sum_Shading2_FP += (Shading_S64->data[i] * FP_S64->data[i]);
    }

    // beta閺�鎯с亣娴滐拷 dr + Sh4Shift
    int64_t beta = ((sum_Shading2_2 * sum_Shading4_FP << (Sh4Shift - ShShift)) - sum_Shading2_FP * sum_Shading2_4) /
                   (((sum_Shading2_2 * sum_Shading4_4) >> sumShift) - ((sum_Shading2_4 * sum_Shading2_4) >> sumShift));

    mat_s64 *tmp = createMat_s64(H, W);
    for (int i = 0; i < H * W; i++) {
        tmp->data[i] = ((FP_S64->data[i] << (Sh4Shift + Sh4Shift)) - Shading4_S64->data[i] * beta) >> Sh4Shift;
    }
    int64_t tmp_sum = 0;
    for (int i = 0; i < H * W; i++) {
        tmp_sum += (tmp->data[i] * Shading_S64->data[i]) >> (Sh4Shift - sumShift);
    }

    int64_t alpha = tmp_sum / sum_Shading2_2; // 瀹革妇些27娴ｏ拷 dr+Sh4Shift

    for (int i = 0; i < H * W; i++) {
        FP_S64->data[i] -= (((Shading_S64->data[i] << (Sh4Shift - ShShift)) * alpha + Shading4_S64->data[i] * beta)
                >> (Sh4Shift + Sh4Shift));
    }

    /***** globalVar.FP鐠у锟斤拷 *****/
    for (int i = 0; i < H * W; i++) {
        globalVar.FP->data[i] = (int) (FP_S64->data[i] >> (dr - fpShift));
    }
    /***** globalVar.FP鐠у锟斤拷 *****/

    matRelease_s64(&tmp);
}


void InitGlobalVar(int H, int W, char *fp_path) {
    /***************************** READ-FP ********************************/
    mat_u16 *pData = createMat_u16(H, W);

    Read(fp_path,pData->data,H * W * 2, 0);

    mat_s64 *FP_S64 = createMat_s64(H, W);
    for (int i = 0; i < FP_S64->rows * FP_S64->cols; i++) {
        FP_S64->data[i] = (int64_t) pData->data[i];
    }
    matRelease_u16(&pData);
    /***************************** READ-FP ********************************/

    /****** 瑜版帊绔撮崠锟� 閺�鎯с亣2^14閸婏拷 ******/
    int64_t max_v = FP_S64->data[0];
    int64_t min_v = FP_S64->data[0];
    for (int i = 1; i < FP_S64->rows * FP_S64->cols; i++) {
        if (FP_S64->data[i] > max_v) max_v = FP_S64->data[i];
        if (FP_S64->data[i] < min_v) min_v = FP_S64->data[i];
    }
    for (int i = 0; i < FP_S64->rows * FP_S64->cols; i++) {
        FP_S64->data[i] = ((FP_S64->data[i] - min_v) << dr) / (max_v - min_v);
    }
    /****** 瑜版帊绔撮崠锟� 閺�鎯с亣2^14閸婏拷 ******/

    int64_t fp_avg = matAvg_s64(FP_S64);
    matSubS_s64(FP_S64, fp_avg, FP_S64);

    globalNuc.ORG = createMat_s32(H, W);
    globalNuc.NUC_DeFPN = createMat_s32(H, W);
    globalNuc.NUC_DeShading = createMat_s32(H, W);
    globalNuc.NUC_DeStrip = createMat_s32(H, W);

    globalVar.alphaFP = 0, globalVar.alphaShading = 0, globalVar.alphaShading4 = 0;
    globalVar.D = 1000;
    globalVar.FP = createMat_s32(H, W);
    globalVar.NU = createMat_s32(H, W);       // 鐠佹澘缍嶆导鎷岊吀瀵版鍩岄惃鍕姜閸у洤瀵�
    globalVar.Strip2 = createMat_s32(H, W);   // 鐠佹澘缍嶆导鎷岊吀瀵版鍩岄惃鍕攽閸掓娅旀竟锟�
    matSetZero_s32(globalVar.NU);
    matSetZero_s32(globalVar.Strip2);

    globalVar.Shading = createMat_s32(H, W);
    globalVar.Shading4 = createMat_s32(H, W);

    Read("Shading.raw",globalVar.Shading->data,4*H*W,0);

    Read("Shading4.raw",globalVar.Shading4->data,4*H*W,0);

    mat_s64 *Shading_S64 = createMat_s64(H, W);
    mat_s64 *Shading4_S64 = createMat_s64(H, W);
    for (int i = 0; i < H * W; i++) {
        Shading_S64->data[i] = globalVar.Shading->data[i];
        Shading4_S64->data[i] = globalVar.Shading4->data[i];
    }

    preProcessShading(Shading_S64, Shading4_S64, FP_S64);  //婢跺嫮鎮奼lobalVal.FP
    InitDeShading(H, W);
    InitDeFPN(FP_S64);
    InitDeNU(H, W);
    InitDeNoise(H, W);

    matRelease_s64(&FP_S64);
    matRelease_s64(&Shading_S64);
    matRelease_s64(&Shading4_S64);
}

// 閸掓繂顫愰崠鏍﹁厬閺嶈宓両ntensity鏉╂稖顢戦柅澶嬪閿涘矂浼╅崗宥呮倵缂侇厼鎯婇悳顖欒厬闁插秴顦查柅澶嬪
void InitIntensity(int FrameRate, int NUIntensity, int ShadingIntensity, int NoiseIntensity,
                   struct SSN_param *ssnParam) {
    if (FrameRate < 25) FrameRate = 25;
    ssnParam->FR_half = (FrameRate >> 1);
    ssnParam->FR_fifth = (int) (FrameRate * 0.2);

    switch (NUIntensity) {
        case 1:
            ssnParam->NU_kSize = 9;
            ssnParam->Strip_kSize = 7;
            break;
        case 2:
            ssnParam->NU_kSize = 17;
            ssnParam->Strip_kSize = 11;
            break;
        case 3:
            ssnParam->NU_kSize = 25;
            ssnParam->Strip_kSize = 15;
            break;
        default:
            ssnParam->NU_kSize = 1;
            ssnParam->Strip_kSize = 0;
            break;
    }
    switch (ShadingIntensity) {
        case 1:
            ssnParam->Shading_Scale = 4;
            break;
        case 2:
            ssnParam->Shading_Scale = 8;
            break;
        case 3:
            ssnParam->Shading_Scale = 10;
            break;
        default:
            ssnParam->Shading_Scale = 0;
            break;
    }
    switch (NoiseIntensity) {
        case 0:
            ssnParam->Noise_kSize = 0;
            break;
        case 1:
            ssnParam->Noise_kSize = 5;
            break;
        case 2:
            ssnParam->Noise_kSize = 9;
            break;
        case 3:
            ssnParam->Noise_kSize = 13;
            break;
        default:
            ssnParam->Noise_kSize = 3;
            break;
    }
}

