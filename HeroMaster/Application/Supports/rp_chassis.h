/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2022, hwx, China, SZU.
  *                            N0   Rights  Reserved
  *                              
  *                   
  * @FileName   : rp_chassis.h   
  * @Version    : v1.2		
  * @Author     : hwx			
  * @Date       : 2022-11-08         
  * @Description: 基本的底盘包，后续有待增加 1.直线修正 2.旋转量占比限制
  *
  *               目前无用变量：底盘状态            -- chassis_work_sate
  *                             旋转量占比         -- chassis_cycle_div_front
  *								底盘最大输出       -- chassis_output_max
  *
  *               目前无用函数：修改旋转量占比函数  -- Chassis_Config_cycle_div_front
  *					
  *				  有待更改的部分：最终发送电流的方式还要去到can的驱动层的函数，希望可以直接用电机包的函数
  *                               
  *
  ******************************************************************************
 */
  /*版本更新说明
  * V1.1 解决了函数名称错误导致的编译报错,有待解决由于驱动不同导致的can发送问题
  * V1.2 解决了一系列问题
  * */
/*******************************************************************************
*                               底盘包使用教程
* @Tips   : 在使用底盘包之前请使用电机包注册好四个底盘电机
* @Tips   : 事先定义好了底盘包的结构体 “Chassis”，但需要用户完成初始化
* @Tips   : 定义一个Chassis_InitTypeDef结构体，并完成结构体成员赋值
* @Tips   : 然后调用函数 Chassic_init 或 使用结构体成员.init 完成底盘注册
* @Tips   : 接下来就可以直接调用 Chassis_Work 或 使用结构体成员.work 让底盘工作
* @Tips   : Congig函数可以在中途调用，修改底盘工作配置
* @Tips   : 
*
*******************************************************************************/

/*******************************************************************************
*                               底盘包使用注意事项
* @Tips   : 需要传入注册好的电机（主要使用到PID与）
* @Tips   : 注意修改宏定义 “CHASSIS_DRV_CAN_USE” 选择与电机通讯所用的外设
* @Tips   : 注意修改宏定义 “CHASSIS_CAN_STD_ID”  修改CAN发送的报文ID
* @Tips   : 记得将裁判系统内容中的底盘功率地址传入初始化结构体
* @Tips   : 若初始化将功率buffer传入，底盘功率限制是默认开启的，
* @Tips   : 若想关闭请调用Chassis_Config_power_limit(POWER_LIMIT_OFF)
*
*******************************************************************************/

#ifndef __RP_CHASSIS_H_
#define __RP_CHASSIS_H_


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "motor.h"
#include "math.h"
#include "drv_can.h"

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Exported variables ---------------------------------------------------------*/
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;


/* Exported macro ------------------------------------------------------------*/
#define CHASSIS_DRV_CAN_USE     (hcan1)  /*hcan1  hcan2*/
#define CHASSIS_CAN_STD_ID      (0x200)


#define CHASSIS_SPEED_MAX  		(8000)	/*底盘最大速度默认值*/
#define CHASSIS_OUTPUT_MAX 		(12000) /*底盘最大输出默认值*/
#define CHASSIS_CYCLE_DIV_FRONT (50)    /*旋转百分比默认值*/


#define CHASSIS_POWER_LIMIT_OFF (0)
#define CHASSIS_POWER_LIMIT_ON  (1)

#define CHASSIS_TOP_CALC_OFF (0)
#define CHASSIS_TOP_CALC_ON  (1)


#define c_abs(x) 					((x)>0? (x):(-(x)))
/* Exported types ------------------------------------------------------------*/


/** 
  * @brief  底盘初始化结构体定义 
  */ 
typedef struct
{
  motor_t *motor_LF;       		 /*!< 左前轮电机初始化
									@ref 在rp_motor_pack中初始化 */

  motor_t *motor_RF;      		 /*!< 右前轮电机初始化
									@ref 在rp_motor_pack中初始化 */

  motor_t *motor_LB;       		 /*!< 左后轮电机初始化
									@ref 在rp_motor_pack中初始化 */

  motor_t *motor_RB;      		 /*!< 右后轮电机初始化
									@ref 在rp_motor_pack中初始化 */

  float   *power_limit_buffer;   /*!< 裁判系统底盘功率限制的buffer，需要该变量地址
									@ref 由裁判系统发回 */

}Chassis_InitTypeDef;

/** 
  * @brief  底盘状态枚举
  */ 
typedef enum 
{
	CHASSIC_OFFLINE = 0,	
	
	CHASSIC_ONLINE,

}chassic_work_state_e;


