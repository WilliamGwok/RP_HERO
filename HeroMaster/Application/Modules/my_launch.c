#include "my_launch.h"
#include "launch_motor.h"
#include "my_hero.h"
#include "math_support.h"
#include "config_can.h"
#include "motor.h"
#include "my_judge.h"
#include "my_vision.h"
#include "config.h"
#include "drv_haltick.h"
#include "my_hero.h"

/*
	float	  kp;
	float 	ki;
	float 	kd;
	
	float   blind_err;	
	float 	integral_max;	
	float   iout_max;
	float 	out_max;
*/

float dial_motor_position_in_pid_param[7]       = {10,0.4,0,0,0,20000,12000};

float dial_motor_position_pid_param[7]          = {0.2,0,0,0,0,0,3000};

float dial_motor_speed_pid_param[7]             = {8.0,0.33,0,0,8000,8000,15000};

float position_motor_speed_pid_param[7]         = {15,0.6,0,0,10000,10000,9000};

float position_motor_position_in_param[7]   = {9,0.01,0,0,8000,10000,9000};

float position_motor_position_param[7]      = {0.5,0,0,3,8000,10000,9000};

float friction_left_motor_speed_pid_param[7]    = {27,0.5,0,0,6000,6000,10000};

float friction_right_motor_speed_pid_param[7]   = {27,0.5,0,0,6000,6000,10000};

Launch_Cnt_t Launch_Cnt;
Launch_Measure_t Launch_Measure;
Launch_Target_t Launch_Target;
Launch_Status_Flag_t Launch_Status_Flag;

Launch_Info_t Launch_Info = 
{
	.fire_rate_fix = 0,
	.flag = &Launch_Status_Flag,
	.measure = &Launch_Measure,
	.target = &Launch_Target,
};

My_Launch_t My_Launch = 
{
	.cnt = &Launch_Cnt,
	.info = &Launch_Info,
};

/**
  * @brief  �����ʼ��
  * @param  None
  * @retval None
  */
void My_Launch_Init(void)
{
	Launch_Motor[DIAL_LAUNCH].init(&Launch_Motor[DIAL_LAUNCH]);
	Launch_Motor[POSITION_LAUNCH].init(&Launch_Motor[POSITION_LAUNCH]);
	Launch_Motor[LEFT_FRIC_LAUNCH].init(&Launch_Motor[LEFT_FRIC_LAUNCH]);
	Launch_Motor[RIGHT_FRIC_LAUNCH].init(&Launch_Motor[RIGHT_FRIC_LAUNCH]);
	
	Launch_Motor[DIAL_LAUNCH].pid_init(&Launch_Motor[DIAL_LAUNCH].pid.position_in,dial_motor_position_in_pid_param);
	Launch_Motor[DIAL_LAUNCH].pid_init(&Launch_Motor[DIAL_LAUNCH].pid.position,dial_motor_position_pid_param);
	Launch_Motor[DIAL_LAUNCH].pid_init(&Launch_Motor[DIAL_LAUNCH].pid.speed,dial_motor_speed_pid_param);
	Launch_Motor[POSITION_LAUNCH].pid_init(&Launch_Motor[POSITION_LAUNCH].pid.speed,position_motor_speed_pid_param);
	Launch_Motor[POSITION_LAUNCH].pid_init(&Launch_Motor[POSITION_LAUNCH].pid.position_in,position_motor_position_in_param);
	Launch_Motor[POSITION_LAUNCH].pid_init(&Launch_Motor[POSITION_LAUNCH].pid.position,position_motor_position_param);
	Launch_Motor[LEFT_FRIC_LAUNCH].pid_init(&Launch_Motor[LEFT_FRIC_LAUNCH].pid.speed,friction_left_motor_speed_pid_param);
	Launch_Motor[RIGHT_FRIC_LAUNCH].pid_init(&Launch_Motor[RIGHT_FRIC_LAUNCH].pid.speed,friction_right_motor_speed_pid_param);

  My_Launch.feed = &Launch_Motor[DIAL_LAUNCH];
	My_Launch.position = &Launch_Motor[POSITION_LAUNCH];
	My_Launch.left_fric = &Launch_Motor[LEFT_FRIC_LAUNCH];
	My_Launch.right_fric = &Launch_Motor[RIGHT_FRIC_LAUNCH];
}

/**
  * @brief  ����������
  * @param  None
  * @retval None
  */
void My_Launch_Heartbeat(void)
{
	Launch_Motor[DIAL_LAUNCH].heartbeat(&Launch_Motor[DIAL_LAUNCH]);
	Launch_Motor[POSITION_LAUNCH].heartbeat(&Launch_Motor[POSITION_LAUNCH]);
	Launch_Motor[LEFT_FRIC_LAUNCH].heartbeat(&Launch_Motor[LEFT_FRIC_LAUNCH]);
	Launch_Motor[RIGHT_FRIC_LAUNCH].heartbeat(&Launch_Motor[RIGHT_FRIC_LAUNCH]);
}

