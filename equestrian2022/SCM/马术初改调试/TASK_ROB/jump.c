/**
  ****************************(C) COPYRIGHT DJI****************************
  * @file       jump.c/h
  * @brief      跳跃实现函数
  ==============================================================================
  **************************** HBUT ROBOCON 2019****************************
  */

#include "jump.h"
#include "posture.h"
#include "remote_control.h"
#define PI 3.14159265358979f

float start_time_ = 0.0f;

float jump_angle = 31;

float jump_extension = 35.0f; // 最大伸腿长度 [cm]

void TrajectoryJump(float t, float launchTime, float stanceHeight, float downAMP) {
    //Need to check if n works
    float n = t/launchTime;
    x = 0;
    y = downAMP*n + stanceHeight;
    //y = downAMP*sin(PI/4 + PI/4*n) + stanceHeight;
}
void StartJumpshuiping(float start_time_s) {
	  start_time_ = start_time_s;
    state = JUMP_PAIGUAN;
}

void StartJump(float start_time_s) {
    start_time_ = start_time_s;
    state = JUMP;
}
void StartJumpdown(float start_time_s) {
    start_time_ = start_time_s;
    state = TEST8;
}
extern int jump_flag;
void ExecuteJump() {
    const float prep_time = 0.8f; // 准备时间 [s]		0.8
    const float launch_time = 0.2f ; // 收缩腿前的持续时间 [s]		0.2
    const float fall_time = 1.5f; //收缩腿后持续时间恢复正常行为 [s]		0.8

    const float stance_height = 15.8f; // 跳跃之前腿的高度 [cm]  14.2
    const float fall_extension = 16.0f; // 降落时的期望腿长 [cm]

    float t = HAL_GetTick()/1000.0f - start_time_/1000.0f; // 跳跃开始后的时间

    if (t < prep_time) {
//        x = -12*sin(21*PI/180);//-4
//        y = 16*cos(21*PI/180);//14.9
				x = 6;//-4   -4
        y = 10;//14.9  11
        CartesianToTheta(1.0);
        // 使用刚度小，阻尼大的增益
        LegGain gains = {5, 0.00, 8, 0.00};
        CommandAllLegs(gains);
    } else if (t >= prep_time && t < prep_time + launch_time) {
				
//        x = -jump_extension*sin(jump_angle*PI/180);//18
//        y = jump_extension*cos(jump_angle*PI/180);//30
				x = 19;//17   18  -15
        y = 23;//27   30
        CartesianToTheta(1.0);
        // 使用高刚度和低阻尼执行跳转
//				theta2=-70;
//				theta1=50;
        LegGain gains = {10.0, 0.00, 5.0, 0.00};
        CommandAllLegs(gains);

    } else if (t >= prep_time + launch_time && t < prep_time + launch_time + fall_time) {
        //使用低刚度和大量的阻尼来处理下降
//        LegGain gains = {10, 0.00, 1.8, 0.00};  //前位置环 后速度环
			  LegGain gains = {10, 0.00, 5, 0.00};  //前位置环 后速度环
        x = -1;//1
        y = 12;//11  16
        CartesianToTheta(1.0);
        CommandAllLegs(gains);
    } else {

				pid_spd_out_limit=3000;
        state = STOP;
//        printf("Jump Complete.\r\n");
    }
}

