#include "key.h"
#include "delay.h"
#include "RF24L01.h"
#include "usart.h"
//按键初始化函数 
//PA15和PC5 设置成输入
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTC时钟

 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
//------------------------------------------------------------------	摇杆
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//PA2   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA2
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//PC2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC2
//------------------------------------------------------------------

	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//PB3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成上拉输入GPIO_Mode_Out_PP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA15
	 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//PB4
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOC5
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//PB5 
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//PB6
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA15
	 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;//PB7
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOC5
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//PB8 
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;//PB8 
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PB8 
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.0
} 
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//返回值：
//0，没有任何按键按下
//KEY0_PRES，KEY0按下
//KEY1_PRES，KEY1按下
//WKUP_PRES，WK_UP按下 
//注意此函数有响应优先级,KEY0>KEY1>WK_UP!!
void KEY_Scan(uint8_t *txt)
{	 
//		txt[8]=txt[9]=txt[10]=txt[11]=txt[12]=txt[13]=txt[14]=txt[15]=txt[16]='0';
	    txt[0]=txt[1]=txt[2]=txt[3]=txt[4]=txt[5]=txt[6]=txt[7]=txt[8]=txt[9]=txt[10]=txt[11]=txt[12]=txt[13]=txt[14]=txt[15]='0';

	  PBout(1)=PBout(3)=PBout(4)=PBout(5)=0;
		PBout(6)=PBout(7)=PBout(8)= PBout(9)=1;	
		if(KEY_P1)
		{
			delay_ms(20);
			PBout(1)=PBout(3)=PBout(4)=PBout(5)=1;
	  	PBout(6)=PBout(7)=PBout(8)=PBout(9)=0;	
			if(KEY_P6)
				txt[0]='1';
			
			else if(KEY_P7)
				txt[4]='1';
			
			else if(KEY_P8)
			  txt[8]='1';
			
			else if(KEY_P9)
			  txt[12]='1';
		}
		
	 else	if(KEY_P3)
		{
			delay_ms(20);
			PBout(1)=PBout(3)=PBout(4)=PBout(5)=1;
	  	PBout(6)=PBout(7)=PBout(8)=PBout(9)=0;	
		  if(KEY_P6)
			  txt[1]='1';
			
			else if(KEY_P7)
			  txt[5]='1';
			
			else if(KEY_P8)
			  txt[9]='1';
			
			else if(KEY_P9)
			  txt[13]='1';
		}
		
		else if(KEY_P4)
		{
			delay_ms(20);
			PBout(1)=PBout(3)=PBout(4)=PBout(5)=1;
	  	PBout(6)=PBout(7)=PBout(8)=PBout(9)=0;	
			if(KEY_P6)
			  txt[2]='1';
			
			else if(KEY_P7)
			  txt[6]='1';
			
			else if(KEY_P8)
			  txt[10]='1';
			else if(KEY_P9)
			  txt[14]='1';
		}
		
				else if(KEY_P5)
		{
			delay_ms(20);
			PBout(1)=PBout(3)=PBout(4)=PBout(5)=1;
	  	PBout(6)=PBout(7)=PBout(8)=PBout(9)=0;	
			if(KEY_P6)
			  txt[3]='1';
			
			else if(KEY_P7)
			  txt[7]='1';
			
			else if(KEY_P8)
			  txt[11]='1';
			else if(KEY_P9)
			  txt[15]='1';
		}
		
		if(!left_key)
		  txt[16]='1';
		else
			txt[16]='0';
		
		if(!right_key)
		  txt[17]='1';
		else
			txt[17]='0';	
}
