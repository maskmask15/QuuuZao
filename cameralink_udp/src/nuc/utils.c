//
// Created by ozgaga on 2022/11/2.
//

#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "xparameters_ps.h"

float findKthNums_f32(float *nums, int l, int r, int k) {
    if (l >= r) return nums[l];
    int i = l - 1, j = r + 1;
    float mid = nums[(l + r) >> 1];
    while (i < j) {
        do i++; while (nums[i] < mid);
        do j--; while (nums[j] > mid);
        if (i < j) {
            float t = nums[i];
            nums[i] = nums[j];
            nums[j] = t;
        }
    }
    if (j - l + 1 > k) return findKthNums_f32(nums, l, j, k);
    else return findKthNums_f32(nums, j + 1, r, k - (j - l + 1));
}

int findKthNums_s32(int *nums, int l, int r, int k) {
    if (l >= r) return nums[l];
    int i = l - 1, j = r + 1;
    int mid = nums[(l + r) >> 1];
    while (i < j) {
        do i++; while (nums[i] < mid);
        do j--; while (nums[j] > mid);
        if (i < j) {
            int t = nums[i];
            nums[i] = nums[j];
            nums[j] = t;
        }
    }
    if (j - l + 1 > k) return findKthNums_s32(nums, l, j, k);
    else return findKthNums_s32(nums, j + 1, r, k - (j - l + 1));
}

void sortNums_s32(int *nums, int l, int r) {
    if (l >= r) return;
    int i = l - 1, j = r + 1;
    int mid = nums[(l + r) >> 1];
    while (i < j) {
        do i++; while (nums[i] < mid);
        do j--; while (nums[j] > mid);
        if (i < j) {
            int t = nums[i];
            nums[i] = nums[j];
            nums[j] = t;
        }
    }
    sortNums_s32(nums, l, j);
    sortNums_s32(nums, j + 1, r);
}

void sortNums_f32(float *nums, int l, int r) {
    if (l >= r) return;
    int i = l - 1, j = r + 1;
    float mid = nums[(l + r) >> 1];
    while (i < j) {
        do i++; while (nums[i] < mid);
        do j--; while (nums[j] > mid);
        if (i < j) {
            float t = nums[i];
            nums[i] = nums[j];
            nums[j] = t;
        }
    }
    sortNums_f32(nums, l, j);
    sortNums_f32(nums, j + 1, r);
}
