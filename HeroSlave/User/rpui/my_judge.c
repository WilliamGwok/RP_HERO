/**
  ******************************************************************************
  * @file           : my_judge.c\h
	* @author         : czf
	* @date           : 
  * @brief          : ���ݡ�RoboMaster_����ϵͳ����Э�鸽¼ V1.3����д
	                    ��Ի����˼佻������
	* @history        : 
  ******************************************************************************
  */

/* Ŀ¼begin */

//���Ĳ�
//**********ʵʱ����
//������
//**********��������
//�ڶ���
//**********��ȡͼ������֡
//********************��ȡֱ������֡
//********************��ȡ��������֡
//********************��ȡ��Բ����֡
//********************��ȡ��Բ����֡
//********************��ȡԲ������֡
//********************��ȡ����������֡
//********************��ȡ����������֡
//********************��ȡ�ַ�����֡
//**********����֡����
//********************���ͻ���һ��ͼ��֡����
//********************���ͻ��ƶ���ͼ��֡����
//********************���ͻ������ͼ��֡����
//********************���ͻ����߸�ͼ��֡����
//********************���ͻ����ַ�֡����
//��һ��
//**********���ڷ�������

/* Ŀ¼end */

#include "my_judge.h"
#include "crc8.h"
#include "crc16.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "judge_protocol.h"
#include "cap_protocol.h"
#include "judge.h"

/* ������ begin */
#define UI_huart huart4  //���ڽӿ�
/* ������ end */

extern UART_HandleTypeDef UI_huart;

client_info_t client_info = 
{
	.robot_id = 1,
	.client_id = 0x0101,
};
uint8_t client_tx_buf[128];

/**************************************************���Ĳ�begin**************************************************/

uint16_t char1_delay = 0;
uint16_t char2_delay = 0;
uint16_t char3_delay = 0;
uint16_t char4_delay = 0;
uint16_t group2_delay = 0;
uint16_t group3_delay = 0;

/**
  * @brief  ʵʱ����
  * @param  �������ڣ�ms��
  * @retval 
  */

void tick_task(uint16_t time)
{
	uint8_t task_num_max = 8;
	static uint8_t task_num = 0;
	
	global_vary_system();
	client_info_update();
	
	switch(task_num)
	{
		case 0:
			if(char1_delay <= 2)
			{
				UI_send_char_1();
			}
			else
			{
				UI_send_Group1();
				if(char1_delay >= 10)
				{
					char1_delay = 0;
				}
			}
			char1_delay++;
			break;
		case 1:
      UI_send_Group1();
			break;
		case 2:
			if(group2_delay <= 2)
			{
				UI_send_Group2();
			}
			else
			{
				UI_send_Group5();
				if(group2_delay >= 4)
				{
					group2_delay = 0;
				}
			}
			group2_delay++;
			break;
		case 3:
			if(group3_delay <= 2)
			{
				UI_send_Group3();
			}
			else
			{
				UI_send_char_5();
				if(group3_delay >= 10)
				{
					group3_delay = 0;
				}
			}
			group3_delay++;
			break;
		case 4:
			UI_send_Group4();
			break;
		case 5:
			if(char2_delay <= 2)
			{
				UI_send_char_2();
			}
			else
			{
				UI_send_Group1();
				if(char2_delay >= 10)
				{
					char2_delay = 0;
				}
			}
			char2_delay++;
			break;
		case 6:
			if(char3_delay <= 2)
			{
				UI_send_char_3();
			}
			else
			{
				UI_send_Group1();
				if(char3_delay >= 10)
				{
					char3_delay = 0;
				}
			}
			char3_delay++;
			break;
		case 7:
			if(char4_delay <= 2)
			{
				UI_send_char_4();
			}
			else
			{
				UI_send_Group1();
				if(char4_delay >= 10)
				{
					char4_delay = 0;
				}
			}
			char4_delay++;
			break;
		case 8:
//			if((global_receive.UI_x != global_receive.UI_x_last) || (global_receive.UI_y != global_receive.UI_y_last))
//			{
//				client_graphic_delete_update(4);
//			}
			break;
	}
	task_num++;
	task_num %= task_num_max;
}

int32_t cnttt = 0;

void client_info_update(void)
{
	switch(judge.judge_info->game_robot_status.robot_id)
	{
		case 1:
			client_info.robot_id = 1;
		  client_info.client_id = 0x0101;
			break;
		case 101:
			client_info.robot_id = 101;
		  client_info.client_id = 0x0165;
			break;
		default:
			break;
	}
}

/**************************************************���Ĳ�end**************************************************/


/**************************************************������begin**************************************************/

/******************************��������begin******************************/

/******************************��������end******************************/

/**************************************************������end**************************************************/

/**************************************************�ڶ���begin**************************************************/

/******************************��ȡͼ������֡begin******************************/

/**
  * @brief  ��ȡֱ������֡
  * @param  
  * @retval ͼ�����ݽṹ��
  */
graphic_data_struct_t draw_line(char *name,  //ͼ����
	             uint8_t operate_tpye,  //ͼ�β���
               uint8_t layer,  //ͼ������0~9
               uint8_t color,  //��ɫ
               uint16_t width,  //�������
               uint16_t start_x,  //��� x ����
               uint16_t start_y,  //��� y ����
               uint16_t end_x,  //�յ� x ����
               uint16_t end_y)  //�յ� y ����
{
	graphic_data_struct_t data;
	
	memcpy(data.graphic_name, name, 3);
	data.operate_tpye = operate_tpye;
	data.graphic_tpye = 0;
	data.layer = layer;
	data.color = color;
	data.start_angle = 0;
	data.end_angle = 0;
	data.width = width;
	data.start_x = start_x;
	data.start_y = start_y;
	data.radius = 0;
	data.end_x = end_x;
	data.end_y = end_y;
	
	return data;
}

