#include <stdio.h>
#include "IRNUC.h"
#include "shareOpConfig.h"
#include "InitParam.h"
#include "../mat/mat.h"
#include "../mat/matCal_s32.h"
#include "xtime_l.h"
#include "file_op.h"


void updateWeight(mat_s32 *dst);

void infrared_NUC(int src_H, int src_W, int res_H, int res_W, char *fp_path, char *video_path, char *res_path,
                  int FrameRate, int NUIntensity, int ShadingIntensity, int NoiseIntensity) {

    /******** init global variables ********/
    InitGlobalVar(res_H, res_W, fp_path);

    /****** switch parameters of Intensity ******/
    struct SSN_param ssnParam;
    InitIntensity(FrameRate, NUIntensity, ShadingIntensity, NoiseIntensity, &ssnParam);

    XTime start_time, end_time;
    u32 time_r,SSN_time = 0;

    double MIN = 0, MAX = 65535;   // 2^16-1
    int cnt = 0;

    mat_s32 *NUC_s32 = createMat_s32(res_H, res_W);
    mat_s32 *weight_s32 = createMat_s32(res_H, res_W);

    updateWeight(weight_s32);

    int mod_cnt1;
    int mod_cnt2;
    //锟斤拷预锟斤拷锟斤拷锟斤拷锟斤拷
    while (1) {
        //锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟酵计拷锟斤拷虏锟斤拷锟�
    	int n = 0;
        while(1){
        	n++;
    		if(get_image() == 1){
    			//锟斤拷mod锟斤拷锟叫断革拷锟斤拷锟斤拷一锟斤拷模锟斤拷
    	        mod_cnt1 = get_mod1();
    	        mod_cnt2 = get_mod2();
    	        //锟接癸拷锟斤拷DDR锟叫讹拷取图片
    			read_nuc(NUC_s32);
    			printf("cpu0 read image update weight\n", n);
    			break;
    		}
    		usleep(1);
        }

        //锟斤拷锟斤拷锟斤拷锟饺拷锟�
        XTime_GetTime(&start_time);
        SSN(NUC_s32, &ssnParam, weight_s32, mod_cnt1, mod_cnt2);
//        SSN(NUC_s32, &ssnParam, weight_s32, 1, 1);
        XTime_GetTime(&end_time);

        cnt++;
        printf("update_frame_num: %d\n", cnt);
        printf("frame %d take %.6lf s\n", cnt,(((double)end_time-start_time)*1000000)/(COUNTS_PER_SECOND) / 1000000);
                SSN_time += ((end_time-start_time)*1000000)/(COUNTS_PER_SECOND);

    }
}
