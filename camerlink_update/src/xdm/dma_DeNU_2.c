#include "dma_DeNU_2.h"
#include "dma_config.h"
//XAxiDma* get_Axidma(int device_id){
//	switch(device_id){
//		case DeNU2_DMA_DEV_ID	:return &deNU2_axidma;
//		case DeStrip_DMA_DEV_ID	:return &deStrip_axidma;
//	}
//	return NULL;
//}

XScuGic* get_XScuGic(){
	return &dma_intc;
}

void set_DeNU2_tx_done(){
	DeNU2_tx_done = 1;
}
void set_DeNU2_rx_done(){
	DeNU2_rx_done = 1;
}
void set_DeNU2_error(){
	DeNU2_error = 1;
}

static void Gaus_Init(){
	DeNU2_tx_done = 0;
	DeNU2_rx_done = 0;
	DeNU2_error = 0;
}

void set_DeStrip_tx_done(){
	DeStrip_tx_done = 1;
}
void set_DeStrip_rx_done(){
	DeStrip_rx_done = 1;
}
void set_DeStrip_error(){
	DeStrip_error = 1;
}

int get_DeStrip_tx_done(){
	return DeStrip_tx_done;
}
int get_DeStrip_rx_done(){
	return DeStrip_rx_done;
}
int get_DeStrip_error(){
	return DeStrip_error;
}


void DeStrip_RT_Init(){
	DeStrip_tx_done = 0;
	DeStrip_rx_done = 0;
	DeStrip_error = 0;
}


