/**
  ******************************************************************************
  * @file           : remote.c/h
  * @brief          : 
  * @note           : 更新时间：2022年1月11日17:24:50
	*                   串口中断->更新数据、中断标志位置true
  *                   控制任务->有中断时->清除中断标志位、检查跳变
  *                   控制任务->无中断时->迭代键盘按键信息
  ******************************************************************************
  */

#include "remote.h"
#include "string.h"
#include "stdbool.h"
#include "config.h"
#include "math_support.h"

bool rc_interrupt = false;                  //遥控器更新中断

rc_base_info_t rc_base_info;
rc_info_t rc_info;
rc_t rc = 
{
  .base_info = &rc_base_info,
  .info = &rc_info,
};

/**
  * @brief  遥控器初始化
  */
void rc_init(rc_t *rc)
{
	/* 基本信息初始化 */
  rc_base_info_init(rc->base_info);
	/* 信息初始化 */
	rc_info_init(rc->info);
  /* 中断标志清除 */
  rc_interrupt_init(rc->base_info);
}

/**
  * @brief  基本信息初始化
  */
void rc_base_info_init(rc_base_info_t *info)
{
	/* 基本信息置零 */
  memset(info,0,sizeof(rc_base_info_t));
	/* 拨杆旋钮状态初始化 */
  info->s1.status = keep_R;
  info->s2.status = keep_R;
  info->thumbwheel.status = keep_R;
	/* 设置长按时间 */
  key_board_cnt_max_set(info);
}

/**
  * @brief  按键长按时间设置
  */
void key_board_cnt_max_set(rc_base_info_t *info)
{
  info->mouse_btn_l.cnt_max = MOUSE_BTN_L_CNT_MAX;
  info->mouse_btn_r.cnt_max = MOUSE_BTN_R_CNT_MAX;
  info->Q.cnt_max = KEY_Q_CNT_MAX;
  info->W.cnt_max = KEY_W_CNT_MAX;
  info->E.cnt_max = KEY_E_CNT_MAX;
  info->R.cnt_max = KEY_R_CNT_MAX;
  info->A.cnt_max = KEY_A_CNT_MAX;
  info->S.cnt_max = KEY_S_CNT_MAX;
  info->D.cnt_max = KEY_D_CNT_MAX;
  info->F.cnt_max = KEY_F_CNT_MAX;
  info->G.cnt_max = KEY_G_CNT_MAX;
  info->Z.cnt_max = KEY_Z_CNT_MAX;
  info->X.cnt_max = KEY_X_CNT_MAX;
  info->C.cnt_max = KEY_C_CNT_MAX;
  info->V.cnt_max = KEY_V_CNT_MAX;
  info->B.cnt_max = KEY_B_CNT_MAX;
  info->Shift.cnt_max = KEY_SHIFT_CNT_MAX;
  info->Ctrl.cnt_max = KEY_CTRL_CNT_MAX;
}

/**
  * @brief  信息初始化
  */
void rc_info_init(rc_info_t *info)
{
	/* 设置失联计数 */
  info->offline_cnt = REMOTE_OFFLINE_CNT_MAX;
  /* 设置状态 */
  info->status = DEV_OFFLINE;
}

/**
  * @brief  中断标志清除
  */
void rc_interrupt_init(rc_base_info_t *info)
{
  rc_interrupt = false;                  //遥控器更新中断
}

/**
  * @brief  遥控器接收中断更新
  * @note   遥控器接收中断产生时更新完数据执行
  */
