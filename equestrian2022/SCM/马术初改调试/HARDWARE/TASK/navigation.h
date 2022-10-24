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


//#define climbing_correction 5 //爬坡矫正
//#define test1_correction 4    //任务1矫正
#define normalWALK_correction 2   //一般情况矫正
#define normalTROT_correction 3   //一般情况矫正
#define climbing_correction 4 //爬坡矫正
#define climbingdown_correction 5    //下坡矫正
#define normalWALKBACK_correction 6   //一般情况矫正
//#define normalBOUND_correction 7   //一般情况矫正
#define test2_correction 8   //上台阶第一阶段纠正
#define test3_correction 9  //上台阶第二阶段纠正
//#define test4_correction 10  //跨绳子
#define test5_correction 11  //上台阶第二阶段纠正
#define test9_correction 12  //上台阶第二阶段纠正
#define normalTROT2_correction 13   //一般情况矫正
#define normalTROT3_correction 14   //一般情况矫正
#define test6_correction 15  //上台阶第二阶段纠正
#define climbing2_correction 16  //上台阶第二阶段纠正

/*偏航角纠正*/
extern float yaw_now;
extern float yaw_set;
extern	float pitch,roll,yaw; 	        //欧拉角


extern int LinearCorrection;
extern int BalanceCorrection;

void navigation_execute(void);


#endif
