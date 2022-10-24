#include "can.h"
#include "usart.h"
#include "delay.h"
#include "bsp_can.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//CAN��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									////////////////////////////////////////////////
  


CAN_HandleTypeDef hcan1;			//CAN1���
CAN_HandleTypeDef hcan2;


////CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1TQ~CAN_SJW_4TQ
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1TQ~CAN_BS2_8TQ;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1TQ~CAN_BS1_16TQ
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+tbs2+1)*brp); ����tbs1��tbs2����ֻ�ù�ע��ʶ���ϱ�־����ţ�����CAN_BS2_1TQ�����Ǿ���Ϊtbs2=1�����㼴�ɡ�
//mode:CAN_MODE_NORMAL,��ͨģʽ;CAN_MODE_LOOPBACK,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ45M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_8tq,6,CAN_MODE_LOOPBACK);
//������Ϊ:45M/((6+8+1)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 

u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
    CAN_FilterConfTypeDef  CAN1_FilerConf;
    
    hcan1.Instance=CAN1; 
    hcan1.Init.Prescaler=brp;    //��Ƶϵ��(Fdiv)Ϊbrp+1
    hcan1.Init.Mode=mode;        //ģʽ���� 
    hcan1.Init.SJW=tsjw;         //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1TQ~CAN_SJW_4TQ
    hcan1.Init.BS1=tbs1;         //tbs1��ΧCAN_BS1_1TQ~CAN_BS1_16TQ
    hcan1.Init.BS2=tbs2;         //tbs2��ΧCAN_BS2_1TQ~CAN_BS2_8TQ
    hcan1.Init.TTCM=DISABLE;     //��ʱ�䴥��ͨ��ģʽ 
    hcan1.Init.ABOM=DISABLE;     //����Զ����߹���
    hcan1.Init.AWUM=DISABLE;     //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    hcan1.Init.NART=ENABLE;      //��ֹ�����Զ����� 
    hcan1.Init.RFLM=DISABLE;     //���Ĳ�����,�µĸ��Ǿɵ� 
    hcan1.Init.TXFP=DISABLE;     //���ȼ��ɱ��ı�ʶ������ 
	
    if(HAL_CAN_Init(&hcan1)!=HAL_OK) return 1;   //��ʼ��
    
    CAN1_FilerConf.FilterIdHigh=0X0000;     //32λID
    CAN1_FilerConf.FilterIdLow=0X0000;
    CAN1_FilerConf.FilterMaskIdHigh=0X0000; //32λMASK
    CAN1_FilerConf.FilterMaskIdLow=0X0000;  
    CAN1_FilerConf.FilterFIFOAssignment=CAN_FILTER_FIFO0;//������0������FIFO0
    CAN1_FilerConf.FilterNumber=0;          //������0
    CAN1_FilerConf.FilterMode=CAN_FILTERMODE_IDMASK;
    CAN1_FilerConf.FilterScale=CAN_FILTERSCALE_32BIT;
    CAN1_FilerConf.FilterActivation=ENABLE; //�����˲���0
    CAN1_FilerConf.BankNumber=14;
	
    if(HAL_CAN_ConfigFilter(&hcan1,&CAN1_FilerConf)!=HAL_OK) return 2;//�˲�����ʼ��
	
    return 0;
}

u8 CAN2_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
    CAN_FilterConfTypeDef  CAN2_FilerConf;
    
    hcan2.Instance=CAN2; 
    hcan2.Init.Prescaler=brp;    //��Ƶϵ��(Fdiv)Ϊbrp+1
    hcan2.Init.Mode=mode;        //ģʽ���� 
    hcan2.Init.SJW=tsjw;         //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1TQ~CAN_SJW_4TQ
    hcan2.Init.BS1=tbs1;         //tbs1��ΧCAN_BS1_1TQ~CAN_BS1_16TQ
    hcan2.Init.BS2=tbs2;         //tbs2��ΧCAN_BS2_1TQ~CAN_BS2_8TQ
    hcan2.Init.TTCM=DISABLE;     //��ʱ�䴥��ͨ��ģʽ 
    hcan2.Init.ABOM=DISABLE;     //����Զ����߹���
    hcan2.Init.AWUM=DISABLE;     //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    hcan2.Init.NART=ENABLE;      //��ֹ�����Զ����� 
    hcan2.Init.RFLM=DISABLE;     //���Ĳ�����,�µĸ��Ǿɵ� 
    hcan2.Init.TXFP=DISABLE;     //���ȼ��ɱ��ı�ʶ������ 
	
    if(HAL_CAN_Init(&hcan2)!=HAL_OK) return 1;   //��ʼ��
    
    CAN2_FilerConf.FilterIdHigh=0X0000;     //32λID
    CAN2_FilerConf.FilterIdLow=0X0000;
    CAN2_FilerConf.FilterMaskIdHigh=0X0000; //32λMASK
    CAN2_FilerConf.FilterMaskIdLow=0X0000;  
    CAN2_FilerConf.FilterFIFOAssignment=CAN_FILTER_FIFO0;//������0������FIFO0
    CAN2_FilerConf.FilterNumber=0;          //������0
    CAN2_FilerConf.FilterMode=CAN_FILTERMODE_IDMASK;
    CAN2_FilerConf.FilterScale=CAN_FILTERSCALE_32BIT;
    CAN2_FilerConf.FilterActivation=ENABLE; //�����˲���0
    CAN2_FilerConf.BankNumber=14;
	
    if(HAL_CAN_ConfigFilter(&hcan2,&CAN2_FilerConf)!=HAL_OK) return 2;//�˲�����ʼ��
	
    return 0;
}


