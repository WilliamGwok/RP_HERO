#include "drv_can.h"
#include "string.h"
#include "config.h"
#include "config_can.h"
//#include "config_car.h"
//#include "remote.h"

/*
drv_can_t example = 
{
	.hcan = &hcan,
	.rx_id = example,
};
*/

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

uint8_t can1_tx_buf[16];//CAN1发送缓存（0x200 0x1FF）
uint8_t can2_tx_buf[16];//CAN2发送缓存（0x200 0x1FF）
can_rx_info_t CAN_RxInfo;

/**
  * @brief  can集体发送
  * @param  
  * @retval 
  */
void CAN_send_all(void)
{	
	/* CAN发送合集 */
	CAN_SendData(&hcan1,0x200,can1_tx_buf);
	CAN_SendData(&hcan1,0x1FF,&can1_tx_buf[8]);
	
	//清零
	memset(can1_tx_buf,0,16);
}

/**
  * @brief  CAN发送函数
  * @param  
  * @retval 
  */
uint8_t CAN_SendData(CAN_HandleTypeDef *hcan, uint32_t stdId, uint8_t *dat)
{
	CAN_TxHeaderTypeDef pHeader;
	uint32_t txMailBox;
	
	//判断can有效性
	if((hcan->Instance != CAN1)&&(hcan->Instance != CAN2))
	{
		return HAL_ERROR;
	}
	
	//配置帧头
	pHeader.StdId = stdId;
	pHeader.IDE = CAN_ID_STD;
	pHeader.RTR = CAN_RTR_DATA;
	pHeader.DLC = 8;
	
	//判断发送成功与否
	if(HAL_CAN_AddTxMessage(hcan, &pHeader, dat, &txMailBox) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	return HAL_OK;
}

/**
  * @brief  CAN1初始化
  * @param  
  * @retval 
  */
void CAN1_Init(void)
{
	CAN_FilterTypeDef sFilterConfig;
	
	// 配置CAN标识符滤波器
	CAN_Filter_ParamsInit(&sFilterConfig);
	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
	
	// 使能接收中断
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	
	// 开启CAN1
	HAL_CAN_Start(&hcan1);
}

/**
  * @brief  CAN2初始化
  * @param  
  * @retval 
  */
void CAN2_Init(void)
{
	CAN_FilterTypeDef sFilterConfig;
	
	// 配置CAN标识符滤波器
	CAN_Filter_ParamsInit(&sFilterConfig);
	HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig);
	
	// 使能接收中断
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
	
	// 开启CAN2
	HAL_CAN_Start(&hcan2);
}

/**
  * @brief  配置CAN标识符滤波器
  * @param  
  * @retval 
  */
void CAN_Filter_ParamsInit(CAN_FilterTypeDef *sFilterConfig)
{
	sFilterConfig->FilterIdHigh = 0;						
	sFilterConfig->FilterIdLow = 0;							
	sFilterConfig->FilterMaskIdHigh = 0;					// 不过滤
	sFilterConfig->FilterMaskIdLow = 0;						// 不过滤
	sFilterConfig->FilterFIFOAssignment = CAN_FILTER_FIFO0;	// 过滤器关联到FIFO0
	sFilterConfig->FilterBank = 0;							// 设置过滤器0
	sFilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;		// 标识符屏蔽模式
	sFilterConfig->FilterScale = CAN_FILTERSCALE_32BIT;		// 32位宽
	sFilterConfig->FilterActivation = ENABLE;				// 激活滤波器
	sFilterConfig->SlaveStartFilterBank = 0;
}

/**
  * @brief  can接受中断，在stm32f4xx_hal_can.c内弱定义
  * @param  
  * @retval 
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxInfo.Header, CAN_RxInfo.Data);
  if(hcan == &hcan1)
  {
    CAN1_Get_Data(CAN_RxInfo.Header.StdId, CAN_RxInfo.Data);
  }
  else if(hcan == &hcan2)
  {
    CAN2_Get_Data(CAN_RxInfo.Header.StdId, CAN_RxInfo.Data);
  }
  else 
  {
    return;
  }
}
