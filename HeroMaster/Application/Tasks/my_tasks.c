#include "my_tasks.h"
#include "cmsis_os.h"
#include "remote.h"
#include "imu.h"
#include "config_can.h"
#include "my_hero.h"
#include "my_chassis.h"
#include "my_gimbal.h"
#include "my_launch.h"
#include "my_vision.h"
#include "string.h"
#include "my_judge.h"
#include "my_vision.h"
#include "my_slave.h"

extern IWDG_HandleTypeDef hiwdg;

void Task_H1(void const * argument)
{
	/*裁判系统数据，软件复位*/
  for(;;)
  {
		rc_ctrl(&rc);
		
		rc_tick_task(&rc);
		
		My_Judge_Realtime_Task(&My_Judge);
		
		My_Vision_Tick_Task(&My_Vision);
		
    osDelay(1);
  }
	
}



void Task_H2(void const * argument)
{
	/*失联检测，视觉交互，车间通讯*/
  for(;;)
  {
		imu_update(&imu);
		
		Hero_Heartbeat();
		
    osDelay(1);
  }
}

void Task_M(void const * argument)
{
	static int16_t vision_delay_cnt = 0;
	/*车间通讯,车辆控制*/
  for(;;)
  {
		Hero_Status_Update(&HERO);
		
		if(vision_delay_cnt >= 5)
		{
			My_Vision_Transmit(&My_Vision);
			vision_delay_cnt = 0;
		}
		else
		{
			vision_delay_cnt++;
		}
			
		My_Gimbal_Work();
		
		My_Chassis_Work();
		
		My_Launcher_Work();
		
		CAN1_Send_With_int16_to_uint8(0x1FF,can1_0x1FF_send_buff);
		CAN2_Send_With_int16_to_uint8(0x1FF,can2_0x1FF_send_buff);
		CAN1_Send_With_int16_to_uint8(0x200,can1_0x200_send_buff);
	  CAN2_Send_With_int16_to_uint8(0x200,can2_0x200_send_buff);
		
		My_Slave_Transmit_Task();
		
		HAL_IWDG_Refresh(&hiwdg);
		
    osDelay(1);
  }

}
