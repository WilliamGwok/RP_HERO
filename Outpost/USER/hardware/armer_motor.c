#include "armer_motor.h"
#include "config_can.h"

motor_t armer_motor = 
{
	.id.drive_type = M_CAN1,
	.id.motor_type = RM3508,
	.id.rx_id = outpost_motor,
		
	.init = motor_class_init,
};

