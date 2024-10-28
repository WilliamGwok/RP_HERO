/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2022, hwx, China, SZU.
  *                            N0   Rights  Reserved
  *                              
  *                   
  * @FileName   : rp_chassis.c   
  * @Version    : v1.1		
  * @Author     : hwx			
  * @Date       : 2022-11-06         
  * @Description:    
  *
  *
  ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "rp_chassis.h"
#include "drv_can.h"
#include "config_can.h"
#include "my_chassis.h"
#include "math_support.h"
/* Exported variables --------------------------------------------------------*/

chassis_t Chassis = {
	
	.init      = Chassis_Init,
	.work      = Chassis_Work,
	
	.config_rotation_ratio  = Chassis_Config_rotation_ratio,
	.config_power_limit     = Chassis_Config_power_limit,
	.config_top_calc        = Chassis_config_top_calc,

};




/* Function  body --------------------------------------------------------*/
/**
  * @Name    Chassic_init
  * @brief   注册底盘
  * @param   底盘类 底盘初始化结构体
  * @retval
  * @author  HWX
  * @Date    2022-11-06
**/

void Chassis_Init(chassis_t *Chassis , Chassis_InitTypeDef *Chassis_Init_structure)
{
	
	
	if(  Chassis_Init_structure->motor_LF           == NULL ||\
		 Chassis_Init_structure->motor_RF           == NULL ||\
		 Chassis_Init_structure->motor_LB           == NULL ||\
		 Chassis_Init_structure->motor_RB           == NULL )
	{
		return;
	}
	
	if( Chassis_Init_structure->power_limit_buffer  == NULL)
    {
		Chassis_Config_power_limit(CHASSIS_POWER_LIMIT_OFF);
    }
 	
	/*注册四个电机*/
	Chassis->motor_LB = Chassis_Init_structure->motor_LB;
	Chassis->motor_LF = Chassis_Init_structure->motor_LF;
	Chassis->motor_RF = Chassis_Init_structure->motor_RF;
	Chassis->motor_RB = Chassis_Init_structure->motor_RB;
	
	/*裁判系统功率缓冲区*/
//	Chassis->work_info.power_limit_buffer = Chassis_Init_structure->power_limit_buffer;
	
	
	Chassis->work_info.output_max              = CHASSIS_SPEED_MAX;
	Chassis->work_info.speed_max               = CHASSIS_OUTPUT_MAX;
	Chassis->work_info.config.power_limit      = CHASSIS_POWER_LIMIT_OFF;
	Chassis->work_info.config.rotation_ratio   = CHASSIS_CYCLE_DIV_FRONT;

}

/**
  * @Name    Chassis_Power_Limit
  * @brief   底盘功率限制(经典祖传算法)
  * @param   底盘 
  * @retval
  * @author  HWX
  * @Date    2022-11-06
**/
void Chassis_Power_Limit(chassis_t * chassis)
{
	if(CHASSIS_POWER_LIMIT_ON)
	{
		int16_t limit_output_current[4];
	
		float buffer = *Chassis.work_info.power_limit_buffer;
		float heat_rate,Limit_k, CHAS_LimitOutput, CHAS_TotalOutput;
		
		limit_output_current[0] = chassis->base_info.output.motor_LF_current;
		limit_output_current[1] = chassis->base_info.output.motor_RF_current;
		limit_output_current[2] = chassis->base_info.output.motor_LB_current;
		limit_output_current[3] = chassis->base_info.output.motor_RB_current;
		
		uint16_t OUT_MAX = 0;
	
		OUT_MAX = CHASSIS_SPEED_MAX * 4;
		
		if(buffer > 60)buffer = 60;//防止飞坡之后缓冲250J变为正增益系数
		
		Limit_k = buffer / 60;
		
		if(buffer < 25)
			Limit_k = Limit_k * Limit_k ;// * Limit_k; //3方
		else
			Limit_k = Limit_k;// * str->Limit_k; //平方
		
		if(buffer < 60)
			CHAS_LimitOutput = Limit_k * OUT_MAX;
		else 
			CHAS_LimitOutput = OUT_MAX;    
		
		CHAS_TotalOutput = c_abs(limit_output_current[0]) + c_abs(limit_output_current[1]) + c_abs(limit_output_current[2]) + c_abs(limit_output_current[3]) ;
		
		heat_rate = CHAS_LimitOutput / CHAS_TotalOutput;
		
	  if(CHAS_TotalOutput >= CHAS_LimitOutput)
	  {
			for(char i = 0 ; i < 4 ; i++)
			{	
				limit_output_current[i] = (int16_t)(limit_output_current[i] * heat_rate);	
			}
		}
		/*重新赋值*/
		chassis->base_info.output.motor_LF_current = limit_output_current[0];
		chassis->base_info.output.motor_RF_current = limit_output_current[1];
		chassis->base_info.output.motor_LB_current = limit_output_current[2];
		chassis->base_info.output.motor_RB_current = limit_output_current[3];
			
	}

}


