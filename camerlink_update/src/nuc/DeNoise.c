//
// Created by ozgaga on 2022/9/30.
//
#include <stdlib.h>

#include "InitParam.h"
#include "../mat/matCal_s32.h"

#include <stdio.h>
#include "xtime_l.h"
#include "shareOpConfig.h"

static mat_s32 *static_sigmaV_square;

void InitDeNoise(int H, int W) {
    static_sigmaV_square = createMat_s32(H, W);
}

void *updateSigmaN() {
    int H = static_sigmaV_square->rows;
    int W = static_sigmaV_square->cols;

    int median_val = H * W / 2;
    /** find median **/
    mat_s32 *tmp_fl = createMat_s32(H, W);
    matCopy_s32(static_sigmaV_square, tmp_fl);
    median_val = findKthNums_s32(tmp_fl->data, 0, H * W - 1, median_val);
    matRelease_s32(&tmp_fl);
    /** find median **/

    int histSize = 100;
    int range[] = {median_val * 3 / 10, median_val};
    int histVal = matHist_s32(static_sigmaV_square, range, histSize);

    set_sigmaN((get_sigmaN() * 9 + (int) (2 * histVal)) / 10);

    return NULL;
}

void DeNoise(mat_s32 *NUC, int NoiseSize, int cnt_mod1) {
    int row = NUC->rows;
    int col = NUC->cols;

    XTime start_time, end_time;
	//XTime_GetTime(&start_time);

    mat_s32 *local_mean = createMat_s32(NUC->rows, NUC->cols);
    matGaussianBlur_s32(NUC, local_mean, NoiseSize);
    mat_s32 *residual = createMat_s32(NUC->rows, NUC->cols);
    matSub_s32(NUC, local_mean, residual);

    /************ ¼ÆËãÔëÉù·½²î ************/
    mat_s32 *sigmaV_square = createMat_s32(NUC->rows, NUC->cols);
    matAbs_s32(residual, sigmaV_square);
    matGaussianBlur_s32(sigmaV_square, sigmaV_square, NoiseSize);
    for (int i = 0; i < row * col; i++) {

        int64_t val = sigmaV_square->data[i];
        sigmaV_square->data[i] = (int) ((val * val) >> (2 * (nucShift + dr) - 31));
    }
    //XTime_GetTime(&end_time);

    //printf("calc take %.6lf s\n",(((double)end_time-start_time)*1000000)/(COUNTS_PER_SECOND) / 1000000);

    if (cnt_mod1) {
    	matCopy_s32(sigmaV_square, static_sigmaV_square);
    	updateSigmaN(NULL);
    }

    matRelease_s32(&sigmaV_square);
    matRelease_s32(&residual);
    matRelease_s32(&local_mean);
}
