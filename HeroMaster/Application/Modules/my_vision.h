#ifndef __MY_VISION_H
#define __MY_VISION_H

#include "stm32f4xx_hal.h"
#include "vision_protocol.h"

typedef struct
{
	uint16_t offline_cnt_max;
}Vision_Config_t;

typedef struct
{
	uint16_t offline_cnt;
	uint8_t status;
}Vision_Info_t;

/*����̨,�Ӿ������İ� begin*/
typedef struct 
{
	float yaw_receive;
	float pitch_receive;
	float yaw_transmit;
	float pitch_transmit;
	bool vision_work;//�Ӿ��Ƿ��ҵ�Ŀ��
	bool detect_top_mode;
	bool top_fire_ok;
}Vision_Transla_Info_t;
/*����̨���Ӿ������İ� end*/

typedef struct
{
	float yaw_org_receive;
	float pitch_org_receive;
	float distance_receive;
	uint8_t capture_Y_O_N;
	uint8_t detect_mode;//�Ӿ�״̬��1 ���� 2 ʶ��С����
	uint8_t gyro_found;
	uint8_t detect_number;
	uint8_t main_mode;
	uint16_t UI_x;
	uint16_t UI_y;
}Vision_Base_Info_t;//��Щ��Ա������������Ӿ�ͳһ�����㹵ͨ

typedef struct 
{
	
	Vision_Config_t* config;
	
	Vision_Info_t* info;
	
	Vision_Transla_Info_t* transla_info;
	
	Vision_Tx_Info_t* tx_info;
	
	Vision_Rx_Info_t* rx_info;
		
	Vision_Base_Info_t* base_info;
}My_Vision_t;

extern My_Vision_t My_Vision;

void My_Vision_Init(void);
void My_Vision_Tick_Task(My_Vision_t *my_vision);
void My_Vision_Receive_Check(My_Vision_t *my_vision);
void My_Vision_Transmit_Check(My_Vision_t *my_vision);
bool My_Vision_Receive(uint8_t *rxBuf);
void My_Vision_Transmit(My_Vision_t *my_vision);
void My_Vision_Auto_Fire_Check(My_Vision_t *my_vision);

#endif

