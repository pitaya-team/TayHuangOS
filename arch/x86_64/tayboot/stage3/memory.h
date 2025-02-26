/* 
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: Flysong
 *
 * memory.h
 *
 * 内存管理函数
 *
 */



#pragma once

#include <tayboot/tay_defs.h>

PUBLIC void memcpy(void *dst, void *src, int sz);
PUBLIC void memset(void *dst, byte val, int sz);
PUBLIC void init_heap(int memsz);
PUBLIC void terminate_heap(int memsz);
PUBLIC void reset_heap(int memsz);
PUBLIC void *malloc(int size);
PUBLIC void *calloc(int num, int size);
PUBLIC void free(void *ptr);
PUBLIC bool query_using(void *ptr, int size); //true when using
PUBLIC void set_heap_top(void *top); //default = 0xFFFF
PUBLIC void set_heap_limit(void *limit); //default = 0xC000
PUBLIC int get_used(void);
PUBLIC int get_remain(void);