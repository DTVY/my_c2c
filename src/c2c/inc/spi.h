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

#ifndef __SPI_H__
#define __SPI_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef void(*spi_cb_t)(void);

void SPI_Config(bool Master);
void SPI_ConfigTransfer(uint8_t *TxBuffer, uint8_t *RxBuffer, uint32_t Len);
void SPI_StartTransfer(void);
void SPI_StopTransfer(void);
void SPI_WaitTransferComplete(void);
void SPI_AsyncTransfer(uint8_t *TxBuffer, uint8_t *RxBuffer, uint32_t Len, spi_cb_t CallBack);
void SPI_SyncTransfer(uint8_t *TxBuffer, uint8_t *RxBuffer, uint32_t Len);
void SPI_RequestInExtiConfig(bool Enable);
bool SPI_RequestInGetStatus(void);
void SPI_RequestOutSetValue(bool Val);

#endif
