#ifndef __VISION_PROTOCOL_H
#define __VISION_PROTOCOL_H

#include "stm32f4xx_hal.h"
#include "stdbool.h"


/*与lzs的视觉协定*/
/*
        角度取0~8191
	
	          前方
	          4096
					   
 左方 6144	车体  2048 右方
	
	         8191/0
						后方
						
	pitch轴角度 角度取0~8191
	
            头顶
						2048

 枪管 4096  车体   0/8191 后脑勺
 
            6144
						脖子
	
*/


typedef __packed struct
{
	uint8_t   SOF;
	uint8_t   datau8_1;//模式
	uint8_t   CRC8;
	float     datas_1;//yaw_angle
	float     datas_2;//pitch_angle
	uint8_t   datau8_3;//
	uint8_t   datau8_2;//己方颜色
	uint8_t   datau8_4;//切换数字
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

