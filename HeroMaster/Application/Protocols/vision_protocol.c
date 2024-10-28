#include "vision_protocol.h"
#include "string.h"
#include "crc16.h"
#include "crc8.h"

extern UART_HandleTypeDef huart3;

Vision_Tx_Info_t Vision_Tx_Info = 
{
	.SOF = 0xA5,
};
Vision_Rx_Info_t Vision_Rx_Info;
uint8_t Vision_TxBuf[30];

bool Vision_Send_Data(void)
{
	memcpy(Vision_TxBuf, &Vision_Tx_Info, sizeof(Vision_Tx_Info_t));
	Append_CRC8_Check_Num(Vision_TxBuf, 3);
	
	
	
	Append_CRC16_Check_Sum(Vision_TxBuf, sizeof(Vision_Tx_Info_t));
	
	if(HAL_UART_Transmit_DMA(&huart3,Vision_TxBuf,sizeof(Vision_Tx_Info_t)) == HAL_OK)
	{
			return true;
	}
	return false;
}

bool Vision_Recieve_Data(uint8_t *rxBuf)
{
	if(rxBuf[0] == 0xA5)
	{
		if(Verify_CRC8_Check_Sum(rxBuf, 3) == true)
		{
			if(Verify_CRC16_Check_Sum(rxBuf, sizeof(Vision_Rx_Info_t)) == true)
			{
				memcpy(&Vision_Rx_Info, rxBuf, sizeof(Vision_Rx_Info_t));
				return true;
			}
		}
	}
	return false;
}

