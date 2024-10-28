#include "my_hero.h"
#include "remote.h"
#include "string.h"
#include "config.h"
#include "my_chassis.h"
#include "my_gimbal.h"
#include "my_launch.h"

/*命令结构体 begin*/
Chassis_Command_t Chassis_Command;
Gimbal_Command_t Gimbal_Command;
Launch_Command_t Launch_Command = 
{
	.Fire = false,
	.Stop = false,
};
Vision_Command_t Vision_Command;
Judge_Command_t Judge_Command;
Hero_Command_t Hero_Command = 
{
	.chassis = &Chassis_Command,
	.gimbal = &Gimbal_Command,
	.launch = &Launch_Command,
	.vision = &Vision_Command,
	.judge = &Judge_Command,
};
/*命令结构体 end*/

/*整车结构体 begin*/
Hero_t HERO = 
{
	.ctrl = RC_CTRL,
	.mode = Init_Mode,
	.command = &Hero_Command,
};
/*整车结构体 end*/


/**
  * @brief  整车初始化（复位）
  * @param  None
  * @retval None
  */
void Hero_Init(void)
{
	/*全车指令清零*/
//	bool test[100];
//	
//	memset(&Hero_Command.chassis, 0, sizeof(Hero_Command.chassis));
//	memcpy(&Hero_Command.gimbal, test, sizeof(Hero_Command.gimbal));
//	memset(&Hero_Command.launch, 0, sizeof(Hero_Command.launch));
//	memset(&Hero_Command.vision, 0, sizeof(Hero_Command.vision));
	HERO.command->vision->Auto_Catch = false;
	
	/*复位成功条件 : 云台yaw，pitch到达指定角度，遥控通道清零*/
//	if(Remote_Start_Safely(&rc) && HERO.command->Gimbal_Init_Flag == true)
	
	if(Remote_Start_Safely(&rc) && HERO.command->Gimbal_Init_Flag == true)
	{
		HERO.command->Hero_Init_Flag = true;
	}
		
	
	/*旧飞坡无，新飞坡有*/
	/*判断裁判系统状态*/
	  /*判断是否复活复位*/
}

/**
  * @brief  整车状态更新
  * @param  Hero_t* hero
  * @retval None
  */
void Hero_Status_Update(Hero_t* hero)
{
	/*遥控器在线，未唤醒*/
	if(hero->command->Hero_Init_Flag != true && rc.info->status == DEV_ONLINE)
	{
		Hero_Init();
		hero->mode = Init_Mode;
	}
	else
	{
		/*遥控器在线且已经唤醒*/
		if(rc.info->status == DEV_ONLINE)
		{
			Rc_Status_Scan(hero);
		}
		/*遥控器离线*/
		else
		{
			hero->mode = Sleep_Mode;
			hero->ctrl = RC_CTRL;//重新启动要先检测遥控
			hero->command->Hero_Init_Flag = false;
			hero->command->Gimbal_Init_Flag = false;
		}
	}
}

/**
  * @brief  整车心跳包
  * @param  None
  * @retval None
  */
void Hero_Heartbeat(void)
{
	My_Chassis_Heartbeat();
	My_Gimbal_Heartbeat();
	My_Launch_Heartbeat();
}







void Rc_Status_Scan(Hero_t* hero)
{
	/*进键盘控制并在比赛中不检测左拨杆，非比赛中检测左拨杆是否开自瞄*/
	Rc_S2_Status_Check(hero);
	
	switch(hero->ctrl)
	{
		case RC_CTRL:
			Rc_S1_Status_Check(hero);
	    Rc_Thumbwheel_Check(hero);
			break;
		case KEY_CTRL:
			/*判断比赛状态 begin*/
			Vision_Debug_Check(hero);
		  /*判断比赛状态 end*/
			Key_Status_Scan(hero);
			break;
		default:
			break;
	}
}

/*发射指令*/
void Rc_S1_Status_Check(Hero_t* hero)
{
	switch(rc.base_info->s1.status)
	{
		case up_R:
		  hero->command->launch->Fire = true;
			break;
		case mid_R:
			break;
		case down_R:
			hero->command->launch->Stop = true;
			break;
		default:
			break;
	}
}

