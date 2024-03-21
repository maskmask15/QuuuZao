#include <stdio.h>
#include "IRNUC.h"
#include "../mat/mat.h"
#include "../mat/matCal_s32.h"
#include "xtime_l.h"
#include "shareOpConfig.h"
#include "../debug.h"

void get_Frame(mat_u16 *frame);
void set_Frame(mat_u16 *data);
void set_all_frame(int cols, int rows, uint16_t *start, int n);
int  get_all_frame(int cols, int rows, uint16_t *start);
void matRectConvert_up16_s32(const uint16_t *src, mat_s32 *dst);

struct SSN_param ssnParam;

#define MAX_ROW 512
#define MAX_COL 640
int32_t NUCP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
int32_t WP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));
int16_t FP[MAX_ROW * MAX_COL] __attribute__ ((aligned(8)));

mat_s32 *NUC;
mat_s32 *weight_s32;
mat_u16 *Frame;

void init_infrared_NUC(int src_H, int src_W, int res_H, int res_W, char *fp_path, char *video_path, char *res_path,
        int FrameRate, int NUIntensity, int ShadingIntensity, int NoiseIntensity){

	// 参数切换
	InitIntensity(FrameRate, NUIntensity, ShadingIntensity, NoiseIntensity, &ssnParam);
	// 需要处理的数据
    NUC = createMat_s32_A(res_H, res_W, NUCP);
    weight_s32 = createMat_s32_A(res_H, res_W, WP);
    Frame = createMat_u16_A(src_H, src_W, FP);
    //閼惧嘲绶遍弶鍐櫢
    int n = 0;
	while(1){
		n++;
		if(get_weight() == 1){
			xil_printf("cpu1 get init weight\n", n);
			read_weight(weight_s32);//闁哄秴娲ょ换鏃�绋夐敓锟�1閻犲洩顕цぐ鍥级閸愵喖娅�
			break;
		}
		sleep(1);
	}
}

//閸ュ搫鐣剧亸鍝勵嚟640 * 512
/**
 * inputData閺勶拷14bit raw鏉堟挸鍙�
 * 閸樿娅旈崥搴ｆ畱婢跺嫮鎮婄紒鎾寸亯鐎涙ê鍙嗘潻鏂挎礀閸婄》绱濋張澶嬫櫏閻ㄥ嫬銇囩亸蹇氬瘱閸ュ瓨妲�
 */
/*
 *
 *
 *
 */
uint16_t *infrared_NUC(uint16_t *inputData){

	int image_size = NUC->rows * NUC->cols;

	memcpy(Frame->data, inputData, Frame->cols * Frame->rows * 2);

    matRectConvert_u16_s32(Frame, NUC, 0, 0);
//    debugMean(NUC);

    if(get_weight() == 1){//婵絽绻嬬粩瀵告暜瑜嶇槐鎴炴叏鐎ｂ晝顓洪柛鎾崇Х椤曚即宕ｉ弽銊︽闁圭櫢鎷�
    	read_weight(weight_s32);
    }
//  debugMean(weight_s32);
	for(int i = 0; i < image_size; i++) NUC->data[i] = (NUC->data[i] << nucShift);
//	debugMean(NUC);
    SSN(NUC, &ssnParam, weight_s32);
//    debugMean(NUC);
	for(int i = 0;i < image_size;i++) Frame->data[i] = (unsigned short)(NUC->data[i]);

	return Frame->data;
}
