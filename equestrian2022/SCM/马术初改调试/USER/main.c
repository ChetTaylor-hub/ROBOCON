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
 ALIENTEK °¢²¨ÂŞSTM32F429¿ª·¢°å FreeRTOSÊµÑé6-3
 FreeRTOSÈÎÎñ¹ÒÆğºÍ»Ö¸´ÊµÑé-HAL¿â°æ±¾
 ¼¼ÊõÖ§³Ö£ºwww.openedv.com
 ÌÔ±¦µêÆÌ£ºhttp://eboard.taobao.com 
 ¹Ø×¢Î¢ĞÅ¹«ÖÚÆ½Ì¨Î¢ĞÅºÅ£º"ÕıµãÔ­×Ó"£¬Ãâ·Ñ»ñÈ¡STM32×ÊÁÏ¡£
 ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾  
 ×÷Õß£ºÕıµãÔ­×Ó @ALIENTEK
************************************************/
#ifndef PI
#define PI 3.14159265358979f
#endif

//ÈÎÎñÓÅÏÈ¼¶
#define START_TASK_PRIO		1
//ÈÎÎñ¶ÑÕ»´óĞ¡	
#define START_STK_SIZE 		128  
//ÈÎÎñ¾ä±ú
TaskHandle_t StartTask_Handler;
//ÈÎÎñº¯Êı
void start_task(void *pvParameters);

/*µç»úÇı¶¯*/
#define MotorControl_TASK_PRIO		4
#define MotorControl_STK_SIZE 		256
TaskHandle_t MotorControlTask_Handler;
void MotorControl_task(void *pvParameters);


/*ÈÎÎñ1 ²½Ì¬Ñ¡Ôñ*/
#define PostureControl_TASK_PRIO		6
#define PostureControl_STK_SIZE 		256
TaskHandle_t PostureControlTask_Handler;
//void PostureControl_task(void *pvParameters);

/*²½Ì¬¾ÀÕı*/
#define Navi_TASK_PRIO		5
#define Navi_STK_SIZE 		256
TaskHandle_t NaviTask_Handler;
void Navi_task(void *pvParameters);

/*Ò£¿ØÆ÷*/
#define Rc_TASK_PRIO		6
#define Rc_STK_SIZE 		256 //256
TaskHandle_t RcTask_Handler;
void Rc_task(void *pvParameters);

//CAN_HandleTypeDef hcan1;			//CAN1¾ä±ú
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
  	HAL_Init();                     //³õÊ¼»¯HAL¿â    
    Stm32_Clock_Init(360,25,2,8);   //ÉèÖÃÊ±ÖÓ,180Mhz
		delay_init(180);   
		LED_Init();
		KEY_Init();
		uart_init(38400);	 /*´®¿Ú³õÊ¼»¯*/
		uart2_init(115200);						//Ò£¿ØÆ÷Í¨ĞÅ
		uart3_init(9600);
		SDRAM_Init();                   //³õÊ¼»¯SDRAM
		while(mpu_dmp_init())
		
			LED0=1;//DS0ÉÁË¸ ;     
	   	LED0=0;	
//	  LCD_Init();                     //³õÊ¼»¯LCD
//		POINT_COLOR=RED;
		CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_6TQ,3,CAN_MODE_NORMAL); //CAN³õÊ¼»¯,²¨ÌØÂÊ1000Kbps 	
		CAN2_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_6TQ,3,CAN_MODE_NORMAL); //CAN³õÊ¼»¯,²¨ÌØÂÊ1000Kbps//CAN_MODE_NORMAL,CAN_MODE_LOOPBACK
	  my_can_filter_init_recv_all(&hcan1);//¿ªÆôCANÂË²¨Æ÷
		my_can_filter_init_recv_all(&hcan2);//¿ªÆôCANÂË²¨Æ÷
	  HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//¿ªÆôCAN1 
    HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);//¿ªÆôCAN2
    pid_param_init();		//ËùÓĞÊ¹ÓÃµ½µÄPID²ÎÊı³õÊ¼»¯
	  TIM3_Init(200-1,9000-1);   // 600  9000

//	  state=START;
    xTaskCreate((TaskFunction_t )start_task,            //ÈÎÎñº¯Êı
                (const char*    )"start_task",          //ÈÎÎñÃû³Æ
                (uint16_t       )START_STK_SIZE,        //ÈÎÎñ¶ÑÕ»´óĞ¡
                (void*          )NULL,                  //´«µİ¸øÈÎÎñº¯ÊıµÄ²ÎÊı
                (UBaseType_t    )START_TASK_PRIO,       //ÈÎÎñÓÅÏÈ¼¶
                (TaskHandle_t*  )&StartTask_Handler);   //ÈÎÎñ¾ä±ú                
    vTaskStartScheduler();          //¿ªÆôÈÎÎñµ÷¶È
}


//¿ªÊ¼ÈÎÎñÈÎÎñº¯Êı
void start_task(void *pvParameters)
{

    taskENTER_CRITICAL();           //½øÈëÁÙ½çÇ
    //´´½¨MotorControl_task
    xTaskCreate((TaskFunction_t )MotorControl_task, //4µç»úÇı¶¯
                (const char*    )"MotorControl_task",
                (uint16_t       )MotorControl_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )MotorControl_TASK_PRIO,
                (TaskHandle_t*  )&MotorControlTask_Handler);
			

    //´´½¨NAVIGATIONÈÎÎñ
    xTaskCreate((TaskFunction_t )Navi_task,  
                (const char*    )"Navi_task",  //5½ÃÕı
                (uint16_t       )Navi_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Navi_TASK_PRIO,
                (TaskHandle_t*  )&NaviTask_Handler);
								
    //´´½¨PostureControl_task
    xTaskCreate((TaskFunction_t )PostureControl_task,  //5×ËÌ¬
                (const char*    )"PostureControl_task",
                (uint16_t       )PostureControl_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )PostureControl_TASK_PRIO,
                (TaskHandle_t*  )&PostureControlTask_Handler);
		//´´½¨Rc_task
    xTaskCreate((TaskFunction_t )Rc_task,
                (const char*    )"Rc_task",  //Ò£¿ØÆ÷
                (uint16_t       )Rc_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Rc_TASK_PRIO,
                (TaskHandle_t*  )&RcTask_Handler);

    vTaskDelete(StartTask_Handler); //É¾³ı¿ªÊ¼ÈÎÎñ
    taskEXIT_CRITICAL();            //ÍË³öÁÙ½çÇø
}

u8 num=4;




