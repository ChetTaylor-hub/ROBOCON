#include "posture.h"
#include "delay.h"
#include "combinations.h"
#include "pid.h"
#include "FreeRTOS.h"
#include "task.h"
#include "moto_ctrl.h"
#include "jump.h"
#include "navigation.h"

float x, y, theta1, theta2;
//float now_time=0.0f;
enum States state = REALSE;
#define PI 3.14159265358979f
int keynum;

//extern TaskHandle_t MotorControlTask_Handler;

float _angle_initial,_rotate_angle;

int TurnLeftFlag1,TurnLeftFlag2,TurnRightFlag1,TurnRightFlag2,ClimbingFlag1,ClimbingFlag2;

int _leg_active[4]= {1,1,1,1};

int rc_ctrl_flag = 0;
int kuaibunum=3;
//初始化步态增益
LegGain gait_gains = {15.5, 0.00, 25.0, 0.00};
//设定每一种步态的具体增益
LegGain state_gait_gains[] = {
    //{kp_pos, kd_pos} kp_spd, kd_spd,
    {22.0, 0.00, 8.0, 0.00},  //位置环 速度环 TROT
    {22.0, 0.00, 8.0, 0.00},  //TEST1
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},  //TEST12
    {22.0, 0.00, 8.0, 0.00},  //CLIMB
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00}, //STOP
    {22.0, 0.00, 8.0, 0.00}, //REL
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
    {22.0, 0.00, 8.0, 0.00},
		{22.0, 0.00, 8.0, 0.00},
//		{22.0, 0.00, 8.0, 0.00},
};


//初始化步态参数
GaitParams gait_params = {17.3,12.0,4.0,0.00,0.25,1};
//设定每一种步态的具体参数
GaitParams state_gait_params[] = {
    //{stance_height, step_length, up_amp, down_amp, flight_percent, freq}  cm
    //{步高, 步长, 抬腿高, 压腿高, t, 频率}	单位 cm
    {16.0, 16.0, 5.00, 2.00, 0.25, 1.85}, // TROT	双拍步态 对角小跑
    {26.0, 10.00, 3, 2.00, 0.20, 2.0},//1
    {26.0, 10.00, 3, 2.00, 0.20, 2.0},//2
    {26.0, 10.00, 3, 2.00, 0.20, 2.0},//3
    {NAN, NAN, NAN, NAN, NAN, NAN},		//4
    {NAN, NAN, NAN, NAN, NAN, NAN},		//5
    {NAN, NAN, NAN, NAN, NAN, NAN},		//6
    {NAN, NAN, NAN, NAN, NAN, NAN},		//7
    {NAN, NAN, NAN, NAN, NAN, NAN},		//8
    {NAN, NAN, NAN, NAN, NAN, NAN},		//9
//    {18.0, 5.00, 3, 2.00, 0.25, 2.0}, // ROTATE LEFT			{23.0, 10.00, 3, 2.00, 0.20, 2.0},  {21.0, 16.0, 5.00, 3.00, 0.25, 2.3},
//    {18.0, 5.00, 3, 2.00, 0.25, 2.0},	// ROTATE RIGHT			{23.0, 10.00, 3, 2.00, 0.20, 2.0},
   {21.0, 7.00, 3, 2.00, 0.25, 2.0}, // ROTATE LEFT  
   {21.0, 7.00, 3, 2.00, 0.25, 2.0}, // ROTATE RIGHT   		
  {NAN, NAN, NAN, NAN, NAN, NAN},		//12

    {16.3, 0.00, 5.70, 3.00, 0.30, 2.8}, // CLIMBING	爬坡

    {16.3, 0.00, 6.0, 0.00, 2.5, 1.5}, // WALK
    {16.0, 16.0, 5.00, 2.00, 0.25, 1.85}, // WALK_BACK

    {23.0, 7.00, 3, 2.00, 0.25, 2.0}, // ROTATE LEFT			{23.0, 10.00, 3, 2.00, 0.20, 2.0},  {21.0, 16.0, 5.00, 3.00, 0.25, 2.3},
    {21.0, 7.00, 5, 2.00, 0.25, 2.0},	// ROTATE RIGHT			{23.0, 10.00, 3, 2.00, 0.20, 2.0},

    {17.0, 0.00, 6.00, 4.00, 0.35, 2.0}, //BOUND
    {17.0, 0.00, 6.00, 0.00, 0.35, 2.0}, // GALLOP
    {16.0, 16.0, 5.00, 2.00, 0.25, 1.85}, // TROT2	双拍步态 对角小跑
		{16.0, 16.0, 5.00, 2.00, 0.25, 1.85}, // TROT2	双拍步态 对角小跑
    {21.0, 5.00, 3, 2.00, 0.25, 2.0}, // LEFT25			{23.0, 10.00, 3, 2.00, 0.20, 2.0},  {21.0, 16.0, 5.00, 3.00, 0.25, 2.3},
    {21.0, 5.00, 3, 2.00, 0.25, 2.0},	// RIGHT25			{23.0, 10.00, 3, 2.00, 0.20, 2.0},

    {NAN, NAN, NAN, NAN, NAN, NAN}, // STOP
    {NAN, NAN, NAN, NAN, NAN, NAN}, // RELASE
    {NAN, NAN, NAN, NAN, NAN, NAN}, // JUMP
    {NAN, NAN, NAN, NAN, NAN, NAN}, // START
    {NAN, NAN, NAN, NAN, NAN, NAN}, // END

//    {25.3, 16.0, 15.00, 0.00, 0.30, 1.2} ,// TEST
//		{16.3,0.00,6.0,0.00,2.5,1.5}  //STAIR
};


