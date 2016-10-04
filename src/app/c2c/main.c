/*
 *  Copyright (C) : 2016
 *  File name     : main.c
 *	Author        : Dang Minh Phuong
 *  Email         : kamejoko80@yahoo.com
 *
 *  This program is free software, you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h"
#include "uart_debug.h"
#include "c2c.h"

#define BUFFERSIZE 32

uint8_t aTxBufferMaster[BUFFERSIZE] = "C2C Master";
uint8_t aTxBufferSlaver[BUFFERSIZE] = "C2C Slaver";

extern unsigned char SendChar (unsigned char ch) ;

/**
 * @brief         Delay function
 * @param[in]     uint32_t m
 * @param[in,out] None
 * @return        void
 */
void Delay(uint32_t m)
{
    uint32_t i, j;

    for (i = 0; i < m; i++)
       for(j = 0; j < 10000; j++);
}


/**
 * @brief         Call back function of transmit mode
 * @param[in]     bool Status
 * @param[in]     c2c_frame_t *Frame
 * @param[in,out] None
 * @return        void
 */
void SendFrameCallBack(bool Status, c2c_frame_t *Frame)
{
#ifdef SPI_MASTER
    //printf("Master TX completed\r\n");
#else
    //printf("Slaver TX completed\r\n");
#endif
}

/**
 * @brief         Call back function of receive mode
 * @param[in]     bool Status
 * @param[in]     c2c_frame_t *Frame
 * @param[in,out] None
 * @return        void
 */
void ReceiveFrameCallBack(bool Status, c2c_frame_t *Frame)
{

  uint32_t i;

  if(Status)
  {
#ifdef SPI_MASTER
      printf("Master received : ");
#else
      printf("Slaver received : ");
#endif

      for(i = 0; i < Frame->Len; i++)
      {
          printf("%c", Frame->Data[i]);
      }
      printf("\r\n");

  }
  else
  {
#ifdef SPI_MASTER
    printf("Master received error frame\r\n");
#else
    printf("Slaver received error frame\r\n");
#endif
  }

#ifdef SPI_MASTER
      Delay(10);
      C2C_SendFrameRequest(aTxBufferMaster, 10);
#else
      Delay(10);
      C2C_SendFrameRequest(aTxBufferSlaver, 10);
#endif

}

/**
 * @brief         main function
 * @param[in]     None
 * @param[in,out] None
 * @return        void
 */
int main(void)
{

    uart_debug_init();

    STM_EVAL_LEDInit(LED3);

#ifdef SPI_MASTER

    printf("MASTER DEMO\r\n");

    /* SPI configuration */
    C2C_Init(true);
#else

    printf("SLAVER DEMO\r\n");
    C2C_Init(false);
#endif

#ifdef SPI_MASTER

  /* Register receive callback */
  C2C_RegisterReceiveCallBack((c2c_cb_t)&ReceiveFrameCallBack);
  C2C_RegisterTransmitCallBack((c2c_cb_t)&SendFrameCallBack);

  printf("Master send request\r\n");
  C2C_SendFrameRequest(aTxBufferMaster, 10);

#else /* SPI_SLAVER */

  /* Register receive callback */
  C2C_RegisterReceiveCallBack((c2c_cb_t)&ReceiveFrameCallBack);
  C2C_RegisterTransmitCallBack((c2c_cb_t)&SendFrameCallBack);

#endif /* SPI_MASTER */

  /* we shouldn't return here */
  while(1);
}
