#include "navigation.h"
#include "posture.h"
#include "bsp_uart.h"
#include "inv_mpu.h"
#include "mpu9250.h"
int LinearCorrection=Deny;

int BalanceCorrection=Deny;

int _Sensor[10];
float step_len_initial = 10;//����
float step_high_initial = 16;//����

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
float pitch,roll,yaw; 	        //ŷ����
short gyrox,gyroy,gyroz;        //������ԭʼ���� 
float normal_params_l,normal_params_r;

/*��̬����*/

void Navi_task(void *pvParameters)//mpu9250���ᴫ���� ������
{
   for(;;)
		navigation_execute();
}

void navigation_execute(void)//***********ֱ�߾���
{
	 float _dev_limit = 8.0;   //10
	 float qiao_limit=4.0;
	 float taijie_limit=4.0;

				if(LinearCorrection==normalWALK_correction)		//ֱ�߽���
				{
						step_len_dev=pid_calc(&pid_imu[2],imuinfo.ActVal[0]/1.0f,yaw_set/1.0f);  //����������yaw��Ƕȣ�����������Ȳ����� ���ֽǶ�

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
				
				if(LinearCorrection==normalTROT_correction)		//ֱ�߽���
				{
						step_len_dev=pid_calc(&pid_imu[2],imuinfo.ActVal[0]/1.0f,yaw_set/1.0f);  //����������yaw��Ƕȣ�����������Ȳ����� ���ֽǶ�

						float _dev_limit = 10.0;//14
						if(step_len_dev>_dev_limit)	step_len_dev=_dev_limit;
						else if(step_len_dev<-_dev_limit)	step_len_dev=-_dev_limit;

						normal_params_l=step_len_initial+step_len_dev;//����Ϊ+
						normal_params_r=step_len_initial-step_len_dev;//����Ϊ-
						if(normal_params_l<0) normal_params_l=0;
						if(normal_params_r<0) normal_params_r=0;
						state_detached_params[TROT].detached_params_0.step_length=normal_params_l;
						state_detached_params[TROT].detached_params_2.step_length=normal_params_l;
						state_detached_params[TROT].detached_params_1.step_length=normal_params_r;
						state_detached_params[TROT].detached_params_3.step_length=normal_params_r;				

				}
				
	

		

				



				


				if(LinearCorrection==normalTROT2_correction)		//ֱ�߽���
				{
						step_len_dev=pid_calc(&pid_imu[2],imuinfo.ActVal[0]/1.0f,yaw_set/1.0f);  //����������yaw��Ƕȣ�����������Ȳ����� ���ֽǶ�

						float _dev_limit = 10.0;//14
						if(step_len_dev>_dev_limit)	step_len_dev=_dev_limit;
						else if(step_len_dev<-_dev_limit)	step_len_dev=-_dev_limit;

						normal_params_l=step_len_initial+step_len_dev;//����Ϊ+
						normal_params_r=step_len_initial-step_len_dev;//����Ϊ-
						if(normal_params_l<0) normal_params_l=0;
						if(normal_params_r<0) normal_params_r=0;
						state_detached_params[TORT2].detached_params_0.step_length=normal_params_l;
						state_detached_params[TORT2].detached_params_2.step_length=normal_params_l;
						state_detached_params[TORT2].detached_params_1.step_length=normal_params_r;
						state_detached_params[TORT2].detached_params_3.step_length=normal_params_r;

				}
			

			vTaskDelay(20);
		

}


