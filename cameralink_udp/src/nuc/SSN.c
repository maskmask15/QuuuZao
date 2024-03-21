//
// Created by ozgaga on 2022/11/2.
//

#include "xaxidma.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "IRNUC.h"
#include "../mat/matCal_s32.h"
#include <stdio.h>
#include "xtime_l.h"
#include "shareOpConfig.h"
#include "file_op.h"
#include "../xdm/dma.h"
#include "../debug.h"



volatile int tx_done;      //闂佸憡鐟﹂崹鍧楁晸閹存帞顦﹂柣锝庡墴楠炲骞囬鐘茬伇闂婎偄娴勯幏锟�
volatile int rx_done;      //闂佽浜介崕鏌ュ极閸︻叏鎷烽悷鐗堟拱闁搞劍宀稿浠嬪炊瑜忕粻锟�
volatile int error;        //婵炵鍋愭慨鍓ф椤撱垹绀勬繛宸簼閺呪晠鏌″鍛悙缂佷紮鎷�

XAxiDma* get_Axidma();

void set_tx_done(){
//	printf("set tx\n");
	tx_done = 1;
}

void set_rx_done(){
//	printf("set rx\n");
	rx_done = 1;
}

void set_error(){
//	printf("set error\n");
	error = 1;
}

void DeNoise_FPGA(mat_s32 *NUC_s32, int mean, int N){
	//闂佸憡甯楃换鍌烇綖閹版澘绀岄柡宥冨妿閸ㄥジ鐓崶銊ｄ沪濞ｅ洤锕畷锝夋晸閿燂拷
	tx_done = 0;
	rx_done = 0;
	error   = 0;

	Xil_Out32(DENOISE_BASE_ADDR + DENOISE_OFFSET_N, N);
	Xil_Out32(DENOISE_BASE_ADDR + DENOISE_OFFSET_MEAN, mean);

	int lrngth = NUC_s32->cols * NUC_s32->rows * 4;
	int status;

	Xil_DCacheFlushRange((UINTPTR)  (NUC_s32->data), lrngth);   //闂佸憡甯￠弨閬嶅蓟婵傚儭ta Cache
    status = XAxiDma_SimpleTransfer(get_Axidma(), (UINTPTR) NUC_s32->data,
    		lrngth, XAXIDMA_DMA_TO_DEVICE);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    status = XAxiDma_SimpleTransfer(get_Axidma(), (UINTPTR) NUC_s32->data,
     		lrngth, XAXIDMA_DEVICE_TO_DMA);

    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }
    Xil_DCacheFlushRange((UINTPTR)(NUC_s32->data), lrngth);

    while (!(tx_done && rx_done) && !error){};

    if (error) {
        xil_printf("Failed test transmit%s done, "
                "receive%s done\r\n", tx_done ? "" : " not",
                rx_done ? "" : " not");
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

void wait_DeNoise_FPGA(mat_s32 *NUC_s32, int mean, int N){
	//闂佸憡甯楃换鍌烇綖閹版澘绀岄柡宥冨妿閸ㄥジ鐓崶銊ｄ沪濞ｅ洤锕畷锝夋晸閿燂拷
//	printf("DeNoise_fpga\n");
	Xil_Out32(DENOISE_BASE_ADDR + DENOISE_OFFSET_N, N);
	Xil_Out32(DENOISE_BASE_ADDR + DENOISE_OFFSET_MEAN, mean);

	int lrngth = NUC_s32->cols * NUC_s32->rows * 4;
	int status;

	Xil_DCacheFlushRange((UINTPTR)  (NUC_s32->data), lrngth);   //闂佸憡甯￠弨閬嶅蓟婵傚儭ta Cache
    status = XAxiDma_SimpleTransfer(get_Axidma(DeNoise_DMA_DEV_ID), (UINTPTR) NUC_s32->data,
    		lrngth, XAXIDMA_DMA_TO_DEVICE);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }
//    printf("XAXIDMA_DMA_TO_DEVICE\n");
    status = XAxiDma_SimpleTransfer(get_Axidma(DeNoise_DMA_DEV_ID), (UINTPTR) NUC_s32->data,
     		lrngth, XAXIDMA_DEVICE_TO_DMA);

    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    Xil_DCacheInvalidateRange((UINTPTR)(NUC_s32->data), lrngth);

	wait_clear_intr(DMA_MASK_DE_NOISE_TX, DeNoise_DMA_DEV_ID, XAXIDMA_DMA_TO_DEVICE);
	wait_clear_intr(DMA_MASK_DE_NOISE_RX, DeNoise_DMA_DEV_ID, XAXIDMA_DEVICE_TO_DMA);

//	usleep(100);

    return XST_SUCCESS;
}

void SSN(mat_s32 *NUC_s32, struct SSN_param *ssnParam, mat_s32 *weight_s32) {
    static int cnt_mod1 = 0, cnt_mod2 = 0;
    //闂佽绻愮换鎰板箹閻━_s32闂備焦妞块崢鎼佸疾閻樺弬鐔兼晸娴犲鐓曟繛鍡樺姇閻忊晠鏌℃担瑙勫磳闁硅櫕鐩畷妤呭川椤栨粎鍘鹃梻浣告惈缁嬪嫰骞忛敓锟� 闂備線娼荤拹鐔煎礉鐎ｎ剙鍨濋柣妯款嚙閸愶拷闂傚偊鎷烽柟閭﹀灠楠烇拷闂備礁鎼ú銈夋偤閵娾晛钃熷┑鐘插婵ジ鏌ㄥ☉妯侯伀闁哄棴鎷�
    if((cnt_mod1 == 0 || cnt_mod2 == 0) && get_image() == 0){//閸欘垯浜掗崘娆忓弳閺佺増宓侀敍灞借嫙娑撴柨缍嬮悞鍫曟付鐟曚焦娲块弬鐗堟綀闁插秹鍋呮稊鍫濆晸閸忥拷
    	set_mod1(cnt_mod1 == 0);
    	set_mod2(cnt_mod2 == 0);
    	write_nuc(NUC_s32);
    }

    int ORG_mean = matAvg_s32(NUC_s32);
//    printf("ORG_mean:%d", ORG_mean);
    matSubS_s32(NUC_s32, ORG_mean, NUC_s32);

    matAdd_s32(NUC_s32, weight_s32, NUC_s32);
//    debugMean(NUC_s32);
//    printf("get_sigmaN:%d", get_sigmaN());
    if (ssnParam->Noise_kSize != 0) wait_DeNoise_FPGA(NUC_s32, ORG_mean, get_sigmaN());
//    debugMean(NUC_s32);
    cnt_mod1++;
    cnt_mod2++;
    if (cnt_mod1 == ssnParam->FR_half) cnt_mod1 = 0;
    if (cnt_mod2 == ssnParam->FR_fifth) cnt_mod2 = 0;
}
