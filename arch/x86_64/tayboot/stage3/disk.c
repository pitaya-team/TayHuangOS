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
 * disk.h
 *
 * 实现硬盘操作函数
 *
 */



#include "disk.h"
#include "int_handlers.h"
#include <tayboot/ports.h>

PRIVATE volatile bool FLAG_IDE0 = false;
PRIVATE volatile bool FLAG_IDE1 = false;
PRIVATE volatile bool ENABLE_PROCCESS_IDE0 = false;
PRIVATE volatile bool ENABLE_PROCCESS_IDE1 = false;

PRIVATE void process_ide0_irq(int irq) { //处理IDE0的IRQ
    if (! ENABLE_PROCCESS_IDE0)
        return;
    FLAG_IDE0 = true;
}

PRIVATE void process_ide1_irq(int irq) { //处理IDE1的IRQ
    if (! ENABLE_PROCCESS_IDE1)
        return;
    FLAG_IDE1 = true;
}

PUBLIC void init_disk_driver(void) {
    register_irq_handler(14, process_ide0_irq);
    register_irq_handler(15, process_ide1_irq);
    enable_irq(14);
    enable_irq(15);
}

#define IDE_MASK (0x1)
#define SLAVE_MASK (0x2)

#define MAKE_DEVICE_REG(mode, device, lba) (((mode) << 6) | ((device) << 4) | (lba & 0xF) | 0xA0) //MAKE Device寄存器

PRIVATE inline void wait_for_ide0_free(void) { //等待ide0空闲
    while (inb(IDE0_STATUS) & 0x80);
}

PRIVATE inline void relax_ide0(void) { //延迟
    inb (IDE0_STATUS);
    inb (IDE0_STATUS);
    inb (IDE0_STATUS);
    inb (IDE0_STATUS);
    inb (IDE0_STATUS);
}

PRIVATE inline void wait_for_ide1_free(void) { //等待ide1空闲
    while (inb(IDE1_STATUS) & 0x80);
}

PRIVATE inline void relax_ide1(void) { //延迟
    inb (IDE1_STATUS);
    inb (IDE1_STATUS);
    inb (IDE1_STATUS);
    inb (IDE1_STATUS);
    inb (IDE1_STATUS);
}

PRIVATE void identify_ide0_disk(bool slave, void *dst) { //获取IDE0参数
    outb (IDE0_DEVICE, MAKE_DEVICE_REG(0, slave, 0));
    relax_ide0();
    wait_for_ide0_free();
    ENABLE_PROCCESS_IDE0 = true;
    outb (IDE0_FEATURES, 0);
    outb (IDE0_DEVICE_CONTROL, 0);
    outb (IDE0_SECTOR_COUNTER, 1);
    outb (IDE0_LBA_LOW, 0);
    outb (IDE0_LBA_MID, 0);
    outb (IDE0_LBA_HIGH, 0);
    outb (IDE0_COMMAND, 0xEC); //0xEC: IDENTIFY
    while (! FLAG_IDE0); //等待数据准备好
    for (int i = 0 ; i < 256 ; i ++) {
        *(((word*)dst) + i) = inw(IDE0_DATA); //256字数据
    }
    ENABLE_PROCCESS_IDE0 = false;
    FLAG_IDE0 = false;
}

PRIVATE void identify_ide1_disk(bool slave, void *dst) { //获取IDE1参数
    outb (IDE1_DEVICE, MAKE_DEVICE_REG(0, slave, 0));
    relax_ide1();
    wait_for_ide1_free();
    ENABLE_PROCCESS_IDE1 = true;
    outb (IDE1_DEVICE_CONTROL, 0);
    outb (IDE1_FEATURES, 0);
    outb (IDE1_SECTOR_COUNTER, 1);
    outb (IDE1_LBA_LOW, 0);
    outb (IDE1_LBA_MID, 0);
    outb (IDE1_LBA_HIGH, 0);
    outb (IDE1_COMMAND, 0xEC); //0xEC: IDENTIFY
    while (! FLAG_IDE1); //等待数据准备好
    for (int i = 0 ; i < 256 ; i ++) {
        *(((word*)dst) + i) = inw(IDE1_DATA); //256字数据
    }
    FLAG_IDE1 = false;
    ENABLE_PROCCESS_IDE1 = false;
}

PUBLIC void identify_disk(int selector, void *dst) {
    if ((selector & IDE_MASK) == 0) {
        identify_ide0_disk((selector & SLAVE_MASK) != 0, dst);
    }
    else {
        identify_ide1_disk((selector & SLAVE_MASK) != 0, dst);
    }
}

PRIVATE void read_ide0_sector(dword lba, int num, bool slave, void *dst) { //读IDE0的扇区
    outb (IDE0_DEVICE, MAKE_DEVICE_REG(1, slave, lba >> 24));
    relax_ide0();
    wait_for_ide0_free();
    ENABLE_PROCCESS_IDE0 = true;
    outb (IDE0_DEVICE_CONTROL, 0);
    outb (IDE0_FEATURES, 0);
    outb (IDE0_SECTOR_COUNTER, num);
    outb (IDE0_LBA_LOW, lba & 0xFF);
    outb (IDE0_LBA_MID, (lba >> 8) & 0xFF);
    outb (IDE0_LBA_HIGH, (lba >> 16) & 0xFF);
    outb (IDE0_COMMAND, 0x20); //0x20: READ SECTOR
    while (! FLAG_IDE0); //等待数据准备好
    for (int i = 0 ; i < num ; i ++) { //num个扇区
        for (int j = 0 ; j < 256 ; j ++) { //每扇区256个字
            *(((word*)dst) + i * 256 + j) = inw(IDE0_DATA);
        }
    }
    FLAG_IDE0 = false;
    ENABLE_PROCCESS_IDE0 = false;
}


PRIVATE void read_ide1_sector(dword lba, int num, bool slave, void *dst) { //读IDE1的扇区
    outb (IDE1_DEVICE, MAKE_DEVICE_REG(1, slave, lba >> 24));
    relax_ide1();
    wait_for_ide1_free();
    ENABLE_PROCCESS_IDE1 = true;
    outb (IDE1_DEVICE_CONTROL, 0);
    outb (IDE1_FEATURES, 0);
    outb (IDE1_SECTOR_COUNTER, num);
    outb (IDE1_LBA_LOW, lba & 0xFF);
    outb (IDE1_LBA_MID, (lba >> 8) & 0xFF);
    outb (IDE1_LBA_HIGH, (lba >> 16) & 0xFF);
    outb (IDE1_COMMAND, 0x20); //0x20: READ SECTOR
    while (! FLAG_IDE1); //等待数据准备好
    for (int i = 0 ; i < num ; i ++) { //num个扇区
        for (int j = 0 ; j < 256 ; j ++) { //每扇区256个字
            *(((word*)dst) + i * 256 + j) = inw(IDE1_DATA);
        }
    }
    FLAG_IDE1 = false;
    ENABLE_PROCCESS_IDE1 = false;
}

PUBLIC void read_sector(dword lba, int num, int selector, void *dst) {
    if ((selector & IDE_MASK) == 0) {
        read_ide0_sector(lba, num, (selector & SLAVE_MASK) != 0, dst);
    }
    else {
        read_ide1_sector(lba, num, (selector & SLAVE_MASK) != 0, dst);
    }
}