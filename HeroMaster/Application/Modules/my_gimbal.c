#include "my_gimbal.h"
#include "gimbal_motor.h"
#include "imu.h"
#include "my_hero.h"
#include "math_support.h"
#include "config_can.h"
#include "remote.h"
#include "my_vision.h"
#include "config.h"

/*
	float	  kp;
	float 	ki;
	float 	kd;
	
	float   blind_err;	
	float 	integral_max;	
	float   iout_max;
	float 	out_max;
*/
float gimb_y_mec_angle_in_pid_param[7]   = {-500,-6,0,0,8000,6000,28000};

float gimb_y_mec_angle_pid_param[7]      = {0.5,0,0,0,0,0,700};

float gimb_p_mec_angle_in_pid_param[7]   = {-300,-6,0,0,5000,5000,28000};

float gimb_p_mec_angle_pid_param[7]      = {1,0,0,5,0,0,500};

float gimb_y_imu_angle_in_pid_param[7]   = {-400,-0.1,0,0,4000,4000,28000};

float gimb_y_imu_angle_pid_param[7]      = {10.4,0,0,0,0,0,700};

float gimb_p_imu_angle_in_pid_param[7]   = {-500,-2,0,0,5000,5000,28000};

float gimb_p_imu_angle_pid_param[7]      = {13,0,0,0,0,0,500};


Gimbal_Target_t Gimbal_Target;
Gimbal_Measure_t Gimbal_Measure;
Gimbal_For_Chassis_t Gimbal_For_Chassis;
Gimbal_For_Vision_t Gimbal_For_Vision;
Gimbal_For_Launch_t Gimbal_For_Launch;
Gimbal_Cnt_t Gimbal_Cnt;

Gimbal_Info_t Gimbal_Info = 
{
	.yaw_middle_angle_config = GIMBAL_YAW_MIDDLE_ANGLE,
	.pitch_middle_angle_config = GIMBAL_PITCH_MIDDLE_ANGLE,
	.target = &Gimbal_Target,
	.measure = &Gimbal_Measure,
	.gimbal_to_chassis = &Gimbal_For_Chassis,
	.gimbal_to_vision = &Gimbal_For_Vision,
	.gimbal_to_launch = &Gimbal_For_Launch,
};

My_Gimbal_t My_Gimbal = 
{
	.cnt = &Gimbal_Cnt,
	.info = &Gimbal_Info,
};


void My_Gimbal_Init(void)
{
  Gimbal_Motor[PITCH_GIMBAL].init(&Gimbal_Motor[PITCH_GIMBAL]);
	Gimbal_Motor[YAW_GIMBAL].init(&Gimbal_Motor[YAW_GIMBAL]);

	/*��еģʽ�Ƕ�˫��PID begin*/
	Gimbal_Motor[YAW_GIMBAL].pid_init(&Gimbal_Motor[YAW_GIMBAL].pid.mec_angle_in,gimb_y_mec_angle_in_pid_param);
	Gimbal_Motor[YAW_GIMBAL].pid_init(&Gimbal_Motor[YAW_GIMBAL].pid.mec_angle,gimb_y_mec_angle_pid_param);
	Gimbal_Motor[PITCH_GIMBAL].pid_init(&Gimbal_Motor[PITCH_GIMBAL].pid.mec_angle_in,gimb_p_mec_angle_in_pid_param);
	Gimbal_Motor[PITCH_GIMBAL].pid_init(&Gimbal_Motor[PITCH_GIMBAL].pid.mec_angle,gimb_p_mec_angle_pid_param);
	/*��еģʽ�Ƕ�˫��PID end*/
	
	/*������ģʽ�Ƕ�˫��PID begin*/
	Gimbal_Motor[YAW_GIMBAL].pid_init(&Gimbal_Motor[YAW_GIMBAL].pid.imu_angle_in,gimb_y_imu_angle_in_pid_param);
	Gimbal_Motor[YAW_GIMBAL].pid_init(&Gimbal_Motor[YAW_GIMBAL].pid.imu_angle,gimb_y_imu_angle_pid_param);
	Gimbal_Motor[PITCH_GIMBAL].pid_init(&Gimbal_Motor[PITCH_GIMBAL].pid.imu_angle_in,gimb_p_imu_angle_in_pid_param);
	Gimbal_Motor[PITCH_GIMBAL].pid_init(&Gimbal_Motor[PITCH_GIMBAL].pid.imu_angle,gimb_p_imu_angle_pid_param);
	/*������ģʽ�Ƕ�˫��PID end*/
	
	My_Gimbal.pitch_motor = &Gimbal_Motor[PITCH_GIMBAL];
	My_Gimbal.yaw_motor = &Gimbal_Motor[YAW_GIMBAL];
}