/**
  * @brief  ��ȡ��������֡
  * @param  
  * @retval ͼ�����ݽṹ��
  */
graphic_data_struct_t draw_rectangle(char *name,  //ͼ����
	             uint8_t operate_tpye,  //ͼ�β���
               uint8_t layer,  //ͼ������0~9
               uint8_t color,  //��ɫDSFZa
               uint16_t width,  //�������
               uint16_t start_x,  //��� x ����
               uint16_t start_y,  //��� y ����
               uint16_t end_x,  //�ԽǶ��� x ����
               uint16_t end_y)  //�ԽǶ��� y ����
{
	graphic_data_struct_t data;
	
	memcpy(data.graphic_name, name, 3);
	data.operate_tpye = operate_tpye;
	data.graphic_tpye = 1;
	data.layer = layer;
	data.color = color;
	data.start_angle = 0;
	data.end_angle = 0;
	data.width = width;
	data.start_x = start_x;
	data.start_y = start_y;
	data.radius = 0;
	data.end_x = end_x;
	data.end_y = end_y;
	
	return data;
}

/**
  * @brief  ��ȡ��Բ����֡
  * @param  
  * @retval ͼ�����ݽṹ��
  */
graphic_data_struct_t draw_circle(char *name,  //ͼ����
	             uint8_t operate_tpye,  //ͼ�β���
               uint8_t layer,  //ͼ������0~9
               uint8_t color,  //��ɫ
               uint16_t width,  //�������
               uint16_t ciclemid_x,  //Բ�� x ����
               uint16_t ciclemid_y,  //Բ�� y ����
               uint16_t radius)  //�뾶
{
	graphic_data_struct_t data;
	
	memcpy(data.graphic_name, name, 3);
	data.operate_tpye = operate_tpye;
	data.graphic_tpye = 2;
	data.layer = layer;
	data.color = color;
	data.start_angle = 0;
	data.end_angle = 0;
	data.width = width;
	data.start_x = ciclemid_x;
	data.start_y = ciclemid_y;
	data.radius = radius;
	data.end_x = 0;
	data.end_y = 0;
	
	return data;
}

/**
  * @brief  ��ȡ��Բ����֡
  * @param  
  * @retval ͼ�����ݽṹ��
  */
graphic_data_struct_t draw_ellipse(char *name,  //ͼ����
	             uint8_t operate_tpye,  //ͼ�β���
               uint8_t layer,  //ͼ������0~9
               uint8_t color,  //��ɫ
               uint16_t width,  //�������
               uint16_t start_x,  //Բ�� x ����
               uint16_t start_y,  //Բ�� y ����
               uint16_t end_x,  //x ���᳤��
               uint16_t end_y)  //y ���᳤��
{
	graphic_data_struct_t data;
	
	memcpy(data.graphic_name, name, 3);
	data.operate_tpye = operate_tpye;
	data.graphic_tpye = 3;
	data.layer = layer;
	data.color = color;
	data.start_angle = 0;
	data.end_angle = 0;
	data.width = width;
	data.start_x = start_x;
	data.start_y = start_y;
	data.radius = 0;
	data.end_x = end_x;
	data.end_y = end_y;
	
	return data;
}

/**
  * @brief  ��ȡԲ������֡
  * @param  
  * @retval ͼ�����ݽṹ��
  */
graphic_data_struct_t draw_arc(char *name,  //ͼ����
	             uint8_t operate_tpye,  //ͼ�β���
               uint8_t layer,  //ͼ������0~9
               uint8_t color,  //��ɫ
               uint16_t start_angle,  //��ʼ�Ƕ�
               uint16_t end_angle,  //��ֹ�Ƕ�
               uint16_t width,  //�������
               uint16_t circlemin_x,  //Բ�� x ����
               uint16_t circlemin_y,  //Բ�� y ����
               uint16_t end_x,  //x ���᳤��
               uint16_t end_y)  //y ���᳤��
{
	graphic_data_struct_t data;
	
	memcpy(data.graphic_name, name, 3);
	data.operate_tpye = operate_tpye;
	data.graphic_tpye = 4;
	data.layer = layer;
	data.color = color;
	data.start_angle = start_angle;
	data.end_angle = end_angle;
	data.width = width;
	data.start_x = circlemin_x;
	data.start_y = circlemin_y;
	data.radius = 0;
	data.end_x = end_x;
	data.end_y = end_y;
	
	return data;
}

/**
  * @brief  ��ȡ����������֡
  * @param  
  * @retval ͼ�����ݽṹ��
  */
graphic_data_struct_t draw_float(char *name,  //ͼ����
	             uint8_t operate_tpye,  //ͼ�β���
               uint8_t layer,  //ͼ������0~9
               uint8_t color,  //��ɫ
               uint16_t size,  //�����С
               uint16_t decimal,  //С��λ��Ч����
               uint16_t width,  //�������
               uint16_t start_x,  //��� x ����
               uint16_t start_y,  //��� y ����
               int32_t num)  //���� 1000 ���� 32 λ��������int32_t
{
	graphic_data_struct_t data;
	
	memcpy(data.graphic_name, name, 3);
	data.operate_tpye = operate_tpye;
	data.graphic_tpye = 5;
	data.layer = layer;
	data.color = color;
	data.start_angle = size;
	data.end_angle = decimal;
	data.width = width;
	data.start_x = start_x;
	data.start_y = start_y;
	data.radius = num;
	data.end_x = num >> 10;
	data.end_y = num >> 21;
	
	return data;
}

