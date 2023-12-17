#include "pid.h"
PID pid;

extern u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu; //蓝牙遥控相关的变量
extern uint32_t B_Spend;

/**************************************************************************
函数功能：PID数据初始化
入口参数：无
返回  值：无
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
    /*平衡PID环控制参数初始化*/
    //P对应反应快慢
    pid.Sv =  0;
    pid.Kp =-80; //0_720,设置为168            -80
    pid.Kd =-240;//190                       -240

////////								/*速度PID环控制参数初始化*/
    pid.Kp_speed =-2000;
	;
    pid.Ki_speed = pid.Kp_speed/200;//
    pid.EK_speed = 0;
    pid.SEK_speed = 0;;

    pid.left = 0;
    pid.right = 0;				//速度初始值
    //65 2
    /*转向PID环控制参数初始化*/
    pid.Kp_turn =0;
    pid.Angle_turn = 0;
    /*速度PID环控制参数初始化*/

}


/**************************************************************************
函数功能：以下三个函数，分别计算各个环的PID值，并返回
入口参数：未知
返回  值：无
**************************************************************************/


/*小车平衡环部分，微分+比例控制
微分变量为直接读取的加速度*/
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
函数功能：速度PID控制
入口参数：Encoder 转速
返回  值：速度控制PWM

*/
int velocity(int encoder_left,int encoder_right,float pitch)
{
    static float Velocity,Encoder_Least,Encoder,Movement;
    static float Encoder_Integral;
//	  float kp=-90,ki=-0.45;
    //=============速度PI控制器=======================//
    Encoder_Least =(encoder_left+encoder_right)-0;                    //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零）
    Encoder *= 0.7;		                                                //===一阶低通滤波器
    Encoder += Encoder_Least*0.3;	                                    //===一阶低通滤波器
    Encoder_Integral +=Encoder;                                       //===积分出位移 积分时间：10ms
    Encoder_Integral=Encoder_Integral-Movement;                       //===接收遥控器数据，控制前进后退
    if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===积分限幅
    if(Encoder_Integral<-10000)	Encoder_Integral=-10000;              //===积分限幅
    Velocity=Encoder*pid.Kp_speed+Encoder_Integral*pid.Ki_speed;                          //===速度控制
    //return 0;//先调节直立
	if((pitch>=80)||(pitch<=-80))  //小车跌倒后清零
		{
		  Encoder_Integral=0;    
		}	
    return Velocity/10+B_Spend;
}

/**************************************************************************
函数功能：转向PD控制
入口参数：左轮编码器、右轮编码器、Z轴陀螺仪
返回  值：转向控制PWM
作    者：平衡小车之家
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//转向控制
{
    static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
    float Turn_Amplitude=148/Flag_sudu,Kp=42,Kd=0;
    //=============遥控左右旋转部分=======================//
    if(1==Flag_Left||1==Flag_Right)                      //这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
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

    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===转向速度限幅
    if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
    if(Flag_Qian==1||Flag_Hou==1)  Kd=1;
    else Kd=0;   //转向的时候取消陀螺仪的纠正 有点模糊PID的思想
    //=============转向PD控制器=======================//
    Turn=-Turn_Target*Kp -gyro*Kd;                 //===结合Z轴陀螺仪进行PD控制
    return Turn;
}
//转向环
float zhuan_Kp=10.0f;   //期望小车转向，正反馈
float zhuan_Kd=0.6f;    //抑制小车转向，负反馈
//*************************************************************
//函数功能：控制小车转向
//Set_turn：目标旋转角速度
//Gyro_Z:陀螺仪Z轴的角速度
//不是一个严格的PD控制器，为小车的叠加控制
int zhuan(float Set_turn,float Gyro_Z)
{
  int PWM_Out=0; 
	if(Set_turn==0)
	{
	 PWM_Out=zhuan_Kd*Gyro_Z; //没有转向需求，Kd约束小车转向
	}
	if(Set_turn!=0)
	{
	 PWM_Out=zhuan_Kp*Set_turn; //有转向需求，Kp为期望小车转向 
	}
	return PWM_Out;
}