void Rc_S2_Status_Check(Hero_t* hero)
{
	/*右拨杆非跳变检测*/
	switch(rc.base_info->s2.value)
	{
		case up_R:
			/*遥控到键盘跳变默认为陀螺仪模式，键盘状态下可以改机械模式*/
		  if(hero->ctrl == RC_CTRL)
			{
				hero->mode = Imu_Mode;
			}
			hero->ctrl = KEY_CTRL;
			break;
		case mid_R:
			hero->ctrl = RC_CTRL;
		  hero->mode = Imu_Mode;
			break;
		case down_R:
			hero->ctrl = RC_CTRL;
		  hero->mode = Mec_Mode;
			break;
		default:
			break;
	}
}

void Rc_Thumbwheel_Check(Hero_t* hero)
{
	switch(rc.base_info->thumbwheel.status)//检测值
	{
		case down_R:/*实际是向上拨*/
			switch(hero->mode)
			{
				case Imu_Mode:
					hero->command->chassis->Go_Top = true;
					break;
				case Mec_Mode:
//					hero->command->launch->Clear_Heat = true;
				hero->command->launch->Kill_Me_Please = true;
				default:
					break;
			}
			break;
		case mid_R:
			break;
		case up_R:/*实际是向下拨*/
			switch(hero->mode)
			{
				case Imu_Mode:
					hero->command->chassis->Go_Top = false;
					break;
				default:
					break;
			}
			break;
		case keep_R:
			switch(hero->mode)
			{
				case Imu_Mode:
					break;
				default:
					hero->command->chassis->Go_Top = false;
					break;
			}
			default:
			break;
	}
}

void Key_Mouse_l_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->mouse_btn_l.status)
	{
		case relax_K:
			break;
		case up_K:
			break;
		case down_K:
		case short_press_K:
		case long_press_K:
			hero->command->launch->Fire = true;
			break;
		default:
			break;
	}
}	

void Key_Mouse_r_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->mouse_btn_r.status)
	{
		case up_K:
			hero->command->vision->Auto_Catch = false;
			break;
		case down_K:
		case short_press_K:
		case long_press_K:
			hero->command->vision->Auto_Catch = true;
			break;
		default:
			break;
	}
}

void Key_R_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->R.status)
	{
		case relax_K:
		case up_K:
			hero->command->vision->ROI_On = false;
			break;
		case down_K:
		case short_press_K:
		case long_press_K:
			hero->command->vision->ROI_On = true;
			break;
		default:
			break;
	}
}