/**
  * @brief  �������ߴ���
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Offline(My_Launch_t* my_launch)
{
	//�Ա������������־λ
	if(my_launch->left_fric->state.work_state == M_OFFLINE || my_launch->right_fric->state.work_state == M_OFFLINE)
	{
		HERO.command->launch->Kill_Me_Please = false;
		
		my_launch->launch_mode = L_Init;
	  my_launch->feed_mode = F_Init;
	}
}

uint16_t cnt_test_0 = 0;

/**
  * @brief  ����ָ����Ӧ
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Command_React(My_Launch_t* my_launch)
{
	//���
	if(HERO.command->launch->Fire == true || HERO.command->launch->Clear_Heat == true || HERO.command->launch->Kill_Me_Please == true)
	{
		/*��������ѱ�����*/
		if(My_Launch_Ready_Check(my_launch) && my_launch->info->flag->Dial_Wake_OK == true && My_Launch_Fire_Limit_Check(my_launch))
		{
			my_launch->launch_mode = L_Shoot;
			
			/*���������� begin*/
			if(HERO.command->launch->Clear_Heat == true || HERO.command->launch->Kill_Me_Please == true)
			{
				if(HERO.command->launch->Kill_Me_Please == true)
				{
					my_launch->info->feed_reload_bullet_num = 10;
				}
				else
				{
					my_launch->info->feed_reload_bullet_num = My_Launch_Clear_Heat_Cal(my_launch);
				}
			}
			else
			{
				my_launch->info->feed_reload_bullet_num = 1;
			}
			/*���������� end*/
			
			/*����Ӿ��Զ��򵯣�������������״̬�ҿ�������*/
			if(My_Vision.transla_info->detect_top_mode == true && HERO.command->vision->Auto_Catch == true)
			{
				/*�����Դ�ָ�Ϊ�棬�򲻴�*/
				if(My_Vision.transla_info->top_fire_ok != true)
				{
					my_launch->launch_mode = L_Standby;
				}
				else
				{
					//
					cnt_test_0++;
					//
					my_launch->info->feed_reload_bullet_num = 1;
				}
			}
		}
		/*���������δ�����ѣ�����뻽��״̬*/
		else
		{
			my_launch->launch_mode = L_Wake;
		}
		
		HERO.command->launch->Fire = false;
		HERO.command->launch->Clear_Heat = false;
		HERO.command->launch->Kill_Me_Please = false;
	}
	
	//ֹͣ
	if(HERO.command->launch->Stop == true)
	{
		my_launch->launch_mode = L_Stop;
		HERO.command->launch->Stop = false;
	}
	//��ת��
	if(HERO.command->launch->Speed_Up == true)
	{
		my_launch->info->client_modify_fric_speed += 10;
		HERO.command->launch->Speed_Up = false;
	}
	
	if(HERO.command->launch->Speed_Down == true)
	{
		my_launch->info->client_modify_fric_speed -= 10;
		HERO.command->launch->Speed_Down = false;
	}
	
	//������Զ�����Ħ����,����һ����־λ�͹��˚G
	if((HERO.command->judge->Relive_Flag == true || HERO.command->judge->Fighting_Flag == true)\
 		  && my_launch->info->flag->Dial_Wake_OK == false)
	{
		my_launch->launch_mode = L_Wake;
	}
	else//Relive_FlagΪ�ٻ�Dial_Wake_OKΪ��
	{
		HERO.command->judge->Relive_Flag = false;
	}
	
//	//Ħ���ֶ�ת����
//	if(my_launch->info->flag->Fric_Stuck_Flag == true)
//	{
//		my_launch->launch_mode = L_Rescue;
//	}
}

/**
  * @brief  ����״̬��Ӧ
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Status_React(My_Launch_t* my_launch)
{
	switch(HERO.mode)
	{
		case Sleep_Mode:
			my_launch->launch_mode = L_Sleep;
		  my_launch->feed_mode = F_Sleep;
			break;
		case Init_Mode:
			my_launch->launch_mode = L_Init;
		  my_launch->feed_mode = F_Init;
			break;
		default:
			/*״̬������Ҫ: ��һ��״̬���*/
		  if(my_launch->launch_mode == L_Standby)
			{
				/*����ָ����Ӧ����,��������ʽ��Ϊ����һ������δ���ʱ��������κ�ָ��*/
				My_Launch_Command_React(my_launch);
			}
			break;
	}
}

