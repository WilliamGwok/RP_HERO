#ifndef __LAUNCH_MOTOR_H
#define __LAUNCH_MOTOR_H

#include "stm32f4xx_hal.h"
#include "motor.h"
#include "stdbool.h"

/*CAN1*/
#define feed_motor_id  0x207
/*CAN2*/
#define position_motor_id  0x202
#define left_fric_motor_id  0x206
#define right_fric_motor_id  0x207


typedef enum
{
	DIAL_LAUNCH = 0,
	POSITION_LAUNCH ,
	LEFT_FRIC_LAUNCH,
	RIGHT_FRIC_LAUNCH,
	
	LAUNCH_LIST
}launch_motor_list_e;

extern motor_t Launch_Motor[LAUNCH_LIST];

#endif
