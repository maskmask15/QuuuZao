
#ifndef _SHARE_OP_H_
#define _SHARE_OP_H_

#include <stdio.h>
#include "../platform.h"
#include "xil_printf.h"
#include "xscugic.h"
#include "utils.h"

//IMAGE_FLAG和WEIGHT_FLAG标志为0时可以写，标志为1时可以读，否则不能读写
/**
 * 内存划分
 * 名称------起始地址-----长度
 * 共享地址-----0x100000----------0x400000
 * cpu0参数更新 -----0x400000-----0x10000000//256MB
 * cpu1参数更新 -----0x10400000-----0x10000000//256MB
 * FRAME_ADDR_BASE----0x20400000-----0x10000000
 */
#define SHARE_BASE_ADDR 0x70000000

#define MOD1_BASE  	     	SHARE_BASE_ADDR + 0x0
#define MOD2_BASE  	     	SHARE_BASE_ADDR + 0x4
#define IMAGE_FLAG_BASE  	SHARE_BASE_ADDR + 0x8
#define WEIGHT_FLAG_BASE  	SHARE_BASE_ADDR + 0xc
#define WEIGHT_sigmaN		SHARE_BASE_ADDR + 0x10

#define NUC_BASE  	     	SHARE_BASE_ADDR + 0x20
#define WEIGHT_BASE  	    SHARE_BASE_ADDR + (640 * 512 * 4 + 0x20)

#define DENOISE_BASE_ADDR	INSTRUCTION_BASE_ADDR
#define INSTRUCTION_BASE_ADDR   0x80100000
//地址信息
#define DENOISE_OFFSET_N 0x8
#define DENOISE_OFFSET_MEAN 0xc

#define sev() __asm__("sev")
#define CPU1STARTADR 0xfffffff0   //存放 CPU1 应用起始地址的地址
#define CPU1STARTMEM 0x10000000   //CPU1 应用起始地址
//函数定义
void StartCpu1();
void init_share_addr();
void write_nuc(mat_s32 *nuc_s32);
void read_nuc(mat_s32 *nuc_s32);
void write_weight(mat_s32 *weight_s32);
void read_weight(mat_s32 *weight_s32);

static INLINE void set_mod1(int set){
	Xil_Out32((UINTPTR)MOD1_BASE, set);
}

static INLINE int get_mod1(){
	return Xil_In32((UINTPTR)MOD1_BASE);
}

static INLINE void set_mod2(int set){
	Xil_Out32((UINTPTR)MOD2_BASE, set);
}

static INLINE int get_mod2(){
	return Xil_In32((UINTPTR)MOD2_BASE);
}

static INLINE void set_image(int set){
	Xil_Out32((UINTPTR)IMAGE_FLAG_BASE, set);
}

static INLINE int get_image(){
	return Xil_In32((UINTPTR)IMAGE_FLAG_BASE);
}

static INLINE void set_weight(int set){
	Xil_Out32((UINTPTR)WEIGHT_FLAG_BASE, set);
}

static INLINE int get_weight(){
	return Xil_In32((UINTPTR)WEIGHT_FLAG_BASE);
}

static INLINE void set_sigmaN(int set){
	Xil_Out32((UINTPTR)WEIGHT_sigmaN, set);
}

static INLINE int get_sigmaN(){
	return Xil_In32((UINTPTR)WEIGHT_sigmaN);
}

void init_flag();

#endif