/**
  * @brief  �������ݸ���
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Data_Update(My_Launch_t* my_launch)
{
	Launch_Measure_t* measure = my_launch->info->measure;
	
	measure->l_fric_speed = my_launch->left_fric->rx_info.speed;
	measure->r_fric_speed = my_launch->right_fric->rx_info.speed;
	measure->position_speed = my_launch->position->rx_info.speed;
	measure->dial_speed = my_launch->feed->rx_info.speed;
	measure->dial_position = my_launch->feed->rx_info.angle_sum;/*��ֵ������һ�����ֵ�ڳ�ʼ��������*/
	
	/*�������*/
	My_Launch_Rate_of_Fire_Self_Fix(my_launch);
	
	/*����ϵͳ���� begin*/
	My_Launch_Rate_of_Fire_Limit_Check(my_launch);
	
//	My_Launch_Shoot_Times_Check(my_launch);
	/*����ϵͳ���� end*/
}

/**
  * @brief  ������ƣ��ú�������Ħ����������򹩵�ϵͳ����ָ��
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Control(My_Launch_t* my_launch)
{
	switch(my_launch->launch_mode)
	{
		case L_Sleep:
			if(((abs(my_launch->info->measure->l_fric_speed)) + abs(my_launch->info->measure->r_fric_speed)) <= LAUNCH_SLEEP_ALLOW_SPEED)
			{
				can2_0x1FF_send_buff[my_launch->left_fric->id.buff_p] = 0;
				can2_0x1FF_send_buff[my_launch->right_fric->id.buff_p] = 0;
				my_launch->cnt->sleep_cnt = 0;
			}
			else if(my_launch->cnt->sleep_cnt == LAUNCH_SLEEP_RPOCESS_TIME)
			{
				can2_0x1FF_send_buff[my_launch->left_fric->id.buff_p] = 0;
				can2_0x1FF_send_buff[my_launch->right_fric->id.buff_p] = 0;
			}
			else
			{
				my_launch->info->target->l_fric_speed = 0;
				my_launch->info->target->r_fric_speed = 0;
				Launch_L_Fric_Ctrl(my_launch);
				Launch_R_Fric_Ctrl(my_launch);
			}
			break;
		case L_Stop:
			my_launch->info->target->l_fric_speed = 0;
			my_launch->info->target->r_fric_speed = 0;
		  my_launch->launch_mode = L_Standby;
		  my_launch->feed_mode = F_Static;
		  my_launch->info->flag->Dial_Wake_OK = false;
		  my_launch->info->flag->Dial_Wake_Want_To_Stop = false;
			break;
		case L_Init:
			/*�ж��Ƿ�Ϊ����״̬�����ǣ����⸴���־λ�����ٿ���Ħ����*/
		  
		  /*���Ǳ���״̬*/
		  my_launch->info->target->l_fric_speed = 0;
			my_launch->info->target->r_fric_speed = 0;
		  my_launch->info->flag->Dial_Wake_OK = false;
		  my_launch->info->flag->Dial_Wake_Want_To_Stop = false;
		
		  /*�������� begin*/
		  my_launch->launch_mode = L_Standby;
		  /*�������� end*/
			break;
		case L_Wake:
			/*Ħ���� begin*/
		  my_launch->info->target->l_fric_speed = -(my_launch->info->fric_base_speed + LAUNCH_L_FRIC_SPEED_OFFSET + my_launch->info->client_modify_fric_speed + my_launch->info->fire_rate_fix);
			my_launch->info->target->r_fric_speed =  (my_launch->info->fric_base_speed + LAUNCH_R_FRIC_SPEED_OFFSET + my_launch->info->client_modify_fric_speed + my_launch->info->fire_rate_fix);
		  /*Ħ���� end*/
		
		  /*���� begin*/
			/*��λĦ��������*/
			if(My_Launch_Feed_Ready_Check(my_launch))
			{
				my_launch->cnt->dial_wake_delay_cnt++;
			  if(my_launch->cnt->dial_wake_delay_cnt == LAUNCH_DIAL_WAKE_DELAY_TIME)
			  {
		  		my_launch->cnt->dial_wake_delay_cnt = 0;
	  			if(my_launch->feed_mode == F_Static)
		  	  {
		  	  	my_launch->feed_mode = F_Reload;
		    		my_launch->cnt->dial_wake_reload_cnt++;
		   			if(my_launch->cnt->dial_wake_reload_cnt > LAUNCH_FEED_WAKE_RELOAD_TIMES)//������������
		   			{
	    				my_launch->cnt->dial_wake_reload_cnt = 0;
	    				my_launch->info->flag->Dial_Wake_OK = true;
	    				my_launch->launch_mode = L_Standby;
	  	 			}
						/*δ��ָ������*/
						else
						{
							if(my_launch->info->flag->Dial_Wake_Want_To_Stop == true)//���ֶ�ת�����жϲ�����������
							{
								my_launch->cnt->dial_wake_reload_cnt = 0;
	    			    my_launch->info->flag->Dial_Wake_OK = true;
	    			    my_launch->launch_mode = L_Standby;
							}
						}
	  		  }
			  }
			  /*��reload_mode*/
			  else
			  {
				  	
			  }
		  }
			/*��λĦ��������*/
			else
			{
				my_launch->launch_mode = L_Standby;
			}
			
			/*��ƭ��λ��*/
			my_launch->info->feed_reload_bullet_num = 0;
			
		  /*���� end*/
			HERO.command->launch->Fire = false;
			HERO.command->launch->Clear_Heat = false;
			break;
		case L_Shoot:
		  my_launch->feed_mode = F_Reload;
			my_launch->launch_mode = L_Standby;
			break;
