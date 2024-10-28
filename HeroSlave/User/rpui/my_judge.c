/**
  ******************************************************************************
  * @file           : my_judge.c\h
	* @author         : czf
	* @date           : 
  * @brief          : 根据《RoboMaster_裁判系统串口协议附录 V1.3》编写
	                    针对机器人间交互数据
	* @history        : 
  ******************************************************************************
  */

/* 目录begin */

//第四层
//**********实时任务
//第三层
//**********绘制任务
//第二层
//**********获取图像数据帧
//********************获取直线数据帧
//********************获取矩形数据帧
//********************获取整圆数据帧
//********************获取椭圆数据帧
//********************获取圆弧数据帧
//********************获取浮点数数据帧
//********************获取整型数数据帧
//********************获取字符数据帧
//**********发送帧数据
//********************发送绘制一个图形帧数据
//********************发送绘制二个图形帧数据
//********************发送绘制五个图形帧数据
//********************发送绘制七个图形帧数据
//********************发送绘制字符帧数据
//第一层
//**********串口发送数据

/* 目录end */

#include "my_judge.h"
#include "crc8.h"
#include "crc16.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "judge_protocol.h"
#include "cap_protocol.h"
#include "judge.h"

/* 配置区 begin */
#define UI_huart huart4  //串口接口
/* 配置区 end */

extern UART_HandleTypeDef UI_huart;

client_info_t client_info = 
{
	.robot_id = 1,
	.client_id = 0x0101,
};
uint8_t client_tx_buf[128];

/**************************************************第四层begin**************************************************/

uint16_t char1_delay = 0;
uint16_t char2_delay = 0;
uint16_t char3_delay = 0;
uint16_t char4_delay = 0;
uint16_t group2_delay = 0;
uint16_t group3_delay = 0;

/**
  * @brief  实时任务
  * @param  更新周期（ms）
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

/**************************************************第四层end**************************************************/


/**************************************************第三层begin**************************************************/

/******************************绘制任务begin******************************/

/******************************绘制任务end******************************/

/**************************************************第三层end**************************************************/

/**************************************************第二层begin**************************************************/

/******************************获取图像数据帧begin******************************/

/**
  * @brief  获取直线数据帧
  * @param  
  * @retval 图形数据结构体
  */
graphic_data_struct_t draw_line(char *name,  //图形名
	             uint8_t operate_tpye,  //图形操作
               uint8_t layer,  //图层数，0~9
               uint8_t color,  //颜色
               uint16_t width,  //线条宽度
               uint16_t start_x,  //起点 x 坐标
               uint16_t start_y,  //起点 y 坐标
               uint16_t end_x,  //终点 x 坐标
               uint16_t end_y)  //终点 y 坐标
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
  * @brief  获取矩形数据帧
  * @param  
  * @retval 图形数据结构体
  */
graphic_data_struct_t draw_rectangle(char *name,  //图形名
	             uint8_t operate_tpye,  //图形操作
               uint8_t layer,  //图层数，0~9
               uint8_t color,  //颜色DSFZa
               uint16_t width,  //线条宽度
               uint16_t start_x,  //起点 x 坐标
               uint16_t start_y,  //起点 y 坐标
               uint16_t end_x,  //对角顶点 x 坐标
               uint16_t end_y)  //对角顶点 y 坐标
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
  * @brief  获取整圆数据帧
  * @param  
  * @retval 图形数据结构体
  */
graphic_data_struct_t draw_circle(char *name,  //图形名
	             uint8_t operate_tpye,  //图形操作
               uint8_t layer,  //图层数，0~9
               uint8_t color,  //颜色
               uint16_t width,  //线条宽度
               uint16_t ciclemid_x,  //圆心 x 坐标
               uint16_t ciclemid_y,  //圆心 y 坐标
               uint16_t radius)  //半径
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
  * @brief  获取椭圆数据帧
  * @param  
  * @retval 图形数据结构体
  */
graphic_data_struct_t draw_ellipse(char *name,  //图形名
	             uint8_t operate_tpye,  //图形操作
               uint8_t layer,  //图层数，0~9
               uint8_t color,  //颜色
               uint16_t width,  //线条宽度
               uint16_t start_x,  //圆心 x 坐标
               uint16_t start_y,  //圆心 y 坐标
               uint16_t end_x,  //x 半轴长度
               uint16_t end_y)  //y 半轴长度
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
  * @brief  获取圆弧数据帧
  * @param  
  * @retval 图形数据结构体
  */
