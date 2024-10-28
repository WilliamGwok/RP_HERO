#ifndef __JUDGE_H
#define __JUDGE_H

#include "judge_protocol.h"
#include "stdbool.h"

typedef struct JUDGE_SEND_INFO
{
	ext_rfid_status_t rfid_status;
	ext_game_status_t game_status;
	ext_game_robot_status_t game_robot_status;
	ext_power_heat_data_t power_heat_data;
	ext_shoot_data_t shoot_data;
	ext_game_robot_pos_t game_robot_pos;
	ext_robot_hurt_t ext_robot_hurt;
}judge_send_info_t;

typedef struct RADAR_INFO
{
	int16_t receive_id;
	
	bool home_attacked;
}radar_info_t;

typedef struct JUDGE
{
	judge_send_info_t *judge_info;
}judge_t;

extern judge_t judge;
extern radar_info_t radar_info;

#endif
