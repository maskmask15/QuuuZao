//
// Created by ozgaga on 2022/11/2.
//
#include "mat.h"
#include "stdlib.h"

void matConvert_f64_s64(const mat_f64 *src, mat_s64 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = (int64_t) src->data[i];
    }
}

void matConvert_u16_s64(const mat_u16 *src, mat_s64 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = (int64_t) src->data[i];
    }
}

void matMeanBlur_201_s64(const mat_s64 *src, mat_s64 *dst) {
    int H = src->rows;
    int W = src->cols;
    // 横向滤波
    int h = H - 200, w = W - 200;
    int64_t *buffer = (int64_t *) malloc(sizeof(int64_t) * H * w);
    int64_t *buffer_head = buffer;
    int64_t *src_data = src->data;

    for (int i = 0; i < H; i++, src_data += W, buffer_head += w) {
        int64_t s0 = src_data[0];
        for (int k = 1; k < 201; k++) s0 += src_data[k];
        buffer_head[0] = s0;
        for (int j = 1; j < w; j++) {
            s0 += (src_data[j + 200] - src_data[j - 1]);
            buffer_head[j] = s0;
        }
    }

    // 纵向滤波
    for (int j = 0; j < w; j++) {
        int64_t s0 = buffer[j];
        for (int k = 1; k < 201; k++) s0 += buffer[k * w + j];
        dst->data[j] = s0 / (201 * 201);
        for (int i = 1; i < h; i++) {
            s0 += (buffer[(i + 200) * w + j] - buffer[(i - 1) * w + j]);
            dst->data[i * w + j] = s0 / (201 * 201);
        }
    }
    free(buffer);
}

void matGetRect_s64(const mat_s64 *src, mat_s64 *dst, int s_x, int s_y) {
    int h = dst->rows;
    int w = dst->cols;
    int W = src->cols;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            dst->data[i * w + j] = src->data[(i + s_x) * W + (j + s_y)];
        }
    }
}

void matSub_s64(const mat_s64 *src1, const mat_s64 *src2, mat_s64 *dst) {
    for (int i = 0; i < src1->rows * src1->cols; i++) {
        dst->data[i] = src1->data[i] - src2->data[i];
    }
}

void matSubS_s64(const mat_s64 *src, int64_t val, mat_s64 *dst) {
    for (int i = 0; i < src->rows * src->cols; i++) {
        dst->data[i] = src->data[i] - val;
    }
}

int64_t matAvg_s64(const mat_s64 *src) {
    int64_t avg = 0;
    for (int i = 0; i < src->rows * src->cols; i++) {
        avg += src->data[i];
    }
    return avg / (int64_t) (src->rows * src->cols);
}

int64_t matSum_s64(const mat_s64 *src) {
    int64_t sum = 0;
    for (int i = 0; i < src->rows * src->cols; i++) {
        sum += src->data[i];
    }
    return sum;
}

void matPow2_s64(const mat_s64 *src, mat_s64 *dst) {
    for (int i = 0; i < src->rows * src->cols; i++) {
        dst->data[i] = src->data[i] * src->data[i];
    }
}
