/*!
    \file    usbh_msc_fatfs.c
    \brief   USB MSC host FATFS related functions

    \version 2023-06-25, V3.1.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "usb_conf.h"
#include "usbh_msc_core.h"
#include "usbh_usr.h"
#include "diskio.h"
#include "ff.h"
#include "USART0.h"

static volatile DSTATUS state = STA_NOINIT; /* disk status */

extern usbh_host usb_host_msc;

/**
 * @brief       初始化 USBH
 * @param       无
 * @retval      无
 */
DSTATUS USBH_initialize(void)
{
    usb_core_driver *udev = (usb_core_driver *)usb_host_msc.data;

    if (udev->host.connect_status) {
        state &= ~STA_NOINIT;
    }

    return state;
}

/**
 * @brief       获取U盘状态
 * @param       无
 * @retval      无
 */
DSTATUS USBH_status(void)
{
    return state;
}

/**
 * @brief       U盘读扇区操作
 * @param       buff   : 数据缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      执行结果(详见DRESULT定义)
 */
DRESULT USBH_read(BYTE *buff, DWORD sector, UINT count)
{
    BYTE status = USBH_OK;
    usb_core_driver *udev = (usb_core_driver *)usb_host_msc.data;

    if (!count) {
        return RES_PARERR;
    }

    if (state & STA_NOINIT) {
        return RES_NOTRDY;
    }

    if (udev->host.connect_status) {
        do {
            status = usbh_msc_read (&usb_host_msc, 0, sector, buff, count);

            if (!udev->host.connect_status) {
                return RES_ERROR;
            }
        } while(status == USBH_BUSY);
    }

    if (status == USBH_OK) {
        return RES_OK;
    }

    return RES_ERROR;
}

/**
 * @brief       U盘写扇区操作
 * @param       buff   : 数据缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要写入的扇区数
 * @retval      执行结果(详见DRESULT定义)
 */
DRESULT USBH_write(const BYTE *buff, DWORD sector, UINT count)
{
    BYTE status = USBH_OK;
    usb_core_driver *udev = (usb_core_driver *)usb_host_msc.data;

    if (!count) {
        return RES_PARERR;
    }

    if (state & STA_NOINIT) {
        return RES_NOTRDY;
    }

    if (state & STA_PROTECT) {
        return RES_WRPRT;
    }

    if (udev->host.connect_status) {
        do {
            status = usbh_msc_write (&usb_host_msc, 0, sector, (BYTE*)buff, count);

            if (!udev->host.connect_status) {
                return RES_ERROR;
            }
        } while(status == USBH_BUSY);
    }

    if (status == USBH_OK) {
        return RES_OK;
    }

    return RES_ERROR;
}

/**
 * @brief       U盘IO控制操作
 * @param       cmd    : 控制命令
 * @param       buff   : 控制数据
 * @retval      执行结果(详见DRESULT定义)
 */
DRESULT USBH_ioctl(BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;
    msc_lun info;

    if (state & STA_NOINIT) {
        return RES_NOTRDY;
    }

    switch (cmd) {
    /* make sure that no pending write process */
    case CTRL_SYNC:
        res = RES_OK;
        break;

    /* get number of sectors on the disk (dword) */
    case GET_SECTOR_COUNT:
        if (USBH_OK == usbh_msc_lun_info_get(&usb_host_msc, 0, &info)) {
            *(DWORD*)buff = (DWORD)info.capacity.block_nbr;
            res = RES_OK;
        }
        break;

    /* get r/w sector size (word) */
    case GET_SECTOR_SIZE:
        if (USBH_OK == usbh_msc_lun_info_get(&usb_host_msc, 0, &info)) {
            *(WORD*)buff = (DWORD)info.capacity.block_size;
            res = RES_OK;
        }
        break;

    /* get erase block size in unit of sector (dword) */
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 512U;
        break;

    default:
        res = RES_PARERR;
        break;
    }

    return res;
}


