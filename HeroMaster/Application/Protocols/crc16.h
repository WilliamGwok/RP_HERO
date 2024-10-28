/**
  ******************************************************************************
  * @file           : crc16.h
  * @brief          : 
  * @update         : 2021-11-26 17:06:06
  ******************************************************************************
  */
#ifndef __CRC16_H
#define __CRC16_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private function prototypes -----------------------------------------------*/
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);

#endif