graphic_data_struct_t draw_arc(char *name,  //图形名
	             uint8_t operate_tpye,  //图形操作
               uint8_t layer,  //图层数，0~9
               uint8_t color,  //颜色
               uint16_t start_angle,  //起始角度
               uint16_t end_angle,  //终止角度
               uint16_t width,  //线条宽度
               uint16_t circlemin_x,  //圆心 x 坐标
               uint16_t circlemin_y,  //圆心 y 坐标
               uint16_t end_x,  //x 半轴长度
               uint16_t end_y)  //y 半轴长度
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
  * @brief  获取浮点数数据帧
  * @param  
  * @retval 图形数据结构体
  */
graphic_data_struct_t draw_float(char *name,  //图形名
	             uint8_t operate_tpye,  //图形操作
               uint8_t layer,  //图层数，0~9
               uint8_t color,  //颜色
               uint16_t size,  //字体大小
               uint16_t decimal,  //小数位有效个数
               uint16_t width,  //线条宽度
               uint16_t start_x,  //起点 x 坐标
               uint16_t start_y,  //起点 y 坐标
               int32_t num)  //乘以 1000 后，以 32 位整型数，int32_t
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
  * @brief  获取整型数数据帧
  * @param  
  * @retval 图形数据结构体
  */
graphic_data_struct_t draw_int(char *name,  //图形名
	             uint8_t operate_tpye,  //图形操作
               uint8_t layer,  //图层数，0~9
               uint8_t color,  //颜色
               uint16_t size,  //字体大小
               uint16_t width,  //线条宽度
               uint16_t start_x,  //起点 x 坐标
               uint16_t start_y,  //起点 y 坐标
               int32_t num)  //32 位整型数，int32_t
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
  * @brief  获取字符数据帧
  * @param  
  * @retval 图形数据结构体
  */
graphic_data_struct_t draw_char(char *name,  //图形名
	             uint8_t operate_tpye,  //图形操作
               uint8_t layer,  //图层数，0~9
               uint8_t color,  //颜色
               uint16_t size,  //字体大小
               uint16_t length,  //字符长度
               uint16_t width,  //线条宽度
               uint16_t start_x,  //起点 x 坐标
               uint16_t start_y)  //起点 y 坐标
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

/******************************获取图像数据帧end******************************/

/******************************发送帧数据begin******************************/

/**
	* @brief  发送绘制一个图形帧数据
  * @param  
  * @retval 
  */
uint8_t client_send_single_graphic(ext_client_custom_graphic_single_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* 帧头 */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_one_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* 命令码ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* 数据段 */
	data_header.data_cmd_id = ID_draw_one_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], &data.grapic_data_struct, 15);
	
	/* 帧尾 */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_one_graphic + 2);
	
	/* 发送 */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_one_graphic + 2);
}

/**
	* @brief  发送绘制二个图形帧数据
  * @param  
  * @retval 
  */
uint8_t client_send_double_graphic(ext_client_custom_graphic_double_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* 帧头 */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_two_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* 命令码ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* 数据段 */
	data_header.data_cmd_id = ID_draw_two_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], data.grapic_data_struct, 15*2);
	
	/* 帧尾 */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_two_graphic + 2);
	
	/* 发送 */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_two_graphic + 2);
}

/**
	* @brief  发送绘制五个图形帧数据
  * @param  
  * @retval 
  */
uint8_t client_send_five_graphic(ext_client_custom_graphic_five_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* 帧头 */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_five_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* 命令码ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* 数据段 */
	data_header.data_cmd_id = ID_draw_five_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], data.grapic_data_struct, 15*5);
	
	/* 帧尾 */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_five_graphic + 2);
	
	/* 发送 */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_five_graphic + 2);
}

/**
	* @brief  发送绘制七个图形帧数据
  * @param  
  * @retval 
  */
uint8_t client_send_seven_graphic(ext_client_custom_graphic_seven_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* 帧头 */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_seven_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* 命令码ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* 数据段 */
	data_header.data_cmd_id = ID_draw_seven_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], data.grapic_data_struct, 15*7);
	
	/* 帧尾 */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_seven_graphic + 2);
	
	/* 发送 */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_seven_graphic + 2);
}

/**
	* @brief  发送绘制字符帧数据
  * @param  
  * @retval 
  */