/**
*	最低walk {12.0, 15.0, 1.8, 0.00, 0.50, 1.0}
*	效果不错{14.0, 12.0, 3.5, 0.00, 0.50, 1.0},
*	{16.0, 12.00, 5.0, 0.00, 0.20, 2.0}, // WALK		ahead 不错
*	{17.3, 12.00, 4.0, 0.00, 0.35, 2.5}, // WALK		ahead
* 对角小跑步态不错速度和稳定都非常好
*/

GaitParams test_gait_params= {16.0, 15.0, 5.00, 2.00, 0.25, 2.3}; // TROT		//双拍步态 对角小跑
LegGain test_gait_gains= {15.5, 0.00, 25.0, 0.00};


const float TROT_stance_height=26.0;
const float TROT_step_length=25.0;
const float TROT_up_amp=15.2;
const float TROT_down_amp=0.0;
const float TROT_flight_percent=0.20;
const float TROT_freq=0.2;

const float TROT_step_length2=25.0;
const float TROT_up_amp2=15.2;


//        {26.0, 25.00, 15.0, 0.00, 0.20, 0.2},
//        {26.0, 25.00, 15.0, 0.00, 0.20, 0.2},

//        {26.0, 25.00-15, 15.0-13, 0.00, 0.20, 0.2},
//        {26.0, 25.00-15, 15.0-13, 0.00, 0.20, 0.2}


