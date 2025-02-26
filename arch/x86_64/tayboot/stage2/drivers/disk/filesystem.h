/* 
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2021, 2021 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: Flysong
 *
 * filesystem.h
 *
 * 文件系统
 *
 */



#pragma once

#include "../driver.h"

#define FILESYSTEM_EXT_MAGIC (0x8C4BAD95)

PUBLIC void *recognize_file_system(pdriver disk_driver);