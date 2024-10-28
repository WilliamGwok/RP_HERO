/**
  ******************************************************************************
  * @file           : remote.c/h
  * @brief          : 
  * @note           : 更新时间：2022年1月11日17:24:36
	*                   鼠标速度滤波说明：
	*                   在更新中断对鼠标速度初始数据进行均值滤波
	*                   在控制任务中对鼠标速度进行低通滤波
  ******************************************************************************
  */

#ifndef __REMOTE_H
#define __REMOTE_H

#include "stm32f4xx_hal.h"
#include "config_remote.h"
#include "stdbool.h"

/* 按键状态枚举 */
typedef enum
{
  relax_K,        //放松
  down_K,         //按下
  up_K,           //抬起
  short_press_K,  //短按
  long_press_K,   //长按
}key_board_status_e;

/* 遥控器拨杆旋钮状态枚举 */
typedef enum 
{
  keep_R,         //保持
  up_R,           //向上拨
  mid_R,          //向中拨
  down_R,         //向下拨
}remote_status_e;

/* 按键信息 */
typedef struct
{
  uint8_t value;    //值
  uint8_t status;   //状态
  int16_t cnt;      //当前计数
  int16_t cnt_max;  //计数上限
}key_board_info_t;

/* 拨杆信息 */
typedef struct
{
  uint8_t value_last;  //上一次值
  uint8_t value;       //新值
  uint8_t status;      //状态
}remote_switch_info_t;

/* 旋钮信息 */
typedef struct
{
  int16_t value_last;  //上一次值
  int16_t value;       //新值
  uint8_t status;      //状态
}remote_wheel_info_t;

/* 遥控原始信息 */
typedef struct 
{
  /* 遥控器 */
  int16_t                 ch0;                  //右的左右
  int16_t                 ch1;                  //右的前后
  int16_t                 ch2;                  //左的左右
  int16_t                 ch3;                  //左的前后
  remote_switch_info_t    s1;                   //左拨杆
  remote_switch_info_t    s2;                   //右拨杆
  remote_wheel_info_t     thumbwheel;           //左旋扭
  /* 键盘 */
  int16_t                 mouse_vx;             //鼠标x轴速度
  int16_t                 mouse_vy;             //鼠标y轴速度
  int16_t                 mouse_vz;             //鼠标z轴速度
  key_board_info_t        mouse_btn_l;          //鼠标左键
  key_board_info_t        mouse_btn_r;          //鼠标右键
  key_board_info_t        Q;                    //按键Q
  key_board_info_t        W;                    //按键W
  key_board_info_t        E;                    //按键E
  key_board_info_t        R;                    //按键R
  key_board_info_t        A;                    //按键A
  key_board_info_t        S;                    //按键S
  key_board_info_t        D;                    //按键D
  key_board_info_t        F;                    //按键F
  key_board_info_t        G;                    //按键G
  key_board_info_t        Z;                    //按键Z
  key_board_info_t        X;                    //按键X
  key_board_info_t        C;                    //按键C
  key_board_info_t        V;                    //按键V
  key_board_info_t        B;                    //按键B
  key_board_info_t        Shift;                //按键Shift
  key_board_info_t        Ctrl;                 //按键Ctrl
}rc_base_info_t;

/* 遥控信息 */
typedef struct 
{
  int16_t             offline_cnt;  //失联计数
	uint8_t             status;       //状态
	float					  		mouse_x;      //鼠标x轴速度
	float  							mouse_y;      //鼠标y轴速度
	float               mouse_x_K;    //鼠标x轴滤波后速度
	float  							mouse_y_K;    //鼠标y轴滤波后速度
}rc_info_t;

/* 遥控 */
typedef struct
{
  rc_base_info_t     *base_info;
  rc_info_t          *info;
}rc_t;

/* 外部变量 */
extern rc_t rc;

/* 初始化 */
void rc_init(rc_t *rc);
void rc_base_info_init(rc_base_info_t *info);
void key_board_cnt_max_set(rc_base_info_t *info);
void rc_info_init(rc_info_t *info);
void rc_interrupt_init(rc_base_info_t *info);

/* 中断 */
void rc_interrupt_update(rc_t *rc);

/* 滴答任务 */
void rc_tick_task(rc_t *rc);

/* 控制任务 */
void rc_ctrl(rc_t *rc);

/* 状态更新 */
void key_board_status_update(key_board_info_t *key);
void all_key_board_status_update(rc_base_info_t *info);

/* 中断状态更新 */
void rc_switch_status_interrupt_update(rc_base_info_t *info);
void rc_wheel_status_interrupt_update(rc_base_info_t *info);
void key_board_status_interrupt_update(key_board_info_t *key);
void all_key_board_status_interrupt_update(rc_base_info_t *info);
void remote_soft_reset_check(rc_t *rc);
bool Remote_Start_Safely(rc_t *rc);

#endif
