#ifndef TIM_MEASURE_H_
#define TIM_MEASURE_H_

#include "stm32f10x.h"

void TIM_NVIC_Init();		//��ʱ���жϳ�ʼ��
void TIM_GPIO_Init();		//��ʱ���������ų�ʼ��
void TIM_Mode_Init();		//��ʱ����ʼ��
void TIM_IC_Mode_Init();	//��ʱ������ģʽ��ʼ��

void TIM_General_Init();	//��ʱ��ȫ�ֳ�ʼ��




#endif // !TIM_MEASURE_H_
