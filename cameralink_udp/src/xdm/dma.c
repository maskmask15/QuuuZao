#include "dma.h"

static XAxiDma deNU2_axidma;
static XAxiDma deStrip_axidma;
static XAxiDma deNoise_axidma;

void delay(int n){
	while(n--);
}

XAxiDma* get_Axidma(int device_id){
	switch(device_id){
		case DeNU2_DMA_DEV_ID	:return &deNU2_axidma;
		case DeStrip_DMA_DEV_ID	:return &deStrip_axidma;
		case DeNoise_DMA_DEV_ID :return &deNoise_axidma;
	}
	return NULL;
}

u32 DMA_Init(u32 dma_dev_id)
{
	int status;
	XAxiDma_Config *config;
	config = XAxiDma_LookupConfig(dma_dev_id);
	status = XAxiDma_CfgInitialize(get_Axidma(dma_dev_id), config);
    XAxiDma_IntrEnable(get_Axidma(dma_dev_id), XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_IntrEnable(get_Axidma(dma_dev_id), XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    return status ;
}

int xdma_intr_handler(XAxiDma *axidma_inst, int Direction)
{
    int timeout;
    u32 irq_status;
    irq_status = XAxiDma_IntrGetIrq(axidma_inst, Direction);
    XAxiDma_IntrAckIrq(axidma_inst, irq_status, Direction);
    if ((irq_status & XAXIDMA_IRQ_ERROR_MASK)) {
        XAxiDma_Reset(axidma_inst);
        timeout = RESET_TIMEOUT_COUNTER;
        while (timeout) {
            if (XAxiDma_ResetIsDone(axidma_inst))
                break;
            timeout -= 1;
        }
        return -1;
    }
    if ((irq_status & XAXIDMA_IRQ_IOC_MASK))
    	return 0;
    else return 1;
}

int wait_clear_intr(int mask, int dev_id, int Direction){
	int status;
	while(1){
		delay(4000);
		int intr = Xil_In32(INSTRUCTION_BASE_ADDR + INTERRUPT) && mask;
		//printf("%x intr %d\n",mask, intr);
		if(intr != 0){
			status = xdma_intr_handler(get_Axidma(dev_id), Direction);
			break;
		}
	}
	if(status != 0){
		xil_printf("Failed test transmit %d", mask);
	}
	return status;
}

void GaussianBlur_s64_FPGA_while(const mat_s32 *src, mat_s32 *dst){

	int lrngth = src->cols * src->rows * 4;
	int status;

	Xil_DCacheFlushRange((UINTPTR)  (src->data), lrngth);   //鍒锋柊Data Cache
	status = XAxiDma_SimpleTransfer(get_Axidma(DeNU2_DMA_DEV_ID), (UINTPTR) src->data,
			lrngth, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = XAxiDma_SimpleTransfer(get_Axidma(DeNU2_DMA_DEV_ID), (UINTPTR) dst->data,
			lrngth, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Xil_DCacheInvalidateRange((UINTPTR)(dst->data), lrngth);
//	Xil_DCacheFlushRange((UINTPTR)(dst->data), lrngth); cacheUpdate
	//首先获得传输的结果，然后获得传输回的结果
	wait_clear_intr(DMA_MASK_DE_NU_TX, DeNU2_DMA_DEV_ID, XAXIDMA_DMA_TO_DEVICE);
	wait_clear_intr(DMA_MASK_DE_NU_RX, DeNU2_DMA_DEV_ID, XAXIDMA_DEVICE_TO_DMA);

	return XST_SUCCESS;
}
