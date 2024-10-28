#ifndef __JUDGE_POTOCOL_H
#define __JUDGE_POTOCOL_H

#include "stm32f4xx_hal.h"

/* 帧头 */
typedef struct 
{
	uint8_t SOF;  //数据帧起始字节，固定值为 0xA5
	uint16_t data_length;  //数据帧中 data 的长度
	uint8_t seq;  //包序号
	uint8_t CRC8;  //帧头 CRC8 校验
}judge_frame_header_t;

typedef struct 
{
	judge_frame_header_t *frame_header;
	uint16_t cmd_id;
	uint16_t frame_tail;
}drv_judge_info_t;

extern drv_judge_info_t drv_judge_info;

void judge_recive(uint8_t *rxBuf);

/********************from裁判系统串口协议附录V1.3的数据结构体********************/

/* 比赛状态数据：0x0001。发送频率：1Hz */
typedef struct
{
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
	uint64_t SyncTimeStamp;
} ext_game_status_t;

/* 比赛结果数据：0x0002。发送频率：比赛结束后发送 */
typedef struct
{
	uint8_t winner;
} ext_game_result_t;

/* 机器人血量数据：0x0003。发送频率：1Hz */
typedef struct
{
	uint16_t red_1_robot_HP;
	uint16_t red_2_robot_HP; 
	uint16_t red_3_robot_HP; 
	uint16_t red_4_robot_HP; 
	uint16_t red_5_robot_HP; 
	uint16_t red_7_robot_HP; 
	uint16_t red_outpost_HP;
	uint16_t red_base_HP; 
	uint16_t blue_1_robot_HP; 
	uint16_t blue_2_robot_HP; 
	uint16_t blue_3_robot_HP; 
	uint16_t blue_4_robot_HP; 
	uint16_t blue_5_robot_HP; 
	uint16_t blue_7_robot_HP; 
	uint16_t blue_outpost_HP;
	uint16_t blue_base_HP;
} ext_game_robot_HP_t;

/* 人工智能挑战赛加成\惩罚区分布与潜伏模式状态：0x0005。发送频率：1Hz 周期发送，发送范围：所有机器人 */
typedef struct
{
	uint8_t F1_zone_status:1;
	uint8_t F1_zone_buff_debuff_status:3; 
	uint8_t F2_zone_status:1;
	uint8_t F2_zone_buff_debuff_status:3; 
	uint8_t F3_zone_status:1;
	uint8_t F3_zone_buff_debuff_status:3; 
	uint8_t F4_zone_status:1;
	uint8_t F4_zone_buff_debuff_status:3; 
	uint8_t F5_zone_status:1;
	uint8_t F5_zone_buff_debuff_status:3; 
	uint8_t F6_zone_status:1;
	uint8_t F6_zone_buff_debuff_status:3;
	uint16_t red1_bullet_left;
	uint16_t red2_bullet_left;
	uint16_t blue1_bullet_left;
	uint16_t blue2_bullet_left;
	uint8_t lurk_mode;
	uint8_t res;
} ext_ICRA_buff_debuff_zone_status_t;

/* 场地事件数据：0x0101。发送频率：1Hz */
typedef __packed struct
{
	uint32_t event_type;
} ext_event_data_t;