uint8_t client_send_char(ext_client_custom_character_t data)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* 帧头 */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_char_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* 命令码ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* 数据段 */
	data_header.data_cmd_id = ID_draw_char_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	memcpy(&client_tx_buf[13], &data.grapic_data_struct, 15);
	memcpy(&client_tx_buf[28], data.data, 30);
	
	/* 帧尾 */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_draw_char_graphic + 2);
	
	/* 发送 */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_draw_char_graphic + 2);
}

/**
	* @brief  删除一个图层
  * @param  uint8_t delete_layer 删除的图层数
  * @retval 
  */
uint8_t client_graphic_delete_update(uint8_t delete_layer)
{
	frame_t frame;
	ext_student_interactive_header_data_t data_header;
	
	/* 帧头 */
	frame.frame_header.SOF = 0xA5;
	frame.frame_header.data_length = LEN_ID_draw_char_graphic;
	frame.frame_header.seq = 0;
	memcpy(client_tx_buf, &frame.frame_header, 4);
	Append_CRC8_Check_Num(client_tx_buf, 5);
	
	/* 命令码ID */
	frame.cmd_id = 0x301;
	memcpy(&client_tx_buf[5], (void*)&frame.cmd_id, 2);
	
	/* 数据段 */
	data_header.data_cmd_id = ID_delete_graphic;
	data_header.sender_ID = client_info.robot_id;
	data_header.receiver_ID = client_info.client_id;
	memcpy(&client_tx_buf[7], &data_header, 6);
	client_tx_buf[13] = 1;//删除单个图层
	client_tx_buf[14] = delete_layer;//删除图层
	
	/* 帧尾 */
	Append_CRC16_Check_Sum(client_tx_buf, 5 + 2 + LEN_ID_delete_graphic + 2);
	
	/* 发送 */
	return uart_send_data(client_tx_buf, 5 + 2 + LEN_ID_delete_graphic + 2);
}

/******************************发送帧数据end****************************************/

/**************************************************第二层end**************************************************/


/**************************************************第一层begin**************************************************/

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
	* @brief  串口发送数据
  * @param  
  * @retval 
  */
uint8_t uart_send_data(uint8_t *txbuf, uint16_t length)
{
	return HAL_UART_Transmit_DMA(&UI_huart, txbuf, length);
//	 Client_Sent_String(txbuf,length);
//	 return 1;
}

/**************************************************第一层end**************************************************/




/*全局变量设置*/
global_receive_t global_receive;

float global_cap_voltage = 0.0f;
uint16_t global_bullet_remain = 0;
uint8_t global_top_mode = 0; 
uint8_t global_vision_status = 0;
uint8_t global_hero_control_mode = 0;
uint8_t global_fric_status = 0;
uint8_t global_detect_number = 0;
int16_t global_fric_speed = 0;


/*全局变量变化控制 */
void global_vary_system(void)
{
	/*超电全局变量*/
	global_cap_voltage = cap_receive_data.cap_voltage;
	/*剩余弹量*/
	global_top_mode = global_receive.top_mode;
	
	global_vision_status = global_receive.vision_status;
	
	global_hero_control_mode = global_receive.hero_control_mode;
	
	global_fric_status = global_receive.friction_status;
	
	global_detect_number = global_receive.vision_detect_num;
	
	global_fric_speed = global_receive.fric_speed;
}






/********************************************************************字符串显示***********************************************************************/
/*字符串显示*/

/*字符串 显示控制模式*/
char supercap_info[30] = {"MODE:"};
float low_voltage = 15;//低电容容量
/*字符串 显示小陀螺状态*/
char top_info[30] = {"TOP_MODE:"};
/*字符串 显示视觉状态*/
char vision_info[30] = {"VISION:"};
/*字符串 显示摩擦轮状态*/
char fric_info[30] = {"FRIC:"};
/*字符串 显示小陀螺开*/
char top_state_1[30] = {"ON"};
/*字符串 显示小陀螺开*/
char top_state_2[30] = {"OFF"};

/*结构体_控制模式*/
graphic_data_struct_t mode_char_buff;
ext_client_custom_character_t mode_char_character;
/*结构体_小陀螺*/
graphic_data_struct_t top_mode_buff;
ext_client_custom_character_t top_mode_character;
/*结构体_视觉*/
graphic_data_struct_t vision_mode_buff;
ext_client_custom_character_t vision_mode_character;
/*结构体_摩擦轮*/
graphic_data_struct_t fric_mode_buff;
ext_client_custom_character_t fric_mode_character;
/*结构体_小陀螺开关*/
graphic_data_struct_t top_state_buff;
ext_client_custom_character_t top_state_character;

