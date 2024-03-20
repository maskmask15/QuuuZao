#ifndef __DMA_DENU_2_H_
#define __DMA_DENU_2_H_

#include "xparameters_ps.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "xscugic.h"
#include "../mat/mat.h"

#define START_REG 	0x10
#define CONTROL_REG	0x14
#define MAX_REG		0x18
#define MIN_REG		0x1c
#define STEP_REG	0x20
#define MEDIAN_REG	0x24
#define FINISH_REG	0x28
#define INTERRUPT   0x2C


#define DeNU2_DMA_DEV_ID          XPAR_AXIDMA_1_DEVICE_ID
//#define DeNU2_RX_INTR_ID          XPAR_FABRIC_AXIDMA_1_S2MM_INTROUT_VEC_ID
//#define DeNU2_TX_INTR_ID          XPAR_FABRIC_AXIDMA_1_MM2S_INTROUT_VEC_ID

#define DeStrip_DMA_DEV_ID          XPAR_AXIDMA_2_DEVICE_ID
//#define DeStrip_RX_INTR_ID          XPAR_FABRIC_AXIDMA_2_S2MM_INTROUT_VEC_ID
//#define DeStrip_TX_INTR_ID          XPAR_FABRIC_AXIDMA_2_MM2S_INTROUT_VEC_ID

#define INTC_DEVICE_ID      XPAR_SCUGIC_SINGLE_DEVICE_ID
#define RESET_TIMEOUT_COUNTER   10000


u32 DMA_Init(int device_id,u16 tx_intr_id, u16 rx_intr_id,void *tx_intr_handler,void *rx_intr_handler);

XAxiDma* get_Axidma(int device_id);
XScuGic* get_XScuGic();


void DeNU2_rx_intr_handler(void *callback);
void DeNU2_tx_intr_handler(void *callback);
void DeStrip_rx_intr_handler(void *callback);
void DeStrip_tx_intr_handler(void *callback);
void GaussianBlur_s64_FPGA(const mat_s32 *src, mat_s32 *dst);

int get_DeStrip_tx_done();
int get_DeStrip_rx_done();
int get_DeStrip_error();
void DeStrip_RT_Init();

int get_DeNU2_tx_done();
int get_DeNU2_rx_done();
int get_DeNU2_error();

void waitDeStripRxInr();
void waitDeStripTxInr();
void waitDeStripTxRxInr();

int xdma_rx_intr_handler(XAxiDma *axidma_inst);
int xdma_tx_intr_handler(XAxiDma *axidma_inst);

#endif
