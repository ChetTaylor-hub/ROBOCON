#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pid.h"
#include "command.h"
#include "moto_ctrl.h"
#include "posture.h"
#include "moto_ctrl.h"
#include "sdram.h"
#include "mpu9250.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "can.h"
#include "bsp_uart.h"
#include "tim.h"
#include "inv_mpu.h"

/************************************************
 ALIENTEK ������STM32F429������ FreeRTOSʵ��6-3
 FreeRTOS�������ͻָ�ʵ��-HAL��汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
#ifndef PI
#define PI 3.14159265358979f
#endif

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

/*�������*/
#define MotorControl_TASK_PRIO		4
#define MotorControl_STK_SIZE 		256
TaskHandle_t MotorControlTask_Handler;
void MotorControl_task(void *pvParameters);


/*����1 ��̬ѡ��*/
#define PostureControl_TASK_PRIO		6
#define PostureControl_STK_SIZE 		256
TaskHandle_t PostureControlTask_Handler;
//void PostureControl_task(void *pvParameters);

/*��̬����*/
#define Navi_TASK_PRIO		5
#define Navi_STK_SIZE 		256
TaskHandle_t NaviTask_Handler;
void Navi_task(void *pvParameters);

/*ң����*/
#define Rc_TASK_PRIO		6
#define Rc_STK_SIZE 		256 //256
TaskHandle_t RcTask_Handler;
void Rc_task(void *pvParameters);

//CAN_HandleTypeDef hcan1;			//CAN1���
//CAN_HandleTypeDef hcan2;

//int _leg_active[4]= {1,1,1,1};
//u8 mode[4];
int task2[8];
float now_time;
//void SystemClock_Config(void);
u8 p;
int main(void)
{
//	  u8 key;
//	ps[0]=1;
  	HAL_Init();                     //��ʼ��HAL��    
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz
		delay_init(180);   
		LED_Init();
		KEY_Init();
		uart_init(38400);	 /*���ڳ�ʼ��*/
		uart2_init(115200);						//ң����ͨ��
		uart3_init(9600);
		SDRAM_Init();                   //��ʼ��SDRAM
		while(mpu_dmp_init())
		
			LED0=1;//DS0��˸ ;     
	   	LED0=0;	
//	  LCD_Init();                     //��ʼ��LCD
//		POINT_COLOR=RED;
		CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_6TQ,3,CAN_MODE_NORMAL); //CAN��ʼ��,������1000Kbps 	
		CAN2_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_6TQ,3,CAN_MODE_NORMAL); //CAN��ʼ��,������1000Kbps//CAN_MODE_NORMAL,CAN_MODE_LOOPBACK
	  my_can_filter_init_recv_all(&hcan1);//����CAN�˲���
		my_can_filter_init_recv_all(&hcan2);//����CAN�˲���
	  HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//����CAN1 
    HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);//����CAN2
    pid_param_init();		//����ʹ�õ���PID������ʼ��
	  TIM3_Init(200-1,9000-1);   // 600  9000

//	  state=START;
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������                
    vTaskStartScheduler();          //�����������
}


//��ʼ����������
void start_task(void *pvParameters)
{

    taskENTER_CRITICAL();           //�����ٽ��
    //����MotorControl_task
    xTaskCreate((TaskFunction_t )MotorControl_task, //4�������
                (const char*    )"MotorControl_task",
                (uint16_t       )MotorControl_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )MotorControl_TASK_PRIO,
                (TaskHandle_t*  )&MotorControlTask_Handler);
			

    //����NAVIGATION����
    xTaskCreate((TaskFunction_t )Navi_task,  
                (const char*    )"Navi_task",  //5����
                (uint16_t       )Navi_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Navi_TASK_PRIO,
                (TaskHandle_t*  )&NaviTask_Handler);
								
    //����PostureControl_task
    xTaskCreate((TaskFunction_t )PostureControl_task,  //5��̬
                (const char*    )"PostureControl_task",
                (uint16_t       )PostureControl_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )PostureControl_TASK_PRIO,
                (TaskHandle_t*  )&PostureControlTask_Handler);
		//����Rc_task
    xTaskCreate((TaskFunction_t )Rc_task,
                (const char*    )"Rc_task",  //ң����
                (uint16_t       )Rc_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Rc_TASK_PRIO,
                (TaskHandle_t*  )&RcTask_Handler);

    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

u8 num=4;




