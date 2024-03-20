//
// Created by ozgaga on 2022/11/2.
//

#include "IRNUC.h"
#include "InitParam.h"
#include "../mat/matCal_s32.h"
#include <stdio.h>
#include <sys/time.h>
#include <xil_io.h>
#include "xtime_l.h"
#include "shareOpConfig.h"

extern struct GlobalNUC globalNuc;
extern struct GlobalVar globalVar;

void preProcessing(mat_s32 *dst){
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] = (((-globalVar.alphaShading * globalVar.Shading->data[i]) >> (ShShift)) + 
                        ((-globalVar.alphaShading4 * globalVar.Shading4->data[i]) >> (Sh4Shift)) +
                        ((-globalVar.alphaFP * globalVar.FP->data[i]) >> (fpShift)) -
                        globalVar.Strip2->data[i] - 
                        (globalVar.NU->data[i] >> nuShift));
    }
}

void updateWeight(mat_s32 *dst){
    preProcessing(dst);//鐠侊紕鐣婚弶鍐櫢
    while(get_weight() == 1);//缁涘绶熸稉濠佺濞嗏剝娼堥柌宥堫潶鐠囪褰�
	write_weight(dst);//閸愭瑥鍙嗛弬鎵畱閺夊啴鍣�
}

static void deShading(mat_s32 *Shading, int alphaShading, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] += ((alphaShading * Shading->data[i]) >> (ShShift));
    }
}

// Shading4<2^16闂備焦瀵х粙鎴﹀嫉椤掑嫬妫樺ù锝囨嚀缁剁偤鏌ㄩ悤鍌涘2^16, alphaShading4闂備胶灏ㄩ幏鐑芥煙椤栧棔妞掑ù锟�2^5
static void deShading4(mat_s32 *Shading4, int alphaShading4, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] += ((alphaShading4 * Shading4->data[i]) >> (Sh4Shift));
    }
}

static void deFP(mat_s32 *FP, int alphaFP, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] += ((alphaFP * FP->data[i]) >> (fpShift));
    }
}

static void deStrip(mat_s32 *Strip, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
//        dst->data[i] = dst->data[i] - Strip->data[i];
        dst->data[i] -= Strip->data[i];
    }
}

static void deNU(mat_s32 *NU, mat_s32 *dst) {
    for (int i = 0; i < dst->rows * dst->cols; i++) {
        dst->data[i] -= NU->data[i] >> nuShift;
    }
}

void timeStap(struct timeval end_time, struct timeval start_time);

/**
 * 閺囧瓨鏌婇弶鍐櫢閻ㄥ嫪绗佹稉顏呮閺堬拷
 * 1閵嗕礁鍨垫慨瀣
 *
 */
void SSN(mat_s32 *NUC_s32, struct SSN_param *ssnParam, mat_s32 *weight_s32, int mod1, int mod2) {//1,11,21,31,41闂備礁鎲＄敮鎺懳涘Δ鍛槬闁糕剝绋戦崡鎶芥倵濞戞鎴︽偂閿熶粙姊洪幐搴ｂ槈闁兼椿鍨跺畷锝夋倷椤掑倵鏋栭梺璺ㄥ櫐閹凤拷1闂備礁鎲＄划锝夊箯閿燂拷31闂備礁鎼ú銈夋偤閵娾晛钃熷┑鐘叉搐閻鏌涚仦鍓р檨婵炲牞鎷�


    static int first_DeNU = 0;

    struct timeval end_time;
    struct timeval start_time;

    int whenMod1 =1;
    int whenMod2= 1;
    printf("SSN: mod1:%d,mod2:%d\n",mod1,mod2);

    int ORG_mean = matAvg_s32(NUC_s32);
    matSubS_s32(NUC_s32, ORG_mean, NUC_s32);

    if(mod1 == whenMod1){
        matCopy_s32(NUC_s32, globalNuc.ORG);
        DeShading(&ssnParam->Shading_Scale);
    }

    deShading(globalVar.Shading, -globalVar.alphaShading, NUC_s32);
    deShading4(globalVar.Shading4, -globalVar.alphaShading4, NUC_s32);

    if(mod1 == whenMod1){
        matCopy_s32(NUC_s32, globalNuc.NUC_DeShading);
        DeFPN_s32();
    }

    deFP(globalVar.FP, -globalVar.alphaFP, NUC_s32);
    if(mod1 == whenMod1){
        matCopy_s32(NUC_s32, globalNuc.NUC_DeFPN);
        XTime start_time, end_time;
        XTime_GetTime(&start_time);
        DeStrip2_s32(&ssnParam->Strip_kSize);
        XTime_GetTime(&end_time);
		u32 time_r = 0;
		time_r = ((end_time-start_time)*1000000)/(COUNTS_PER_SECOND);
		printf("DeStrip2_s32 time: %.6lf s\n", (double)time_r / 1000000);
    }
    deStrip(globalVar.Strip2, NUC_s32);

    updateWeight(weight_s32);

    if (mod2 ==whenMod2) {
        if (first_DeNU == 0) {//闂備礁鎲￠悷顖涚濞嗘垶顫曞ù鍏兼綑閹瑰爼鏌曟繛鍨姎闁诲繈鍎遍埞鎴︻敊绾板崬鍓伴柣搴＄仛閻擄繝骞嗛崘顔肩妞ゆ帊绶″Σ顖炴⒑缁嬪尅鏀婚柣鐔村妿閹广垹顓奸崶銊ヤ粧闂佽法鍣﹂幏锟�
            first_DeNU = 1;
            int len = NUC_s32->rows * NUC_s32->cols;
            int low_val = (int) ((float) len * 0.2f);
            int up_val = len - low_val;
            mat_s32 *nuc_tmp = createMat_s32(NUC_s32->rows, NUC_s32->cols);
            matCopy_s32(NUC_s32, nuc_tmp);
            //濠电偞鍨堕幐鎼佹晝閿濆洨绀婇柛娑欐綑鐎氬鏌涘┑鍡楊伀闁哄绮欓弻锝咁煥閹邦厼顏�
            low_val = findKthNums_s32(nuc_tmp->data, 0, len - 1, low_val);//闂傚倷绶￠崑鍛暜閹烘梻绀婂┑鐘插娑撳秵淇婇妶鍕厡闁靛棗锕﹂敓钘夌仛濠㈡﹢鎮ч幘鎵挎帡鏁冮崒姘鳖槷闂佺粯眉缁鳖噣骞忛悜鑺ユ櫢闁跨噦鎷�
            up_val = findKthNums_s32(nuc_tmp->data, 0, len - 1, up_val);
            matRelease_s32(&nuc_tmp);
            globalVar.D = up_val - low_val;
        }
		for (int i = 0; i < NUC_s32->rows * NUC_s32->cols; i++) {
			globalNuc.NUC_DeStrip->data[i] = NUC_s32->data[i] << nuShift;
		}
		DeNU_s32(&ssnParam->NU_kSize);
    }

    deNU(globalVar.NU, NUC_s32);

    //闁哄洤鐡ㄩ弻濡攊gmaN_square婵☆垪锟借櫕鍋�
    if (ssnParam->Noise_kSize != 0 && mod1) DeNoise(NUC_s32, ssnParam->Noise_kSize, mod1);

    updateWeight(weight_s32);
}