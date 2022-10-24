#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define KEY0 PCin(5)   	
//#define KEY1 PAin(15)	 
//#define WK_UP  PAin(0)	 
 

#define right_key  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)//��ȡ����0
#define left_key  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//��ȡ����1

#define KEY_P1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)//��ȡ����0
#define KEY_P3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)//��ȡ����0
#define KEY_P4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//��ȡ����1
#define KEY_P5  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//��ȡ����2 
#define KEY_P6  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//��ȡ����0
#define KEY_P7  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//��ȡ����1
#define KEY_P8  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//��ȡ����2 
#define KEY_P9  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//��ȡ����2 




void KEY_Init(void);//IO��ʼ��
void KEY_Scan(uint8_t *txt);  	//����ɨ�躯��					    
#endif
