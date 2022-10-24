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
//³õÊ¼»¯²½Ì¬ÔöÒæ
LegGain gait_gains = {15.5, 0.00, 25.0, 0.00};
//Éè¶¨Ã¿Ò»ÖÖ²½Ì¬µÄ¾ßÌåÔöÒæ
LegGain state_gait_gains[] = {
    //{kp_pos, kd_pos} kp_spd, kd_spd,
    {22.0, 0.00, 8.0, 0.00},  //Î»ÖÃ»· ËÙ¶È»· TROT
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


//³õÊ¼»¯²½Ì¬²ÎÊı
GaitParams gait_params = {17.3,12.0,4.0,0.00,0.25,1};
//Éè¶¨Ã¿Ò»ÖÖ²½Ì¬µÄ¾ßÌå²ÎÊı
GaitParams state_gait_params[] = {
    //{stance_height, step_length, up_amp, down_amp, flight_percent, freq}  cm
    //{²½¸ß, ²½³¤, Ì§ÍÈ¸ß, Ñ¹ÍÈ¸ß, t, ÆµÂÊ}	µ¥Î» cm
    {16.0, 16.0, 5.00, 2.00, 0.25, 1.85}, // TROT	Ë«ÅÄ²½Ì¬ ¶Ô½ÇĞ¡ÅÜ
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

    {16.3, 0.00, 5.70, 3.00, 0.30, 2.8}, // CLIMBING	ÅÀÆÂ

    {16.3, 0.00, 6.0, 0.00, 2.5, 1.5}, // WALK
    {16.0, 16.0, 5.00, 2.00, 0.25, 1.85}, // WALK_BACK

    {23.0, 7.00, 3, 2.00, 0.25, 2.0}, // ROTATE LEFT			{23.0, 10.00, 3, 2.00, 0.20, 2.0},  {21.0, 16.0, 5.00, 3.00, 0.25, 2.3},
    {21.0, 7.00, 5, 2.00, 0.25, 2.0},	// ROTATE RIGHT			{23.0, 10.00, 3, 2.00, 0.20, 2.0},

    {17.0, 0.00, 6.00, 4.00, 0.35, 2.0}, //BOUND
    {17.0, 0.00, 6.00, 0.00, 0.35, 2.0}, // GALLOP
    {16.0, 16.0, 5.00, 2.00, 0.25, 1.85}, // TROT2	Ë«ÅÄ²½Ì¬ ¶Ô½ÇĞ¡ÅÜ
		{16.0, 16.0, 5.00, 2.00, 0.25, 1.85}, // TROT2	Ë«ÅÄ²½Ì¬ ¶Ô½ÇĞ¡ÅÜ
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
*	×îµÍwalk {12.0, 15.0, 1.8, 0.00, 0.50, 1.0}
*	Ğ§¹û²»´í{14.0, 12.0, 3.5, 0.00, 0.50, 1.0},
*	{16.0, 12.00, 5.0, 0.00, 0.20, 2.0}, // WALK		ahead ²»´í
*	{17.3, 12.00, 4.0, 0.00, 0.35, 2.5}, // WALK		ahead
* ¶Ô½ÇĞ¡ÅÜ²½Ì¬²»´íËÙ¶ÈºÍÎÈ¶¨¶¼·Ç³£ºÃ
*/

GaitParams test_gait_params= {16.0, 15.0, 5.00, 2.00, 0.25, 2.3}; // TROT		//Ë«ÅÄ²½Ì¬ ¶Ô½ÇĞ¡ÅÜ
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
    //{²½¸ß, ²½³¤, Ì§ÍÈ¸ß, Ñ¹ÍÈ¸ß, t, ÆµÂÊ}	µ¥Î» cm
//			//TROT ¶Ô½ÇĞ¡ÅÜ  /*-´Ë´¦Îª20m¾ºËÙÅÜ²ÎÊı*/
			  {21.0, 12.0, 5.00, 2.00, 0.25, 3.0},//¿ì²½
			  {21.0, 12.0, 5.00, 2.00, 0.25, 3.0},
			  {21.0, 12.0, 5.00, 2.00, 0.25, 3.0},
			  {21.0, 12.0, 5.00, 2.00, 0.25, 3.0},
//				/*-´Ë´¦Îª20m¾ºËÙÅÜ²ÎÊı*/
////////////////////////////////////
//				{21.0, 5.0, 0.00, 2.00, 0.25, 1.5},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}//ÂÑÊ¯Â·
//        {21.0, 5.0, 0.00, 2.00, 0.25, 1.5},
//        {21.0, 5.0, 0.00, 2.00, 0.25, 1.5},
//        {21.0, 5.0, 0.00, 2.00, 0.25, 1.5},

////				{14.0, 3.0, 1.00, 0.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
////        {14.0, 3.0, 1.00, 0.00, 0.25, 2.0},
////			  {34.0, 3.0, 10.00, 0.00, 0.25, 2.0},
////        {34.0, 3.0, 10.00, 0.00, 0.25, 2.0},
    },
    {   // ÏÂÆÂCLIMBING_DOWN
			  {21.0, 9.00, 4, 1.00, 0.25, 2.5},   // 20  9  5  1  0.25  2.5
        {21.0, 9.00, 4, 1.00, 0.25, 2.5},   //  18  9  5 1     21  9  5  5¿ÉÒÔ×Ô¼ºÂıÂıÅÀ

        {18.0, 9.00, 4, 1.00, 0.25, 2.5},
        {18.0, 9.00, 4, 1.00, 0.25, 2.5}

//        {25.0, 10.00, 3, 2.00, 0.25, 3.8},
//        {25.0, 10.00, 3, 2.00, 0.25,3.8},

//        {25.0, 10.00, 3, 2.00, 0.25, 3.8},
//        {25.0, 10.00, 3, 2.00, 0.25, 3.8}
    },
    {   //TEST2-ÉÏÌ¨½×µÚÒ»½×¶Î
/*ÉÏÌ¨½×µÚÒ»½×¶Î²ÎÊı*/
				{14.0, 3.0, 1.00, 0.00, 0.25, 2.0},  //   {21.0, 12.0, 3.00, 2.00, 0.25, 4.0}
        {14.0, 3.0, 1.00, 0.00, 0.25, 2.0},
			  {34.0, 3.0, 5.00, 0.00, 0.25, 2.0},
        {34.0, 3.0, 5.00, 0.00, 0.25, 2.0},
/*ÉÏÌ¨½×µÚÒ»½×¶Î²ÎÊı*/

    },
    {   //TEST3  -ÉÏÌ¨½×µÚ¶ş½×¶Î
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
    {   // TEST7       //µ÷Õû·½Ïò Ğ¡Ëé²½×ªÍä ×ó
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1}
    },
    {   // TEST8			//µ÷Õû·½Ïò Ğ¡Ëé²½×ªÍä ÓÒ
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1},
        {26.0, 8.0, 4.00, 3.00, 0.25, 2.1}
    },
    {   // TEST9
			  {18.0, 5.0, 3.00, 2.00, 0.25, 2.0},//¿ì²½,ÆµÂÊ2.0
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

    {   //CLIMBING	ÅÀÆÂ-ÉÏÆÂ

        {18.0, 9.00, 3, 1.00, 0.35, 3.0},   // 
        {18.0, 9.00, 3, 1.00, 0.35, 3.0},   //  

        {21.0, 9.00, 3, 1.00, 0.35, 3.0},
        {21.0, 9.00, 3, 1.00, 0.35, 3.0}   //


//				{16.0, 16.0, 3.00, 1.00, 0.25, 3.0},		//´ó²½×ÓÂõÉÏÈ¥¿ÉÒÔ
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
	*@ ÈÎÎñÃû³Æ£ºvoid PostureControl_task(void *pvParameters)
	*@ ¹¦ÄÜ£º×ËÌ¬¿ØÖÆ
	*@ ±¸×¢£ºÑ¡Ôñ×´Ì¬
 *******************************************************************************************/
void PostureControl_task(void *pvParameters)
{
    for(;;)
    {	
        gait_params = state_gait_params[state];
        detached_params = state_detached_params[state];
        switch(state) 
				{
        case STOP:		//21CMÔİÍ£ ps_key[7] 
            x=0;
            y = 21;
						pid_spd_out_limit=3000;//Ô­3000
						CartesianToTheta(1.0);
            LegGain gains = {10, 0.00, 5 , 0.00};       //¸Õ¶ÈĞ¡£¬×èÄá´ó
            //pid_spd_out_limit=400;
            CommandAllLegs(gains);
            break;

        case REALSE:		// ÊÍ·Å Ê²Ã´¶¼²»×ö
            vTaskDelay(500);
            break;

        case CLIMBING:		//ÅÀÆÂ  ps_key[2], mode2
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0,1.0, 1.0, 1.0);  //·ûºÅÎª  -
            break;

        case WALK:		//Ç°½ø
            gait_detached(detached_params, 0.75, 0.25, 0.0, 0.5, 1.0, 1.0, 1.0, 1.0);
            break;

        case WALK_BACK:		//ºóÍË  ps_key[14], mode2
					  gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, -1.0, -1.0, -1.0, -1.0);//leg_offsetÓ¦¸ÃÏàÍ¬£¿£¿£¿
            break;

        case CLIMBING2:		//ÉÏÆÂ·½·¨2  ps_key[3], mode2
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.00, 1.0, 1.0, 1.0, 1.0);
            break;

        case GALLOP:		//Ï®²½
            gait_detached(detached_params,0.9, 0.7, 0.2, 0.0, 1.0, 1.0, 1.0, 1.0);
            break;

        case ROTAT_LEFT:	//Ò»½××ó×ª  ps_key[5] ,mode0
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.00, -1.0, 1.0, -1.0, 1.0); //Î´·ÖÀëµÄ²½Ì¬

            break;
        case ROTAT_RIGHT:		//Ò»½×ÓÒ×ª  ps_key[6],mode0
            gait_detached(detached_params,0.5, 0.0, 0.0, 0.5, 1.0, -1.0, 1.0, -1.0); //Î´·ÖÀëµÄ²½Ì¬

            break;
        case JUMP:		//ÌøÔ¾
            ExecuteJumpup();
            break;

        case START:		//³õÊ¼»¯
            StartPosToMiddlePos();
            break;

        case END:		//½áÊø
            MiddlePosToEndPos();
            break;
				//ºóÍÈ±ÈÇ°ÍÈµÍ
        case TROT:	//3.0¾ºËÙÅÜ
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offsetÓ¦¸ÃÏàÍ¬£¿£¿£¿
            break;

        case CLIMBING_DOWN:		//ÏÂÆÂ
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);
            break;

        case TEST2:		 //¿ì²½ÉÏÌ¨½×20cm//
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offsetÓ¦¸ÃÏàÍ¬£¿£¿£¿ 
				break;

        case TEST3:		 //Ò»½×ÔİÍ£10cm
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

        case TEST4:	    //Á½¼¶Ì¨½×ÔİÍ£×´Ì¬
					
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

        case TEST5:		//Ò»½×Ç°½ø-¿ì²½ÉÏÌ¨½×10cm
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.00, 1.0, 1.0, 1.0, 1.0);
            break;

        case TEST6:		 //ÏÂË«Ä¾ÇÅ ps_key[11]
            gait_detached(detached_params, 0.0, 0.5, 0.5, 0.00, 1.0, 1.0, 1.0, 1.0);
            break;

        case TEST7:		 //Ò»¼¶Ì¨½×ÔİÍ£×´Ì¬-ÏÂÌ¨½×
            x=0;
            //y = 17.3205081;
            y = 26;
						pid_spd_out_limit=3000;
						CartesianToTheta(1.0);
