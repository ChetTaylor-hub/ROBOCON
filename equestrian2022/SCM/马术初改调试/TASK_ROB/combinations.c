#include "combinations.h"
#include "posture.h"
#include "command.h"
#include "moto_ctrl.h"
#include "delay.h"
#include "task.h"
#include "navigation.h"

float temp_theta;
#define PI 3.14159265358979f
//bool optoelec_switch[2];
u8 countnum=0;
/**
*组合动作 放在这里写了
*有：		JUMP		开始动作		结束动作		越障		过程逻辑
*/

//void StepOver(void)
//{

//    state=REALSE;				//释放
//    vTaskDelay(500);

//    StepOver_one_leg(0);  //跨0
//    vTaskDelay(100);

//    StepOver_one_leg(1);	//跨1
//    vTaskDelay(100);



//    state=REALSE;				//释放
//    vTaskDelay(150);

////    x = 18*sin(40*PI/180);		//前两个腿保持一致小小的站起来 后一点
////    y = 18*cos(40*PI/180);
////    CartesianToTheta(1.0);
////    SetCoupledPosition(0);
////    SetCoupledPosition(1);
////    vTaskDelay(100);

//    _leg_active[0]=NO;		//关闭前两个腿
//    _leg_active[1]=NO;
//    state=TEST4;				//前面低下来走
//    vTaskDelay(500);
//    _leg_active[0]=YES;		//打开前两个腿
//    _leg_active[1]=YES;
//    vTaskDelay(1500);

//    state=TEST6;				//小碎步
//    vTaskDelay(1000);

//    state=REALSE; 			//释放

//    // pid_spd_out_limit=  2500;

//    StepOver_one_leg(2);	//跨2

//    vTaskDelay(400);

//    StepOver_one_leg(3);	//跨3
//    vTaskDelay(400);

//    // pid_spd_out_limit=  6720;

//    state=REALSE;		//释放
//    vTaskDelay(500);

//    x = 18.6*sin(28*PI/180);		//后两个腿 小站 向前走
//    y = 18.6*cos(28*PI/180);
//    CartesianToTheta(1.0);
//    SetCoupledPosition(2);
//    SetCoupledPosition(3);
//    vTaskDelay(100);

//    _leg_active[2]=NO;		//关闭后两个腿
//    _leg_active[3]=NO;
//    state=TEST5; //前面正常 后面低下来走
//    vTaskDelay(500);

//    _leg_active[2]=YES;		//打开后两个腿
//    _leg_active[3]=YES;
//    vTaskDelay(1000);

//    state=REALSE;		//释放
//    vTaskDelay(250);

//    state=TROT;		    //小跑跑下沙丘

//    vTaskDelay(3000);


//    // pid_spd_out_limit=  6700;


//}


//void StepOver_one_leg(int LegId)
//{

//    //------四个腿子全部抬到最高-----//

//    if(LegId==0||LegId==2)
//    {
//        x = 0;
//        y = 28;
//        CartesianToTheta(1.0);
//        CommandAllLegs_v();
//        vTaskDelay(500);
//    }
//    else if(LegId==1||LegId==3)
//    {
//        vTaskDelay(200);
//    }

//    //--------对角腿缩最短--------//
//    int rev_legid, rev_legid1;
//    if(LegId==0) {
//        rev_legid=3 ;
//        y = 25;
//    }
//    else if(LegId==1) {
//        x = 0;
//        y = 28;
//        CartesianToTheta(1.0);
//        SetCoupledPosition(3);
//        vTaskDelay(300);

//        rev_legid=2 ;
//        y = 25 ;
//    }

//    else if(LegId==2) {
//        rev_legid=1 ;
//        y = 25;
//    }
//    else if(LegId==3) {
//        rev_legid=0 ;
//        y = 16;
//    }
//    x = 0;
//    CartesianToTheta(1.0);
//    SetCoupledPosition(rev_legid);
//    vTaskDelay(400);

//    //----只有leg3的时候运行----前三个腿进入平衡状态---//

//    if(LegId==3)
//    {

