#ifndef __MY_LAUNCH_H
#define __MY_LAUNCH_H

#include "stm32f4xx_hal.h"
#include "motor.h"
#include "stdbool.h"

#define LAUNCH_SLEEP_RPOCESS_TIME 1000
#define LAUNCH_SLEEP_ALLOW_SPEED 200
#define LAUNCH_FRIC_MAX_ALLOW_SPEED 8000
#define LAUNCH_L_FRIC_SPEED_OFFSET 0
#define LAUNCH_R_FRIC_SPEED_OFFSET 0
#define LAUNCH_FRIC_ORG_SPEED_16 5700
#define LAUNCH_FRIC_ORG_SPEED_10 4080
#define LAUNCH_FRIC_ORG_SPEED_KILL 8000
#define LAUNCH_FRIC_READY_SPEED 2000
#define LAUNCH_DIAL_INIT_F_SPEED 2000
#define LAUNCH_DIAL_INIT_B_SPEED -1000
#define LAUNCH_DIAL_F_INIT_TIME 150
#define LAUNCH_DIAL_B_INIT_TIME 2500
#define LAUNCH_DIAL_STUCK_B_POSITION 12452
#define LAUNCH_DIAL_RELOAD_ONE_ALLOW_TIME 400 //0.4s
#define LAUNCH_DIAL_RELOAD_TWO_ALLOW_TIME 500 //0.5s
#define LAUNCH_DIAL_RELOAD_THREE_ALLOW_TIME 600 //0.6s
#define LAUNCH_DIAL_RELOAD_TEN_ALLOW_TIME 2000 //2s
#define LAUNCH_DIAL_STUCK_PROCESS_TIMES 2
#define LAUNCH_POSITION_INIT_SPEED 4000
#define LAUNCH_POSITION_INIT_TIME 400
#define LAUNCH_FEED_WAKE_RELOAD_TIMES 12
#define LAUNCH_DIAL_WAKE_DELAY_TIME 300
#define LAUNCH_POSITION_RELOAD_ONE_TIME 220//220//旧飞坡180
#define LAUNCH_POSITION_RELOAD_TWO_TIME 350
#define LAUNCH_POSITION_RELOAD_THREE_TIME 520
#define LAUNCH_POSITION_RELOAD_TEN_TIME 2500
#define LAUNCH_POSITION_RELOAD_SPEED -8000//-8000
#define DIAL_RELOAD_ONE_ANGLE 33480
#define DIAL_RELOAD_TWO_ANGLE 60000
#define DIAL_RELOAD_THREE_ANGLE 72000
#define DIAL_RELOAD_TEN_ANGLE 180000
#define DIAL_RELOAD_OFFSET 0
#define FIRE_PERFECT_RATE_MAX 15.05f
#define FIRE_PERFECT_RATE_LOW 14.75f
#define FRIC_RESCUE_SPEED 2500
#define FRIC_RESCUE_ALLOW_TIME 1000


typedef enum 
{
	L_Sleep,
	L_Stop,
	L_Init,
	L_Wake,
  L_Shoot,	
	L_Rescue,
	L_Standby,
}Launch_Mode_e;

typedef enum
{
	F_Sleep,
	F_Init,
	F_Reload,
	F_Static,
}Feed_Mode_e;

typedef struct
{
	int16_t l_fric_speed;
	int16_t r_fric_speed;
	int16_t position_speed;
	int16_t dial_speed;
	int32_t dial_position;
}Launch_Measure_t;

typedef struct
{
	int16_t l_fric_speed;
	int16_t r_fric_speed;
	int16_t position_speed;
	int16_t dial_speed;
	int32_t dial_position;
}Launch_Target_t;

typedef struct
{
	bool Dial_Init_Flag;
	bool Position_Init_Flag;
	bool Dial_Wake_OK;//拨盘唤醒完成状态标志位
	bool Dial_Processing_Flag;//拨盘转动过程标志位
	bool Dial_Stuck_Processing_Flag;//拨盘处理堵转过程标志位
	bool Position_Reload_OK;//限位供弹流程结束标志位
	bool Dial_Reload_OK;//拨盘供弹结束标志位
	bool Dial_Wake_Want_To_Stop;
	bool Position_Processing_Flag;
	bool Fric_Stuck_Flag;
}Launch_Status_Flag_t;

