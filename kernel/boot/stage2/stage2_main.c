// SPDX-License-Identifier: GPL-2.0-only
/* -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * Author: Flysong
 *
 * kernel/boot/stage2/stage2_main.c
 *
 * Stage2 main function here
 */



#include "drivers/drivers.h"
#include "drivers/devices.h"
#include "printf.h"

#include "intcall.h"
#include "heap.h"
#include "drivers/memory/memory_driver.h"
#include "drivers/disk/disk_driver.h"
#include "drivers/disk/filesystems/fs_fat16.h"
#include <string.h>
#include "pm/entry.h"
#include "tools.h"

void print_splash(void) {
    APACK(dk, load_file) pack;
    pack.name = "SPLASHESTXT";
    pack.offset = 0;
    pack.segment = 0x1000;
    a_disk_driver.pc_handle(&a_disk_driver, DK_CMD_LOAD_FILE, &pack);
    int cnt = 0;
    for (int i = 0 ; i < 512 ; i ++) {
        stfs(0x1000);
        if (rdfs8(i) == '\n') {
            cnt ++;
        }
    }
    char splash[33];
    int choice = random(get_clock_time(), 0, cnt);
    int t = 0;
    for (int i = 0 ; i < 512 ; i ++) {
        stfs(0x1000);
        if (t == choice) {
            int j = 0;
            while (rdfs8(i) != '\n') {
                splash[j ++] = rdfs8(i ++);
            }
            splash[j] = 0;
            break;
        }
        if (rdfs8(i) == '\n') {
            t ++;
        }
    }
    printf ("%s\n", splash);
}

void entry(void) {
    ll_init_heap();

    init_devices();
    init_drivers();
    clrscr();

    init_heap();

    a_disk_driver.pc_handle(&a_disk_driver, DK_CMD_INIT, NULL);

    printf ("it's stage2!\n");
    print_splash();

    go_to_protect_mode();

    terminate_drivers();
}