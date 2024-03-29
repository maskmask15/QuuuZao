#include <stdio.h>
#include "xparameters.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "display_demo.h"
#include "lwipopts.h"
#include "sleep.h"

#define packet_size 2560	// send size
/* defined by each RAW mode application */
int net_init(void);
int sendpic(const char *pic, int piclen, int fn,int sn);
int macInput(void);

extern u8 *pFrames[DISPLAY_NUM_FRAMES];
extern int WriteOneFrameEnd;
extern int frame_length_curr;
extern u32 targetPicHeader;
static int frame_num = 0;

int lwip_loop()
{

	net_init();


	while (1) {
		macInput();

		//printf("WriteOneFrameEnd:%d\ntargetPicHeader:%d\n",WriteOneFrameEnd,targetPicHeader);
		if((WriteOneFrameEnd >= 0) && (targetPicHeader != 0))
		{
			int index = WriteOneFrameEnd;
//			xil_printf("frma idx %d\n", index);
			int sn = 0;
			int cot;
			Xil_DCacheInvalidateRange((u32)pFrames[index], frame_length_curr);
			/* Separate camera 1 frame in package */
			for(int i=0;i<frame_length_curr;i+=packet_size)
			{
				if((i+packet_size)>frame_length_curr)
				{
					cot = frame_length_curr-i;
				}
				else
				{
					cot = packet_size;
				}
//				printf("start send pic\n");
				usleep(20);
				sendpic((const char *)pFrames[index]+i, cot, frame_num, sn++);
			}
			WriteOneFrameEnd = -1;
			frame_num++;
		}
		usleep(1000);
	}

	/* never reached */
	cleanup_platform();

	return 0;
}