//            LegGain gains = {10, 0.00, 5 , 0.00};       //¸Õ¶ÈĞ¡£¬×èÄá´ó
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

        case TEST8:		//²âÊÔ²½Ì¬
            ExecuteJumpdown();
            break;

        case TEST9:		//18cmÇ°½ø--Ç°ºóÍÈ¶¼ÔÚÆ½ºâÄ¾ÉÏµÄ¿ì²½×ß
				    gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offsetÓ¦¸ÃÏàÍ¬£¿£¿£¿

            break;

        case TEST10:		//21cm×ó×ª-Ô­µØ
            gait(gait_params, gait_gains, 0.0, 0.5, 0.5, 0.00, -1.0, 1.0, -1.0, 1.0); //Î´·ÖÀëµÄ²½Ì¬
            break;

        case TEST11:		 //21cmÓÒ×ª-Ô­µØ
            gait(gait_params, gait_gains, 0.0, 0.5, 0.5, 0.00, 1.0, -1.0, 1.0, -1.0); //Î´·ÖÀëµÄ²½Ì¬
            break;

        case TEST12:  //18cmÔİÍ£-Ç°ºóÍÈ¶¼ÔÚÆ½ºâÄ¾ÉÏµÄ×´Ì¬
            x=3;
            y = 18;
						pid_spd_out_limit=3000;
						CartesianToTheta(1.0);
            //pid_spd_out_limit=400;
            CommandAllLegs(gains);
            break;
				case TORT2: //21cm¿ì²½
