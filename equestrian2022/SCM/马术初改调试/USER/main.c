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
 ALIENTEK 阿波罗STM32F429开发板 FreeRTOS实验6-3
 FreeRTOS任务挂起和恢复实验-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
#ifndef PI
#define PI 3.14159265358979f
#endif

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

/*电机驱动*/
#define MotorControl_TASK_PRIO		4
#define MotorControl_STK_SIZE 		256
TaskHandle_t MotorControlTask_Handler;
void MotorControl_task(void *pvParameters);


/*任务1 步态选择*/
#define PostureControl_TASK_PRIO		6
#define PostureControl_STK_SIZE 		256
TaskHandle_t PostureControlTask_Handler;
//void PostureControl_task(void *pvParameters);

/*步态纠正*/
#define Navi_TASK_PRIO		5
#define Navi_STK_SIZE 		256
TaskHandle_t NaviTask_Handler;
void Navi_task(void *pvParameters);

/*遥控器*/
#define Rc_TASK_PRIO		6
#define Rc_STK_SIZE 		256 //256
TaskHandle_t RcTask_Handler;
void Rc_task(void *pvParameters);

//CAN_HandleTypeDef hcan1;			//CAN1句柄
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
  	HAL_Init();                     //初始化HAL库    
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
		delay_init(180);   
		LED_Init();
		KEY_Init();
		uart_init(38400);	 /*串口初始化*/
		uart2_init(115200);						//遥控器通信
		uart3_init(9600);
		SDRAM_Init();                   //初始化SDRAM
		while(mpu_dmp_init())
		
			LED0=1;//DS0闪烁 ;     
	   	LED0=0;	
//	  LCD_Init();                     //初始化LCD
//		POINT_COLOR=RED;
		CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_6TQ,3,CAN_MODE_NORMAL); //CAN初始化,波特率1000Kbps 	
		CAN2_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_6TQ,3,CAN_MODE_NORMAL); //CAN初始化,波特率1000Kbps//CAN_MODE_NORMAL,CAN_MODE_LOOPBACK
	  my_can_filter_init_recv_all(&hcan1);//开启CAN滤波器
		my_can_filter_init_recv_all(&hcan2);//开启CAN滤波器
	  HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//开启CAN1 
    HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);//开启CAN2
    pid_param_init();		//所有使用到的PID参数初始化
	  TIM3_Init(200-1,9000-1);   // 600  9000

//	  state=START;
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄                
    vTaskStartScheduler();          //开启任务调度
}


//开始任务任务函数
void start_task(void *pvParameters)
{

    taskENTER_CRITICAL();           //进入临界�
    //创建MotorControl_task
    xTaskCreate((TaskFunction_t )MotorControl_task, //4电机驱动
                (const char*    )"MotorControl_task",
                (uint16_t       )MotorControl_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )MotorControl_TASK_PRIO,
                (TaskHandle_t*  )&MotorControlTask_Handler);
			

    //创建NAVIGATION任务
    xTaskCreate((TaskFunction_t )Navi_task,  
                (const char*    )"Navi_task",  //5矫正
                (uint16_t       )Navi_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Navi_TASK_PRIO,
                (TaskHandle_t*  )&NaviTask_Handler);
								
    //创建PostureControl_task
    xTaskCreate((TaskFunction_t )PostureControl_task,  //5姿态
                (const char*    )"PostureControl_task",
                (uint16_t       )PostureControl_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )PostureControl_TASK_PRIO,
                (TaskHandle_t*  )&PostureControlTask_Handler);
		//创建Rc_task
    xTaskCreate((TaskFunction_t )Rc_task,
                (const char*    )"Rc_task",  //遥控器
                (uint16_t       )Rc_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Rc_TASK_PRIO,
                (TaskHandle_t*  )&RcTask_Handler);

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

u8 num=4;




