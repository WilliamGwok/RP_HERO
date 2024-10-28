#include "drv_can.h"
#include "drv_uart.h"
#include "string.h"
#include "drv_tim.h"

void driver_init(void)
{
  CAN1_Init();
  CAN2_Init();
	USART1_Init();
  USART2_Init();
  USART3_Init();
  USART4_Init();
  USART5_Init();
	PWM_Init();
}




