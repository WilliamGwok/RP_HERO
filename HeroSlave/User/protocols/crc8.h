/**
  ******************************************************************************
  * @file           : crc8.h
  * @brief          : 
  * @update         : 2021-11-26 16:00:52
  ******************************************************************************
  */
#ifndef __CRC8_H
#define __CRC8_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private function prototypes -----------------------------------------------*/
void Append_CRC8_Check_Num( uint8_t *pchMessage, uint16_t dwLength);
uint32_t Verify_CRC8_Check_Sum( uint8_t *pchMessage, uint16_t dwLength);
uint8_t Get_CRC8_Check_Num( uint8_t *pchMessage, uint16_t dwLength, uint8_t ucCRC8 );

#endif