/**
  * @brief  ��̨������
  * @param  None
  * @retval None
  */
void My_Gimbal_Heartbeat(void)
{
	Gimbal_Motor[YAW_GIMBAL].heartbeat(&Gimbal_Motor[YAW_GIMBAL]);
	Gimbal_Motor[PITCH_GIMBAL].heartbeat(&Gimbal_Motor[PITCH_GIMBAL]);
}

/**
  * @brief  ��̨���ߴ���,����̨�ѳ�ʼ���������ߣ�������˵�ǳ�����ɱ�ˣ������������̨���ˣ�������ʱ�˳���ѡ��,��֤û����
            �������ƶ���������
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void My_Gimbal_Offline(My_Gimbal_t* my_gimbal)
{
	if((my_gimbal->pitch_motor->state.work_state == M_OFFLINE || \
		 my_gimbal->yaw_motor->state.work_state == M_OFFLINE) && HERO.command->Gimbal_Init_Flag == true && \
		 my_gimbal->cnt->gimbal_danger_offline_cnt <= 3)//4�δ�ŻῨ6��7�����̿ɲٿ�
	{
		HERO.command->Gimbal_Init_Flag = false;
		my_gimbal->cnt->gimbal_danger_offline_cnt++;
	}
	else
	{
		
	}
}

/**
  * @brief  ��ָ̨����Ӧ
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void My_Gimbal_Command_React(My_Gimbal_t* my_gimbal)
{
	Gimbal_Command_t* command = HERO.command->gimbal;
	
	if(command->Trun_Left_45 == true)
	{
		my_gimbal->info->target->yaw_imu_angle -= 45.f;
		command->Trun_Left_45 = false;
	}
	if(command->Trun_Right_45 == true)
	{
		my_gimbal->info->target->yaw_imu_angle += 45.f;
		command->Trun_Right_45 = false;
	}
	if(command->Head_Swap == true)
	{
		if(my_gimbal->cnt->head_swap_cnt == 0)//�״λ�û�ͷָ��
		{
			my_gimbal->info->target->yaw_imu_angle += 180.f;
			my_gimbal->cnt->head_swap_cnt++;
		}
		else//��ͷָ��ִ����
		{
			my_gimbal->cnt->head_swap_cnt++;
			
			if(abs(my_gimbal->yaw_motor->pid.imu_angle.info.err) <= GIMBAL_HEAD_SWAP_ANGLE)//��ͷָ��ִ����
			{
				command->Head_Swap = false;
				my_gimbal->cnt->head_swap_cnt = 0;
			}
			else if(my_gimbal->cnt->head_swap_cnt >= GIMBAL_INIT_RPOCESS_TIME)//��ͷָ��ִ�г�ʱ
			{
				my_gimbal->info->target->yaw_imu_angle = my_gimbal->info->measure->yaw_imu_angle;
				command->Head_Swap = false;
				my_gimbal->cnt->head_swap_cnt = 0;
			}
		}
	}
	
	/*������ָ��*/
	if(HERO.command->vision->Auto_Catch == true)
	{
		my_gimbal->mode = G_Auto;
	}
	
	switch(my_gimbal->mode)
	{
		case G_Imu:
			if(HERO.command->gimbal->Stop_Right_Hit == true)
	    {
	    	my_gimbal->info->yaw_middle_angle_config =  GIMBAL_YAW_MIDDLE_ANGLE + 1024;
	    }
	    else if(HERO.command->gimbal->Stop_Left_Hit == true)
	    {
	    	my_gimbal->info->yaw_middle_angle_config =  GIMBAL_YAW_MIDDLE_ANGLE - 1024;
	    }
	    else
	    {
	    	my_gimbal->info->yaw_middle_angle_config =  GIMBAL_YAW_MIDDLE_ANGLE;
	    }
			break;
		default:
			HERO.command->gimbal->Stop_Right_Hit = false;
		  HERO.command->gimbal->Stop_Left_Hit = false;
		  my_gimbal->info->yaw_middle_angle_config =  GIMBAL_YAW_MIDDLE_ANGLE;
			break;
	}
