#ifndef __PID_H__
#define __PID_H__

#define ZHONGZHI 1

#include "sys.h"
//PID�ṹ��
typedef struct 
{
	signed int  Sv;			//�û��趨ƽ��λ��ֵ
	float Pv;
					/*ƽ�⻷�����趨 */
	float  Kd;					//ƽ��΢����?��?
	signed int  Kp;			//ƽ�������ϵ��
	
	

	
					/*�ٶȻ������趨 */
	signed int left;      //Ŀ�����ٶ�
	signed int right;     //Ŀ�����ٶ�
	signed int move;     //Ŀ�����ٶ�
	float  Ki_speed;				//�ٶȻ�������ϵ��
	float  Kp_speed;		//�ٶȻ�������ϵ��
	float  Kd_speed;		//�ٶȻ�΢����ϵ��
	signed int EK_speed;  	//�ٶ�ƫ��
	signed int SEK_speed;   //��ʷƫ��֮��
	signed int DIF_speed;

					/*ת�򻷲����趨 */
	float Kp_turn;					//ת�򻷱�����ϵ��
	float Angle_turn;				//Ŀ��ת��Ƕ�
	
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
