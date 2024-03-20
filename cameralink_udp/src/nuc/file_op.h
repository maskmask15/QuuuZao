/*
 * file_op.h
 *
 *  Created on: 2023骞�9鏈�20鏃�
 *      Author: Unityliu
 */
#include "xparameters.h"
#include "xil_printf.h"
#include "stdio.h"
#include "xil_io.h"
#include <stdlib.h>
#include <string.h>
#include "ff.h"
#include "xil_types.h"


u32 Init();
u32 Write(char *FileName,u32 src_addr,u32 byte_len,u32 start_addr);
u32 Read(char *FileName,void * DestinationAddress,u32 ByteLength,void * start_addr);

#ifndef SRC_FILE_OP_FILE_OP_H_
#define SRC_FILE_OP_FILE_OP_H_

#endif /* SRC_FILE_OP_FILE_OP_H_ */