DetachedParam detached_params;
DetachedParam state_detached_params[] = {

    { 
/////////////////
		//{stance_height, step_length, up_amp, down_amp, flight_percent, freq}  cm
    //{步高, 步长, 抬腿高, 压腿高, t, 频率}	单位 cm
//			//TROT 对角小跑  /*-此处为20m竞速跑参数*/
			  {21.0, 12.0, 5.00, 2.00, 0.25, 3.0},//快步
			  {21.0, 12.0, 5.00, 2.00, 0.25, 3.0},
			  {21.0, 12.0, 5.00, 2.00, 0.25, 3.0},
			  {21.0, 12.0, 5.00, 2.00, 0.25, 3.0},
//				/*-此处为20m竞速跑参数*/
////////////////////////////////////
//				{21.0, 5.0, 0.00, 2.00, 0.25, 1.5},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}//卵石路
//        {21.0, 5.0, 0.00, 2.00, 0.25, 1.5},
//        {21.0, 5.0, 0.00, 2.00, 0.25, 1.5},
//        {21.0, 5.0, 0.00, 2.00, 0.25, 1.5},

////				{14.0, 3.0, 1.00, 0.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
////        {14.0, 3.0, 1.00, 0.00, 0.25, 2.0},
////			  {34.0, 3.0, 10.00, 0.00, 0.25, 2.0},
////        {34.0, 3.0, 10.00, 0.00, 0.25, 2.0},
    },
    {   // 下坡CLIMBING_DOWN
			  {21.0, 9.00, 4, 1.00, 0.25, 2.5},   // 20  9  5  1  0.25  2.5
        {21.0, 9.00, 4, 1.00, 0.25, 2.5},   //  18  9  5 1     21  9  5  5可以自己慢慢爬

        {18.0, 9.00, 4, 1.00, 0.25, 2.5},
        {18.0, 9.00, 4, 1.00, 0.25, 2.5}

//        {25.0, 10.00, 3, 2.00, 0.25, 3.8},
//        {25.0, 10.00, 3, 2.00, 0.25,3.8},

//        {25.0, 10.00, 3, 2.00, 0.25, 3.8},
//        {25.0, 10.00, 3, 2.00, 0.25, 3.8}
    },
    {   //TEST2-上台阶第一阶段
/*上台阶第一阶段参数*/
				{14.0, 3.0, 1.00, 0.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
        {14.0, 3.0, 1.00, 0.00, 0.25, 2.0},
			  {34.0, 3.0, 5.00, 0.00, 0.25, 2.0},
        {34.0, 3.0, 5.00, 0.00, 0.25, 2.0},
/*上台阶第一阶段参数*/

    },
    {   //TEST3  -上台阶第二阶段
			  {14.0, 12.0, 2.00, 2.00, 0.25, 2.0},
        {14.0, 12.0, 2.00, 2.00, 0.25, 2.0},
				{23.0, 7.0, 9.00, 2.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
        {23.0, 7.0, 9.00, 2.00, 0.25, 2.0},				

    },
    {   // TEST4
        {21.0, 12.0, 2.00, 2.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
        {21.0, 12.0, 2.00, 2.00, 0.25, 2.0},
        {21.0, 12.0, 2.00, 2.00, 0.25, 2.0},
        {21.0, 12.0, 2.00, 2.00, 0.25, 2.0}
    },
    {   // TEST5
			  {15.0, 5.0, 2.00, 2.00, 0.25, 2.0},
        {15.0, 5.0, 2.00, 2.00, 0.25, 2.0},
				{26.0, 5.0, 3.00, 2.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
        {26.0, 5.0, 3.00, 2.00, 0.25, 2.0},				
    },
    {   // TEST6
				{26.0, 10.0, 3.00, 2.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
        {26.0, 10.0, 3.00, 2.00, 0.25, 2.0},				
			  {16.0, 10.0, 3.00, 2.00, 0.25, 2.0},
        {16.0, 10.0, 3.00, 2.00, 0.25, 2.0},
    },
    {   // TEST7       //调整方向 小碎步转弯 左
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1}
    },
    {   // TEST8			//调整方向 小碎步转弯 右
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1}
    },
    {   // TEST9
			  {18.0, 5.0, 3.00, 2.00, 0.25, 2.0},//快步,频率2.0
			  {18.0, 5.0, 3.00, 2.00, 0.25, 2.0},
			  {18.0, 5.0, 3.00, 2.00, 0.25, 2.0},
			  {18.0, 5.0, 3.00, 2.00, 0.25, 2.0},
    },
    {   // TEST10
        {28.0, 4.00, 3, 0.00, 0.25, 2.3},
        {28.0, 4.00, 3, 0.00, 0.25,2.3},

        {28.0, 4.00, 3, 0.00, 0.25, 2.3},
        {28.0, 4.00, 3, 0.00, 0.25, 2.3}
    },
    {   // TEST11
        {28.0, 4.00, 3, 0.00, 0.25, 2.3},
        {28.0, 4.00, 3, 0.00, 0.25,2.3},

        {28.0, 4.00, 3, 0.00, 0.25, 2.3},
        {28.0, 4.00, 3, 0.00, 0.25, 2.3}
    },
    {   // TEST12
        {20.3, 30.00, 5.70, 3.00, 0.44, 2.8},
        {20.3, 30.00, 5.70, 3.00, 0.44, 2.8},

        {20.3, 30.00, 5.70, 3.00, 0.44, 2.8},
        {20.3, 30.00, 5.70, 3.00, 0.44, 2.8},
    },

    {   //CLIMBING	爬坡-上坡

        {18.0, 9.00, 3, 1.00, 0.35, 3.0},   // 
        {18.0, 9.00, 3, 1.00, 0.35, 3.0},   //  

        {21.0, 9.00, 3, 1.00, 0.35, 3.0},
        {21.0, 9.00, 3, 1.00, 0.35, 3.0}   //


//				{16.0, 16.0, 3.00, 1.00, 0.25, 3.0},		//大步子迈上去可以
//        {16.0, 16.0, 3.00, 1.00, 0.25, 3.0},
//
//        {17.0, 16.0, 3.00, 1.00, 0.25, 3.0},
//        {17.0, 16.0, 3.00, 1.00, 0.25, 3.0}

    },

    {
        {25, 8.00, 6.0, 0.00, 0.20, 1.0}, // WALK   {27.3, 12.00, 16.0, 0.00, 0.20, 1.0}, // WALK
        {25, 8.00, 6.0, 0.00, 0.20, 1.0}, // WALK
        {25, 8.00, 6.0, 0.00, 0.20, 1.0}, // WALK
        {25, 8.00, 6.0, 0.00, 0.20, 1.0} // WALK
    },

    {
        {21.0, 12.0, 5.00, 2.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
        {21.0, 12.0, 5.00, 2.00, 0.25, 2.0},
        {21.0, 12.0, 5.00, 2.00, 0.25, 2.0},
        {21.0, 12.0, 5.00, 2.00, 0.25, 2.0}
    },

    {
        {16.0, 7.00, 3, 2.00, 0.25, 1.5}, // ROTATE LEFT
        {16.0, 7.00, 3, 2.00, 0.25, 1.5}, // ROTATE LEFT
        {26.0, 7.00, 3, 2.00, 0.25, 1.5}, // ROTATE LEFT
        {26.0, 7.00, 3, 2.00, 0.25, 1.5} // ROTATE LEFT
    },

    {
        {16.0, 7.00, 3, 2.00, 0.25, 1.5},	// ROTATE RIGHT
        {16.0, 7.00, 3, 2.00, 0.25, 1.5},	// ROTATE RIGHT
        {26.0, 7.00, 3, 2.00, 0.25, 1.5},	// ROTATE RIGHT
        {26.0, 7.00, 3, 2.00, 0.25, 1.5}	// ROTATE RIGHT
    },

    {
        {18.0, 9.00, 5, 1.00, 0.35, 2.5},   //CLIMBING2
        {18.0, 9.00, 5, 1.00, 0.35, 2.5},//CLIMBING2
        {21.0, 9.00, 5, 1.00, 0.35, 2.5},//CLIMBING2
        {21.0, 9.00, 5, 1.00, 0.35,2.5},//CLIMBING2
    },

    {
        {21.0, 16.00, 6.00, 1.00, 0.25, 3.8}, // GALLOP
        {21.0, 16.00, 6.00, 1.00, 0.25, 3.8}, // GALLOP
        {21.0, 16.00, 6.00, 1.00, 0.25, 3.8}, // GALLOP
        {21.0, 16.00, 6.00, 1.00, 0.25, 3.8} // GALLOP
    },
		{
			  {21.0, 12.0, 5.00, 2.00, 0.25, 2.0},//TORT2
			  {21.0, 12.0, 5.00, 2.00, 0.25, 2.0},
			  {21.0, 12.0, 5.00, 2.00, 0.25, 2.0},
			  {21.0, 12.0, 5.00, 2.00, 0.25, 2.0}
		},
		{
				{21.0, 5.0, 2.00, 2.00, 0.25, 1.5},  //TROT3
        {21.0, 5.0, 2.00, 2.00, 0.25, 1.5},
        {21.0, 5.0, 2.00, 2.00, 0.25, 1.5},
        {21.0, 5.0, 2.00, 2.00, 0.25, 1.5},
		},
		{
        {18.0, 9.00, 5, 1.00, 0.35, 2.5},   //CLIMBING2
        {18.0, 9.00, 5, 1.00, 0.35, 2.5},//CLIMBING2
        {21.0, 9.00, 5, 1.00, 0.35, 2.5},//CLIMBING2
        {21.0, 9.00, 5, 1.00, 0.35,2.5},//CLIMBING2
		},
		{
        {18.0, 9.00, 5, 1.00, 0.35, 2.5},   //CLIMBING2
        {18.0, 9.00, 5, 1.00, 0.35, 2.5},//CLIMBING2
        {21.0, 9.00, 5, 1.00, 0.35, 2.5},//CLIMBING2
        {21.0, 9.00, 5, 1.00, 0.35,2.5},//CLIMBING2
		},


};

/*
																					0----------1
																					-          -
																					-          -
																					-          -
																					-          -
																					2----------3
*/
DetachedParam RcDetachedParam = {
    {21.0, 0.0, 3.00, 2.00, 0.25, 3.2},
    {21.0, 0.0, 3.00, 2.00, 0.25, 3.2},
    {21.0, 0.0, 3.00, 2.00, 0.25, 3.2},
    {21.0, 0.0, 3.00, 2.00, 0.25, 3.2}
};
int jump_flag=0;
/*******************************************************************************************
	*@ 任务名称：void PostureControl_task(void *pvParameters)
	*@ 功能：姿态控制
	*@ 备注：选择状态
 *******************************************************************************************/
void PostureControl_task(void *pvParameters)
{
    for(;;)
    {	
        gait_params = state_gait_params[state];
        detached_params = state_detached_params[state];
        switch(state) 
				{
        case STOP:		//21CM暂停 ps_key[7] 
            x=0;
            y = 21;
						pid_spd_out_limit=3000;//原3000
						CartesianToTheta(1.0);
            LegGain gains = {10, 0.00, 5 , 0.00};       //刚度小，阻尼大
            //pid_spd_out_limit=400;
            CommandAllLegs(gains);
            break;

        case REALSE:		// 释放 什么都不做
            vTaskDelay(500);
            break;

        case CLIMBING:		//爬坡  ps_key[2], mode2
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0,1.0, 1.0, 1.0);  //符号为  -
            break;

        case WALK:		//前进
            gait_detached(detached_params, 0.75, 0.25, 0.0, 0.5, 1.0, 1.0, 1.0, 1.0);
            break;

        case WALK_BACK:		//后退  ps_key[14], mode2
					  gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, -1.0, -1.0, -1.0, -1.0);//leg_offset应该相同？？？
            break;

        case CLIMBING2:		//上坡方法2  ps_key[3], mode2
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.00, 1.0, 1.0, 1.0, 1.0);
            break;

        case GALLOP:		//袭步
            gait_detached(detached_params,0.9, 0.7, 0.2, 0.0, 1.0, 1.0, 1.0, 1.0);
            break;

        case ROTAT_LEFT:	//一阶左转  ps_key[5] ,mode0
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.00, -1.0, 1.0, -1.0, 1.0); //未分离的步态

            break;
        case ROTAT_RIGHT:		//一阶右转  ps_key[6],mode0
            gait_detached(detached_params,0.5, 0.0, 0.0, 0.5, 1.0, -1.0, 1.0, -1.0); //未分离的步态

            break;
        case JUMP:		//跳跃
            ExecuteJumpup();
            break;

        case START:		//初始化
            StartPosToMiddlePos();
            break;

        case END:		//结束
            MiddlePosToEndPos();
            break;
				//后腿比前腿低
        case TROT:	//3.0竞速跑
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offset应该相同？？？
            break;

        case CLIMBING_DOWN:		//下坡
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);
            break;

        case TEST2:		 //快步上台阶20cm//
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offset应该相同？？？ 
				break;

        case TEST3:		 //一阶暂停10cm
            x=0;
	          y=30;
			      CartesianToTheta(1.0);
	          SetCoupledPosition(2);
            SetCoupledPosition(3);
			
			      x=0;
	          y=20;
			      CartesianToTheta(1.0);
	          SetCoupledPosition(0);
            SetCoupledPosition(1);	
			      vTaskDelay(500);

            break;

        case TEST4:	    //两级台阶暂停状态
					
						x=0;
            //y = 17.3205081;
            y = 34;
						pid_spd_out_limit=3000;
            CartesianToTheta(1.0);
            //pid_spd_out_limit=400;
	          SetCoupledPosition(2);
            SetCoupledPosition(3);	

            x=0;
            //y = 17.3205081;
            y = 14;
						pid_spd_out_limit=3000;
            CartesianToTheta(1.0);
            //pid_spd_out_limit=400;
	          SetCoupledPosition(0);
            SetCoupledPosition(1);	
            break;

        case TEST5:		//一阶前进-快步上台阶10cm
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.00, 1.0, 1.0, 1.0, 1.0);
            break;

        case TEST6:		 //下双木桥 ps_key[11]
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.00, 1.0, 1.0, 1.0, 1.0);
            break;

        case TEST7:		 //一级台阶暂停状态-下台阶
            x=0;
            //y = 17.3205081;
            y = 26;
						pid_spd_out_limit=3000;
						CartesianToTheta(1.0);
