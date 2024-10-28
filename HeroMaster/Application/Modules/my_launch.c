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
  * @brief  发射初始化
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
  * @brief  发射心跳包
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
  * @brief  发射离线处理
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Offline(My_Launch_t* my_launch)
{
	//自爆死亡后清理标志位
	if(my_launch->left_fric->state.work_state == M_OFFLINE || my_launch->right_fric->state.work_state == M_OFFLINE)
	{
		HERO.command->launch->Kill_Me_Please = false;
		
		my_launch->launch_mode = L_Init;
	  my_launch->feed_mode = F_Init;
	}
}

uint16_t cnt_test_0 = 0;

/**
  * @brief  发射指令响应
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Launch_Command_React(My_Launch_t* my_launch)
{
	//射击
	if(HERO.command->launch->Fire == true || HERO.command->launch->Clear_Heat == true || HERO.command->launch->Kill_Me_Please == true)
	{
		/*发射机构已被唤醒*/
		if(My_Launch_Ready_Check(my_launch) && my_launch->info->flag->Dial_Wake_OK == true && My_Launch_Fire_Limit_Check(my_launch))
		{
			my_launch->launch_mode = L_Shoot;
			
			/*发弹量计算 begin*/
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
			/*发弹量计算 end*/
			
			/*检测视觉自动打弹，若进入检测陀螺状态且开启自瞄*/
			if(My_Vision.transla_info->detect_top_mode == true && HERO.command->vision->Auto_Catch == true)
			{
				/*若可以打弹指令不为真，则不打弹*/
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
		/*若发射机构未被唤醒，则进入唤醒状态*/
		else
		{
			my_launch->launch_mode = L_Wake;
		}
		
		HERO.command->launch->Fire = false;
		HERO.command->launch->Clear_Heat = false;
		HERO.command->launch->Kill_Me_Please = false;
	}
	
	//停止
	if(HERO.command->launch->Stop == true)
	{
		my_launch->launch_mode = L_Stop;
		HERO.command->launch->Stop = false;
	}
	//调转速
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
	
	//复活后自动就绪摩擦轮,好像一个标志位就够了欸
	if((HERO.command->judge->Relive_Flag == true || HERO.command->judge->Fighting_Flag == true)\
 		  && my_launch->info->flag->Dial_Wake_OK == false)
	{
		my_launch->launch_mode = L_Wake;
	}
	else//Relive_Flag为假或Dial_Wake_OK为真
	{
		HERO.command->judge->Relive_Flag = false;
	}
	
//	//摩擦轮堵转处理
//	if(my_launch->info->flag->Fric_Stuck_Flag == true)
//	{
//		my_launch->launch_mode = L_Rescue;
//	}
}

/**
  * @brief  发射状态响应
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
			/*状态更改需要: 上一个状态完成*/
		  if(my_launch->launch_mode == L_Standby)
			{
				/*发射指令响应函数,这样的形式以为着在一个环节未完成时不会接收任何指令*/
				My_Launch_Command_React(my_launch);
			}
			break;
	}
}

/**
  * @brief  发射数据更新
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
	measure->dial_position = my_launch->feed->rx_info.angle_sum;/*该值与另外一个相关值在初始化后清零*/
	
	/*检测射速*/
	My_Launch_Rate_of_Fire_Self_Fix(my_launch);
	
	/*裁判系统限制 begin*/
	My_Launch_Rate_of_Fire_Limit_Check(my_launch);
	
//	My_Launch_Shoot_Times_Check(my_launch);
	/*裁判系统限制 end*/
}

