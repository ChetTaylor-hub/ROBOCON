//#include "led.h"
//#include "delay.h"
//#include "sys.h"
//#include "usart.h"
//#include "key.h"
//#include "adc.h"
//#include "oled.h"
//#include "timer.h"

////   2021  7.9  �޸�
// int main(void)
// {	
//	 u8 tri_time[2]={0};
//	 uint8_t txt[18]={"000000000000000000"};
//	 int right_x,right_y,left_x,left_y;	 
//	 	 delay_init();	    	 //��ʱ������ʼ��	  
//	 LED_Init();		  	 	//��ʼ����LED���ӵ�Ӳ���ӿ�
//	 TIM3_Int_Init(19,7199);//500hz�ļ���Ƶ�ʣ�2ms  7199
//	 uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600

//		KEY_Init();          	//��ʼ���밴�����ӵ�Ӳ���ӿ�
//		OLED_Init();			//��ʼ��OLED  
//		OLED_Clear(); 
//	 
//	again:
//		OLED_Clear(); 
//		dj_init=0;
//		dw_init=0;
//		End=0;
//		while(!Connection)//��������
//		{
//			OLED_ShowString(0,1,"Linking...",8);//
//			printf("sqqqqqqqqqqqqqqqqqqe"); //  18
//			dj_init=0;
//			dw_init=0;
//			End=0;
//		}
//		OLED_Clear(); 
//		while(1)
//		{
//			OLED_ShowString(0,1,"init:",4);//
//			if(dj_init)
//				OLED_ShowString(6,2,"dianji_ing...",4);//
//			if(dw_init)
//			{
//				OLED_ShowString(6,2,"dianji_ing...",4);//
//				OLED_ShowString(6,3,"dingwei_ing...",4);//
//				delay_ms(200);
//				break;
//			}
//			if(!Connection || End)//����ʧ�ܣ���������	
//				goto again;
//			
//		}
//		OLED_Clear(); 
//		mode=0;
//		while(1)
//		{
//			KEY_Scan(txt);
//			printf("s%se\r\n",txt);
//			if(tri_time[1]==0)
//			{
//				OLED_Clear();  
//			}
//			tri_time[1]=1;
//			tri_time[0]=0;
//			OLED_ShowString(30,1,"---------",15);
//			OLED_ShowString(30,2,"video:",15);
//			OLED_ShowNum(79,2,video,1,8);
//			OLED_ShowString(30,3,"---------",15);
//		}	
//}
// 

#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "adc.h"
#include "oled.h"
#include "timer.h"

//   2021  7.9  �޸�
 int main(void)
 {	
	 u8 tri_time[2]={0};
	 uint8_t txt[18]={"000000000000000000"};
	 int right_x,right_y,left_x,left_y;	 
	 delay_init();	    	 //��ʱ������ʼ��	  
	 LED_Init();		  	 	//��ʼ����LED���ӵ�Ӳ���ӿ�
	 TIM3_Int_Init(19,7199);//500hz�ļ���Ƶ�ʣ�2ms  7199
	 uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600

		KEY_Init();          	//��ʼ���밴�����ӵ�Ӳ���ӿ�
		OLED_Init();			//��ʼ��OLED  
		OLED_Clear(); 
	 
	again:
		OLED_Clear(); 
		dj_init=0;
		dw_init=0;
		End=0;
		while(!Connection)//��������
		{
			OLED_ShowString(0,1,"Linking...",8);//
			printf("sqqqqqqqqqqqqqqqqqqe"); //  18
			dj_init=0;
			dw_init=0;
			End=0;
		}
		OLED_Clear(); 
		while(1)
		{
			OLED_ShowString(0,1,"init:",4);//
			if(dj_init)
				OLED_ShowString(6,2,"dianji_ing...",4);//
			if(dw_init)
			{
				OLED_ShowString(6,2,"dianji_ing...",4);//
				OLED_ShowString(6,3,"dingwei_ing...",4);//
				delay_ms(200);
				break;
			}
			if(!Connection || End)//����ʧ�ܣ���������	
				goto again;
			
		}
		OLED_Clear(); 
		mode=0;
		while(1)
		{		
			KEY_Scan(txt);
			printf("s%se\r\n",txt);
		  if(tri_time[0]==0)
			{
				OLED_Clear();  
			}
			tri_time[0]=1;
			tri_time[1]=0  ;
			OLED_ShowString(30,1,"video:",15);
			OLED_ShowNum(79,1,video,1,8);
			OLED_ShowNum(15,1,txt[16]-'0',1,8);
			OLED_ShowNum(115,1,txt[17]-'0',1,8);
 
			OLED_ShowNum(10,2,txt[0]-'0',1,8);
			OLED_ShowNum(25,2,txt[1]-'0',1,8);
			OLED_ShowNum(40,2,txt[2]-'0',1,8);
			OLED_ShowNum(55,2,txt[3]-'0',1,8);
			
			OLED_ShowNum(70,2,txt[4]-'0',1,8);
			OLED_ShowNum(85,2,txt[5]-'0',1,8);
			OLED_ShowNum(100,2,txt[6]-'0',1,8);
			OLED_ShowNum(115,2,txt[7]-'0',1,8);
			 
			OLED_ShowNum(10,3,txt[8]-'0',1,8);
			OLED_ShowNum(25,3,txt[9]-'0',1,8);
			OLED_ShowNum(40,3,txt[10]-'0',1,8);
			OLED_ShowNum(55,3,txt[11]-'0',1,8);
			
			OLED_ShowNum(70,3,txt[12]-'0',1,8);
			OLED_ShowNum(85,3,txt[13]-'0',1,8);
			OLED_ShowNum(100,3,txt[14]-'0',1,8);
			OLED_ShowNum(115,3,txt[15]-'0',1,8);
		 
			if(!Connection || End)//����ʧ�ܣ���������	
				goto again;	
		}		
	
}