//            LegGain gains = {10, 0.00, 5 , 0.00};       //刚度小，阻尼大
            //pid_spd_out_limit=400;
	          SetCoupledPosition(0);
            SetCoupledPosition(1);	
						
	          x=0;
            //y = 17.3205081;
            y = 15;
						pid_spd_out_limit=3000;
						CartesianToTheta(1.0);
            //pid_spd_out_limit=400;
	          SetCoupledPosition(2);
            SetCoupledPosition(3);	
            break;

        case TEST8:		//测试步态
            ExecuteJumpdown();
            break;

        case TEST9:		//18cm前进--前后腿都在平衡木上的快步走
				    gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offset应该相同？？？

            break;

        case TEST10:		//21cm左转-原地
            gait(gait_params, gait_gains, 0.0, 0.5, 0.5, 0.00, -1.0, 1.0, -1.0, 1.0); //未分离的步态
            break;

        case TEST11:		 //21cm右转-原地
            gait(gait_params, gait_gains, 0.0, 0.5, 0.5, 0.00, 1.0, -1.0, 1.0, -1.0); //未分离的步态
            break;

        case TEST12:  //18cm暂停-前后腿都在平衡木上的状态
            x=3;
            y = 18;
						pid_spd_out_limit=3000;
						CartesianToTheta(1.0);
            //pid_spd_out_limit=400;
            CommandAllLegs(gains);
            break;
				case TORT2: //21cm快步
