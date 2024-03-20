//
// Created by ozgaga on 2022/10/1.
//

#include "mat.h"
#include <stdlib.h>
#include "xparameters_ps.h"

mat_f32 *createMat_f32(int rows, int cols) {
    mat_f32 *mat = (mat_f32 *) malloc(sizeof(mat_f32));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (float *) malloc(sizeof(float) * rows * cols);
    return mat;
}

void matRelease_f32(mat_f32 **array) {
    mat_f32 *arr = *array;
    free(arr->data), arr->data = NULL;
    free(arr), arr = NULL;
}

mat_f64 *createMat_f64(int rows, int cols) {
    mat_f64 *mat = (mat_f64 *) malloc(sizeof(mat_f64));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (double *) malloc(sizeof(double) * rows * cols);
    return mat;
}

void matRelease_f64(mat_f64 **array) {
    mat_f64 *arr = *array;
    free(arr->data), arr->data = NULL;
    free(arr), arr = NULL;
}

mat_u16 *createMat_u16(int rows, int cols) {
    mat_u16 *mat = (mat_u16 *) malloc(sizeof(mat_u16));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (uint16_t *) malloc(sizeof(uint16_t) * rows * cols);
    return mat;
}

void matRelease_u16(mat_u16 **array) {
    mat_u16 *arr = *array;
    free(arr->data), arr->data = NULL;
    free(arr), arr = NULL;
}

mat_u16 *createMat_u16_A(int rows, int cols, uint16_t* data) {
    mat_u16 *mat = (mat_u16 *) malloc(sizeof(mat_u16));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = data;
    return mat;
}

void matRelease_u16_A(mat_u16 **array) {
    mat_u16 *arr = *array;
    free(arr);
}

mat_s32 *createMat_s32(int rows, int cols) {
    mat_s32 *mat = (mat_s32 *) malloc(sizeof(mat_s32));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (int32_t *) malloc(sizeof(int32_t) * rows * cols);
    return mat;
}

void matRelease_s32(mat_s32 **array) {
    mat_s32 *arr = *array;
    free(arr->data), arr->data = NULL;
    free(arr), arr = NULL;
}

mat_s32 *createMat_s32_A(int rows, int cols, int32_t *data) {
    mat_s32 *mat = (mat_s32 *) malloc(sizeof(mat_s32));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = data;
    return mat;
}

void matRelease_s32_A(mat_s32 **array) {
    mat_s32 *arr = *array;
    arr->data = NULL;
    free(arr), arr = NULL;
}

mat_s64 *createMat_s64(int rows, int cols) {
    mat_s64 *mat = (mat_s64 *) malloc(sizeof(mat_s64));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (int64_t *) malloc(sizeof(int64_t) * rows * cols);
    return mat;
}

void matRelease_s64(mat_s64 **array) {
    mat_s64 *arr = *array;
    free(arr->data), arr->data = NULL;
    free(arr), arr = NULL;
}

