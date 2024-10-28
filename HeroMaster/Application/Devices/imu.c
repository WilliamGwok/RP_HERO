/**
  ******************************************************************************
  * @file           : imu.c\h
  * @brief          : 
  * @note           : finish 2022-2-13 19:59:24
  ******************************************************************************
  */
	
#include "imu.h"
#include "config_imu.h"
#include "bmi.h"
#include "stdbool.h"
#include "math_support.h"
#include "ave_filter.h"

extern float Kp;
extern float kp;

bool imu_init_Y_O_N = false;

imu_config_t imu_config = 
{
	.dif_speed_fil_times = dif_speed_fil_times_init,
	.Kp = Kp_init,
	.kp = kp_init,
	.gyro_lowpass = gyro_lowpass_init,
	.acce_lowpass = acce_lowpass_init,
};
imu_base_info_t imu_base_info;
imu_info_t imu_info;
imu_t imu = 
{
	.base_info = &imu_base_info,
	.config = &imu_config,
	.info = &imu_info,
};

ave_filter_t imu_pitch_dif_speed_ave_filter;
ave_filter_t imu_roll_dif_speed_ave_filter;
ave_filter_t imu_yaw_dif_speed_ave_filter;

/**
  * @brief  
  * @param  
  * @retval 
  */
void imu_init(imu_t *imu)
{
	imu_info_init(imu->info);
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void imu_info_init(imu_info_t *info)
{
	info->init_cnt = 0;
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void imu_update(imu_t *imu)
{
	imu_base_info_t *base_info = imu->base_info;
	imu_info_t *info = imu->info;
	
	/* 陀螺仪初始化滴答计时 */
	if(info->init_cnt < imu_init_time)
	{
		info->init_cnt++;
	}
	else if(imu_init_Y_O_N == false)
	{
		Kp = imu->config->Kp;
		imu_init_Y_O_N = true;
	}
	
	/* 获取陀螺仪数据 */
	BMI_Get_RawData(&base_info->ggx,&base_info->ggy,&base_info->ggz,&base_info->aax,&base_info->aay,&base_info->aaz);

	/* 原始数据低通滤波 */
	base_info->ggx_K = lowpass(base_info->ggx_K, base_info->ggx, imu->config->gyro_lowpass);
	base_info->ggy_K = lowpass(base_info->ggy_K, base_info->ggy, imu->config->gyro_lowpass);
	base_info->ggz_K = lowpass(base_info->ggz_K, base_info->ggz, imu->config->gyro_lowpass);
	base_info->aax_K = lowpass(base_info->aax_K, base_info->aax, 1);//imu->config->acce_lowpass);
	base_info->aay_K = lowpass(base_info->aay_K, base_info->aay, 1);//imu->config->acce_lowpass);
	base_info->aaz_K = lowpass(base_info->aaz_K, base_info->aaz, 1);//imu->config->acce_lowpass);

	/* 解算陀螺仪数据 */
	BMI_Get_EulerAngle(&base_info->pitch,  &base_info->roll,  &base_info->yaw,\
	                   &base_info->pitch_, &base_info->roll_, &base_info->yaw_,\
	                   &base_info->ggx_K,  &base_info->ggy_K, &base_info->ggz_K,\
	                   &base_info->aax_K,  &base_info->aay_K, &base_info->aaz_K);
	
	/* 计算陀螺仪数据 */
	//pitch
	base_info->pitch_dif_speed = base_info->pitch_;// - base_info->pitch_last_;
	if(abs(base_info->pitch_dif_speed) > 180)//跳变角度处检查
	{
		base_info->pitch_dif_speed -= one(base_info->pitch_dif_speed) * 360;
	}
	base_info->pitch_dif_speed *= 1000.f;
	base_info->pitch_dif_speed_ave = ave_fil_update(&imu_pitch_dif_speed_ave_filter,base_info->pitch_dif_speed,imu->config->dif_speed_fil_times);
	base_info->pitch_last_ = base_info->pitch_;
	//roll
	base_info->roll_dif_speed = base_info->roll_ - 180;// - base_info->roll_last_;
	if(abs(base_info->roll_dif_speed) > 180)//跳变角度处检查
	{
		base_info->roll_dif_speed -= one(base_info->roll_dif_speed) * 360;
	}
	base_info->roll_dif_speed *= 1000.f;
	base_info->roll_dif_speed_ave = ave_fil_update(&imu_roll_dif_speed_ave_filter,base_info->roll_dif_speed,imu->config->dif_speed_fil_times);
	base_info->roll_last_ = base_info->roll_;
	//yaw
	base_info->yaw_dif_speed = base_info->yaw_;// - base_info->yaw_last_;
	if(abs(base_info->yaw_dif_speed) > 180)//跳变角度处检查
	{
		base_info->yaw_dif_speed -= one(base_info->yaw_dif_speed) * 360;
	}
	base_info->yaw_dif_speed *= 1000.f;
	base_info->yaw_dif_speed_ave = ave_fil_update(&imu_yaw_dif_speed_ave_filter,base_info->yaw_dif_speed,imu->config->dif_speed_fil_times);
	base_info->yaw_last_ = base_info->yaw_;
}