typedef struct
{
	int16_t client_modify_fric_speed;//操作手改摩擦轮转速用
	int16_t fric_base_speed;
	int16_t fire_rate_fix;
	int32_t dial_delta_offset;//拨盘误差补偿，初始化时清零
	int32_t dial_final_target;//拨盘处理堵转时用
	int32_t dial_target_reserve;//堵得没办法时返回原来位置，返回时又堵了。。。
	uint16_t feed_reload_bullet_num;//请求打弹个数
	uint32_t position_reload_max;//根据请求打弹个数不同，限位运动的时间不同
	uint32_t dial_reload_angle;//根据请求打弹个数不同，拨盘转动角度不同
	uint32_t feed_reload_max_time;
	
	Launch_Status_Flag_t* flag;
	Launch_Measure_t* measure;
	Launch_Target_t* target;
}Launch_Info_t;

typedef struct
{
	uint32_t sleep_cnt;
	uint32_t position_init_cnt;
	uint32_t dial_init_cnt;
	uint32_t dial_stuck_cnt;//堵转处理次数
	uint32_t dial_reload_allow_cnt;//拨盘供弹状态超时处理
	uint32_t dial_wake_reload_cnt;//唤醒补弹的补弹个数，13个可将空弹链推到限位
	uint32_t dial_wake_delay_cnt;//唤醒状态下，拨盘每发之间的供弹间隔，防太快卡弹
	uint32_t position_reload_time;//限位供弹时长，与速度一起对应走的路程
	uint32_t position_delay_dial_cnt;//限位相对拨盘的启动延时，保证拨盘把弹顶到限位后限位轮再转动
  uint32_t fric_test_fire_cnt;
	int32_t dial_test_fire_cnt;
	int32_t fric_test_fire_delay_cnt;
	uint32_t fric_rescue_cnt;
	uint32_t rate_below_perfect;
	uint32_t rate_above_perfect;
}Launch_Cnt_t;

typedef struct My_Launch_struct_t
{
	Launch_Cnt_t* cnt;
	
	Launch_Info_t* info;
	
	Launch_Mode_e launch_mode;
	
	Feed_Mode_e feed_mode;
	
	motor_t* feed;
	
	motor_t* position;
	
	motor_t* left_fric;
	
	motor_t* right_fric;
}My_Launch_t;

extern My_Launch_t My_Launch;

void My_Launch_Init(void);
void My_Launch_Heartbeat(void);
void Launch_L_Fric_Ctrl(My_Launch_t* my_launch);
void Launch_R_Fric_Ctrl(My_Launch_t* my_launch);
uint8_t My_Launch_Ready_Check(My_Launch_t* my_launch);
uint8_t My_Launch_Feed_Ready_Check(My_Launch_t* my_launch);
void My_Launcher_Work(void);
void My_Feeder_Control(My_Launch_t* my_launch);
void Launch_L_Fric_Ctrl(My_Launch_t* my_launch);
void Launch_R_Fric_Ctrl(My_Launch_t* my_launch);
void Position_Ctrl(My_Launch_t* my_launch);
void Dial_Position_Ctrl(My_Launch_t* my_launch);
void Dial_Speed_Ctrl(My_Launch_t* my_launch);
void My_Launch_Shoot_Times_Check(My_Launch_t* my_launch);
uint16_t My_Launch_Clear_Heat_Cal(My_Launch_t* my_launch);
bool My_Launch_Fire_Limit_Check(My_Launch_t* my_launch);
void My_Launch_Rate_of_Fire_Limit_Check(My_Launch_t* my_launch);
void My_Launch_Rate_of_Fire_Self_Fix(My_Launch_t* my_launch);
void Fric_Stuck_Rescue(My_Launch_t* my_launch);

#endif

