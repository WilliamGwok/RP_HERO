#include "config_uart.h"
#include "judge_protocol.h"
#include "string.h"

uint8_t test_buff [128];

void USART1_rxDataHandler(uint8_t *rxBuf)
{
}

void USART2_rxDataHandler(uint8_t *rxBuf)
{
}

void USART3_rxDataHandler(uint8_t *rxBuf)
{
}

void USART4_rxDataHandler(uint8_t *rxBuf)
{
	judge_receive(rxBuf);
//	memcpy(test_buff,rxBuf,100);
}

void USART5_rxDataHandler(uint8_t *rxBuf)
{
//  judge_receive(rxBuf);
//	memcpy(test_buff,rxBuf,100);
}


