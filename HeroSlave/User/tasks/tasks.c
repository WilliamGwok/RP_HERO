#include "tasks.h"
#include "cmsis_os.h"
#include "my_judge.h"
#include "drv_tim.h"

extern IWDG_HandleTypeDef hiwdg;

void Control_Task(void const * argument)
{
	uint16_t UI_delay = 0;
	
  for(;;)
  {
		if(UI_delay >= 11)//数据交互不超30Hz
		{
			tick_task(1);
			UI_delay = 0;
		}
		else
		{
			UI_delay++;
		}
		
		HAL_IWDG_Refresh(&hiwdg);
    osDelay(1);
  }
}




