#ifndef __PID_H__
#define __PID_H__

#define ZHONGZHI 1

#include "sys.h"
//PID结构体
typedef struct 
{
	signed int  Sv;			//用户设定平衡位置值
	float Pv;
					/*平衡环参数设定 */
	float  Kd;					//平衡微分项?凳?
	signed int  Kp;			//平衡比例项系数
	
	

	
					/*速度环参数设定 */
	signed int left;      //目标左速度
	signed int right;     //目标右速度
	signed int move;     //目标右速度
	float  Ki_speed;				//速度环积分项系数
	float  Kp_speed;		//速度环比例项系数
	float  Kd_speed;		//速度环微分项系数
	signed int EK_speed;  	//速度偏差
	signed int SEK_speed;   //历史偏差之和
	signed int DIF_speed;

					/*转向环参数设定 */
	float Kp_turn;					//转向环比例项系数
	float Angle_turn;				//目标转向角度
	
}PID;

extern PID pid;

int balance(float Angle);
void PID_Init(void);
int16_t Limit(float Output_val,float Max);
int velocity(int encoder_left,int encoder_right,float pitch);
//int velocity(int encoder_left,int encoder_right,int RPWM,float Angle);
//int turn(float gyro);
int turn(int encoder_left,int encoder_right,float gyro);
int zhuan(float Set_turn,float Gyro_Z);
#endif
