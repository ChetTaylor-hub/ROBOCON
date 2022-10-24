#include "can.h"
#include "usart.h"
#include "delay.h"
#include "bsp_can.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//CAN驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									////////////////////////////////////////////////
  


CAN_HandleTypeDef hcan1;			//CAN1句柄
CAN_HandleTypeDef hcan2;


////CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1TQ~CAN_SJW_4TQ
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1TQ~CAN_BS2_8TQ;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1TQ~CAN_BS1_16TQ
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+tbs2+1)*brp); 其中tbs1和tbs2我们只用关注标识符上标志的序号，例如CAN_BS2_1TQ，我们就认为tbs2=1来计算即可。
//mode:CAN_MODE_NORMAL,普通模式;CAN_MODE_LOOPBACK,回环模式;
//Fpclk1的时钟在初始化的时候设置为45M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_8tq,6,CAN_MODE_LOOPBACK);
//则波特率为:45M/((6+8+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 

u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
    CAN_FilterConfTypeDef  CAN1_FilerConf;
    
    hcan1.Instance=CAN1; 
    hcan1.Init.Prescaler=brp;    //分频系数(Fdiv)为brp+1
    hcan1.Init.Mode=mode;        //模式设置 
    hcan1.Init.SJW=tsjw;         //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1TQ~CAN_SJW_4TQ
    hcan1.Init.BS1=tbs1;         //tbs1范围CAN_BS1_1TQ~CAN_BS1_16TQ
    hcan1.Init.BS2=tbs2;         //tbs2范围CAN_BS2_1TQ~CAN_BS2_8TQ
    hcan1.Init.TTCM=DISABLE;     //非时间触发通信模式 
    hcan1.Init.ABOM=DISABLE;     //软件自动离线管理
    hcan1.Init.AWUM=DISABLE;     //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    hcan1.Init.NART=ENABLE;      //禁止报文自动传送 
    hcan1.Init.RFLM=DISABLE;     //报文不锁定,新的覆盖旧的 
    hcan1.Init.TXFP=DISABLE;     //优先级由报文标识符决定 
	
    if(HAL_CAN_Init(&hcan1)!=HAL_OK) return 1;   //初始化
    
    CAN1_FilerConf.FilterIdHigh=0X0000;     //32位ID
    CAN1_FilerConf.FilterIdLow=0X0000;
    CAN1_FilerConf.FilterMaskIdHigh=0X0000; //32位MASK
    CAN1_FilerConf.FilterMaskIdLow=0X0000;  
    CAN1_FilerConf.FilterFIFOAssignment=CAN_FILTER_FIFO0;//过滤器0关联到FIFO0
    CAN1_FilerConf.FilterNumber=0;          //过滤器0
    CAN1_FilerConf.FilterMode=CAN_FILTERMODE_IDMASK;
    CAN1_FilerConf.FilterScale=CAN_FILTERSCALE_32BIT;
    CAN1_FilerConf.FilterActivation=ENABLE; //激活滤波器0
    CAN1_FilerConf.BankNumber=14;
	
    if(HAL_CAN_ConfigFilter(&hcan1,&CAN1_FilerConf)!=HAL_OK) return 2;//滤波器初始化
	
    return 0;
}

u8 CAN2_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
    CAN_FilterConfTypeDef  CAN2_FilerConf;
    
    hcan2.Instance=CAN2; 
    hcan2.Init.Prescaler=brp;    //分频系数(Fdiv)为brp+1
    hcan2.Init.Mode=mode;        //模式设置 
    hcan2.Init.SJW=tsjw;         //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1TQ~CAN_SJW_4TQ
    hcan2.Init.BS1=tbs1;         //tbs1范围CAN_BS1_1TQ~CAN_BS1_16TQ
    hcan2.Init.BS2=tbs2;         //tbs2范围CAN_BS2_1TQ~CAN_BS2_8TQ
    hcan2.Init.TTCM=DISABLE;     //非时间触发通信模式 
    hcan2.Init.ABOM=DISABLE;     //软件自动离线管理
    hcan2.Init.AWUM=DISABLE;     //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    hcan2.Init.NART=ENABLE;      //禁止报文自动传送 
    hcan2.Init.RFLM=DISABLE;     //报文不锁定,新的覆盖旧的 
    hcan2.Init.TXFP=DISABLE;     //优先级由报文标识符决定 
	
    if(HAL_CAN_Init(&hcan2)!=HAL_OK) return 1;   //初始化
    
    CAN2_FilerConf.FilterIdHigh=0X0000;     //32位ID
    CAN2_FilerConf.FilterIdLow=0X0000;
    CAN2_FilerConf.FilterMaskIdHigh=0X0000; //32位MASK
    CAN2_FilerConf.FilterMaskIdLow=0X0000;  
    CAN2_FilerConf.FilterFIFOAssignment=CAN_FILTER_FIFO0;//过滤器0关联到FIFO0
    CAN2_FilerConf.FilterNumber=0;          //过滤器0
    CAN2_FilerConf.FilterMode=CAN_FILTERMODE_IDMASK;
    CAN2_FilerConf.FilterScale=CAN_FILTERSCALE_32BIT;
    CAN2_FilerConf.FilterActivation=ENABLE; //激活滤波器0
    CAN2_FilerConf.BankNumber=14;
	
    if(HAL_CAN_ConfigFilter(&hcan2,&CAN2_FilerConf)!=HAL_OK) return 2;//滤波器初始化
	
    return 0;
}