//		case L_Rescue:
//			my_launch->info->target->l_fric_speed = FRIC_RESCUE_SPEED;
//		  my_launch->info->target->r_fric_speed = -(FRIC_RESCUE_SPEED);
//		  
//		  HERO.command->gimbal->Rescue_Fric_Up = true;
//		  
//		  if((abs(my_launch->info->measure->l_fric_speed) >= 2000) && (abs(my_launch->info->measure->r_fric_speed) >= 2000))
//			{
//				HERO.command->gimbal->Rescue_Fric_Down = true;//��gimbal��0
//				my_launch->info->flag->Fric_Stuck_Flag = false;
//				my_launch->launch_mode = L_Wake;
//			}
//			
//			my_launch->cnt->fric_rescue_cnt++;
//			if(my_launch->cnt->fric_rescue_cnt >= FRIC_RESCUE_ALLOW_TIME)
//			{
//				my_launch->cnt->fric_rescue_cnt = 0;
//				my_launch->info->flag->Fric_Stuck_Flag = false;
//				HERO.command->gimbal->Rescue_Fric_Up = false;
//				HERO.command->gimbal->Rescue_Fric_Down = false;
//				my_launch->launch_mode = L_Wake;
//			}
//			
//			break;
		case L_Standby:
			/*����̬*/
		  if(my_launch->info->flag->Dial_Wake_OK == true)
			{
				my_launch->info->target->l_fric_speed = -(my_launch->info->fric_base_speed + LAUNCH_L_FRIC_SPEED_OFFSET + my_launch->info->client_modify_fric_speed + my_launch->info->fire_rate_fix);
			  my_launch->info->target->r_fric_speed =  (my_launch->info->fric_base_speed + LAUNCH_R_FRIC_SPEED_OFFSET + my_launch->info->client_modify_fric_speed + my_launch->info->fire_rate_fix);
			}
		  /*���������������٣�����Ħ����ת��*/
			break;
		default:
			break;
	}
	
	/*˯��ģʽ�������*/
	if(my_launch->launch_mode != L_Sleep)
	{
		/*Ħ���ֳ������ƣ�˯�߼���ֵ����*/
		my_launch->cnt->sleep_cnt = 0;
		
		Launch_L_Fric_Ctrl(my_launch);
		Launch_R_Fric_Ctrl(my_launch);
	}
}

/*test begin*/
uint32_t pos_test = 0;
uint32_t dia_test = 0;

uint32_t pos_offset = 0;
uint32_t dial_offset = 0;
/*test end*/

