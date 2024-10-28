#include "my_judge.h"
#include "config.h"
#include "my_hero.h"

Judge_Info_t Judge_Info;
Judge_Status_t Judge_Status = 
{
	.offline_cnt_max = JUDGE_OFFLINE_CNT_MAX,
};
My_Judge_t My_Judge = 
{
	.info = &Judge_Info,
	.status = &Judge_Status,
};

/**
  * @brief  裁判系统初始化
  * @param  My_Judge_t * my_judge
  * @retval None
  */
void My_Judge_Init()
{
	My_Judge.status->offline_cnt = My_Judge.status->offline_cnt_max;
	My_Judge.status->status = DEV_OFFLINE;
}

/**
  * @brief  裁判系统实时任务，检测离线与状态更新
  * @param  My_Judge_t * my_judge
  * @retval None
  */
void My_Judge_Realtime_Task(My_Judge_t* my_judge)
{
	my_judge->status->offline_cnt++;
	if(my_judge->status->offline_cnt >= my_judge->status->offline_cnt_max)
	{
		My_Judge_Init();
	}
	
	My_Judge_Update(my_judge);
}

/**
  * @brief  裁判系统数据分析后更新状态
  * @param  My_Judge_t * my_judge
  * @retval None
  */
void My_Judge_Update(My_Judge_t * my_judge)
{
	my_judge->info->remain_HP_now = my_judge->info->remain_HP;
	
	if(my_judge->info->game_status == 4)//比赛状态为对战中
	{
		if(my_judge->info->remain_HP_now > 0 && my_judge->info->remain_HP_last == 0)//对战中复活
		{
			HERO.command->judge->Relive_Flag = true;
		}
		
		HERO.command->judge->Fighting_Flag = true;
	}
	else
	{
		HERO.command->judge->Fighting_Flag = false;
		HERO.command->judge->Relive_Flag = false;
	}
}


