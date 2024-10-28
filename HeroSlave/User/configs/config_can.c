#include "config_can.h"
#include "cap_protocol.h"
#include "cap.h"
#include "my_judge.h"
#include "string.h"

/*slave_code_1 个位为视觉标志位 0~9
               十位为整车控制位 0~9
               百位为超电开启标志位 1*/

/*slave_code_2 个位为摩擦轮标志位 0~9
               十位为换头标志位 0~9
               百位为小陀螺标志位 1*/
uint8_t master_code_1 = 0;
uint8_t master_code_2 = 0;
uint8_t master_data[8];

void CAN1_Get_Data(uint32_t id, uint8_t *data)
{
	switch(id)
	{
		case 0x0FF:
			memcpy(master_data, data, sizeof(&data));
			master_code_1 = data[0];
		
		  master_code_2 = data[1];
		
		
		  global_receive.vision_status = master_code_1 % 10;
		  
		  global_receive.hero_control_mode = master_code_1 / 10 % 10;
		
		  
		  if(master_code_1 >= 100)
			{
				cap.normal_switch = 1;
			}
			else
			{
				cap.normal_switch = 0;
			}
			
			global_receive.friction_status = master_code_2 % 10;
			
			if((master_code_2 / 10 % 10) == 1)
			{
				global_receive.head_change = 1;
			}
			else
			{
				global_receive.head_change = 0;
			}
			
			if(master_code_2 >= 100)
			{
				global_receive.top_mode = 1;
			}
			else
			{
				global_receive.top_mode = 0;
			}
			
			global_receive.UI_x_last = global_receive.UI_x;
      global_receive.UI_y_last = global_receive.UI_y;		
			
			memcpy(&global_receive.UI_x, &data[2], 2);
		  memcpy(&global_receive.UI_y, &data[4], 2);
			
			global_receive.fric_speed = (int16_t)(data[6]<<8 | data[7]); 
			break;
		case 0x0FE:
			memcpy(&global_receive.pitch_angle, &data[0], 4);
		  memcpy(&global_receive.ROI_Mode, &data[4], 1);
		  global_receive.Accelerate = data[5];
		  global_receive.vision_detect_num = data[6];
		  global_receive.stop_hit = data[7];
			break;
		default:
			break;
	}
}

void CAN2_Get_Data(uint32_t id, uint8_t *data)
{
	switch(id)
	{
		case 0x030:
			cap_receive(data);
			break;
		default:
			break;
	}
}

