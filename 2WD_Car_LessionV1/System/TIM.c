#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//开启TIM2时钟
	
	TIM_InternalClockConfig(TIM2);//使用内部时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//定义时基单元结构体
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;//ARR自动重装值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;//PSC不分频
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//重复计数器的值，高级定时器特有
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);//写入参数
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除更新标志位
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//中断输出
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断优先级分组
	
	NVIC_InitTypeDef NVIC_InitStructure;//NVIC结构体
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//定时器通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//响应优先级
	NVIC_Init(&NVIC_InitStructure);//写入参数
	
	TIM_Cmd(TIM2, ENABLE);//开启定时器
}



