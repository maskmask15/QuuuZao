//
// Created by ozgaga on 2022/10/2.
//
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mat.h"

float matHist_f32(const mat_f32 *src, const float *range, int histSize) {
    float min_v = range[0];
    float max_v = range[1];
    float dis = (float) histSize / (max_v - min_v);
    uint16_t *buffer = (uint16_t *) malloc(sizeof(uint16_t) * histSize);
    memset(buffer, 0, sizeof(uint16_t) * histSize);
    for (int i = 0; i < src->rows * src->cols; i += 4) {
        float v0 = src->data[i];
        float v1 = src->data[i + 1];
        float v2 = src->data[i + 2];
        float v3 = src->data[i + 3];
        if (v0 >= min_v && v0 < max_v) buffer[(int) ((v0 - min_v) * dis)]++;
        if (v1 >= min_v && v1 < max_v) buffer[(int) ((v1 - min_v) * dis)]++;
        if (v2 >= min_v && v2 < max_v) buffer[(int) ((v2 - min_v) * dis)]++;
        if (v3 >= min_v && v3 < max_v) buffer[(int) ((v3 - min_v) * dis)]++;
    }

    int maxIndex = 0;
    for (int i = 1; i < histSize; i++) {
        if (buffer[i] > buffer[maxIndex]) maxIndex = i;
    }
    free(buffer);

    return min_v + (float) maxIndex / dis;
}

int matHist_s32(const mat_s32 *src, const int *range, int histSize) {
    int min_v = range[0];
    int max_v = range[1];
    int val = max_v - min_v;
    if (val < histSize) histSize = val;
    int step = 0;
    while (val >= histSize) {
        val -= histSize;
        step++;
    }
    if (val > 0) step++;

    uint16_t *buffer = (uint16_t *) malloc(sizeof(uint16_t) * histSize);
    memset(buffer, 0, sizeof(uint16_t) * histSize);  //初始化为0
    for (int i = 0; i < src->rows * src->cols; i++) {
        int v0 = src->data[i];
        if (v0 >= min_v && v0 < max_v) buffer[(v0 - min_v) / step]++;
    }

    int maxIndex = 0;
    for (int i = 1; i < histSize; i++) {
        if (buffer[i] > buffer[maxIndex]) maxIndex = i;
    }

    free(buffer);

    return min_v + maxIndex * step;
}
