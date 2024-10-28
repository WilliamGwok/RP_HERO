#ifndef __SERVE_PROTOCOL_H
#define __SERVE_PROTOCOL_H

#include "stm32f4xx_hal.h"

void trans_to_master_powerheat(void);
void trans_to_master_robot_status(void);
void trans_to_master_shoot_data(void);
void trans_to_master_robot_pos(void);
void trans_to_master_robot_hurt(void);

#endif




