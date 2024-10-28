#include "config_judge.h"
#include "judge_protocol.h"
#include "string.h"
#include "judge.h"
#include "cap.h"
#include "cap_protocol.h"
#include "serve_protocol.h"
#include "my_judge.h"

extern uint8_t cap_send_buff_2E[8];//0x2E
extern uint8_t cap_send_buff_2F[8];//0x2F

void Armor_Hurt_Detect(void);

void judge_update(uint16_t id,uint8_t *rxbuf)
{
	switch(id)
	{
		case ID_rfid_status:
			memcpy(&judge.judge_info->rfid_status, rxbuf, LEN_rfid_status);
			break;
		case ID_game_state:
			memcpy(&judge.judge_info->game_status, rxbuf, LEN_game_state);
			if(judge.judge_info->game_status.game_progress == 0x04)
			{
				cap.record_switch = 1;
			}
			else 
			{
				cap.record_switch = 0;
			}
			break;
		case ID_power_heat_data:
			memcpy(&judge.judge_info->power_heat_data, rxbuf, LEN_power_heat_data);
			cap_2E_send();
			trans_to_master_powerheat();
			break;
		case ID_game_robot_state:	
			memcpy(&judge.judge_info->game_robot_status, rxbuf, LEN_game_robot_state);
			cap_2F_send();
			trans_to_master_robot_status();
			break;
		case ID_shoot_data:
			memcpy(&judge.judge_info->shoot_data, rxbuf, LEN_shoot_data);
			trans_to_master_shoot_data();
			break;
		case ID_game_robot_pos:
			memcpy(&judge.judge_info->game_robot_pos, rxbuf, LEN_game_robot_pos);
			break;
		case ID_robot_hurt:
			memcpy(&judge.judge_info->ext_robot_hurt, rxbuf, LEN_robot_hurt);
		  Armor_Hurt_Detect();
			break;
		default:
			break;
	}
}


//从正面顺时针装甲板顺序为0，1，2，3
void Armor_Hurt_Detect(void)
{
	if(judge.judge_info->ext_robot_hurt.hurt_type == 0)
	{
		global_receive.want_draw_hurt = true;
		global_receive.armor_show_cnt = 0;
		switch(judge.judge_info->ext_robot_hurt.armor_id)
		{
			case 0:
				if(global_receive.head_change != 1)
				{
					global_receive.armor_hurt_pos_x = Client_mid_position_x;
				  global_receive.armor_hurt_pos_y = Client_mid_position_y + 300;
				}
				else
				{
					global_receive.armor_hurt_pos_x = Client_mid_position_x;
				  global_receive.armor_hurt_pos_y = Client_mid_position_y - 500;
				}
				break;
			case 3:
				if(global_receive.head_change != 1)
				{
					global_receive.armor_hurt_pos_x = Client_mid_position_x + 900;
				  global_receive.armor_hurt_pos_y = Client_mid_position_y + 20;
				}
				else
				{
					global_receive.armor_hurt_pos_x = Client_mid_position_x - 900;
				  global_receive.armor_hurt_pos_y = Client_mid_position_y + 20;
				}
				break;
			case 2:
				if(global_receive.head_change != 1)
				{
					global_receive.armor_hurt_pos_x = Client_mid_position_x;
				  global_receive.armor_hurt_pos_y = Client_mid_position_y - 500;
				}
				else
				{
					global_receive.armor_hurt_pos_x = Client_mid_position_x;
				  global_receive.armor_hurt_pos_y = Client_mid_position_y + 300;
				}
				break;
			case 1:
				if(global_receive.head_change != 1)
				{
					global_receive.armor_hurt_pos_x = Client_mid_position_x - 900;
				  global_receive.armor_hurt_pos_y = Client_mid_position_y + 20;
				}
				else
				{
					global_receive.armor_hurt_pos_x = Client_mid_position_x + 900;
				  global_receive.armor_hurt_pos_y = Client_mid_position_y + 20;
				}
				break;
			default:
				break;
		}
	}
	else
	{
		
	}
}
