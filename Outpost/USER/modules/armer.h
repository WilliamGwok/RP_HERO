#ifndef __ARMER_H
#define __ARMER_H

#include "stm32f4xx_hal.h"
#include "config_motor.h"
#include "drv_can.h"
#include "config_can.h"

#define GAME_SPEED  631//912
#define HALF_SPEED 315//456
#define DEAD_SPEED 0

void armer_init(void);
void armer_control(void);

#endif


 