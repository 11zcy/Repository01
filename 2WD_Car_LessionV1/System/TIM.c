#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//����TIM2ʱ��
	
	TIM_InternalClockConfig(TIM2);//ʹ���ڲ�ʱ��
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//����ʱ����Ԫ�ṹ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//���ò���Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//�������ϼ���
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;//ARR�Զ���װֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;//PSC����Ƶ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//�ظ���������ֵ���߼���ʱ������
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);//д�����
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//������±�־λ
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//�ж����
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�ж����ȼ�����
	
	NVIC_InitTypeDef NVIC_InitStructure;//NVIC�ṹ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//��ʱ��ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);//д�����
	
	TIM_Cmd(TIM2, ENABLE);//������ʱ��
}



