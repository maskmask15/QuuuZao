//
// Created by ozgaga on 2022/11/2.
//

#ifndef INFRARED_NUC_C_FIXED_IRNUC_H
#define INFRARED_NUC_C_FIXED_IRNUC_H

#include "utils.h"

void SSN(mat_s32 *NUC_s32, struct SSN_param *ssnParam, mat_s32 *weight_s32);

void DeNoise(mat_s32 *NUC, int NoiseSize, int cnt_mod1);

#endif //INFRARED_NUC_C_FIXED_IRNUC_H
