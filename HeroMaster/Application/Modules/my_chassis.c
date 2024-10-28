#include "my_chassis.h"
#include "rp_chassis.h"
#include "chassis_motor.h"
#include "remote.h"
#include "math_support.h"
#include "my_hero.h"
#include "config_can.h"
#include "string.h"
#include "my_gimbal.h"
#include "my_judge.h"
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
float wheels_speed_param[7] = {8.0f,0.33f,0,0,8000,8000,15000};

/*rp_pack_require begin*/
extern chassis_t Chassis;
Chassis_InitTypeDef Chassis_All;
/*rp_pack_require end*/

Chassis_Mode_t Chassis_Mode;
Chassis_RC_Input_t Chassis_RC_Input;
Chassis_Speed_Measure_t Chassis_Speed_Measure;
Chassis_Cnt_t Chassis_Cnt;

Chassis_Info_t Chassis_Info = 
{
	.speed_up_buff = 1,
	.rc_input = &Chassis_RC_Input,
	.speed_measure = &Chassis_Speed_Measure,
};

My_Chassis_t My_Chassis = 
{
	.mode = &Chassis_Mode,
	.info = &Chassis_Info,
	.cnt = &Chassis_Cnt,
	.rp_pack = &Chassis,
};

/**
  * @brief  底盘初始化
  * @param  None
  * @retval None
  */
void My_Chassis_Init(void)
{
	Chassis_Motor[LF_WHEEL].init(&Chassis_Motor[LF_WHEEL]);
	Chassis_Motor[RF_WHEEL].init(&Chassis_Motor[RF_WHEEL]);
	Chassis_Motor[LB_WHEEL].init(&Chassis_Motor[LB_WHEEL]);
	Chassis_Motor[RB_WHEEL].init(&Chassis_Motor[RB_WHEEL]);
	
	Chassis_Motor[LF_WHEEL].pid_init(&Chassis_Motor[LF_WHEEL].pid.speed,wheels_speed_param);
	Chassis_Motor[RF_WHEEL].pid_init(&Chassis_Motor[RF_WHEEL].pid.speed,wheels_speed_param);
	Chassis_Motor[LB_WHEEL].pid_init(&Chassis_Motor[LB_WHEEL].pid.speed,wheels_speed_param);
	Chassis_Motor[RB_WHEEL].pid_init(&Chassis_Motor[RB_WHEEL].pid.speed,wheels_speed_param);
  
	
	Chassis_All.motor_LF = &Chassis_Motor[LF_WHEEL];
	Chassis_All.motor_RF = &Chassis_Motor[RF_WHEEL];
	Chassis_All.motor_LB = &Chassis_Motor[LB_WHEEL];
	Chassis_All.motor_RB = &Chassis_Motor[RB_WHEEL];
	
	My_Chassis.rp_pack = &Chassis;
	
	Chassis.init(&Chassis, &Chassis_All);
}

/**
  * @brief  底盘心跳包
  * @param  None
  * @retval None
  */
void My_Chassis_Heartbeat(void)
{
	Chassis_Motor[LF_WHEEL].heartbeat(&Chassis_Motor[LF_WHEEL]);
	Chassis_Motor[RF_WHEEL].heartbeat(&Chassis_Motor[RF_WHEEL]);
	Chassis_Motor[LB_WHEEL].heartbeat(&Chassis_Motor[LB_WHEEL]);
	Chassis_Motor[RB_WHEEL].heartbeat(&Chassis_Motor[RB_WHEEL]);
}

/**
  * @brief  底盘离线处理
  * @param  None
  * @retval None
  */
void My_Chassis_Offline(void)
{
	
}

/**
  * @brief  底盘命令响应
  * @param  My_Chassis_t* my_chassis
  * @retval None
  */
void My_Chassis_Command_React(My_Chassis_t* my_chassis)
{
		/*加速指令响应*/
	if(HERO.command->chassis->Accelerate == true)
	{
		my_chassis->info->speed_up_buff = 2;
	}
	else
	{
		my_chassis->info->speed_up_buff = 1;
	}
	
	if(abs(my_chassis->rp_pack->base_info.target.front_speed) != 0 || abs(my_chassis->rp_pack->base_info.target.right_speed) != 0)
	{	
		HERO.command->gimbal->Stop_Left_Hit = false;
		HERO.command->gimbal->Stop_Right_Hit = false;
	}
}

/**
  * @brief  底盘依据整车状态响应
  * @param  My_Chassis_t* my_chassis
  * @retval None
  */