//        if(GROUND_SELECT==RED_GROUNG) {  //红蓝场判断 红场

//            x = -18*sin(17.2*PI/180);
//            y = 18*cos(17.2*PI/180);
//            CartesianToTheta(1.0);
//            SetCoupledPosition(0);

//            x = -21*sin(15*PI/180);
//            y = 21*cos(15*PI/180);
//            CartesianToTheta(1.0);
//            SetCoupledPosition(1);

//            x = 22*sin(10*PI/180);
//            y = 22*cos(40*PI/180)-4;		//高度保持
//            CartesianToTheta(1.0);
//            SetCoupledPosition(2);

//            vTaskDelay(1500);
//        }
//        else if(GROUND_SELECT==BLUE_GROUNG) {

//            x = -18*sin(17.2*PI/180);
//            y = 18*cos(17.2*PI/180);
//            CartesianToTheta(1.0);
//            SetCoupledPosition(0);

//            x = -21*sin(15*PI/180);
//            y = 21*cos(15*PI/180);
//            CartesianToTheta(1.0);
//            SetCoupledPosition(1);

//            x = 22*sin(10*PI/180);
//            y = 22*cos(40*PI/180)-4;		//高度保持
//            CartesianToTheta(1.0);
//            SetCoupledPosition(2);

//            vTaskDelay(1300);
//        }

//    }

//    //--------腿最短向后--------//
//    if(LegId==3)
//    {
//        x = -10*sin(80*PI/180);
//        y = 10*cos(80*PI/180);
//        CartesianToTheta(1.0);
//        SetCoupledPosition(LegId);
//        vTaskDelay(200);
//    }
//    else
//    {

////        x = -10*sin(60*PI/180);
////        y = 10*cos(60*PI/180);
////        CartesianToTheta(1.0);
////        SetCoupledPosition(LegId);
////        vTaskDelay(600);

//    }

//    //-------腿最短向前----------//
//    x = 10*sin(60*PI/180);
//    y = 10*cos(60*PI/180);
//    CartesianToTheta(1.0);
//    SetCoupledPosition(LegId);
//    vTaskDelay(260);

//    //--------腿最长状态向前--------//
//    x = 26*sin(80*PI/180);
//    y = 26*cos(80*PI/180);
//    CartesianToTheta(1.0);
//    SetCoupledPosition(LegId);
//    vTaskDelay(180);

//    //--------腿最长状态向下触地--------//
//    if(LegId==0||LegId==1)
//    {
//        x = 29*sin(34*PI/180);
//        y = 29*cos(34*PI/180);
//    }
//    else if(LegId==2)
//    {
//        x = 22*sin(40*PI/180);
//        y = 22*cos(40*PI/180);
//    }
//    else if(LegId==3)
//    {
//        x = 20*sin(50*PI/180);
//        y = 20*cos(50*PI/180);
//    }
//    CartesianToTheta(1.0);
//    SetCoupledPosition(LegId);
//    vTaskDelay(180);

//}


void CrossTheLinefirstleg(void)
{
	  pid_spd_out_limit=  3000;
	  state=REALSE;
    CrossTheLine_one_leg(0); //跨0
    vTaskDelay(50);

    CrossTheLine_one_leg(1);	//跨1
    vTaskDelay(50);
}

void CrossTheLinesecondleg(void)
{
	  state=REALSE;

    CrossTheLine_one_leg(2);		//跨2
    vTaskDelay(50);

    CrossTheLine_one_leg(3);		//跨3
    vTaskDelay(50);
}
//void CrossTheLine(void)
//{

//    pid_spd_out_limit=  3000;

//    state=REALSE;

//    CrossTheLine_one_leg(0); //跨0
//    vTaskDelay(50);

//    CrossTheLine_one_leg(1);	//跨1
//    vTaskDelay(50);

//    // now_time=HAL_GetTick();
//    LinearCorrection=normalTROT_correction;		//打开跨绳子时候的步态纠偏
//    state=TROT;  //------------向前走让后腿接触绳子
//    vTaskDelay(1600);
//    state=REALSE;

