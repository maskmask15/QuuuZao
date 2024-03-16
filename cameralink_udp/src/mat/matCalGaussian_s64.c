//
// Created by ozgaga on 2022/11/2.
//

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mat.h"

#define SIZE 25

// 闁插洤褰囬惃鍕煙濡楀牊妲搁敍灞肩箽鐠囦焦鏆熼幑顔荤瑝濠с垹鍤惃鍕閹绘劒绗呴敍灞界殺閸楅袧閺嶅憡鏂佹径顪鹃崐宥忕礉閺堬拷閸氬骸鐨㈤崡椋幮濈紒鎾寸亯缂傗晛鐨瑇閸婂稄绱遍崣顖炲櫚閻€劎些娴ｅ秴鐤勯悳鎵級閺�锟�
void getGaussKer_s64(int64_t *gaussKer, int offset) {
    double *gauss_tmp = (double *) malloc(sizeof(double) * (offset + 1));
    double sigmaX = (double) (offset >> 1);
    double scale2X = -0.5 / (sigmaX * sigmaX);
    double dSum = 0;
    for (int i = 0; i <= offset; i++) {
        gauss_tmp[i] = exp(scale2X * i * i);
        dSum += (gauss_tmp[i] + gauss_tmp[i]);
    }
    dSum = ((double) (1 << SIZE)) / (dSum - gauss_tmp[0]);
    for (int i = 0; i <= offset; i++) {
        gaussKer[i] = (int64_t) round(gauss_tmp[i] * dSum);
    }
    free(gauss_tmp);
}

void SymmRowVecBlur_s64(const mat_s64 *src_, int64_t *dst, const int64_t *gaussKer, int offset) {
    int H = src_->rows, W = src_->cols;
    int64_t *row = (int64_t *) malloc(sizeof(int64_t) * (W + offset + offset));
    int64_t *src = src_->data;

    for (int i = 0; i < H; i++, dst += W, src += W) {
        memcpy(row + offset, src, W * sizeof(int64_t));
        for (int j = 0; j < offset; j++) {
            row[j] = src[offset - 1 - j];
            row[W + offset + j] = src[W - 1 - j];
        }
        for (int j = 0; j < W; j++) {
            int64_t s0 = (row[j + offset]) * gaussKer[0];
            for (int k = 1; k <= offset; k++) {
                s0 += (gaussKer[k] * (row[j + offset - k] + row[j + offset + k]));
            }
            dst[j] = ((s0) >> SIZE);
        }
    }
    free(row);
}

void SymmColVecBlur_s64(int64_t *src, mat_s64 *dst_, const int64_t *gaussKer, int offset) {
    int H = dst_->rows, W = dst_->cols;
    int64_t *dst = dst_->data;

    int64_t **cols = (int64_t **) malloc(sizeof(int64_t *) * (H + offset + offset));
    for (int i = 0; i < offset; i++) {
        cols[i] = src + W * (offset - 1 - i);
        cols[H + offset + i] = src + W * (H - 1 - i);
    }
    for (int i = 0; i < H; i++) {
        cols[i + offset] = src + i * W;
    }
    for (int i = 0; i < H; i++, dst += W) {
        for (int j = 0; j < W; j++) {
            int64_t s0 = (cols[i + offset][j]) * gaussKer[0];
            for (int k = 1; k <= offset; k++) {
                s0 += (gaussKer[k] * (cols[i + offset - k][j] + cols[i + offset + k][j]));
            }
            dst[j] = (s0 >> SIZE);
        }
    }
    free(cols);
}


void matGaussianBlur_s64(const mat_s32 *src, mat_s32 *dst, int kSize) {
    // create gaussian kernel
    int offset = (kSize - 1) >> 1;
    int64_t *gaussKer = (int64_t *) malloc(sizeof(int64_t) * (offset + 1));
    getGaussKer_s64(gaussKer, offset);

    mat_s64 *tmp = createMat_s64(src->rows, src->cols);
    for (int i = 0; i < src->rows * src->cols; i++) tmp->data[i] = src->data[i];

    // SymmRowVec Blur
    int64_t *rows = (int64_t *) malloc(sizeof(int64_t) * src->rows * src->cols);
    SymmRowVecBlur_s64(tmp, rows, gaussKer, offset);

    // SymmColVec Blur
    SymmColVecBlur_s64(rows, tmp, gaussKer, offset);

    for (int i = 0; i < src->rows * src->cols; i++) dst->data[i] = (int) tmp->data[i];

    matRelease_s64(&tmp);
    free(rows);
    free(gaussKer);
}
