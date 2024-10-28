#ifndef __MY_GIMBAL_H
#define __MY_GIMBAL_H

#include "stm32f4xx_hal.h"
#include "motor.h"
#include "stdbool.h"

#define GIMBAL_YAW_MIDDLE_ANGLE 5000//长沙 6666 深圳1468
#define GIMBAL_PITCH_MIDDLE_ANGLE 4595//长沙7793 深圳4595
#define GIMBAL_PITCH_MEC_SUP_ANGLE 730
#define GIMBAL_PITCH_MEC_LOW_ANGLE -450
#define GIMBAL_PITCH_IMU_SUP_ANGLE 38
#define GIMBAL_PITCH_IMU_LOW_ANGLE -25
#define GIMBAL_INIT_RPOCESS_TIME 1500
#define GIMBAL_INIT_ANGLE_ERR 100
#define GIMBAL_HEAD_SWAP_ANGLE 2

typedef enum
{
	G_Sleep,
	G_Init,
	G_Imu,
	G_Mec,
	G_Auto,
}Gimbal_Mode_e;

typedef struct 
{
	float yaw_mec_angle;
	float pitch_mec_angle;
	float yaw_imu_angle;
	float pitch_imu_angle;
}Gimbal_Target_t;

typedef struct
{
	int16_t yaw_mec_angle; //-4096~4096 顺时针为正
	int16_t pitch_mec_angle; //-4096~4096 向上为正
	int16_t yaw_mec_speed; //顺时针为正
	int16_t pitch_mec_speed; //向上为正
	
	float yaw_imu_angle; //-180~180 顺时针为正
	float pitch_imu_angle; //-180~180 向上为正
	float yaw_imu_speed; //顺时针为正
	float pitch_imu_speed; //向上为正
	
}Gimbal_Measure_t;

typedef struct
{
	float Back_Control;
	float yaw_delta;
}Gimbal_For_Chassis_t;

typedef struct
{
	bool exam;
}Gimbal_For_Vision_t;

typedef struct
{
	int16_t pitch_mec_angle;
}Gimbal_For_Launch_t;

typedef struct
{
	uint32_t gimbal_danger_offline_cnt;
	uint32_t init_cnt;
	uint32_t head_swap_cnt;
}Gimbal_Cnt_t;

typedef struct
{
	int16_t yaw_middle_angle_config;
	
	int16_t pitch_middle_angle_config;
	
	Gimbal_Target_t* target;
	
	Gimbal_Measure_t* measure;
	
	Gimbal_For_Chassis_t* gimbal_to_chassis;
	
	Gimbal_For_Vision_t* gimbal_to_vision;
	
	Gimbal_For_Launch_t* gimbal_to_launch;
}Gimbal_Info_t;

typedef struct My_Gimbal_struct_t
{
	Gimbal_Mode_e mode;
	
	Gimbal_Cnt_t* cnt;
	
	motor_t* yaw_motor;
	
	motor_t* pitch_motor;

	Gimbal_Info_t* info;
}My_Gimbal_t;

extern My_Gimbal_t My_Gimbal;

void My_Gimbal_Init(void);
void My_Gimbal_Heartbeat(void);
void My_Gimbal_Command_React(My_Gimbal_t* my_gimbal);
void My_Gimbal_Status_React(My_Gimbal_t* my_gimbal);
void My_Gimbal_Control(My_Gimbal_t* my_gimbal);
void My_Gimbal_Work(void);
void My_Gimbal_Data_Update(My_Gimbal_t* my_gimbal);
void My_Gimbal_Yaw_Angle_Check(My_Gimbal_t* my_gimbal);
void My_Gimbal_Pitch_Angle_Check(My_Gimbal_t* my_gimbal);
void Gimbal_Yaw_Mec_Ctrl(My_Gimbal_t* my_gimbal);
void Gimbal_Pitch_Mec_Ctrl(My_Gimbal_t* my_gimbal);
void Gimbal_Yaw_Imu_Ctrl(My_Gimbal_t* my_gimbal);
void Gimbal_Pitch_Imu_Ctrl(My_Gimbal_t* my_gimbal);

#endif

