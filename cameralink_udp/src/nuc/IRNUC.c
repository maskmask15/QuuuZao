//
// Created by ozgaga on 2022/11/2.
//

//#include <sys/time.h>
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

	InitIntensity(FrameRate, NUIntensity, ShadingIntensity, NoiseIntensity, &ssnParam);
	//閻㈠疇顕拋锛勭暬閹碉拷闂囷拷閸愬懎鐡�
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


//void infrared_NUC(int src_H, int src_W, int res_H, int res_W, char *fp_path, char *video_path, char *res_path,
//                  int FrameRate, int NUIntensity, int ShadingIntensity, int NoiseIntensity) {
//	//闁告帗绻傞～鎰板礌閿燂拷
//
//    InitIntensity(FrameRate, NUIntensity, ShadingIntensity, NoiseIntensity, &ssnParam);
//
//
//    XTime start_time, end_time;
//    u32 time_r,SSN_time = 0;
//
//
//    double MIN = 0, MAX = 65535;   // 2^16-1
//    int cnt = 0;
//
//
//    mat_s32 *NUC = createMat_s32(res_H, res_W);
//    mat_s32 *weight_s32 = createMat_s32(res_H, res_W);
//    uint16_t *read_addr_position = FRAME_ADDR_BASE;
//    mat_u16 *Frame = createMat_u16(src_H, src_W);
//
//    uint16_t *cur_addr = read_addr_position;
//    //闁告帗绻傞～鎰板礌閺嶃劍缍�闂佽鎷�
//	int n = 0;
//	while(1){
//		n++;
//		if(get_weight() == 1){
//			xil_printf("cpu1 get init weight\n", n);
//			read_weight(weight_s32);//闁哄秴娲ょ换鏃�绋夐敓锟�1閻犲洩顕цぐ鍥级閸愵喖娅�
//			break;
//		}
//		sleep(1);
//	}
//
//	//闁兼儳鍢茬欢閬嶅炊閸撗冾暬闁轰胶澧楀畵锟�
//	int frame_num = get_all_frame(src_W, src_H, FRAME_ADDR_BASE);
//	printf("frame sum n:%d\n", frame_num);
//	//鐎殿噯鎷峰┑顔碱儓椤撳摜绮婚敓锟�
//    int othor_frame_num = frame_num;
//    XTime_GetTime(&start_time);
//    int frame_size = src_H * src_W;
//    while (othor_frame_num--) {
//    	int image_size = res_H * res_W;
//    	for(int i = 0;i < image_size;i++){
//    		Frame->data[i] = cur_addr[i];
//    	}
//
//        matRectConvert_u16_s32(Frame, NUC, 0, 0);
//
//        if(get_weight() == 1){//婵絽绻嬬粩瀵告暜瑜嶇槐鎴炴叏鐎ｂ晝顓洪柛鎾崇Х椤曚即宕ｉ弽銊︽闁圭櫢鎷�
//        	read_weight(weight_s32);
//        }
//
//		for (int i = 0; i < image_size; i++) NUC->data[i] = (NUC->data[i] << nucShift);
//        /******* 闂傚倸鍊搁崐鎼佸磹閹间焦鍋嬮煫鍥ㄧ☉绾剧粯绻涢幋鐑囦緵闁绘帊绮欓獮鏍庨锟芥俊鑲╃磼閿熻棄鐣濋崟顒傚弳闂佺粯鏌ㄦ晶搴ㄦ儗濞嗘劦娈介柣鎴濇处鐎氬綊鏌ｉ悢鍝ョ煀闂佸府缍侀獮鍐槼闁诡垱妫冮崺銉╁幢濡眹锟藉啴姊绘担鑺ョ《闁革綇绠撻獮蹇涙晸閿燂拷 ********/
//
//        SSN(NUC, &ssnParam, weight_s32);
//
//    	for(int i = 0;i < image_size;i++){
//    		cur_addr[i] = (unsigned short)NUC->data[i];
//    	}
//
//		cur_addr += frame_size;
//		printf("fram num %d\n", cnt);
//		cnt++;
//    }
//
//	XTime_GetTime(&end_time);
//	time_r = ((end_time-start_time)*1000000)/(COUNTS_PER_SECOND);
//	printf("cpu0 total frame %d total time: %.6lf s\n", frame_num, (double)time_r / 1000000);
//    matRelease_s32(&NUC);
//
//    set_all_frame(src_W, src_H, FRAME_ADDR_BASE, frame_num);
//}