/**
  * @Name    Chassis_Config_power_limit
  * @brief   开关底盘功率限制（进入自杀模式）
  * @param   off_or_on
  * @retval
  * @author  HWX
  * @Date    2022-11-06
**/
void Chassis_Config_power_limit(uint8_t off_or_on)
{
	if(off_or_on == CHASSIS_POWER_LIMIT_OFF || off_or_on == CHASSIS_POWER_LIMIT_ON)
	{
		Chassis.work_info.config.power_limit = off_or_on;
	}

}

/**
  * @Name    Chassis_Config_cycle_div_front
  * @brief   修改旋转量比重
  * @param   cycle_div_front ( 0 ~ 100)
  * @retval
  * @author  HWX
  * @Date    2022-11-06
**/
void Chassis_Config_rotation_ratio(uint8_t rotation_ratio)
{
//	if(rotation_ratio >= 0 || rotation_ratio <= 100)
//	{
//		Chassis.work_info.config.rotation_ratio = rotation_ratio;
//	}
}

/**
  * @Name    Chassis_Config_cycle_div_front
  * @brief   修改旋转量比重
  * @param   cycle_div_front ( 0 ~ 100)
  * @retval
  * @author  HWX
  * @Date    2022-11-06
**/
void Chassis_config_top_calc(uint8_t off_or_on)
{
	if(off_or_on == CHASSIS_TOP_CALC_OFF || off_or_on == CHASSIS_TOP_CALC_ON)
	{
		Chassis.work_info.config.top_cacl = off_or_on;
	}

}

/**
  * @Name    Chassis_Speed_Limit
  * @brief   速度限制（等比例缩小）
  * @param   底盘 三个方向速度
  * @retval
  * @author  HWX
  * @Date    2022-11-06
**/
void Chassis_Speed_Limit(chassis_t *chassis)
{
	int16_t speed_sum;
	float RATE;
	speed_sum = c_abs(chassis->base_info.target.front_speed) + \
	            c_abs(chassis->base_info.target.right_speed) + \
                c_abs(chassis->base_info.target.cycle_speed);
	
	if(speed_sum > chassis->work_info.speed_max)
	{
		RATE = (float)chassis->work_info.speed_max / (float)speed_sum;
	}
	else 
	{
		RATE = 1;
	}
	
	chassis->base_info.target.front_speed = (float)chassis->base_info.target.front_speed * RATE;
	chassis->base_info.target.right_speed = (float)chassis->base_info.target.right_speed * RATE;
	chassis->base_info.target.cycle_speed = (float)chassis->base_info.target.cycle_speed * RATE;
}

#define RADIAN_COEF 57.3f //弧度系数
#define WHEELBASE 432.f//车体轴距 单位mm
#define WHEELTRACK 329.f//车体轮距 单位mm
#define PERIMETER 150.f //轮子直径 单位mm
#define CHASSIS_DECELE_RATIO 19.f //电机减速比


float rotate_ratio_f = 6.63f;//6.63
float wheel_rpm_ratio = 2.42f;//2.42


/**
  * @Name    Chassis_Speed_Calculating
  * @brief   底盘速度解算（速度分解）
  * @param   三个方向速度
  * @retval
  * @author  GJW
  * @Date    2023-7-25
**/
void Chassis_Speed_Calculating(chassis_t *chassis, int16_t front, int16_t right, int16_t cycle)
{
	chassis->base_info.output.motor_LF_speed   = (  front + right + cycle * rotate_ratio_f) * wheel_rpm_ratio;
	chassis->base_info.output.motor_RF_speed   = (- front + right + cycle * rotate_ratio_f) * wheel_rpm_ratio;
	chassis->base_info.output.motor_LB_speed   = (  front - right + cycle * rotate_ratio_f) * wheel_rpm_ratio;
	chassis->base_info.output.motor_RB_speed   = (- front - right + cycle * rotate_ratio_f) * wheel_rpm_ratio;
}





