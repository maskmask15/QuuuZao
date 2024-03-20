//
// Created by ozgaga on 2022/10/3.
//

#ifndef INFRARED_NUC_C_MATCAL_S32_H
#define INFRARED_NUC_C_MATCAL_S32_H

#include "mat.h"

void matConvert_f32_s32(const mat_f32 *src, mat_s32 *dst);

void matGetRect_s32(const mat_s32 *src, mat_s32 *dst, int s_x, int s_y);

void matSub_s32(const mat_s32 *src1, const mat_s32 *src2, mat_s32 *dst);

void matAddS_s32(const mat_s32 *src, int val, mat_s32 *dst);

void matSubS_s32(const mat_s32 *src, int val, mat_s32 *dst);

void matMaxS_s32(const mat_s32 *src, int max_val, mat_s32 *dst);

int matAvg_s32(const mat_s32 *src);

void matSetZero_s32(mat_s32 *src);

void matAdd_s32(const mat_s32 *src1, const mat_s32 *src2, mat_s32 *dst);

void matAbs_s32(const mat_s32 *src, mat_s32 *dst);

void matPow2_s32(const mat_s32 *src, mat_s32 *dst);

void matCopy_s32(const mat_s32 *src, mat_s32 *dst);

void matMaxZero_s32(const mat_s32 *src, mat_s32 *dst);

void matDiv_s32(const mat_s32 *src1, const mat_s32 *src2, mat_s32 *dst);

void matMul_s32(const mat_s32 *src1, const mat_s32 *src2, mat_s32 *dst);

uint16_t matStd_s32(mat_s32 *src);

//float matStd_s32(mat_s32 *src);

void matMeanBlur_201_s32(const mat_s32 *src, mat_s32 *dst);

void matGetMedRow_s32(mat_s32 *src_, mat_s32 *dst_);

void matGetMedCol_s32(const mat_s32 *src, mat_s32 *dst);

void matGaussianBlur_s32(const mat_s32 *src, mat_s32 *dst, int kSize);

int matHist_s32(const mat_s32 *src, const int *range, int histSize);

void matSort_s32(const mat_s32 *src, mat_s32 *dst);

void matRectConvert_u16_s32(const mat_u16 *src, mat_s32 *dst, int s_x, int s_y);

void matFilter_Rect50_s32(const mat_s32 *src, mat_s32 *dst);

int matSum_s32(const mat_s32 *src);

int64_t matSum_s32_re64(const mat_s32 *src);

#endif //INFRARED_NUC_C_MATCAL_S32_H