//	/*Ħ�����Ծ�ָ��*/
//	if(HERO.command->gimbal->Rescue_Fric_Up == true)
//	{
//		switch(my_gimbal->mode)
//		{
//			case G_Imu:
//				my_gimbal->info->target->pitch_imu_angle = GIMBAL_PITCH_IMU_SUP_ANGLE - 10;
//				break;
//			case G_Mec:
//				my_gimbal->info->target->pitch_mec_angle = GIMBAL_PITCH_MEC_SUP_ANGLE - 200;
//				break;
//			default:
//				break;
//		}
//		HERO.command->gimbal->Rescue_Fric_Up = false;
//	}
//	
//	if(HERO.command->gimbal->Rescue_Fric_Down == true)
//	{
//		switch(my_gimbal->mode)
//		{
//			case G_Imu:
//				my_gimbal->info->target->pitch_imu_angle = GIMBAL_PITCH_IMU_LOW_ANGLE + 10;
//				break;
//			case G_Mec:
//				my_gimbal->info->target->pitch_mec_angle = GIMBAL_PITCH_MEC_LOW_ANGLE + 100;
//				break;
//			default:
//				break;
//		}
//		HERO.command->gimbal->Rescue_Fric_Down = false;
//	}
}

/**
  * @brief  ��̨״̬��Ӧ
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void My_Gimbal_Status_React(My_Gimbal_t* my_gimbal)
{
	switch(HERO.mode)
	{
		case Init_Mode:
			my_gimbal->mode = G_Init;
			break;
		case Sleep_Mode:
			/*��̨ж��*/
		  my_gimbal->mode = G_Sleep;
		
		  /*�ر�����*/
		  HERO.command->vision->Auto_Catch = false;
			break;
		case Imu_Mode:
			my_gimbal->mode = G_Imu;
			break;
		case Mec_Mode:
			/*�ж��Ƿ�����,����my_hero�п���*/
			my_gimbal->mode = G_Mec;
			break;
		default:
			break;
	}
}