/* 补给站动作标识：0x0102。发送频率：动作改变后发送, 发送范围：己方机器人 */
typedef __packed struct
{
	uint8_t supply_projectile_id; 
	uint8_t supply_robot_id; 
	uint8_t supply_projectile_step; 
	uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

/* 裁判警告信息：cmd_id (0x0104)。发送频率：己方警告发生后发送 */
typedef __packed struct
{
	uint8_t level;
	uint8_t foul_robot_id; 
} ext_referee_warning_t;

/* 飞镖发射口倒计时：cmd_id (0x0105)。发送频率：1Hz 周期发送，发送范围：己方机器人 */
typedef __packed struct
{
	uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;

/* 比赛机器人状态：0x0201。发送频率：10Hz */
typedef __packed struct
{
	uint8_t robot_id;
	uint8_t robot_level;
	uint16_t remain_HP;
	uint16_t max_HP;
	uint16_t shooter_id1_17mm_cooling_rate;
	uint16_t shooter_id1_17mm_cooling_limit;
	uint16_t shooter_id1_17mm_speed_limit;
	uint16_t shooter_id2_17mm_cooling_rate;
	uint16_t shooter_id2_17mm_cooling_limit;
	uint16_t shooter_id2_17mm_speed_limit;
	uint16_t shooter_id1_42mm_cooling_rate;
	uint16_t shooter_id1_42mm_cooling_limit;
	uint16_t shooter_id1_42mm_speed_limit;
	uint16_t chassis_power_limit;
	uint8_t mains_power_gimbal_output : 1;
	uint8_t mains_power_chassis_output : 1;
	uint8_t mains_power_shooter_output : 1;
} ext_game_robot_status_t;

/* 实时功率热量数据：0x0202。发送频率：50Hz */
typedef __packed struct
{
	uint16_t chassis_volt; 
	uint16_t chassis_current; 
	float chassis_power;
	uint16_t chassis_power_buffer; 
	uint16_t shooter_id1_17mm_cooling_heat;
	uint16_t shooter_id2_17mm_cooling_heat;
	uint16_t shooter_id1_42mm_cooling_heat;
} ext_power_heat_data_t;

/* 机器人位置：0x0203。发送频率：10Hz */
typedef __packed struct
{
	float x;
	float y;
	float z;
	float yaw;
} ext_game_robot_pos_t;

/* 机器人增益：0x0204。发送频率：1Hz */
typedef __packed struct
{
	uint8_t power_rune_buff;
}ext_buff_t;

/* 空中机器人能量状态：0x0205。发送频率：10Hz */
typedef __packed struct
{
	uint8_t attack_time;
} aerial_robot_energy_t;

/* 伤害状态：0x0206。发送频率：伤害发生后发送 */
typedef __packed struct
{
	uint8_t armor_id : 4;
	uint8_t hurt_type : 4;
} ext_robot_hurt_t;

/* 实时射击信息：0x0207。发送频率：射击后发送 */
typedef __packed struct
{
	uint8_t bullet_type;
	uint8_t shooter_id;
	uint8_t bullet_freq;
	float bullet_speed;
} ext_shoot_data_t;

/* 子弹剩余发射数：0x0208。发送频率：10Hz 周期发送，所有机器人发送 */
typedef __packed struct
{
	uint16_t bullet_remaining_num_17mm;
	uint16_t bullet_remaining_num_42mm;
	uint16_t coin_remaining_num;
} ext_bullet_remaining_t;

/* 机器人 RFID 状态：0x0209。发送频率：1Hz，发送范围：单一机器人 */
typedef __packed struct
{
	uint32_t rfid_status;
} ext_rfid_status_t;

/* 飞镖机器人客户端指令数据：0x020A。发送频率：10Hz，发送范围：单一机器人 */
typedef __packed struct
{
	uint8_t dart_launch_opening_status;
	uint8_t dart_attack_target;
	uint16_t target_change_time;
	uint16_t operate_launch_cmd_time;
} ext_dart_client_cmd_t;

/********************命令码枚举********************/
enum
{
	ID_game_state       						= 0x0001,//比赛状态数据，1Hz
	ID_game_result 	   							= 0x0002,//比赛结果数据，比赛结束发送
	ID_game_robot_HP       					= 0x0003,//比赛机器人血量数据，1Hz发送
	ID_dart_status									= 0x0004,//飞镖发射状态，飞镖发射时发送
	ID_ICRA_buff_debuff_zone_status = 0x0005,//人工智能挑战赛加成与惩罚区状态，1Hz
	ID_event_data  									= 0x0101,//场地事件数据，1Hz
	ID_supply_projectile_action   	= 0x0102,//场地补给站动作标识数据
	ID_referee_warning					 		= 0x0104,//裁判警告数据，警告后发送
	ID_dart_remaining_time					= 0x0105,//飞镖发射口倒计时，1Hz
	ID_game_robot_state    					= 0x0201,//机器人状态数据，10Hz
	ID_power_heat_data    					= 0x0202,//实时功率热量数据，50Hz
	ID_game_robot_pos        				= 0x0203,//机器人位置数据，10Hz
	ID_buff_musk										= 0x0204,//机器人增益数据，1Hz
	ID_aerial_robot_energy					= 0x0205,//空中机器人能量状态数据，10Hz，只有空中机器人主控发送
	ID_robot_hurt										= 0x0206,//伤害状态数据，伤害发生后发送
	ID_shoot_data										= 0x0207,//实时射击数据，子弹发射后发送
	ID_bullet_remaining							= 0x0208,//弹丸剩余发送数，仅空中机器人，哨兵机器人以及ICRA机器人发送，1Hz
	ID_rfid_status									= 0x0209,//机器人RFID状态，1Hz
	ID_interactive_header_data			= 0x0301 //机器人交互数据，发送方触发发送
};

/********************长度枚举********************/
enum
{
	LEN_FRAME_HEAD 	                = 5,	    // 帧头长度
	LEN_CMD_ID 		                  = 2,	    // 命令码长度
	LEN_FRAME_TAIL 	                = 2,	    // 帧尾CRC16
	LEN_game_state       						= 3,			//0x0001,//比赛状态数据，1Hz
	LEN_game_result 	   						= 1,			//0x0002,//比赛结果数据，比赛结束发送
	LEN_game_robot_HP       				= 32,			//0x0003,//比赛机器人血量数据，1Hz发送
	LEN_dart_status									= 3,			//0x0004,//飞镖发射状态，飞镖发射时发送
	LEN_ICRA_buff_debuff_zone_status= 3,			//0x0005,//人工智能挑战赛加成与惩罚区状态，1Hz
	LEN_event_data  								= 4,			//0x0101,//场地事件数据，1Hz
	LEN_supply_projectile_action   	= 4,			//0x0102,//场地补给站动作标识数据
	LEN_referee_warning					 		= 2,			//0x0104,//裁判警告数据，警告后发送
	LEN_dart_remaining_time					= 1,			//0x0105,//飞镖发射口倒计时，1Hz
	LEN_game_robot_state    				= 27,			//0x0201,//机器人状态数据，10Hz
	LEN_power_heat_data    					= 16,			//0x0202,//实时功率热量数据，50Hz
	LEN_game_robot_pos        			= 16,			//0x0203,//机器人位置数据，10Hz
	LEN_buff_musk										= 1,			//0x0204,//机器人增益数据，1Hz
	LEN_aerial_robot_energy					= 3,			//0x0205,//空中机器人能量状态数据，10Hz，只有空中机器人主控发送
	LEN_robot_hurt									= 1,			//0x0206,//伤害状态数据，伤害发生后发送
	LEN_shoot_data									= 7,			//0x0207,//实时射击数据，子弹发射后发送
	LEN_bullet_remaining						= 2,			//0x0208,//弹丸剩余发送数，仅空中机器人，哨兵机器人以及ICRA机器人发送，1Hz
	LEN_rfid_status									= 4				//0x0209,//机器人RFID状态，1Hz
//	LEN_interactive_header_data			= n			//0x0301 //机器人交互数据，发送方触发发送
};

void judge_receive(uint8_t *rxbuf);

#endif