//	  if(ps_key[3]==1)
//		{
//			state=STOP;
//		}
//		if(ps_key[7]==1)
//		{
//      CrossTheLine_one_leg(2);		//跨2
//      vTaskDelay(50);

//      CrossTheLine_one_leg(3);		//跨3
//      vTaskDelay(50);
//		}

//    //**********中间行走**********//
//    // now_time=HAL_GetTick();
//		if(ps_key[2]==1)
//      state=TROT;

//    vTaskDelay(50);

//    //OpenMvInspect(openmv_Red);  //检测到红色之后开始迈第二根绳子

////    IndicateLED_On;

////    state=STOP;
////    state=REALSE;

////    //**********END**********//

////    CrossTheLine_one_leg(0);
////    vTaskDelay(50);

//////    IndicateLED_Off;

////    CrossTheLine_one_leg(1);
////    vTaskDelay(50);

////    // now_time=HAL_GetTick();
////    state=TEST4;  //跨绳子步态
////    vTaskDelay(1600);
////    state=REALSE;

////    CrossTheLine_one_leg(2);
////    vTaskDelay(50);

////    CrossTheLine_one_leg(3);
////    vTaskDelay(50);



//}

void CrossTheLine_one_leg(int LegId)
{

    //------四个腿子全部抬到最高-----//
    x = 0;
    y = 25;
    CartesianToTheta(1.0);
    CommandAllLegs_v();
    vTaskDelay(150);

    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

	  if(LegId==2)
		{
			x = 0;
      y =18;                 //21;
      CartesianToTheta(1.0);
      SetCoupledPosition(rev_legid); 
      vTaskDelay(800);               

      //当前腿先向下蹬一下保持平衡//
      x = 0;
      y = 28.9;
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(1000);

//      //--------腿最长到后面--------//
//      x = -20*sin(60*PI/180);
//      y = 20*cos(60*PI/180);
//      CartesianToTheta(1.0);
//      SetCoupledPosition(LegId);
//      vTaskDelay(140);

      //--------腿缩最短在后面--------//
      x = -14*sin(80*PI/180);
      y = 14*cos(80*PI/180);
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(220);

      //--------腿最短状态向前--------//
      x = 14*sin(60*PI/180);
      y = 14*cos(60*PI/180);
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(150);

      //--------腿伸长向前--------//
      x = 14*sin(55*PI/180);
      y = 19*cos(55*PI/180);
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(140);

      //--------腿最长向下触地--------//
      x = 29*sin(1*PI/180); //1°
      y = 29*cos(1*PI/180);
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(140);

      //------四个腿恢复初始位置-----//
      x = 0;
      y = 25;
      CartesianToTheta(1.0);
      CommandAllLegs_v();
      vTaskDelay(100);
		}
		else
		{
    x = 0;
    y = 18;
    CartesianToTheta(1.0);
    SetCoupledPosition(rev_legid); 
    vTaskDelay(800);               

    //当前腿先向下蹬一下保持平衡//
    x = 0;
    y = 28.9;
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(1000);

//    //--------腿最长到后面--------//
//    x = -20*sin(60*PI/180);
//    y = 20*cos(60*PI/180);
//    CartesianToTheta(1.0);
//    SetCoupledPosition(LegId);
//    vTaskDelay(140);

    //--------腿缩最短在后面--------//
    x = -14*sin(80*PI/180);
    y = 14*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);

    //--------腿最短状态向前--------//
    x = 14*sin(60*PI/180);
    y = 14*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(150);

    //--------腿伸长向前--------//
    x = 14*sin(55*PI/180);
    y = 19*cos(55*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(140);

    //--------腿最长向下触地--------//
    x = 29*sin(1*PI/180); //1°
    y = 29*cos(1*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(140);

    //------四个腿恢复初始位置-----//
    x = 0;
    y = 25;
    CartesianToTheta(1.0);
    CommandAllLegs_v();
    vTaskDelay(100);
	}
}

////void OpenMvInspect(int color)
//{

//    u8 _rel_time = 10;
//    u16 time_flag = 0;
//    while(1)
//    {
//        time_flag+=10;

//        if(time_flag==10000)
//            break;

//        vTaskDelay(_rel_time);
//        if(openmv2info.ActVal[2]==color)
//        {
//            vTaskDelay(_rel_time);
//            if(openmv2info.ActVal[2]==color)
//            {
//                vTaskDelay(_rel_time);
//                if(openmv2info.ActVal[2]==color)
//                {
//                    vTaskDelay(_rel_time);
//                    if(openmv2info.ActVal[2]==color)
//                    {
//                        vTaskDelay(_rel_time);
//                        if(openmv2info.ActVal[2]==color)
//                            break;
//                    }
//                }

//            }
//        }
//    }

//}

float OpenMv_Line_Dec(void)
{

//	float dev_calc_ang;
//
//	dev_calc_ang = pid_calc(&pid_openmv_dev,openmvinfo.ActVal[0],);
//
//
//	return
}

//void Climbing_Comb(void)
//{

//    IndicateLED_On;
//    //...上坡------------
//    while(keyInf1!=0) //第一段完成-------等待光电开关--进入爬坡程序-----
//        vTaskDelay(500);

//    IndicateLED_Off;

//    LinearCorrection=Deny;

//    LinearCorrection=climbing_correction;
//    //LinearCorrection=normal_correction;

//    yaw_set=imuinfo.ActVal[0];//设定当前角度为前进方向

//    // now_time=HAL_GetTick();
//    climbing_offset_flag=YES;
//    _climbing_offset_angle=15.0;

//    state= CLIMBING;



//		 while(1)		//停
//            vTaskDelay(500);



//    vTaskDelay(3000);  //延时3000




//    u16 time_flag = 0;

//    while(1)
//    {
//        time_flag+=10;

//        if(time_flag==10000)
//            break;

//        vTaskDelay(10);
//        if(openmv2info.ActVal[2]==openmv_Red)
//            break;

//    }
//    //OpenMvInspect(openmv_Red);  //等待检测到红色 色块 摄像头已经检测到了爬到了坡顶




//    IndicateLED_On;

//    // vTaskDelay(1500);

//    IndicateLED_Off;

//    state_detached_params[CLIMBING].detached_params_0.stance_height+=2;
//    state_detached_params[CLIMBING].detached_params_1.stance_height+=2;
//    vTaskDelay(150);

//    state_detached_params[CLIMBING].detached_params_0.stance_height+=2;
//    state_detached_params[CLIMBING].detached_params_1.stance_height+=2;
//    vTaskDelay(150);

//    state_detached_params[CLIMBING].detached_params_0.stance_height+=2;
//    state_detached_params[CLIMBING].detached_params_1.stance_height+=2;
//    vTaskDelay(150);

//    state_detached_params[CLIMBING].detached_params_0.stance_height+=2;
//    state_detached_params[CLIMBING].detached_params_1.stance_height+=2;
//    vTaskDelay(150);

//    state_detached_params[CLIMBING].detached_params_0.stance_height+=2;
//    state_detached_params[CLIMBING].detached_params_1.stance_height+=2;
//    vTaskDelay(150);

//    state= REALSE;
//    vTaskDelay(300);

//    x=0;
//    y = 24;
//    CartesianToTheta(1.0);
//    CommandAllLegs_v();

//    vTaskDelay(800);

//    CAN_RoboModule_DRV_Position_Mode(0,1,4700,2200*4*15.15);		//杆子升起来  2100

//vTaskDelay(600);
//    x=0;
//    y = 26;
//    CartesianToTheta(1.0);
//    CommandAllLegs_v();
//		
//		
//		vTaskDelay(900);
//    x=0;
//    y = 26;
//    CartesianToTheta(1.0);
//    CommandAllLegs_v();
//		
//		
//    state= REALSE;

//    vTaskDelay(5000);

//    CAN_RoboModule_DRV_Position_Mode(0,1,2500,0);

//}


//更改四腿运动延时，可以缩短上台阶时间
void shuangmubridge_one_leg(int LegId)
{
    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

    x = 0;
    y = 24; 
    CartesianToTheta(1.0);
    SetCoupledPosition(rev_legid); 
    vTaskDelay(800);               

    //当前腿先向下蹬一下保持平衡//
    x = 0;
    y = 28;
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(1000);

    //--------腿缩最短在后面--------//
    x = -13*sin(80*PI/180);
    y = 21*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);
				    //--------腿最短状态向前--------//
    x = 13*sin(60*PI/180);
    y = 13*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(400);
		
		
		x = 14;
    y = 16;
    CartesianToTheta(1.0);
    SetCoupledPosition(0);
	    vTaskDelay(1500);
	
		x=0;
    y=31;
		CartesianToTheta(1.0);
		SetCoupledPosition(2);
    SetCoupledPosition(3);
    vTaskDelay(1500);
}
void shuangmubridge_two_leg(int LegId)
{
	
    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

    x = 0;
    y = 26;
    CartesianToTheta(1.0);
    SetCoupledPosition(rev_legid); 
    vTaskDelay(800);               

    //当前腿先向下蹬一下保持平衡//
    x = 0;
    y = 27;
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(1000);

    //--------腿缩最短在后面--------//
    x = -13*sin(80*PI/180);
    y = 21*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);
				    //--------腿最短状态向前--------//
    x = 13*sin(60*PI/180);
    y = 13*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(400);
			
			x = 14;
      y = 16;
      CartesianToTheta(1.0);
      SetCoupledPosition(1);
    vTaskDelay(1500);
			
		x=0;
    y=31;
		CartesianToTheta(1.0);
    SetCoupledPosition(3);
    vTaskDelay(500);

	  if(tri_mode==1)           //当跨上一阶台阶后，使用这个参数！但向前蹬效果不明显 .countnum=0时，后腿会从第一阶台阶蹦下去，不可取
		{
			x=-9;
			y=31;
			CartesianToTheta(1.0);
      SetCoupledPosition(2);
			SetCoupledPosition(3);
			vTaskDelay(800);  
			
	     x=0;
	     y=36;
			 CartesianToTheta(1.0);
	     SetCoupledPosition(2);
       SetCoupledPosition(3);
	     vTaskDelay(800);  
			 
		}			 