//CAN底层驱动，引脚配置，时钟配置，中断配置
//此函数会被HAL_CAN_Init()调用
//hcan:CAN句柄
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;
  {
    __HAL_RCC_CAN1_CLK_ENABLE();                //使能CAN1时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			    //开启GPIOA时钟
    
	  GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;   //PA11,12
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //推挽复用
    GPIO_Initure.Pull=GPIO_PULLUP;              //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;         //快速
    GPIO_Initure.Alternate=GPIO_AF9_CAN1;       //复用为CAN1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //初始化
    
#if CAN1_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_FMP0);//FIFO0消息挂起中断允许.	  
    //CAN1->IER|=1<<1;		//FIFO0消息挂起中断允许.	
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn,1,2);    //抢占优先级1，子优先级2
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);          //使能中断
#endif	
	}
	{
		__HAL_RCC_CAN2_CLK_ENABLE();                //使能CAN1时钟
	  __HAL_RCC_GPIOB_CLK_ENABLE();			    //开启GPIOA时钟
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6;   //PB5,PB6
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //推挽复用
    GPIO_Initure.Pull=GPIO_PULLUP;              //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;         //快速
    GPIO_Initure.Alternate=GPIO_AF9_CAN2;       //复用为CAN1
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);         //初始化

#if CAN2_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&hcan2,CAN_IT_FMP0);//FIFO0消息挂起中断允许.	  
    //CAN1->IER|=1<<1;		//FIFO0消息挂起中断允许.	
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn,2,2);    //抢占优先级1，子优先级2
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);          //使能中断
#endif	
	}

}

#if CAN1_RX0_INT_ENABLE                         //使能RX0中断
#if CAN2_RX0_INT_ENABLE                         //使能RX0中断

//CAN中断服务函数
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);//此函数会调用CAN_Receive_IT()接收数据
}
//CAN中断服务函数
void CAN2_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan2);//此函数会调用CAN_Receive_IT()接收数据
}

//CAN中断处理过程
//此函数会被CAN_Receive_IT()调用
//hcan:CAN句柄
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    int i=0;	
		//RxMessage.StdId=0x203;
    //CAN_Receive_IT()函数会关闭FIFO0消息挂号中断，因此我们需要重新打开
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
//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_Send_Msg(u16 id,u8* msg,u8 len)
{	
    u16 i=0;
    hcan1.pTxMsg->StdId=id;        //标准标识符
    hcan1.pTxMsg->ExtId=0x00;        //扩展标识符(29位)
    hcan1.pTxMsg->IDE=CAN_ID_STD;    //使用标准帧
    hcan1.pTxMsg->RTR=CAN_RTR_DATA;  //数据帧
    hcan1.pTxMsg->DLC=len;                
    for(i=0;i<len;i++)
    hcan1.pTxMsg->Data[i]=msg[i];
    if(HAL_CAN_Transmit(&hcan1,10)!=HAL_OK) return 1;     //发送
    return 0;		
}
//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN2_Send_Msg(u16 id,u8* msg,u8 len)
{	
    u16 i=0;
    hcan2.pTxMsg->StdId=id;        //标准标识符
    hcan2.pTxMsg->ExtId=0x00;        //扩展标识符(29位)
    hcan2.pTxMsg->IDE=CAN_ID_STD;    //使用标准帧
    hcan2.pTxMsg->RTR=CAN_RTR_DATA;  //数据帧
    hcan2.pTxMsg->DLC=len;                
    for(i=0;i<len;i++)
    hcan2.pTxMsg->Data[i]=msg[i];
    if(HAL_CAN_Transmit(&hcan2,10)!=HAL_OK) return 1;     //发送
    return 0;		
}

//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	if(HAL_CAN_Receive(&hcan1,CAN_FIFO0,0)!=HAL_OK) return 0;//接收数据，超时时间设置为0	
	for(i=0;i<hcan1.pRxMsg->DLC;i++)
	buf[i]=hcan1.pRxMsg->Data[i];
	return hcan1.pRxMsg->DLC;	
}
u8 CAN2_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	if(HAL_CAN_Receive(&hcan2,CAN_FIFO0,0)!=HAL_OK) return 0;//接收数据，超时时间设置为0	
	for(i=0;i<hcan2.pRxMsg->DLC;i++)
	buf[i]=hcan2.pRxMsg->Data[i];
	return hcan2.pRxMsg->DLC;	
}



