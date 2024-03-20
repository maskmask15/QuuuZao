//
// Created by ozgaga on 2022/11/2.
//

#ifndef INFRARED_NUC_C_FIXED_UTILS_H
#define INFRARED_NUC_C_FIXED_UTILS_H

#include <stdbool.h>
#include <pthread.h>
#include "../mat/mat.h"
#include "InitParam.h"

struct GlobalVar {
    mat_s32 *FP;
    mat_s32 *NU;
    mat_s32 *Strip2;
    mat_s32 *Shading;
    mat_s32 *Shading4;
    int alphaFP;
    int alphaShading;
    int alphaShading4;
    int D;
};

struct GlobalNUC {
    mat_s32 *ORG;
    mat_s32 *NUC_DeFPN;
    mat_s32 *NUC_DeShading;
    mat_s32 *NUC_DeStrip;
};


//struct pthread_nuc {
//    pthread_t pthread;
//    bool running;
//};

struct SSN_param {
    int FR_half;
    int FR_fifth;
    int NU_kSize;
    int Strip_kSize;
    int Shading_Scale;
    int Noise_kSize;
};

float findKthNums_f32(float *nums, int l, int r, int k);

int findKthNums_s32(int *nums, int l, int r, int k);

void sortNums_s32(int *nums, int l, int r);

void sortNums_f32(float *nums, int l, int r);

void writeRes_TXT(mat_s32 *NUC, int cnt, char *res_path);

void writeRes_RAW(mat_s32 *NUC, int cnt, char *res_path);

#endif //INFRARED_NUC_C_FIXED_UTILS_H