//		 countnum++;
		 
			 


}
void shuangmubridge_three_leg(int LegId)  //第三条腿上台阶
{
	     x=0;
	     y=16;
			 CartesianToTheta(1.0);
	     SetCoupledPosition(0);
       SetCoupledPosition(1);
	     vTaskDelay(300); //////////////
	     x=0;
	     y=36;
			 CartesianToTheta(1.0);
	     SetCoupledPosition(2);
       SetCoupledPosition(3);
	     vTaskDelay(300); /////////////////////

	
    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

//      x = 14*sin(60*PI/180);
//      y = 14*cos(60*PI/180);
	    x=10;
	    y=13;
      CartesianToTheta(1.0);
      SetCoupledPosition(rev_legid); 
      vTaskDelay(800);               

      //当前腿先向下蹬一下保持平衡//
      x = 0;
      y = 37;
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(1000);

    //--------腿缩最短在后面--------//
    x = -13*sin(80*PI/180);
    y = 21*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);
				    //--------腿最短状态向前--------//
    x = 14*sin(60*PI/180);
    y = 14*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(150);
				

			x=10;
			y=28;
			CartesianToTheta(1.0);
      SetCoupledPosition(2);
			vTaskDelay(800);////////////////
			
			x=6;
			y=26;
			CartesianToTheta(1.0);
      SetCoupledPosition(2);
			vTaskDelay(800);///////////////////////////
			
			x = 5;
      y = 16;
      CartesianToTheta(1.0);
      SetCoupledPosition(0);
			SetCoupledPosition(1);
			vTaskDelay(800);		///////////////////////////	
}

