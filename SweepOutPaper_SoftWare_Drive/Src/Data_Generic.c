#include "Data_Generic.h"

struct {
	volatile uint8_t PaperRemaining_Status;
	volatile uint8_t SingleOutPaper_Status;
	volatile uint8_t SingleOutPaper_FeedbackStatus;
}PaperDeviceInfo;


osThreadId PaperRemainingAndSingleRollStatusGenericTaskHandle;

void PaperRemainingAndSingleRollStatusGenericTask(void const * argument);

void Data_GenericFREERTOS_Init(void)
{
	/* definition and creation of PaperRemainingAndSingleRollStatusGenericTask */
	osThreadDef(PaperRemainingAndSingleRollStatusGenericTask, PaperRemainingAndSingleRollStatusGenericTask, osPriorityNormal, 0, 128);
	PaperRemainingAndSingleRollStatusGenericTaskHandle = osThreadCreate(osThread(PaperRemainingAndSingleRollStatusGenericTask), NULL);		
}

	uint16_t AD_Data[2];
/* PaperRemainingAndSingleRollStatusGenericTask function */
void PaperRemainingAndSingleRollStatusGenericTask(void const * argument)
{	
  /* USER CODE BEGIN PaperRemainingAndSingleRollStatusGenericTask */
  /* Infinite loop */	

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)AD_Data, 10);
	for(;;)
  {						
		if(AD_Data[0]<3000){
			PaperDeviceInfo.PaperRemaining_Status=1;
		}
		else{
			PaperDeviceInfo.PaperRemaining_Status=0;
		}			
		if(AD_Data[1]<3700){
			PaperDeviceInfo.SingleOutPaper_Status=1;
		}
		else{
			PaperDeviceInfo.SingleOutPaper_Status=0;
		}
		osDelay(5);
  }
  /* USER CODE END PaperRemainingAndSingleRollStatusGenericTask */
}



/*
*****************************************************************************
*@brief		取值函数，搬运纸量剩余状态及出纸状态数据
*@param		PaperDeviceInfo_n name_num：索取数据对应的枚举变量@PaperDeviceInfo_n
*@param		void*extern_data：存放传递出来的数据
*@retval	None
*@par
*****************************************************************************
*/
void Get_PaperDeviceInfo_Data(PaperDeviceInfo_n name_num,void*extern_data)//*extern_data用来存放get到的值
{
	if(name_num==PaperRemaining_Status_n){
		*(uint8_t*)extern_data=PaperDeviceInfo.PaperRemaining_Status;
	} 
	else if(name_num==SingleOutPaper_Status_n){
		*(uint8_t*)extern_data=PaperDeviceInfo.SingleOutPaper_Status;
	}	
	else if(name_num==SingleOutPaper_FeedbackStatus_n){
		*(uint8_t*)extern_data=PaperDeviceInfo.SingleOutPaper_FeedbackStatus;
		PaperDeviceInfo.SingleOutPaper_FeedbackStatus=0x00;
	}	
}

/*
*****************************************************************************
*@brief		修改纸量剩余状态及出纸状态数据
*@param		PaperDeviceInfo_n name_num：修改数据对应的枚举变量@PaperDeviceInfo_n
*@param		void* extern_data：传递修改的目标值
*@retval	None
*@par
*****************************************************************************
*/
void Set_PaperDeviceInfo_Data(PaperDeviceInfo_n name_num,void*extern_data)//*extern_data用来存放get到的值
{
	if(name_num==PaperRemaining_Status_n){
		PaperDeviceInfo.PaperRemaining_Status=*(uint8_t*)extern_data;
	} 
	else if(name_num==SingleOutPaper_Status_n){
		PaperDeviceInfo.SingleOutPaper_Status=*(uint8_t*)extern_data;
	}	
	else if(name_num==SingleOutPaper_FeedbackStatus_n){
		PaperDeviceInfo.SingleOutPaper_FeedbackStatus=*(uint8_t*)extern_data;
	}	
}


