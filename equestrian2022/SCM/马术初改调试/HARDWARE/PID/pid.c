#include "pid.h"
#include "sys.h"
#include "can.h"
#include "usart.h"
#include "delay.h"
#include "bsp_can.h"
#include "math.h"
#define MOTOR_MAX 4

#define ABS(x)		((x>0)? (x): (-x))
void abs_limit(float *a, float ABS_MAX) {
    if(*a > ABS_MAX)
        *a = ABS_MAX;
    if(*a < -ABS_MAX)
        *a = -ABS_MAX;
}
/*?????--------------------------------------------------------------*/
static void pid_param_init(
    pid_t *pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    float 	kp,
    float 	ki,
    float 	kd)
{

    pid->IntegralLimit = intergral_limit;
    pid->MaxOutput = maxout;
    pid->pid_mode = mode;

    pid->p = kp;
    pid->i = ki;
    pid->d = kd;

}
/*中途更改参数----------------------------------------------------------*/
static void pid_reset(pid_t	*pid, float kp, float ki, float kd)
{
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}

void pid_reset_kpkd(pid_t	*pid, float kp, float kd)
{
    pid->p = kp;
    pid->d = kd;
}

float pid_calc(pid_t* pid, float get, float set) {
    pid->get[NOW] = get;
    pid->set[NOW] = set;
    pid->err[NOW] = set - get;	//set - measure
    if (pid->max_err != 0 && ABS(pid->err[NOW]) >  pid->max_err  )
        return 0;
    if (pid->deadband != 0 && ABS(pid->err[NOW]) < pid->deadband)
        return 0;

    if(pid->pid_mode == POSITION_PID) //???p
    {
        pid->pout = pid->p * pid->err[NOW];
        pid->iout += pid->i * pid->err[NOW];
        pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST] );
        abs_limit(&(pid->iout), pid->IntegralLimit);
        pid->pos_out = pid->pout + pid->iout + pid->dout;
        abs_limit(&(pid->pos_out), pid->MaxOutput);
        pid->last_pos_out = pid->pos_out;	//update last time
    }
    else if(pid->pid_mode == DELTA_PID)//???P
    {
        pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);
        pid->iout = pid->i * pid->err[NOW];
        pid->dout = pid->d * (pid->err[NOW] - 2*pid->err[LAST] + pid->err[LLAST]);

        abs_limit(&(pid->iout), pid->IntegralLimit);
        pid->delta_u = pid->pout + pid->iout + pid->dout;
        pid->delta_out = pid->last_delta_out + pid->delta_u;
        abs_limit(&(pid->delta_out), pid->MaxOutput);
        pid->last_delta_out = pid->delta_out;	//update last time
    }

    pid->err[LLAST] = pid->err[LAST];
    pid->err[LAST] = pid->err[NOW];
    pid->get[LLAST] = pid->get[LAST];
    pid->get[LAST] = pid->get[NOW];
    pid->set[LLAST] = pid->set[LAST];
    pid->set[LAST] = pid->set[NOW];
    return pid->pid_mode==POSITION_PID ? pid->pos_out : pid->delta_out;
//
}

