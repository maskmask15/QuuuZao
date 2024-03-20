#include "./mat/matCal_s32.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "./nuc/utils.h"

#define _QUZAO_DEBUG

int32_t compareMean_s32(const mat_s32 *src, char *file, char* line);
int32_t retMean_s32(const mat_s32 *src, int l, int r);


#ifdef _QUZAO_DEBUG
#define debugMean(x) compareMean_s32(x,__FILE__,__LINE__)
#else
#define debugMean(x)
#endif