void ExecuteJumpup() {
    // min radius = 0.8
    // max radius = 0.25

    const float prep_time = 0.8f; // 准备时间 [s]		0.8
    const float launch_time = 0.2f ; // 收缩腿前的持续时间 [s]		0.2
    const float fall_time = 0.8f; //收缩腿后持续时间恢复正常行为 [s]		0.8

    const float stance_height = 16.0f; // 跳跃之前腿的高度 [cm]  14.2
    const float fall_extension = 16.0f; // 降落时的期望腿长 [cm]

//	// -----------------    用腿定位-------------///
//	 x = -jump_extension*sin(45*PI/180);
//        y = jump_extension*cos(45*PI/180);
//        CartesianToTheta(1.0);
//        CommandAllLegs_v();
//				vTaskDelay(200);
//

    float t = HAL_GetTick()/1000.0f - start_time_/1000.0f; // 跳跃开始后的时间

    if (t < prep_time) {
        x = -10*sin(21*PI/180);

        y = stance_height*cos(21*PI/180);
//        x = 0;
//        y = stance_height;
        CartesianToTheta(1.0);
        // 使用刚度小，阻尼大的增益
        LegGain gains = {5, 0.00, 8, 0.00};

        //CommandAllLegs_v();
        CommandAllLegs(gains);

    } else if (t >= prep_time && t < prep_time + launch_time) {

        x = -jump_extension*sin(jump_angle*PI/180);
        y = jump_extension*cos(jump_angle*PI/180);
//        x = 0;
//        y = jump_extension;

        CartesianToTheta(1.0);

        // 使用高刚度和低阻尼执行跳转
        LegGain gains = {10.0, 0.00, 5.0, 0.00};
        //CommandAllLegs_v();
        CommandAllLegs(gains);

    } else if (t >= prep_time + launch_time && t < prep_time + launch_time + fall_time) {
        //使用低刚度和大量的阻尼来处理下降
//        LegGain gains = {10, 0.00, 1.8, 0.00};  //前位置环 后速度环
			  LegGain gains = {10, 0.00, 5, 0.00};  //前位置环 后速度环

////////        x = 0;
//////////        y = fall_extension;
////////				y=21;
////////        CartesianToTheta(1.0);
////////				SetCoupledPosition(0);
////////        SetCoupledPosition(1);

////////				x=0;
////////				y=21;
//////////				y=26;
////////				CartesianToTheta(1.0);
////////				SetCoupledPosition(2);
////////        SetCoupledPosition(3);

          x = 0;
          y = fall_extension;
          CartesianToTheta(1.0);
				  SetCoupledPosition(0);
          SetCoupledPosition(1);

				  x=0;
				  y=26;
				  CartesianToTheta(1.0);
				  SetCoupledPosition(2);
          SetCoupledPosition(3);
    } 
}

void ExecuteJumpdown() {
    // min radius = 0.8
    // max radius = 0.25

    const float prep_time = 1.0f; // 准备时间 [s]		0.8
    const float launch_time = 0.2f ; // 收缩腿前的持续时间 [s]		0.2
    const float fall_time = 0.8f; //收缩腿后持续时间恢复正常行为 [s]		0.8

    const float stance_height = 16.0f; // 跳跃之前腿的高度 [cm]  14.2
    const float fall_extension = 16.0f; // 降落时的期望腿长 [cm]

//	// -----------------    用腿定位-------------///
//	 x = -jump_extension*sin(45*PI/180);
//        y = jump_extension*cos(45*PI/180);
//        CartesianToTheta(1.0);
//        CommandAllLegs_v();
//				vTaskDelay(200);
//

    float t = HAL_GetTick()/1000.0f - start_time_/1000.0f; // 跳跃开始后的时间

    if (t < prep_time) {
        x = -10*sin(21*PI/180);

        y = stance_height*cos(21*PI/180);
//        x = 0;
//        y = stance_height;
        CartesianToTheta(1.0);
        // 使用刚度小，阻尼大的增益
        LegGain gains = {5, 0.00, 8, 0.00};

        //CommandAllLegs_v();
        CommandAllLegs(gains);

    } else if (t >= prep_time && t < prep_time + launch_time) {

        x = -jump_extension*sin(jump_angle*PI/180);
        y = jump_extension*cos(jump_angle*PI/180);
//        x = 0;
//        y = jump_extension;

        CartesianToTheta(1.0);

        // 使用高刚度和低阻尼执行跳转
        LegGain gains = {10.0, 0.00, 5.0, 0.00};
        //CommandAllLegs_v();
        CommandAllLegs(gains);

    } else if (t >= prep_time + launch_time && t < prep_time + launch_time + fall_time) {
        //使用低刚度和大量的阻尼来处理下降
//        LegGain gains = {10, 0.00, 1.8, 0.00};  //前位置环 后速度环
			  LegGain gains = {10, 0.00, 5, 0.00};  //前位置环 后速度环
				  if(tri_mode==0)
					{
						x = 0;
            y = fall_extension;
            CartesianToTheta(1.0);
		        SetCoupledPosition(2);
            SetCoupledPosition(3);  

				    x=0;
				    y=26;
				    CartesianToTheta(1.0);
				    SetCoupledPosition(0);
            SetCoupledPosition(1);
					}
					
					if(tri_mode==1)
					{
            x = 0;
            y = fall_extension;
            CartesianToTheta(1.0);
				    SetCoupledPosition(2);
            SetCoupledPosition(3);  

				    x=0;
				    y=32;
				    CartesianToTheta(1.0);
				    SetCoupledPosition(0);
            SetCoupledPosition(1);
					}
    }
}

