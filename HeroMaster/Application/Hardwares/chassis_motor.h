#ifndef __CHASSIS_MOTOR_H
#define __CHASSIS_MOTOR_H

#include "stm32f4xx_hal.h"
#include "motor.h"
#include "stdbool.h"

/*CAN1*/
#define lf_motor_id  0x201
#define rf_motor_id  0x202
#define lb_motor_id  0x203
#define rb_motor_id  0x204

typedef enum
{
	LF_WHEEL = 0,
	RF_WHEEL ,
	LB_WHEEL ,
	RB_WHEEL ,
	
	CHASSIS_LIST
}chassis_motor_list_e;

extern motor_t Chassis_Motor[CHASSIS_LIST];

#endif



