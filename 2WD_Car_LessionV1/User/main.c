#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "sys.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "OLED.h"

#include "Encoder.h"
#include "PID.h"
#include "PWM.h"
#include "TIM.h"
#include "Serial.h"
#include "EXTI.h"
#include "stdio.h"

float pitch,roll,yaw; 		//欧拉角
int16_t aacx,aacy,aacz;		//加速度传感器原始数据
int16_t gyrox,gyroy,gyroz;	//陀螺仪原始数据

int8_t Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu=2; //蓝牙遥控相关的变量
int16_t L_temp,R_temp,Moto1,Moto2;	//陀螺仪原始数据 0
uint8_t Usart2_Rec_Data = 0;
uint32_t BL_Spend = 0;
uint32_t BR_Spend = 0;
uint32_t B_Spend = 0;


int main(void)
 {	 

	
	delay_init();	//延时初始化 
	SystemInit();
	Serial_TX_RX_Init();
	USART_ReceiveData(USART2);
//	OLED_Init();
	Encoder_Init();
	PID_Init();
	PWM_Init();// 1/7200--->1KHz

	while(MPU_Init()!=0);//初始化MPU6050
	while(mpu_dmp_init()!=0);//初始化DMP 
	
	Exti_Init();//IN中断初始化
	Timer_Init();//100Hz
	while(1)              
 	{
//			OLED_ShowSignedNum(1,1,pitch*10,3);			
//			OLED_ShowSignedNum(2,1,L_temp,2);
//			OLED_ShowSignedNum(3,1,R_temp,2);
//			OLED_ShowSignedNum(2,6,Moto1,4);
//			OLED_ShowSignedNum(3,6,Moto2,4);
//			OLED_ShowSignedNum(4,1,gyroy,5);
	} 	
}
 
void TIM2_IRQHandler(void)  //100Hz//中断3
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)//判断是否中断溢出
	{
				 L_temp=Get_Left_Counter();
				 R_temp=Get_Right_Counter();    
			
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除中断标志位
	}

}

void EXTI15_10_IRQHandler(void) //中断1
{
	if(EXTI_GetITStatus(EXTI_Line14)==SET)
	{
        		mpu_dmp_get_data(&pitch,&roll,&yaw);
				MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);
						
				pid.Pv = pitch*10;				//角度*十倍
				Moto1 = balance(pid.Pv) + velocity(L_temp,R_temp,pid.Pv)+zhuan(BL_Spend,gyroz);		//获得PWM输出值
				Moto2 = balance(pid.Pv) + velocity(L_temp,R_temp,pid.Pv)+zhuan(BR_Spend,gyroz);
				
				Moto1=Limit_Pwm(Moto1,7200);
				Moto2=Limit_Pwm(Moto2,7200);		
				
				Set_compare(1,Moto1);
				Set_compare(2,Moto2);

		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	
}

void USART2_IRQHandler(void)//中断2
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
	/********************串口接收功能函数************************/
	 USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	
	 Usart2_Rec_Data = USART_ReceiveData(USART2);
		 if(Usart2_Rec_Data==0x30)BL_Spend+=50;            //左转
	else if(Usart2_Rec_Data==0x31)BR_Spend+=50;            //右转
	else if(Usart2_Rec_Data==0x32)B_Spend+=50;             //前进
	else if(Usart2_Rec_Data==0x33)B_Spend-=50;             //后退
//	else if(Usart2_Rec_Data==0x35);      //任务一
//	else if(Usart2_Rec_Data==0x36);      //任务二
//	else 
//	{
//		printf("Error:No this Command!");
//	}
	/**********************************************************/
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		USART_ReceiveData(USART2);
	}
}
