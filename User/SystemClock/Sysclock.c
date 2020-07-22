#include "Sysclock.h"

void HSE_System_Clock_Init()
{
	RCC_DeInit();						//RCCʱ��Դȡ����������
	RCC_HSEConfig(RCC_HSE_ON);			//�����ⲿHSE�߾���ʱ��

	__IO uint32_t HSE_Start_Status = 0;	//HSE�������״̬����

	HSE_Start_Status = RCC_WaitForHSEStartUp();//�ȴ�HSE��ʽ����
	if (HSE_Start_Status == SUCCESS)		//��������ɹ��ٽ�����һ������
	{
		//ָ��Ԥȡʹ�ܣ��ӿ�ָ��ִ���ٶ�
		//
		FLASH_PrefetchBufferCmd(ENABLE);
		FLASH_SetLatency(FLASH_Latency_2);

		//�������ַ�Ƶ����
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		RCC_PCLK2Config(RCC_HCLK_Div1);

		//����PLL���໷ʱ����Դ��HSE��9��Ƶ(72MHz)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		//RCCʹ��
		RCC_PLLCmd(ENABLE);
		//�ȴ�PLL�ȶ�
		while (RCC_FLAG_PLLRDY == RESET);
		//�趨ϵͳʱ����PLL���໷
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		//�ȴ����óɹ�
		while (RCC_GetSYSCLKSource() != 0x08);
	}
	else
	{
		//����޷�����HSE��ֹͣ���г���
		while (1);
	}

}