void My_Chassis_Status_React(My_Chassis_t* my_chassis)
{
	switch(HERO.mode)
	{
		case Init_Mode:
			my_chassis->mode->Move_Mode = C_M_stop;
		  my_chassis->mode->Spin_Mode = C_S_stop;
			break;
		case Sleep_Mode:
			/*底盘停下后卸力*/
		  my_chassis->mode->Move_Mode = C_M_sleep;
		  my_chassis->mode->Spin_Mode = C_S_sleep;
			break;
		case Imu_Mode:
			my_chassis->mode->Move_Mode = C_M_special;
		  my_chassis->mode->Spin_Mode = C_S_follow;
		
		  /*要判断换头指令 begin*/
		  if(HERO.command->gimbal->Head_Swap == true)
			{
				my_chassis->mode->Spin_Mode = C_S_stop;
			}
			 /*要判断换头指令 end*/
			
			/*要判断小陀螺指令 begin 优先级高于换头指令*/
			if(HERO.command->chassis->Go_Top == true)
			{
				my_chassis->mode->Spin_Mode = C_S_top;
				HERO.command->gimbal->Stop_Left_Hit = false;
				HERO.command->gimbal->Stop_Right_Hit = false;
			}
		  /*要判断小陀螺指令 begin*/
		 
			break;
		case Mec_Mode:
			my_chassis->mode->Move_Mode = C_M_normal;
		  my_chassis->mode->Spin_Mode = C_S_normal;
			break;
		default:
			break;
	}
}

void My_Chassis_Data_Update(My_Chassis_t* my_chassis)
{
	/*遥控器数据更新*/
	My_Chassis_Input(my_chassis);
	
	/*整车速度更新*/
	My_Chassis_Speed_Update(my_chassis);
	
	/*裁判系统需要 begin*/
  /*裁判系统功率限制更新*/
	if(My_Judge.status->status == DEV_ONLINE)
	{
		My_Chassis.rp_pack->work_info.power_limit_buffer = &My_Judge.info->chassis_power_buffer;
	}
  /*end*/
}

/**
  * @brief  底盘控制
  * @param  My_Chassis_t* my_chassis
  * @retval None
  */