//					  gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offsetÓ¦¸ÃÏàÍ¬
						gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offsetÓ¦¸ÃÏàÍ¬£¿£¿£
            break;
				case TROT3: //º£ÑóÇò¿ì²½
  					gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, 1.0, 1.0, 1.0);//leg_offsetÓ¦¸ÃÏàÍ¬£¿£¿£¿
				    break;
				case CLIMBING_LEFT:   //ÉÏÆÂ×ó×ª
  					gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, -1.0, 1.0, -1.0, 1.0);//leg_offsetÓ¦¸ÃÏàÍ¬£¿£¿£¿
				    break;
				case CLIMBING_RIGHT: //ÉÏÆÂÓÒ×ª
  					gait_detached(detached_params, 0.0, 0.5, 0.5, 0.0, 1.0, -1.0, 1.0, -1.0);//leg_offsetÓ¦¸ÃÏàÍ¬£¿£¿£¿
				    break;
				case JUMP_PAIGUAN://ÅÅ¹ÜÌøÔ¾
					  ExecuteJump();
				    break;
				case STOP25://25cmÔİÍ£--->¸ÄÎª15cmÔİÍ£´óÍÈË®Æ½
						pid_spd_out_limit=1000;
						theta2=-70;
						theta1=50;
 
						CommandAllLegs_v();
						break;
				case STOP_TAIJIE:  //Ã»ÓÃµ½,Í¬TEST3
					  x=6;
						y =10;
						pid_spd_out_limit=1000;
						CartesianToTheta(1.0);

            CommandAllLegs(gains);
            break;

				case STOP18://Î´ÓÃµ½£¬Í¬TEST12
            x=19;
						y = 23;
						pid_spd_out_limit=1000;
						jump_flag=1;
						CartesianToTheta(1.0);
            CommandAllLegs(gains);
						jump_flag=0;
						break;
				case STOP_QIAOBAN://õÎõÎ°åÉÏµÄÔİÍ£×´Ì¬
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
* FUNCTION : ËÄÍÈ·ÖÀëµÄÍÈ²¿ÔöÒæº¯Êı
*/

