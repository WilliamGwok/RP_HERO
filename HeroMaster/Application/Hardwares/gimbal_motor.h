#ifndef __GIMBAL_MOTOR_H
#define __GIMBAL_MOTOR_H

#include "stm32f4xx_hal.h"
#include "motor.h"
#include "stdbool.h"

/*CAN1*/
#define yaw_motor_id  0x208
/*CAN2*/
#define pitch_motor_id  0x205

typedef enum
{
	YAW_GIMBAL = 0,
	PITCH_GIMBAL ,
	
	GIMBAL_LIST
}gimbal_motor_list_e;

extern motor_t Gimbal_Motor[GIMBAL_LIST];

#endif


