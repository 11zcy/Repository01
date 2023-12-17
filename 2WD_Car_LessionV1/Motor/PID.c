#include "pid.h"
PID pid;

extern u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu; //����ң����صı���
extern uint32_t B_Spend;

/**************************************************************************
�������ܣ�PID���ݳ�ʼ��
��ڲ�������
����  ֵ����
**************************************************************************/

int16_t Limit(float Output_val,float Max)
{
	int16_t Output=Output_val;
	if(Output>Max||Output<-Max)
	{
		Output=Max*((Output<0)?-1:1);
	}
    return Output; 
}

int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

void PID_Init()
{
    /*ƽ��PID�����Ʋ�����ʼ��*/
    //P��Ӧ��Ӧ����
    pid.Sv =  0;
    pid.Kp =-80; //0_720,����Ϊ168            -80
    pid.Kd =-240;//190                       -240

////////								/*�ٶ�PID�����Ʋ�����ʼ��*/
    pid.Kp_speed =-2000;
	;
    pid.Ki_speed = pid.Kp_speed/200;//
    pid.EK_speed = 0;
    pid.SEK_speed = 0;;

    pid.left = 0;
    pid.right = 0;				//�ٶȳ�ʼֵ
    //65 2
    /*ת��PID�����Ʋ�����ʼ��*/
    pid.Kp_turn =0;
    pid.Angle_turn = 0;
    /*�ٶ�PID�����Ʋ�����ʼ��*/

}


/**************************************************************************
�������ܣ����������������ֱ�����������PIDֵ��������
��ڲ�����δ֪
����  ֵ����
**************************************************************************/


/*С��ƽ�⻷���֣�΢��+��������
΢�ֱ���Ϊֱ�Ӷ�ȡ�ļ��ٶ�*/
extern short gyroy;
int balance(float Angle)
{

    signed  int  Bias;
    int balance;
    Bias=(Angle-pid.Sv)+pid.move;
    balance=pid.Kp*Bias+gyroy*pid.Kd/100;

    return balance;
}
/*
�������ܣ��ٶ�PID����
��ڲ�����Encoder ת��
����  ֵ���ٶȿ���PWM

*/
int velocity(int encoder_left,int encoder_right,float pitch)
{
    static float Velocity,Encoder_Least,Encoder,Movement;
    static float Encoder_Integral;
//	  float kp=-90,ki=-0.45;
    //=============�ٶ�PI������=======================//
    Encoder_Least =(encoder_left+encoder_right)-0;                    //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩
    Encoder *= 0.7;		                                                //===һ�׵�ͨ�˲���
    Encoder += Encoder_Least*0.3;	                                    //===һ�׵�ͨ�˲���
    Encoder_Integral +=Encoder;                                       //===���ֳ�λ�� ����ʱ�䣺10ms
    Encoder_Integral=Encoder_Integral-Movement;                       //===����ң�������ݣ�����ǰ������
    if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===�����޷�
    if(Encoder_Integral<-10000)	Encoder_Integral=-10000;              //===�����޷�
    Velocity=Encoder*pid.Kp_speed+Encoder_Integral*pid.Ki_speed;                          //===�ٶȿ���
    //return 0;//�ȵ���ֱ��
	if((pitch>=80)||(pitch<=-80))  //С������������
		{
		  Encoder_Integral=0;    
		}	
    return Velocity/10+B_Spend;
}

/**************************************************************************
�������ܣ�ת��PD����
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//ת�����
{
    static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
    float Turn_Amplitude=148/Flag_sudu,Kp=42,Kd=0;
    //=============ң��������ת����=======================//
    if(1==Flag_Left||1==Flag_Right)                      //��һ������Ҫ�Ǹ�����תǰ���ٶȵ����ٶȵ���ʼ�ٶȣ�����С������Ӧ��
    {
        if(++Turn_Count==1)
            Encoder_temp=myabs(encoder_left+encoder_right);
        Turn_Convert=50/Encoder_temp;
        if(Turn_Convert<0.6)Turn_Convert=0.6;
        if(Turn_Convert>3)Turn_Convert=3;
    }
    else
    {
        Turn_Convert=0.9;
        Turn_Count=0;
        Encoder_temp=0;
    }
    if(1==Flag_Left)	           Turn_Target-=Turn_Convert;
    else if(1==Flag_Right)	     Turn_Target+=Turn_Convert;
    else Turn_Target=0;

    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===ת���ٶ��޷�
    if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
    if(Flag_Qian==1||Flag_Hou==1)  Kd=1;
    else Kd=0;   //ת���ʱ��ȡ�������ǵľ��� �е�ģ��PID��˼��
    //=============ת��PD������=======================//
    Turn=-Turn_Target*Kp -gyro*Kd;                 //===���Z�������ǽ���PD����
    return Turn;
}
//ת��
float zhuan_Kp=10.0f;   //����С��ת��������
float zhuan_Kd=0.6f;    //����С��ת�򣬸�����
//*************************************************************
//�������ܣ�����С��ת��
//Set_turn��Ŀ����ת���ٶ�
//Gyro_Z:������Z��Ľ��ٶ�
//����һ���ϸ��PD��������ΪС���ĵ��ӿ���
int zhuan(float Set_turn,float Gyro_Z)
{
  int PWM_Out=0; 
	if(Set_turn==0)
	{
	 PWM_Out=zhuan_Kd*Gyro_Z; //û��ת������KdԼ��С��ת��
	}
	if(Set_turn!=0)
	{
	 PWM_Out=zhuan_Kp*Set_turn; //��ת������KpΪ����С��ת�� 
	}
	return PWM_Out;
}