/**
  * @brief  �������ƣ�������λ�Ͳ���
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Feeder_Control(My_Launch_t* my_launch)
{
	/*��λ���� begin*/
	switch(my_launch->feed_mode)
	{
		case F_Sleep:
			can2_0x200_send_buff[my_launch->position->id.buff_p] = 0;
			break;
		case F_Init:
			my_launch->info->target->position_speed = 0;
			Position_Ctrl(my_launch);
			break;
		case F_Reload:
			/*������󷢵���*/
			switch(my_launch->info->feed_reload_bullet_num)
			{
				case 1:
				  my_launch->info->position_reload_max = LAUNCH_POSITION_RELOAD_ONE_TIME;
				  break;
				case 2:
				  my_launch->info->position_reload_max = LAUNCH_POSITION_RELOAD_TWO_TIME;
				  break;
				case 3:
				  my_launch->info->position_reload_max = LAUNCH_POSITION_RELOAD_THREE_TIME;
				  break;
				case 10:
					my_launch->info->position_reload_max = LAUNCH_POSITION_RELOAD_TEN_TIME;
					break;
				default:
					my_launch->info->target->position_speed = 0;
					my_launch->info->flag->Position_Reload_OK = true;
				  break;
			}
			if(my_launch->info->flag->Position_Reload_OK != true)
			{
				my_launch->cnt->position_reload_time++;
				my_launch->info->target->position_speed = LAUNCH_POSITION_RELOAD_SPEED;
				if(my_launch->cnt->position_reload_time >= my_launch->info->position_reload_max \
					 || my_launch->position->c_stuck_flag(my_launch->position, 1, 500, 100))
				{
				  my_launch->info->target->position_speed = 0;
				  my_launch->cnt->position_reload_time = 0;
				  my_launch->info->flag->Position_Reload_OK = true;
				}
			 }
			Position_Ctrl(my_launch);
			break;
		case F_Static:
			my_launch->info->target->position_speed = 0;
		  my_launch->info->flag->Position_Reload_OK = false;
		  Position_Ctrl(my_launch);
			break;
		default:
			break;
	}
	/*��λ���� end*/
	
	/*���̿��� begin*/
	switch(my_launch->feed_mode)
	{
		case F_Sleep:
			can1_0x1FF_send_buff[my_launch->feed->id.buff_p] = 0; 
		  /*���б�־λ���� begin*/
		  my_launch->info->flag->Dial_Processing_Flag = false;
		  my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
		  /*���б�־λ���� end*/
		  
		  /*���м���ֵ���� begin*/
		  my_launch->cnt->dial_reload_allow_cnt = 0;
		  my_launch->cnt->dial_stuck_cnt = 0;
			my_launch->cnt->dial_reload_allow_cnt = 0;
		  /*���м���ֵ���� begin*/
			break;
		case F_Init:
       /*���б�־λ���� begin*/
		  my_launch->info->flag->Dial_Processing_Flag = false;
		  my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
		  /*���б�־λ���� end*/
		  
		  /*���м���ֵ���� begin*/
		  my_launch->cnt->dial_reload_allow_cnt = 0;
		  my_launch->cnt->dial_stuck_cnt = 0;
			my_launch->cnt->dial_reload_allow_cnt = 0;
		  /*���м���ֵ���� begin*/
		  
		  my_launch->feed_mode = F_Static;
			break;
		case F_Reload:
			/*��λ��ת*/
			if(my_launch->info->flag->Position_Reload_OK == true || (my_launch->cnt->position_reload_time >= (my_launch->info->position_reload_max - 50)) \
				 || my_launch->info->feed_reload_bullet_num >= 2)
			{
			/*��һ���յ�ָ�����Ŀ��ֵ*/
			  if(my_launch->info->flag->Dial_Processing_Flag != true)
			  {
				  /*���򼸷�,wake����*/
				  switch(my_launch->info->feed_reload_bullet_num)
				  {
					  case 1:
						  my_launch->info->dial_reload_angle = DIAL_RELOAD_ONE_ANGLE;
						  my_launch->info->feed_reload_max_time = LAUNCH_DIAL_RELOAD_ONE_ALLOW_TIME;
						  break;
					  case 2:
						  my_launch->info->dial_reload_angle = DIAL_RELOAD_TWO_ANGLE;
						  my_launch->info->feed_reload_max_time = LAUNCH_DIAL_RELOAD_TWO_ALLOW_TIME;
						  break;
					  case 3:
						  my_launch->info->dial_reload_angle = DIAL_RELOAD_THREE_ANGLE;
						  my_launch->info->feed_reload_max_time = LAUNCH_DIAL_RELOAD_THREE_ALLOW_TIME;
						  break;
						case 10:
							my_launch->info->dial_reload_angle = DIAL_RELOAD_TEN_ANGLE;
						  my_launch->info->feed_reload_max_time = LAUNCH_POSITION_RELOAD_TEN_TIME;
							break;
					  default:
						  if(my_launch->launch_mode == L_Wake)
						  {
							  my_launch->info->dial_reload_angle = DIAL_RELOAD_ONE_ANGLE;
							  my_launch->cnt->dial_test_fire_cnt = 0;
						  }
						  else
						  {
							  my_launch->info->dial_reload_angle = 0;
						  }
							my_launch->info->feed_reload_max_time = LAUNCH_POSITION_RELOAD_ONE_TIME;
						  break;
				  }
				  my_launch->info->target->dial_position = my_launch->info->measure->dial_position + (my_launch->info->dial_reload_angle);
				  my_launch->info->dial_final_target = my_launch->info->target->dial_position;
				  my_launch->info->flag->Dial_Processing_Flag = true;
				  my_launch->info->dial_target_reserve = my_launch->info->measure->dial_position;
			  }
			  else
			  {
				  /*δ��Ŀ��ֵ��ת�����ڴ����ת*/
				  if(my_launch->feed->c_stuck_flag(my_launch->feed, 2, 1000, 80) || my_launch->info->flag->Dial_Stuck_Processing_Flag == true)
				  {
					  if(my_launch->info->flag->Dial_Wake_OK != true)
					  {
						  my_launch->info->flag->Dial_Wake_Want_To_Stop = true;
						  my_launch->info->flag->Dial_Reload_OK = true;
					  }
//						/*��ת���� begin*/
//					  /*��ǰ��ת�����һ���Ƕ�*/
//				  	if(my_launch->info->flag->Dial_Stuck_Processing_Flag != true)
//			  		{
//			  			if(my_launch->cnt->dial_stuck_cnt < LAUNCH_DIAL_STUCK_PROCESS_TIMES)
//			  			{
//		  					my_launch->info->target->dial_position = my_launch->info->measure->dial_position - LAUNCH_DIAL_STUCK_B_POSITION;
//		  			    my_launch->cnt->dial_stuck_cnt++;
//		  				  my_launch->info->flag->Dial_Stuck_Processing_Flag = true;
//		  				}
//			  			/*�µ�û�취�ˣ�����λ��*/
//			  			else
//			  			{
//			  				my_launch->info->target->dial_position = my_launch->info->dial_target_reserve;
//			  				my_launch->info->flag->Dial_Processing_Flag = false;
//								my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
//  		  			  my_launch->info->flag->Dial_Reload_OK = true;
//					  	  my_launch->cnt->dial_stuck_cnt = 0;
//								
//						  }
//					  }
//					  /*���ڴ����ת*/
//					  else
//					  {
//						  /*�����˶�ת�򵽴����Ŀ��ֵ*/
//						  if(my_launch->feed->c_stuck_flag(my_launch->feed, 2000, 100) || \
//						  		abs(my_launch->feed->pid.position.info.err) <= 2000)
//						  {
//							  /*������Ӧ����Ŀ��ֵ����ת�������������ת�������ֵδ����*/
//							  my_launch->info->target->dial_position = my_launch->info->dial_final_target;
//							  my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
//						  }
//					  }
////            /*��ת���� end*/
//					}
//					else
//					{
						/*���δ򵯲������ת*/
              my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
              my_launch->info->target->dial_position = my_launch->info->measure->dial_position;
					    my_launch->info->flag->Dial_Processing_Flag = false;
  				    my_launch->info->flag->Dial_Reload_OK = true;
					    my_launch->cnt->dial_stuck_cnt = 0;
//					}
				  }
				  /*û�ж�ת*/
				  else
				  {
					  /*����Ŀ��ֵ*/
					  if(abs(my_launch->feed->pid.position.info.err) <= 1000)
					  {
					  	my_launch->info->dial_delta_offset = my_launch->info->target->dial_position - my_launch->info->measure->dial_position;
						  my_launch->info->flag->Dial_Processing_Flag = false;
  					  my_launch->info->flag->Dial_Reload_OK = true;
						  my_launch->cnt->dial_stuck_cnt = 0;
					  }
					  /*��������Ŀ��ֵ*/
					  else
					  {
						
					  }
				  }
			  }
		  }
			
			/*��ʱ���� begin*/
			my_launch->cnt->dial_reload_allow_cnt++;
			if(my_launch->cnt->dial_reload_allow_cnt == my_launch->info->feed_reload_max_time)
			{
				my_launch->cnt->dial_stuck_cnt = 0;
				my_launch->cnt->dial_reload_allow_cnt = 0;
				my_launch->info->flag->Dial_Processing_Flag = false;
				my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
				my_launch->info->target->dial_position = my_launch->info->measure->dial_position;
				my_launch->feed_mode = F_Static;
			}
		  /*��ʱ���� end*/
			
		  Dial_Position_Ctrl(my_launch);
			break;
		case F_Static:
			my_launch->info->target->dial_speed = 0;
		  my_launch->cnt->dial_reload_allow_cnt = 0;
		  my_launch->cnt->position_delay_dial_cnt = 0;
		  my_launch->info->flag->Dial_Reload_OK = false;
		  my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
		  Dial_Speed_Ctrl(my_launch);
			break;
		default:
			break;
	}
	/*���̿��� end*/
	
	if(my_launch->feed_mode == F_Reload)
	{
		if(my_launch->info->flag->Dial_Reload_OK == true && my_launch->info->flag->Position_Reload_OK == true)
		{
			my_launch->feed_mode = F_Static;
			my_launch->info->flag->Position_Reload_OK = false;
			my_launch->info->flag->Dial_Reload_OK = false;
			HERO.command->launch->Kill_Me_Please = false;
		}
	}
}

