//
// Created by ozgaga on 2022/11/2.
//

#ifndef INFRARED_NUC_C_FIXED_INITPARAM_H
#define INFRARED_NUC_C_FIXED_INITPARAM_H

#include "utils.h"
#include "../mat/mat.h"

#define dr 14

#define ShShift 13
#define Sh4Shift 13

#define fpShift 11
//#define fpShift 16

#define nuShift 4

#define nucShift 4

void InitGlobalVar(int H, int W, char *fp_path);

void InitIntensity(int FrameRate, int NUIntensity, int ShadingIntensity, int NoiseIntensity,
                   struct SSN_param *ssnParam);

void InitDeShading(int H, int W);

void InitDeFPN(mat_s64 *FP_S64);

void InitDeNoise(int H, int W);

void InitDeNU(int H, int W);

#endif //INFRARED_NUC_C_FIXED_INITPARAM_H
