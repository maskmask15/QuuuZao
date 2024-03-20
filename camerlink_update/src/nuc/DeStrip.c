//
// Created by ozgaga on 2022/11/6.
//

#include "InitParam.h"
#include "../mat/matCal_s32.h"
#include "../mat/mat.h"
#include "shareOpConfig.h"
#include <math.h>
#include <stdio.h>
//#include "../xdm/dma_DeNU_2.h"
#include "xil_io.h"
#include "../xdm/dma.h"


extern struct GlobalNUC globalNuc;
extern struct GlobalVar globalVar;
extern struct pthread_nuc pthread_DeStrip;

void DeStrip_s32(mat_s32 *Y, int L) {
    int H = Y->rows;
    int W = Y->cols;

    mat_s32 *local_mean = createMat_s32(H, W);
    matGaussianBlur_s32(Y, local_mean, L);
    mat_s32 *residual = createMat_s32(H, W);
    matSub_s32(Y, local_mean, residual);

    matAbs_s32(residual, Y);
    matGaussianBlur_s32(Y, Y, L);

//    matPow2_s32(Y, Y);
    for (int i = 0; i < H * W; i++) {
        int val = (Y->data[i] >> (14 + nucShift - 15));
        Y->data[i] = val * val + 1; //闂佸搫鐗為幏鐑芥煕濮橆剚鎹ｆ繛鏉戭槼閵囨劙骞橀懠顒冨2^28闂佺尨鎷烽柟顖嗕椒娴�2^3 < 2^31
//        Y->data[i] = val * val; //闂佸搫鐗為幏鐑芥煕濮橆剚鎹ｆ繛鏉戭槼閵囨劙骞橀懠顒冨2^28闂佺尨鎷烽柟顖嗕椒娴�2^3 < 2^31
    }

    /** find median **/
    int median_val = (H * W / 2);
    mat_s32 *tmp_Y = createMat_s32(H, W);
    matCopy_s32(Y, tmp_Y);
    median_val = findKthNums_s32(tmp_Y->data, 0, H * W - 1, median_val);		// nowstate = 0

    printf("C mid :%d\n",median_val);

    matRelease_s32(&tmp_Y);
    /** find median **/

    int histSize = 100;
    int range[] = {3 * median_val / 10, median_val};							// max min step
    int sigmaN_square = matHist_s32(Y, range, histSize);						// now state = 1


    mat_s32 *sigmaU_square = createMat_s32(H, W);
    for (int i = 0; i < H * W; i++) sigmaU_square->data[i] = Y->data[i] - sigmaN_square;
    matMaxZero_s32(sigmaU_square, sigmaU_square);

    for (int i = 0; i < H * W; i++) {
        Y->data[i] = (int) ((int64_t) sigmaU_square->data[i] * (int64_t) residual->data[i]) / Y->data[i];
    }

//    matDiv_s32(sigmaU_square, Y, Y);
//    matMul_s32(Y, residual, Y);

    matAdd_s32(Y, local_mean, Y);

    matRelease_s32(&sigmaU_square);
    matRelease_s32(&residual);
    matRelease_s32(&local_mean);
}

void DeStrip_loop_once(mat_s32 *data){
	Xil_Out32(DENOISE_BASE_ADDR + START_REG, 0x1);
	Xil_Out32(DENOISE_BASE_ADDR + START_REG, 0x0);
	dmaSend(DeStrip_DMA_DEV_ID,data->data, data->cols * data->rows * 4);

//	printf("send done:%d\n",(Xil_In32(DENOISE_BASE_ADDR + FINISH_REG) & 0x1));
//	printf("send donedone\n");

	while((Xil_In32(DENOISE_BASE_ADDR + FINISH_REG) & 0x1) != 0x1);
//	printf("while done\n");
	Xil_Out32(DENOISE_BASE_ADDR + FINISH_REG, 0x0);

	waitDeStripTxInr();
	DeStrip_RT_Init();
}