/**
  * @brief  ��̨���ݸ���,(�Ƕȣ��ٶ�)
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void My_Gimbal_Data_Update(My_Gimbal_t* my_gimbal)
{
	/*��̨ԭʼ��е�Ƕȣ��ٶȸ��� begin*/
	my_gimbal->info->measure->yaw_mec_angle = my_gimbal->yaw_motor->rx_info.angle;
	my_gimbal->info->measure->pitch_mec_angle = my_gimbal->pitch_motor->rx_info.angle;
	
	my_gimbal->info->measure->yaw_mec_speed = imu.base_info->yaw_dif_speed_ave;
	my_gimbal->info->measure->pitch_mec_speed = imu.base_info->pitch_dif_speed_ave;
	/*��̨ԭʼ��е�Ƕȣ��ٶȸ��� end*/
	
	/*��̨ԭʼ�����ǽǶȣ��ٶȸ��� begin*/
	my_gimbal->info->measure->yaw_imu_angle = imu.base_info->yaw;
	my_gimbal->info->measure->pitch_imu_angle = imu.base_info->pitch;
	
	my_gimbal->info->measure->yaw_imu_speed = imu.base_info->yaw_dif_speed_ave;
	my_gimbal->info->measure->pitch_imu_speed = imu.base_info->pitch_dif_speed_ave;
	/*��̨ԭʼ�����ǽǶȣ��ٶȸ��� end*/
	
	/*��̨��е�Ƕȴ��� begin*/
	my_gimbal->info->measure->yaw_mec_angle = my_gimbal->info->yaw_middle_angle_config - my_gimbal->info->measure->yaw_mec_angle;
	My_Gimbal_Yaw_Angle_Check(my_gimbal);
	
	my_gimbal->info->measure->pitch_mec_angle = my_gimbal->info->pitch_middle_angle_config - my_gimbal->info->measure->pitch_mec_angle;
	My_Gimbal_Pitch_Angle_Check(my_gimbal);
	
	/*��ͷ��־λ*/
	if(abs(my_gimbal->info->measure->yaw_mec_angle) >= 2048)
	{
		my_gimbal->info->gimbal_to_chassis->Back_Control = -1;
	}
	else
	{
		my_gimbal->info->gimbal_to_chassis->Back_Control = 1;
	}
	/*��̨��е�Ƕȴ��� begin*/
}

/**
  * @brief  ��̨��Ϣ����
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void Gimbal_Data_Transform(My_Gimbal_t* my_gimbal)
{
	/*�����̵����� begin*/
	int16_t yaw_angle_err = my_gimbal->info->measure->yaw_mec_angle;
	
	if(abs(yaw_angle_err) > 2048)
	{
		yaw_angle_err -= 4096 * sgn(yaw_angle_err);
	}
	
	my_gimbal->info->gimbal_to_chassis->yaw_delta = yaw_angle_err;
	/*�����̵����� end*/
	
	/*����������� begin*/
	
	my_gimbal->info->gimbal_to_launch->pitch_mec_angle = my_gimbal->info->measure->pitch_mec_angle;
	
	/*����������� end*/
}

