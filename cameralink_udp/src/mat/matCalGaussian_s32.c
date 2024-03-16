//
// Created by ozgaga on 2022/10/5.
//

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mat.h"

#define SIZE 12

// 闁插洤褰囬惃鍕煙濡楀牊妲搁敍灞肩箽鐠囦焦鏆熼幑顔荤瑝濠с垹鍤惃鍕閹绘劒绗呴敍灞界殺閸楅袧閺嶅憡鏂佹径顪鹃崐宥忕礉閺堬拷閸氬骸鐨㈤崡椋幮濈紒鎾寸亯缂傗晛鐨瑇閸婂稄绱遍崣顖炲櫚閻€劎些娴ｅ秴鐤勯悳鎵級閺�锟�
void getGaussKer_s32(int *gaussKer, int offset) {
    float *gauss_tmp = (float *) malloc(sizeof(float) * (offset + 1));
    double sigmaX = offset >> 1;
    double scale2X = -0.5 / (sigmaX * sigmaX);
    double dSum = 0;
    for (int i = 0; i <= offset; i++) {
        gauss_tmp[i] = (float) (exp(scale2X * i * i));
        dSum += (gauss_tmp[i] + gauss_tmp[i]);
    }
    dSum = (float) (1 << SIZE) / (dSum - gauss_tmp[0]);
    for (int i = 0; i <= offset; i++) {
        gaussKer[i] = (int) roundf(gauss_tmp[i] * (float) dSum);
    }
    free(gauss_tmp);
}

void SymmRowVecBlur_s32(const mat_s32 *src_, int *dst, const int *gaussKer, int offset) {
    int H = src_->rows, W = src_->cols;
    int *row = (int *) malloc(sizeof(int) * (W + offset + offset));
    int *src = src_->data;

    for (int i = 0; i < H; i++, dst += W, src += W) {
        memcpy(row + offset, src, W * sizeof(int));
        for (int j = 0; j < offset; j++) {
            row[j] = src[offset - 1 - j];
            row[W + offset + j] = src[W - 1 - j];
        }
        for (int j = 0; j < W; j++) {
            int s0 = (row[j + offset]) * gaussKer[0];
            for (int k = 1; k <= offset; k++) {
                s0 += (gaussKer[k] * (row[j + offset - k] + row[j + offset + k]));
            }
            dst[j] = (s0) >> SIZE;
        }
    }
    free(row);
}

void SymmColVecBlur_s32(int *src, mat_s32 *dst_, const int *gaussKer, int offset) {
    int H = dst_->rows, W = dst_->cols;
    int *dst = dst_->data;

    int **cols = (int **) malloc(sizeof(int *) * (H + offset + offset));
    for (int i = 0; i < offset; i++) {
        cols[i] = src + W * (offset - 1 - i);
        cols[H + offset + i] = src + W * (H - 1 - i);
    }
    for (int i = 0; i < H; i++) {
        cols[i + offset] = src + i * W;
    }
    for (int i = 0; i < H; i++, dst += W) {
        for (int j = 0; j < W; j++) {
            int s0 = (cols[i + offset][j]) * gaussKer[0];
            for (int k = 1; k <= offset; k++) {
                s0 += (gaussKer[k] * (cols[i + offset - k][j] + cols[i + offset + k][j]));
            }
            dst[j] = (s0 >> SIZE);
        }
    }
    free(cols);
}


void matGaussianBlur_s32(const mat_s32 *src, mat_s32 *dst, int kSize) {
    // create gaussian kernel
    int offset = (kSize - 1) >> 1;
    int *gaussKer = (int *) malloc(sizeof(int) * (offset + 1));
    getGaussKer_s32(gaussKer, offset);

    // SymmRowVec Blur
    int *rows = (int *) malloc(sizeof(int) * src->rows * src->cols);
    SymmRowVecBlur_s32(src, rows, gaussKer, offset);

    // SymmColVec Blur
    SymmColVecBlur_s32(rows, dst, gaussKer, offset);

    free(rows);
    free(gaussKer);
}