/**
  * @Name    Chassis_Top_Speed_Calculating
  * @brief   底盘小陀螺速度解算(将这个函数放在Work前)
  * @param   底盘 三个方向速度 差角（请换算成弧度制，并且注意数据类型是float）
  * @retval 
  * @author  HWX
  * @Date    2022-11-06
**/
void Chassis_Top_Speed_Calculating(chassis_t *chassis)
{
	if(chassis->work_info.config.top_cacl)
	{
	  float detal_angle = chassis->base_info.measure.top_detal_angle;
	  
	  float sin_x = (float)sin((double)(detal_angle));
	  float cos_x = (float)cos((double)(detal_angle));
	  int16_t tem_front =  (chassis->base_info.target.front_speed);
	  int16_t tem_right =  (chassis->base_info.target.right_speed);
	
	 /*警钟长鸣！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！*/
	 chassis->base_info.target.front_speed =  (int16_t)(cos_x * tem_front)-(sin_x * tem_right);
	 chassis->base_info.target.right_speed =  (int16_t)(sin_x * tem_front)+(cos_x * tem_right);
	}
}

/**
  * @Name    Chassis_Pid_Calculating
  * @brief   使用到底盘包
  * @param   底盘 
  * @retval 
  * @author  HWX
  * @Date    2022-11-07
**/
void Chassis_Pid_Calculating(chassis_t *chassis)
{
	int16_t LF_fil = 0,RF_fil = 0,LB_fil = 0,RB_fil = 0;
	
	LF_fil = chassis->motor_LF->rx_info.speed;
	RF_fil = chassis->motor_RF->rx_info.speed;
	LB_fil = chassis->motor_LB->rx_info.speed;
	RB_fil = chassis->motor_RB->rx_info.speed;
	
	chassis->base_info.output.motor_LF_current = chassis->motor_LF->c_pid1(&chassis->motor_LF->pid.speed,LF_fil,chassis->base_info.output.motor_LF_speed);
	chassis->base_info.output.motor_RF_current = chassis->motor_RF->c_pid1(&chassis->motor_RF->pid.speed,RF_fil,chassis->base_info.output.motor_RF_speed);
	chassis->base_info.output.motor_LB_current = chassis->motor_LB->c_pid1(&chassis->motor_LB->pid.speed,LB_fil,chassis->base_info.output.motor_LB_speed);
	chassis->base_info.output.motor_RB_current = chassis->motor_RB->c_pid1(&chassis->motor_RB->pid.speed,RB_fil,chassis->base_info.output.motor_RB_speed);

	/* 直线校正begin */
	int16_t front_err, right_err, cycle_err;
	front_err = sgn(chassis->base_info.target.front_speed - My_Chassis.info->speed_measure->front_speed);
	right_err = sgn(chassis->base_info.target.right_speed - My_Chassis.info->speed_measure->right_speed);
	cycle_err = sgn(chassis->base_info.target.cycle_speed - My_Chassis.info->speed_measure->cycle_speed);
	chassis->base_info.output.motor_LF_current += abs(chassis->base_info.output.motor_LF_current) * (float)( front_err + right_err + cycle_err) * CHASSIS_STRAIGHT_RECTIFY;
	chassis->base_info.output.motor_RF_current += abs(chassis->base_info.output.motor_RF_current) * (float)(-front_err + right_err + cycle_err) * CHASSIS_STRAIGHT_RECTIFY;
	chassis->base_info.output.motor_LB_current += abs(chassis->base_info.output.motor_LB_current) * (float)( front_err - right_err + cycle_err) * CHASSIS_STRAIGHT_RECTIFY;
	chassis->base_info.output.motor_RB_current += abs(chassis->base_info.output.motor_RB_current) * (float)(-front_err - right_err + cycle_err) * CHASSIS_STRAIGHT_RECTIFY;
	/* 直线校正end */
}

/**
  * @Name    Chassis_Work
  * @brief   底盘核心函数
  * @param   底盘 三个方向速度
  * @retval
  * @author  HWX
  * @Date    2022-11-06
**/
void Chassis_Work(chassis_t *chassis)
{
//	int16_t output_current[4];
	
	int16_t front = chassis->base_info.target.front_speed;
	int16_t right = chassis->base_info.target.right_speed;
	int16_t cycle = chassis->base_info.target.cycle_speed;
	
	
	/*底盘速度解算*/
	Chassis_Speed_Calculating(chassis, front, right, cycle);
	
	/*旋转量算法*/
	
	
	/*小陀螺底盘解算*/
//	Chassis_Top_Speed_Calculating(chassis);
	
	/*底盘速度限制*/
	Chassis_Speed_Limit(chassis);
	
	/*PID计算*/
	Chassis_Pid_Calculating(chassis);

	
	/*裁判系统速度限制*/
	Chassis_Power_Limit(chassis);

	can1_0x200_send_buff[0] = chassis->base_info.output.motor_LF_current;
	can1_0x200_send_buff[1] = chassis->base_info.output.motor_RF_current;
	can1_0x200_send_buff[2] = chassis->base_info.output.motor_LB_current;
	can1_0x200_send_buff[3] = chassis->base_info.output.motor_RB_current;	
}