/**
  * @brief  发射控制，该函数控制摩擦轮输出，向供弹系统发送指令
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
			/*判断是否为比赛状态，若是，则检测复活标志位，快速开启摩擦轮*/
		  
		  /*若非比赛状态*/
		  my_launch->info->target->l_fric_speed = 0;
			my_launch->info->target->r_fric_speed = 0;
		  my_launch->info->flag->Dial_Wake_OK = false;
		  my_launch->info->flag->Dial_Wake_Want_To_Stop = false;
		
		  /*仅测试用 begin*/
		  my_launch->launch_mode = L_Standby;
		  /*仅测试用 end*/
			break;
		case L_Wake:
			/*摩擦轮 begin*/
		  my_launch->info->target->l_fric_speed = -(my_launch->info->fric_base_speed + LAUNCH_L_FRIC_SPEED_OFFSET + my_launch->info->client_modify_fric_speed + my_launch->info->fire_rate_fix);
			my_launch->info->target->r_fric_speed =  (my_launch->info->fric_base_speed + LAUNCH_R_FRIC_SPEED_OFFSET + my_launch->info->client_modify_fric_speed + my_launch->info->fire_rate_fix);
		  /*摩擦轮 end*/
		
		  /*供弹 begin*/
			/*限位摩擦轮在线*/
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
		   			if(my_launch->cnt->dial_wake_reload_cnt > LAUNCH_FEED_WAKE_RELOAD_TIMES)//补弹次数够了
		   			{
	    				my_launch->cnt->dial_wake_reload_cnt = 0;
	    				my_launch->info->flag->Dial_Wake_OK = true;
	    				my_launch->launch_mode = L_Standby;
	  	 			}
						/*未到指定次数*/
						else
						{
							if(my_launch->info->flag->Dial_Wake_Want_To_Stop == true)//出现堵转，则判断补弹次数够了
							{
								my_launch->cnt->dial_wake_reload_cnt = 0;
	    			    my_launch->info->flag->Dial_Wake_OK = true;
	    			    my_launch->launch_mode = L_Standby;
							}
						}
	  		  }
			  }
			  /*在reload_mode*/
			  else
			  {
				  	
			  }
		  }
			/*限位摩擦轮离线*/
			else
			{
				my_launch->launch_mode = L_Standby;
			}
			
			/*诱骗限位轮*/
			my_launch->info->feed_reload_bullet_num = 0;
			
		  /*供弹 end*/
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
//				HERO.command->gimbal->Rescue_Fric_Down = true;//在gimbal置0
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
			/*过渡态*/
		  if(my_launch->info->flag->Dial_Wake_OK == true)
			{
				my_launch->info->target->l_fric_speed = -(my_launch->info->fric_base_speed + LAUNCH_L_FRIC_SPEED_OFFSET + my_launch->info->client_modify_fric_speed + my_launch->info->fire_rate_fix);
			  my_launch->info->target->r_fric_speed =  (my_launch->info->fric_base_speed + LAUNCH_R_FRIC_SPEED_OFFSET + my_launch->info->client_modify_fric_speed + my_launch->info->fire_rate_fix);
			}
		  /*可以在这里检测射速，调节摩擦轮转速*/
			break;
		default:
			break;
	}
	
	/*睡眠模式独立输出*/
	if(my_launch->launch_mode != L_Sleep)
	{
		/*摩擦轮持续控制，睡眠计数值清零*/
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
  * @brief  供弹控制，包括限位和拨盘
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void My_Feeder_Control(My_Launch_t* my_launch)
{
	/*限位控制 begin*/
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
			/*检测请求发弹量*/
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
	/*限位控制 end*/
	
	/*拨盘控制 begin*/
	switch(my_launch->feed_mode)
	{
		case F_Sleep:
			can1_0x1FF_send_buff[my_launch->feed->id.buff_p] = 0; 
		  /*所有标志位清零 begin*/
		  my_launch->info->flag->Dial_Processing_Flag = false;
		  my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
		  /*所有标志位清零 end*/
		  
		  /*所有计数值清零 begin*/
		  my_launch->cnt->dial_reload_allow_cnt = 0;
		  my_launch->cnt->dial_stuck_cnt = 0;
			my_launch->cnt->dial_reload_allow_cnt = 0;
		  /*所有计数值清零 begin*/
			break;
		case F_Init:
       /*所有标志位清零 begin*/
		  my_launch->info->flag->Dial_Processing_Flag = false;
		  my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
		  /*所有标志位清零 end*/
		  
		  /*所有计数值清零 begin*/
		  my_launch->cnt->dial_reload_allow_cnt = 0;
		  my_launch->cnt->dial_stuck_cnt = 0;
			my_launch->cnt->dial_reload_allow_cnt = 0;
		  /*所有计数值清零 begin*/
		  
		  my_launch->feed_mode = F_Static;
			break;
		case F_Reload:
			/*限位先转*/
			if(my_launch->info->flag->Position_Reload_OK == true || (my_launch->cnt->position_reload_time >= (my_launch->info->position_reload_max - 50)) \
				 || my_launch->info->feed_reload_bullet_num >= 2)
			{
			/*第一次收到指令，设置目标值*/
			  if(my_launch->info->flag->Dial_Processing_Flag != true)
			  {
				  /*看打几发,wake除外*/
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
				  /*未达目标值堵转或正在处理堵转*/
				  if(my_launch->feed->c_stuck_flag(my_launch->feed, 2, 1000, 80) || my_launch->info->flag->Dial_Stuck_Processing_Flag == true)
				  {
					  if(my_launch->info->flag->Dial_Wake_OK != true)
					  {
						  my_launch->info->flag->Dial_Wake_Want_To_Stop = true;
						  my_launch->info->flag->Dial_Reload_OK = true;
					  }
//						/*堵转处理 begin*/
//					  /*向前堵转，向后拨一定角度*/
//				  	if(my_launch->info->flag->Dial_Stuck_Processing_Flag != true)
//			  		{
//			  			if(my_launch->cnt->dial_stuck_cnt < LAUNCH_DIAL_STUCK_PROCESS_TIMES)
//			  			{
//		  					my_launch->info->target->dial_position = my_launch->info->measure->dial_position - LAUNCH_DIAL_STUCK_B_POSITION;
//		  			    my_launch->cnt->dial_stuck_cnt++;
//		  				  my_launch->info->flag->Dial_Stuck_Processing_Flag = true;
//		  				}
//			  			/*堵得没办法了，到限位了*/
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
//					  /*正在处理堵转*/
//					  else
//					  {
//						  /*向后回退堵转或到达回退目标值*/
//						  if(my_launch->feed->c_stuck_flag(my_launch->feed, 2000, 100) || \
//						  		abs(my_launch->feed->pid.position.info.err) <= 2000)
//						  {
//							  /*重新响应最终目标值，堵转处理结束，但堵转处理计数值未清零*/
//							  my_launch->info->target->dial_position = my_launch->info->dial_final_target;
//							  my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
//						  }
//					  }
////            /*堵转处理 end*/
//					}
//					else
//					{
						/*单次打弹不处理堵转*/
              my_launch->info->flag->Dial_Stuck_Processing_Flag = false;
              my_launch->info->target->dial_position = my_launch->info->measure->dial_position;
					    my_launch->info->flag->Dial_Processing_Flag = false;
  				    my_launch->info->flag->Dial_Reload_OK = true;
					    my_launch->cnt->dial_stuck_cnt = 0;
//					}
				  }
				  /*没有堵转*/
				  else
				  {
					  /*到达目标值*/
					  if(abs(my_launch->feed->pid.position.info.err) <= 1000)
					  {
					  	my_launch->info->dial_delta_offset = my_launch->info->target->dial_position - my_launch->info->measure->dial_position;
						  my_launch->info->flag->Dial_Processing_Flag = false;
  					  my_launch->info->flag->Dial_Reload_OK = true;
						  my_launch->cnt->dial_stuck_cnt = 0;
					  }
					  /*正在走向目标值*/
					  else
					  {
						
					  }
				  }
			  }
		  }
			
			/*超时处理 begin*/
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
		  /*超时处理 end*/
			
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
	/*拨盘控制 end*/
	
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

/*测试变量 begin*/
int16_t l_fric_current_test = 0;
int16_t l_fric_speed_test = 0;
int16_t r_fric_current_test = 0;
int16_t r_fric_speed_test = 0;
/*测试变量 end*/


void My_Launcher_Work(void)
{
	My_Launch_Offline(&My_Launch);
	
	My_Launch_Data_Update(&My_Launch);
	
	My_Launch_Status_React(&My_Launch);
	
	My_Launch_Control(&My_Launch);
	
	My_Feeder_Control(&My_Launch);
	
	/*测试代码 begin*/
	l_fric_current_test = My_Launch.left_fric->rx_info.current;
	l_fric_speed_test = My_Launch.left_fric->rx_info.speed;
	r_fric_current_test = My_Launch.right_fric->rx_info.current;
	r_fric_speed_test = My_Launch.right_fric->rx_info.speed;
	/*测试代码 begin*/
}


/*...............................................处理函数...............................................*/

/**
  * @brief  发射准备检测
  * @param  My_Launch_t* my_launch
  * @retval uint8_t 完成返回1，未完成返回0
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
  * @brief  允许供弹检测
  * @param  My_Launch_t* my_launch
  * @retval uint8_t 完成返回1，未完成返回0
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
  * @brief  摩擦轮电流变化检测打弹数
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
  * @brief  清空热量计算可打子弹数量
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
  * @brief  裁判系统热量限制
  * @param  My_Launch_t* my_launch
  * @retval true 有热量可打，false 无热量可打
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
  * @brief  裁判系统射速限制
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
  * @brief  摩擦轮转速自适应
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
	
	/*视觉测试代码 begin*/
	if(fire_rate_now != fire_rate_last)
	{
		test_rate[cnt_test] = fire_rate_now;
		cnt_test++;
		if(cnt_test == 299)
		{
			cnt_test = 0;
		}
	}
	/*视觉测试代码 end*/
	
	fire_rate_last = fire_rate_now;
}

/**
  * @brief  摩擦轮堵转自救
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
  * @brief  补偿pitch角度对供弹角度、时间的影响
  * @param  My_Launch_t* my_launch,补偿的类型，输入1表示对限位补偿，输入2表示对拨盘补偿
  * @retval None
  */
//uint32_t My_Launch_P_A_Offset(My_Launch_t* my_launch, uint8_t type)
//{
//	
//}

/*...............................................输出函数...............................................*/

/**
  * @brief  l_fric速度控制
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Launch_L_Fric_Ctrl(My_Launch_t* my_launch)
{
	motor_t *motor = my_launch->left_fric;
	
	/*输出前检查目标值*/
	if(abs(my_launch->info->target->l_fric_speed) > LAUNCH_FRIC_MAX_ALLOW_SPEED)
	{
		my_launch->info->target->l_fric_speed = sgn(my_launch->info->target->l_fric_speed) * LAUNCH_FRIC_MAX_ALLOW_SPEED;
	}
	
	/*输出*/
	can2_0x1FF_send_buff[motor->id.buff_p] = motor->c_speed(motor,my_launch->info->target->l_fric_speed);
}

/**
  * @brief  r_fric速度控制
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Launch_R_Fric_Ctrl(My_Launch_t* my_launch)
{
	motor_t *motor = my_launch->right_fric;
	
	/*输出前检查目标值*/
	if(abs(my_launch->info->target->r_fric_speed) > LAUNCH_FRIC_MAX_ALLOW_SPEED)
	{
		my_launch->info->target->r_fric_speed = sgn(my_launch->info->target->r_fric_speed) * LAUNCH_FRIC_MAX_ALLOW_SPEED;
	}
	
	/*输出*/
	can2_0x1FF_send_buff[motor->id.buff_p] = motor->c_speed(motor,my_launch->info->target->r_fric_speed);
}

/**
  * @brief  position速度控制
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Position_Ctrl(My_Launch_t* my_launch)
{
	motor_t* motor = my_launch->position;
	
	can2_0x200_send_buff[motor->id.buff_p] = motor->c_speed(motor,my_launch->info->target->position_speed);
}


/**
  * @brief  dial位置控制
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Dial_Position_Ctrl(My_Launch_t* my_launch)
{
	motor_t* motor = my_launch->feed;
	
	can1_0x1FF_send_buff[motor->id.buff_p] = motor->c_posit(motor,my_launch->info->target->dial_position);
}

/**
  * @brief  dial速度控制
  * @param  My_Launch_t* my_launch
  * @retval None
  */
void Dial_Speed_Ctrl(My_Launch_t* my_launch)
{
	motor_t* motor = my_launch->feed;
	
	can1_0x1FF_send_buff[motor->id.buff_p] = motor->c_speed(motor,my_launch->info->target->dial_speed);
}

