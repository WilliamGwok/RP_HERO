#include "config_motor.h"

pid_info_t post_pid_info = 
{
	.kp = 7.5f,
	.ki = 0.2f,
	.kd = 0.0f,
	.integral_max = 3000.0f,
	.out_max = 5000.0f,
};

pid_t post_pid = 
{
	.info = &post_pid_info,
};
//typedef struct 
//{
//	float	  target;
//	float	  measure;
//	float 	err;
//	float 	last_err;
//	float	  integral;
//	float 	pout;
//	float 	iout;
//	float 	dout;
//	float 	out;
//	/* ≈‰÷√ */
//	float   blind_err;
//	float   integral_bias;
//	float	  kp;
//	float 	ki;
//	float 	kd;
//	float 	integral_max;
//	float 	out_max;
//}pid_info_t;