/*不变字符串*/
void UI_send_char_1(void)
{
  static int operate_tpye = ADD;
	
	/*显示小陀螺状态 begin*/
	top_mode_buff = draw_char("gc0",  //图形名
	                        operate_tpye,  //图形操作
                          1,  //图层数，0~9
                          GREEN,  //颜色
                          18,  //字体大小
                          strlen(top_info),  //字符长度
                          2,  //线条宽度
                          Client_mid_position_x - 800,  //起点 x 坐标
                          Client_mid_position_y + 196);  //起点 y 坐标
	
	top_mode_character.grapic_data_struct = top_mode_buff;
	memcpy(&top_mode_character.data,&top_info,sizeof(top_info));
		
	client_send_char(top_mode_character);

  /*显示小陀螺状态 end*/
}

void UI_send_char_2(void)
{
  static int operate_tpye = ADD;
	
	/*显示视觉状态 begin*/
	vision_mode_buff = draw_char("gc1",  //图形名
	                        operate_tpye,  //图形操作
                          1,  //图层数，0~9
                          GREEN,  //颜色
                          18,  //字体大小
                          strlen(vision_info),  //字符长度
                          2,  //线条宽度
                          Client_mid_position_x - 800,  //起点 x 坐标
                          Client_mid_position_y + 84);  //起点 y 坐标
	
	vision_mode_character.grapic_data_struct = vision_mode_buff;
	memcpy(&vision_mode_character.data,&vision_info,sizeof(vision_info));
		
	client_send_char(vision_mode_character);

  /*显示视觉状态 end*/
}

void UI_send_char_3(void)
{
  static int operate_tpye = ADD;
	
	/*显示摩擦轮状态 begin*/
	fric_mode_buff = draw_char("gc2",  //图形名
	                        operate_tpye,  //图形操作
                          1,  //图层数，0~9
                          GREEN,  //颜色
                          18,  //字体大小
                          strlen(fric_info),  //字符长度
                          2,  //线条宽度
                          Client_mid_position_x - 800,  //起点 x 坐标
                          Client_mid_position_y + 28);  //起点 y 坐标
	
	fric_mode_character.grapic_data_struct = fric_mode_buff;
	memcpy(&fric_mode_character.data,&fric_info,sizeof(fric_info));
		
	client_send_char(fric_mode_character);

  /*显示摩擦轮状态 end*/
	
//	cnt++;
//  cnt %= 10;
}

void UI_send_char_4(void)
{
  static int operate_tpye = ADD;
	
	/*显示模式字符串 begin*/
	mode_char_buff = draw_char("gc3",  //图形名
	                        operate_tpye,  //图形操作
                          1,  //图层数，0~9
                          GREEN,  //颜色
                          18,  //字体大小
                          strlen(supercap_info),  //字符长度
                          2,  //线条宽度
                          Client_mid_position_x - 800,  //起点 x 坐标//-100
                          Client_mid_position_y + 140);  //起点 y 坐标//-250
	
	mode_char_character.grapic_data_struct = mode_char_buff;
	memcpy(&mode_char_character.data,&supercap_info,sizeof(supercap_info));
	
	client_send_char(mode_char_character);

	/*显示模式字符串 end*/
}

