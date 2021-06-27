#ifndef __HOSTSEVERGPRS_AT24CxxMem_H
#define __HOSTSEVERGPRS_AT24CxxMem_H

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "AT24Cxx.h" 
#include "Sim900A_GPRS_Communication.h" 


#define Fram16CurrentSettingPaperAmountAddress	0x00


typedef enum{
	Fram16Status_n=50,
	Fram16CurrentSettingPaperAmount_n
}Fram16Info_n;


typedef struct{
	uint8_t WriteFram16_MemoryID;
	uint8_t WriteFram16_MemoryData;
}WriteFram16_QueueTypedef;


void HostSeverGprs_At24CXX_FREERTOS_Init(void);										//服务器控制器传输数据任务初始化函数

osMessageQId Get_WriteFram16QueueHandle(void);										//获取WriteFram16QueueHandle队列句柄
osThreadId Get_WriteFram16TaskHandle(void);												//获取WriteFram16TaskHandle任务句柄

void FirstPowerOnReadFram16Init(void);														//上电读Fram16函数
void Get_Fram16Info_Data(Fram16Info_n name_num,void*extern_data);//取值函数，搬运Fram16状态数据@Fram16Info_n
void Set_Fram16Info_Data(Fram16Info_n name_num,void*extern_data);//修改ram16状态数据@Fram16Info_n

#endif


