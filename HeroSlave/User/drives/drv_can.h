/**
  ******************************************************************************
  * @file           : drv_can.c\h
  * @brief          : 
  * @note           : finish 2022-2-12 12:08:23
  ******************************************************************************
  */

#ifndef __DEV_CAN_H
#define __DEV_CAN_H

#include "stm32f4xx_hal.h"

typedef struct 
{
  CAN_RxHeaderTypeDef Header;
  uint8_t Data[8];
}can_rx_info_t;

typedef struct 
{
	CAN_HandleTypeDef *hcan;
	uint32_t rx_id;
}drv_can_t;

void CAN_send_all(void); //CAN集体发送函数

void CAN1_Init(void); //CAN1初始化
void CAN2_Init(void); //CAN2初始化
void CAN_Filter_ParamsInit(CAN_FilterTypeDef *sFilterConfig); //配置CAN标识符滤波器
uint8_t CAN_SendData(CAN_HandleTypeDef *hcan, uint32_t stdId, uint8_t *dat); //CAN发送函数

#endif
