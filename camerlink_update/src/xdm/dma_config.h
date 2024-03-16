

#ifndef _DMA_CONFIG_H_
#define _DMA_CONFIG_H_

volatile int DeNU2_tx_done;
volatile int DeNU2_rx_done;
volatile int DeNU2_error;

volatile int DeStrip_tx_done;
volatile int DeStrip_rx_done;
volatile int DeStrip_error;


static XAxiDma deNU2_axidma;
static XScuGic dma_intc;

static XAxiDma deStrip_axidma;

static void set_DeNU2_tx_done();
static void set_DeNU2_rx_done();
static void set_DeNU2_error();
static void Gaus_Init();

static void set_DeStrip_tx_done();
static void set_DeStrip_rx_done();
static void set_DeStrip_error();

static int  setup_intr_system();

#endif
