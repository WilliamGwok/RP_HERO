/**
  ******************************************************************************
  * @file           : pid.c\h
  * @brief          : 
  * @note           : finish 2022-2-13 19:19:47
  ******************************************************************************
  */

#ifndef __PID_H
#define __PID_H

typedef struct 
{
	float	  target;
	float	  measure;
	float 	err;
	float 	last_err;
	float	  integral;
	float 	pout;
	float 	iout;
	float 	dout;
	float 	out;
	/* ≈‰÷√ */
	float   blind_err;
	float   integral_bias;
	float	  kp;
	float 	ki;
	float 	kd;
	float 	integral_max;
	float 	out_max;
}pid_info_t;

typedef struct
{
	pid_info_t *info;
}pid_t;


void pid_init(pid_t *pid);
void single_pid_cal(pid_info_t *pid);
void nonlinear_pid_cal(pid_info_t *pid);
float fal(float err, float a, float d);

#endif
