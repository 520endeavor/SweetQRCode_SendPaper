#ifndef __SIM900A_GPRS_COMMUNICATION_H
#define __SIM900A_GPRS_COMMUNICATION_H

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "usart.h"
#include "string.h"	 
#include "tim.h"
#include "MatrixKeyboard4By4.h"
#include "Data_Generic.h"
#include "HostSeverGprs_AT24CxxMem.h"

#define USART1_MAX_SEND_LEN			400		//����ͻ����ֽ���
#define USART1_MAX_RECV_LEN			400		//�����ջ����ֽ���
#define GPRS_PROTOCOL_RECV_LEN	8			//GPRSЭ������ֽ���				
	
typedef enum{
	OutPaper_Cmd_n=100,
	OutPaper_SetModeOfLength_Cmd_n,
	OutPaper_SetLength_Data_n,
}GPRS_Receive_Data_n;
	
extern volatile uint8_t USART1_TX_BUF[USART1_MAX_SEND_LEN]; 								//���ͻ���,���USART1_MAX_SEND_LEN�ֽ�   	  	
extern volatile uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN]; 								//���ջ���,���USART1_MAX_RECV_LEN���ֽ�.
extern volatile uint16_t USART1_RX_STA;

void Sim900A_GPRS_FREERTOS_Init(void);																			//Sim900A_GPRSͨѶ�����ʼ������
	
osThreadId Get_Sim900A_GPRS_ReceiveTaskHandle(void);												//��ȡSim900A_GPRS_ReceiveTask������
osThreadId Get_Sim900A_GPRS_Initiative_TransmitTaskHandle(void);						//��ȡSim900A_GPRS_Initiative_TransmitTask������
osThreadId Get_Sim900A_GPRS_Feadback_TransmitTaskHandle(void);							//��ȡSim900A_GPRS_Feadback_TransmitTaskHandle������
EventGroupHandle_t Get_Sim900A_GPRS_ReceiveAndSwitchEventGroupHandle(void);	//��ȡSim900A_GPRS_ReceiveEventGroupHandle�¼���־����
	
void Get_GPRS_Receive_Data(GPRS_Receive_Data_n name_num,void*extern_data);	//ȡֵ����������GPRS���յ�����@GPRS_Receive_Data_n
void Set_GPRS_Receive_Data(GPRS_Receive_Data_n name_num,void*extern_data);	//�޸�GPRS���ܵ�����@GPRS_Receive_Data_n

#endif

