//
// Created by ozgaga on 2022/9/30.
//

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mat.h"

void getGaussKer_f32(float *gaussKer, int offset) {
    double sigmaX = offset >> 1;
    double scale2X = -0.5 / (sigmaX * sigmaX);
    double dSum = 0;
    for (int i = 0; i <= offset; i++) {
        gaussKer[i] = (float) (exp(scale2X * i * i));
        dSum += (gaussKer[i] + gaussKer[i]);
    }
    dSum = 1.0 / (dSum - gaussKer[0]);
    for (int i = 0; i <= offset; i++) {
        gaussKer[i] *= (float) dSum;
    }
}

void SymmRowVecBlur_f32(const mat_f32 *src_, float *dst, const float *gaussKer, int offset) {
    int H = src_->rows, W = src_->cols;
    float *row = (float *) malloc(sizeof(float) * (W + offset + offset));

    float *src = src_->data;

    for (int i = 0; i < H; i++, dst += W, src += W) {
        memcpy(row + offset, src, W * sizeof(float));
        for (int j = 0; j < offset; j++) {
            row[j] = src[offset - 1 - j];
            row[W + offset + j] = src[W - 1 - j];
        }
        for (int j = 0; j < W; j++) {
            float s0 = row[j + offset] * gaussKer[0];
            for (int k = 1; k <= offset; k++) {
                s0 += (gaussKer[k] * (row[j + offset - k] + row[j + offset + k]));
            }
            dst[j] = s0;
        }
    }
    free(row);
}

void SymmColVecBlur_f32(float *src, mat_f32 *dst_, const float *gaussKer, int offset) {
    int H = dst_->rows, W = dst_->cols;
    float *dst = dst_->data;

    float **cols = (float **) malloc(sizeof(float *) * (H + offset + offset));
    for (int i = 0; i < offset; i++) {
        cols[i] = src + W * (offset - 1 - i);
        cols[H + offset + i] = src + W * (H - 1 - i);
    }
    for (int i = 0; i < H; i++) {
        cols[i + offset] = src + i * W;
    }
    for (int i = 0; i < H; i++, dst += W) {
        for (int j = 0; j < W; j++) {
            float s0 = cols[i + offset][j] * gaussKer[0];
            for (int k = 1; k <= offset; k++) {
                s0 += (gaussKer[k] * (cols[i + offset - k][j] + cols[i + offset + k][j]));
            }
            dst[j] = s0;
        }
    }
    free(cols);
}


void matGaussianBlur_f32(const mat_f32 *src, mat_f32 *dst, int kSize) {
    // create gaussian kernel
    int offset = (kSize - 1) >> 1;
    float *gaussKer = (float *) malloc(sizeof(float) * (offset + 1));
    getGaussKer_f32(gaussKer, offset);

    // SymmRowVec Blur
    float *rows = (float *) malloc(sizeof(float) * src->rows * src->cols);
    SymmRowVecBlur_f32(src, rows, gaussKer, offset);

    // SymmColVec Blur
    SymmColVecBlur_f32(rows, dst, gaussKer, offset);

    free(rows);
    free(gaussKer);
}
