
//#include "moto_ctrl.h"
//#include "pid.h"
//#include "bsp_can.h"
//#include "cmsis_os.h"

////void send_chassis_cur1_4(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
////void send_chassis_cur5_8(int16_t motor5, int16_t motor6, int16_t motor7, int16_t motor8);

//float ref_agle[8]= {0};
//float temp_angle;
//temp_data temp_pid= {0};     //pid中间数据

//float test_speed=0;
//int pid_spd_out_limit = 6720;
///*******************************************************************************************
//	*@ 函数名称：void MotorControl_task(void *pvParameters)
//	*@ 功能： 接受电机反馈数据并且进行PID计算 输出电流大小控制电机位置
//	*@ 备注： FREERTOS任务函数
// *******************************************************************************************/
//void MotorControl_task(void *pvParameters)
//{
//    for(;;) {
//        moto_behaviour();
//    }

//}

//void moto_behaviour(void)
//{
//    if(IsMotoReadyOrNot== IsReady) {
//        for(int i=0; i<8; i++)
//            ref_agle[i]=temp_pid.ref_agle[i];
//        IsMotoReadyOrNot= NotReady;
//    }

//    for(int i=0; i<8; i++) {
//        
//        pid_calc(&pid_pos[i],moto_chassis[i].total_angle*360/8191,ref_agle[i]*360/8191);  //位置环 角度控制

//        if(pid_pos[i].pos_out>pid_spd_out_limit)pid_pos[i].pos_out=pid_spd_out_limit;	//最高速度限制
//        else if(pid_pos[i].pos_out<-pid_spd_out_limit)pid_pos[i].pos_out=-pid_spd_out_limit; //最高速度限制

//        //temp_pid.out[i] = pid_calc(&pid_spd[i],moto_chassis[i].speed_rpm,test_speed);  //速度环 测试
//        //temp_pid.out[i] = pid_calc(&pid_spd[i],moto_chassis[i].speed_rpm,pid_pos[i].pos_out);  //速度环 速度控制
//        moto_chassis[i].given_current = pid_calc(&pid_spd[i],moto_chassis[i].speed_rpm,pid_pos[i].pos_out);  //速度环 速度控制
//    }

//    run();

//    osDelay(3);		//控制频率  给	1	7号和8号电机会失控

//}

///**
//NAME: void pid_param_init(void)
// FUNCTION : pid参数填充初始化
////*/
//void pid_param_init(void)
//{

//    for (int i = 0; i < 8; i++)//  						20,0.01,0  37,0.008   8.0f, 0.000f   16
//        PID_struct_init(&pid_pos[i], POSITION_PID, 100000.0f, 2000.0f, 8.0f, 0.0008f, 0.0f);  //位置环PID参数设置（pid结构体，PID类型，最大输出，比例限制，P , I , D ）

//    for (int i = 0; i < 8; i++)  //									16384.0f对应20A					15.5f,0,0 // 16.0f, 0.001f   20.0f, 0.004297f   22.0f, 0.01399f
//        PID_struct_init(&pid_spd[i], POSITION_PID, 16000.0f, 2000.0f, 16.0f, 0.000100, 0.0f);		//速度环PID（pid结构体，PID类型，最大输出，比例限制，P , I , D ）
//    //20.0f, 0.004297f, 0.0f     22.0f, 0.001

//}

