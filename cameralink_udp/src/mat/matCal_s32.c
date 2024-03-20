//
// Created by ozgaga on 2022/10/3.
//

#include "matCal_s32.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../nuc/utils.h"

void matConvert_f32_s32(const mat_f32 *src, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = (int) roundf(src->data[i]);
    }
}

void matGetRect_s32(const mat_s32 *src, mat_s32 *dst, int s_x, int s_y) {
    int h = dst->rows;
    int w = dst->cols;
    int W = src->cols;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            dst->data[i * w + j] = src->data[(i + s_x) * W + (j + s_y)];
        }
    }
}

void matSub_s32(const mat_s32 *src1, const mat_s32 *src2, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = src1->data[i] - src2->data[i];
    }
}

void matAddS_s32(const mat_s32 *src, int val, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = src->data[i] + val;
    }
}

void matSubS_s32(const mat_s32 *src, int val, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = src->data[i] - val;
    }
}

// 婵絾妫佺欢婵囩瀹�鍐挎嫹閸滃啰绀夐弶鈺傛煥濞叉牠寮撮弶鎴欎海闁稿鎷�
void matMaxS_s32(const mat_s32 *src, int max_val, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        if (src->data[i] < max_val) dst->data[i] = max_val;
        else dst->data[i] = src->data[i];
    }
}

int matAvg_s32(const mat_s32 *src) {
    int64_t avg = 0;
    for (int i = 0; i < src->rows * src->cols; i++) avg += src->data[i];
    return (int) (avg / (src->rows * src->cols));
}

void matSetZero_s32(mat_s32 *src) {
    memset(src->data, 0, sizeof(int32_t) * src->rows * src->cols);
}

void matAdd_s32(const mat_s32 *src1, const mat_s32 *src2, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = src1->data[i] + src2->data[i];
    }
}

void matAbs_s32(const mat_s32 *src, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = abs(src->data[i]);
    }
}

void matPow2_s32(const mat_s32 *src, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = src->data[i] * src->data[i];
    }
}

void matCopy_s32(const mat_s32 *src, mat_s32 *dst) {
    memcpy(dst->data, src->data, sizeof(int) * dst->rows * dst->cols);
}

void matMaxZero_s32(const mat_s32 *src, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        if (src->data[i] < 0) dst->data[i] = 0;
        else dst->data[i] = src->data[i];
    }
}

void matDiv_s32(const mat_s32 *src1, const mat_s32 *src2, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = src1->data[i] / src2->data[i];
    }
}

void matMul_s32(const mat_s32 *src1, const mat_s32 *src2, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = src1->data[i] * src2->data[i];
    }
}

// 閻犱緤绱曢悾濠氬棘閻熺増鈻�
uint16_t matStd_s32(mat_s32 *src) {
    int mean = matAvg_s32(src);
    uint32_t std = 0;
    for (int i = 0; i < src->rows * src->cols; i++) std += ((src->data[i] - mean) * (src->data[i] - mean));
    uint32_t x = std / (src->rows * src->cols);
    /***** sqrt(int) *****/
    uint16_t res = 0;
    uint16_t add = 0x8000;
    for (int i = 0; i < 16; i++) {
        uint16_t tmp = res | add;
        uint32_t g2 = tmp * tmp;
        if (x >= g2) res = tmp;
        add >>= 1;
    }
    /***** sqrt(int) *****/
    return res;
}

//float matStd_s32(mat_s32 *src){
//    float mean = (float)matAvg_s32(src);
//    float std = 0;
//    for (int i = 0; i < src->rows * src->cols; i++) std += ((src->data[i] - mean) * (src->data[i] - mean));
//    return sqrtf(std / (float)(src->rows * src->cols));
//}

void matMeanBlur_201_s32(const mat_s32 *src, mat_s32 *dst) {
    int H = src->rows;
    int W = src->cols;
    // 婵☆垼浜滈幃婊冾煥閵堝棗鐨�
    int h = H - 200, w = W - 200;
    int32_t *buffer = (int32_t *) malloc(sizeof(int32_t) * H * w);
    int32_t *buffer_head = buffer;
    int32_t _size = 201 * 201;  // 婵櫢鎷�201闁汇劌瀚敓鑺ュ笚閺嗭拷
    int32_t *src_data = src->data;

    for (int i = 0; i < H; i++, src_data += W, buffer_head += w) {
        int32_t s0 = src_data[0];
        for (int k = 1; k < 201; k++) s0 += src_data[k];
        buffer_head[0] = s0;
        for (int j = 1; j < w; j++) {
            s0 += (src_data[j + 200] - src_data[j - 1]);
            buffer_head[j] = s0;
        }
    }

    // 缂侀箖娼ч幃婊冾煥閵堝棗鐨�
    for (int j = 0; j < w; j++) {
        int32_t s0 = buffer[j];
        for (int k = 1; k < 201; k++) s0 += buffer[k * w + j];
        dst->data[j] = s0 / _size;
        for (int i = 1; i < h; i++) {
            s0 += (buffer[(i + 200) * w + j] - buffer[(i - 1) * w + j]);
            dst->data[i * w + j] = s0 / _size;
        }
    }
    free(buffer);
}

void matSort_s32(const mat_s32 *src, mat_s32 *dst) {
    memcpy(dst->data, src->data, sizeof(int) * dst->rows * dst->cols);
    sortNums_s32(dst->data, 0, dst->rows * dst->cols - 1);
}

void matRectConvert_u16_s32(const mat_u16 *src, mat_s32 *dst, int s_x, int s_y) {
    int W = src->cols;
    int h = dst->rows;
    int w = dst->cols;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            dst->data[i * w + j] = (int) src->data[(i + s_x) * W + (j + s_y)];
        }
    }
}

// kernel = {-1,1}, 濞达絽妫欏Σ鎼佸捶閵娿儱绁圭紒澶樺灟閼垫垶鎯旈弮鍥跺殙闁哄嫮妲�1,-1}闁挎稑濂旂徊楣冨础妞嬪巩婵嬪及椤栨艾鍐�闁活偓鎷风紒鐘愁殘濞堟垿鏁嶇仦鎯ь暡濞寸姰鍎遍悿鍕⒔閸涱喗笑-1闁挎冻鎷�1
void matFilter_Rect50_s32(const mat_s32 *src, mat_s32 *dst) {
    int H = src->rows, W = src->cols;
    for (int i = 50, ii = 0; i < H - 50; i++, ii++) {
        for (int j = 50, jj = 0; j < W - 51; j++, jj++) {
            dst->data[ii * (W - 101) + jj] = src->data[i * W + j + 1] - src->data[i * W + j];
        }
    }
}

int matSum_s32(const mat_s32 *src) {
    int sum = 0;
    for (int i = 0; i < src->rows * src->cols; i++) sum += src->data[i];
    return sum;
}

int64_t matSum_s32_re64(const mat_s32 *src) {
    int64_t sum = 0;
    for (int i = 0; i < src->rows * src->cols; i++) sum += src->data[i];
    return sum;
}
