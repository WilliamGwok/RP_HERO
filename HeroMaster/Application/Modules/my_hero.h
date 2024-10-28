#ifndef __MY_HERO_H
#define __MY_HERO_H

#include "stm32f4xx_hal.h"
#include "motor.h"
#include "stdbool.h"

typedef enum 
{
	RC_CTRL = 0,
	KEY_CTRL,
}Hero_Ctrl_e;

typedef enum
{
	Init_Mode = 0,
	Sleep_Mode,
	Imu_Mode,
	Mec_Mode,
}Hero_Mode_e;

/*chassis command begin*/
typedef struct Chassis_Command_struct_t
{
	bool Go_Top;
	bool Top_Speed_Up;
	bool Accelerate;
}Chassis_Command_t;
/*end*/

/*gimbal command begin*/
typedef struct Gimbal_Command_struct_t
{
	bool Trun_Right_45;
	bool Trun_Left_45;
	bool Head_Swap;
	bool Rescue_Fric_Up;
	bool Rescue_Fric_Down;
	bool Stop_Left_Hit;
	bool Stop_Right_Hit;
	bool Ctrl_Ready;
}Gimbal_Command_t;
/*end*/

/*launcher command begin*/
typedef struct Launch_Command_struct_t
{
	bool Fire;
	bool Stop;
	bool Clear_Heat;
	bool Kill_Me_Please;
	bool Ctrl_Ready;
	bool Speed_Up;
	bool Speed_Down;
}Launch_Command_t;
/*end*/

/*vision command begin*/
typedef struct Vision_Command_struct_t
{
	bool Auto_Catch;
	bool ROI_On;
}Vision_Command_t;
/*end*/

/*vision command begin*/
typedef struct Judge_Command_struct_t
{
	bool Fighting_Flag;
	bool Relive_Flag;
}Judge_Command_t;
/*end*/

/*hero command begin*/
typedef struct Hero_Command_struct_t
{
	bool Hero_Init_Flag;
	
	bool Gimbal_Init_Flag;
	
	bool Launcher_Init_Flag;
	
	Chassis_Command_t* chassis;
	
	Gimbal_Command_t* gimbal;
	
	Launch_Command_t* launch;
	
	Vision_Command_t* vision;
	
	Judge_Command_t* judge;
}Hero_Command_t;
/*end*/

typedef struct Hero_struct_t
{
	Hero_Ctrl_e ctrl;
	
	Hero_Mode_e mode;
	
	Hero_Command_t* command;
}Hero_t;

extern Hero_t HERO;

void Hero_Status_Update(Hero_t* hero);
void Hero_Heartbeat(void);

void Rc_S1_Status_Check(Hero_t* hero);
void Rc_S2_Status_Check(Hero_t* hero);
void Rc_Thumbwheel_Check(Hero_t* hero);
void Rc_Status_Scan(Hero_t* hero);
void Key_Status_Scan(Hero_t* hero);
void Vision_Debug_Check(Hero_t* hero);


#endif