void UI_send_char_5(void)
{
	static int cnt = 0;
  static int operate_tpye = ADD;
	
	if(cnt == 0)
  {
    operate_tpye = ADD;  //一秒有一次是ADD
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
	
	/*显示小陀螺字符串 begin*/
	top_state_buff = draw_char("gc4",  //图形名
	                        operate_tpye,  //图形操作
                          1,  //图层数，0~9
                          color,  //颜色
                          22,  //字体大小
                          strlen(top_state_1),  //字符长度
                          2,  //线条宽度
                          Client_mid_position_x - 623,  //起点 x 坐标//-100
                          Client_mid_position_y + 198);  //起点 y 坐标//-250
	
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

	/*显示模式字符串 end*/
}
/******************************************************************图像显示**********************************************************************/

/*变group*/
ext_client_custom_graphic_seven_t Group1;
ext_client_custom_graphic_seven_t Group4;

/*不变group*/
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
    operate_tpye = ADD;  //一秒有一次是ADD
  }
  else 
  {
    operate_tpye = MODIFY;
  }
	
	/*显示超电条*/
	if(global_cap_voltage <= low_voltage)
	{
		super_cap_color = PINK;
	}
	else
	{
		super_cap_color = WHITE;
	}
	
	Group1.grapic_data_struct[0] = draw_line("g10",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       super_cap_color,  //颜色
                                       20,  //线条宽度
                                       Client_mid_position_x - 130,  //起点 x 坐标
                                       10,  //起点 y 坐标
                                       (uint16_t)(Client_mid_position_x - 130 + 0.5f * global_cap_voltage * global_cap_voltage),  //终点 x 坐标
                                       10);
																			
  /*画摩擦轮转速*/
	Group1.grapic_data_struct[1] = draw_int("g11",  //图形名
	                                    operate_tpye,  //图形操作
                                      0,  //图层数，0~9
                                      GREEN,  //颜色
                                      30,  //字体大小
                                      2,  //线条宽度
                                      Client_mid_position_x + 400,  //起点 x 坐标
                                      Client_mid_position_y + 200,  //起点 y 坐标
                                      (int32_t)global_fric_speed);  //32 位整型数，int32_t
	
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
	
	/*画ROI框*/
	Group1.grapic_data_struct[2] = draw_rectangle("g12",  //图形名
	                                    operate_tpye,  //图形操作
                                      0,  //图层数，0~9
                                      ry_rectangle_color,  //颜色DSFZa
                                      ry_rectangle_width,  //线条宽度
                                      Client_mid_position_x - 120,  //起点 x 坐标
                                      Client_mid_position_y - 120,  //起点 y 坐标
                                      Client_mid_position_x + 120,  //对角顶点 x 坐标
                                      Client_mid_position_y - 235);  //对角顶点 y 坐标
	
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
	
	Group1.grapic_data_struct[3] = draw_circle("g13",  //图形名
	                                     operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       mode_color,  //颜色
                                       2,  //线条宽度
                                       Client_mid_position_x - 610,  //圆心 x 坐标
                                       Client_mid_position_y + 131,  //圆心 y 坐标
                                       13);  //半径
	
	Group1.grapic_data_struct[4] = draw_circle("g14",  //图形名
	                                     operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       mode_color,  //颜色
                                       2,  //线条宽度
                                       Client_mid_position_x - 610,  //圆心 x 坐标
                                       Client_mid_position_y + 131,  //圆心 y 坐标
                                       22);  //半径
	
	/*自瞄抓取目标*/
	Group1.grapic_data_struct[5] = draw_circle("g15",  //图形名
	                                     operate_tpye,  //图形操作
                                       4,  //图层数，0~9
                                       YELLOW,  //颜色
                                       2,  //线条宽度
                                       global_receive.UI_x,  //圆心 x 坐标
                                       global_receive.UI_y,  //圆心 y 坐标
                                       13);  //半径
	
  /*受击装甲板*/	
	if(global_receive.want_draw_hurt == true && global_receive.armor_show_cnt <= 120)
	{
		Group1.grapic_data_struct[6] = draw_circle("g16",  //图形名
	                                     operate_tpye,  //图形操作
                                       5,  //图层数，0~9
                                       ORANGE,  //颜色
                                       40,  //线条宽度
                                       global_receive.armor_hurt_pos_x,  //圆心 x 坐标
                                       global_receive.armor_hurt_pos_y,  //圆心 y 坐标
                                       20);  //半径
		
		global_receive.armor_show_cnt++;
	}
	else
	{
		Group1.grapic_data_struct[6] = draw_circle("g16",  //图形名
	                                     operate_tpye,  //图形操作
                                       5,  //图层数，0~9
                                       ORANGE,  //颜色
                                       50,  //线条宽度
                                       0,  //圆心 x 坐标
                                       0,  //圆心 y 坐标
                                       25);  //半径
		global_receive.armor_show_cnt = 0;
		global_receive.want_draw_hurt = false;
	}
	
	client_send_seven_graphic(Group1);
  
  cnt++;
  cnt %= 10;
}


uint16_t AIM_X = 956;
uint16_t AIM_Y = 395;//540


