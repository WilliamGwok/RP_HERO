#include "armer.h"
#include "drv_haltick.h"
#include "armer_motor.h"
/*3510*/
void armer_init(void)
{
	pid_init(&post_pid);
	
	post_pid_info.target = GAME_SPEED;
}

uint32_t num_cnt = 0;
uint32_t period_buff = 0;
uint8_t direction_random_change(void)
{
	uint32_t random_num = 0;
	
	random_num = micros();
	
	if(random_num % 9 == 0)
	{
		num_cnt++;
	}
	period_buff = random_num % 7 + 1;
	
	if(num_cnt >= period_buff * 800 + 800)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int16_t post_output = 0;
int8_t test = 1;

void armer_control(void)
{
	post_pid_info.measure = post_speed;
	
	single_pid_cal(&post_pid_info);
	
	post_output = (int16_t)post_pid_info.out;
	
	can1_0x200_send_buff[3] = post_output;
	
	CAN1_Send_With_int16_to_uint8(0x200, can1_0x200_send_buff);
}

/*3508*/

//float armer_motor_speed_pid_param[7]    = {27,0.5,0,0,6000,6000,10000};

//void armer_init(void)
//{
//	armer_motor.init(&armer_motor);
//	
//	armer_motor.pid_init(&armer_motor.pid.speed,armer_motor_speed_pid_param);
//}

//void armer_control(void)
//{
//	can1_0x200_send_buff[armer_motor.id.buff_p] = armer_motor.c_speed(&armer_motor,GAME_SPEED);
//}