/**
  * @brief  ��ȡ����������֡
  * @param  
  * @retval ͼ�����ݽṹ��
  */
graphic_data_struct_t draw_int(char *name,  //ͼ����
	             uint8_t operate_tpye,  //ͼ�β���
               uint8_t layer,  //ͼ������0~9
               uint8_t color,  //��ɫ
               uint16_t size,  //�����С
               uint16_t width,  //�������
               uint16_t start_x,  //��� x ����
               uint16_t start_y,  //��� y ����
               int32_t num)  //32 λ��������int32_t
{
	graphic_data_struct_t data;
	
	memcpy(data.graphic_name, name, 3);
	data.operate_tpye = operate_tpye;
	data.graphic_tpye = 6;
	data.layer = layer;
	data.color = color;
	data.start_angle = size;
	data.end_angle = 0;
	data.width = width;
	data.start_x = start_x;
	data.start_y = start_y;
	data.radius = num;
	data.end_x = num >> 10;
	data.end_y = num >> 21;
	
	return data;
}

/**
  * @brief  ��ȡ�ַ�����֡
  * @param  
  * @retval ͼ�����ݽṹ��
  */
graphic_data_struct_t draw_char(char *name,  //ͼ����
	             uint8_t operate_tpye,  //ͼ�β���
               uint8_t layer,  //ͼ������0~9
               uint8_t color,  //��ɫ
               uint16_t size,  //�����С
               uint16_t length,  //�ַ�����
               uint16_t width,  //�������
               uint16_t start_x,  //��� x ����
               uint16_t start_y)  //��� y ����
{
	graphic_data_struct_t data;
	
	memcpy(data.graphic_name, name, 3);
	data.operate_tpye = operate_tpye;
	data.graphic_tpye = 7;
	data.layer = layer;
	data.color = color;
	data.start_angle = size;
	data.end_angle = length;
	data.width = width;
	data.start_x = start_x;
	data.start_y = start_y;
	data.radius = 0;
	data.end_x = 0;
	data.end_y = 0;
	
	return data;
}

/******************************��ȡͼ������֡end******************************/

/******************************����֡����begin******************************/

/**
	* @brief  ���ͻ���һ��ͼ��֡����
  * @param  
  * @retval 
  */
uint8_t client_send_single_graphic(ext_client_custom_graphic_single_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* ֡ͷ */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_one_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* ������ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* ���ݶ� */
	data_header.data_cmd_id = ID_draw_one_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], &data.grapic_data_struct, 15);
	
	/* ֡β */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_one_graphic + 2);
	
	/* ���� */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_one_graphic + 2);
}

/**
	* @brief  ���ͻ��ƶ���ͼ��֡����
  * @param  
  * @retval 
  */
uint8_t client_send_double_graphic(ext_client_custom_graphic_double_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* ֡ͷ */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_two_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* ������ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* ���ݶ� */
	data_header.data_cmd_id = ID_draw_two_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], data.grapic_data_struct, 15*2);
	
	/* ֡β */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_two_graphic + 2);
	
	/* ���� */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_two_graphic + 2);
}

/**
	* @brief  ���ͻ������ͼ��֡����
  * @param  
  * @retval 
  */
uint8_t client_send_five_graphic(ext_client_custom_graphic_five_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* ֡ͷ */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_five_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* ������ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* ���ݶ� */
	data_header.data_cmd_id = ID_draw_five_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], data.grapic_data_struct, 15*5);
	
	/* ֡β */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_five_graphic + 2);
	
	/* ���� */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_five_graphic + 2);
}

/**
	* @brief  ���ͻ����߸�ͼ��֡����
  * @param  
  * @retval 
  */
uint8_t client_send_seven_graphic(ext_client_custom_graphic_seven_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* ֡ͷ */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_seven_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* ������ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* ���ݶ� */
	data_header.data_cmd_id = ID_draw_seven_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], data.grapic_data_struct, 15*7);
	
	/* ֡β */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_seven_graphic + 2);
	
	/* ���� */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_seven_graphic + 2);
}

/**
	* @brief  ���ͻ����ַ�֡����
  * @param  
  * @retval 
  */
uint8_t client_send_char(ext_client_custom_character_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* ֡ͷ */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_char_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* ������ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* ���ݶ� */
	data_header.data_cmd_id = ID_draw_char_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], &data.grapic_data_struct, 15);
	memcpy(&client_tx_buf[28], data.data, 30);
	
	/* ֡β */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_char_graphic + 2);
	
	/* ���� */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_char_graphic + 2);
}

/**
	* @brief  ɾ��һ��ͼ��
  * @param  uint8_t delete_layer ɾ����ͼ����
  * @retval 
  */
uint8_t client_graphic_delete_update(uint8_t delete_layer)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* ֡ͷ */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_char_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* ������ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* ���ݶ� */
	data_header.data_cmd_id = ID_delete_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	client_tx_buf[13] = 1;//ɾ������ͼ��
	client_tx_buf[14] = delete_layer;//ɾ��ͼ��
	
	/* ֡β */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_delete_graphic + 2);
	
	/* ���� */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_delete_graphic + 2);
}