/*画准心*/
void UI_send_Group2(void)
{
  static int operate_tpye = ADD;
	
	/*up to down*/
//	/*横0*/
//	Group2.grapic_data_struct[0] = draw_line("g20",  //图形名
//                                       operate_tpye,  //图形操作
//                                       1,  //图层数，0~9
//                                       FUCHSIA,  //颜色
//                                       1,  //线条宽度
//                                       AIM_X - 80,  //起点 x 坐标
//                                       AIM_Y + 50,  //起点 y 坐标
//                                       AIM_X + 80,  //终点 x 坐标
//                                       AIM_Y + 50);
	
	/*横1，6m*/
	Group2.grapic_data_struct[0] = draw_int("g20",  //图形名
	                                    operate_tpye,  //图形操作
                                      1,  //图层数，0~9
                                      YELLOW,  //颜色
                                      10,  //字体大小
                                      2,  //线条宽度
                                      AIM_X + 85,  //起点 x 坐标
                                      AIM_Y,  //起点 y 坐标
                                      (int32_t)6);  //32 位整型数，int32_t
	
	Group2.grapic_data_struct[1] = draw_line("g21",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       YELLOW,  //颜色
                                       2,  //线条宽度
                                       AIM_X - 80,  //起点 x 坐标
                                       AIM_Y - 5,  //起点 y 坐标
                                       AIM_X + 80,  //终点 x 坐标
                                       AIM_Y - 5);
	/*横2，7.1m*/
	Group2.grapic_data_struct[2] = draw_line("g22",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       GREEN,  //颜色
                                       2,  //线条宽度
                                       AIM_X - 80,  //起点 x 坐标
                                       AIM_Y - 30,  //起点 y 坐标
                                       AIM_X + 80,  //终点 x 坐标
                                       AIM_Y - 30);
																			 
	Group2.grapic_data_struct[3] = draw_int("g23",  //图形名
	                                    operate_tpye,  //图形操作
                                      1,  //图层数，0~9
                                      YELLOW,  //颜色
                                      10,  //字体大小
                                      2,  //线条宽度
                                      AIM_X + 85,  //起点 x 坐标
                                      AIM_Y - 25,  //起点 y 坐标
                                      (int32_t)7);  //32 位整型数，int32_t
																			 
//	 /*横3*/
//	Group2.grapic_data_struct[3] = draw_line("g23",  //图形名
//                                       operate_tpye,  //图形操作
//                                       1,  //图层数，0~9
//                                       FUCHSIA,  //颜色
//                                       1,  //线条宽度
//                                       AIM_X - 30,  //起点 x 坐标
//                                       AIM_Y - 80,  //起点 y 坐标
//                                       AIM_X + 30,  //终点 x 坐标
//                                       AIM_Y - 80);
																			 
	 /*竖1*/
	Group2.grapic_data_struct[4] = draw_line("g24",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       GREEN,  //颜色
                                       1,  //线条宽度
                                       AIM_X,  //起点 x 坐标
                                       AIM_Y + 75,  //起点 y 坐标
                                       AIM_X,  //终点 x 坐标
                                       10);
	/*7m线上下*/														
	Group2.grapic_data_struct[5] = draw_line("g25",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       GREEN,  //颜色
                                       2,  //线条宽度
                                       AIM_X - 15,  //起点 x 坐标
                                       AIM_Y - 22,  //起点 y 坐标
                                       AIM_X + 15,  //终点 x 坐标
                                       AIM_Y - 22);
																			 
	Group2.grapic_data_struct[6] = draw_line("g26",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       GREEN,  //颜色
                                       2,  //线条宽度
                                       AIM_X - 15,  //起点 x 坐标
                                       AIM_Y - 38,  //起点 y 坐标
                                       AIM_X + 15,  //终点 x 坐标
                                       AIM_Y - 38);
																			 
//	/*斜1*/
//	Group2.grapic_data_struct[5] = draw_line("g25",  //图形名
//                                       operate_tpye,  //图形操作
//                                       1,  //图层数，0~9
//                                       FUCHSIA,  //颜色
//                                       1,  //线条宽度
//                                       AIM_X - 80,  //起点 x 坐标
//                                       AIM_Y + 50,  //起点 y 坐标
//                                       AIM_X - 30,  //终点 x 坐标
//                                       AIM_Y - 80);	

//   /*斜2*/
//	Group2.grapic_data_struct[6] = draw_line("g26",  //图形名
//                                       operate_tpye,  //图形操作
//                                       1,  //图层数，0~9
//                                       FUCHSIA,  //颜色
//                                       1,  //线条宽度
//                                       AIM_X + 80,  //起点 x 坐标
//                                       AIM_Y + 50,  //起点 y 坐标
//                                       AIM_X + 30,  //终点 x 坐标
//                                       AIM_Y - 80);	
	
	client_send_seven_graphic(Group2);
}

