#ifndef __DATA_GENERIC_H
#define __DATA_GENERIC_H

#include "stm32f4xx_hal.h"
#include "adc.h"
#include "dma.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

typedef enum{
	PaperRemaining_Status_n=1,
	SingleOutPaper_Status_n,
	SingleOutPaper_FeedbackStatus_n
}PaperDeviceInfo_n;

void Data_GenericFREERTOS_Init(void);									//Data_Generic中建立任务的函数

void Get_PaperDeviceInfo_Data(PaperDeviceInfo_n name_num,void*extern_data);//取值函数，搬运纸量剩余状态及出纸状态数据@PaperDeviceInfo_n

void Set_PaperDeviceInfo_Data(PaperDeviceInfo_n name_num,void*extern_data);//修改纸量剩余状态及出纸状态数据@PaperDeviceInfo_n

#endif

