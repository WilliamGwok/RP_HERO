#include "cap_protocol.h"
#include "drv_can.h"
#include "judge.h"
#include "cap.h"
#include "string.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

uint8_t cap_send_buff_2E[8];//0x2E
uint8_t cap_send_buff_2F[8];//0x2F

cap_receive_data_t cap_receive_data;

void cap_2E_send(void)
{
	cap_send_buff_2E[0] = judge.judge_info->power_heat_data.chassis_power_buffer >> 8;
	cap_send_buff_2E[1] = judge.judge_info->power_heat_data.chassis_power_buffer;
	cap_send_buff_2E[2] = judge.judge_info->power_heat_data.chassis_volt >> 8;
	cap_send_buff_2E[3] = judge.judge_info->power_heat_data.chassis_volt;
	cap_send_buff_2E[4] = judge.judge_info->power_heat_data.chassis_current >> 8;
	cap_send_buff_2E[5] = judge.judge_info->power_heat_data.chassis_current;
	CAN_SendData(&hcan2,0x2E,cap_send_buff_2E);
}

void cap_2F_send(void)
{
	uint16_t temp = 0;
	
	cap_send_buff_2F[0] = judge.judge_info->game_robot_status.chassis_power_limit >> 8;
	cap_send_buff_2F[1] = judge.judge_info->game_robot_status.chassis_power_limit;
	
	temp = 300;//限制电容放电功率
	cap_send_buff_2F[2] = temp >> 8;
	cap_send_buff_2F[3] = temp;
	
	temp = 150;//限制电容充电功率
	cap_send_buff_2F[4] = temp >> 8;
	cap_send_buff_2F[5] = temp;
	
	if(cap.normal_switch == 1)
	{
		if(cap.record_switch == 1)
		{
			temp = 0x0700;
		}
		else
		{
			temp = 0x0300;
		}
	}
	else
	{
		temp = 0x0000;
	}
	memcpy(&cap_send_buff_2F[6],&temp,2);
	CAN_SendData(&hcan2,0x2F,cap_send_buff_2F);
}

void cap_receive(uint8_t *data)
{
	cap_receive_data.cap_voltage = int16_to_float((uint16_t)(data[0] << 8 | data[1]),32000,-32000,30,0);
	cap_receive_data.cap_current = int16_to_float((uint16_t)(data[2] << 8 | data[3]),32000,-32000,20,-20);
	cap_receive_data.cap_state.state = (uint16_t)(data[4] << 8 | data[5]);
}

int16_t float_to_int16(float a, float a_max, float a_min, int16_t b_max, int16_t b_min)
{
    int16_t b = (a - a_min) / (a_max - a_min) * (float)(b_max - b_min) + (float)b_min + 0.5f;   //加0.5使向下取整变成四舍五入
    return b;
}

float int16_to_float(int16_t a, int16_t a_max, int16_t a_min, float b_max, float b_min)
{
    float b = (float)(a - a_min) / (float)(a_max - a_min) * (b_max - b_min) + b_min;
    return b;
}