/*发送可通过线*/
void UI_send_Group3(void)
{
  static int operate_tpye = ADD;
	
	/*左斜*/
	Group3.grapic_data_struct[0] = draw_line("g30",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       WHITE,  //颜色
                                       1,  //线条宽度
                                       Client_mid_position_x - 230,  //起点 x 坐标
                                       Client_mid_position_y - 60,  //起点 y 坐标
                                       Client_mid_position_x - 520,  //终点 x 坐标
                                       0);
	
	/*右斜*/
  Group3.grapic_data_struct[1] = draw_line("g31",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       WHITE,  //颜色
                                       1,  //线条宽度
                                       Client_mid_position_x + 230,  //起点 x 坐标
                                       Client_mid_position_y - 60,  //起点 y 坐标
                                       Client_mid_position_x + 520,  //终点 x 坐标
                                       0);

  /*横零*/
  Group3.grapic_data_struct[2] = draw_line("g32",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       WHITE,  //颜色
                                       2,  //线条宽度
                                       Client_mid_position_x - 160,  //起点 x 坐标
                                       Client_mid_position_y,  //起点 y 坐标
                                       Client_mid_position_x + 160,  //终点 x 坐标
                                       Client_mid_position_y);
	/*准心 10.5m*/																		 
	Group3.grapic_data_struct[3] = draw_line("g33",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       BLACK,  //颜色
                                       2,  //线条宽度
                                       AIM_X - 60,  //起点 x 坐标
                                       AIM_Y - 120,  //起点 y 坐标
                                       AIM_X + 60,  //终点 x 坐标
                                       AIM_Y - 120);
																			 
	Group3.grapic_data_struct[4] = draw_line("g34",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       GREEN,  //颜色
                                       2,  //线条宽度
                                       AIM_X - 15,  //起点 x 坐标
                                       AIM_Y - 110,  //起点 y 坐标
                                       AIM_X + 15,  //终点 x 坐标
                                       AIM_Y - 110);
																			 
	Group3.grapic_data_struct[5] = draw_line("g35",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       GREEN,  //颜色
                                       2,  //线条宽度
                                       AIM_X - 15,  //起点 x 坐标
                                       AIM_Y - 130,  //起点 y 坐标
                                       AIM_X + 15,  //终点 x 坐标
                                       AIM_Y - 130);										
										
	Group3.grapic_data_struct[6] = draw_int("g36",  //图形名
	                                    operate_tpye,  //图形操作
                                      1,  //图层数，0~9
                                      GREEN,  //颜色
                                      10,  //字体大小
                                      2,  //线条宽度
                                      AIM_X + 65,  //起点 x 坐标
                                      AIM_Y - 115,  //起点 y 坐标
                                      (int32_t)11);  //32 位整型数，int32_t									
											
	client_send_seven_graphic(Group3);
}

