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
  * @brief  �Ӿ���ʼ��
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Init(void)
{
	memset(My_Vision.base_info, 0, sizeof(Vision_Base_Info_t));
	memset(My_Vision.info, 0, sizeof(Vision_Base_Info_t));
	memset(My_Vision.rx_info, 0, sizeof(Vision_Rx_Info_t));
	memset(My_Vision.transla_info, 0, sizeof(Vision_Transla_Info_t));
	
	/*���Ӿ�Ҫ��ǿ��main_modeһֱΪ1*/
	My_Vision.base_info->main_mode = 1;
	My_Vision.info->status = DEV_OFFLINE;
}

/**
  * @brief  �Ӿ�������
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
  * @brief  ���¸��Ӿ���������
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Transmit_Data_Update(My_Vision_t *my_vision)
{
	/*������̨λ��*/
	My_Vision_Transmit_Check(my_vision);
	my_vision->tx_info->datas_1 = my_vision->transla_info->yaw_transmit;
	my_vision->tx_info->datas_2 = my_vision->transla_info->pitch_transmit;
	
	/*���Ӿ�Ҫ�����main_mode*/
	if(HERO.command->vision->ROI_On != true)
	{
		my_vision->base_info->main_mode = 1;
	}
	else
	{
		my_vision->base_info->main_mode = 7;
	}
	my_vision->tx_info->datau8_1 = my_vision->base_info->main_mode;
	
	/*���¼�����ɫ*/
	my_vision->tx_info->datau8_2 = My_Judge.info->car_color;
	
	/*�������岻���Ķ���*/
	my_vision->tx_info->datau8_3 = 0;
}

/**
  * @brief  ���Ӿ����ݣ��ڴ����жϵ���
  * @param  uint8_t *rxBuf
  * @retval None
  */
bool My_Vision_Receive(uint8_t *rxBuf)
{
	Vision_Base_Info_t *base_info = My_Vision.base_info;
	Vision_Info_t *info = My_Vision.info;
	
	/*�յ����ݣ�������߼���ֵ*/
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
  * @brief  ���Ӿ�������
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Transmit(My_Vision_t *my_vision)
{
	My_Vision_Transmit_Data_Update(my_vision);
	
	Vision_Send_Data();
}


/*...............................................������...............................................*/

/**
  * @brief  ������Ӿ��յ��ĽǶȣ�0~8191ת-180~180
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Receive_Check(My_Vision_t *my_vision)
{
	/*��������� begin*/
	float yaw_org = my_vision->base_info->yaw_org_receive;
	float pitch_org = my_vision->base_info->pitch_org_receive;
	/*��������� end*/
	
	
	/*�Ӿ��ҵ�Ŀ��*/
	if(my_vision->base_info->capture_Y_O_N != 0)
	{
		my_vision->transla_info->vision_work = true;
		
		/*�Ƕȴ��� begin*/
		my_vision->transla_info->yaw_receive = ((4096.f - yaw_org) / 4096.f) * 180.f;
		my_vision->transla_info->pitch_receive = ((4096.f - pitch_org) / 4096.f) * 180.f;
		/*�Ƕȴ��� end*/
	}
	else
	{
		/*��������*/
		my_vision->transla_info->vision_work = false;
		my_vision->transla_info->yaw_receive = imu.base_info->yaw;
		my_vision->transla_info->pitch_receive = imu.base_info->pitch;
	}
}
	
/**
  * @brief  �������Ӿ��ĽǶȣ�-180~180ת0~8191
  * @param  My_Vision_t *my_vision
  * @retval None
  */
void My_Vision_Transmit_Check(My_Vision_t *my_vision)
{
	float yaw_org = imu.base_info->yaw;
	float pitch_org = imu.base_info->pitch;
	
	/*�Ƕȴ��� begin*/
	my_vision->transla_info->yaw_transmit = 4096.f -  (yaw_org / 180.f) * 4096;
	my_vision->transla_info->pitch_transmit = 4096.f -  (pitch_org / 180.f) * 4096;
	/*�Ƕȴ��� end*/
}

uint16_t cnt_test_1 = 0;

/**
  * @brief  �����Զ��򵯱�־λ
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

