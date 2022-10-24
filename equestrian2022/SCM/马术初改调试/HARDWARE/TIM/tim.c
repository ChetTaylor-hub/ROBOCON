#include "tim.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdlib.h"
TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 

void TIM3_Init(u16 arr,u16 psc)
{
	TIM3_Handler.Instance=TIM3;//通用定时器3
	TIM3_Handler.Init.Prescaler=psc; //分频系数
	TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//向上计数器
	TIM3_Handler.Init.Period=arr;//自动装载值
	TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
	HAL_TIM_Base_Init(&TIM3_Handler);
	
	HAL_TIM_Base_Start_IT(&TIM3_Handler);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
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
		*Ptr ++ = '0';            //???????�0�
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
			if(tim2_js==1)//20ms 向遥控器发送连接数据
			{
				tim2_js=0;
				if(tri_mode==0)
				{
					HAL_UART_Transmit(&UART2_Handler,"i",1,1000);	//发送数据"i" ，目的保持连接
					while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);		//等待发送结束
				}
				if(tri_mode==1)//按键0按下
				{
					HAL_UART_Transmit(&UART2_Handler,ch,1,1000);	//发送数据"k"
					while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);		//等待发送结束
				}
			}
			if(ps_tim==50)//1s  遥控器无接收扫描，串口2接收到遥控器发的数据，ps_tim清零
			{
				 ps_tim=0;//
				 LED0=~LED0;
			}
		}
}
