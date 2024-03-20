#include <stdio.h>
#include "xparameters.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "display_demo.h"
#include "lwipopts.h"
#include "sleep.h"
#include "nuc/file_op.h"

#define packet_size (640 * 4)	// send size
/* defined by each RAW mode application */
int net_init(void);
int sendpic(const char *pic, int piclen, int fn,int sn);
int macInput(void);

extern u8 *pFrames[DISPLAY_NUM_FRAMES];
extern int WriteOneFrameEnd;
extern int frame_length_curr;
extern u32 targetPicHeader;
static int frame_num = 0;

int nuc_init();
uint16_t *infrared_NUC(uint16_t *inputData);

//#define SD_FILE
//��������ˣ��ͻ��2000֡����SD��

int lwip_loop()
{

	net_init();
	nuc_init();
	while (1) {
		macInput();
		if((WriteOneFrameEnd >= 0) && (targetPicHeader != 0))
		{
			int index = WriteOneFrameEnd;
			//xil_printf("frma idx %d\n", index);
			int sn = 0;
			int cot;
			Xil_DCacheInvalidateRange((u32)pFrames[index], frame_length_curr);

			uint16_t *return_Data = infrared_NUC(pFrames[index]);

#ifdef SD_FILE

			char file_name[30];
			sprintf(file_name,"image_%d.raw",frame_num-100);


			if(frame_num >= 100 && frame_num < 2100){
				printf("start store frame:%d\n",frame_num);
				int stat = Write(file_name,return_Data,frame_length_curr,0);
				if(stat == XST_SUCCESS){
					printf("store frame %d sucess\n",frame_num);
				}else{
					printf("store frame %d fail!\n",frame_num);
				}
			}


#endif
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
				//printf("================\nframe_num=%d\nsn=%d\n===========\n",frame_num,sn);
				usleep(20);
//				if(sn < 4){
//					sendpic((const char *)pFrames[index] + i, cot, frame_num, sn++);
//				}else{
				sendpic((const char *)return_Data + i, cot, frame_num, sn++);
//				}
				usleep(20);
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