#ifndef __MY_CHASSIC_H
#define __MY_CHASSIC_H

#include "stm32f4xx_hal.h"
#include "motor.h"
#include "stdbool.h"
#include "rp_chassis.h"

#define CHASSIS_MAX_SPEED 8000
#define CHASSIS_STRAIGHT_RECTIFY 0.1f
#define CHASSIS_TOP_SPEED 3200
#define CHASSIS_SLEEP_RPOCESS_TIME 1500
#define CHASSIS_SLEEP_ALLOW_SPEED 200

typedef enum
{
	C_M_sleep,
	C_M_stop,
	C_M_normal,
	C_M_special,
}Chassis_Move_Mode_e;

typedef enum
{
	C_S_sleep,
	C_S_stop,
	C_S_normal,
	C_S_follow,
	C_S_top,
}Chassis_Spin_Mode_e;

typedef struct
{
	int16_t ch3_now;
	int16_t ch3_last;
	int16_t ch3_process;
	int16_t ch2_now;
	int16_t ch2_last;
	int16_t ch2_process;
}Chassis_RC_Input_t;

typedef struct
{
	int16_t front_speed;
	int16_t right_speed;
	int16_t cycle_speed;
}Chassis_Speed_Measure_t;

typedef struct
{
	uint32_t sleep_cnt;
	
}Chassis_Cnt_t;

typedef struct
{
	float speed_up_buff;
	
	Chassis_RC_Input_t* rc_input;
	
	Chassis_Speed_Measure_t* speed_measure;
	
}Chassis_Info_t;

typedef struct
{
	Chassis_Move_Mode_e Move_Mode;
	
	Chassis_Spin_Mode_e Spin_Mode;
}Chassis_Mode_t;

typedef struct
{
	Chassis_Mode_t* mode;
	
	Chassis_Info_t* info;
	
	Chassis_Cnt_t* cnt;
	
	chassis_t* rp_pack;
}My_Chassis_t;

extern My_Chassis_t My_Chassis;

void My_Chassis_Init(void);
void My_Chassis_Heartbeat(void);
void My_Chassis_Input(My_Chassis_t* my_chassis);
void My_Chassis_Speed_Update(My_Chassis_t* my_chassis);
void My_Chassis_Speed_Limit(My_Chassis_t* my_chassis);

void My_Chassis_Work(void);

#endif