void shuangmubridge_four_leg(int LegId)
{
			x = 0;
      y = 16;
      CartesianToTheta(1.0);
      SetCoupledPosition(0);
			SetCoupledPosition(1);
			vTaskDelay(500);///////////////
			
		x = 0;
    y = 26;
    CartesianToTheta(1.0);
    SetCoupledPosition(2);
    vTaskDelay(500);//////////////////////

	    //------四个腿子全部抬到最高-----//

    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

//    x = 14*sin(60*PI/180);
//    y = 14*cos(60*PI/180);
    x=7;
		y=12;
    CartesianToTheta(1.0);
    SetCoupledPosition(rev_legid); 
    vTaskDelay(800);               

      //当前腿先向下蹬一下保持平衡//
      x = 0;
      y = 37;
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(1000);

    //--------腿缩最短在后面--------//
    x = -13*sin(80*PI/180);
    y = 21*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);
				    //--------腿最短状态向前--------//
    x = 14*sin(60*PI/180);
    y = 14*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(150);


		x = 10;
    y = 26;
    CartesianToTheta(1.0);
    SetCoupledPosition(3);
    vTaskDelay(500);///////////////////

		x = 5;
    y = 26;
    CartesianToTheta(1.0);
    SetCoupledPosition(3);
    vTaskDelay(500);//////////////////////
		
		x = 0;
    y = 16;
    CartesianToTheta(1.0);
    SetCoupledPosition(0);
    vTaskDelay(500);////////////////////
      

