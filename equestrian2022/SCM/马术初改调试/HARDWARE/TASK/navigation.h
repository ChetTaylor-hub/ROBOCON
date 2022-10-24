#ifndef NAVIGATION_H
#define NAVIGATION_H
#include "stdint.h"

//typedef union _imu_data
//{
//    uint8_t data[24];
//    float ActVal[6];
//} imudata;

//extern imudata imuinfo ;


#define Permit 1
#define Deny 0


//#define climbing_correction 5 //���½���
//#define test1_correction 4    //����1����
#define normalWALK_correction 2   //һ���������
#define normalTROT_correction 3   //һ���������
#define climbing_correction 4 //���½���
#define climbingdown_correction 5    //���½���
#define normalWALKBACK_correction 6   //һ���������
//#define normalBOUND_correction 7   //һ���������
#define test2_correction 8   //��̨�׵�һ�׶ξ���
#define test3_correction 9  //��̨�׵ڶ��׶ξ���
//#define test4_correction 10  //������
#define test5_correction 11  //��̨�׵ڶ��׶ξ���
#define test9_correction 12  //��̨�׵ڶ��׶ξ���
#define normalTROT2_correction 13   //һ���������
#define normalTROT3_correction 14   //һ���������
#define test6_correction 15  //��̨�׵ڶ��׶ξ���
#define climbing2_correction 16  //��̨�׵ڶ��׶ξ���

/*ƫ���Ǿ���*/
extern float yaw_now;
extern float yaw_set;
extern	float pitch,roll,yaw; 	        //ŷ����


extern int LinearCorrection;
extern int BalanceCorrection;

void navigation_execute(void);


#endif
