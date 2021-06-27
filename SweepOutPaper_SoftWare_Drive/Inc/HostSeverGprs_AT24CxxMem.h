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


void HostSeverGprs_At24CXX_FREERTOS_Init(void);										//�������������������������ʼ������

osMessageQId Get_WriteFram16QueueHandle(void);										//��ȡWriteFram16QueueHandle���о��
osThreadId Get_WriteFram16TaskHandle(void);												//��ȡWriteFram16TaskHandle������

void FirstPowerOnReadFram16Init(void);														//�ϵ��Fram16����
void Get_Fram16Info_Data(Fram16Info_n name_num,void*extern_data);//ȡֵ����������Fram16״̬����@Fram16Info_n
void Set_Fram16Info_Data(Fram16Info_n name_num,void*extern_data);//�޸�ram16״̬����@Fram16Info_n

#endif


