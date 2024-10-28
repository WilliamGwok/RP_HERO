#include "gimbal_motor.h"
#include "motor.h"

motor_t Gimbal_Motor[GIMBAL_LIST] = 
{
	[YAW_GIMBAL] = 
	{
		.id.drive_type = M_CAN1,
		.id.motor_type = GM6020,
		.id.rx_id = yaw_motor_id,
		
		.init = motor_class_init,
	},
	[PITCH_GIMBAL] = 
	{
		.id.drive_type = M_CAN2,
		.id.motor_type = GM6020,
		.id.rx_id = pitch_motor_id,
		
		.init = motor_class_init,
	},
};
