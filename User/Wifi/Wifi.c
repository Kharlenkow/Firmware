#include "Wifi.h"
#include <string.h>

char Client_IP_Address[20];				//�û�IP��ַ
extern struct Cache_Container Wifi_Cache;//���ô��ڻ���

//########## Wifi����ָ�� ############
const char WIFI_CMD_RETURE_DISABLED[] ="ATE0\r\n";    //�ر�ָ����Թ���    
const char WIFI_CMD_BASIC_TEST[] = "AT\r\n";					//��������ָ��
const char WIFI_CMD_AP_BUILD[] = "AT+CWDHCP_CUR=0,1\r\n";		//����̨DHCP����
const char WIFI_CMD_AP_SETUP[] = "AT+CWSAP_CUR=\"AMR\",\"lj62772283\",5,4\r\n";		//����̨��Ϣ����WPA/WPA2
const char WIFI_CMD_AP_CONNECTION_GET[] = "AT+CWLIF\r\n";							//���ӿͻ��˲�ѯ
const char WIFI_CMD_AP_MULTI_CONNECTION_DISABLE[] = "AT+CIPMUX=0\r\n";				//TCPֻ��������
const char WIFI_CMD_DIRECT_CONNECTION_STEP1[] = "AT+CIPMODE=1\r\n";							//����͸���׶�1
const char WIFI_CMD_DIRECT_CONNECTION_STEP2[] = "AT+CIPSEND\r\n";							//����͸���׶�2
const char WIFI_CMD_DIRECT_CONNECTION_END[] = "+++";									//�ر�͸��

void Wifi_Delay_A_While()      //����ʱ����
{
		uint32_t i=0xffffff;
	for(uint32_t i0=0;i0<i;i0++);
	
}

char* WIFI_CMD_TCP_SETUP(char* ip_address)
{
	char result[45];
	strcpy(result, "AT+CIPSTART=\"TCP\",\"");
	strcat(result, ip_address);
	strcat(result, "\",5960,1\r\n\0");
	return result;
}

struct Cache_Container Cache_Init_Get()
{
	struct Cache_Container result;
	result.iterator = 0;
	result.cache[0]='\0';
	return result;
}

void Cache_Clear(struct Cache_Container* Cache)
{
	if(Cache->iterator==0)
		return;
	for (uint8_t i = 0; i < 100; i++)
	{
		Cache->cache[i] = '\0';
	}
	Cache->iterator = 0;
}

void Cache_Add(struct Cache_Container* Cache,char data)
{
	Cache->cache[Cache->iterator] = data;
	Cache->iterator++;							//������ǰ��һ��

}

enum Wifi_State Wifi_Init()
{
	WIFI_USART_CFG();		//���ڳ�ʼ��
	Wifi_Send(WIFI_CMD_DIRECT_CONNECTION_END,0,false);//���˳�֮ǰ�������õ�͸��ģʽ
	Wifi_Delay_A_While();
	Wifi_Send(WIFI_CMD_RETURE_DISABLED, 0, false);		//��ָֹ��ش�
	Wifi_Delay_A_While();
	Cache_Clear(&Wifi_Cache);
	Wifi_Send(WIFI_CMD_BASIC_TEST, 0, false);
	if (!Wifi_Receive_CMD_CMP("OK",2,2))
	{
		Cache_Clear(&Wifi_Cache);
		return AT_Init_Test_Failed;
	}
	Cache_Clear(&Wifi_Cache);
	Wifi_Send(WIFI_CMD_AP_BUILD, 0, false);
	if (!Wifi_Receive_CMD_CMP("OK",2,2))
	{
		Cache_Clear(&Wifi_Cache);
		return AP_Build_Failed;
	}
	Cache_Clear(&Wifi_Cache);
	Wifi_Send(WIFI_CMD_AP_SETUP, 0, false);
	if (!Wifi_Receive_CMD_CMP("OK",2,2))
	{
		Cache_Clear(&Wifi_Cache);
		return AP_SETUP_Failed;
	}
	Cache_Clear(&Wifi_Cache);
	return NO_ERROR;
}

bool Wifi_Client_Connected()
{
	Wifi_Send(WIFI_CMD_AP_CONNECTION_GET, 0, false);
	if (Wifi_Receive_CMD_CMP("OK",2,2))
	{
		Cache_Clear(&Wifi_Cache);
		return false;
	}
	else
	{
		return true;		//�ݻ���ջ���
	}
}

void Wifi_Get_Client_IP()
{
	for (uint8_t i = 0; Wifi_Cache.cache[i] != ','; i++)
	{
		Client_IP_Address[i] = Wifi_Cache.cache[i];
	}
	Cache_Clear(&Wifi_Cache);
}

enum Wifi_State Wifi_TCP_Setup()
{
	if (Client_IP_Address[0] !='1')
		return NO_CLIENT_CONNECT;
	char cmd[45];
	strcpy(cmd,WIFI_CMD_TCP_SETUP(Client_IP_Address));
	Wifi_Send(cmd, 0, false);
	if (!Wifi_Receive_CMD_CMP("CONNECT",7,0))
	{
		Cache_Clear(&Wifi_Cache);
		return TCP_SETUP_Failed;
	}
	Cache_Clear(&Wifi_Cache);
	return NO_ERROR;
}

enum Wifi_State Wifi_Direct_CMD(bool mode)
{
	if (mode)
	{
		Wifi_Send(WIFI_CMD_DIRECT_CONNECTION_STEP1, 0, false);
		if (!Wifi_Receive_CMD_CMP("OK",2,2))
		{
			Cache_Clear(&Wifi_Cache);
			return DIRECT_CONNECTION_Failed;
		}
		Cache_Clear(&Wifi_Cache);
		Wifi_Send(WIFI_CMD_DIRECT_CONNECTION_STEP2, 0, false);
		Cache_Clear(&Wifi_Cache);
		return NO_ERROR;
	}
	else
	{
		Wifi_Send(WIFI_CMD_DIRECT_CONNECTION_END, 0, false);
		Cache_Clear(&Wifi_Cache);
		return NO_ERROR;
	}
}

void Wifi_Send(const char* string, uint32_t data, bool is_data)
{
	if (is_data)
	{
		Usart_SendUint32(WIFI_USARTx, data);
	}
	else
	{
		Usart_SendString(WIFI_USARTx, string);
	}
}

void Wifi_Receive_IRQ_CMD(bool mode)
{
	if (mode)
	{
		NVIC_EnableIRQ(WIFI_USART_IRQ);
	}
	else
	{
		NVIC_DisableIRQ(WIFI_USART_IRQ);
	}
}

bool Wifi_Receive_CMD_CMP(const char* Cmd,uint8_t len,uint8_t start_since)
{
	Wifi_Delay_A_While();
	if(Wifi_Cache.iterator==0)
		return false;
	for (int j = start_since; j < len+start_since; j++)		//0x0d 0x0a ��ͷ��ֱ������ ĩβͬ��
	{
		if (Wifi_Cache.cache[j] != Cmd[j-start_since])
		{
			return false;
		}
	}
	return true;
}
