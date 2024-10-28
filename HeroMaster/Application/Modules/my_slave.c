#include "my_slave.h"
#include "my_hero.h"
#include "my_chassis.h"
#include "my_vision.h"
#include "my_launch.h"
#include "my_gimbal.h"
#include "config.h"
#include "math_support.h"
#include "string.h"
#include "drv_can.h"

uint8_t Slave_Buff_1[8];
uint8_t Slave_Buff_2[8];
Slave_Work_Status_t Slave_Work_Status;
Slave_Pack_t Slave_Pack;
My_Slave_t My_Slave = 
{
	.status = &Slave_Work_Status,
	.pack = &Slave_Pack,
};

extern CAN_HandleTypeDef hcan1;


uint8_t slave_code_1 = 0;
uint8_t slave_code_2 = 0;
uint32_t can_send_delay_cnt = 0;

/*slave_code_1 个位为视觉标志位 0~9
               十位为整车控制位 0~9
               百位为超电开启标志位 1*/

/*slave_code_2 个位为摩擦轮标志位 0~9
               十位为换头标志位 0~9
               百位为小陀螺标志位 1*/

void My_Slave_Transmit_Task(void)
{
	/*每次都更新*/
	slave_code_1 = 0;
	slave_code_2 = 0;
	
	/*发送视觉状态标志位*/
	if(My_Vision.info->status == DEV_ONLINE)
	{
		if(HERO.command->vision->Auto_Catch == true)
		{
			slave_code_1 = 1;
		}
		if(My_Vision.base_info->detect_mode == 2)//优先级高于Auto_Catch
		{
			slave_code_1 = 2;
		}
		if(HERO.command->vision->Auto_Catch == false && My_Vision.base_info->detect_mode != 2)
		{
			slave_code_1 = 0;
		}
	}
	else
	{
		slave_code_1 = 0;
	}
	
	/*3 发送整车控制标志位 0为imu或其它， 1为mec*/
	if(HERO.mode == Mec_Mode)
	{
		slave_code_1 += 10;
	}
	else if(HERO.mode == Imu_Mode)
	{
		slave_code_1 += 20;
	}
	else
	{
		slave_code_1 += 0;
	}
	
	/*开启超电*/
	slave_code_1 += 100;
	
	Slave_Buff_1[0] = slave_code_1;
	
	
	
	/*发送摩擦轮状态标志位*/
	if(My_Launch.left_fric->state.work_state == M_OFFLINE || My_Launch.right_fric->state.work_state == M_OFFLINE)
	{
		slave_code_2 = 0;
	}
	else
	{
		if(abs(My_Launch.left_fric->rx_info.speed) >= 3000 && abs(My_Launch.right_fric->rx_info.speed) >= 3000)
		{
			slave_code_2 = 2;
		}
		else
		{
			slave_code_2 = 1;
		}
	}
	
	if(My_Gimbal.info->gimbal_to_chassis->Back_Control == -1)
	{
		slave_code_2 += 10;
	}
	else
	{
		slave_code_2 += 0;
	}
	
	/*发送小陀螺标志位*/
	if(My_Chassis.mode->Spin_Mode == C_S_top)
	{
		slave_code_2 += 100; 
	}
	else
	{
		slave_code_2 += 0; 
	}
	
	Slave_Buff_1[1] = slave_code_2;
	
	
	if(My_Vision.base_info->capture_Y_O_N != 0)
	{
		memcpy(&Slave_Buff_1[2], &My_Vision.base_info->UI_x, 2);
	
	  memcpy(&Slave_Buff_1[4], &My_Vision.base_info->UI_y, 2);
	}
	else
	{
		memset(&Slave_Buff_1[2], 0, 4);
	}
	
	/*6 7 发送摩擦轮实时转速*/
	Slave_Buff_1[6] = (uint8_t)((int16_t)My_Launch.info->target->r_fric_speed >> 8);
	
	Slave_Buff_1[7] = (uint8_t)((int16_t)My_Launch.info->target->r_fric_speed);

	memcpy(&Slave_Buff_2[0], &My_Gimbal.info->measure->pitch_imu_angle, sizeof(My_Gimbal.info->measure->pitch_imu_angle));
	
	/*ROI*/
	if(HERO.command->vision->ROI_On == true && My_Vision.info->status == DEV_ONLINE)
	{
		Slave_Buff_2[4] = 1;
	}
	else
	{
		Slave_Buff_2[4] = 0;
	}
	
	if(HERO.command->chassis->Accelerate == true)
	{
		Slave_Buff_2[5] = 1;
	}
	else
	{
		Slave_Buff_2[5] = 0;
	}
	
	Slave_Buff_2[6] = My_Vision.rx_info->datau8_5;
	
	if(HERO.command->gimbal->Stop_Right_Hit == true)
	{
		Slave_Buff_2[7] = 2;
	}
	else if(HERO.command->gimbal->Stop_Left_Hit == true)
	{
		Slave_Buff_2[7] = 1;
	}
	else
	{
		Slave_Buff_2[7] = 0;
	}
	
	/*降低发送频率,2ms发一次*/
	switch(can_send_delay_cnt)
	{
		case 0:
			CAN_SendData(&hcan1, 0x0FF, Slave_Buff_1);
		  can_send_delay_cnt = 1;
			break;
		case 1:
			CAN_SendData(&hcan1, 0x0FE, Slave_Buff_2);
			can_send_delay_cnt = 0;
			break;
		default:
			break;
	}
}
