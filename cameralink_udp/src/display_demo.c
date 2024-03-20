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

	frame_length_curr = 0;
	/* Stop vdma write process, disable vdma interrupt */
	/*
	 * Initial vdma write path, set call back function and register interrupt to GIC
	 */
	vdma_write_init(XPAR_AXIVDMA_0_DEVICE_ID, &vdma_vin, w * 2, h, w * 2,
		(unsigned int)pFrames[0], (unsigned int)pFrames[1], (unsigned int)pFrames[2]);

	XAxiVdma_SetCallBack(&vdma_vin, XAXIVDMA_HANDLER_GENERAL, WriteCallBack, (void *)&vdma_vin, XAXIVDMA_WRITE);

	XAxiVdma_SetCallBack(&vdma_vin, XAXIVDMA_HANDLER_ERROR, WriteErrorCallBack, (void *)&vdma_vin, XAXIVDMA_WRITE);

	InterruptConnect(&XScuGicInstance, XPAR_FABRIC_AXI_VDMA_0_S2MM_INTROUT_INTR, XAxiVdma_WriteIntrHandler, (void *)&vdma_vin);

	/* Start vdma write process, enable vdma interrupt */
	XAxiVdma_IntrEnable(&vdma_vin, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_WRITE);
	usleep(1);
	vdma_write_start(&vdma_vin);
	frame_length_curr = w*h*2;
}

XScuGic* get_XScuGic()
{
	return &XScuGicInstance;
}

int lwip_loop();


int main(void)
{

//	int wait_seconds = 20;
//	while(wait_seconds--){
//		printf("wait %d seconds\n",wait_seconds);
//		usleep(1000);
//	}
	//init_share_addr();
	int i = 0;
	for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
	{
		pFrames[i] = frameBuf[i];
	}
	/*
	 * Interrupt initialization
	 */
//	InterruptInit(XPAR_XSCUTIMER_0_DEVICE_ID, &XScuGicInstance);
	InterruptInit(XPAR_SCUGIC_0_DEVICE_ID, &XScuGicInstance);

	/*
	 * Reconfiguration Sensor and VDMA
	 */
	resetVideoFmt(640, 512);
	/*
	 * Start lwip engine
	 */
	printf("lwip_loop\n");
	lwip_loop();
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
//	printf("WriteCallBack:%d\n", WriteCallBackNum++);
	if (Mask & XAXIVDMA_IXR_FRMCNT_MASK)
	{
		if(WriteOneFrameEnd >= 0)
		{
//			xil_printf("WriteOneFrameEnd = %d\n", WriteOneFrameEnd);
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
		XAxiVdma_StartParking((XAxiVdma*)CallbackRef, wr_index, XAXIVDMA_WRITE);		// 禁止更改某一帧缓存地址
		WriteOneFrameEnd = hold_rd;													// 值表示哪一个写buf结束
		//xil_printf("WriteOneFrameEnd = %d\n", WriteOneFrameEnd);

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