/******************************����֡����end****************************************/

/**************************************************�ڶ���end**************************************************/


/**************************************************��һ��begin**************************************************/

void Usart4_Sent_Byte(uint8_t ch)
{
	UART4->DR = ch;
	while((UART4->SR&0x40)==0);
}

void Usart4_Sent_string(uint8_t *string, uint16_t length)
{
	uint16_t i;
	
	for(i = 0; i<length; i++)
	{
		Usart4_Sent_Byte(string[i]);
	}
}

void Client_Sent_String(uint8_t *string, uint16_t length)
{
	Usart4_Sent_string(string, length);
}

/**
	* @brief  ���ڷ�������
  * @param  
  * @retval 
  */
uint8_t uart_send_data(uint8_t *txbuf, uint16_t length)
{
	return HAL_UART_Transmit_DMA(&UI_huart, txbuf, length);
//	 Client_Sent_String(txbuf,length);
//	 return 1;
}

/**************************************************��һ��end**************************************************/




/*ȫ�ֱ�������*/
global_receive_t global_receive;

float global_cap_voltage = 0.0f;
uint16_t global_bullet_remain = 0;
uint8_t global_top_mode = 0; 
uint8_t global_vision_status = 0;
uint8_t global_hero_control_mode = 0;
uint8_t global_fric_status = 0;
uint8_t global_detect_number = 0;
int16_t global_fric_speed = 0;


/*ȫ�ֱ����仯���� */
void global_vary_system(void)
{
	/*����ȫ�ֱ���*/
	global_cap_voltage = cap_receive_data.cap_voltage;
	/*ʣ�൯��*/
	global_top_mode = global_receive.top_mode;
	
	global_vision_status = global_receive.vision_status;
	
	global_hero_control_mode = global_receive.hero_control_mode;
	
	global_fric_status = global_receive.friction_status;
	
	global_detect_number = global_receive.vision_detect_num;
	
	global_fric_speed = global_receive.fric_speed;
}






/********************************************************************�ַ�����ʾ***********************************************************************/
/*�ַ�����ʾ*/

/*�ַ��� ��ʾ����ģʽ*/
char supercap_info[30] = {"MODE:"};
float low_voltage = 15;//�͵�������
/*�ַ��� ��ʾС����״̬*/
char top_info[30] = {"TOP_MODE:"};
/*�ַ��� ��ʾ�Ӿ�״̬*/
char vision_info[30] = {"VISION:"};
/*�ַ��� ��ʾĦ����״̬*/
char fric_info[30] = {"FRIC:"};
/*�ַ��� ��ʾС���ݿ�*/
char top_state_1[30] = {"ON"};
/*�ַ��� ��ʾС���ݿ�*/
char top_state_2[30] = {"OFF"};

/*�ṹ��_����ģʽ*/
graphic_data_struct_t mode_char_buff;
ext_client_custom_character_t mode_char_character;
/*�ṹ��_С����*/
graphic_data_struct_t top_mode_buff;
ext_client_custom_character_t top_mode_character;
/*�ṹ��_�Ӿ�*/
graphic_data_struct_t vision_mode_buff;
ext_client_custom_character_t vision_mode_character;
/*�ṹ��_Ħ����*/
graphic_data_struct_t fric_mode_buff;
ext_client_custom_character_t fric_mode_character;
/*�ṹ��_С���ݿ���*/
graphic_data_struct_t top_state_buff;
ext_client_custom_character_t top_state_character;

/*�����ַ���*/
void UI_send_char_1(void)
{
  static int operate_tpye = ADD;
	
	/*��ʾС����״̬ begin*/
	top_mode_buff = draw_char("gc0",  //ͼ����
	                        operate_tpye,  //ͼ�β���
                          1,  //ͼ������0~9
                          GREEN,  //��ɫ
                          18,  //�����С
                          strlen(top_info),  //�ַ�����
                          2,  //�������
                          Client_mid_position_x - 800,  //��� x ����
                          Client_mid_position_y + 196);  //��� y ����
	
	top_mode_character.grapic_data_struct = top_mode_buff;
	memcpy(&top_mode_character.data,&top_info,sizeof(top_info));
		
	client_send_char(top_mode_character);

  /*��ʾС����״̬ end*/
}

void UI_send_char_2(void)
{
  static int operate_tpye = ADD;
	
	/*��ʾ�Ӿ�״̬ begin*/
	vision_mode_buff = draw_char("gc1",  //ͼ����
	                        operate_tpye,  //ͼ�β���
                          1,  //ͼ������0~9
                          GREEN,  //��ɫ
                          18,  //�����С
                          strlen(vision_info),  //�ַ�����
                          2,  //�������
                          Client_mid_position_x - 800,  //��� x ����
                          Client_mid_position_y + 84);  //��� y ����
	
	vision_mode_character.grapic_data_struct = vision_mode_buff;
	memcpy(&vision_mode_character.data,&vision_info,sizeof(vision_info));
		
	client_send_char(vision_mode_character);

  /*��ʾ�Ӿ�״̬ end*/
}

