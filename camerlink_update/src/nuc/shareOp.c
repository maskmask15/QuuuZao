#include "shareOpConfig.h"

void read_share_data(mat_s32 *shareData, int *addr, int *flag){
	int sum = shareData->cols * shareData->rows;
	for(int i = 0; i < sum; i++){
		shareData->data[i] = Xil_In32(addr + i);
	}
	Xil_Out32(flag, 0);
}

void write_share_data(mat_s32 *shareData, int *addr, int *flag){
	int sum = shareData->cols * shareData->rows;
	for(int i = 0; i < sum; i++){
		Xil_Out32(addr + i, shareData->data[i]);
	}
	Xil_Out32(flag, 1);
}

void read_weight(mat_s32 *weight_s32){
	read_share_data(weight_s32, WEIGHT_BASE, WEIGHT_FLAG_BASE);
}

void write_weight(mat_s32 *weight_s32){
	write_share_data(weight_s32, WEIGHT_BASE, WEIGHT_FLAG_BASE);
}

void read_nuc(mat_s32 *nuc_s32){
	read_share_data(nuc_s32, NUC_BASE, IMAGE_FLAG_BASE);
}

void write_nuc(mat_s32 *nuc_s32){
	write_share_data(nuc_s32, NUC_BASE, IMAGE_FLAG_BASE);
}


void init_share_addr(){
	Xil_SetTlbAttributes(SHARE_BASE_ADDR, 0x14de2);  //绂佺敤 0xfffffff0 鐨� Cache 灞炴��
	Xil_SetTlbAttributes(SHARE_BASE_ADDR + 0x100000, 0x14de2);
	Xil_SetTlbAttributes(SHARE_BASE_ADDR + 0x200000, 0x14de2);
	//Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);
}

//鍚姩cpu1
void StartCpu1(void){
    Xil_Out32(CPU1STARTADR, CPU1STARTMEM);  //鍚� CPU1STARTADR(0Xffffffff0)鍦板潃鍐欏叆 CPU1 鐨勮闂唴瀛樺熀鍦板潃
    dmb();    //绛夊緟鍐呭瓨鍐欏叆瀹屾垚(鍚屾)
    sev();    //閫氳繃"SEV"鎸囦护鍞ら啋 CPU1 骞惰烦杞嚦鐩稿簲鐨勭▼搴�
}
/**
 * 娓呯┖鎵�鏈夋爣蹇椾綅
 */
void init_flag(){
	set_sigmaN(0);
	set_weight(0);
	set_mod1(0);
	set_mod2(0);
	set_image(0);
}