//		x=0;
//		y=16;
//		CartesianToTheta(1.0);
//		SetCoupledPosition(0);
//		vTaskDelay(500);
//		
//		x=0;
//		y=16;
//		CartesianToTheta(1.0);
//		SetCoupledPosition(1);
//		SetCoupledPosition(2);
//		vTaskDelay(500);


}

void shuangmubridge()
{	
		if(keynum==1)
		{
		  pid_spd_out_limit=  3000;
	    state=REALSE;
      shuangmubridge_one_leg(0);
      vTaskDelay(50);
		}
		if(keynum==2)
		{
			pid_spd_out_limit=  3000;
	    state=REALSE;
      shuangmubridge_two_leg(1);
      vTaskDelay(50);
		}
		if(keynum==3)
		{
			pid_spd_out_limit=  3000;
	    state=REALSE;
      shuangmubridge_three_leg(2);
      vTaskDelay(50);

		}
		if(keynum==4)
		{
			pid_spd_out_limit=  3000;
	    state=REALSE;
      shuangmubridge_four_leg(3);
      vTaskDelay(50);
		}



}
void pinghengmu_one_leg(int LegId)
{
    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

    x = 0;
    y = 21; 
    CartesianToTheta(1.0);
    SetCoupledPosition(rev_legid); 
    vTaskDelay(800);               

    //当前腿先向下蹬一下保持平衡//
    x = 0;
    y = 25;
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(1000);

    //--------腿缩最短在后面--------//
    x = -13*sin(80*PI/180);
    y = 21*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);
				    //--------腿最短状态向前--------//
    x = 14*sin(60*PI/180);
    y = 14*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(150);
		
		
		x = 14;
    y = 16;
    CartesianToTheta(1.0);
    SetCoupledPosition(0);
		
		x=0;
    y=28;
		CartesianToTheta(1.0);
    SetCoupledPosition(3);
    vTaskDelay(500);
}
void pinghengmu_two_leg(int LegId)
{
    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

    x = 0;
    y = 23; 
    CartesianToTheta(1.0);
    SetCoupledPosition(rev_legid); 
    vTaskDelay(800);               

    //当前腿先向下蹬一下保持平衡//
    x = 0;
    y = 25;
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(1000);

    //--------腿缩最短在后面--------//
    x = -13*sin(80*PI/180);
    y = 21*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);
				    //--------腿最短状态向前--------//
    x = 14*sin(60*PI/180);
    y = 14*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(150);
			
			x = 14;
      y = 16;
      CartesianToTheta(1.0);
      SetCoupledPosition(1);
	////////////////////////////////////////////////////////