void gait_detached(	DetachedParam d_params,
                    float leg0_offset, float leg1_offset,float leg2_offset, float leg3_offset,
                    float leg0_direction, float leg1_direction,float leg2_direction, float leg3_direction) {

    float t = HAL_GetTick()/1000.0-now_time/1000.0;//now_time=0;tÀÛ¼Ó

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
//	printf("Ê±¼ät: %f  \r\n",t);
}

/**
* NAME: void gait(	GaitParams params,float leg0_offset, float leg1_offset,float leg2_offset, float leg3_offset)
* FUNCTION : ²úÉúÊ±¼äÂö³å Éè¶¨Ã¿¸öÍÈµÄ²ÎÊı µ÷ÕûÍÈµÄÔËĞĞ·½Ïò ½øĞĞ²¹³¥
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

    //¸Ä±äPD
    // ChangeTheGainsOfPD(gains);
}

/**
* NAME: void CoupledMoveLeg(float t, GaitParams params,float gait_offset, float leg_direction, int LegId)
* FUNCTION : Çı¶¯²¢ÁªÍÈ ´«µİ²ÎÊı
*/
void CoupledMoveLeg(float t, GaitParams params,float gait_offset, float leg_direction, int LegId)
{
    SinTrajectory(t, params, gait_offset);		//×ã¶Ë°ÚÏß¹ì¼£Éú³ÉÆ÷
    CartesianToTheta(leg_direction);		//µÑ¿¨¶û×ø±ê×ª»»µ½Ù¤Âí×ø±ê


    SetCoupledPosition(LegId);		//·¢ËÍÊı¾İ¸øµç»úÇı¶¯º¯Êı
}

