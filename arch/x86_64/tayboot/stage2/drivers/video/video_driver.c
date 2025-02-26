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
 * video_driver.c
 *
 * 视频驱动
 *
 */



#include "video_driver.h"

PRIVATE bool initialize_driver(pdevice device, pdriver driver, id_t id) {
    if (driver->state != DS_UNINITIALIZE || device->type != DT_VIDEO)
        return false;
    driver->dev_ty = DT_VIDEO;
    driver->state = DS_IDLE;
    driver->extensions = NULL;
    driver->id = id;
    driver->device = device;
    device->driver = driver;
    return true;
}

DEF_SUB_CMD(readbyte) {
    driver->state = DS_BUSY;
    PAPACK(vd, readbyte) args = (PAPACK(vd, readbyte))pack;
    stgs(0xB800);
    args->out->ch = rdgs8((args->pos_y * 80 + args->pos_x) * 2);
    args->out->color = rdgs8((args->pos_y * 80 + args->pos_x) * 2 + 1);
    driver->state = DS_IDLE;
    return true;
}

void _clrscr(void);
DEF_SUB_CMD(clrscr) {
    driver->state = DS_BUSY;
    _clrscr();
    ed_callasm();
    driver->state = DS_IDLE;
    return true;
}

DEF_SUB_CMD(writebyte) {
    driver->state = DS_BUSY;
    PAPACK(vd, writebyte) args = (PAPACK(vd, writebyte))pack;
    stgs(0xB800);
    stgs8((args->pos_y * 80 + args->pos_x) * 2, args->ch);
    stgs8((args->pos_y * 80 + args->pos_x) * 2 + 1, args->color);
    driver->state = DS_IDLE;
    return true;
}

PRIVATE bool process_center(pdriver driver, word cmdty, argpack_t pack) {
    if (driver->state != DS_IDLE || driver->device->type != DT_VIDEO)
        return false;
    switch (cmdty) {
    case VD_CMD_READ_BYTE:
        return SUB_CMD(readbyte)(driver, pack);
    case VD_CMD_WRITE_BYTE:
        return SUB_CMD(writebyte)(driver, pack);
    case VD_CMD_CLRSCR:
        return SUB_CMD(clrscr)(driver, pack);
    }
    return false;
}

PRIVATE bool terminate_driver(pdriver driver) {
    if (driver->state == DS_TERMAINATED || driver->device->type != DT_VIDEO)
        return false;
    driver->state = DS_TERMAINATED;
    return true;
}

PUBLIC void create_video_driver(pdriver driver) {
    driver->init_handle = (initialize_handle_t)initialize_driver;
    driver->pc_handle = (pc_handle_t)process_center;
    driver->terminate_handle = (terminate_handle_t)terminate_driver;
    driver->state = DS_UNINITIALIZE;
}