//	  if(tri_mode==0)//上第一级台阶
//		{
//			x=0;
//			y=30;
//			CartesianToTheta(1.0);
//      SetCoupledPosition(2);
//			SetCoupledPosition(3);
//			vTaskDelay(800);  
//			
//	     x=-7;
//	     y=35;
//			 CartesianToTheta(1.0);
//	     SetCoupledPosition(2);
//       SetCoupledPosition(3);
//	     vTaskDelay(800);  
//			 

//			x=0;
//			y=30;
//			CartesianToTheta(1.0);
//      SetCoupledPosition(2);
//			SetCoupledPosition(3);
//			vTaskDelay(800);  
//			
//	     x=-7;
//	     y=32;
//			 CartesianToTheta(1.0);
//	     SetCoupledPosition(2);
//       SetCoupledPosition(3);
//	     vTaskDelay(800); 
//		 }	
//	  if(tri_mode==0)           //当跨上一阶台阶后，使用这个参数！但向前蹬效果不明显 .countnum=0时，后腿会从第一阶台阶蹦下去，不可取
//		{
//			x=-8;
//			y=28;
//			CartesianToTheta(1.0);
//      SetCoupledPosition(2);
//			SetCoupledPosition(3);
//			vTaskDelay(1500);  
//			
//	     x=0;
//	     y=35;
//			 CartesianToTheta(1.0);
//	     SetCoupledPosition(2);
//       SetCoupledPosition(3);
//	     vTaskDelay(1500);  
//			 
//		}			 
//		 countnum++;
			x=0;
			y=26;
			CartesianToTheta(1.0);
      SetCoupledPosition(2);
			SetCoupledPosition(3);
			vTaskDelay(1500);  
			
	     x=-7;
	     y=28;
			 CartesianToTheta(1.0);
	     SetCoupledPosition(2);
       SetCoupledPosition(3);
	     vTaskDelay(1500);  

			 


}
void pinghengmu_three_leg(int LegId)  //第三条腿上台阶
{
    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

//      x = 14*sin(60*PI/180);
//      y = 14*cos(60*PI/180);
	    x=10;
	    y=13;
      CartesianToTheta(1.0);
      SetCoupledPosition(rev_legid); 
      vTaskDelay(800);               

      //当前腿先向下蹬一下保持平衡//
      x = 0;
      y = 30;
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(1600);

    //--------腿缩最短在后面--------//
    x = -13*sin(80*PI/180);
    y = 21*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);
				    //--------腿最短状态向前--------//
    x = 14*sin(60*PI/180);
    y = 14*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(150);
				

			x=10;
			y=21;
			CartesianToTheta(1.0);
      SetCoupledPosition(2);
			vTaskDelay(1000);
						
			x = 5;
      y = 14;
      CartesianToTheta(1.0);
      SetCoupledPosition(0);
			SetCoupledPosition(1);
			vTaskDelay(500);
			
}

