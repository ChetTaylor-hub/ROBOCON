#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
#include "FreeRTOS.h"
#include "task.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2015/6/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����	

extern u8 tri_mode;  	
extern uint8_t *ch;
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART_RX_BUF2[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART_RX_BUF3[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 

extern u16 USART_RX_STA;         		//����״̬���	
extern u16 USART2_RX_STA;         		//����״̬���	
extern u16 USART3_RX_STA;         		//����״̬���	
extern u16 hh;
extern u8 p;
extern UART_HandleTypeDef UART1_Handler; //UART���
extern UART_HandleTypeDef UART2_Handler; //UART���
extern UART_HandleTypeDef UART3_Handler; //UART���

#define RXBUFFERSIZE   1 //�����С
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL��USART����Buffer
extern u8 aRxBuffer2[RXBUFFERSIZE];//HAL��USART����Buffer
extern u8 aRxBuffer3[RXBUFFERSIZE];//HAL��USART����Buffer


#define RXBUFFERSIZE2   1 //�����С

/* ң����*/
extern u8 USART2_START;
extern u8 USART3_START;

extern u8 ps_key[18];
extern u8 ps_left[2];
extern u8 ps_right[2];
extern u8 ps_tim;//ң�������ն�ʱ����
extern u8 ps_ks;//ң������ȫ��
extern u8 ps_stop;//ң�����޽��գ�ֹͣ

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
extern u8 ps[3];

extern TaskHandle_t RcTask_Handler;


#endif
