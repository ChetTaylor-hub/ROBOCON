
//#include "moto_ctrl.h"
//#include "pid.h"
//#include "bsp_can.h"
//#include "cmsis_os.h"

////void send_chassis_cur1_4(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
////void send_chassis_cur5_8(int16_t motor5, int16_t motor6, int16_t motor7, int16_t motor8);

//float ref_agle[8]= {0};
//float temp_angle;
//temp_data temp_pid= {0};     //pid�м�����

//float test_speed=0;
//int pid_spd_out_limit = 6720;
///*******************************************************************************************
//	*@ �������ƣ�void MotorControl_task(void *pvParameters)
//	*@ ���ܣ� ���ܵ���������ݲ��ҽ���PID���� ���������С���Ƶ��λ��
//	*@ ��ע�� FREERTOS������
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
//        pid_calc(&pid_pos[i],moto_chassis[i].total_angle*360/8191,ref_agle[i]*360/8191);  //λ�û� �Ƕȿ���

//        if(pid_pos[i].pos_out>pid_spd_out_limit)pid_pos[i].pos_out=pid_spd_out_limit;	//����ٶ�����
//        else if(pid_pos[i].pos_out<-pid_spd_out_limit)pid_pos[i].pos_out=-pid_spd_out_limit; //����ٶ�����

//        //temp_pid.out[i] = pid_calc(&pid_spd[i],moto_chassis[i].speed_rpm,test_speed);  //�ٶȻ� ����
//        //temp_pid.out[i] = pid_calc(&pid_spd[i],moto_chassis[i].speed_rpm,pid_pos[i].pos_out);  //�ٶȻ� �ٶȿ���
//        moto_chassis[i].given_current = pid_calc(&pid_spd[i],moto_chassis[i].speed_rpm,pid_pos[i].pos_out);  //�ٶȻ� �ٶȿ���
//    }

//    run();

//    osDelay(3);		//����Ƶ��  ��	1	7�ź�8�ŵ����ʧ��

//}

///**
//NAME: void pid_param_init(void)
// FUNCTION : pid��������ʼ��
////*/
//void pid_param_init(void)
//{

//    for (int i = 0; i < 8; i++)//  						20,0.01,0  37,0.008   8.0f, 0.000f   16
//        PID_struct_init(&pid_pos[i], POSITION_PID, 100000.0f, 2000.0f, 8.0f, 0.0008f, 0.0f);  //λ�û�PID�������ã�pid�ṹ�壬PID���ͣ����������������ƣ�P , I , D ��

//    for (int i = 0; i < 8; i++)  //									16384.0f��Ӧ20A					15.5f,0,0 // 16.0f, 0.001f   20.0f, 0.004297f   22.0f, 0.01399f
//        PID_struct_init(&pid_spd[i], POSITION_PID, 16000.0f, 2000.0f, 16.0f, 0.000100, 0.0f);		//�ٶȻ�PID��pid�ṹ�壬PID���ͣ����������������ƣ�P , I , D ��
//    //20.0f, 0.004297f, 0.0f     22.0f, 0.001

//}