//					  gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offset应该相同
						gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offset应该相同？？�
            break;
				case TROT3: //海洋球快步
  					gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offset应该相同？？？
				    break;
				case CLIMBING_LEFT:   //上坡左转
  					gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, -1.0, 1.0, -1.0, 1.0);//leg_offset应该相同？？？
				    break;
				case CLIMBING_RIGHT: //上坡右转
  					gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, -1.0, 1.0, -1.0);//leg_offset应该相同？？？
				    break;
				case JUMP_PAIGUAN://排管跳跃
					  ExecuteJump();
				    break;
				case STOP25://25cm暂停--->改为15cm暂停大腿水平
						pid_spd_out_limit=1000;
						theta2=-70;
						theta1=50;
 
						CommandAllLegs_v();
						break;
				case STOP_TAIJIE:  //没用到,同TEST3
					  x=6;
						y =10;
						pid_spd_out_limit=1000;
						CartesianToTheta(1.0);

            CommandAllLegs(gains);
            break;

				case STOP18://未用到，同TEST12
            x=19;
						y = 23;
						pid_spd_out_limit=1000;
						jump_flag=1;
						CartesianToTheta(1.0);
            CommandAllLegs(gains);
						jump_flag=0;
						break;
				case STOP_QIAOBAN://跷跷板上的暂停状态
            x=0; //  0      
						y=18; // 18
						CartesianToTheta(1.0);
						SetCoupledPosition(0);
						SetCoupledPosition(1);
				 		x=0;   // 0
						y=22;  //28
						CartesianToTheta(1.0);
						SetCoupledPosition(2);
						SetCoupledPosition(3);
				    break;
        }
        vTaskDelay(10);
    }
}

