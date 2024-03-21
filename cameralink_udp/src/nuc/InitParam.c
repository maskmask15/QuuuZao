#include <stdlib.h>
#include <math.h>
#include "InitParam.h"
#include "../mat/matCal_s32.h"
#include "../mat/matCal_s64.h"
#include <stdio.h>

struct GlobalNUC globalNuc;
struct GlobalVar globalVar;

// Shading_S64闁猴拷閹佷海濞存粣鎷�2^ShShift
// Shading4_S64闁猴拷閹佷海濞存粣鎷�2^Sh4Shift
// FP_S64闁猴拷閹佷海濞存粣鎷�2^dr
void preProcessShading(mat_s64 *Shading_S64, mat_s64 *Shading4_S64, mat_s64 *FP_S64) {
    int H = FP_S64->rows;
    int W = FP_S64->cols;

    // 濞戞挸顦柌婊堝冀閸ヮ剙娅ら柡锟介幆褋浜� 2 ^ 10
    static int sumShift = 10;
    int64_t sum_Shading2_2 = 15567050;
    int64_t sum_Shading2_4 = 11981357;
    int64_t sum_Shading4_4 = 10135232;

    // 闁猴拷閹佷海濞存粣鎷� 2^(dr+ShShift)=27
    int64_t sum_Shading4_FP = 0;
    for (int i = 0; i < H * W; i++) {
        sum_Shading4_FP += (Shading4_S64->data[i] * FP_S64->data[i]);
    }

    // 闁猴拷閹佷海濞存粣鎷� 2^(dr+Sh4Shift)=27
    int64_t sum_Shading2_FP = 0;
    for (int i = 0; i < H * W; i++) {
        sum_Shading2_FP += (Shading_S64->data[i] * FP_S64->data[i]);
    }

    // beta闁猴拷閹佷海濞存粣鎷� dr + Sh4Shift
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

    int64_t alpha = tmp_sum / sum_Shading2_2; // 鐎归潻濡囦簺27濞达綇鎷� dr+Sh4Shift

    for (int i = 0; i < H * W; i++) {
        FP_S64->data[i] -= (((Shading_S64->data[i] << (Sh4Shift - ShShift)) * alpha + Shading4_S64->data[i] * beta)
                >> (Sh4Shift + Sh4Shift));
    }

    /***** globalVar.FP閻犙冾儏閿熸枻鎷� *****/
    for (int i = 0; i < H * W; i++) {
        globalVar.FP->data[i] = (int) (FP_S64->data[i] >> (dr - fpShift));
    }
    /***** globalVar.FP閻犙冾儏閿熸枻鎷� *****/

    matRelease_s64(&tmp);
}


// 初始化中根据Intensity进行选择，避免后续循环中重复选择
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

