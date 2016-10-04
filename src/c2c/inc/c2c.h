/*
 *  Copyright (C) : 2016
 *  File name     : spi.h
 *	Author        : Dang Minh Phuong
 *  Email         : kamejoko80@yahoo.com
 *
 *  This program is free software, you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef __C2C_H__
#define __C2C_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "spi.h"

#define MAX_DATA_LEN     (255)
#define SOF              (0xAA)
#define FRAME_SIZE       sizeof(c2c_frame_t)

#define SPI_MASTER

typedef enum
{
    C2C_MASTER = 0x01,
    C2C_SLAVER
} c2c_host_t;

typedef struct
{
    uint8_t Sof ; // Start of frame
    uint8_t Len ; // Data Len
    uint8_t Data[MAX_DATA_LEN];
    uint8_t Csm;  // Data checksum
} __attribute__((packed, aligned(1))) c2c_frame_t;

typedef void (*c2c_cb_t)(uint8_t Status, c2c_frame_t *Frame);

typedef struct
{
    uint8_t     Mode;
    uint8_t     State;
    c2c_cb_t    SndCallBack;
    c2c_cb_t    RcvCallBack;
    c2c_frame_t Frame;
} c2c_info_t;

void C2C_Init(bool Master);
void C2C_SendFrameRequest(uint8_t *Data, uint8_t Len);
void C2C_ReceiveFrameRequest(void);
void C2C_RegisterReceiveCallBack(c2c_cb_t RcvCallBack);
void C2C_RegisterTransmitCallBack(c2c_cb_t SndCallBack);

#endif