/*
* NAME: SinTrajectory (float t,GaitParams params, float gaitOffset)
* FUNCTION : ÕıÏÒ¹ì¼£Éú³ÉÆ÷
*/
void SinTrajectory (float t,GaitParams params, float gaitOffset) {
    static float p = 0;
    static float prev_t = 0;

    float stanceHeight = params.stance_height; //¹·Éíµ½µØÃæµÄ¾àÀë (cm) walk 27.9/27.3
    float downAMP = params.down_amp;//ÏÂ²¿Õñ·ù (cm) walk 0
    float upAMP = params.up_amp;//ÉÏ²¿Õñ·ùy (cm)   walk 16
    float flightPercent = params.flight_percent;//°Ú¶¯Ïà°Ù·Ö±È (cm) walk 0.2
    float stepLength = params.step_length;// Ò»²½µÄ¾àÀë (cm)  walk 12
    float FREQ = params.freq;// Ò»²½µÄÆµÂÊ (Hz)  walk 1

    p += FREQ * (t - prev_t);//walk pÀÛ¼ÓËÙ¶ÈÂı tort pÀÛ¼ÓËÙ¶È¿ì
    prev_t = t;

    float gp = fmod((p+gaitOffset),1.0);//Çóx/yµÄÓàÊı ¶¼ÊÇdoubleÀàĞÍ gp<=1,x-[x/y]*y=fmod [x/y]È¡Õû
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
* FUNCTION : µÑ¿¨¶û×ø±ê×ª»»µ½½Ç¶È×ø±ê Ò²¾ÍÊÇ½«xy×ª»»³Étheta
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
	
		L3=sqrt(		pow(x+1.75,2)	+		pow(y,2)	);//pow(x,y)¼ÆËãxµÄy´Î·½
		L4=sqrt(		pow(x-1.75,2)	+		pow(y,2)	);//pow(x,y)¼ÆËãxµÄy´Î·½
	
		//ÍÈL3µÄÉÏÏÂÏŞ »¹ÓĞ´øÉÌÈ¶
    if(L3<9) L3=9;//9  Ô­12.5
    else if(L3>32) L3=32;//32   Ô­37.5
		//ÍÈL4µÄÉÏÏÂÏŞ »¹ÓĞ´øÉÌÈ¶
		if(L4<9) L4=9;//9  Ô­12.5
    else if(L4>32) L4=32;//32   Ô­37.5
		
		//½Ç¶È3
		N1=asin((x+1.75)/L3)*180.0/PI;//»¡¶È±ä½Ç¶È
    M1=acos(	(pow(L3,2)+pow(L1,2)-pow(L2,2))/(2*L1*L3)	)*180.0/PI;
		A3=M1-N1;//µç»ú×ó²à
		//½Ç¶È4
    N2=asin((x-1.75)/L4)*180.0/PI;//»¡¶È±ä½Ç¶È
    M2=acos(	(pow(L4,2)+pow(L1,2)-pow(L2,2))/(2*L1*L4)	)*180.0/PI;
    A4=M2+N2;//µç»úÓÒ²à

    if(leg_direction==1.0) {
        theta2=(A3-90.0);
        theta1=(A4-90.0);
    } else if(leg_direction==-1.0) {
        theta1=(A3-90.0);
        theta2=(A4-90.0);
    }
//    L=sqrt(		pow(x,2)	+		pow(y,2)	);//pow(x,y)¼ÆËãxµÄy´Î·½

//    if(L<12.5) L=9;//9  Ô­12.5
//    else if(L>37.5) L=32;//32   Ô­37.5

//    // vTaskSuspend(MotorControlTask_Handler);

//    N=asin(x/L)*180.0/PI;//»¡¶È±ä½Ç¶È
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
float _climbing_offset_angle=4360; //Ä¿Ç°10¡ã¡¢5¡ãĞ§¹û×îºÃ£¬£¨µ¹×ÅÉÏ£©  10   12  13  15   8¶¼ÊÔ¹ı
float climbing_zhunbei_angle=6540;

/**
* NAME: void SetCoupledPosition( int LegId)
* FUNCTION : ·¢ËÍµç»ú¿ØÖÆ½Ç¶È
*/
/**
* µçµ÷IDÉèÖÃ ´Ó×óÍùÓÒ´ÓÉÏµ½ÏÂ
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
		else if(climbing_offset_flag==2)   //  ¸ø¸ºÖµËÄ¸öÂíÍÈ·½Ïò¶¼Õı³££¬×¢Òâµçµ÷ID
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
			//A3=M1-N1;µç»ú×ó²à   A4=M2+N2;//µç»úÓÒ²à
			//Ò»°ãÇé¿ö: 
			//theta2=(A3-90.0);   theta1=(A4-90.0);
			
				if(LegId==0)
        {
            temp_pid.ref_agle[1]=theta2*ReductionAndAngleRatio;//µçµ÷2 Õı¶Ôµç»ú¿¿ÓÒ²à ¸©ÊÓ¿¿ÄÚ
            temp_pid.ref_agle[0]=-theta1*ReductionAndAngleRatio;//µçµ÷1 Õı¶Ôµç»ú¿¿×ó²à ¸©ÊÓ¿¿Íâ
//						temp_pid.ref_agle[1]=-theta2*ReductionAndAngleRatio; 
//						temp_pid.ref_agle[0]=theta1*ReductionAndAngleRatio;  
        }
        else if(LegId==1)
        {
//            temp_pid.ref_agle[2]=theta1*ReductionAndAngleRatio;//µçµ÷3 Õı¶Ôµç»ú¿¿×ó²à ¸©ÊÓ¿¿Íâ
//            temp_pid.ref_agle[3]=theta2*ReductionAndAngleRatio;//µçµ÷4 Õı¶Ôµç»ú¿¿ÓÒ²à ¸©ÊÓ¿¿ÄÚ
						temp_pid.ref_agle[2]=theta1*ReductionAndAngleRatio;//µçµ÷3 Õı¶Ôµç»ú¿¿×ó²à ¸©ÊÓ¿¿Íâ
            temp_pid.ref_agle[3]=-theta2*ReductionAndAngleRatio;//µçµ÷4 Õı¶Ôµç»ú¿¿ÓÒ²à ¸©ÊÓ¿¿ÄÚ
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
    IsMotoReadyOrNot= IsReady;		//Êı¾İÌî³äÍê±Ï

}

/**
* NAME: void CommandAllLegs(void)
* FUNCTION : ¿ØÖÆËùÓĞµç»ú
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
 *¸Ä±äÍÈ²¿ÔöÒæ
 *µ÷ÓÃÁËPID_reset_kpKd º¯Êı
 */
void ChangeTheGainsOfPD(LegGain gains)
{

    for (int i = 0; i < 8; i++) {
        pid_reset_kpkd(&pid_pos[i],gains.kp_pos,gains.kd_pos);
        pid_reset_kpkd(&pid_spd[i],gains.kp_spd,gains.kd_spd);

    }
}