/**
  * @brief  ��̨����
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void My_Gimbal_Control(My_Gimbal_t* my_gimbal)
{
	/*��̨Yaw���� begin*/
	switch(my_gimbal->mode)
	{
		case G_Sleep:
			can1_0x1FF_send_buff[my_gimbal->yaw_motor->id.buff_p] = 0;
			break;
		case G_Init:
		  if(abs(my_gimbal->info->measure->yaw_mec_angle) <= GIMBAL_INIT_ANGLE_ERR && abs(my_gimbal->info->measure->pitch_mec_angle) <= GIMBAL_INIT_ANGLE_ERR)
			{
				HERO.command->Gimbal_Init_Flag = true;
				my_gimbal->cnt->init_cnt = 0;
			}
			else if(my_gimbal->cnt->init_cnt == GIMBAL_INIT_RPOCESS_TIME)
			{
				HERO.command->Gimbal_Init_Flag = true;
				my_gimbal->cnt->init_cnt = 0;
			}
			else
			{
				my_gimbal->cnt->init_cnt++;
			}
			
			my_gimbal->info->target->yaw_mec_angle = 0;
			my_gimbal->info->target->pitch_mec_angle = 0;
			
		  Gimbal_Yaw_Mec_Ctrl(my_gimbal);
		  Gimbal_Pitch_Mec_Ctrl(my_gimbal);
			
			/*�����ǽǶ�Ŀ��ֵͬ��*/
		  my_gimbal->info->target->yaw_imu_angle = my_gimbal->info->measure->yaw_imu_angle;
			my_gimbal->info->target->pitch_imu_angle = my_gimbal->info->measure->pitch_imu_angle;
			break;
		case G_Imu:
			switch(HERO.ctrl)
			{
				case RC_CTRL:
					my_gimbal->info->target->yaw_imu_angle += rc.base_info->ch0 / 2000.f;
				  Gimbal_Yaw_Imu_Ctrl(my_gimbal);
					break;
				case KEY_CTRL:
					my_gimbal->info->target->yaw_imu_angle += rc.info->mouse_x_K / 500.f;
				  Gimbal_Yaw_Imu_Ctrl(my_gimbal);
					break;
				default:
					break;
			}
			
			/*��е�Ƕ�Ŀ��ֵͬ��*/
		  my_gimbal->info->target->yaw_mec_angle = my_gimbal->info->measure->yaw_mec_angle;
			
			break;
		case G_Mec:
			my_gimbal->info->target->yaw_mec_angle = 0;
		  Gimbal_Yaw_Mec_Ctrl(my_gimbal);
		
		  /*�����ǽǶ�Ŀ��ֵͬ��*/
		  my_gimbal->info->target->yaw_imu_angle = my_gimbal->info->measure->yaw_imu_angle;
			break;
		case G_Auto:
			if(My_Vision.info->status == DEV_ONLINE && My_Vision.base_info->capture_Y_O_N == 1)
			{
			  my_gimbal->info->target->yaw_imu_angle = My_Vision.transla_info->yaw_receive;
			}
			else
			{
				switch(HERO.ctrl)
			  {
				  case RC_CTRL:
				  	my_gimbal->info->target->yaw_imu_angle += rc.base_info->ch0 / 2000.f;
					  break;
				  case KEY_CTRL:
				  	my_gimbal->info->target->yaw_imu_angle += rc.info->mouse_x_K / 500.f;
					  break;
				  default:
					  break;
			  }
			}
			
			/*��е�Ƕ�Ŀ��ֵͬ��*/
		  my_gimbal->info->target->yaw_mec_angle = my_gimbal->info->measure->yaw_mec_angle;
			
		  Gimbal_Yaw_Imu_Ctrl(my_gimbal);
			break;
		default:
			break;
	}
	/*��̨Yaw���� end*/
	
	/*��̨Pitch���� begin*/
	switch(my_gimbal->mode)
	{
		case G_Sleep:
			can2_0x1FF_send_buff[my_gimbal->pitch_motor->id.buff_p] = 0;
			break;
		case G_Init:
			break;
		case G_Imu:
			switch(HERO.ctrl)
			{
				case RC_CTRL:
					my_gimbal->info->target->pitch_imu_angle += rc.base_info->ch1 / 2100.f;
				  Gimbal_Pitch_Imu_Ctrl(my_gimbal);
					break;
				case KEY_CTRL:
					my_gimbal->info->target->pitch_imu_angle -= rc.info->mouse_y_K / 1000.f;
				  Gimbal_Pitch_Imu_Ctrl(my_gimbal);
					break;
				default:
					break;
			}
			
			/*��е�Ƕ�Ŀ��ֵͬ��*/
		  my_gimbal->info->target->pitch_mec_angle = my_gimbal->info->measure->pitch_mec_angle;
			
			break;
		case G_Mec:
			switch(HERO.ctrl)
			{
				case RC_CTRL:
					my_gimbal->info->target->pitch_mec_angle += rc.base_info->ch1 / 10000.f * 25.f;
				  Gimbal_Pitch_Mec_Ctrl(my_gimbal);
					break;
				case KEY_CTRL:
					my_gimbal->info->target->pitch_mec_angle -= rc.info->mouse_y_K / 3000.f;
				  Gimbal_Pitch_Mec_Ctrl(my_gimbal);
					break;
				default:
					break;
			}
			
			/*�����ǽǶ�Ŀ��ֵͬ��*/
		  my_gimbal->info->target->pitch_imu_angle = my_gimbal->info->measure->pitch_imu_angle;
			
			break;
		case G_Auto:
			if(My_Vision.info->status == DEV_ONLINE && My_Vision.base_info->capture_Y_O_N == 1)
			{
				my_gimbal->info->target->pitch_imu_angle = My_Vision.transla_info->pitch_receive;
			}
			else
			{
				switch(HERO.ctrl)
			  {
				  case RC_CTRL:
					  my_gimbal->info->target->pitch_imu_angle += rc.base_info->ch1 / 10000.f * 25.f;
					  break;
				  case KEY_CTRL:
					  my_gimbal->info->target->pitch_imu_angle -= rc.info->mouse_y_K / 3000.f;
					  break;
				  default:
					  break;
			  }
			}
			
			/*��е�Ƕ�Ŀ��ֵͬ��*/
		  my_gimbal->info->target->pitch_mec_angle = my_gimbal->info->measure->pitch_mec_angle;
			
			Gimbal_Pitch_Imu_Ctrl(my_gimbal);
			break;
		default:
			break;
	}
	/*��̨Pitch���� end*/
}


