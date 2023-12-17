#include "stm32f10x.h"                  // Device header

int16_t L_Temp;
int16_t R_Temp;

void Encoder_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//L_Encoder
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//R_Encoder
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;//L:PA6 PA7||R:PB6 PB7
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; //无用 被编码器托管
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//无用 被编码器托管
	TIM_TimeBaseInitStruct.TIM_Period=65536-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=1-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICStructInit(&TIM_ICInitStruct);
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;//L_A相
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM3,&TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;//L_B相
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM3,&TIM_ICInitStruct);
	
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;//R_A相
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM4,&TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;//R_B相
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM4,&TIM_ICInitStruct);
	
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	
	TIM_Cmd(TIM3,ENABLE);
	TIM_Cmd(TIM4,ENABLE);
}

int16_t Get_Left_Counter(void)//1322每圈
{
	L_Temp=TIM_GetCounter(TIM4);
	TIM_SetCounter(TIM4,0);
	return L_Temp;
}

int16_t Get_Right_Counter(void)
{
	R_Temp=TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3,0);
	return R_Temp;
}
