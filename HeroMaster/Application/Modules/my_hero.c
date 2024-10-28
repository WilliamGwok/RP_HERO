#include "my_hero.h"
#include "remote.h"
#include "string.h"
#include "config.h"
#include "my_chassis.h"
#include "my_gimbal.h"
#include "my_launch.h"

/*����ṹ�� begin*/
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
/*����ṹ�� end*/

/*�����ṹ�� begin*/
Hero_t HERO = 
{
	.ctrl = RC_CTRL,
	.mode = Init_Mode,
	.command = &Hero_Command,
};
/*�����ṹ�� end*/


/**
  * @brief  ������ʼ������λ��
  * @param  None
  * @retval None
  */
void Hero_Init(void)
{
	/*ȫ��ָ������*/
//	bool test[100];
//	
//	memset(&Hero_Command.chassis, 0, sizeof(Hero_Command.chassis));
//	memcpy(&Hero_Command.gimbal, test, sizeof(Hero_Command.gimbal));
//	memset(&Hero_Command.launch, 0, sizeof(Hero_Command.launch));
//	memset(&Hero_Command.vision, 0, sizeof(Hero_Command.vision));
	HERO.command->vision->Auto_Catch = false;
	
	/*��λ�ɹ����� : ��̨yaw��pitch����ָ���Ƕȣ�ң��ͨ������*/
//	if(Remote_Start_Safely(&rc) && HERO.command->Gimbal_Init_Flag == true)
	
	if(Remote_Start_Safely(&rc) && HERO.command->Gimbal_Init_Flag == true)
	{
		HERO.command->Hero_Init_Flag = true;
	}
		
	
	/*�ɷ����ޣ��·�����*/
	/*�жϲ���ϵͳ״̬*/
	  /*�ж��Ƿ񸴻λ*/
}

/**
  * @brief  ����״̬����
  * @param  Hero_t* hero
  * @retval None
  */
void Hero_Status_Update(Hero_t* hero)
{
	/*ң�������ߣ�δ����*/
	if(hero->command->Hero_Init_Flag != true && rc.info->status == DEV_ONLINE)
	{
		Hero_Init();
		hero->mode = Init_Mode;
	}
	else
	{
		/*ң�����������Ѿ�����*/
		if(rc.info->status == DEV_ONLINE)
		{
			Rc_Status_Scan(hero);
		}
		/*ң��������*/
		else
		{
			hero->mode = Sleep_Mode;
			hero->ctrl = RC_CTRL;//��������Ҫ�ȼ��ң��
			hero->command->Hero_Init_Flag = false;
			hero->command->Gimbal_Init_Flag = false;
		}
	}
}

/**
  * @brief  ����������
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
	/*�����̿��Ʋ��ڱ����в�����󲦸ˣ��Ǳ����м���󲦸��Ƿ�����*/
	Rc_S2_Status_Check(hero);
	
	switch(hero->ctrl)
	{
		case RC_CTRL:
			Rc_S1_Status_Check(hero);
	    Rc_Thumbwheel_Check(hero);
			break;
		case KEY_CTRL:
			/*�жϱ���״̬ begin*/
			Vision_Debug_Check(hero);
		  /*�жϱ���״̬ end*/
			Key_Status_Scan(hero);
			break;
		default:
			break;
	}
}

/*����ָ��*/
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
	/*�Ҳ��˷�������*/
	switch(rc.base_info->s2.value)
	{
		case up_R:
			/*ң�ص���������Ĭ��Ϊ������ģʽ������״̬�¿��ԸĻ�еģʽ*/
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
	switch(rc.base_info->thumbwheel.status)//���ֵ
	{
		case down_R:/*ʵ�������ϲ�*/
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
		case up_R:/*ʵ�������²�*/
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

void Key_C_Status_Check(Hero_t* hero)//�л�ģʽ����
{
  switch(rc.base_info->C.status)
	{
		case relax_K:
			//�ж��Ƿ������飬���������飬��ǿ�ƽ���������ģʽ
		  //���Կ������鰴c��
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

/*����������Ա���ť*/
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

