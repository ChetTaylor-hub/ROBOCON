#ifndef _PID_H
#define _PID_H
#include "sys.h"


void Motor_AngleCal(float T,short num);

extern float motor_err; 
extern float mode; 
extern float angle_a,angle_b;
void SetM3508(short ID1 ,short ID2, short ID3, short ID4) ;//ID代表电机ID,给各个电机电流，电机1-4
void SetM35082(short ID5 ,short ID6, short ID7, short ID8);   //电机5-8



enum {
    LLAST	= 0,
    LAST 	= 1,
    NOW 	= 2,

    POSITION_PID,
    DELTA_PID,
};
typedef struct __pid_t
{
    float p;
    float i;
    float d;

    float set[3];				//目标值,包含NOW， LAST， LLAST上上次
    float get[3];				//测量值
    float err[3];				//误差


    float pout;							//p输出
    float iout;							//i输出
    float dout;							//d输出

    float pos_out;						//本次位置式输出
    float last_pos_out;				//上次输出
    float delta_u;						//本次增量值
    float delta_out;					//本次增量式输出 = last_delta_out + delta_u
    float last_delta_out;

    float max_err;
    float deadband;				//err < deadband return
    uint32_t pid_mode;
    uint32_t MaxOutput;				//输出限幅
    uint32_t IntegralLimit;		//积分限幅

    void (*f_param_init)(struct __pid_t *pid,  //PID参数初始化
                         uint32_t pid_mode,
                         uint32_t maxOutput,
                         uint32_t integralLimit,
                         float p,
                         float i,
                         float d);
    void (*f_pid_reset)(struct __pid_t *pid, float p, float i, float d);		//pid三个参数修改

} pid_t;

void PID_struct_init(
    pid_t* pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,

    float 	kp,
    float 	ki,
    float 	kd);

float pid_calc(pid_t* pid, float fdb, float ref);
void pid_reset(pid_t	*pid, float kp, float ki, float kd);
void pid_reset_kpkd(pid_t	*pid, float kp, float kd);

		
extern pid_t pid_pos[8];
extern pid_t pid_spd[8];

extern pid_t pid_imu[3];
extern pid_t pid_track;
extern pid_t pid_rotate[2];
extern pid_t pid_climbing;
extern pid_t pid_test1;




#endif