/**
* NAME: void gait_detached
* FUNCTION : 四腿分离的腿部增益函数
*/

void gait_detached(	DetachedParam d_params,
                    float leg0_offset, float leg1_offset,float leg2_offset, float leg3_offset,
                    float leg0_direction, float leg1_direction,float leg2_direction, float leg3_direction) {

    float t = HAL_GetTick()/1000.0-now_time/1000.0;//now_time=0;t累加

    // const float leg0_direction = 1.0;
    if(_leg_active[0]==YES)
        CoupledMoveLeg( t, d_params.detached_params_0, leg0_offset, leg0_direction, 0);
																											//0.25
    // const float leg1_direction = 1.0;
    if(_leg_active[1]==YES)
        CoupledMoveLeg( t, d_params.detached_params_1, leg1_offset, leg1_direction, 1);
																											//0.75
    // const float leg2_direction = 1.0;
    if(_leg_active[2]==YES)
        CoupledMoveLeg( t, d_params.detached_params_2, leg2_offset, leg2_direction, 2);
																											//0.0
    //  const float leg3_direction = 1.0;
    if(_leg_active[3]==YES)
        CoupledMoveLeg( t, d_params.detached_params_3, leg3_offset, leg3_direction, 3);
																											//0.5
//	printf("时间t: %f  \r\n",t);
}

/**
* NAME: void gait(	GaitParams params,float leg0_offset, float leg1_offset,float leg2_offset, float leg3_offset)
* FUNCTION : 产生时间脉冲 设定每个腿的参数 调整腿的运行方向 进行补偿
*/
void gait(	GaitParams params,LegGain gains,
            float leg0_offset, float leg1_offset,float leg2_offset, float leg3_offset,
            float leg0_direction, float leg1_direction,float leg2_direction, float leg3_direction) {

//    if (!IsValidGaitParams(params) || !IsValidLegGain(gains)) {
//        return;
//    }

    float t = HAL_GetTick()/1000.0-now_time/1000.0;

    //printf("\r\n t=%f",t);

    // const float leg0_direction = 1.0;
    CoupledMoveLeg( t, params, leg0_offset, leg0_direction, 0);

    // const float leg1_direction = 1.0;
    CoupledMoveLeg( t, params, leg1_offset, leg1_direction, 1);

    // const float leg2_direction = 1.0;
    CoupledMoveLeg( t, params, leg2_offset, leg2_direction, 2);

    //  const float leg3_direction = 1.0;
    CoupledMoveLeg( t, params, leg3_offset, leg3_direction, 3);

    //改变PD
    // ChangeTheGainsOfPD(gains);
}

/**
* NAME: void CoupledMoveLeg(float t, GaitParams params,float gait_offset, float leg_direction, int LegId)
* FUNCTION : 驱动并联腿 传递参数
*/
void CoupledMoveLeg(float t, GaitParams params,float gait_offset, float leg_direction, int LegId)
{
    SinTrajectory(t, params, gait_offset);		//足端摆线轨迹生成器
    CartesianToTheta(leg_direction);		//笛卡尔坐标转换到伽马坐标


    SetCoupledPosition(LegId);		//发送数据给电机驱动函数
}

