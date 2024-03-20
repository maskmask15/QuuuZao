#include "debug.h"

#define MEAN_DATA 1.37

int32_t retMean_s32(const mat_s32 *src, int l, int r) {
//	static int32_t last_mean;
	int64_t sum = 0;
    int w = src->cols;
    for (int i = l; i < r; i++) {
        for (int j = 0; j < w; j++) {
        	sum += src->data[w * i + j];
        }
    }
    int32_t mean = sum / ((r - l) * w);
    return mean;
}

int32_t compareMean_s32(const mat_s32 *src, char *file, char* line) {
//	static int32_t last_mean;
	int32_t errorMean = retMean_s32(src, 0, 3);
	int32_t rightMean = retMean_s32(src, 50, 53);
	if(abs(errorMean)/abs(rightMean) > MEAN_DATA){
		printf("mean error %d %d\n", errorMean, rightMean);
	}
    return abs(rightMean - errorMean);
}