void rc_interrupt_update(rc_t *rc)
{
	/* 鼠标速度均值滤波 */
	static int16_t mouse_x[REMOTE_SMOOTH_TIMES], mouse_y[REMOTE_SMOOTH_TIMES];
	static int16_t index = 0;
	index++;
	if(index == REMOTE_SMOOTH_TIMES)
	{
		index = 0;
	}
	rc->info->mouse_x -= (float)mouse_x[index] / (float)REMOTE_SMOOTH_TIMES;
	rc->info->mouse_y -= (float)mouse_y[index] / (float)REMOTE_SMOOTH_TIMES;
	mouse_x[index] = rc->base_info->mouse_vx;
	mouse_y[index] = rc->base_info->mouse_vy;
	rc->info->mouse_x += (float)mouse_x[index] / (float)REMOTE_SMOOTH_TIMES;
	rc->info->mouse_y += (float)mouse_y[index] / (float)REMOTE_SMOOTH_TIMES;
	
	/* 中断更新 */
  rc_interrupt = true;
  rc->info->offline_cnt = 0;
  rc->info->status = DEV_ONLINE;
}

/**
  * @brief  遥控器滴答任务
  */
void rc_tick_task(rc_t *rc)
{
  rc->info->offline_cnt ++;
  if(rc->info->offline_cnt > REMOTE_OFFLINE_CNT_MAX)
  {
    rc_init(rc);
  }
	else 
	{
		remote_soft_reset_check(rc);
	}
}

/**
  * @brief  控制任务
  */
void rc_ctrl(rc_t *rc)
{
  if(rc_interrupt == true)
  {
    rc_interrupt = false;
		/* 遥控器拨杆状态更新 */
    rc_switch_status_interrupt_update(rc->base_info);
		/* 遥控器旋钮状态更新 */
    rc_wheel_status_interrupt_update(rc->base_info);
		/* 键盘按键状态更新 */
    all_key_board_status_interrupt_update(rc->base_info);
  }
  else 
  {
		/* 键盘按键状态更新 */
    all_key_board_status_update(rc->base_info);
  }
	
	/* 鼠标速度低通滤波 */
	rc->info->mouse_x_K = lowpass(rc->info->mouse_x_K,rc->info->mouse_x,0.1);
	rc->info->mouse_y_K = lowpass(rc->info->mouse_y_K,rc->info->mouse_y,0.1);
}

/**
  * @brief  遥控器拨杆状态跳变判断并更新
  */
void rc_switch_status_interrupt_update(rc_base_info_t *info)
{
  /* 左拨杆判断 */
  if(info->s1.value != info->s1.value_last)
  {
    switch(info->s1.value)
    {
      case 1:
        info->s1.status = up_R;
        break;
      case 3:
        info->s1.status = mid_R;
        break;
      case 2:
        info->s1.status = down_R;
        break;
      default:
        break;
    }
    info->s1.value_last = info->s1.value;
  }
  else 
  {
    info->s1.status = keep_R;
  }
  /* 右拨杆判断 */
  if(info->s2.value != info->s2.value_last)
  {
    switch(info->s2.value)
    {
      case 1:
        info->s2.status = up_R;
        break;
      case 3:
        info->s2.status = mid_R;
        break;
      case 2:
        info->s2.status = down_R;
        break;
      default:
        break;
    }
    info->s2.value_last = info->s2.value;
  }
  else 
  {
    info->s2.status = keep_R;
  }
}

/**
  * @brief  遥控器旋钮状态跳变判断并更新
  */
void rc_wheel_status_interrupt_update(rc_base_info_t *info)
{
  if(abs(info->thumbwheel.value_last) < WHEEL_JUMP_VALUE)
  {
    if(info->thumbwheel.value > WHEEL_JUMP_VALUE)
    {
      info->thumbwheel.status = up_R;
    }
    else if(info->thumbwheel.value < -WHEEL_JUMP_VALUE)
    {
      info->thumbwheel.status = down_R;
    }
    else 
    {
      info->thumbwheel.status = keep_R;
    }
  }
  else 
  {
    info->thumbwheel.status = keep_R;
  }
  info->thumbwheel.value_last = info->thumbwheel.value;
}

/**
  * @brief  遥控器接收产生中断时所有键盘按键状态判断并更新
  */
