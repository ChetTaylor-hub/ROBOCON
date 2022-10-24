#include "remote_control.h"
#include "usart.h"
#include "posture.h"
#include "key.h"
#include "combinations.h"
#include "command.h"
#include "delay.h"
#include "task.h"
#include "navigation.h"
#include "inv_mpu.h"
#include "mpu9250.h"
#include "bsp_uart.h"
#include "jump.h"
#define PI 3.14159265358979f
int StartFlag=0;
float step_len_throttle;
float step_len_throttle_yaw;
float rc_params_l,rc_params_r;
u8 pskey=1;


float step_len_rotate_angle;


void rc_params_clean(void)
{
    step_len_throttle=0;
    step_len_throttle_yaw=0;
    step_len_rotate_angle=0;
    state_gait_params[ROTAT_LEFT].step_length=0;
    RcDetachedParam.detached_params_0.step_length=0;
    RcDetachedParam.detached_params_2.step_length=0;
    RcDetachedParam.detached_params_1.step_length=0;
    RcDetachedParam.detached_params_3.step_length=0;
}

void Rc_task(void *pvParameters)
{
	u8 b;
	int t=0;
	u8 flag_aj=7;
	float pitch,roll,yaw; 	        //ŷ����
	short gyrox,gyroy,gyroz;        //������ԭʼ����
  int nump=0;
	for(;;)
	{
		t++;
		if(mpu_mpl_get_data(&pitch,&roll,&yaw)==0)
		{   
//		mpu_mpl_get_data(&pitch,&roll,&yaw);
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ����������
	  imuinfo.ActVal[0]=yaw;
//			printf("%f\r\n",yaw);
//			LED0=!LED0;
		}
//		if(t%500==0)
//		{t=0;}
		if(ps_key[0]==1&&nump==0)
		{
			  state=START;
			  climbing_offset_flag=NO;
				yaw_set=yaw;
			  nump=1;
		}
		if(ps_key[0]==1)//suishi
		{
			tri_mode=1;
		}
		if(ps_key[1]==1)
		{
			tri_mode=0;
		}

///////////////		
			if(ps_key[7]==1)  //21cm�첽
			{
			  pid_spd_out_limit=3000;//ԭ 3000
			  climbing_offset_flag=NO;
			  state=TORT2;
				LinearCorrection=normalTROT2_correction;
				yaw_set=yaw;
			}

			else if(ps_key[8]==1) //25cm��ͣ-->��Ϊ15cm��ͣ
		  {
		    climbing_offset_flag=NO;
			  state=STOP25;
		  }
			else if(ps_key[15]==1) //21cm��ͣ
		  {
//			rc_params_clean();
		    climbing_offset_flag=NO;
			  state=STOP;
		  }
			else if(ps_key[14]==1&&tri_mode==0)  //Ƶ��3.0������
			{
			  pid_spd_out_limit=3000;
			  climbing_offset_flag=NO;
			  state=TROT;
				LinearCorrection=normalTROT_correction;
				yaw_set=yaw;

			}
			else if(ps_key[2]==1&&tri_mode==0)//21cm��ת-ԭ��
		  {
			  state=STOP_TAIJIE;
			  climbing_offset_flag=NO;
		  }	
			else if(ps_key[3]==1&&tri_mode==0)//21cm��ת-ԭ��
		  {
			  state=STOP18;
			  climbing_offset_flag=NO;
		  }	
		  else if(ps_key[4]==1&&tri_mode==0)     //�Ź���Ծ
		  {
				pid_spd_out_limit=8000;
			  climbing_offset_flag=NO;
			  StartJumpshuiping(HAL_GetTick());
//			  state=WALK_BACK;
//			  climbing_offset_flag=NO;
		  }
			else if(ps_key[5]==1&&tri_mode==0)//21cm��ת-ԭ��
		  {
			  state=TEST10;
			  climbing_offset_flag=NO;
		  }	
		  else if(ps_key[6]==1&&tri_mode==0)     //21cm��ת-ԭ��
		  {
			  state=TEST11;
			  climbing_offset_flag=NO;
		  }
			
			vTaskDelay(5);
	}

}