void pinghengmu_four_leg(int LegId)
{
			x = 0;
      y = 18;
      CartesianToTheta(1.0);
      SetCoupledPosition(0);
			SetCoupledPosition(1);
			vTaskDelay(500);
			  x = 3;
      y = 18;
      CartesianToTheta(1.0);
      SetCoupledPosition(2);
      vTaskDelay(500);


			

	    //------四个腿子全部抬到最高-----//

    //--------对角腿缩最短--------//
    int rev_legid;
    if(LegId==0) rev_legid=3;
    else if(LegId==1) rev_legid=2;
    else if(LegId==2) rev_legid=1;
    else if(LegId==3) rev_legid=0;

//    x = 14*sin(60*PI/180);
//    y = 14*cos(60*PI/180);
    x=7;
		y=12;
    CartesianToTheta(1.0);
    SetCoupledPosition(rev_legid); 
    vTaskDelay(1200);               

      //当前腿先向下蹬一下保持平衡//
      x = 0;
      y = 32;
      CartesianToTheta(1.0);
      SetCoupledPosition(LegId);
      vTaskDelay(1600);

    //--------腿缩最短在后面--------//
    x = -13*sin(80*PI/180);
    y = 21*cos(80*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(220);
				    //--------腿最短状态向前--------//
    x = 14*sin(60*PI/180);
    y = 14*cos(60*PI/180);
    CartesianToTheta(1.0);
    SetCoupledPosition(LegId);
    vTaskDelay(150);


		x =5;
    y = 18;
    CartesianToTheta(1.0);
    SetCoupledPosition(3);
    vTaskDelay(500);

		x = 3;//////////////////////////////////////
    y = 18;
    CartesianToTheta(1.0);
    SetCoupledPosition(3);
    vTaskDelay(500);
		
		x = 0;
    y = 18;
    CartesianToTheta(1.0);
    SetCoupledPosition(0);
    vTaskDelay(500);
  ////////////////////////////////  
		x = 5;
    y = 18;
    CartesianToTheta(1.0);
    SetCoupledPosition(2);
    SetCoupledPosition(3);
    vTaskDelay(500);

			x = 18;
      y = 14;
      CartesianToTheta(1.0);
      SetCoupledPosition(0);
			SetCoupledPosition(1);
			vTaskDelay(500);

			x = 5;
      y = 14;
      CartesianToTheta(1.0);
      SetCoupledPosition(0);
			SetCoupledPosition(1);
			vTaskDelay(500);
//////////////////////////
//		x=0;
//		y=16;
//		CartesianToTheta(1.0);
//		SetCoupledPosition(0);
//		vTaskDelay(500);
//		
//		x=0;
//		y=16;
//		CartesianToTheta(1.0);
//		SetCoupledPosition(1);
//		SetCoupledPosition(2);
//		vTaskDelay(500);


}

u8 start_ready_flag[8] = {0};
void StartPosToMiddlePos (void)
{
    state = REALSE;

    pid_spd_out_limit=3000;

    temp_theta=-55;//原本是60  顺时针 是正数，逆时针是负数
	//后腿比前腿低
    LegGain gains = {10, 0.00, 5, 0.00};  //原本不改PID，      
		ChangeTheGainsOfPD(gains);
		
//    temp_pid.ref_agle[0]-=temp_theta*ReductionAndAngleRatio; 
		temp_pid.ref_agle[0]-=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[1]+=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[2]+=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[3]-=temp_theta*ReductionAndAngleRatio;

    temp_pid.ref_agle[4]-=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[5]+=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[6]+=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[7]-=temp_theta*ReductionAndAngleRatio;

    IsMotoReadyOrNot= IsReady;		//数据填充完毕

    vTaskDelay(1500);
//----------------------------设置当前点为零点--------------------------//
    memset(&moto_chassis,0,sizeof(moto_measure_t)*8);

    for(int i=0; i<8; i++)
        temp_pid.ref_agle[i]=ref_agle[i]=0;

    state = REALSE;

    vTaskDelay(500);
		state = STOP;
    //----------------------------蹲下来--------------------------//
//    x=0;
//    y = 11;
//    CartesianToTheta(1.0);

//    CommandAllLegs_v();

//    vTaskDelay(1000);


    pid_spd_out_limit=  3000;//原 3000

}

void MiddlePosToEndPos (void)
{

    pid_spd_out_limit=  1000;

    state = REALSE;
    x=0;
    y = 17.3205081;

    CartesianToTheta(1.0);
    CommandAllLegs_v();

    vTaskDelay(1200);

    pid_spd_out_limit=  600;

    temp_theta=-99.0;

    temp_pid.ref_agle[0]-=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[1]-=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[2]+=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[3]+=temp_theta*ReductionAndAngleRatio;

    temp_pid.ref_agle[4]-=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[5]-=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[6]+=temp_theta*ReductionAndAngleRatio;
    temp_pid.ref_agle[7]+=temp_theta*ReductionAndAngleRatio;

    IsMotoReadyOrNot= IsReady;		//数据填充完毕

    vTaskDelay(2000);

    pid_spd_out_limit=  1000;



    state = REALSE;
}
