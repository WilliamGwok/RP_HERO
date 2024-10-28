#ifndef __CAP_H
#define __CAP_H

#include "stm32f4xx_hal.h"

typedef struct CAP
{
	uint8_t normal_switch;
	uint8_t record_switch;
}cap_t;

extern cap_t cap;

#endif



