#include "launch_motor.h"
#include "motor.h"

motor_t Launch_Motor[LAUNCH_LIST] = 
{
	[DIAL_LAUNCH] = 
	{
		.id.drive_type = M_CAN1,
		.id.motor_type = RM3508,
		.id.rx_id = feed_motor_id,
		
		.init = motor_class_init,
	},
	[POSITION_LAUNCH] = 
	{
		.id.drive_type = M_CAN2,
		.id.motor_type = RM2006,
		.id.rx_id = position_motor_id,
		
		.init = motor_class_init,
	},
	[LEFT_FRIC_LAUNCH] = 
	{
		.id.drive_type = M_CAN2,
		.id.motor_type = RM3508,
		.id.rx_id = left_fric_motor_id,
		
		.init = motor_class_init,
	},
	[RIGHT_FRIC_LAUNCH] = 
	{
		.id.drive_type = M_CAN2,
		.id.motor_type = RM3508,
		.id.rx_id = right_fric_motor_id,
		
		.init = motor_class_init,
	},
};

