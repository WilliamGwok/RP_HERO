#include "config_can.h"
#include "string.h"
#include "drv_haltick.h"
#include "armer_motor.h"


int16_t can1_0x200_send_buff[4];//id -> 1 buff[0] -> voltage
uint8_t outpost_motor_rxBuf[8];
int16_t outpost_motor_info[4];
uint16_t post_angle = 0;
int16_t post_speed = 0;

void CAN1_rxDataHandler(uint32_t canId, uint8_t *rxBuf)
{
	switch(canId)
	{
		case outpost_motor:
			/*3510*/
			memcpy(outpost_motor_rxBuf,rxBuf,sizeof(outpost_motor_rxBuf));
		  post_angle = ((uint16_t)rxBuf[0] << 8 | rxBuf[1]);//mec angle
		  post_speed = ((uint16_t)rxBuf[2] << 8 | rxBuf[3]);//speed rpm
		  /*3508*/
//		  armer_motor.rx(&armer_motor,rxBuf,8);
		  break;
		default:
			break;
	}
}

void CAN2_rxDataHandler(uint32_t canId, uint8_t *rxBuf)
{
//	switch(canId)
//	{
//		  break;
//	}
}

