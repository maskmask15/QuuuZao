/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/udp.h"
#include "lwip/inet.h"
#include "xil_printf.h"
#include "platform.h"

/* server port to listen on/connect to */
#define UDP_CONN_PORT 5001
#define TX_HEAD 0x00020002

struct ip4_addr target_addr;	// 记录地址
u32 targetPicHeader = 0;		// 使能
static struct udp_pcb *pcb = NULL;

/*
 * Send frame data with udp
 *
 * @param pic is frame pointer will be send
 * @param piclen is frame length in one package
 * @param sn is Serial number for each ethernet package
 *
 * @format  Header(8bytes)+data(piclen)
 * @HeaderFormat {ReceivedFirstData|0x01, 0x00, 0x02, 0x00, 0x02, SerialNumber(3bytes)}
 */

static int pbuf_flag = 0;
static int count = 0;
int stat = -1;
int status = -1;
struct pbuf pbuf_arr[1024];
int pbuf_index = 0;
struct pbuf *q;

int sendpic(const char *pic, int piclen, int fn, int sn)
{

	if(targetPicHeader == 0)
	{
		return -1;
	}
//#define ARR_TEST
#ifdef ARR_TEST
	q = pbuf_arr + pbuf_index;


	q -> payload = (void *)malloc(100 + piclen);
	q -> next = NULL;
#endif

#ifndef ARR_TEST
	q = pbuf_alloc(PBUF_TRANSPORT, 100 + piclen, PBUF_POOL);
#endif

	if(!q)
	{
		xil_printf("pbuf_alloc %d fail\n\r", piclen+100);
		return -3;
	}else{
		pbuf_flag++;
	}

//	if(!(count % 3000)){
//			printf("pbuf_flag :%d\n",pbuf_flag);
//			printf("tot_len :%d,len:%d,ref :%d\n",q->tot_len,q->len,q->ref);
//		}

	(*(u32 *)q->payload) = TX_HEAD;
	(*((u32 *)q->payload + 1)) = fn;
	(*((u32 *)q->payload + 2)) = sn;
	//printf("%x %x\n", q->payload, q->payload + 12);
	memcpy((u32 *)q->payload + 3, pic, piclen);
	q->len = q->tot_len = 12 + piclen;
	status = udp_sendto(pcb, q, &target_addr, UDP_CONN_PORT);
	//xil_printf("Send AC %d , %d %p %p, %d\n\r", fn ,sn, &fn, &sn, status);



#ifdef ARR_TEST
	free(q->payload);
	pbuf_index = pbuf_index < 1023?pbuf_index + 1:0;
#endif

#ifndef ARR_TEST
	stat = pbuf_free(q);
#endif
	if(stat == 0){
		count ++;
	}else{
		pbuf_flag -= stat;
	}

	return 0;
}

void print_app_header()
{
	xil_printf("\n\r\n\r-----lwIP UDP DEMO ------\n\r");
	xil_printf("UDP packets sent to port 8080\n\r");
}

/*
 * Call back funtion for udp receiver
 *
 * @param arg is argument
 * @param pcb is udp pcb pointer
 * @param p_rx is pbuf pointer
 * @param addr is IP address
 * @param port is udp port
 *
 */
void udp_recive(void *arg, struct udp_pcb *pcb, struct pbuf *p_rx, const ip_addr_t *addr, u16_t port)
{
	char *pData;
	if(p_rx != NULL)
	{
		pData = (char *)p_rx->payload;
		xil_printf("%s\n",pData);

		if(p_rx->len >= 5)
		{
			printf("%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",pData[0],pData[1],pData[2],pData[3],pData[4],pData[5],pData[6],pData[7]);
			/* Check received data if they are query command from PC, format as random&0xFE, 0x00, 0x02, 0x00, 0x01 */
			//包头判断 0x00020001 zynq接收
			if(((pData[0]) == 0x01) &&
					(pData[1] == 0x00) &&
					((pData[2] == 0x02)&&
					(pData[3]) == 0x00))
			{
				/* Reply data, 16 bytes, random|0x01, 0x00, 0x02, 0x00, 0x01, mac_address, ip_address, 0x02 */
				targetPicHeader = pData[4];
				memcpy(&target_addr, addr, sizeof(target_addr));
			}
		}
		pbuf_free(p_rx);
	}
}

void start_application(void)
{
	err_t err;

	/* Create Server PCB */
	pcb = udp_new();
	if (!pcb) {
		xil_printf("UDP server: Error creating PCB. Out of Memory\r\n");
		return;
	}

	err = udp_bind(pcb, IP_ADDR_ANY, UDP_CONN_PORT);
	if (err != ERR_OK) {
		xil_printf("UDP server: Unable to bind to port");
		xil_printf(" %d: err = %d\r\n", UDP_CONN_PORT, err);
		udp_remove(pcb);
		return;
	}

	/* specify callback to use for incoming connections */
	udp_recv(pcb, (udp_recv_fn)udp_recive, NULL);

	return;
}