void dmaSend(int device_id,void *src,int len){
	int status;
	Xil_DCacheFlushRange((UINTPTR)  (src), len);
	status = XAxiDma_SimpleTransfer(get_Axidma(device_id), (UINTPTR) src,
			len, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

}

void dmaGet(int device_id,void *dst,int len){
	int status;
	status = XAxiDma_SimpleTransfer(get_Axidma(device_id), (UINTPTR) dst,
			len, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
//	while (!(DeStrip_tx_done && DeStrip_rx_done) && !DeStrip_error){};
}

void DeStripInrErrorHander(){
	if(get_DeStrip_error()){
		printf("DeStrip Inr Error\n");
		exit(1);
	}
}

void waitClearDeStripRxInr(){
	wait_clear_intr(DMA_MASK_DE_DESTRIP_RX, DeStrip_DMA_DEV_ID, XAXIDMA_DEVICE_TO_DMA);
}

void waitClearDeStripTxInr(){
	wait_clear_intr(DMA_MASK_DE_DESTRIP_TX, DeStrip_DMA_DEV_ID, XAXIDMA_DMA_TO_DEVICE);
}

void waitClearDeStripRxTxInr(){
	waitClearDeStripTxInr();
	waitClearDeStripRxInr();
}

void waitDeStripRxInr(){
	while(!get_DeStrip_rx_done() && !get_DeStrip_error());
	DeStripInrErrorHander();
}
void waitDeStripTxInr(){
	while(!get_DeStrip_tx_done() && !get_DeStrip_error());
	DeStripInrErrorHander();
}

void waitDeStripTxRxInr(){
	while (!(get_DeStrip_tx_done() && get_DeStrip_rx_done()) && !get_DeStrip_error());
	DeStripInrErrorHander();
}

void wait_DeStrip_loop_once(mat_s32 *data){
	Xil_Out32(DENOISE_BASE_ADDR + START_REG, 0x1);
	Xil_Out32(DENOISE_BASE_ADDR + START_REG, 0x0);
	dmaSend(DeStrip_DMA_DEV_ID,data->data, data->cols * data->rows * 4);

//	printf("send done:%d\n",(Xil_In32(DENOISE_BASE_ADDR + FINISH_REG) & 0x1));
//	printf("send donedone\n");
//	printf("while fin\n");
	while((Xil_In32(DENOISE_BASE_ADDR + FINISH_REG) & 0x1) != 0x1);
//	printf("while done\n");
	Xil_Out32(DENOISE_BASE_ADDR + FINISH_REG, 0x0);

	waitClearDeStripTxInr();

	//Xil_DCacheInvalidateRange((UINTPTR)data->data, data->cols * data->rows * 4);
//	usleep(1);
}

void Mat_t(mat_s32 *src,mat_s32 *dst){
	for(int i = 0;i<src->rows;i++){//511 640
		for(int j = 0;j<src->cols;j++){
			dst->data[src->rows*j + i] = src->data[i*src->cols + j];
		}
	}
}


int compare(int a,int b){
	if(abs(a - b) < 5) return 0;
	else return 1;
}

void Mat_compare(mat_s32 *src,mat_s32 *dst,char *str){
	int errorCount = 0;
	for(int i = 0;i<src->rows;i++){//511 640
		for(int j = 0;j<src->cols;j++){
			if(compare(src->data[src->rows * i + j] , dst->data[src->rows * i + j]))
				errorCount ++;
		}
	}
	printf("%s error count :%d\n",str,errorCount);
}

#define MAX_ROW 512
#define MAX_COL 640
volatile int32_t YP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t TMPP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t TMPTP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t S2P[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));

//#define DEBUG_COMPARE

#ifdef DEBUG_COMPARE

volatile int32_t YP_B[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t TMPP_B[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t TMPTP_B[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
volatile int32_t S2P_B[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
#endif

void *DeStrip2_s32(void *value) {
    static int first = 0;
    int Strip_kSize = *((int *) value);

    int H = globalNuc.NUC_DeFPN->rows;
    int W = globalNuc.NUC_DeFPN->cols;

    mat_s32 *Y = createMat_s32_A(H, W, YP);
    matCopy_s32(globalNuc.NUC_DeFPN, Y);
    mat_s32 *tmp = createMat_s32_A(H, W, TMPP);
    mat_s32 *tmp_T = createMat_s32_A(W, H, TMPTP);
    matSetZero_s32(tmp);
    mat_s32 *S2 = createMat_s32_A(H, W, S2P);
    matSetZero_s32(S2);


#ifdef DEBUG_COMPARE
    //测试变量：
    int Strip_kSize_b = *((int *) value);
    mat_s32 *Y_b = createMat_s32_A(H, W,YP_B);
    matCopy_s32(globalNuc.NUC_DeFPN, Y_b);
    mat_s32 *tmp_b = createMat_s32_A(H, W, TMPP_B);
    matSetZero_s32(tmp_b);
    mat_s32 *S2_b = createMat_s32_A(H, W, S2P_B);
    matSetZero_s32(S2_b);
#endif


    DeStrip_RT_Init();
    for (int k = 0; k < 9; k++) {
//    	printf("loop:%d\n",k);
    	Mat_t(tmp,tmp_T);
		Xil_Out32(DENOISE_BASE_ADDR + CONTROL_REG, 0x20);
		wait_DeStrip_loop_once(tmp_T);
//		printf("1\n");
		Xil_Out32(DENOISE_BASE_ADDR + CONTROL_REG, 0x21);
		wait_DeStrip_loop_once(tmp);
//		printf("2\n");
		Xil_Out32(DENOISE_BASE_ADDR + CONTROL_REG, 0x02);
//		printf("n2\n");
		wait_DeStrip_loop_once(Y);
//		printf("3\n");
		int mid = Xil_In32(DENOISE_BASE_ADDR + MEDIAN_REG);
		//printf("FPGA mid :%d\n",mid);
		int histSize = 100;
		int range[] = {3 * mid / 10, mid};

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

		Xil_Out32(DENOISE_BASE_ADDR + MAX_REG ,max_v);
		Xil_Out32(DENOISE_BASE_ADDR + MIN_REG ,min_v);
		Xil_Out32(DENOISE_BASE_ADDR + STEP_REG,step);

		Xil_Out32(DENOISE_BASE_ADDR + CONTROL_REG, 0x12);
		wait_DeStrip_loop_once(Y);
//		printf("4\n");
		Xil_Out32(DENOISE_BASE_ADDR + CONTROL_REG, 0x22);
		Xil_Out32(DENOISE_BASE_ADDR + START_REG, 0x1);
		Xil_Out32(DENOISE_BASE_ADDR + START_REG, 0x0);
		dmaSend(DeStrip_DMA_DEV_ID,Y->data,Y->cols*Y->rows*4);
		Xil_DCacheInvalidateRange((UINTPTR)tmp->data, tmp->cols * tmp->rows*4);
		dmaGet(DeStrip_DMA_DEV_ID,tmp->data,tmp->cols*tmp->rows*4);
//		printf("while fin\n");
//		while((Xil_In32(DENOISE_BASE_ADDR + FINISH_REG) & 0x1) != 0x1);
//		printf("while done\n");
		Xil_Out32(DENOISE_BASE_ADDR + FINISH_REG, 0x0);
		waitClearDeStripRxTxInr();

//		usleep(1);
//		printf("5\n");
	}

    matRelease_s32_A(&tmp_T);
    matGetMedCol_s32(tmp, S2);
	matGetMedRow_s32(tmp, tmp);
	matAdd_s32(S2, tmp, S2);
	matSubS_s32(S2, matAvg_s32(S2), S2);

//========================================
//    for (int k = 0; k < 9; k++) {
//
//    	// axi_lite : sel = 0, now_state = 0, 閺夌儐鍓涢悿鍡涙儗閳哄懏鈻坱mp, START_REG = 1 		w
//    	matGetMedCol_s32(tmp, S2);			// sel = 0 		tmp > T
//// w   	START_REG = 0;
//    	// while(finish_reg != 1) r
//// w   	finish_reg = 0;
//    	// axi_lite : sel = 1, now_state = 0, tmp, START_REG = 1 		w
//    	matGetMedRow_s32(tmp, tmp);			// sel = 1		tmp
//    	// w   	START_REG = 0;
//    	// while(finish_reg != 1) r
//// w   	finish_reg = 0;
//
//    	// 閺夆晜鐟╅崳椋庢啺娴ｇ懓惟Y閺夊牊鎸搁崣鍡樼▔婢跺鍋ч柨娑樿嫰閸ㄥ酣宕氶锝囨勾mid, hist, tmp
//    	// sel = 2 state = 0 婵懓鍊烽懙鎴﹀磹閿燂拷, 闁绘帟娉涢幃妤呭捶閵娿倗鐟愬ù锝呯У濠э拷鐎电増顨呴崺灞剧▔椤撶儑鎷烽梻缈犵鞍闁告艾鍑界槐婵喰ф穱绉坣, max, step閺夊牊鎸搁崣鍡欑磼濡炬潝
//    	// sel = 2 state = 1 婵懓鍊诲ú鍧楀棘閻熺増绂�
//    	// sel = 2 state = 2 婵懓锟斤拷mp
//    	matAdd_s32(S2, tmp, S2);			// sel = 2 state = 0
//    	matSubS_s32(S2, matAvg_s32(S2), S2);
//		matSub_s32(Y, S2, tmp);								//Y
//		DeStrip_s32(tmp, Strip_kSize);
//		matSub_s32(Y, tmp, tmp);							// tmp
//    }
    // 缂佹鍏涚弧锟芥繛鍡忥拷铏剷闁绘粠鍨崇划銊╁级閻旀湹绨伴柛姘嚱缁辨繄鎲版担鍛婅含ps缂佹棏鍨辨晶鐣屾偘鐏為棿绨板☉鎾愁儑閻ｈ崵锟芥冻鎷�
	//		matGetMedCol_s32(tmp, S2);
	//        matGetMedRow_s32(tmp, tmp);
	//
	//        matAdd_s32(S2, tmp, S2);
	//        matSubS_s32(S2, matAvg_s32(S2), S2);

#ifdef DEBUG_COMPARE
    for (int k = 0; k < 9; k++) {
        matSub_s32(Y_b, S2_b, tmp_b);								//Y
        DeStrip_s32(tmp_b, Strip_kSize_b);
        matSub_s32(Y, tmp_b, tmp_b);							// tmp

        matGetMedCol_s32(tmp_b, S2_b);			// sel = 0 		tmp > T
        matGetMedRow_s32(tmp_b, tmp_b);			// sel = 1		tmp

        matAdd_s32(S2_b, tmp_b, S2_b);			// sel = 2 state = 0
        matSubS_s32(S2_b, matAvg_s32(S2_b), S2_b);
    }


    Mat_compare(S2,S2_b,"S2");
    Mat_compare(tmp,tmp_b,"tmp");
    Mat_compare(Y,Y_b,"Y");

#endif
//=================================================


	matRelease_s32_A(&tmp);

    uint16_t sigma = matStd_s32(S2);
    for (int i = 0; i < H * W; i++) {
        if (S2->data[i] < -sigma) S2->data[i] = -sigma;
        else if (S2->data[i] > sigma) S2->data[i] = sigma;
    }
    if (first == 0) {
        first = 1;
        matCopy_s32(S2, globalVar.Strip2);
    } else {
        for (int i = 0; i < H * W; i++) {
            globalVar.Strip2->data[i] = (globalVar.Strip2->data[i] * 8 + S2->data[i] * 2) / 10;
        }
    }

    matRelease_s32_A(&S2);
    matRelease_s32_A(&Y);

    return NULL;
}
//==============================================
//void *DeStrip2_s32(void *value) {
//    static int first = 0;
//    int Strip_kSize = *((int *) value);
//
//    int H = globalNuc.NUC_DeFPN->rows;
//    int W = globalNuc.NUC_DeFPN->cols;
//
//    mat_s32 *Y = createMat_s32(H, W);
//    matCopy_s32(globalNuc.NUC_DeFPN, Y);
//
//    mat_s32 *tmp = createMat_s32(H, W);
//    matSetZero_s32(tmp);
//    mat_s32 *S2 = createMat_s32(H, W);
//    matSetZero_s32(S2);
//
//    for (int k = 0; k < 9; k++) {
//        matSub_s32(Y, S2, tmp);
//        DeStrip_s32(tmp, Strip_kSize);
//        matSub_s32(Y, tmp, tmp);
//
//        matGetMedCol_s32(tmp, S2);
//        matGetMedRow_s32(tmp, tmp);
//
//        matAdd_s32(S2, tmp, S2);
//        matSubS_s32(S2, matAvg_s32(S2), S2);
//    }
//    matRelease_s32(&tmp);
//
//    uint16_t sigma = matStd_s32(S2);
//    for (int i = 0; i < H * W; i++) {
//        if (S2->data[i] < -sigma) S2->data[i] = -sigma;
//        else if (S2->data[i] > sigma) S2->data[i] = sigma;
//    }
//
//    if (first == 0) {
//        first = 1;
//        matCopy_s32(S2, globalVar.Strip2);
//    } else {
//        for (int i = 0; i < H * W; i++) {
//            globalVar.Strip2->data[i] = (globalVar.Strip2->data[i] * 8 + S2->data[i] * 2) / 10;
//        }
//    }
//
//    matRelease_s32(&S2);
//    matRelease_s32(&Y);
//
//
//    return NULL;
//}
