#include "tim.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdlib.h"
TIM_HandleTypeDef TIM3_Handler;      //¶¨Ê±Æ÷¾ä±ú 

void TIM3_Init(u16 arr,u16 psc)
{
	TIM3_Handler.Instance=TIM3;//Í¨ÓÃ¶¨Ê±Æ÷3
	TIM3_Handler.Init.Prescaler=psc; //·ÖÆµÏµÊı
	TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//ÏòÉÏ¼ÆÊıÆ÷
	TIM3_Handler.Init.Period=arr;//×Ô¶¯×°ÔØÖµ
	TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//Ê±ÖÓ·ÖÆµÒò×Ó
	HAL_TIM_Base_Init(&TIM3_Handler);
	
	HAL_TIM_Base_Start_IT(&TIM3_Handler);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //Ê¹ÄÜTIM3Ê±ÖÓ
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //ÉèÖÃÖĞ¶ÏÓÅÏÈ¼¶£¬ÇÀÕ¼ÓÅÏÈ¼¶1£¬×ÓÓÅÏÈ¼¶3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //¿ªÆôITM3ÖĞ¶Ï   
	}
}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}

u8 tim2_js=0;
uint8_t* Int_To_Str(int x,uint8_t *Str)
{
  int t;
  uint8_t *Ptr,Buf[5];                 //???????????????,
  int i = 0;                             //????
  Ptr = Str;                           //??????????,??????,????????(????),
  if(x < 10)  // ?????10,???0x??
  {
		*Ptr ++ = '0';            //???????“0”
		*Ptr ++ = x+0x30;
	}
	else
	{
		while(x > 0)
		{
			t = x % 10;
			x = x / 10;
			Buf[i++] = t+0x30;  // ?????????ASCII???
		}
		i -- ;
		for(;i >= 0;i --)  // ?????????
		{
			*(Ptr++) = Buf[i];
		}
	}
	*Ptr = '\0';
	return Str;
}

//u8 ps_tim=0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM3_Handler))
	{
			tim2_js++;
			ps_tim++;
			if(tim2_js==1)//20ms ÏòÒ£¿ØÆ÷·¢ËÍÁ¬½ÓÊı¾İ
			{
				tim2_js=0;
				if(tri_mode==0)
				{
					HAL_UART_Transmit(&UART2_Handler,"i",1,1000);	//·¢ËÍÊı¾İ"i" £¬Ä¿µÄ±£³ÖÁ¬½Ó
					while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);		//µÈ´ı·¢ËÍ½áÊø
				}
				if(tri_mode==1)//°´¼ü0°´ÏÂ
				{
					HAL_UART_Transmit(&UART2_Handler,ch,1,1000);	//·¢ËÍÊı¾İ"k"
					while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);		//µÈ´ı·¢ËÍ½áÊø
				}
			}
			if(ps_tim==50)//1s  Ò£¿ØÆ÷ÎŞ½ÓÊÕÉ¨Ãè£¬´®¿Ú2½ÓÊÕµ½Ò£¿ØÆ÷·¢µÄÊı¾İ£¬ps_timÇåÁã
			{
				 ps_tim=0;//
				 LED0=~LED0;
			}
		}
}