void UI_send_char_3(void)
{
  static int operate_tpye = ADD;
	
	/*��ʾĦ����״̬ begin*/
	fric_mode_buff = draw_char("gc2",  //ͼ����
	                        operate_tpye,  //ͼ�β���
                          1,  //ͼ������0~9
                          GREEN,  //��ɫ
                          18,  //�����С
                          strlen(fric_info),  //�ַ�����
                          2,  //�������
                          Client_mid_position_x - 800,  //��� x ����
                          Client_mid_position_y + 28);  //��� y ����
	
	fric_mode_character.grapic_data_struct = fric_mode_buff;
	memcpy(&fric_mode_character.data,&fric_info,sizeof(fric_info));
		
	client_send_char(fric_mode_character);

  /*��ʾĦ����״̬ end*/
	
//	cnt++;
//  cnt %= 10;
}

void UI_send_char_4(void)
{
  static int operate_tpye = ADD;
	
	/*��ʾģʽ�ַ��� begin*/
	mode_char_buff = draw_char("gc3",  //ͼ����
	                        operate_tpye,  //ͼ�β���
                          1,  //ͼ������0~9
                          GREEN,  //��ɫ
                          18,  //�����С
                          strlen(supercap_info),  //�ַ�����
                          2,  //�������
                          Client_mid_position_x - 800,  //��� x ����//-100
                          Client_mid_position_y + 140);  //��� y ����//-250
	
	mode_char_character.grapic_data_struct = mode_char_buff;
	memcpy(&mode_char_character.data,&supercap_info,sizeof(supercap_info));
	
	client_send_char(mode_char_character);

	/*��ʾģʽ�ַ��� end*/
}

void UI_send_char_5(void)
{
	static int cnt = 0;
  static int operate_tpye = ADD;
	
	if(cnt == 0)
  {
    operate_tpye = ADD;  //һ����һ����ADD
  }
  else 
  {
    operate_tpye = MODIFY;
  }
	
	uint8_t color = WHITE;
	
	if(global_top_mode == 1)
	{
		color = FUCHSIA;
	}
	else
	{
		color = WHITE;
	}
	
	/*��ʾС�����ַ��� begin*/
	top_state_buff = draw_char("gc4",  //ͼ����
	                        operate_tpye,  //ͼ�β���
                          1,  //ͼ������0~9
                          color,  //��ɫ
                          22,  //�����С
                          strlen(top_state_1),  //�ַ�����
                          2,  //�������
                          Client_mid_position_x - 623,  //��� x ����//-100
                          Client_mid_position_y + 198);  //��� y ����//-250
	
	if(global_top_mode == 1)
	{
		top_state_character.grapic_data_struct = top_state_buff;
	  memcpy(&top_state_character.data,&top_state_1,sizeof(top_state_1));
	}
	else
	{
		top_state_character.grapic_data_struct = top_state_buff;
	  memcpy(&top_state_character.data,&top_state_2,sizeof(top_state_2));
	}
	
	client_send_char(top_state_character);
	
	cnt++;
  cnt %= 10;

	/*��ʾģʽ�ַ��� end*/
}
/******************************************************************ͼ����ʾ**********************************************************************/

/*��group*/
ext_client_custom_graphic_seven_t Group1;
ext_client_custom_graphic_seven_t Group4;

/*����group*/
ext_client_custom_graphic_seven_t Group2;
ext_client_custom_graphic_seven_t Group3;
ext_client_custom_graphic_seven_t Group5;

