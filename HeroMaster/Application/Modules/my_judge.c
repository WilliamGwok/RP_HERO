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
  * @brief  ����ϵͳ��ʼ��
  * @param  My_Judge_t * my_judge
  * @retval None
  */
void My_Judge_Init()
{
	My_Judge.status->offline_cnt = My_Judge.status->offline_cnt_max;
	My_Judge.status->status = DEV_OFFLINE;
}

/**
  * @brief  ����ϵͳʵʱ���񣬼��������״̬����
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
  * @brief  ����ϵͳ���ݷ��������״̬
  * @param  My_Judge_t * my_judge
  * @retval None
  */
void My_Judge_Update(My_Judge_t * my_judge)
{
	my_judge->info->remain_HP_now = my_judge->info->remain_HP;
	
	if(my_judge->info->game_status == 4)//����״̬Ϊ��ս��
	{
		if(my_judge->info->remain_HP_now > 0 && my_judge->info->remain_HP_last == 0)//��ս�и���
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