/*���Ա��� begin*/
int16_t l_fric_current_test = 0;
int16_t l_fric_speed_test = 0;
int16_t r_fric_current_test = 0;
int16_t r_fric_speed_test = 0;
/*���Ա��� end*/


void My_Launcher_Work(void)
{
	My_Launch_Offline(&My_Launch);
	
	My_Launch_Data_Update(&My_Launch);
	
	My_Launch_Status_React(&My_Launch);
	
	My_Launch_Control(&My_Launch);
	
	My_Feeder_Control(&My_Launch);
	
	/*���Դ��� begin*/
	l_fric_current_test = My_Launch.left_fric->rx_info.current;
	l_fric_speed_test = My_Launch.left_fric->rx_info.speed;
	r_fric_current_test = My_Launch.right_fric->rx_info.current;
	r_fric_speed_test = My_Launch.right_fric->rx_info.speed;
	/*���Դ��� begin*/
}


/*...............................................������...............................................*/

/**
  * @brief  ����׼�����
  * @param  My_Launch_t* my_launch
  * @retval uint8_t ��ɷ���1��δ��ɷ���0
  */
uint8_t My_Launch_Ready_Check(My_Launch_t* my_launch)
{
	if(my_launch->left_fric->state.work_state == M_ONLINE && my_launch->right_fric->state.work_state == M_ONLINE \
		 && my_launch->position->state.work_state == M_ONLINE \
	   && abs(my_launch->left_fric->rx_info.speed) >= LAUNCH_FRIC_READY_SPEED \
	   && abs(my_launch->right_fric->rx_info.speed) >= LAUNCH_FRIC_READY_SPEED)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
  * @brief  ���������
  * @param  My_Launch_t* my_launch
  * @retval uint8_t ��ɷ���1��δ��ɷ���0
  */
uint8_t My_Launch_Feed_Ready_Check(My_Launch_t* my_launch)
{
	if(my_launch->left_fric->state.work_state == M_ONLINE && my_launch->right_fric->state.work_state == M_ONLINE \
		 && my_launch->position->state.work_state == M_ONLINE)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
  * @brief  Ħ���ֵ����仯������
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Shoot_Times_Check(My_Launch_t* my_launch)
{
	static int16_t L_Current_Now = 0, L_Current_Last = 0, R_Current_Now = 0, R_Current_Last = 0;
	if(my_launch->info->flag->Dial_Wake_OK == true)
	{
		L_Current_Now = my_launch->left_fric->rx_info.current;
		R_Current_Now = my_launch->right_fric->rx_info.current;
		
		if((abs(L_Current_Now - L_Current_Last) >= 4000 || abs(R_Current_Now - R_Current_Last) >= 4000) \
 			 && my_launch->cnt->fric_test_fire_delay_cnt == 0)
		{
			my_launch->cnt->fric_test_fire_cnt++;
			my_launch->cnt->fric_test_fire_delay_cnt += 200;
		}
		else
		{
			if(my_launch->cnt->fric_test_fire_delay_cnt != 0)
			{
				my_launch->cnt->fric_test_fire_delay_cnt--;
			}
			else
			{
				my_launch->cnt->fric_test_fire_delay_cnt = 0;
			}
			R_Current_Last = R_Current_Now;
			L_Current_Last = L_Current_Now;
		}
	}
	else
	{
		my_launch->cnt->fric_test_fire_cnt = 0;
		my_launch->cnt->fric_test_fire_delay_cnt = 0;
	}
}


/**
  * @brief  �����������ɴ��ӵ�����
  * @param  My_Launch_t* my_launch
  * @retval None
  */
uint16_t My_Launch_Clear_Heat_Cal(My_Launch_t* my_launch)
{
	uint16_t bullet_num = 0;
	
	if(My_Judge.status->status == DEV_ONLINE)
	{
		bullet_num = (My_Judge.info->shooter_cooling_limit - My_Judge.info->shooter_cooling_heat) / 100;
	}
	else
	{
		bullet_num = 2;
	}
	
	return bullet_num;
}

/**
  * @brief  ����ϵͳ��������
  * @param  My_Launch_t* my_launch
  * @retval true �������ɴ�false �������ɴ�
  */
bool My_Launch_Fire_Limit_Check(My_Launch_t* my_launch)
{
	if(My_Judge.status->status == DEV_ONLINE)
	{
		if((My_Judge.info->shooter_cooling_limit - My_Judge.info->shooter_cooling_heat) >= 100 || HERO.command->launch->Kill_Me_Please == true)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

/**
  * @brief  ����ϵͳ��������
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Rate_of_Fire_Limit_Check(My_Launch_t* my_launch)
{
	if(My_Judge.status->status == DEV_ONLINE)
	{
		switch(My_Judge.info->shooter_id1_42mm_speed_limit)
		{
			case 10:
				my_launch->info->fric_base_speed = LAUNCH_FRIC_ORG_SPEED_10;
				break;
			case 16:
				my_launch->info->fric_base_speed = LAUNCH_FRIC_ORG_SPEED_16;
				break;
			default:
				my_launch->info->fric_base_speed = LAUNCH_FRIC_ORG_SPEED_16;
				break;
		}
	}
	else
	{
		my_launch->info->fric_base_speed = LAUNCH_FRIC_ORG_SPEED_16;
	}
}

/*test*/
float test_rate[300];
int16_t cnt_test = 0;
float fire_rate_now = 0.f;
float fire_rate_last = 0.f;

/**
  * @brief  Ħ����ת������Ӧ
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Rate_of_Fire_Self_Fix(My_Launch_t* my_launch)
{
	fire_rate_now = My_Judge.info->shooting_speed;
	
	if(My_Judge.status->status == DEV_ONLINE && fire_rate_now != 0 && (fire_rate_now != fire_rate_last) \
		 && abs(My_Judge.info->shooting_speed - 14.9f) <= 5.f)
	{
		if(My_Judge.info->shooting_speed >= FIRE_PERFECT_RATE_MAX)
		{
			my_launch->info->fire_rate_fix -= 15;
			my_launch->cnt->rate_above_perfect++;
			if(my_launch->cnt->rate_above_perfect >= 2)
			{
				my_launch->info->fire_rate_fix -= 7;
				my_launch->cnt->rate_above_perfect = 0;
			}
		}
		else if(My_Judge.info->shooting_speed <= FIRE_PERFECT_RATE_LOW)
		{
			my_launch->info->fire_rate_fix += 7;
			my_launch->cnt->rate_below_perfect++;
			if(my_launch->cnt->rate_below_perfect >= 2)
				{
					my_launch->info->fire_rate_fix += 15;
					my_launch->cnt->rate_below_perfect = 0;
				}
		}
		else
		{
			my_launch->cnt->rate_above_perfect = 0;
			my_launch->cnt->rate_below_perfect = 0;
		}
	}
	else
	{
		
	}
	
	/*�Ӿ����Դ��� begin*/
	if(fire_rate_now != fire_rate_last)
	{
		test_rate[cnt_test] = fire_rate_now;
		cnt_test++;
		if(cnt_test == 299)
		{
			cnt_test = 0;
		}
	}
	/*�Ӿ����Դ��� end*/
	
	fire_rate_last = fire_rate_now;
}

/**
  * @brief  Ħ���ֶ�ת�Ծ�
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Fric_Stuck_Rescue(My_Launch_t* my_launch)
{
	if(my_launch->left_fric->c_stuck_flag(my_launch->left_fric, 1, 1000, 1000) && \
		 my_launch->right_fric->c_stuck_flag(my_launch->right_fric, 1, 1000, 1000) && my_launch->info->flag->Fric_Stuck_Flag != true)
	{
		my_launch->info->flag->Fric_Stuck_Flag = true;
	}
	else
	{
		my_launch->info->flag->Fric_Stuck_Flag = false;
	}
}

/**
  * @brief  ����pitch�ǶȶԹ����Ƕȡ�ʱ���Ӱ��
  * @param  My_Launch_t* my_launch,���������ͣ�����1��ʾ����λ����������2��ʾ�Բ��̲���
  * @retval None
  */
//uint32_t My_Launch_P_A_Offset(My_Launch_t* my_launch, uint8_t type)
//{
//	
//}

/*...............................................�������...............................................*/

/**
  * @brief  l_fric�ٶȿ���
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Launch_L_Fric_Ctrl(My_Launch_t* my_launch)
{
	motor_t *motor = my_launch->left_fric;
	
	/*���ǰ���Ŀ��ֵ*/
	if(abs(my_launch->info->target->l_fric_speed) > LAUNCH_FRIC_MAX_ALLOW_SPEED)
	{
		my_launch->info->target->l_fric_speed = sgn(my_launch->info->target->l_fric_speed) * LAUNCH_FRIC_MAX_ALLOW_SPEED;
	}
	
	/*���*/
	can2_0x1FF_send_buff[motor->id.buff_p] = motor->c_speed(motor,my_launch->info->target->l_fric_speed);
}

/**
  * @brief  r_fric�ٶȿ���
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Launch_R_Fric_Ctrl(My_Launch_t* my_launch)
{
	motor_t *motor = my_launch->right_fric;
	
	/*���ǰ���Ŀ��ֵ*/
	if(abs(my_launch->info->target->r_fric_speed) > LAUNCH_FRIC_MAX_ALLOW_SPEED)
	{
		my_launch->info->target->r_fric_speed = sgn(my_launch->info->target->r_fric_speed) * LAUNCH_FRIC_MAX_ALLOW_SPEED;
	}
	
	/*���*/
	can2_0x1FF_send_buff[motor->id.buff_p] = motor->c_speed(motor,my_launch->info->target->r_fric_speed);
}

/**
  * @brief  position�ٶȿ���
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Position_Ctrl(My_Launch_t* my_launch)
{
	motor_t* motor = my_launch->position;
	
	can2_0x200_send_buff[motor->id.buff_p] = motor->c_speed(motor,my_launch->info->target->position_speed);
}


/**
  * @brief  dialλ�ÿ���
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Dial_Position_Ctrl(My_Launch_t* my_launch)
{
	motor_t* motor = my_launch->feed;
	
	can1_0x1FF_send_buff[motor->id.buff_p] = motor->c_posit(motor,my_launch->info->target->dial_position);
}

/**
  * @brief  dial�ٶȿ���
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Dial_Speed_Ctrl(My_Launch_t* my_launch)
{
	motor_t* motor = my_launch->feed;
	
	can1_0x1FF_send_buff[motor->id.buff_p] = motor->c_speed(motor,my_launch->info->target->dial_speed);
}

