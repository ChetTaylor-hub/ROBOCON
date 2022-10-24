#ifndef _PID_H
#define _PID_H
#include "sys.h"


void Motor_AngleCal(float T,short num);

extern float motor_err; 
extern float mode; 
extern float angle_a,angle_b;
void SetM3508(short ID1 ,short ID2, short ID3, short ID4) ;//ID������ID,������������������1-4
void SetM35082(short ID5 ,short ID6, short ID7, short ID8);   //���5-8



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

    float set[3];				//Ŀ��ֵ,����NOW�� LAST�� LLAST���ϴ�
    float get[3];				//����ֵ
    float err[3];				//���


    float pout;							//p���
    float iout;							//i���
    float dout;							//d���

    float pos_out;						//����λ��ʽ���
    float last_pos_out;				//�ϴ����
    float delta_u;						//��������ֵ
    float delta_out;					//��������ʽ��� = last_delta_out + delta_u
    float last_delta_out;

    float max_err;
    float deadband;				//err < deadband return
    uint32_t pid_mode;
    uint32_t MaxOutput;				//����޷�
    uint32_t IntegralLimit;		//�����޷�

    void (*f_param_init)(struct __pid_t *pid,  //PID������ʼ��
                         uint32_t pid_mode,
                         uint32_t maxOutput,
                         uint32_t integralLimit,
                         float p,
                         float i,
                         float d);
    void (*f_pid_reset)(struct __pid_t *pid, float p, float i, float d);		//pid���������޸�

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

