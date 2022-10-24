#include "navigation.h"
#include "posture.h"
#include "bsp_uart.h"
#include "inv_mpu.h"
#include "mpu9250.h"
int LinearCorrection=Deny;

int BalanceCorrection=Deny;

int _Sensor[10];
float step_len_initial = 10;//步长
float step_high_initial = 16;//步高

float step_len_dev=0.0;
float step_high_dev=0.0;
float flight_percent_dev=0.0;

float yaw_now=0.0;
float yaw_set=0.0;

float roll_set=0.0;
float pitch_set=0.0;

float roll_offset=0.0;
float pitch_offset=0.0;

float _dev_angel;

int stage=0 ;

int _count_navi;
float pitch,roll,yaw; 	        //欧拉角
short gyrox,gyroy,gyroz;        //陀螺仪原始数据 
float normal_params_l,normal_params_r;

/*姿态矫正*/

void Navi_task(void *pvParameters)//mpu9250九轴传感器 陀螺仪
{
   for(;;)
		navigation_execute();
}

void navigation_execute(void)//***********直线纠正
{
	 float _dev_limit = 8.0;   //10
	 float qiao_limit=4.0;
	 float taijie_limit=4.0;

				if(LinearCorrection==normalWALK_correction)		//直线矫正
				{
						step_len_dev=pid_calc(&pid_imu[2],imuinfo.ActVal[0]/1.0f,yaw_set/1.0f);  //计算陀螺仪yaw轴角度，输出给两腿腿部差速 保持角度

						float _dev_limit = 10.0;//14
						if(step_len_dev>_dev_limit)	step_len_dev=_dev_limit;
						else if(step_len_dev<-_dev_limit)	step_len_dev=-_dev_limit;

						normal_params_l=step_len_initial+step_len_dev;
						normal_params_r=step_len_initial-step_len_dev;
						if(normal_params_l<0) normal_params_l=0;
						if(normal_params_r<0) normal_params_r=0;
						state_detached_params[WALK].detached_params_0.step_length=normal_params_l;
						state_detached_params[WALK].detached_params_2.step_length=normal_params_l;
						state_detached_params[WALK].detached_params_1.step_length=normal_params_r;
						state_detached_params[WALK].detached_params_3.step_length=normal_params_r;
					
				}
				
				if(LinearCorrection==normalTROT_correction)		//直线矫正
				{
						step_len_dev=pid_calc(&pid_imu[2],imuinfo.ActVal[0]/1.0f,yaw_set/1.0f);  //计算陀螺仪yaw轴角度，输出给两腿腿部差速 保持角度

						float _dev_limit = 10.0;//14
						if(step_len_dev>_dev_limit)	step_len_dev=_dev_limit;
						else if(step_len_dev<-_dev_limit)	step_len_dev=-_dev_limit;

						normal_params_l=step_len_initial+step_len_dev;//正走为+
						normal_params_r=step_len_initial-step_len_dev;//正走为-
						if(normal_params_l<0) normal_params_l=0;
						if(normal_params_r<0) normal_params_r=0;
						state_detached_params[TROT].detached_params_0.step_length=normal_params_l;
						state_detached_params[TROT].detached_params_2.step_length=normal_params_l;
						state_detached_params[TROT].detached_params_1.step_length=normal_params_r;
						state_detached_params[TROT].detached_params_3.step_length=normal_params_r;				

				}
				
	

		

				



				


				if(LinearCorrection==normalTROT2_correction)		//直线矫正
				{
						step_len_dev=pid_calc(&pid_imu[2],imuinfo.ActVal[0]/1.0f,yaw_set/1.0f);  //计算陀螺仪yaw轴角度，输出给两腿腿部差速 保持角度

						float _dev_limit = 10.0;//14
						if(step_len_dev>_dev_limit)	step_len_dev=_dev_limit;
						else if(step_len_dev<-_dev_limit)	step_len_dev=-_dev_limit;

						normal_params_l=step_len_initial+step_len_dev;//正走为+
						normal_params_r=step_len_initial-step_len_dev;//正走为-
						if(normal_params_l<0) normal_params_l=0;
						if(normal_params_r<0) normal_params_r=0;
						state_detached_params[TORT2].detached_params_0.step_length=normal_params_l;
						state_detached_params[TORT2].detached_params_2.step_length=normal_params_l;
						state_detached_params[TORT2].detached_params_1.step_length=normal_params_r;
						state_detached_params[TORT2].detached_params_3.step_length=normal_params_r;

				}
			

			vTaskDelay(20);
		

}


