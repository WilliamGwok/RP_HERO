/**
  ******************************************************************************
  * @file           : remote_potocol.c/h
  * @brief          : 
  * @note           : finish 2022-2-13 13:54:06
  ******************************************************************************
  */

#include "remote_potocol.h"
#include "math_support.h"

/**
  * @brief  遥控信息更新
  */
void rc_base_info_update(rc_base_info_t *info, uint8_t *rxBuf)
{
  info->ch0 = (rxBuf[0]      | rxBuf[1] << 8                 ) & 0x07FF;
  info->ch0 -= 1024;
  info->ch1 = (rxBuf[1] >> 3 | rxBuf[2] << 5                 ) & 0x07FF;
  info->ch1 -= 1024;
  info->ch2 = (rxBuf[2] >> 6 | rxBuf[3] << 2 | rxBuf[4] << 10) & 0x07FF;
  info->ch2 -= 1024;
  info->ch3 = (rxBuf[4] >> 1 | rxBuf[5] << 7                 ) & 0x07FF;
  info->ch3 -= 1024;
  info->s1.value = ((rxBuf[5] >> 4) & 0x000C) >> 2;
  info->s2.value = ( rxBuf[5] >> 4) & 0x0003;

  info->mouse_vx = rxBuf[6]  | (rxBuf[7 ] << 8);
  info->mouse_vy = rxBuf[8]  | (rxBuf[9 ] << 8);
  info->mouse_vz = rxBuf[10] | (rxBuf[11] << 8);
  info->mouse_btn_l.value = rxBuf[12] & 0x01;
  info->mouse_btn_r.value = rxBuf[13] & 0x01;
  info->W.value =   rxBuf[14]        & 0x01;
  info->S.value = ( rxBuf[14] >> 1 ) & 0x01;
  info->A.value = ( rxBuf[14] >> 2 ) & 0x01;
  info->D.value = ( rxBuf[14] >> 3 ) & 0x01;
  info->Shift.value = ( rxBuf[14] >> 4 ) & 0x01;
  info->Ctrl.value = ( rxBuf[14] >> 5 ) & 0x01;
  info->Q.value = ( rxBuf[14] >> 6 ) & 0x01 ;
  info->E.value = ( rxBuf[14] >> 7 ) & 0x01 ;
  info->R.value = ( rxBuf[15] >> 0 ) & 0x01 ;
  info->F.value = ( rxBuf[15] >> 1 ) & 0x01 ;
  info->G.value = ( rxBuf[15] >> 2 ) & 0x01 ;
  info->Z.value = ( rxBuf[15] >> 3 ) & 0x01 ;
  info->X.value = ( rxBuf[15] >> 4 ) & 0x01 ;
  info->C.value = ( rxBuf[15] >> 5 ) & 0x01 ;
  info->V.value = ( rxBuf[15] >> 6 ) & 0x01 ;
  info->B.value = ( rxBuf[15] >> 7 ) & 0x01 ;

  info->thumbwheel.value = ((int16_t)rxBuf[16] | ((int16_t)rxBuf[17] << 8)) & 0x07ff;
  info->thumbwheel.value -= 1024;
}

/**
  * @brief  遥控信息检查
  * @note   对超出限幅的信息处理
  */
void rc_base_info_check(rc_base_info_t *info)
{
  if(abs(info->ch0) > 660)
  {
    info->ch0 = 660 * sgn(info->ch0);
  }
  if(abs(info->ch1) > 660)
  {
    info->ch1 = 660 * sgn(info->ch1);
  }
  if(abs(info->ch2) > 660)
  {
    info->ch2 = 660 * sgn(info->ch2);
  }
  if(abs(info->ch3) > 660)
  {
    info->ch3 = 660 * sgn(info->ch3);
  }
  if(abs(info->thumbwheel.value) > 660)
  {
    info->thumbwheel.value = 660 * sgn(info->thumbwheel.value);
  }
}
