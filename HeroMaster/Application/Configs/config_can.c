#include "config_can.h"
#include "drv_haltick.h"
#include "chassis_motor.h"
#include "gimbal_motor.h"
#include "launch_motor.h"
#include "config.h"
#include "my_judge.h"
#include "string.h"

//200为前4个，1FF为后4个
int16_t can1_0x200_send_buff[4];
int16_t can2_0x200_send_buff[4];
int16_t can1_0x1FF_send_buff[4];
int16_t can2_0x1FF_send_buff[4];

void CAN1_rxDataHandler(uint32_t canId, uint8_t *rxBuf)
{
	switch(canId)
	{
		/*底盘 begin*/
		case chassis_motor_LF_id:
			Chassis_Motor[LF_WHEEL].rx(&Chassis_Motor[LF_WHEEL],rxBuf,8);
		  break;
		case chassis_motor_RF_id:
			Chassis_Motor[RF_WHEEL].rx(&Chassis_Motor[RF_WHEEL],rxBuf,8);
		  break;
		case chassis_motor_LB_id:
			Chassis_Motor[LB_WHEEL].rx(&Chassis_Motor[LB_WHEEL],rxBuf,8);
		  break;
		case chassis_motor_RB_id:
			Chassis_Motor[RB_WHEEL].rx(&Chassis_Motor[RB_WHEEL],rxBuf,8);
		  break;
		/*底盘 end*/
		
		/*云台 begin*/
		case yaw_motor_id:
			Gimbal_Motor[YAW_GIMBAL].rx(&Gimbal_Motor[YAW_GIMBAL],rxBuf,8);
		  break;
		/*云台 end*/
		
		/*发射 begin*/
		case feed_motor_id:
			Launch_Motor[DIAL_LAUNCH].rx(&Launch_Motor[DIAL_LAUNCH],rxBuf,8);
		  break;
		/*发射 end*/
		
		/*裁判系统 begin*/
		case power_heat_data_id:
			memcpy(&My_Judge.info->chassis_power_buffer,&rxBuf[0],2);
		  memcpy(&My_Judge.info->shooter_cooling_heat,&rxBuf[2],2);
		  memcpy(&My_Judge.info->rfid,&rxBuf[4],1);
		  memcpy(&My_Judge.info->remain_HP, &rxBuf[5], 2);
		  memcpy(&My_Judge.info->game_status, &rxBuf[7], 1);
		  My_Judge.status->offline_cnt = 0;
		  My_Judge.status->status = DEV_ONLINE;
			break;
		case game_robot_status_id:
			memcpy(&My_Judge.info->shooter_cooling_limit,&rxBuf[0],2);
		  memcpy(&My_Judge.info->car_color,&rxBuf[2],1);
		  memcpy(&My_Judge.info->chassis_power_limit,&rxBuf[3],2);
		  memcpy(&My_Judge.info->shooter_id1_42mm_speed_limit,&rxBuf[5],2);
		  My_Judge.status->offline_cnt = 0;
		  My_Judge.status->status = DEV_ONLINE;
			break;
		case shoot_data_id:
			memcpy(&My_Judge.info->shooting_speed,&rxBuf[0],4);
		  break;
		/*裁判系统 end*/
		default:
			break;
	}
}

void CAN2_rxDataHandler(uint32_t canId, uint8_t *rxBuf)
{
	switch(canId)
	{
		/*云台 begin*/
		case pitch_motor_id:
			Gimbal_Motor[PITCH_GIMBAL].rx(&Gimbal_Motor[PITCH_GIMBAL],rxBuf,8);
		  break;
		/*云台 end*/
		
		/*发射 begin*/
		case position_motor_id:
			Launch_Motor[POSITION_LAUNCH].rx(&Launch_Motor[POSITION_LAUNCH],rxBuf,8);
		  break;
		case left_fric_motor_id:
			Launch_Motor[LEFT_FRIC_LAUNCH].rx(&Launch_Motor[LEFT_FRIC_LAUNCH],rxBuf,8);
		  break;
		case right_fric_motor_id:
			Launch_Motor[RIGHT_FRIC_LAUNCH].rx(&Launch_Motor[RIGHT_FRIC_LAUNCH],rxBuf,8);
		  break;
		/*发射 end*/
		default:
			break;
	}
}