/*
* NAME: SinTrajectory (float t,GaitParams params, float gaitOffset)
* FUNCTION : 正弦轨迹生成器
*/
void SinTrajectory (float t,GaitParams params, float gaitOffset) {
    static float p = 0;
    static float prev_t = 0;

    float stanceHeight = params.stance_height; //狗身到地面的距离 (cm) walk 27.9/27.3
    float downAMP = params.down_amp;//下部振幅 (cm) walk 0
    float upAMP = params.up_amp;//上部振幅y (cm)   walk 16
    float flightPercent = params.flight_percent;//摆动相百分比 (cm) walk 0.2
    float stepLength = params.step_length;// 一步的距离 (cm)  walk 12
    float FREQ = params.freq;// 一步的频率 (Hz)  walk 1

    p += FREQ * (t - prev_t);//walk p累加速度慢 tort p累加速度快
    prev_t = t;

    float gp = fmod((p+gaitOffset),1.0);//求x/y的余数 都是double类型 gp<=1,x-[x/y]*y=fmod [x/y]取整
    if (gp <= flightPercent) {
        x = (gp/flightPercent)*stepLength - stepLength/2.0;
        y = -upAMP*sin(PI*gp/flightPercent) + stanceHeight;
    }
    else {
        float percentBack = (gp-flightPercent)/(1.0-flightPercent);
        x = -percentBack*stepLength + stepLength/2.0;
        y = downAMP*sin(PI*percentBack) + stanceHeight;
    }
}


/*
* NAME: void CartesianToThetaGamma(float leg_direction)
* FUNCTION : 笛卡尔坐标转换到角度坐标 也就是将xy转换成theta
*/
void CartesianToTheta(float leg_direction)
{
    float L=0;
    float N=0;
    double M=0;
    float A1=0;
    float A2=0;

	
		float L3=0,L4=0;
    float N1=0,N2=0;
    double M1=0,M2=0;
    float A3=0;
    float A4=0;
	
		L3=sqrt(		pow(x+1.75,2)	+		pow(y,2)	);//pow(x,y)计算x的y次方
		L4=sqrt(		pow(x-1.75,2)	+		pow(y,2)	);//pow(x,y)计算x的y次方
	
		//腿L3的上下限 还有带商榷
    if(L3<9) L3=9;//9  原12.5
    else if(L3>32) L3=32;//32   原37.5
		//腿L4的上下限 还有带商榷
		if(L4<9) L4=9;//9  原12.5
    else if(L4>32) L4=32;//32   原37.5
		
		//角度3
		N1=asin((x+1.75)/L3)*180.0/PI;//弧度变角度
    M1=acos(	(pow(L3,2)+pow(L1,2)-pow(L2,2))/(2*L1*L3)	)*180.0/PI;
		A3=M1-N1;//电机左侧
		//角度4
    N2=asin((x-1.75)/L4)*180.0/PI;//弧度变角度
    M2=acos(	(pow(L4,2)+pow(L1,2)-pow(L2,2))/(2*L1*L4)	)*180.0/PI;
    A4=M2+N2;//电机右侧

    if(leg_direction==1.0) {
        theta2=(A3-90.0);
        theta1=(A4-90.0);
    } else if(leg_direction==-1.0) {
        theta1=(A3-90.0);
        theta2=(A4-90.0);
    }
//    L=sqrt(		pow(x,2)	+		pow(y,2)	);//pow(x,y)计算x的y次方

//    if(L<12.5) L=9;//9  原12.5
//    else if(L>37.5) L=32;//32   原37.5

//    // vTaskSuspend(MotorControlTask_Handler);

//    N=asin(x/L)*180.0/PI;//弧度变角度
//    M=acos(	(pow(L,2)+pow(L1,2)-pow(L2,2))/(2*L1*L)	)*180.0/PI;
//    A1=M-N;

//    A2=M+N;

//    if(leg_direction==1.0) {
//        theta2=(A1-90.0);
//        theta1=(A2-90.0);
//    } else if(leg_direction==-1.0) {
//        theta1=(A1-90.0);
//        theta2=(A2-90.0);
//    }
}

int climbing_offset_flag=NO;
float _climbing_offset_angle=4360; //目前10°、5°效果最好，（倒着上）  10   12  13  15   8都试过
float climbing_zhunbei_angle=6540;

/**
* NAME: void SetCoupledPosition( int LegId)
* FUNCTION : 发送电机控制角度
*/
/**
* 电调ID设置 从左往右从上到下
* 1-----------------3
*  2---------------4
*  |               |
*	 |							 |
*  |							 |
*  |							 |
* 5-----------------7
*  6---------------8
*/

