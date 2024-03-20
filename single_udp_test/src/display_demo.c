/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "display_demo.h"
#include <stdio.h>
#include "math.h"
#include <ctype.h>
#include <stdlib.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "vdma.h"
#include "sleep.h"
#include "xscugic.h"
#include "zynq_interrupt.h"
#include "xil_printf.h"
/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

/*
 * Display Driver structs
 */

XAxiVdma vdma_vout;
XScuGic XScuGicInstance;

XAxiVdma vdma_vin;

static int WriteError;

int wr_index = 0;
int rd_index = 0;
int frame_length_curr;
/*
 * Framebuffers for video data
 */
u8 frameBuf[DISPLAY_NUM_FRAMES][DEMO_MAX_FRAME] __attribute__ ((aligned(64)));
u8 *pFrames[DISPLAY_NUM_FRAMES]; //array of pointers to the frame buffers
int WriteOneFrameEnd = -1;

/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */
static void WriteCallBack(void *CallbackRef, u32 Mask);
static void WriteErrorCallBack(void *CallbackRef, u32 Mask);


void resetDma(){
	vdma_write_stop(&vdma_vin);
	XAxiVdma_IntrDisable(&vdma_vin, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_WRITE);
	XAxiVdma_Selftest(&vdma_vin);
}

void resetVideoFmt(int w, int h)
{

	int status = -1;
	frame_length_curr = 0;
	/* Stop vdma write process, disable vdma interrupt */
	/*
	 * Initial vdma write path, set call back function and register interrupt to GIC
	 */
	// 初始化VDMA写入路径,设置VDMA的相关参数
	status = vdma_write_init(XPAR_AXIVDMA_0_DEVICE_ID, &vdma_vin, w * 2, h, w * 2,
		(unsigned int)pFrames[0], (unsigned int)pFrames[1], (unsigned int)pFrames[2]);
	if(status != XST_SUCCESS)
		printf("=====vdma_write_init status:%d\n",status);

	// 设置VDMA的回调函数, 处理写
	status = XAxiVdma_SetCallBack(&vdma_vin, XAXIVDMA_HANDLER_GENERAL, WriteCallBack, (void *)&vdma_vin, XAXIVDMA_WRITE);
	if(status != XST_SUCCESS)
		printf("=====XAxiVdma_SetCallBack status:%d\n",status);

	// 设置VDMA的回调函数, 处理写错
	status = XAxiVdma_SetCallBack(&vdma_vin, XAXIVDMA_HANDLER_ERROR, WriteErrorCallBack, (void *)&vdma_vin, XAXIVDMA_WRITE);
	if(status != XST_SUCCESS)
		printf("=====XAxiVdma_SetCallBack status:%d\n",status);

	// 中断连接
	status = InterruptConnect(&XScuGicInstance, XPAR_FABRIC_AXI_VDMA_0_S2MM_INTROUT_INTR, XAxiVdma_WriteIntrHandler, (void *)&vdma_vin);
	if(status != XST_SUCCESS)
		printf("=====InterruptConnect status:%d\n",status);

	/* Start vdma write process, enable vdma interrupt */
	// 初始化使能
	XAxiVdma_IntrEnable(&vdma_vin, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_WRITE);

	usleep(1);
	// 开始VDMA写入
	status = vdma_write_start(&vdma_vin);
	if(status != XST_SUCCESS)
		printf("=====vdma_write_start status:%d\n",status);
	// 表示一帧图像的总字节数, 即图像数据的长度,宽 * 高 * 2, 因为每个像素是16位, 2字节
	frame_length_curr = w*h*2;
}


XScuGic* get_XScuGic()
{
	return &XScuGicInstance;
}

int lwip_loop();


int main(void)
{
//	net_init();
	int i = 0;
	for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
	{
		pFrames[i] = frameBuf[i];
	}
	/*
	 * Interrupt initialization
	 */
	//InterruptInit(XPAR_XSCUTIMER_0_DEVICE_ID, &XScuGicInstance);
	//InterruptInit(XPAR_SCUGIC_SINGLE_DEVICE_ID, &XScuGicInstance);
	InterruptInit(XPAR_SCUGIC_0_DEVICE_ID, &XScuGicInstance);
	/*
	 * Reconfiguration Sensor and VDMA
	 */
	resetVideoFmt(640, 512);
	/*
	 * Start lwip engine
	 */
	printf("\nlwip_loop\n");
	lwip_loop();


	while(1){


	}

	return 0;
}

/*****************************************************************************/
/*
 * Call back function for write channel
 *
 * This callback only clears the interrupts and updates the transfer status.
 *
 * @param	CallbackRef is the call back reference pointer
 * @param	Mask is the interrupt mask passed in from the driver
 *
 * @return	None
 *
 ******************************************************************************/
int WriteCallBackNum = 0;
static void WriteCallBack(void *CallbackRef, u32 Mask)
{
	int status = -1;
	printf("WriteCallBack:%d\n", WriteCallBackNum++);
	//若Mask中包含XAXIVDMA_IXR_FRMCNT_MASK, 代表帧计数器触发, 一帧图像传输完成
	if (Mask & XAXIVDMA_IXR_FRMCNT_MASK)
	{
		if(WriteOneFrameEnd >= 0)
		{
			xil_printf("WriteOneFrameEnd = %d\n", WriteOneFrameEnd);
			return;
		}
		int hold_rd = rd_index;
		if(wr_index == 2)
		{
			wr_index = 0;
			rd_index = 2;
		}
		else
		{
			rd_index = wr_index;
			wr_index++;
		}
		/* Set park pointer */
		status = XAxiVdma_StartParking((XAxiVdma*)CallbackRef, wr_index, XAXIVDMA_WRITE);		// 禁止更改某一帧缓存地址

		if(status != XST_SUCCESS)
				printf("=====XAxiVdma_StartParking status:%d\n",status);

		WriteOneFrameEnd = hold_rd;													// 值表示哪一个写buf结束
		xil_printf("WriteOneFrameEnd = %d ---- wr_index = %d\n", WriteOneFrameEnd,wr_index);
	}
}


/*****************************************************************************/
/*
 * Call back function for write channel error interrupt
 *
 * @param	CallbackRef is the call back reference pointer
 * @param	Mask is the interrupt mask passed in from the driver
 *
 * @return	None
 *
 ******************************************************************************/
int WriteErrorCallBackNum = 0;
static void WriteErrorCallBack(void *CallbackRef, u32 Mask)
{
	printf("WriteErrorCallBack:%d\n", WriteErrorCallBackNum++);
	if (Mask & XAXIVDMA_IXR_ERROR_MASK) {
		WriteError += 1;
	}
//	resetDma();
//	resetVideoFmt(640, 512);
}


