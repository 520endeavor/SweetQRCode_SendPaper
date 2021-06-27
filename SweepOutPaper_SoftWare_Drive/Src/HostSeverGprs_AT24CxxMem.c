#include "HostSeverGprs_AT24CxxMem.h"

struct {
	volatile uint8_t Fram16Status;	
	volatile uint8_t Fram16CurrentSettingPaperAmount;
}Fram16Info;


/* USER CODE BEGIN Variables */
osThreadId CheckFram16StatusTaskHandle;
osThreadId WriteFram16TaskHandle;
osThreadId ReadFram16TaskHandle;
osMessageQId WriteFram16QueueHandle;


/* Function prototypes -------------------------------------------------------*/
void CheckFram16StatusTask(void const * argument);
void WriteFram16Task(void const * argument);
void ReadFram16Task(void const * argument);

osMessageQId Get_WriteFram16QueueHandle(void){
	return WriteFram16QueueHandle;
}

osThreadId Get_WriteFram16TaskHandle(void){
	return WriteFram16TaskHandle;
}


void HostSeverGprs_At24CXX_FREERTOS_Init(void){
  /* Create the thread(s) */
	/* definition and creation of CheckFram16StstusTask */
  osThreadDef(CheckFram16StatusTask, CheckFram16StatusTask, osPriorityNormal, 0, 128);
  CheckFram16StatusTaskHandle = osThreadCreate(osThread(CheckFram16StatusTask), NULL);
	/* definition and creation of WriteFram16Task */
  osThreadDef(WriteFram16Task, WriteFram16Task, osPriorityNormal, 0, 128);
  WriteFram16TaskHandle = osThreadCreate(osThread(WriteFram16Task), NULL);
	/* definition and creation of ReadFram16Task */
  osThreadDef(ReadFram16Task, ReadFram16Task, osPriorityNormal, 0, 128);
  ReadFram16TaskHandle = osThreadCreate(osThread(ReadFram16Task), NULL);
	/* definition and creation of WriteFram16Queue */
  osMessageQDef(WriteFram16Queue, 2, WriteFram16_QueueTypedef);
  WriteFram16QueueHandle = osMessageCreate(osMessageQ(WriteFram16Queue), NULL);
}


/* CheckFram16StatusTask function */
void CheckFram16StatusTask(void const * argument)
{
  /* USER CODE BEGIN CheckFram16StatusTask */
  /* Infinite loop */
	for(;;)
  {	   		
		Fram16Info.Fram16Status=AT24Cxx_Check()^0x01;				
		osDelay(6000);			
  }
  /* USER CODE END CheckFram16StatusTask */
}

/* WriteFram16Task function */
void WriteFram16Task(void const * argument)
{
  /* USER CODE BEGIN WriteFram16Task */
  /* Infinite loop */
	WriteFram16_QueueTypedef WriteFram16_QueueData;
	for(;;)
  {	  
		if(xQueueReceive(WriteFram16QueueHandle,&WriteFram16_QueueData,(TickType_t)0)==pdTRUE){	
			if(AT24Cxx_Check()==0){
				AT24Cxx_WriteOneByte(WriteFram16_QueueData.WriteFram16_MemoryID,WriteFram16_QueueData.WriteFram16_MemoryData);				
				xTaskNotify(ReadFram16TaskHandle,(uint32_t)WriteFram16_QueueData.WriteFram16_MemoryID,eSetValueWithOverwrite);	
			}	
		}			
		osDelay(2);	
  }
  /* USER CODE END WriteFram16Task */
}

/* ReadFram16Task function */
void ReadFram16Task(void const * argument)
{
  /* USER CODE BEGIN WriteFram16Task */
  /* Infinite loop */
	uint8_t Memory_ID;
	for(;;)
  {	    
		xTaskNotifyWait( 0,0xFFFFFFFF,(uint32_t*)&Memory_ID,portMAX_DELAY);
		if(Memory_ID==Fram16CurrentSettingPaperAmountAddress){
			Fram16Info.Fram16CurrentSettingPaperAmount=AT24Cxx_ReadOneByte(Memory_ID);	
		}			
  }
  /* USER CODE END ReadFram16Task */
}


/*
*****************************************************************************
*@brief		取值函数，搬运Fram16状态数据
*@param		Fram16Info_n name_num：索取数据对应的枚举变量@Fram16Info_n
*@param		void*extern_data：存放传递出来的数据
*@retval	None
*@par
*****************************************************************************
*/
void Get_Fram16Info_Data(Fram16Info_n name_num,void*extern_data)//*extern_data用来存放get到的值
{
	if(name_num==Fram16Status_n){
		*(uint8_t*)extern_data=Fram16Info.Fram16Status;
	} 
	if(name_num==Fram16CurrentSettingPaperAmount_n){
		*(uint8_t*)extern_data=Fram16Info.Fram16CurrentSettingPaperAmount;
	} 
}

/*
*****************************************************************************
*@brief		修改ram16状态数据
*@param		Fram16Info_n name_num：索取数据对应的枚举变量@Fram16Info_n
*@param		void*extern_data：存放传递出来的数据
*@retval	None
*@par
*****************************************************************************
*/
void Set_Fram16Info_Data(Fram16Info_n name_num,void*extern_data)//*extern_data用来存放get到的值
{
	if(name_num==Fram16Status_n){
		Fram16Info.Fram16Status=*(uint8_t*)extern_data;
	} 
	if(name_num==Fram16CurrentSettingPaperAmount_n){
		Fram16Info.Fram16CurrentSettingPaperAmount=*(uint8_t*)extern_data;
	} 
}

/*
*****************************************************************************
*@brief		Fram16数据清除函数，只运行一次
*@param		None
*@retval	None
*@par
*****************************************************************************
*/
static void CLEARData_For_Fram16(void)
{
	AT24Cxx_WriteOneByte(Fram16CurrentSettingPaperAmountAddress,0x3C);					//出厂设置60CM			
}

/*
*****************************************************************************
*@brief		初次上电读Fram16数据函数
*@param		None
*@retval	None
*@par
*****************************************************************************
*/
void FirstPowerOnReadFram16Init(void)
{
//	CLEARData_For_Fram16();
	Fram16Info.Fram16CurrentSettingPaperAmount=AT24Cxx_ReadOneByte(Fram16CurrentSettingPaperAmountAddress);
}

