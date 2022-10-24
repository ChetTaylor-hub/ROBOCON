//#ifndef _COMMAND_H
//#define _COMMAND_H
//#include "sys.h"
//#include "pid.h"
//#include "usart.h"
//#include "math.h"
//#include "can.h"
//#include "delay.h"
//#include "led.h"
//#include "key.h"

//#define YES 1
//#define NO 0
//#define PI 3.1415
//#define ReductionAndAngleRatio  436.926337  //3591/187*8191/360=436.926337

//#define IsReady  1
//#define NotReady 0
//extern float ref_agle[8];

//extern float pitch,roll,yaw; 	        //欧拉角
//extern int IsMotoReadyOrNot;
////int IsMotoReadyOrNot= NotReady;
//extern int _leg_active[4];
//extern float now_time;
//extern float x, y, theta1, theta2;
//extern float temp_theta;
//extern u8 mode1;
//extern float node[8],node_flag[8];
//typedef struct  {		// 腿部PID增益结构体

//    float kp_pos;		//位置环
//    float kd_pos;

//    float kp_spd;		//速度环
//    float kd_spd;

//} LegGain;
//extern LegGain test_gait_gains;
//extern LegGain gait_gains;



//typedef struct  {		// 腿部参数结构体
//    float stance_height ; // 狗身到地面的距离 (cm)
//    float step_length ; // 一步的距离 (cm)
//    float up_amp ; // 上部振幅y (cm)
//    float down_amp ; // 下部振幅 (cm)
//    float flight_percent ; // 摆动相百分比 (cm)
//    float freq ; // 一步的频率 (Hz)
//} GaitParams;
//extern GaitParams gait_params;
//extern GaitParams state_gait_params[];
//////////extern GaitParams state_gait_params[];
////初始化步态参数

///////////单腿参数
//typedef struct {

//    GaitParams detached_params_0;
//    GaitParams detached_params_1;

//    GaitParams detached_params_2;
//    GaitParams detached_params_3;

//} DetachedParam;
//extern DetachedParam detached_params;
//extern DetachedParam state_detached_params[];
//extern DetachedParam  RcDetachedParam;

///*双拍步态 标志位PostureControl_task*/  
////void PostureControl_task(void);/*对角腿步态实现*/
////int rc_ctrl_flag = 1;

///*四腿分离的腿部增益函数*/
//void gait_detached(	DetachedParam d_params,
//                    float leg0_offset, float leg1_offset,float leg2_offset, float leg3_offset,
//                    float leg0_direction, float leg1_direction,float leg2_direction, float leg3_direction);
//void gait_detached_WALK(	DetachedParam d_params,
//                    float leg0_offset, float leg1_offset,float leg2_offset, float leg3_offset,
//                    float leg0_direction, float leg1_direction,float leg2_direction, float leg3_direction);
////void PostureControl_task(void);
//void PostureControl_WALK_task(void);
//									
//										
//										
//extern int climbing_offset_flag;
////void SinTrajectory (float t, GaitParams params, float gait_offset) ;
////void CartesianToTheta(float leg_direction) ;
////void SetCoupledPosition(int LegId);
////void CoupledMoveLeg(float t, GaitParams params,float gait_offset, float leg_direction, int LegId);
////void moto_behaviour(void);
//void stop(void);
//void gait(	GaitParams params,LegGain gait_gains,
//            float leg0_offset, float leg1_offset,float leg2_offset, float leg3_offset,
//            float leg0_direction, float leg1_direction,float leg2_direction, float leg3_direction);
//void task(void);
//void PID_stop(void);
//void CommandAllLegs(LegGain gains);



//#endif