//CAN�ײ��������������ã�ʱ�����ã��ж�����
//�˺����ᱻHAL_CAN_Init()����
//hcan:CAN���
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;
  {
    __HAL_RCC_CAN1_CLK_ENABLE();                //ʹ��CAN1ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			    //����GPIOAʱ��
    
	  GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;   //PA11,12
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;              //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;         //����
    GPIO_Initure.Alternate=GPIO_AF9_CAN1;       //����ΪCAN1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
    
#if CAN1_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_FMP0);//FIFO0��Ϣ�����ж�����.	  
    //CAN1->IER|=1<<1;		//FIFO0��Ϣ�����ж�����.	
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn,1,2);    //��ռ���ȼ�1�������ȼ�2
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);          //ʹ���ж�
#endif	
	}
	{
		__HAL_RCC_CAN2_CLK_ENABLE();                //ʹ��CAN1ʱ��
	  __HAL_RCC_GPIOB_CLK_ENABLE();			    //����GPIOAʱ��
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6;   //PB5,PB6
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;              //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;         //����
    GPIO_Initure.Alternate=GPIO_AF9_CAN2;       //����ΪCAN1
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);         //��ʼ��

#if CAN2_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&hcan2,CAN_IT_FMP0);//FIFO0��Ϣ�����ж�����.	  
    //CAN1->IER|=1<<1;		//FIFO0��Ϣ�����ж�����.	
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn,2,2);    //��ռ���ȼ�1�������ȼ�2
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);          //ʹ���ж�
#endif	
	}

}

#if CAN1_RX0_INT_ENABLE                         //ʹ��RX0�ж�
#if CAN2_RX0_INT_ENABLE                         //ʹ��RX0�ж�

//CAN�жϷ�����
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);//�˺��������CAN_Receive_IT()��������
}
//CAN�жϷ�����
void CAN2_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan2);//�˺��������CAN_Receive_IT()��������
}

//CAN�жϴ������
//�˺����ᱻCAN_Receive_IT()����
//hcan:CAN���
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    int i=0;	
		//RxMessage.StdId=0x203;
    //CAN_Receive_IT()������ر�FIFO0��Ϣ�Һ��жϣ����������Ҫ���´�
		 switch(hcan->pRxMsg->StdId) {
    case CAN_3508Moto1_ID:
    case CAN_3508Moto2_ID:
    case CAN_3508Moto3_ID:
    case CAN_3508Moto4_ID:
    case CAN_3508Moto5_ID:
    case CAN_3508Moto6_ID:
    case CAN_3508Moto7_ID:
    case CAN_3508Moto8_ID:
    {
        static u8 i;
        i = hcan->pRxMsg->StdId - CAN_3508Moto1_ID;

        moto_chassis[i].msg_cnt++ <= 50	?	get_moto_offset(&moto_chassis[i], hcan) : get_moto_measure(&moto_chassis[i], hcan);
       get_moto_measure(&moto_info, hcan);
        //get_moto_measure(&moto_chassis[i], _hcan);
    }
    break;
		

    }
		
		//printf("canID:%x\r\n",hcan->pRxMsg->StdId);

	__HAL_CAN_ENABLE_IT(hcan, CAN_IT_FMP0);
}

#endif	
#endif
//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_Send_Msg(u16 id,u8* msg,u8 len)
{	
    u16 i=0;
    hcan1.pTxMsg->StdId=id;        //��׼��ʶ��
    hcan1.pTxMsg->ExtId=0x00;        //��չ��ʶ��(29λ)
    hcan1.pTxMsg->IDE=CAN_ID_STD;    //ʹ�ñ�׼֡
    hcan1.pTxMsg->RTR=CAN_RTR_DATA;  //����֡
    hcan1.pTxMsg->DLC=len;                
    for(i=0;i<len;i++)
    hcan1.pTxMsg->Data[i]=msg[i];
    if(HAL_CAN_Transmit(&hcan1,10)!=HAL_OK) return 1;     //����
    return 0;		
}
//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN2_Send_Msg(u16 id,u8* msg,u8 len)
{	
    u16 i=0;
    hcan2.pTxMsg->StdId=id;        //��׼��ʶ��
    hcan2.pTxMsg->ExtId=0x00;        //��չ��ʶ��(29λ)
    hcan2.pTxMsg->IDE=CAN_ID_STD;    //ʹ�ñ�׼֡
    hcan2.pTxMsg->RTR=CAN_RTR_DATA;  //����֡
    hcan2.pTxMsg->DLC=len;                
    for(i=0;i<len;i++)
    hcan2.pTxMsg->Data[i]=msg[i];
    if(HAL_CAN_Transmit(&hcan2,10)!=HAL_OK) return 1;     //����
    return 0;		
}

//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	if(HAL_CAN_Receive(&hcan1,CAN_FIFO0,0)!=HAL_OK) return 0;//�������ݣ���ʱʱ������Ϊ0	
	for(i=0;i<hcan1.pRxMsg->DLC;i++)
	buf[i]=hcan1.pRxMsg->Data[i];
	return hcan1.pRxMsg->DLC;	
}
u8 CAN2_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	if(HAL_CAN_Receive(&hcan2,CAN_FIFO0,0)!=HAL_OK) return 0;//�������ݣ���ʱʱ������Ϊ0	
	for(i=0;i<hcan2.pRxMsg->DLC;i++)
	buf[i]=hcan2.pRxMsg->Data[i];
	return hcan2.pRxMsg->DLC;	
}