void UI_send_Group1(void)
{
	static int cnt = 0;
  static int operate_tpye = ADD;
	uint8_t super_cap_color = 0;
	uint8_t ry_rectangle_color = 0;
	uint16_t ry_rectangle_width = 0;
	uint8_t mode_color = 0;
  
  if(cnt == 0)
  {
    operate_tpye = ADD;  //һ����һ����ADD
  }
  else 
  {
    operate_tpye = MODIFY;
  }
	
	/*��ʾ������*/
	if(global_cap_voltage <= low_voltage)
	{
		super_cap_color = PINK;
	}
	else
	{
		super_cap_color = WHITE;
	}
	
	Group1.grapic_data_struct[0] = draw_line("g10",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       super_cap_color,  //��ɫ
                                       20,  //�������
                                       Client_mid_position_x - 130,  //��� x ����
                                       10,  //��� y ����
                                       (uint16_t)(Client_mid_position_x - 130 + 0.5f * global_cap_voltage * global_cap_voltage),  //�յ� x ����
                                       10);
																			
  /*��Ħ����ת��*/
	Group1.grapic_data_struct[1] = draw_int("g11",  //ͼ����
	                                    operate_tpye,  //ͼ�β���
                                      0,  //ͼ������0~9
                                      GREEN,  //��ɫ
                                      30,  //�����С
                                      2,  //�������
                                      Client_mid_position_x + 400,  //��� x ����
                                      Client_mid_position_y + 200,  //��� y ����
                                      (int32_t)global_fric_speed);  //32 λ��������int32_t
	
	if(global_receive.ROI_Mode == 1)
	{
		ry_rectangle_color = FUCHSIA;
		ry_rectangle_width = 1;
	}
	else
	{
		ry_rectangle_color = WHITE;
		ry_rectangle_width = 1;
	}
	
	/*��ROI��*/
	Group1.grapic_data_struct[2] = draw_rectangle("g12",  //ͼ����
	                                    operate_tpye,  //ͼ�β���
                                      0,  //ͼ������0~9
                                      ry_rectangle_color,  //��ɫDSFZa
                                      ry_rectangle_width,  //�������
                                      Client_mid_position_x - 120,  //��� x ����
                                      Client_mid_position_y - 120,  //��� y ����
                                      Client_mid_position_x + 120,  //�ԽǶ��� x ����
                                      Client_mid_position_y - 235);  //�ԽǶ��� y ����
	
	if(global_receive.hero_control_mode == 2)
	{
		mode_color = GREEN;
	}
	else if(global_receive.hero_control_mode == 1)
	{
		mode_color = FUCHSIA;
	}
	else
	{
		mode_color = BLACK;
	}
	
	Group1.grapic_data_struct[3] = draw_circle("g13",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       mode_color,  //��ɫ
                                       2,  //�������
                                       Client_mid_position_x - 610,  //Բ�� x ����
                                       Client_mid_position_y + 131,  //Բ�� y ����
                                       13);  //�뾶
	
	Group1.grapic_data_struct[4] = draw_circle("g14",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       mode_color,  //��ɫ
                                       2,  //�������
                                       Client_mid_position_x - 610,  //Բ�� x ����
                                       Client_mid_position_y + 131,  //Բ�� y ����
                                       22);  //�뾶
	
	/*����ץȡĿ��*/
	Group1.grapic_data_struct[5] = draw_circle("g15",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       4,  //ͼ������0~9
                                       YELLOW,  //��ɫ
                                       2,  //�������
                                       global_receive.UI_x,  //Բ�� x ����
                                       global_receive.UI_y,  //Բ�� y ����
                                       13);  //�뾶
	
  /*�ܻ�װ�װ�*/	
	if(global_receive.want_draw_hurt == true && global_receive.armor_show_cnt <= 120)
	{
		Group1.grapic_data_struct[6] = draw_circle("g16",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       5,  //ͼ������0~9
                                       ORANGE,  //��ɫ
                                       40,  //�������
                                       global_receive.armor_hurt_pos_x,  //Բ�� x ����
                                       global_receive.armor_hurt_pos_y,  //Բ�� y ����
                                       20);  //�뾶
		
		global_receive.armor_show_cnt++;
	}
	else
	{
		Group1.grapic_data_struct[6] = draw_circle("g16",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       5,  //ͼ������0~9
                                       ORANGE,  //��ɫ
                                       50,  //�������
                                       0,  //Բ�� x ����
                                       0,  //Բ�� y ����
                                       25);  //�뾶
		global_receive.armor_show_cnt = 0;
		global_receive.want_draw_hurt = false;
	}
	
	client_send_seven_graphic(Group1);
  
  cnt++;
  cnt %= 10;
}


uint16_t AIM_X = 956;
uint16_t AIM_Y = 395;//540


/*��׼��*/
void UI_send_Group2(void)
{
  static int operate_tpye = ADD;
	
	/*up to down*/
//	/*��0*/
//	Group2.grapic_data_struct[0] = draw_line("g20",  //ͼ����
//                                       operate_tpye,  //ͼ�β���
//                                       1,  //ͼ������0~9
//                                       FUCHSIA,  //��ɫ
//                                       1,  //�������
//                                       AIM_X - 80,  //��� x ����
//                                       AIM_Y + 50,  //��� y ����
//                                       AIM_X + 80,  //�յ� x ����
//                                       AIM_Y + 50);
	
	/*��1��6m*/
	Group2.grapic_data_struct[0] = draw_int("g20",  //ͼ����
	                                    operate_tpye,  //ͼ�β���
                                      1,  //ͼ������0~9
                                      YELLOW,  //��ɫ
                                      10,  //�����С
                                      2,  //�������
                                      AIM_X + 85,  //��� x ����
                                      AIM_Y,  //��� y ����
                                      (int32_t)6);  //32 λ��������int32_t
	
	Group2.grapic_data_struct[1] = draw_line("g21",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       YELLOW,  //��ɫ
                                       2,  //�������
                                       AIM_X - 80,  //��� x ����
                                       AIM_Y - 5,  //��� y ����
                                       AIM_X + 80,  //�յ� x ����
                                       AIM_Y - 5);
	/*��2��7.1m*/
	Group2.grapic_data_struct[2] = draw_line("g22",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       GREEN,  //��ɫ
                                       2,  //�������
                                       AIM_X - 80,  //��� x ����
                                       AIM_Y - 30,  //��� y ����
                                       AIM_X + 80,  //�յ� x ����
                                       AIM_Y - 30);
																			 
	Group2.grapic_data_struct[3] = draw_int("g23",  //ͼ����
	                                    operate_tpye,  //ͼ�β���
                                      1,  //ͼ������0~9
                                      YELLOW,  //��ɫ
                                      10,  //�����С
                                      2,  //�������
                                      AIM_X + 85,  //��� x ����
                                      AIM_Y - 25,  //��� y ����
                                      (int32_t)7);  //32 λ��������int32_t
																			 
//	 /*��3*/
//	Group2.grapic_data_struct[3] = draw_line("g23",  //ͼ����
//                                       operate_tpye,  //ͼ�β���
//                                       1,  //ͼ������0~9
//                                       FUCHSIA,  //��ɫ
//                                       1,  //�������
//                                       AIM_X - 30,  //��� x ����
//                                       AIM_Y - 80,  //��� y ����
//                                       AIM_X + 30,  //�յ� x ����
//                                       AIM_Y - 80);
																			 
	 /*��1*/
	Group2.grapic_data_struct[4] = draw_line("g24",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       GREEN,  //��ɫ
                                       1,  //�������
                                       AIM_X,  //��� x ����
                                       AIM_Y + 75,  //��� y ����
                                       AIM_X,  //�յ� x ����
                                       10);
	/*7m������*/														
	Group2.grapic_data_struct[5] = draw_line("g25",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       GREEN,  //��ɫ
                                       2,  //�������
                                       AIM_X - 15,  //��� x ����
                                       AIM_Y - 22,  //��� y ����
                                       AIM_X + 15,  //�յ� x ����
                                       AIM_Y - 22);
																			 
	Group2.grapic_data_struct[6] = draw_line("g26",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       GREEN,  //��ɫ
                                       2,  //�������
                                       AIM_X - 15,  //��� x ����
                                       AIM_Y - 38,  //��� y ����
                                       AIM_X + 15,  //�յ� x ����
                                       AIM_Y - 38);
																			 
//	/*б1*/
//	Group2.grapic_data_struct[5] = draw_line("g25",  //ͼ����
//                                       operate_tpye,  //ͼ�β���
//                                       1,  //ͼ������0~9
//                                       FUCHSIA,  //��ɫ
//                                       1,  //�������
//                                       AIM_X - 80,  //��� x ����
//                                       AIM_Y + 50,  //��� y ����
//                                       AIM_X - 30,  //�յ� x ����
//                                       AIM_Y - 80);	

//   /*б2*/
//	Group2.grapic_data_struct[6] = draw_line("g26",  //ͼ����
//                                       operate_tpye,  //ͼ�β���
//                                       1,  //ͼ������0~9
//                                       FUCHSIA,  //��ɫ
//                                       1,  //�������
//                                       AIM_X + 80,  //��� x ����
//                                       AIM_Y + 50,  //��� y ����
//                                       AIM_X + 30,  //�յ� x ����
//                                       AIM_Y - 80);	
	
	client_send_seven_graphic(Group2);
}

