#include "usart.h"
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //os 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/9/7
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.0修改说明 
////////////////////////////////////////////////////////////////////////////////// 	  
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
u8 tri_mode=0;

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_RX_BUF2[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节
u8 USART_RX_BUF3[USART_REC_LEN];

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	
u16 USART2_RX_STA=0;       //接收状态标记	
u16 USART3_RX_STA=0; 
u16 hh=0;

u8 aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
u8 aRxBuffer2[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
u8 aRxBuffer3[RXBUFFERSIZE];//HAL库使用的串口接收缓冲

UART_HandleTypeDef UART1_Handler; //UART句柄
UART_HandleTypeDef UART2_Handler; //UART句柄
UART_HandleTypeDef UART3_Handler; //UART句柄

u8 USART3_START=0;
u8 ps[3]={0};

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{	
	//UART 初始化设置
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//初始化IO 串口1 
//bound:波特率
void uart2_init(u32 bound)
{	
	//UART 初始化设置
	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //波特率
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRxBuffer2, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

void uart3_init(u32 bound)
{	
	//UART 初始化设置
	UART3_Handler.Instance=USART3;					    //USART3
	UART3_Handler.Init.BaudRate=bound;				    //波特率
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART3_Handler);					    //HAL_UART_Init()会使能UART3
	
	HAL_UART_Receive_IT(&UART3_Handler, (u8 *)aRxBuffer3, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}
//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//复用为USART1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
	}
	  else if(huart->Instance==USART2)//如果是串口2，进行串口2 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
	
		GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART2;	//复用为USART1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA2

		GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA3
		
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//使能USART2中断通道
		HAL_NVIC_SetPriority(USART2_IRQn,2,3);			//抢占优先级3，子优先级3
	
	}
		else if(huart->Instance==USART3)//如果是串口3，进行串口3 MSP初始化
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART3_CLK_ENABLE();			//使能USART1时钟
	
		GPIO_Initure.Pin=GPIO_PIN_10;			//PB10
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART3;	//复用为USART1
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB10

		GPIO_Initure.Pin=GPIO_PIN_11;			//PB11
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB11
		
		HAL_NVIC_EnableIRQ(USART3_IRQn);		//使能USART1中断通道
		HAL_NVIC_SetPriority(USART3_IRQn,2,1);	//抢占优先级3，子优先级3

	}
}

u8 USART2_START=0;
u8 tri=7;
u8 ps_key[18]={0};
u8 ps_left[2]={0};
u8 ps_right[2]={0};
u8 ps_tim=0;
u8 ps_ks=0;
u8 ps_stop=1;//遥控器关闭状态  1关闭   0打开
uint8_t *ch;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if(huart->Instance==USART1)//如果是串口1
	{
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(aRxBuffer[0]!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(aRxBuffer[0]==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=aRxBuffer[0] ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}

	}
	else if(huart->Instance==USART2)//如果是串口2///遥控器
	{
		if(aRxBuffer2[0]=='s')
		{
			USART2_START=1;//开始接收数据
			ps_tim=0;
		}
		if(USART2_START)//接到's',开始接受数据
		{
			USART_RX_BUF2[USART2_RX_STA++]=aRxBuffer2[0];
			if(aRxBuffer2[0]=='e')//接到'e'，接受完成
			{		
				if(USART_RX_BUF2[18]!='q' && USART2_RX_STA==20)
				{			
//					ps_left[0]=(USART_RX_BUF2[1]-'0')*10+(USART_RX_BUF2[2]-'0');
//					ps_left[1]=(USART_RX_BUF2[3]-'0')*10+(USART_RX_BUF2[4]-'0');
//					ps_right[0]=(USART_RX_BUF2[5]-'0')*10+(USART_RX_BUF2[6]-'0');
//					ps_right[1]=(USART_RX_BUF2[7]-'0')*10+(USART_RX_BUF2[8]-'0');
					ps_key[0]=USART_RX_BUF2[1]-'0';
					ps_key[1]=USART_RX_BUF2[2]-'0';
					ps_key[2]=USART_RX_BUF2[3]-'0';
					ps_key[3]=USART_RX_BUF2[4]-'0';
					ps_key[4]=USART_RX_BUF2[5]-'0';
					ps_key[5]=USART_RX_BUF2[6]-'0';
					ps_key[6]=USART_RX_BUF2[7]-'0';
					ps_key[7]=USART_RX_BUF2[8]-'0';
					ps_key[8]=USART_RX_BUF2[9]-'0';
					ps_key[9]=USART_RX_BUF2[10]-'0';
					ps_key[10]=USART_RX_BUF2[11]-'0';
					ps_key[11]=USART_RX_BUF2[12]-'0';
					ps_key[12]=USART_RX_BUF2[13]-'0';
					ps_key[13]=USART_RX_BUF2[14]-'0';
					ps_key[14]=USART_RX_BUF2[15]-'0';
					ps_key[15]=USART_RX_BUF2[16]-'0';
					ps_key[16]=USART_RX_BUF2[17]-'0';
					ps_key[17]=USART_RX_BUF2[18]-'0';

					ps_stop=0;//遥控器打开--接收正常
				}
				USART2_RX_STA=0;
				USART2_START=0;//接收完成
			}
		}
	}
	else if(huart->Instance==USART3)//如果是串口3
	{
		if(aRxBuffer3[0]=='s')
		{
			USART3_START=1;//开始接收数据
		}
		if(USART3_START)//接到's',开始接受数据
		{
			USART_RX_BUF3[USART3_RX_STA++]=aRxBuffer3[0];
			
			if(aRxBuffer3[0]=='e')//接到'e'，接受完成
			{
//				  printf("USART3_RX_BUF=%s",&USART3_RX_BUF[1]);
				ps[0]=USART_RX_BUF3[1]-'0';
				ps_stop=0;//遥控器打开--接收正常
				USART3_RX_STA=0;
				USART3_START=0;//接收完成
				if(ps[0]==0)
					ch="0";
				else if(ps[0]==1)
					ch="1";
				else if(ps[0]==2)
					ch="2";
				else if(ps[0]==3)
					ch="3";
//				ch="3";
//				if(tri_mode==1)//按键0按下
//				{
//					HAL_UART_Transmit(&UART2_Handler,"i",1,1000);	//发送数据"k"
//					while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);		//等待发送结束
//				}

//				ch=ps[0]+'0';
//				ch='0';
//				HAL_UART_Transmit(&UART2_Handler,&ch,1,1000);	//发送数据"k"
//				while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);	
			}
			
		}
		
	}
}
 
//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>maxDelay) break;	
	}
} 

//串口2中断服务程序
void USART2_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;
		
	HAL_UART_IRQHandler(&UART2_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART2_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>maxDelay) break;		
	
	}
	timeout=0;
	while(HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRxBuffer2, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>maxDelay) break;	
	}
} 

#endif	
//串口3中断服务程序
void USART3_IRQHandler(void)                	
{
	u32 timeout=0;
	
	HAL_UART_IRQHandler(&UART3_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART3_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>HAL_MAX_DELAY) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART3_Handler, (u8 *)aRxBuffer3, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>HAL_MAX_DELAY) break;	
	}
}
/*下面代码我们直接把中断控制逻辑写在中断服务函数内部。*/
/*


//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 
	u8 Res;
	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        HAL_UART_Receive(&UART1_Handler,&Res,1,1000); 
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
	}
	HAL_UART_IRQHandler(&UART1_Handler);	
} 
#endif	
*/
 