/** 
  * @brief  底盘基本信息->目标信息
  */ 
typedef __packed struct 
{
	int16_t front_speed;  /*!< @ref 目标前进速度 */
	int16_t right_speed;  /*!< @ref 目标左移速度 */
	int16_t cycle_speed;  /*!< @ref 目标旋转速度 */	
	
}chassis_target_t;

/** 
  * @brief  底盘基本信息->测量信息
  */ 
typedef __packed struct 
{
	int16_t front_speed; /*!< @ref 当前前进速度 */
	int16_t right_speed; /*!< @ref 当前左移速度 */
	int16_t cycle_speed; /*!< @ref 当前旋转速度 */
	
	float   top_detal_angle;  /*!< @ref 小陀螺解算角度 */
	
}chassis_measure_t;


/** 
  * @brief  底盘基本信息->输出信息
  */ 
typedef __packed struct 
{
	int16_t motor_LF_speed; /*!< @ref 左前输出速度 */
	int16_t motor_RF_speed; /*!< @ref 右前输出速度 */
	int16_t motor_LB_speed; /*!< @ref 左后输出速度 */
	int16_t motor_RB_speed; /*!< @ref 右后输出速度 */
	
	int16_t motor_LF_current; /*!< @ref 左前输出电流 */
	int16_t motor_RF_current; /*!< @ref 右前输出电流 */
	int16_t motor_LB_current; /*!< @ref 左后输出电流 */
	int16_t motor_RB_current; /*!< @ref 右后输出电流 */
	
}chassis_output_t;


/** 
  * @brief  底盘基本信息定义
  */ 
typedef __packed struct 
{
	
	chassis_target_t     target;
	chassis_measure_t    measure;
	chassis_output_t     output;

}chassis_base_info_t;



/** 
  * @brief  底盘工作状态信息定义
  */ 
typedef struct
{
	
	uint8_t              rotation_ratio;    /*!< @ref 底盘旋转占比 (0 ~ 100) */
	uint8_t              power_limit; /*!< @ref 裁判系统功率限制状态(ON/OFF)*/
	uint8_t              top_cacl;    /*!< @ref 底盘小陀螺解算状态 (ON/OFF) */
		
}chassis_config_t;

/** 
  * @brief  底盘工作状态信息定义
  */ 
typedef struct
{
	
	chassis_config_t     config; 
	
	uint16_t 			 output_max; 		/*!< @ref 底盘最大输出 */
	uint16_t 			 speed_max;  		/*!< @ref 底盘最高速度 */
	chassic_work_state_e work_sate;  		/*!< @ref 底盘工作状态 */
	
	int16_t               *power_limit_buffer;
		
	
}chassis_work_info_t;


/** 
  * @brief  底盘类定义
  */ 
typedef struct chassis_class_t
{	
	/*外部电机*/
	motor_t        		 *motor_LF;
	motor_t        		 *motor_RF;
	motor_t       		 *motor_LB;
	motor_t        		 *motor_RB;
	
	
	chassis_base_info_t   base_info;
	chassis_work_info_t   work_info;
	
	void                (*init)(struct chassis_class_t *chassis,Chassis_InitTypeDef *Chassis_Init_structure);
	void                (*work)(struct chassis_class_t *chassis);
	
	void                (*config_rotation_ratio)(uint8_t rotation_ratio);	
	void                (*config_power_limit)(uint8_t off_or_on);
	void                (*config_top_calc)(uint8_t off_or_on);

}chassis_t;





/* Exported functions --------------------------------------------------------*/

/*init*/
void Chassis_Init(chassis_t * chassis , Chassis_InitTypeDef *Chassis_Init_structure);

/*work*/
void Chassis_Work(chassis_t *chassis);

/*top_calc*/
void Chassis_Top_Speed_Calculating(chassis_t *chassis);

/*config_cycle_div_front*/
void Chassis_Config_rotation_ratio(uint8_t rotation_ratio);

/*config_power_limit*/
void Chassis_Config_power_limit(uint8_t off_or_on);

/*config_power_limit*/
void Chassis_Config_power_limit(uint8_t off_or_on);

/*config_top_calc*/
void Chassis_config_top_calc(uint8_t off_or_on);



/*底盘速度分解*/
void Chassis_Speed_Calculating(chassis_t *chassis, int16_t front, int16_t right, int16_t cycle);

/*底盘功率限制*/
void Chassis_Power_Limit(chassis_t * chassis);

/*底盘电机PID计算*/
void Chassis_Pid_Calculating(chassis_t *chassis );


//直线修正
//
#endif


 