/*���Ϳ�ͨ����*/
void UI_send_Group3(void)
{
  static int operate_tpye = ADD;
	
	/*��б*/
	Group3.grapic_data_struct[0] = draw_line("g30",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       WHITE,  //��ɫ
                                       1,  //�������
                                       Client_mid_position_x - 230,  //��� x ����
                                       Client_mid_position_y - 60,  //��� y ����
                                       Client_mid_position_x - 520,  //�յ� x ����
                                       0);
	
	/*��б*/
  Group3.grapic_data_struct[1] = draw_line("g31",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       WHITE,  //��ɫ
                                       1,  //�������
                                       Client_mid_position_x + 230,  //��� x ����
                                       Client_mid_position_y - 60,  //��� y ����
                                       Client_mid_position_x + 520,  //�յ� x ����
                                       0);

  /*����*/
  Group3.grapic_data_struct[2] = draw_line("g32",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       WHITE,  //��ɫ
                                       2,  //�������
                                       Client_mid_position_x - 160,  //��� x ����
                                       Client_mid_position_y,  //��� y ����
                                       Client_mid_position_x + 160,  //�յ� x ����
                                       Client_mid_position_y);
	/*׼�� 10.5m*/																		 
	Group3.grapic_data_struct[3] = draw_line("g33",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       BLACK,  //��ɫ
                                       2,  //�������
                                       AIM_X - 60,  //��� x ����
                                       AIM_Y - 120,  //��� y ����
                                       AIM_X + 60,  //�յ� x ����
                                       AIM_Y - 120);
																			 
	Group3.grapic_data_struct[4] = draw_line("g34",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       GREEN,  //��ɫ
                                       2,  //�������
                                       AIM_X - 15,  //��� x ����
                                       AIM_Y - 110,  //��� y ����
                                       AIM_X + 15,  //�յ� x ����
                                       AIM_Y - 110);
																			 
	Group3.grapic_data_struct[5] = draw_line("g35",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       GREEN,  //��ɫ
                                       2,  //�������
                                       AIM_X - 15,  //��� x ����
                                       AIM_Y - 130,  //��� y ����
                                       AIM_X + 15,  //�յ� x ����
                                       AIM_Y - 130);										
										
	Group3.grapic_data_struct[6] = draw_int("g36",  //ͼ����
	                                    operate_tpye,  //ͼ�β���
                                      1,  //ͼ������0~9
                                      GREEN,  //��ɫ
                                      10,  //�����С
                                      2,  //�������
                                      AIM_X + 65,  //��� x ����
                                      AIM_Y - 115,  //��� y ����
                                      (int32_t)11);  //32 λ��������int32_t									
											
	client_send_seven_graphic(Group3);
}

