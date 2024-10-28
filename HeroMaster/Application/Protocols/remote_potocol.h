/**
  ******************************************************************************
  * @file           : remote_potocol.c/h
  * @brief          : 
  * @note           : finish 2022-2-13 13:57:59
  ******************************************************************************
  */

#ifndef __REMOTE_POTOCOL_H
#define __REMOTE_POTOCOL_H

#include "remote.h"

void rc_base_info_update(rc_base_info_t *info, uint8_t *rxBuf);
void rc_base_info_check(rc_base_info_t *info);

#endif
