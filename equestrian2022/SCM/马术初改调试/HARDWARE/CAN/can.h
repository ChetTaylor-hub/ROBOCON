#ifndef __CAN_H
#define __CAN_H

//#define ID1 15
//#define ID2 54
//#define ID3 53

#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F746������
//CAN��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//CAN1����RX0�ж�ʹ��
#define CAN1_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.
#define CAN2_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.


extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode);//CAN��ʼ��
u8 CAN2_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode); //CAN��ʼ��,������1000Kbps 		
u8 CAN1_Send_Msg(u16 id,u8* msg,u8 len);						//��������
u8 CAN2_Send_Msg(u16 id,u8* msg,u8 len);
u8 CAN1_Receive_Msg(u8 *buf);							//��������
u8 CAN2_Receive_Msg(u8 *buf);							//��������

#endif