void My_Gimbal_Work(void)
{
	My_Gimbal_Status_React(&My_Gimbal);
	
	My_Gimbal_Command_React(&My_Gimbal);
	
	My_Gimbal_Data_Update(&My_Gimbal);
	
	Gimbal_Data_Transform(&My_Gimbal);
	
	My_Gimbal_Control(&My_Gimbal);
	
	My_Gimbal_Offline(&My_Gimbal);
}





/*...............................................������...............................................*/

/**
  * @brief  �����ǽǶ�תΪ��е�Ƕȣ�-180~180 -> -4096~4096
  * @param  float angle_org ԭʼ�����ǽǶ�
  * @retval int16_t angle_process ת��Ϊ��е�Ƕȵ������ǽǶ�
  */
float My_Gimbal_Angle_Imu_to_Mec(float angle_org)
{
	float angle_process;
	
	angle_org = constrain(angle_org, -180.f, 180.f);
	
	angle_process = (sgn(angle_org) * (((angle_org) / 360.f) * 8191.f));

	return angle_process;
}


/**
  * @brief  Yaw��е�������ǽǶȵĲ���ֵ��Ŀ��ֵԼ��
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void My_Gimbal_Yaw_Angle_Check(My_Gimbal_t* my_gimbal)
{
	Gimbal_Info_t *Info = my_gimbal->info;
	
	float angle;
	
	angle = Info->measure->yaw_imu_angle;//-180��~180��
	while (abs(angle) > 180)
	{
		angle -= 360 * sgn(angle);
	}
	Info->measure->yaw_imu_angle = angle;
	
	angle = Info->target->yaw_imu_angle;//-180��~180��
	while (abs(angle) > 180)
	{
		angle -= 360 * sgn(angle);
	}
	Info->target->yaw_imu_angle = angle;
	
	angle = Info->measure->yaw_mec_angle;//-4096~4096
	while (abs(angle) > 4096)
	{
		angle -= 8192 * sgn(angle);
	}
	Info->measure->yaw_mec_angle = angle;
	
	angle = Info->target->yaw_mec_angle;//-4096~4096
	while (abs(angle) > 4096)
	{
		angle -= 8192 * sgn(angle);
	}
	Info->target->yaw_mec_angle = angle;
}

/**
  * @brief  Pitch��е�������ǽǶȵĲ���ֵ��Ŀ��ֵԼ��
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void My_Gimbal_Pitch_Angle_Check(My_Gimbal_t* my_gimbal)
{
	Gimbal_Info_t *Info = my_gimbal->info;
	
	float angle;
	
	angle = my_gimbal->info->measure->pitch_mec_angle;
	while (abs(angle) > 4096)
	{
		angle -= 8192 * sgn(angle);
	}
	my_gimbal->info->measure->pitch_mec_angle = angle;
	
	angle = Info->target->pitch_mec_angle;
	if(angle > GIMBAL_PITCH_MEC_SUP_ANGLE)
	{
		angle = GIMBAL_PITCH_MEC_SUP_ANGLE;
	}
	if(angle < GIMBAL_PITCH_MEC_LOW_ANGLE)
	{
		angle = GIMBAL_PITCH_MEC_LOW_ANGLE;
	}
	Info->target->pitch_mec_angle = angle;
	
	angle = Info->target->pitch_imu_angle;
	if(angle > GIMBAL_PITCH_IMU_SUP_ANGLE)
	{
		angle = GIMBAL_PITCH_IMU_SUP_ANGLE;
	}
	if(angle < GIMBAL_PITCH_IMU_LOW_ANGLE)
	{
		angle = GIMBAL_PITCH_IMU_LOW_ANGLE;
	}
	Info->target->pitch_imu_angle = angle;
}


/*...............................................�������...............................................*/

