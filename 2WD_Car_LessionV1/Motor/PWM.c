#include "stm32f10x.h"                  // Device header

#define Left_Motor_turn_Positive  			GPIO_SetBits(GPIOA,GPIO_Pin_4),GPIO_ResetBits(GPIOA,GPIO_Pin_5) 
#define Left_Motor_turn_Negative 			GPIO_SetBits(GPIOA,GPIO_Pin_5),GPIO_ResetBits(GPIOA,GPIO_Pin_4) 

#define Right_Motor_turn_Positive  			GPIO_SetBits(GPIOA,GPIO_Pin_0),GPIO_ResetBits(GPIOA,GPIO_Pin_1) 
#define Right_Motor_turn_Negative 			GPIO_SetBits(GPIOA,GPIO_Pin_1),GPIO_ResetBits(GPIOA,GPIO_Pin_0) 

void PWM_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;//PWM_OC1
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;//L转向
	GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5);
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM1);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=7200-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=10-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=0;
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);
	TIM_OC2Init(TIM1,&TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能	
	
	TIM_Cmd(TIM1,ENABLE);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	
    TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
}

void Set_compare(uint8_t Oc,int16_t count)
{
	if(Oc==1)
	{
		if(count<0)
		{
			count*=-1;
			Left_Motor_turn_Negative;
		}
		else
		{
			Left_Motor_turn_Positive;
		}
		TIM_SetCompare1(TIM1,count);	
	}
	if(Oc==2)
	{
		if(count<0)
		{
			count*=-1;
			Right_Motor_turn_Negative;
		}
		else
		{
			Right_Motor_turn_Positive;
		}
		TIM_SetCompare2(TIM1,count);	
	}


}

int16_t Limit_Pwm(int16_t val,int16_t Max)
{	
	int16_t Limit_val=val;
	if(Limit_val>Max|Limit_val<Max*-1)
	{
	 Limit_val=Max*((Limit_val<0)?-1:1);	
	}
	 return Limit_val; 
	
	
}

