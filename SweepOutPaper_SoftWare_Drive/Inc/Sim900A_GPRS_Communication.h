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

#define USART1_MAX_SEND_LEN			400		//最大发送缓存字节数
#define USART1_MAX_RECV_LEN			400		//最大接收缓存字节数
#define GPRS_PROTOCOL_RECV_LEN	8			//GPRS协议接收字节数				
	
typedef enum{
	OutPaper_Cmd_n=100,
	OutPaper_SetModeOfLength_Cmd_n,
	OutPaper_SetLength_Data_n,
}GPRS_Receive_Data_n;
	
extern volatile uint8_t USART1_TX_BUF[USART1_MAX_SEND_LEN]; 								//发送缓冲,最大USART1_MAX_SEND_LEN字节   	  	
extern volatile uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN]; 								//接收缓冲,最大USART1_MAX_RECV_LEN个字节.
extern volatile uint16_t USART1_RX_STA;

void Sim900A_GPRS_FREERTOS_Init(void);																			//Sim900A_GPRS通讯任务初始化函数
	
osThreadId Get_Sim900A_GPRS_ReceiveTaskHandle(void);												//获取Sim900A_GPRS_ReceiveTask任务句柄
osThreadId Get_Sim900A_GPRS_Initiative_TransmitTaskHandle(void);						//获取Sim900A_GPRS_Initiative_TransmitTask任务句柄
osThreadId Get_Sim900A_GPRS_Feadback_TransmitTaskHandle(void);							//获取Sim900A_GPRS_Feadback_TransmitTaskHandle任务句柄
EventGroupHandle_t Get_Sim900A_GPRS_ReceiveAndSwitchEventGroupHandle(void);	//获取Sim900A_GPRS_ReceiveEventGroupHandle事件标志组句柄
	
void Get_GPRS_Receive_Data(GPRS_Receive_Data_n name_num,void*extern_data);	//取值函数，搬运GPRS接收的数据@GPRS_Receive_Data_n
void Set_GPRS_Receive_Data(GPRS_Receive_Data_n name_num,void*extern_data);	//修改GPRS接受的数据@GPRS_Receive_Data_n

#endif

