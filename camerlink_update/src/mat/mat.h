//
// Created by ozgaga on 2022/10/1.
//

#ifndef INFRARED_NUC_C_MAT_H
#define INFRARED_NUC_C_MAT_H

#include <stdint.h>

typedef struct mat_f32 {
    int rows;     /* matrix's row */
    int cols;     /* matrix's col */
    float *data;  /* float32 */
} mat_f32;

typedef struct mat_f64 {
    int rows;
    int cols;
    double *data;  /* double64 */
} mat_f64;

typedef struct mat_u16 {
    int rows;
    int cols;
    uint16_t *data;   /* unsigned short int */
} mat_u16;

typedef struct mat_s32 {
    int rows;
    int cols;
    int32_t *data;
} mat_s32;

typedef struct mat_s64 {
    int rows;
    int cols;
    int64_t *data;
} mat_s64;

mat_f32 *createMat_f32(int rows, int cols);

mat_f64 *createMat_f64(int rows, int cols);

mat_u16 *createMat_u16(int rows, int cols);

mat_s32 *createMat_s32(int rows, int cols);

mat_s64 *createMat_s64(int rows, int cols);

mat_s32 *createMat_s32_A(int rows, int cols, int32_t *data);

mat_u16 *createMat_u16_A(int rows, int cols, uint16_t *data);

void matRelease_f32(mat_f32 **array);

void matRelease_f64(mat_f64 **array);

void matRelease_u16(mat_u16 **array);

void matRelease_s32(mat_s32 **array);

void matRelease_s64(mat_s64 **array);

void matRelease_s32_A(mat_s32 **array);

void matRelease_u16_A(mat_u16 **array);

#endif //INFRARED_NUC_C_MAT_H