float pid_sp_calc(pid_t* pid, float get, float set, float gyro) {
    pid->get[NOW] = get;
    pid->set[NOW] = set;
    pid->err[NOW] = set - get;	//set - measure


    if(pid->pid_mode == POSITION_PID) //???p
    {
        pid->pout = pid->p * pid->err[NOW];
        if(fabs(pid->i) >= 0.001f)
            pid->iout += pid->i * pid->err[NOW];
        else
            pid->iout = 0;
        pid->dout = -pid->d * gyro/100.0f;
        abs_limit(&(pid->iout), pid->IntegralLimit);
        pid->pos_out = pid->pout + pid->iout + pid->dout;
        abs_limit(&(pid->pos_out), pid->MaxOutput);
        pid->last_pos_out = pid->pos_out;	//update last time
    }
    else if(pid->pid_mode == DELTA_PID)//???P
    {
//        pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);
//        pid->iout = pid->i * pid->err[NOW];
//        pid->dout = pid->d * (pid->err[NOW] - 2*pid->err[LAST] + pid->err[LLAST]);
//
//        abs_limit(&(pid->iout), pid->IntegralLimit);
//        pid->delta_u = pid->pout + pid->iout + pid->dout;
//        pid->delta_out = pid->last_delta_out + pid->delta_u;
//        abs_limit(&(pid->delta_out), pid->MaxOutput);
//        pid->last_delta_out = pid->delta_out;	//update last time
    }

    pid->err[LLAST] = pid->err[LAST];
    pid->err[LAST] = pid->err[NOW];
    pid->get[LLAST] = pid->get[LAST];
    pid->get[LAST] = pid->get[NOW];
    pid->set[LLAST] = pid->set[LAST];
    pid->set[LAST] = pid->set[NOW];
    return pid->pid_mode==POSITION_PID ? pid->pos_out : pid->delta_out;
//
}
/*pid?????-----------------------------------------------------------------*/
void PID_struct_init(
    pid_t* pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,

    float 	kp,
    float 	ki,
    float 	kd)
{
    /*init function pointer*/
    pid->f_param_init = pid_param_init;
    pid->f_pid_reset = pid_reset;
//	pid->f_cal_pid = pid_calc;
//	pid->f_cal_sp_pid = pid_sp_calc;	//addition

    /*init pid param */
    pid->f_param_init(pid, mode, maxout, intergral_limit, kp, ki, kd);

}
pid_t pid_pos[8];
pid_t pid_spd[8];

pid_t pid_imu[3] = {0};
pid_t pid_track = {0};
pid_t pid_rotate[2] = {0};
pid_t pid_climbing;
pid_t pid_test1;







void SetM3508(short ID1 ,short ID2, short ID3, short ID4)   //?????????????
{
	  u8 sendMotorBuff[8];
    unsigned short can_id = 0x200;
    sendMotorBuff[0] = ( ID1 >> 8 ) ;
    sendMotorBuff[1] = ID1;

	  sendMotorBuff[2] = ( ID2 >> 8 );
    sendMotorBuff[3] = ID2 ;
	
    sendMotorBuff[4] = ( ID3 >> 8 ) ;
    sendMotorBuff[5] = ID3;


    sendMotorBuff[6] = ( ID4 >> 8 ) ;
    sendMotorBuff[7] = ID4;	
	  CAN1_Send_Msg(can_id,sendMotorBuff,8);
	  if(CAN1_Send_Msg(can_id,sendMotorBuff,8))
			CAN1_Receive_Msg(sendMotorBuff);

			
}

void SetM35082(short ID5 ,short ID6, short ID7, short ID8)   //?????????????
{
	  u8 sendMotorBuff[8];
    unsigned short can_id = 0x1FF;
    sendMotorBuff[0] = ( ID5 >> 8 ) ;
    sendMotorBuff[1] = ID5;

	  sendMotorBuff[2] = ( ID6 >> 8 );
    sendMotorBuff[3] = ID6 ;
	
    sendMotorBuff[4] = ( ID7 >> 8 ) ;
    sendMotorBuff[5] = ID7;


    sendMotorBuff[6] = ( ID8 >> 8 ) ;
    sendMotorBuff[7] = ID8;	
	  CAN2_Send_Msg(can_id,sendMotorBuff,8);
	  if(CAN2_Send_Msg(can_id,sendMotorBuff,8))
			CAN2_Receive_Msg(sendMotorBuff);
			
}





void run()
{
	SetM3508(moto_chassis[0].given_current,moto_chassis[1].given_current,moto_chassis[2].given_current,moto_chassis[3].given_current);
	SetM35082(moto_chassis[4].given_current,moto_chassis[5].given_current,moto_chassis[6].given_current,moto_chassis[7].given_current);
}