void Key_Q_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->Q.status)
	{
		case relax_K:
			break;
		case down_K:
			switch(hero->mode)
			{
				case Imu_Mode:
					if(hero->command->gimbal->Ctrl_Ready != true)
					{
						hero->command->gimbal->Trun_Left_45 = true;
					}
					else
					{
						hero->command->gimbal->Stop_Left_Hit = true;
					}
					break;
				default:
					break;
			}
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_W_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->W.status)
	{
		case relax_K:
			break;
		case down_K:
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_E_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->E.status)
	{
		case relax_K:
			break;
		case down_K:
			switch(hero->mode)
			{
				case Imu_Mode:
					if(hero->command->gimbal->Ctrl_Ready != true)
					{
						hero->command->gimbal->Trun_Right_45 = true;
					}
					else
					{
						hero->command->gimbal->Stop_Right_Hit = true;
					}
					break;
				default:
					break;
			}
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_S_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->S.status)
	{
		case relax_K:
			break;
		case down_K:
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_D_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->D.status)
	{
		case relax_K:
			break;
		case down_K:
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_F_Status_Check(Hero_t* hero)
{
	static uint32_t top_switch = 0;
  switch(rc.base_info->F.status)
	{
		case relax_K:
			switch(hero->mode)
			{
				case Imu_Mode:
					break;
				default:
					hero->command->chassis->Go_Top = false;
				  top_switch = 0;
					break;
			}
			break;
		case down_K:
			switch(hero->mode)
			{
				case Imu_Mode:
					top_switch++;
				  switch(top_switch % 2)
					{
						case 0:
							hero->command->chassis->Go_Top = false;
							break;
						case 1:
						  hero->command->chassis->Go_Top = true;
						  break;
					}
					break;
				default:
					break;
			}
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_Z_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->Z.status)
	{
		case relax_K:
			hero->command->launch->Speed_Down = false;
			break;
		case down_K:
			if(hero->command->launch->Ctrl_Ready == true)
			{
				hero->command->launch->Speed_Down = true;
			}
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_X_Status_Check(Hero_t* hero)
{
	switch(rc.base_info->X.status)
	{
		case relax_K:
			hero->command->launch->Speed_Up = false;
			break;
		case down_K:
			if(hero->command->launch->Ctrl_Ready == true)
			{
				hero->command->launch->Speed_Up = true;
			}
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_C_Status_Check(Hero_t* hero)//切换模式按键
{
  switch(rc.base_info->C.status)
	{
		case relax_K:
			//判断是否开启自瞄，若进入自瞄，则强制进入陀螺仪模式
		  //测试开着自瞄按c键
		case up_K:
			hero->mode = Imu_Mode;
			break;
		case down_K:
		case short_press_K:
		case long_press_K:
			hero->mode = Mec_Mode;
			break;
		default:
			break;
	}
}

void Key_V_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->V.status)
	{
		case relax_K:
			break;
		case down_K:
			switch(hero->mode)
			{
				case Imu_Mode:
					hero->command->gimbal->Head_Swap = true;
					break;
				default:
					break;
			}
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_B_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->B.status)
	{
		case relax_K:
			break;
		case down_K:
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

/*清空热量、自爆按钮*/
void Key_G_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->G.status)
	{
		case relax_K:
			break;
		case down_K:
			if(hero->command->launch->Ctrl_Ready != true)
			{
				hero->command->launch->Clear_Heat = true;
			}
			else
			{
				hero->command->launch->Kill_Me_Please = true;
			}
			break;
		case up_K:
			break;
		case short_press_K:
			break;
		case long_press_K:
			break;
		default:
			break;
	}
}

void Key_Shift_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->Shift.status)
	{
		case relax_K:
			hero->command->chassis->Accelerate = false;
			break;
		case up_K:
			hero->command->chassis->Accelerate = false;
			break;
		case down_K:
		case short_press_K:
		case long_press_K:
			hero->command->chassis->Accelerate = true;
			break;
		default:
			break;
	}
}

void Key_Ctrl_Status_Check(Hero_t* hero)
{
  switch(rc.base_info->Ctrl.status)
	{
		case relax_K:
			hero->command->launch->Ctrl_Ready = false;
		  hero->command->gimbal->Ctrl_Ready = false;
			break;
		case up_K:
			hero->command->launch->Ctrl_Ready = false;
		  hero->command->gimbal->Ctrl_Ready = false;
			break;
		case down_K:
		case short_press_K:
		case long_press_K:
			hero->command->launch->Ctrl_Ready = true;

		
		  if(hero->command->gimbal->Ctrl_Ready != true && \
				 (hero->command->gimbal->Stop_Left_Hit == true | hero->command->gimbal->Stop_Right_Hit == true))
			{
				hero->command->gimbal->Stop_Left_Hit = false;
				hero->command->gimbal->Stop_Right_Hit = false;
			}
			else
			{
				hero->command->gimbal->Ctrl_Ready = true;
			}
			break;
		default:
			break;
	}
}

void Key_Status_Scan(Hero_t* hero)
{
	Key_Mouse_l_Status_Check(hero);
	Key_Mouse_r_Status_Check(hero);
	Key_Q_Status_Check(hero);
  Key_W_Status_Check(hero);
  Key_E_Status_Check(hero);
  Key_S_Status_Check(hero);
  Key_D_Status_Check(hero);
  Key_F_Status_Check(hero);
  Key_C_Status_Check(hero);
  Key_V_Status_Check(hero);
	Key_R_Status_Check(hero);
  Key_B_Status_Check(hero);
	Key_G_Status_Check(hero);
  Key_Shift_Status_Check(hero);
  Key_Ctrl_Status_Check(hero);
	Key_Z_Status_Check(hero);
	Key_X_Status_Check(hero);
}

void Vision_Debug_Check(Hero_t* hero)
{
	switch(rc.base_info->s1.status)
	{
		case up_R:
			hero->command->vision->Auto_Catch = false;
			break;
		case mid_R:
			hero->command->vision->Auto_Catch = false;
			break;
		case down_R:
			hero->command->vision->Auto_Catch = true;
			break;
		default:
			break;
	}
}

