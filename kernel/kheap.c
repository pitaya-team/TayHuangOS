/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: Flysong
 *
 * kheap.c
 *
 * 内核堆
 *
 */



#include "kheap.h"

typedef struct {
    void *start;
    void *limit;
} kh_seg;

#define KH_SEG_NUM (8192)
PRIVATE kh_seg *KHEAP_SEGMENTS = NULL;
PRIVATE void *KHEAP_TOP = NULL;
PRIVATE void *KHEAP_BOTTOM = NULL;

PUBLIC void init_kheap(_IN void *kheap_limit) {
    KHEAP_SEGMENTS = (kh_seg*)(kheap_limit - sizeof(kh_seg) * KH_SEG_NUM);
    KHEAP_TOP = (void*)(kheap_limit - sizeof(kh_seg) * KH_SEG_NUM);
    KHEAP_BOTTOM = (void*)KHEAP_BASE;
    memset(KHEAP_SEGMENTS, 0, sizeof(kh_seg) * KH_SEG_NUM);
}

PRIVATE void *__get_segment_limit(_IN void *base, _IN int size) {
    for (int i = 0 ; i < KH_SEG_NUM ; i ++) {
        if (max(KHEAP_SEGMENTS[i].start, base) < min(KHEAP_SEGMENTS[i].limit, (base + size))) {
            return KHEAP_SEGMENTS[i].limit;
        }
    }
    return NULL;
}

PRIVATE void *__lookup_free_mem(_IN int size) {
    for (void *i = KHEAP_BOTTOM ; i < KHEAP_TOP ;) {
        void *lim = __get_segment_limit(i, size);
        if (lim == NULL)
            return i;
        i = lim + 1;
    }
    return NULL;
}

PRIVATE int __lookup_free_kh_seg(void) {
    for (int i = 0 ; i < KH_SEG_NUM ; i ++) {
        if ((KHEAP_SEGMENTS[i].start == NULL) && (KHEAP_SEGMENTS[i].limit == NULL)) {
            return i;
        }
    }
    return -1;
}

PRIVATE bool __insert_kh_seg(_IN void *start, _IN void *limit) {
    int idx = __lookup_free_kh_seg();
    if (idx == -1) {
        return false;
    }
    KHEAP_SEGMENTS[idx].start = start;
    KHEAP_SEGMENTS[idx].limit = limit;
    return true;
}

PRIVATE void __delete_kh_seg(_IN void *start) {
    for (int i = 0 ; i < KH_SEG_NUM ; i ++) {
        if ((KHEAP_SEGMENTS[i].start == start)) {
            KHEAP_SEGMENTS[i].start = NULL;
            KHEAP_SEGMENTS[i].limit = NULL;
            break;
        }
    }
}

PUBLIC void *malloc(_IN int size) {
    void *mem = __lookup_free_mem(size);
    if (mem == NULL)
        return NULL;
    if (! __insert_kh_seg(mem, mem + size - 1))
        return NULL;
    return mem;
}

PUBLIC void free(_IN void *ptr) {
    __delete_kh_seg(ptr);
}