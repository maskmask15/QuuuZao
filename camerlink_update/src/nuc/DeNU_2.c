//
// Created by ozgaga on 2022/11/6.
//

#include "InitParam.h"
#include "../mat/mat.h"
#include "../mat/matCal_s32.h"
#include "../mat/matCal_s64.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "xtime_l.h"
#include "xparameters.h"
#include "xaxidma.h"
#include "file_op.h"
//#include "../xdm/dma_DeNU_2.h"
#include "../xdm/dma.h"

extern struct GlobalNUC globalNuc;
extern struct GlobalVar globalVar;
extern struct pthread_nuc pthread_DeNU;

static mat_s32 *sigmaUT = NULL;
static mat_s32 *thetaUT = NULL;

#define MAX_ROW 512
#define MAX_COL 640
volatile int32_t LMSP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t LMP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t SSSP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t SSP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));

void InitDeNU(int H, int W) {
    sigmaUT = createMat_s32(H, W);
    thetaUT = createMat_s32(H, W);
    for (int i = 0; i < H * W; i++) {
        sigmaUT->data[i] = 1  << (nucShift + nuShift);
        thetaUT->data[i] = 0;
    }
}


void *DeNU_s32(void *value) {
    static int flag = 0;

    XTime start_time, end_time;

    int NU_kSize = *((int *) value);
    int H = globalNuc.NUC_DeStrip->rows;
    int W = globalNuc.NUC_DeStrip->cols;

    int mu = (globalVar.D << nuShift) / 1000;  // ��������, Դ�����ѱ��Ŵ�NUC_Shift

    XTime_GetTime(&start_time);
    mat_s32 *local_mean_src = createMat_s32_A(H, W, LMSP);   // local_mean
    mat_s32 *local_mean = createMat_s32_A(H, W, LMP);   // local_mean

    matSub_s32(globalNuc.NUC_DeStrip, globalVar.NU, local_mean_src);


    //matGaussianBlur_s64(local_mean_src, local_mean, NU_kSize);

//    printf("------------------------GaussFPGA_start\n");
    GaussianBlur_s64_FPGA_while(local_mean_src,local_mean);
    matRelease_s32_A(&local_mean_src);
    mat_s32 *sigmaU_square_src = createMat_s32_A(H, W, SSSP);   // local_mean sigmaU_square
    mat_s32 *sigmaU_square = createMat_s32_A(H, W, SSP);   // local_mean sigmaU_square
    matSub_s32(globalNuc.NUC_DeStrip, local_mean, sigmaU_square_src);
    matAbs_s32(sigmaU_square_src, sigmaU_square_src);
    GaussianBlur_s64_FPGA_while(sigmaU_square_src,sigmaU_square);
    matRelease_s32_A(&sigmaU_square_src);
    for (int i = 0; i < H * W; i++) thetaUT->data[i] = (thetaUT->data[i] * 8 + globalNuc.NUC_DeStrip->data[i] * 2) / 10;

    for (int i = 0; i < H * W; i++) {
        sigmaUT->data[i] = (sigmaUT->data[i] * 9 +
                            abs(globalNuc.NUC_DeStrip->data[i] - thetaUT->data[i])) / 10;
    }

    int len = (H * W) / 501;
    int total_num = H * W / len;

    mat_s32 *T = createMat_s32(1, total_num);  // local_mean sigmaU_square T
    int pos = len - 1;
    for (int i = 0; i < total_num; i++) {
        T->data[i] = sigmaUT->data[pos];
        pos += len;
    }

    matSort_s32(T, T);

    mat_s32 *S = createMat_s32(1, total_num);  // local_mean sigmaU_square T S
    pos = len - 1;
    for (int i = 0; i < total_num; i++) {
        S->data[i] = sigmaU_square->data[pos];
        pos += len;
    }
    matRelease_s32_A(&sigmaU_square);  // local_mean S T
    matSort_s32(S, S);

    int rnk = 0;
    int i = 0, j = 0;
    while ((i < total_num) && (j < total_num)) {
        if (S->data[j] < T->data[i]) {
            rnk += (i + j - 1);
            j++;
        } else {
            i++;
        }
    }
    matRelease_s32(&T);  // local_mean S
    matRelease_s32(&S);  // local_mean

    rnk += (total_num * (total_num - j) + (j + total_num - 1) * (total_num - j) / 2);

    /**** ������˶������NU ****/
    if (flag <= 10) {
        flag++;
    } else {
        if (rnk < (1.45e5)) {
            for (int k = 0; k < H * W; k++) {
                local_mean->data[k] = globalNuc.NUC_DeStrip->data[k] - local_mean->data[k] - globalVar.NU->data[k];
            }
            for (int k = 0; k < H * W; k++) {
                if (local_mean->data[k] < 0) local_mean->data[k] = -1;
                else if (local_mean->data[k] > 0) local_mean->data[k] = 1;
            }
            for (int k = 0; k < H * W; k++) {
                globalVar.NU->data[k] += (local_mean->data[k] * mu * 2);
            }
        }
    }

    XTime_GetTime(&end_time);
	u32 time_r = 0;
	time_r = ((end_time-start_time)*1000000)/(COUNTS_PER_SECOND);
	printf("DeNU_s32 time: %.6lf s\n", (double)time_r / 1000000);


    matRelease_s32_A(&local_mean);  // NULL
//    pthread_DeNU.running = false;

    return NULL;
}
