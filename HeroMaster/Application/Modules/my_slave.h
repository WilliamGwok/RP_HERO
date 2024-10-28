#ifndef __MY_SLAVE_H
#define __MY_SLAVE_H

#include "stm32f4xx_hal.h"

typedef struct
{
	uint16_t offline_cnt;
	uint8_t status;
}Slave_Work_Status_t;

typedef struct
{
	uint8_t top;
	
}Slave_Pack_t;

typedef struct
{
	Slave_Work_Status_t* status;
	
	Slave_Pack_t* pack;
}My_Slave_t;

void My_Slave_Transmit_Task(void);

#endif

