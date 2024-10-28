#include "serve_protocol.h"
#include "string.h"
#include "judge.h"
#include "drv_can.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

uint8_t serve_buf_1[8];//power_heat
uint8_t serve_buf_2[8];//game_robot_status
uint8_t serve_buf_3[8];//shoot_data
uint8_t serve_buf_4[8];//game_robot_pos
uint8_t serve_buf_5[8];//robot_hurt

void trans_to_master_powerheat(void)
{
	uint8_t rfid = judge.judge_info->rfid_status.rfid_status;
	memcpy(&serve_buf_1[0],(void*)&judge.judge_info->power_heat_data.chassis_power_buffer,2);
	memcpy(&serve_buf_1[2],(void*)&judge.judge_info->power_heat_data.shooter_id1_42mm_cooling_heat,2);
	if(rfid == 0x02)
	{
		serve_buf_1[4] = 1;
	}
	else
	{
		serve_buf_1[4] = 0;
	}
	/*Õû³µÊ£ÓàÑªÁ¿*/
	memcpy(&serve_buf_1[5], (void*)&judge.judge_info->game_robot_status.remain_HP,2);
	uint8_t game_pro = judge.judge_info->game_status.game_progress;
	memcpy(&serve_buf_1[7], (void*)&game_pro, 1);
	CAN_SendData(&hcan1, 0x100, serve_buf_1);
}

void trans_to_master_robot_status(void)
{
	memcpy(&serve_buf_2[0],(void*)&judge.judge_info->game_robot_status.shooter_id1_42mm_cooling_limit,2);
	if(judge.judge_info->game_robot_status.robot_id == 1)
	{
		serve_buf_2[2] = 0;
	}
	else if(judge.judge_info->game_robot_status.robot_id == 101)
	{
		serve_buf_2[2] = 1;
	}
//	memcpy(&serve_buf_2[2],(void*)&judge.judge_info->game_robot_status.,2);
	memcpy(&serve_buf_2[3],(void*)&judge.judge_info->game_robot_status.chassis_power_limit,2);
	memcpy(&serve_buf_2[5],(void*)&judge.judge_info->game_robot_status.shooter_id1_42mm_speed_limit,2);
	CAN_SendData(&hcan1, 0x101, serve_buf_2);
}

void trans_to_master_shoot_data(void)
{
	memcpy(serve_buf_3, (void*)&judge.judge_info->shoot_data.bullet_speed, 4);
	CAN_SendData(&hcan1, 0x102, serve_buf_3);
}

void trans_to_master_robot_pos(void)
{
	memcpy(serve_buf_4, (void*)&judge.judge_info->game_robot_pos.yaw, 4);
	CAN_SendData(&hcan1, 0x103, serve_buf_4);
}

void trans_to_master_robot_hurt(void)
{
	if(judge.judge_info->ext_robot_hurt.hurt_type == 0x02 || \
		 judge.judge_info->ext_robot_hurt.hurt_type == 0x04)
	{
		serve_buf_5[0] = judge.judge_info->ext_robot_hurt.hurt_type;
		CAN_SendData(&hcan1, 0x104, serve_buf_5);
	}
}