void My_Chassis_Control(My_Chassis_t* my_chassis)
{
	/*简洁代码变量 begin*/
	int16_t front_hub = 0, right_hub = 0, cycle_hub = 0;
	/*简洁代码变量 end*/
	
	/*来自云台的数据 begin*/
	int16_t yaw_delta = My_Gimbal.info->gimbal_to_chassis->yaw_delta;
	/*
	if(yaw_delta < 2048)
	{
		yaw_delta += 4096;
	}
	else if(yaw_delta > 2048)
	{
		
	}
	*/
	
	float yaw_angle_err_f = (double)yaw_delta / 4096.f * 3.14159;
	/*来自云台的数据 end*/
	
	/*平移控制 begin*/
	switch(my_chassis->mode->Move_Mode)
	{
		case C_M_sleep:
		  /*清空遥控通道值*/
		  memset(my_chassis->info->rc_input, 0, sizeof(*my_chassis->info->rc_input));
		
		  my_chassis->cnt->sleep_cnt++;
		  if(my_chassis->cnt->sleep_cnt == CHASSIS_SLEEP_RPOCESS_TIME)
			{
				memset(&can1_0x200_send_buff, 0, sizeof(can1_0x200_send_buff));
				my_chassis->cnt->sleep_cnt = CHASSIS_SLEEP_RPOCESS_TIME - 1;
			}
		  else if(my_chassis->info->speed_measure->front_speed + my_chassis->info->speed_measure->right_speed + \
				      my_chassis->info->speed_measure->cycle_speed <= CHASSIS_SLEEP_ALLOW_SPEED \
			        && my_chassis->cnt->sleep_cnt < CHASSIS_SLEEP_RPOCESS_TIME)
			{
				memset(&can1_0x200_send_buff, 0, sizeof(can1_0x200_send_buff));
			}
			else
			{
				/*停下后卸力*/
		    front_hub = 0;
		    right_hub = 0;
		    cycle_hub = 0;
				my_chassis->rp_pack->work(my_chassis->rp_pack);
			}
			break;
		case C_M_stop:
			front_hub = 0;
		  right_hub = 0;
			break;
		case C_M_normal:
			switch(HERO.ctrl)
			{
				case RC_CTRL:
					front_hub = (int16_t)(my_chassis->info->rc_input->ch3_process * (int16_t)(CHASSIS_MAX_SPEED / 660));
				  right_hub = (int16_t)(my_chassis->info->rc_input->ch2_process * (int16_t)(CHASSIS_MAX_SPEED / 660));
				
					break;
				case KEY_CTRL:
					front_hub += (int16_t)((float)rc.base_info->W.cnt / (float)KEY_W_CNT_MAX * (float)CHASSIS_MAX_SPEED);
				  front_hub -= (int16_t)((float)rc.base_info->S.cnt / (float)KEY_S_CNT_MAX * (float)CHASSIS_MAX_SPEED);
				  right_hub += (int16_t)((float)rc.base_info->D.cnt / (float)KEY_D_CNT_MAX * (float)CHASSIS_MAX_SPEED);
				  right_hub -= (int16_t)((float)rc.base_info->A.cnt / (float)KEY_A_CNT_MAX * (float)CHASSIS_MAX_SPEED);
				  front_hub *= my_chassis->info->speed_up_buff;
				  right_hub *= my_chassis->info->speed_up_buff;
					break;
				default:
					break;
			}
			break;
		case C_M_special:
			switch(HERO.ctrl)
			{
				case RC_CTRL:
					front_hub = rc.base_info->ch3 * cos(yaw_angle_err_f) - rc.base_info->ch2 * sin(yaw_angle_err_f);
					right_hub = rc.base_info->ch2 * cos(yaw_angle_err_f) + rc.base_info->ch3 * sin(yaw_angle_err_f);
					front_hub *= CHASSIS_MAX_SPEED/ 660.f * My_Gimbal.info->gimbal_to_chassis->Back_Control;
					right_hub *= CHASSIS_MAX_SPEED / 660.f * My_Gimbal.info->gimbal_to_chassis->Back_Control;
					break;
				case KEY_CTRL:
					front_hub += (float)rc.base_info->W.cnt / (float)KEY_W_CNT_MAX * (float)CHASSIS_MAX_SPEED;
					front_hub -= (float)rc.base_info->S.cnt / (float)KEY_S_CNT_MAX * (float)CHASSIS_MAX_SPEED;
					right_hub += (float)rc.base_info->D.cnt / (float)KEY_D_CNT_MAX * (float)CHASSIS_MAX_SPEED;
					right_hub -= (float)rc.base_info->A.cnt / (float)KEY_A_CNT_MAX * (float)CHASSIS_MAX_SPEED;
				
				  front_hub = (front_hub * cos(yaw_angle_err_f) - right_hub * sin(yaw_angle_err_f)) * My_Gimbal.info->gimbal_to_chassis->Back_Control * my_chassis->info->speed_up_buff;
					right_hub = (right_hub * cos(yaw_angle_err_f) + front_hub * sin(yaw_angle_err_f)) * My_Gimbal.info->gimbal_to_chassis->Back_Control * my_chassis->info->speed_up_buff;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	/*平移控制 end*/
	
	/*旋转控制 begin*/
	switch(my_chassis->mode->Spin_Mode)
	{
		case C_S_sleep:
			HERO.command->chassis->Go_Top = false;
			/*停下后卸力*/
			break;
		case C_S_stop:
			cycle_hub = 0;
			break;
		case C_S_normal:
			switch(HERO.ctrl)
			{
				case RC_CTRL:
					cycle_hub = (rc.base_info->ch0 * (CHASSIS_MAX_SPEED / 660.f));
					break;
				case KEY_CTRL:
					cycle_hub = rc.info->mouse_x_K;
					break;
				default:
					break;
			}
			break;
		case C_S_follow:
			//折线
			if(abs(yaw_delta) >= 100)
			{
				cycle_hub = yaw_delta * 6.f - 300.f * sgn(yaw_delta);
			}
			else 
			{
				cycle_hub = yaw_delta * 3.f;
			}
			break;
		case C_S_top:
			cycle_hub = CHASSIS_TOP_SPEED * my_chassis->info->speed_up_buff;
			break;
		default:
			break;
	}
	/*旋转控制 end*/
	
	/*底盘输出 begin*/
	my_chassis->rp_pack->base_info.target.front_speed = front_hub / 2;
	my_chassis->rp_pack->base_info.target.right_speed = right_hub / 2;
	my_chassis->rp_pack->base_info.target.cycle_speed = cycle_hub / 10;
	
//	My_Chassis_Speed_Limit(my_chassis);
	
	//睡眠模式独立输出
	if(HERO.mode != Sleep_Mode)
	{
		my_chassis->rp_pack->work(my_chassis->rp_pack);
	}
	/*底盘输出 end*/
}


void My_Chassis_Work(void)
{
	My_Chassis_Command_React(&My_Chassis);
	
	My_Chassis_Status_React(&My_Chassis);
	
	/*Data before Control!!*/
	My_Chassis_Data_Update(&My_Chassis);
	
	My_Chassis_Control(&My_Chassis);
}



/*...............................................处理函数...............................................*/

/**
  * @brief  底盘(遥控器)输入处理
  * @param  My_Chassis_t* my_chassis
  * @retval None
  */
void My_Chassis_Input(My_Chassis_t* my_chassis)
{
	Chassis_RC_Input_t *rc_input = my_chassis->info->rc_input;
	
	rc_input->ch3_now = rc.base_info->ch3;
	if(rc_input->ch3_now != 0)
	{
		if(abs(rc_input->ch3_now - rc_input->ch3_last) > 30)
		{
			rc_input->ch3_now += 30 * sgn(rc_input->ch3_now - rc_input->ch3_last);
			rc_input->ch3_last = rc_input->ch3_now;
		}
		else
		{
			rc_input->ch3_last = rc_input->ch3_now;
		}
	}
	else
	{
		if(abs(rc_input->ch3_now - rc_input->ch3_last) > 40)
		{
  		rc_input->ch3_now += 60 * sgn(rc_input->ch3_now - rc_input->ch3_last);
		}
		else
		{
			rc_input->ch3_now = 0;
			rc_input->ch3_last = rc_input->ch3_now;
		}
	}
	rc_input->ch2_now = rc.base_info->ch2;
	if(rc_input->ch2_now != 0)
	{
		if(abs(rc_input->ch2_now - rc_input->ch2_last) > 30)
		{
		  rc_input->ch2_now += 30 * sgn(rc_input->ch2_now - rc_input->ch2_last);
			rc_input->ch2_last = rc_input->ch2_now;
		}
		else
		{
			rc_input->ch2_last = rc_input->ch2_now;
		}
	}
	else
	{
		if(abs(rc_input->ch2_now - rc_input->ch2_last) > 40)
		{
			rc_input->ch2_now += 60 * sgn(rc_input->ch2_now - rc_input->ch2_last);
		}
		else
		{
			rc_input->ch2_now = 0;
			rc_input->ch2_last = rc_input->ch2_now;
		}
	}
	
	rc_input->ch3_process = rc_input->ch3_now;
	rc_input->ch2_process = rc_input->ch2_now;
}

/**
  * @brief  底盘运动速度更新,用于在底盘包中的直线校正
  * @param  My_Chassis_t* my_chassis
  * @retval None
  */
void My_Chassis_Speed_Update(My_Chassis_t* my_chassis)
{
	int16_t motor_speed_LF = my_chassis->rp_pack->motor_LF->rx_info.speed;
	int16_t motor_speed_RF = my_chassis->rp_pack->motor_RF->rx_info.speed;
	int16_t motor_speed_LB = my_chassis->rp_pack->motor_LB->rx_info.speed;
	int16_t motor_speed_RB = my_chassis->rp_pack->motor_RB->rx_info.speed;
	
	my_chassis->info->speed_measure->front_speed = (motor_speed_LF - motor_speed_RF + motor_speed_LB - motor_speed_RB) / 4;
	my_chassis->info->speed_measure->right_speed = (motor_speed_LF + motor_speed_RF - motor_speed_LB - motor_speed_RB) / 4;
	my_chassis->info->speed_measure->cycle_speed = (motor_speed_LF + motor_speed_RF + motor_speed_LB + motor_speed_RB) / 4; 
}

/**
  * @brief  底盘电机速度限制
  * @param  My_Chassis_t* my_chassis
  * @retval None
  */
void My_Chassis_Speed_Limit(My_Chassis_t* my_chassis)
{
	int16_t speed_sum;
	float K;
	
	speed_sum = abs(my_chassis->rp_pack->base_info.target.front_speed) \
            + abs(my_chassis->rp_pack->base_info.target.right_speed) \
            + abs(my_chassis->rp_pack->base_info.target.cycle_speed);
	
	if(speed_sum > 5000 && HERO.command->chassis->Accelerate != true)
	{
		K = (float)5000.f / (float)speed_sum;
	}
	else 
	{
		K = 1;
	}
	my_chassis->rp_pack->base_info.target.front_speed = my_chassis->rp_pack->base_info.target.front_speed * K;
	my_chassis->rp_pack->base_info.target.right_speed = my_chassis->rp_pack->base_info.target.right_speed * K;
	my_chassis->rp_pack->base_info.target.cycle_speed = my_chassis->rp_pack->base_info.target.cycle_speed * K;
}

