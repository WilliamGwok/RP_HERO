#include "drv_all.h"
#include "armer.h"

void init_all(void)
{
	CAN1_Init();
	CAN2_Init();
	USART2_Init();
	
//	armer_init();
}