void all_key_board_status_interrupt_update(rc_base_info_t *info)
{
  key_board_status_interrupt_update(&info->mouse_btn_l);
  key_board_status_interrupt_update(&info->mouse_btn_r);
  key_board_status_interrupt_update(&info->Q);
  key_board_status_interrupt_update(&info->W);
  key_board_status_interrupt_update(&info->E);
  key_board_status_interrupt_update(&info->R);
  key_board_status_interrupt_update(&info->A);
  key_board_status_interrupt_update(&info->S);
  key_board_status_interrupt_update(&info->D);
  key_board_status_interrupt_update(&info->F);
  key_board_status_interrupt_update(&info->G);
  key_board_status_interrupt_update(&info->Z);
  key_board_status_interrupt_update(&info->X);
  key_board_status_interrupt_update(&info->C);
  key_board_status_interrupt_update(&info->V);
  key_board_status_interrupt_update(&info->B);
  key_board_status_interrupt_update(&info->Shift);
  key_board_status_interrupt_update(&info->Ctrl);
}

/**
  * @brief  遥控器接收产生中断时键盘按键状态判断并更新
  */
void key_board_status_interrupt_update(key_board_info_t *key)
{
  switch(key->status)
  {
    case relax_K:
      if(key->value == 1)
      {
        key->status = down_K;
        key->cnt = 0;
      }
      break;
    case short_press_K:
      if(key->value == 0)
      {
        key->status = up_K;
				key->cnt = 0;
      }
      else if(key->value == 1)
      {
        key->cnt++;
        if(key->cnt >= key->cnt_max)
        {
          key->status = long_press_K;
					key->cnt = key->cnt_max;
        }
      }
      break;
    case long_press_K:
      if(key->value == 0)
      {
        key->status = up_K;
				key->cnt = 0;
      }
      break;
    default:
      break;
  }
}

/**
  * @brief  所有键盘按键状态判断并更新
  */
void all_key_board_status_update(rc_base_info_t *info)
{
  key_board_status_update(&info->mouse_btn_l);
  key_board_status_update(&info->mouse_btn_r);
  key_board_status_update(&info->Q);
  key_board_status_update(&info->W);
  key_board_status_update(&info->E);
  key_board_status_update(&info->R);
  key_board_status_update(&info->A);
  key_board_status_update(&info->S);
  key_board_status_update(&info->D);
  key_board_status_update(&info->F);
  key_board_status_update(&info->G);
  key_board_status_update(&info->Z);
  key_board_status_update(&info->X);
  key_board_status_update(&info->C);
  key_board_status_update(&info->V);
  key_board_status_update(&info->B);
  key_board_status_update(&info->Shift);
  key_board_status_update(&info->Ctrl);
}

/**
  * @brief  键盘按键状态判断并更新
  */
void key_board_status_update(key_board_info_t *key)
{
  switch(key->status)
  {
    case down_K:
      key->status = short_press_K;
      key->cnt++;
      break;
    case up_K:
      key->status = relax_K;
			key->cnt = 0;
      break;
    case short_press_K:
      key->cnt++;
      if(key->cnt >= key->cnt_max)
      {
        key->value = long_press_K;
				key->cnt = key->cnt_max;
      }
      break;
    default:
      break;
  }
}

void remote_soft_reset_check(rc_t *rc)
{
		if((rc->base_info->Z.status != relax_K)&&(rc->base_info->X.status != relax_K)&&(rc->base_info->C.status != relax_K))
		{
			HAL_NVIC_SystemReset();
		}
}

/**
  * @brief  遥控器通道值归零启动（防止误触）
  * @param  rc_t *rc
  * @retval bool值，0为有非零通道值，1为所有通道值归零
  */
bool Remote_Start_Safely(rc_t *rc)
{
	static uint32_t rc_receive_delay_cnt = 0;
	
	if(rc->base_info->ch0 == 0 && rc->base_info->ch1 == 0 && rc->base_info->ch2 == 0 && rc->base_info->ch3 == 0)
	{
		rc_receive_delay_cnt++;
		if(rc_receive_delay_cnt >= 200)
		{
			rc_receive_delay_cnt = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	/*判断比赛状态看是否需要超时退出*/
}
