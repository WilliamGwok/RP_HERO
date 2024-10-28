#include "drv_all.h"
#include "drv_can.h"
#include "drv_uart.h"
#include "remote.h"
#include "imu.h"
#include "my_chassis.h"
#include "my_gimbal.h"
#include "my_launch.h"
#include "my_judge.h"
#include "my_vision.h"

void Driver_Init(void)
{
  CAN1_Init();
	
  CAN2_Init();
	
	USART1_Init();
	
	USART2_Init();
	
	USART3_Init();
	
	USART4_Init();
	
	USART5_Init();
}

void Device_Init(void)
{	
	imu_init(&imu);
	
  rc_init(&rc);
	
	My_Chassis_Init();
	
	My_Gimbal_Init();
	
	My_Launch_Init();	
	
	My_Judge_Init();
	
	My_Vision_Init();
}

