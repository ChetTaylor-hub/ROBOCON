#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
#include "FreeRTOS.h"
#include "task.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.csom
//修改日期:2015/6/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.0修改说明 
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收	

extern u8 tri_mode;  	
extern uint8_t *ch;
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART_RX_BUF2[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART_RX_BUF3[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 

extern u16 USART_RX_STA;         		//接收状态标记	
extern u16 USART2_RX_STA;         		//接收状态标记	
extern u16 USART3_RX_STA;         		//接收状态标记	
extern u16 hh;
extern u8 p;
extern UART_HandleTypeDef UART1_Handler; //UART句柄
extern UART_HandleTypeDef UART2_Handler; //UART句柄
extern UART_HandleTypeDef UART3_Handler; //UART句柄

#define RXBUFFERSIZE   1 //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL库USART接收Buffer
extern u8 aRxBuffer2[RXBUFFERSIZE];//HAL库USART接收Buffer
extern u8 aRxBuffer3[RXBUFFERSIZE];//HAL库USART接收Buffer


#define RXBUFFERSIZE2   1 //缓存大小

/* 遥控器*/
extern u8 USART2_START;
extern u8 USART3_START;

extern u8 ps_key[18];
extern u8 ps_left[2];
extern u8 ps_right[2];
extern u8 ps_tim;//遥控器接收定时计数
extern u8 ps_ks;//遥控器完全打开
extern u8 ps_stop;//遥控器无接收，停止

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
extern u8 ps[3];

extern TaskHandle_t RcTask_Handler;


#endif
