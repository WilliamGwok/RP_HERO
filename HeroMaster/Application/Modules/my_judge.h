#ifndef __MY_JUDGE_H
#define __MY_JUDGE_H

#include "stm32f4xx_hal.h"

#define JUDGE_OFFLINE_CNT_MAX 1000

typedef struct
{
	int16_t chassis_power_buffer;           //底盘缓存功率
	int32_t chassis_out_put_max;            //底盘最大输出
	uint16_t shooter_cooling_limit;					//机器人 42mm 枪口热量上限
	uint16_t shooter_cooling_heat; 					//机器人 42mm 枪口热量
	uint8_t car_color;                      //2蓝色 1红色
	uint8_t hurt_type;                      //伤害种类
	uint16_t chassis_power_limit;           //底盘功率限制
	uint16_t shooter_id1_42mm_speed_limit;  //射速上限
	uint16_t remain_HP;                     //剩余血量
	uint8_t game_status;                    //比赛状态
	uint16_t remain_HP_now;
	uint16_t remain_HP_last;
	uint8_t rfid;
	float shooting_speed;
}Judge_Info_t;

typedef struct
{
	uint16_t offline_cnt_max;
	uint8_t status;
	uint16_t offline_cnt;
}Judge_Status_t;

typedef struct
{
	Judge_Info_t* info;
	Judge_Status_t* status;
}My_Judge_t;

extern My_Judge_t My_Judge;

void My_Judge_Init(void);
void My_Judge_Realtime_Task(My_Judge_t* my_judge);
void My_Judge_Update(My_Judge_t * my_judge);

#endif

