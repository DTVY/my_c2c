/*
 *  Copyright (C) : 2016
 *  File name     : spi.c
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

#include "spi.h"
#include "c2c.h"

/***************************************************************
 *  MACRO DEFINITION
 ***************************************************************/

#define C2C_STATE_IDLE (0)
#define C2C_STATE_RQST (1)
#define C2C_STATE_FINI (2)

#define C2C_MODE_TX    (0)
#define C2C_MODE_RX    (1)


/***************************************************************
 *  GLOBAL VARIABLE DEFINITION
 ***************************************************************/

static uint8_t    DummyBuff[FRAME_SIZE];
static c2c_info_t Info;

/***************************************************************
 *  FUNCTION DECLARATION
 ***************************************************************/

/**
 * @brief         Calculate data checksum
 * @param[in]     uint8_t *Data
 * @param[in]     uint16_t Len
 * @return        uint8_t
 */
static uint8_t C2C_CheckSum(uint8_t *Data, uint16_t Len)
{
  uint8_t i;
  uint32_t Sum = 0;

  for (i = 0; i < Len; i++)
  {
     Sum = Sum + Data[i];
  }
  Sum = -Sum;
  return (uint8_t)(Sum & 0xFF);
}

/**
 * @brief         Initialize C2C
 * @param[in]     bool Master
 * @param[out]    None
 * @return        void
 */
void C2C_Init(bool Master)
{
  /* Initialize SPI bus */
  SPI_Config(Master);

  /* Initialize C2C */
  Info.State       = C2C_STATE_IDLE;
  Info.Mode        = C2C_MODE_RX;
  Info.RcvCallBack = NULL;
  Info.SndCallBack = NULL;
}

/**
 * @brief         Check validation of received frame.
 * @param[in]     c2c_frame_t *Frame
 * @param[out]    None
 * @return        bool
 */
static bool C2C_CheckValidFrame(c2c_frame_t *Frame)
{
   uint8_t CheckSum;

   /* Check valid start of frame */
   if (SOF != Frame->Sof)
   {
       return false;
   }

   CheckSum = C2C_CheckSum(Frame->Data, Frame->Len);

   /* Check valid data check sum */
   if (CheckSum != Frame->Csm)
   {
       return false;
   }

   return true;
}

/**
 * @brief         C2C Handler
 * @param[in]     void
 * @param[out]    None
 * @return        void
 */
static void C2C_Handler(void)
{
    switch(Info.State)
    {
        case C2C_STATE_IDLE:
            SPI_RequestInExtiConfig(true);
        break;

        case C2C_STATE_RQST:

            if(C2C_MODE_TX == Info.Mode)
            {
               #ifdef SPI_MASTER
                    /* Generate falling edge to request sending data */
                    SPI_RequestOutSetValue(false);

                    /* Next state */
                    Info.State = C2C_STATE_FINI;

                    /* Transfer request */
                    SPI_AsyncTransfer((uint8_t *)&Info.Frame, DummyBuff, FRAME_SIZE, &C2C_Handler);

               #else
                    /* Next state */
                    Info.State = C2C_STATE_FINI;

                    /* Transfer request */
                    SPI_AsyncTransfer((uint8_t *)&Info.Frame, DummyBuff, FRAME_SIZE, &C2C_Handler);

                    /* Generate falling edge to request sending data */
                    SPI_RequestOutSetValue(false);
               #endif
            }
            else
            {
                /* Next state */
                Info.State = C2C_STATE_FINI;

                /* Transfer request */
                SPI_AsyncTransfer(DummyBuff, (uint8_t *)&Info.Frame, FRAME_SIZE, &C2C_Handler);
            }

        break;

        case C2C_STATE_FINI:

            if(C2C_MODE_TX == Info.Mode)
            {
                /* Release request to send */
                SPI_RequestOutSetValue(true);

                /* Notify transmit has completed */
                if (NULL != Info.SndCallBack)
                {
                    /* Call callback function */
                    Info.SndCallBack(true, &Info.Frame);
                }

                /* Enable receive request */
                SPI_RequestInExtiConfig(true);
            }
            else
            {
                /* Check valid frame */
                if (true == C2C_CheckValidFrame(&Info.Frame))
                {
                   /* Next state */
                   Info.State = C2C_STATE_IDLE;

                   if (NULL != Info.RcvCallBack)
                   {
                       Info.RcvCallBack(true, &Info.Frame);
                   }

                   SPI_RequestInExtiConfig(true);
                }
                else
                {
                    /* Next state */
                   Info.State = C2C_STATE_IDLE;

                   if (NULL != Info.RcvCallBack)
                   {
                       Info.RcvCallBack(false, &Info.Frame);
                   }

                   SPI_RequestInExtiConfig(true);
                }
            }
        break;

        default:
        break;
    }
}


/**
 * @brief         Request sending data via C2C bus
 * @param[in]     void
 * @param[out]    None
 * @return        void
 */
void C2C_SendFrameRequest(uint8_t *Data, uint8_t Len)
{
    SPI_RequestOutSetValue(true);

    /* Make sure the other side is not busy */
    if(false == SPI_RequestInGetStatus())
    {
        return;
    }

    /* Disable request in interrupt */
    SPI_RequestInExtiConfig(false);

    /* Set mode */
    Info.Mode = C2C_MODE_TX;

    /* Request transfer data */
    Info.State = C2C_STATE_RQST;

    /* Copy data */
    memcpy(Info.Frame.Data, Data, Len);

    /* Build the frame */
    Info.Frame.Sof = SOF;
    Info.Frame.Len = Len;
    Info.Frame.Csm = C2C_CheckSum(Info.Frame.Data, Len);

    /* Request Transfer */
    C2C_Handler();
}

/**
 * @brief         Request receiving data via C2C bus
 * @param[in]     void
 * @param[out]    None
 * @return        void
 */
void C2C_ReceiveFrameRequest(void)
{
    /* Set mode */
    Info.Mode = C2C_MODE_RX;
    /* Request transfer data */
    Info.State = C2C_STATE_RQST;
    /* Request Transfer */
    C2C_Handler();
}

/**
 * @brief         Register receiving data call back function
 * @param[in]     c2c_cb_t RcvCallBack
 * @param[out]    None
 * @return        void
 */
void C2C_RegisterReceiveCallBack(c2c_cb_t RcvCallBack)
{
    Info.RcvCallBack = RcvCallBack;
}

/**
 * @brief         Register sending data call back function
 * @param[in]     c2c_cb_t RcvCallBack
 * @param[out]    None
 * @return        void
 */
void C2C_RegisterTransmitCallBack(c2c_cb_t SndCallBack)
{
    Info.SndCallBack = SndCallBack;
}
