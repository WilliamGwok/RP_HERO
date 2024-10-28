#ifndef __VISION_PROTOCOL_H
#define __VISION_PROTOCOL_H

#include "stm32f4xx_hal.h"
#include "stdbool.h"


/*��lzs���Ӿ�Э��*/
/*
        �Ƕ�ȡ0~8191
	
	          ǰ��
	          4096
					   
 �� 6144	����  2048 �ҷ�
	
	         8191/0
						��
						
	pitch��Ƕ� �Ƕ�ȡ0~8191
	
            ͷ��
						2048

 ǹ�� 4096  ����   0/8191 ������
 
            6144
						����
	
*/


typedef __packed struct
{
	uint8_t   SOF;
	uint8_t   datau8_1;//ģʽ
	uint8_t   CRC8;
	float     datas_1;//yaw_angle
	float     datas_2;//pitch_angle
	uint8_t   datau8_3;//
	uint8_t   datau8_2;//������ɫ
	uint8_t   datau8_4;//�л�����
	uint16_t  CRC16;
}Vision_Tx_Info_t;

typedef __packed struct
{
	uint8_t  SOF;
	uint8_t  datau8_1;//detect_mode
	uint8_t  CRC8;
	float    dataf_1;  //pitch
	float    dataf_2;  //yaw
	uint8_t  datau8_2; //is_find_target
	uint8_t  datau8_3;  //distance
	uint8_t  datau8_4;  //gyro_found
	uint8_t  datau8_5;//car_number
	uint16_t datau16_1;//UI
	uint16_t datau16_2;//UI
	uint16_t CRC16;
}Vision_Rx_Info_t;

extern Vision_Tx_Info_t Vision_Tx_Info;
extern Vision_Rx_Info_t Vision_Rx_Info;

bool Vision_Send_Data(void);
bool Vision_Recieve_Data(uint8_t *rxBuf);

#endif

