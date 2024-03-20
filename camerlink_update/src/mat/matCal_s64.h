//
// Created by ozgaga on 2022/11/2.
//

#ifndef INFRARED_NUC_C_MATCAL_S64_H
#define INFRARED_NUC_C_MATCAL_S64_H

void matGaussianBlur_s64(const mat_s32 *src, mat_s32 *dst, int kSize);

void matConvert_f64_s64(const mat_f64 *src, mat_s64 *dst);

void matConvert_u16_s64(const mat_u16 *src, mat_s64 *dst);

void matMeanBlur_201_s64(const mat_s64 *src, mat_s64 *dst);

void matGetRect_s64(const mat_s64 *src, mat_s64 *dst, int s_x, int s_y);

void matSub_s64(const mat_s64 *src1, const mat_s64 *src2, mat_s64 *dst);

void matSubS_s64(const mat_s64 *src, int64_t val, mat_s64 *dst);

int64_t matAvg_s64(const mat_s64 *src);

int64_t matSum_s64(const mat_s64 *src);

void matPow2_s64(const mat_s64 *src, mat_s64 *dst);

#endif //INFRARED_NUC_C_MATCAL_S64_H
