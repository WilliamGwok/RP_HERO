#include "my_vision.h"
#include "my_judge.h"
#include "string.h"
#include "config.h"
#include "imu.h"
#include "math_support.h"
#include "vision_protocol.h"
#include "my_hero.h"

Vision_Config_t Vision_Config = 
{
	.offline_cnt_max = 50,
};
Vision_Info_t Vision_Info;
Vision_Base_Info_t Vision_Base_Info;
Vision_Transla_Info_t Vision_Transla_Info;
My_Vision_t My_Vision = 
{
	.config = &Vision_Config,
	.info = &Vision_Info,
	.transla_info = &Vision_Transla_Info,
	.tx_info = &Vision_Tx_Info,
	.rx_info = &Vision_Rx_Info,
	.base_info = &Vision_Base_Info,
};

/**
  * @brief  视觉初始化
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Init(void)
{
	memset(My_Vision.base_info, 0, sizeof(Vision_Base_Info_t));
	memset(My_Vision.info, 0, sizeof(Vision_Base_Info_t));
	memset(My_Vision.rx_info, 0, sizeof(Vision_Rx_Info_t));
	memset(My_Vision.transla_info, 0, sizeof(Vision_Transla_Info_t));
	
	/*按视觉要求，强制main_mode一直为1*/
	My_Vision.base_info->main_mode = 1;
	My_Vision.info->status = DEV_OFFLINE;
}

/**
  * @brief  视觉心跳包
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Tick_Task(My_Vision_t *my_vision)
{
	Vision_Info_t *info = my_vision->info;
	
	info->offline_cnt++;
	if(info->offline_cnt >= my_vision->config->offline_cnt_max)
	{
		info->offline_cnt = my_vision->config->offline_cnt_max + 1;
		info->status = DEV_OFFLINE;
	}
	else
	{
		info->status = DEV_ONLINE;
	}
}

/**
  * @brief  更新给视觉发的数据
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Transmit_Data_Update(My_Vision_t *my_vision)
{
	/*更新云台位置*/
	My_Vision_Transmit_Check(my_vision);
	my_vision->tx_info->datas_1 = my_vision->transla_info->yaw_transmit;
	my_vision->tx_info->datas_2 = my_vision->transla_info->pitch_transmit;
	
	/*按视觉要求更新main_mode*/
	if(HERO.command->vision->ROI_On != true)
	{
		my_vision->base_info->main_mode = 1;
	}
	else
	{
		my_vision->base_info->main_mode = 7;
	}
	my_vision->tx_info->datau8_1 = my_vision->base_info->main_mode;
	
	/*更新己方颜色*/
	my_vision->tx_info->datau8_2 = My_Judge.info->car_color;
	
	/*更新意义不明的东西*/
	my_vision->tx_info->datau8_3 = 0;
}

/**
  * @brief  收视觉数据，在串口中断调用
  * @param  uint8_t *rxBuf
  * @retval None
  */
bool My_Vision_Receive(uint8_t *rxBuf)
{
	Vision_Base_Info_t *base_info = My_Vision.base_info;
	Vision_Info_t *info = My_Vision.info;
	
	/*收到数据，清空离线计数值*/
	info->offline_cnt = 0;
	
	if(Vision_Recieve_Data(rxBuf) == true)
	{
		memcpy(&base_info->pitch_org_receive,(void*)&Vision_Rx_Info.dataf_1,4);
		memcpy(&base_info->yaw_org_receive,(void*)&Vision_Rx_Info.dataf_2,4);
		memcpy(&base_info->detect_mode,&Vision_Rx_Info.datau8_1,1);
		memcpy(&base_info->capture_Y_O_N,&Vision_Rx_Info.datau8_2,1);
		memcpy(&base_info->distance_receive,&Vision_Rx_Info.datau8_3,1);
		memcpy(&base_info->gyro_found,&Vision_Rx_Info.datau8_4,1);
		memcpy(&base_info->detect_number,&Vision_Rx_Info.datau8_5,1);
		memcpy(&base_info->UI_x, (void*)&Vision_Rx_Info.datau16_1, 2);
		memcpy(&base_info->UI_y, (void*)&Vision_Rx_Info.datau16_2, 2);
		
		My_Vision_Receive_Check(&My_Vision);
		
		My_Vision_Auto_Fire_Check(&My_Vision);
		
		return true;
	}
	return false;
}

/**
  * @brief  给视觉发数据
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Transmit(My_Vision_t *my_vision)
{
	My_Vision_Transmit_Data_Update(my_vision);
	
	Vision_Send_Data();
}


/*...............................................处理函数...............................................*/

/**
  * @brief  处理从视觉收到的角度，0~8191转-180~180
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Receive_Check(My_Vision_t *my_vision)
{
	/*简洁代码变量 begin*/
	float yaw_org = my_vision->base_info->yaw_org_receive;
	float pitch_org = my_vision->base_info->pitch_org_receive;
	/*简洁代码变量 end*/
	
	
	/*视觉找到目标*/
	if(my_vision->base_info->capture_Y_O_N != 0)
	{
		my_vision->transla_info->vision_work = true;
		
		/*角度处理 begin*/
		my_vision->transla_info->yaw_receive = ((4096.f - yaw_org) / 4096.f) * 180.f;
		my_vision->transla_info->pitch_receive = ((4096.f - pitch_org) / 4096.f) * 180.f;
		/*角度处理 end*/
	}
	else
	{
		/*不开自瞄*/
		my_vision->transla_info->vision_work = false;
		my_vision->transla_info->yaw_receive = imu.base_info->yaw;
		my_vision->transla_info->pitch_receive = imu.base_info->pitch;
	}
}
	
/**
  * @brief  处理发给视觉的角度，-180~180转0~8191
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Transmit_Check(My_Vision_t *my_vision)
{
	float yaw_org = imu.base_info->yaw;
	float pitch_org = imu.base_info->pitch;
	
	/*角度处理 begin*/
	my_vision->transla_info->yaw_transmit = 4096.f -  (yaw_org / 180.f) * 4096;
	my_vision->transla_info->pitch_transmit = 4096.f -  (pitch_org / 180.f) * 4096;
	/*角度处理 end*/
}

uint16_t cnt_test_1 = 0;

/**
  * @brief  处理自动打弹标志位
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Auto_Fire_Check(My_Vision_t *my_vision)
{
	if(my_vision->base_info->detect_mode == 2)
	{
		if(my_vision->base_info->gyro_found == 2)
		{
			my_vision->transla_info->top_fire_ok = true;
			cnt_test_1++;
		}
		else
		{
			my_vision->transla_info->top_fire_ok = false;
		}
		my_vision->transla_info->detect_top_mode = true;
	}
	else
	{
		my_vision->transla_info->top_fire_ok = false;
		my_vision->transla_info->detect_top_mode = false;
	}
}

