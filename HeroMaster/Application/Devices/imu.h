/**
  ******************************************************************************
  * @file           : imu.c\h
  * @brief          : 
  * @note           : finish 2022-2-13 19:59:35
  ******************************************************************************
  */

#ifndef __IMU_H
#define __IMU_H

#include "stm32f4xx_hal.h"

typedef struct 
{
	uint16_t dif_speed_fil_times;
	float Kp;
	float kp;
	float gyro_lowpass;
	float acce_lowpass;
}imu_config_t;

typedef struct 
{
	short ggx;							    //陀螺仪x轴速度
	short ggy;							    //陀螺仪y轴速度
	short ggz;							    //陀螺仪z轴速度
	float ggx_K;						    //陀螺仪低通滤波后x轴速度
	float ggy_K;						    //陀螺仪低通滤波后y轴速度
	float ggz_K;						    //陀螺仪低通滤波后z轴速度
	short aax;							    //加速度计x轴加速度
	short aay;							    //加速度计y轴加速度
	short aaz;							    //加速度计z轴加速度
	float aax_K;						    //加速度计低通滤波后x轴加速度
	float aay_K;						    //加速度计低通滤波后y轴加速度
	float aaz_K;						    //加速度计低通滤波后z轴加速度
	float pitch_last_;				  //陀螺仪上一次pitch轴角度(不考虑加速度计)
	float roll_last_;				    //陀螺仪上一次roll轴角度(不考虑加速度计)
	float yaw_last_;					  //陀螺仪上一次yaw轴角度(不考虑加速度计)
	float pitch;						    //陀螺仪pitch轴角度(+-90d)
	float roll;							    //陀螺仪roll轴角度(+-180d)
	float yaw;							    //陀螺仪yaw轴角度(+-180d)
	float pitch_;						    //陀螺仪pitch轴角度(+-180d)(不考虑加速度计)
	float roll_;							  //陀螺仪roll轴角度(+-180d)(不考虑加速度计)
	float yaw_;							    //陀螺仪yaw轴角度(+-180d)(不考虑加速度计)
	float pitch_dif_speed;      //陀螺仪pitch轴差分速度
	float roll_dif_speed;       //陀螺仪roll轴差分速度
	float yaw_dif_speed;        //陀螺仪yaw轴差分速度
	float pitch_dif_speed_ave;  //陀螺仪pitch轴经过均值滤波后差分速度
	float roll_dif_speed_ave;   //陀螺仪roll轴经过均值滤波后差分速度
	float yaw_dif_speed_ave;    //陀螺仪yaw轴经过均值滤波后差分速度
}imu_base_info_t;

/* 陀螺仪信息 */
typedef struct
{
	uint16_t init_cnt;
}imu_info_t;
 
/* 陀螺仪 */
typedef struct
{
	imu_config_t *config;
	imu_base_info_t *base_info;
	imu_info_t *info;
}imu_t;

extern imu_t imu;

void imu_init(imu_t *imu);
void imu_info_init(imu_info_t *info);
void imu_update(imu_t *imu);

#endif
