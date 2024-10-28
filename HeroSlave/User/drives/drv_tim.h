#ifndef __DRV_TIM_H
#define __DRV_TIM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported macro ------------------------------------------------------------*/
#define FRIC_PWM_L	TIM3->CCR1
#define FRIC_PWM_R	TIM3->CCR2
#define SERVO_PWM	TIM1->CCR2
#define BUZZ_PWM    TIM11->CCR1

/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void PWM_Init(void);
void BUZZ_Freq(uint16_t frq);

#endif
