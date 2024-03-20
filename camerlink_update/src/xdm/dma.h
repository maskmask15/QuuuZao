#ifndef __DMA_H_
#define __DMA_H_

#include "xparameters_ps.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "xscugic.h"
#include "../mat/mat.h"
#include "../nuc/shareOpConfig.h"

#define START_REG 	0x10
#define CONTROL_REG	0x14
#define MAX_REG		0x18
#define MIN_REG		0x1c
#define STEP_REG	0x20
#define MEDIAN_REG	0x24
#define FINISH_REG	0x28
#define INTERRUPT   0x2C

#define RESET_TIMEOUT_COUNTER   10000

#define DMA_MASK_DE_NOISE_RX 0x02
#define DMA_MASK_DE_NOISE_TX 0x01
#define DMA_MASK_DE_NU_RX	 0x08
#define DMA_MASK_DE_NU_TX	 0x04
#define DMA_MASK_DE_DESTRIP_RX 0x20
#define DMA_MASK_DE_DESTRIP_TX 0x10

#define DeStrip_DMA_DEV_ID          XPAR_AXIDMA_2_DEVICE_ID
#define DeNU2_DMA_DEV_ID          	XPAR_AXIDMA_1_DEVICE_ID
#define DeNoise_DMA_DEV_ID			XPAR_AXIDMA_0_DEVICE_ID

//u32 DMA_Init(int device_id,u16 tx_intr_id, u16 rx_intr_id,void *tx_intr_handler,void *rx_intr_handler);

u32 DMA_Init(u32 dma_dev_id);

int xdma_rx_intr_handler(XAxiDma *axidma_inst);
int xdma_tx_intr_handler(XAxiDma *axidma_inst);

int wait_clear_intr(int mask, int dev_id, int Direction);

void GaussianBlur_s64_FPGA_while(const mat_s32 *src, mat_s32 *dst);

#endif
