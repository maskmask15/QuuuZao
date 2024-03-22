#include <stdlib.h>
#include <math.h>
#include "InitParam.h"
#include "../mat/matCal_s32.h"
#include "../mat/matCal_s64.h"
#include <stdio.h>


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