/*发送各种状态标志位*/
void UI_send_Group4(void)
{
	static int cnt = 0;
  static int operate_tpye = ADD;
	uint8_t vision_color = 0;
//	uint8_t top_color = 0;
	uint8_t fric_color = 0;
  
  if(cnt == 0)
  {
    operate_tpye = ADD;  //一秒有一次是ADD
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
	
	Group4.grapic_data_struct[0] = draw_circle("g40",  //图形名
	                                     operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       vision_color,  //颜色
                                       2,  //线条宽度
                                       Client_mid_position_x - 610,  //圆心 x 坐标
                                       Client_mid_position_y + 75,  //圆心 y 坐标
                                       13);  //半径
	
	Group4.grapic_data_struct[1] = draw_circle("g41",  //图形名
	                                     operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       vision_color,  //颜色
                                       2,  //线条宽度
                                       Client_mid_position_x - 610,  //圆心 x 坐标
                                       Client_mid_position_y + 75,  //圆心 y 坐标
                                       22);  //半径
	
	
	
	if(global_receive.stop_hit != 0)
	{
		  Group4.grapic_data_struct[2] = draw_line("g42",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       FUCHSIA,  //颜色
                                       5,  //线条宽度
                                       Client_mid_position_x - 200,  //起点 x 坐标
                                       Client_mid_position_y - 280,  //起点 y 坐标
                                       Client_mid_position_x + 200,  //终点 x 坐标
                                       Client_mid_position_y - 280);
	}
	else
	{
		Group4.grapic_data_struct[2] = draw_line("g42",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       GREEN,  //颜色
                                       0,  //线条宽度
                                       Client_mid_position_x - 150,  //起点 x 坐标
                                       Client_mid_position_y - 200,  //起点 y 坐标
                                       Client_mid_position_x + 150,  //终点 x 坐标
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
//  Group4.grapic_data_struct[2] = draw_circle("g42",  //图形名
//	                                     operate_tpye,  //图形操作
//                                       1,  //图层数，0~9
//                                       top_color,  //颜色
//                                       2,  //线条宽度
//                                       Client_mid_position_x - 610,  //圆心 x 坐标
//                                       Client_mid_position_y + 187,  //圆心 y 坐标
//                                       13);  //半径
//	
//	Group4.grapic_data_struct[3] = draw_circle("g43",  //图形名
//	                                     operate_tpye,  //图形操作
//                                       1,  //图层数，0~9
//                                       top_color,  //颜色
//                                       2,  //线条宽度
//                                       Client_mid_position_x - 610,  //圆心 x 坐标
//                                       Client_mid_position_y + 187,  //圆心 y 坐标
//                                       22);  //半径
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
	
  Group4.grapic_data_struct[4] = draw_circle("g44",  //图形名
	                                     operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       fric_color,  //颜色
                                       2,  //线条宽度
                                       Client_mid_position_x - 610,  //圆心 x 坐标
                                       Client_mid_position_y + 19,  //圆心 y 坐标
                                       13);  //半径
	
	Group4.grapic_data_struct[5] = draw_circle("g45",  //图形名
	                                     operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       fric_color,  //颜色
                                       2,  //线条宽度
                                       Client_mid_position_x - 610,  //圆心 x 坐标
                                       Client_mid_position_y + 19,  //圆心 y 坐标
                                       22);  //半径
	
//	/*pitch 角度*/
//	Group4.grapic_data_struct[6] = draw_float("g46",  //图形名
//	                                    operate_tpye,  //图形操作
//                                      1,  //图层数，0~9
//                                      WHITE,  //颜色
//                                      24,  //字体大小
//                                      2,  //小数位有效个数
//                                      2,  //线条宽度
//                                      Client_mid_position_x - 800,  //起点 x 坐标
//                                      Client_mid_position_y + 245,  //起点 y 坐标
//                                      (int32_t)((global_receive.pitch_angle) * 1000));//乘以 1000 后，以 32 位整型数，int32_t

  Group4.grapic_data_struct[6] = draw_int("g46",  //图形名
	                                    operate_tpye,  //图形操作
                                      1,  //图层数，0~9
                                      YELLOW,  //颜色
                                      24,  //字体大小
                                      2,  //线条宽度
                                      Client_mid_position_x - 800,  //起点 x 坐标
                                      Client_mid_position_y + 245,  //起点 y 坐标
                                      (int32_t)global_detect_number);  //32 位整型数，int32_t		
																			 
	client_send_seven_graphic(Group4);
  
  cnt++;
  cnt %= 10;
}

void UI_send_Group5(void)
{
  static int operate_tpye = ADD;
	
	/*准心 4m*/																		 
	Group5.grapic_data_struct[0] = draw_line("g50",  //图形名
                                       operate_tpye,  //图形操作
                                       1,  //图层数，0~9
                                       WHITE,  //颜色
                                       2,  //线条宽度
                                       AIM_X - 80,  //起点 x 坐标
                                       AIM_Y + 30,  //起点 y 坐标
                                       AIM_X + 80,  //终点 x 坐标
                                       AIM_Y + 30);
	
	Group5.grapic_data_struct[1] = draw_int("g51",  //图形名
	                                    operate_tpye,  //图形操作
                                      1,  //图层数，0~9
                                      YELLOW,  //颜色
                                      10,  //字体大小
                                      2,  //线条宽度
                                      AIM_X + 85,  //起点 x 坐标
                                      AIM_Y + 35,  //起点 y 坐标
                                      (int32_t)4);  //32 位整型数，int32_t		
																			
																			
																			 						
											
	client_send_seven_graphic(Group5);
}

/************************************************************************************************************************************************/






