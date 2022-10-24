#ifndef MOTO_CTRL_H
#define MOTO_CTRL_H


#define IsReady  1
#define NotReady 0



void moto_behaviour(void);
void pid_param_init(void);
void coordinate_trans(float X,float Y);
typedef struct
{
    float ref_agle[8];
    float out[8];
} temp_data;

extern int pid_spd_out_limit;
extern int given_current_limit;
extern int IsMotoReadyOrNot;
extern float ref_agle[8];
extern temp_data temp_pid;
extern float test_speed;
#endif