void SetCoupledPosition( int LegId)
{
    if(climbing_offset_flag==YES)
    {
        if(LegId==0)
        {
            temp_pid.ref_agle[1]=-theta1*ReductionAndAngleRatio+_climbing_offset_angle;
            temp_pid.ref_agle[0]=-theta2*ReductionAndAngleRatio-_climbing_offset_angle;
        }
        else if(LegId==1)
        {
            temp_pid.ref_agle[2]=theta1*ReductionAndAngleRatio-_climbing_offset_angle;
            temp_pid.ref_agle[3]=theta2*ReductionAndAngleRatio+_climbing_offset_angle;
        }

        else if(LegId==2)
        {
            temp_pid.ref_agle[5]=-theta1*ReductionAndAngleRatio+_climbing_offset_angle;
            temp_pid.ref_agle[4]=-theta2*ReductionAndAngleRatio-_climbing_offset_angle;
        }
        else if(LegId==3)
        {
            temp_pid.ref_agle[6]=+theta1*ReductionAndAngleRatio-_climbing_offset_angle;
            temp_pid.ref_agle[7]=+theta2*ReductionAndAngleRatio+_climbing_offset_angle;
        }
    }
		else if(climbing_offset_flag==2)   //  给负值四个马腿方向都正常，注意电调ID
    {
        if(LegId==0)   // + -
        {
            temp_pid.ref_agle[1]=-theta1*ReductionAndAngleRatio+climbing_zhunbei_angle;
            temp_pid.ref_agle[0]=-theta2*ReductionAndAngleRatio-climbing_zhunbei_angle;
        }
        else if(LegId==1)// -+
        {
            temp_pid.ref_agle[2]=+theta1*ReductionAndAngleRatio-climbing_zhunbei_angle;
            temp_pid.ref_agle[3]=+theta2*ReductionAndAngleRatio+climbing_zhunbei_angle;
        }

        else if(LegId==2) // +  -
        {
            temp_pid.ref_agle[5]=-theta1*ReductionAndAngleRatio+climbing_zhunbei_angle;
            temp_pid.ref_agle[4]=-theta2*ReductionAndAngleRatio-climbing_zhunbei_angle;
        }
        else if(LegId==3) // -  +
        {
            temp_pid.ref_agle[6]=+theta1*ReductionAndAngleRatio-climbing_zhunbei_angle;
            temp_pid.ref_agle[7]=+theta2*ReductionAndAngleRatio+climbing_zhunbei_angle;
        }
    }

    else {
			//A3=M1-N1;电机左侧   A4=M2+N2;//电机右侧
			//一般情况: 
			//theta2=(A3-90.0);   theta1=(A4-90.0);
			
				if(LegId==0)
        {
            temp_pid.ref_agle[1]=theta2*ReductionAndAngleRatio;//电调2 正对电机靠右侧 俯视靠内
            temp_pid.ref_agle[0]=-theta1*ReductionAndAngleRatio;//电调1 正对电机靠左侧 俯视靠外
//						temp_pid.ref_agle[1]=-theta2*ReductionAndAngleRatio; 
//						temp_pid.ref_agle[0]=theta1*ReductionAndAngleRatio;  
        }
        else if(LegId==1)
        {
//            temp_pid.ref_agle[2]=theta1*ReductionAndAngleRatio;//电调3 正对电机靠左侧 俯视靠外
//            temp_pid.ref_agle[3]=theta2*ReductionAndAngleRatio;//电调4 正对电机靠右侧 俯视靠内
						temp_pid.ref_agle[2]=theta1*ReductionAndAngleRatio;//电调3 正对电机靠左侧 俯视靠外
            temp_pid.ref_agle[3]=-theta2*ReductionAndAngleRatio;//电调4 正对电机靠右侧 俯视靠内
        }

        else if(LegId==2)
        {
            temp_pid.ref_agle[5]=theta2*ReductionAndAngleRatio;
            temp_pid.ref_agle[4]=-theta1*ReductionAndAngleRatio;
        }
        else if(LegId==3)
        {
            temp_pid.ref_agle[6]=theta1*ReductionAndAngleRatio;
            temp_pid.ref_agle[7]=-theta2*ReductionAndAngleRatio;
        }
			
			
    }
    IsMotoReadyOrNot= IsReady;		//数据填充完毕

}

/**
* NAME: void CommandAllLegs(void)
* FUNCTION : 控制所有电机
*/
void CommandAllLegs(LegGain gains)
{
    ChangeTheGainsOfPD(gains);

    SetCoupledPosition(0);
    SetCoupledPosition(1);
    SetCoupledPosition(2);
    SetCoupledPosition(3);
}

void CommandAllLegs_v(void)
{
    SetCoupledPosition(0);
    SetCoupledPosition(1);
    SetCoupledPosition(2);
    SetCoupledPosition(3);
}

/**
 *改变腿部增益
 *调用了PID_reset_kpKd 函数
 */
void ChangeTheGainsOfPD(LegGain gains)
{

    for (int i = 0; i < 8; i++) {
        pid_reset_kpkd(&pid_pos[i],gains.kp_pos,gains.kd_pos);
        pid_reset_kpkd(&pid_spd[i],gains.kp_spd,gains.kd_spd);

    }
}









