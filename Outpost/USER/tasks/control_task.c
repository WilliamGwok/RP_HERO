#include "control_task.h"
#include "cmsis_os.h"


extern IWDG_HandleTypeDef hiwdg;


void control_task(void const * argument)
{
  /* USER CODE BEGIN control_task */
	armer_init();
  /* Infinite loop */
  for(;;)
  {
		HAL_IWDG_Refresh(&hiwdg);
		
		armer_control();
    osDelay(1);
  }
  /* USER CODE END control_task */
}