void DeNU2_tx_intr_handler(void *callback)
{
    int timeout;
    u32 irq_status;
    XAxiDma *axidma_inst = (XAxiDma *) callback;
    irq_status = XAxiDma_IntrGetIrq(axidma_inst, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_IntrAckIrq(axidma_inst, irq_status, XAXIDMA_DMA_TO_DEVICE);
    if ((irq_status & XAXIDMA_IRQ_ERROR_MASK)) {
    	set_DeNU2_error();
        XAxiDma_Reset(axidma_inst);
        timeout = RESET_TIMEOUT_COUNTER;
        while (timeout) {
            if (XAxiDma_ResetIsDone(axidma_inst))
                break;
            timeout -= 1;
        }
        return;
    }
    if ((irq_status & XAXIDMA_IRQ_IOC_MASK))
    	set_DeNU2_tx_done();
}

void DeNU2_rx_intr_handler(void *callback)
{
    u32 irq_status;
    int timeout;
    XAxiDma *axidma_inst = (XAxiDma *) callback;
    irq_status = XAxiDma_IntrGetIrq(axidma_inst, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrAckIrq(axidma_inst, irq_status, XAXIDMA_DEVICE_TO_DMA);
    if ((irq_status & XAXIDMA_IRQ_ERROR_MASK)) {
    	set_DeNU2_error();
        XAxiDma_Reset(axidma_inst);
        timeout = RESET_TIMEOUT_COUNTER;
        while (timeout) {
            if (XAxiDma_ResetIsDone(axidma_inst))
                break;
            timeout -= 1;
        }
        return;
    }
    if ((irq_status & XAXIDMA_IRQ_IOC_MASK))
    	set_DeNU2_rx_done();
}

void DeStrip_tx_intr_handler(void *callback)
{
    int timeout;
    u32 irq_status;
    XAxiDma *axidma_inst = (XAxiDma *) callback;
    irq_status = XAxiDma_IntrGetIrq(axidma_inst, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_IntrAckIrq(axidma_inst, irq_status, XAXIDMA_DMA_TO_DEVICE);
    if ((irq_status & XAXIDMA_IRQ_ERROR_MASK)) {
    	set_DeStrip_error();
        XAxiDma_Reset(axidma_inst);
        timeout = RESET_TIMEOUT_COUNTER;
        while (timeout) {
            if (XAxiDma_ResetIsDone(axidma_inst))
                break;
            timeout -= 1;
        }
        return;
    }
    if ((irq_status & XAXIDMA_IRQ_IOC_MASK))
    	set_DeStrip_tx_done();
}

void DeStrip_rx_intr_handler(void *callback)
{
    u32 irq_status;
    int timeout;
    XAxiDma *axidma_inst = (XAxiDma *) callback;
    irq_status = XAxiDma_IntrGetIrq(axidma_inst, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrAckIrq(axidma_inst, irq_status, XAXIDMA_DEVICE_TO_DMA);
    if ((irq_status & XAXIDMA_IRQ_ERROR_MASK)) {
    	set_DeStrip_error();
        XAxiDma_Reset(axidma_inst);
        timeout = RESET_TIMEOUT_COUNTER;
        while (timeout) {
            if (XAxiDma_ResetIsDone(axidma_inst))
                break;
            timeout -= 1;
        }
        return;
    }
    if ((irq_status & XAXIDMA_IRQ_IOC_MASK))
    	set_DeStrip_rx_done();
}



//static int setup_intr_system()
//{
//    int status;
//    XScuGic_Config *intc_config;
//    XScuGic* int_ins_ptr = get_XScuGic();
//
//    intc_config = XScuGic_LookupConfig(INTC_DEVICE_ID);
//    if (NULL == intc_config) {
//        return XST_FAILURE;
//    }
//    status = XScuGic_CfgInitialize(int_ins_ptr, intc_config,
//            intc_config->CpuBaseAddress);
//    if (status != XST_SUCCESS) {
//        return XST_FAILURE;
//    }
//
//
//    XScuGic_SetPriorityTriggerType(int_ins_ptr, DeNU2_RX_INTR_ID, 0xA0, 0x3);
//    XScuGic_SetPriorityTriggerType(int_ins_ptr, DeNU2_TX_INTR_ID, 0xA0, 0x3);
//    XScuGic_SetPriorityTriggerType(int_ins_ptr, DeStrip_RX_INTR_ID, 0xA0, 0x3);
//    XScuGic_SetPriorityTriggerType(int_ins_ptr, DeStrip_TX_INTR_ID, 0xA0, 0x3);
//
//    status = XScuGic_Connect(int_ins_ptr, DeNU2_TX_INTR_ID,
//            (Xil_InterruptHandler) DeNU2_tx_intr_handler, get_Axidma(DeNU2_DMA_DEV_ID));
//    if (status != XST_SUCCESS) {
//        return status;
//    }
//    status = XScuGic_Connect(int_ins_ptr, DeNU2_RX_INTR_ID,
//            (Xil_InterruptHandler) DeNU2_rx_intr_handler, get_Axidma(DeNU2_DMA_DEV_ID));
//    if (status != XST_SUCCESS) {
//        return status;
//    }
//
//    status = XScuGic_Connect(int_ins_ptr, DeStrip_TX_INTR_ID,
//			(Xil_InterruptHandler) DeStrip_tx_intr_handler, get_Axidma(DeStrip_DMA_DEV_ID));
//	if (status != XST_SUCCESS) {
//		return status;
//	}
//	status = XScuGic_Connect(int_ins_ptr, DeStrip_RX_INTR_ID,
//			(Xil_InterruptHandler) DeStrip_rx_intr_handler, get_Axidma(DeStrip_DMA_DEV_ID));
//	if (status != XST_SUCCESS) {
//		return status;
//	}
//
//	XScuGic_InterruptMaptoCpu(int_ins_ptr, 0x00, DeNU2_TX_INTR_ID);
//	XScuGic_InterruptMaptoCpu(int_ins_ptr, 0x00, DeNU2_RX_INTR_ID);
//	XScuGic_InterruptMaptoCpu(int_ins_ptr, 0x00, DeStrip_TX_INTR_ID);
//	XScuGic_InterruptMaptoCpu(int_ins_ptr, 0x00, DeStrip_RX_INTR_ID);
//
//    XScuGic_Enable(int_ins_ptr, DeNU2_TX_INTR_ID);
//    XScuGic_Enable(int_ins_ptr, DeNU2_RX_INTR_ID);
//    XScuGic_Enable(int_ins_ptr, DeStrip_TX_INTR_ID);
//    XScuGic_Enable(int_ins_ptr, DeStrip_RX_INTR_ID);
//
//
//    Xil_ExceptionInit();
//    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
//            (Xil_ExceptionHandler) XScuGic_InterruptHandler,
//            (void *) int_ins_ptr);
//    Xil_ExceptionEnable();
//
////    XAxiDma_IntrDisable(axidma_ptr, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
////    XAxiDma_IntrDisable(axidma_ptr, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
//    XAxiDma_IntrEnable(get_Axidma(DeNU2_DMA_DEV_ID), XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
//    XAxiDma_IntrEnable(get_Axidma(DeNU2_DMA_DEV_ID), XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
//
//    XAxiDma_IntrEnable(get_Axidma(DeStrip_DMA_DEV_ID), XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
//    XAxiDma_IntrEnable(get_Axidma(DeStrip_DMA_DEV_ID), XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
//    return XST_SUCCESS;
//}



//u32 DMA_Init(int device_id,u16 tx_intr_id, u16 rx_intr_id,void *tx_intr_handler,void *rx_intr_handler)
//{
//	int status;
//	XAxiDma_Config *config;
//	config = XAxiDma_LookupConfig(DeNU2_DMA_DEV_ID);
//	status = XAxiDma_CfgInitialize(get_Axidma(DeNU2_DMA_DEV_ID), config);
//
//	config = XAxiDma_LookupConfig(DeStrip_DMA_DEV_ID);
//	status = XAxiDma_CfgInitialize(get_Axidma(DeStrip_DMA_DEV_ID), config);
//
//	status = setup_intr_system();
//	//status = setup_intr_system(get_XScuGic(DeStrip_DMA_DEV_ID), get_Axidma(DeStrip_DMA_DEV_ID), DeStrip_TX_INTR_ID, DeStrip_RX_INTR_ID,DeStrip_tx_intr_handler,DeStrip_rx_intr_handler);
//	//status = setup_intr_system(&deNU2_intc, &deNU2_axidma, tx_intr_id, rx_intr_id,tx_intr_handler,rx_intr_handler);
//
//    return XST_SUCCESS ;
//}


void GaussianBlur_s64_FPGA(const mat_s32 *src, mat_s32 *dst){

	Gaus_Init();
	int lrngth = src->cols * src->rows * 4;
	int status;

	Xil_DCacheFlushRange((UINTPTR)  (src->data), lrngth);   //刷新Data Cache
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

	while (!(DeNU2_tx_done && DeNU2_rx_done) && !DeNU2_error){};

	if (DeNU2_error) {
		xil_printf("Failed test transmit%s done, "
				"receive%s done\r\n", DeNU2_tx_done ? "" : " not",
						DeNU2_rx_done ? "" : " not");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