/*���͸���״̬��־λ*/
void UI_send_Group4(void)
{
	static int cnt = 0;
  static int operate_tpye = ADD;
	uint8_t vision_color = 0;
//	uint8_t top_color = 0;
	uint8_t fric_color = 0;
  
  if(cnt == 0)
  {
    operate_tpye = ADD;  //һ����һ����ADD
  }
  else 
  {
    operate_tpye = MODIFY;
  }
	
	if(global_vision_status == 2)
	{
		vision_color = FUCHSIA;
	}
	else
	{
		if(global_vision_status == 1)
		{
			vision_color = GREEN;
		}
		else
		{
			vision_color = YELLOW;
		}
	}
	
	Group4.grapic_data_struct[0] = draw_circle("g40",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       vision_color,  //��ɫ
                                       2,  //�������
                                       Client_mid_position_x - 610,  //Բ�� x ����
                                       Client_mid_position_y + 75,  //Բ�� y ����
                                       13);  //�뾶
	
	Group4.grapic_data_struct[1] = draw_circle("g41",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       vision_color,  //��ɫ
                                       2,  //�������
                                       Client_mid_position_x - 610,  //Բ�� x ����
                                       Client_mid_position_y + 75,  //Բ�� y ����
                                       22);  //�뾶
	
	
	
	if(global_receive.stop_hit != 0)
	{
		  Group4.grapic_data_struct[2] = draw_line("g42",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       FUCHSIA,  //��ɫ
                                       5,  //�������
                                       Client_mid_position_x - 200,  //��� x ����
                                       Client_mid_position_y - 280,  //��� y ����
                                       Client_mid_position_x + 200,  //�յ� x ����
                                       Client_mid_position_y - 280);
	}
	else
	{
		Group4.grapic_data_struct[2] = draw_line("g42",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       GREEN,  //��ɫ
                                       0,  //�������
                                       Client_mid_position_x - 150,  //��� x ����
                                       Client_mid_position_y - 200,  //��� y ����
                                       Client_mid_position_x + 150,  //�յ� x ����
                                       Client_mid_position_y - 200);
	}
//	/**/
//	if(global_top_mode == 1)
//	{
//		top_color = FUCHSIA;
//	}
//	else
//	{
//		top_color = YELLOW;
//	}
//	
//  Group4.grapic_data_struct[2] = draw_circle("g42",  //ͼ����
//	                                     operate_tpye,  //ͼ�β���
//                                       1,  //ͼ������0~9
//                                       top_color,  //��ɫ
//                                       2,  //�������
//                                       Client_mid_position_x - 610,  //Բ�� x ����
//                                       Client_mid_position_y + 187,  //Բ�� y ����
//                                       13);  //�뾶
//	
//	Group4.grapic_data_struct[3] = draw_circle("g43",  //ͼ����
//	                                     operate_tpye,  //ͼ�β���
//                                       1,  //ͼ������0~9
//                                       top_color,  //��ɫ
//                                       2,  //�������
//                                       Client_mid_position_x - 610,  //Բ�� x ����
//                                       Client_mid_position_y + 187,  //Բ�� y ����
//                                       22);  //�뾶
	/**/
	if(global_fric_status == 0)
	{
		fric_color = BLACK;
	}
	else
	{
		if(global_fric_status == 1)
		{
			fric_color = YELLOW;
		}
		else
		{
			fric_color = FUCHSIA;
		}
	}
	
  Group4.grapic_data_struct[4] = draw_circle("g44",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       fric_color,  //��ɫ
                                       2,  //�������
                                       Client_mid_position_x - 610,  //Բ�� x ����
                                       Client_mid_position_y + 19,  //Բ�� y ����
                                       13);  //�뾶
	
	Group4.grapic_data_struct[5] = draw_circle("g45",  //ͼ����
	                                     operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       fric_color,  //��ɫ
                                       2,  //�������
                                       Client_mid_position_x - 610,  //Բ�� x ����
                                       Client_mid_position_y + 19,  //Բ�� y ����
                                       22);  //�뾶
	
//	/*pitch �Ƕ�*/
//	Group4.grapic_data_struct[6] = draw_float("g46",  //ͼ����
//	                                    operate_tpye,  //ͼ�β���
//                                      1,  //ͼ������0~9
//                                      WHITE,  //��ɫ
//                                      24,  //�����С
//                                      2,  //С��λ��Ч����
//                                      2,  //�������
//                                      Client_mid_position_x - 800,  //��� x ����
//                                      Client_mid_position_y + 245,  //��� y ����
//                                      (int32_t)((global_receive.pitch_angle) * 1000));//���� 1000 ���� 32 λ��������int32_t

  Group4.grapic_data_struct[6] = draw_int("g46",  //ͼ����
	                                    operate_tpye,  //ͼ�β���
                                      1,  //ͼ������0~9
                                      YELLOW,  //��ɫ
                                      24,  //�����С
                                      2,  //�������
                                      Client_mid_position_x - 800,  //��� x ����
                                      Client_mid_position_y + 245,  //��� y ����
                                      (int32_t)global_detect_number);  //32 λ��������int32_t		
																			 
	client_send_seven_graphic(Group4);
  
  cnt++;
  cnt %= 10;
}

void UI_send_Group5(void)
{
  static int operate_tpye = ADD;
	
	/*׼�� 4m*/																		 
	Group5.grapic_data_struct[0] = draw_line("g50",  //ͼ����
                                       operate_tpye,  //ͼ�β���
                                       1,  //ͼ������0~9
                                       WHITE,  //��ɫ
                                       2,  //�������
                                       AIM_X - 80,  //��� x ����
                                       AIM_Y + 30,  //��� y ����
                                       AIM_X + 80,  //�յ� x ����
                                       AIM_Y + 30);
	
	Group5.grapic_data_struct[1] = draw_int("g51",  //ͼ����
	                                    operate_tpye,  //ͼ�β���
                                      1,  //ͼ������0~9
                                      YELLOW,  //��ɫ
                                      10,  //�����С
                                      2,  //�������
                                      AIM_X + 85,  //��� x ����
                                      AIM_Y + 35,  //��� y ����
                                      (int32_t)4);  //32 λ��������int32_t		
																			
																			
																			 						
											
	client_send_seven_graphic(Group5);
}

/************************************************************************************************************************************************/