/**
  * @brief  Yaw��е�Ƕȿ���
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void Gimbal_Yaw_Mec_Ctrl(My_Gimbal_t* my_gimbal)
{
	motor_t *motor = my_gimbal->yaw_motor;
	/*���ǰ���Ŀ��ֵ*/
	My_Gimbal_Yaw_Angle_Check(my_gimbal);
	/*���*/
	can1_0x1FF_send_buff[motor->id.buff_p] = motor->c_pid2(&motor->pid.mec_angle,&motor->pid.mec_angle_in,
	                                                        my_gimbal->info->measure->yaw_mec_angle,my_gimbal->info->measure->yaw_mec_speed,
	                                                        my_gimbal->info->target->yaw_mec_angle,1);
}

/**
  * @brief  Pitch��е�Ƕȿ���
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void Gimbal_Pitch_Mec_Ctrl(My_Gimbal_t* my_gimbal)
{
	motor_t *motor = my_gimbal->pitch_motor;
	/*���ǰ���Ŀ��ֵ*/
	My_Gimbal_Pitch_Angle_Check(my_gimbal);
	/*���*/
	can2_0x1FF_send_buff[motor->id.buff_p] = motor->c_pid2(&motor->pid.mec_angle,&motor->pid.mec_angle_in,
	                                                        my_gimbal->info->measure->pitch_mec_angle,my_gimbal->info->measure->pitch_mec_speed,
	                                                        my_gimbal->info->target->pitch_mec_angle,1);
}

/**
  * @brief  Yaw�����ǽǶȿ���
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void Gimbal_Yaw_Imu_Ctrl(My_Gimbal_t* my_gimbal)
{
	motor_t *motor = my_gimbal->yaw_motor;
	/*���ǰ���Ŀ��ֵ*/
	My_Gimbal_Yaw_Angle_Check(my_gimbal);
	/*���*/
	can1_0x1FF_send_buff[motor->id.buff_p] = motor->c_pid2(&motor->pid.imu_angle,&motor->pid.imu_angle_in,
	                                                        my_gimbal->info->measure->yaw_imu_angle,my_gimbal->info->measure->yaw_imu_speed,
	                                                        my_gimbal->info->target->yaw_imu_angle,3);
}

/**
  * @brief  Pitch�����ǽǶȿ���
  * @param  My_Gimbal_t* my_gimbal
  * @retval None
  */
void Gimbal_Pitch_Imu_Ctrl(My_Gimbal_t* my_gimbal)
{
	motor_t *motor = my_gimbal->pitch_motor;
	/*���ǰ���Ŀ��ֵ*/
	My_Gimbal_Pitch_Angle_Check(my_gimbal);
	/*���*/
	can2_0x1FF_send_buff[motor->id.buff_p] = motor->c_pid2(&motor->pid.imu_angle,&motor->pid.imu_angle_in,
	                                                        my_gimbal->info->measure->pitch_imu_angle,my_gimbal->info->measure->pitch_imu_speed,
	                                                        my_gimbal->info->target->pitch_imu_angle,3);
}

