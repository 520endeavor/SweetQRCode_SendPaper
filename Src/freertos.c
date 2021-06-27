/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "usart.h"
#include "gpio.h"
#include "AT24Cxx.h" 
#include "tim.h"
#include "adc.h"	
#include "string.h"
#include "MatrixKeyboard4By4.h" 
#include "Sim900A_GPRS_Communication.h" 
#include "HostSeverGprs_AT24CxxMem.h"
#include "Data_Generic.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
#define Ratio_PaperLenAndTimerDelay				10								//出纸长度与延时时常的比例
#define ManualSet_PaperLength_Limits_Up		120								//手动设置纸长时，上限:120cm
#define	ManualSet_PaperLength_Limits_Down	0									//手动设置纸长时，下限:0cm
#define	ManualSet_PaperLength_StepChange	20								//手动设置纸长时，单次改变步长:20Cm

volatile uint8_t OutPaper_State_Sum=0;

uint32_t PaperLenAndTime_Convert(uint8_t PaperLen);					//纸长与出纸电机开启时间换算函数
void CutPaperAndCloseTheExit(uint8_t CutPaperState);				//控制切纸开启和关闭函数，含出纸前后开启、关闭出口
void SendOutPaper_Action(uint8_t OutPaper_SetLength_Data);

/* USER CODE BEGIN Variables */
osThreadId Server_SendOutPaperTaskHandle;
osThreadId SwitchSetPaperLengthTaskHandle;
osThreadId Switch_SendOutPaperTaskHandle;
osThreadId CheckOutPaperStateTaskHandle;
osThreadId RunningLightTaskHandle;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void Server_SendOutPaperTask(void const * argument);
void SwitchSetPaperLengthTask(void const * argument);
void Switch_SendOutPaperTask(void const * argument);
void CheckOutPaperStateTask(void const * argument);
void RunningLightTask(void const * argument);


void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
	/* definition and creation of Server_SendOutPaperTask */
  osThreadDef(Server_SendOutPaperTask, Server_SendOutPaperTask, osPriorityNormal, 0, 128);
  Server_SendOutPaperTaskHandle = osThreadCreate(osThread(Server_SendOutPaperTask), NULL);
	/* definition and creation of Switch_SendOutPaperTask */
  osThreadDef(Switch_SendOutPaperTask, Switch_SendOutPaperTask, osPriorityNormal, 0, 128);
  Switch_SendOutPaperTaskHandle = osThreadCreate(osThread(Switch_SendOutPaperTask), NULL);
  /* definition and creation of SwitchSetPaperLengthTask */
  osThreadDef(SwitchSetPaperLengthTask, SwitchSetPaperLengthTask, osPriorityNormal, 0, 128);
  SwitchSetPaperLengthTaskHandle = osThreadCreate(osThread(SwitchSetPaperLengthTask), NULL);
	/* definition and creation of CheckOutPaperStateTask */
  osThreadDef(CheckOutPaperStateTask, CheckOutPaperStateTask, osPriorityNormal, 0, 128);
  CheckOutPaperStateTaskHandle = osThreadCreate(osThread(CheckOutPaperStateTask), NULL);
	/* definition and creation of RunningLightTask */
  osThreadDef(RunningLightTask, RunningLightTask, osPriorityNormal, 0, 64);
  RunningLightTaskHandle = osThreadCreate(osThread(RunningLightTask), NULL);
	
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* Server_SendOutPaperTask function */
void Server_SendOutPaperTask(void const * argument)
{
  /* USER CODE BEGIN Server_SendOutPaperTask */
  /* Infinite loop */
	WriteFram16_QueueTypedef WriteFram16_QueueData;
	uint8_t Sever_OutPaper_Cmd=0,Sever_OutPaper_SetModeOfLength_Cmd=0,Sever_OutPaper_SetLength_Data=0,Fram16_OutPaper_SetLength_Data=0;
	vTaskSuspend(CheckOutPaperStateTaskHandle);
  for(;;)
  {	
		xEventGroupWaitBits(Get_Sim900A_GPRS_ReceiveAndSwitchEventGroupHandle(),0x01,pdTRUE,pdFALSE,portMAX_DELAY);
		Get_GPRS_Receive_Data(OutPaper_Cmd_n,&Sever_OutPaper_Cmd);
		Get_GPRS_Receive_Data(OutPaper_SetModeOfLength_Cmd_n,&Sever_OutPaper_SetModeOfLength_Cmd);
		Get_GPRS_Receive_Data(OutPaper_SetLength_Data_n,&Sever_OutPaper_SetLength_Data);
		Get_Fram16Info_Data(Fram16CurrentSettingPaperAmount_n,&Fram16_OutPaper_SetLength_Data);			//获取Fram16中的纸长设置
		WriteFram16_QueueData.WriteFram16_MemoryID=Fram16CurrentSettingPaperAmountAddress;
		WriteFram16_QueueData.WriteFram16_MemoryData=Sever_OutPaper_SetLength_Data;
		if(Sever_OutPaper_SetModeOfLength_Cmd==0x03){																								//永久修改纸长
			xQueueSend(Get_WriteFram16QueueHandle(),&WriteFram16_QueueData,(TickType_t)0 );
		}		
		if(Sever_OutPaper_Cmd==1){
			if(Sever_OutPaper_SetModeOfLength_Cmd==0x01){																							//不修改纸长		
				SendOutPaper_Action(Fram16_OutPaper_SetLength_Data);				
			}
			else if((Sever_OutPaper_SetModeOfLength_Cmd==0x02)||(Sever_OutPaper_SetModeOfLength_Cmd==0x03)){	//有改变纸长命令
				SendOutPaper_Action(Sever_OutPaper_SetLength_Data);				
			}
		}
  }
  /* USER CODE END Server_SendOutPaperTask */
}

/* Switch_SendOutPaperTask function */
void Switch_SendOutPaperTask(void const * argument)
{
  /* USER CODE BEGIN Switch_SendOutPaperTask */
  /* Infinite loop */
	uint8_t OutPaperKey_key_up[2]={1,1},SwitchValue[2]={0,0};
	uint8_t Fram16_OutPaper_SetLength_Data=0,SendOut_Flag=0;
  for(;;)
  {
		for(uint8_t i=0;i<2;i++)
		{
			if(i==0){
				SwitchValue[i]=HAL_GPIO_ReadPin(ManualOutPaper_FourByOneKey1_GPIO_Port, ManualOutPaper_FourByOneKey1_Pin);
			}
			else{
				SwitchValue[i]=HAL_GPIO_ReadPin(ManualOutPaperAloneKey_GPIO_Port, ManualOutPaperAloneKey_Pin);
			}
			if(OutPaperKey_key_up[i]&&SwitchValue[i]==GPIO_PIN_SET){
				osDelay(10);
				OutPaperKey_key_up[i]=0;
				if(SwitchValue[i]==GPIO_PIN_SET){																													//独立出纸开关或者4*1出纸开关按下
					SendOut_Flag=1;																																					//置位出纸标志位
					if(i==1){
						HAL_GPIO_WritePin(GPIOD,ManualOutPaperAloneLED_Pin, GPIO_PIN_SET);										//独立出纸开关，打开亮灯指示
					}		
				}				
			}
			else if(SwitchValue[i]==GPIO_PIN_RESET){
				OutPaperKey_key_up[i]=1;
				if(i==1){																																									//独立出纸开关，关闭亮灯指示
					HAL_GPIO_WritePin(GPIOD,ManualOutPaperAloneLED_Pin, GPIO_PIN_RESET);
				}
			}
		}
		if(SendOut_Flag==1){
			SendOut_Flag=0;
			Get_Fram16Info_Data(Fram16CurrentSettingPaperAmount_n,&Fram16_OutPaper_SetLength_Data);			//获取Fram16中的纸长设置
			SendOutPaper_Action(Fram16_OutPaper_SetLength_Data);	
		}			
    osDelay(20);
  }
  /* USER CODE END Switch_SendOutPaperTask */
}

/* SwitchSetPaperLengthTask function */
void SwitchSetPaperLengthTask(void const * argument)
{
  /* USER CODE BEGIN SwitchSetPaperLengthTask */
  /* Infinite loop */
	uint8_t FourByOneKey2_key_up=1,Fram16_OutPaper_SetLength_Data=0;
	WriteFram16_QueueTypedef WriteFram16_QueueData;
  for(;;)
  {
		if((FourByOneKey2_key_up==1)&&HAL_GPIO_ReadPin(PaperMoveExtent_FourByOneKey2_GPIO_Port, PaperMoveExtent_FourByOneKey2_Pin)==GPIO_PIN_SET){
			osDelay(10);
			FourByOneKey2_key_up=0;
			if(HAL_GPIO_ReadPin(PaperMoveExtent_FourByOneKey2_GPIO_Port, PaperMoveExtent_FourByOneKey2_Pin)==GPIO_PIN_SET){
				Get_Fram16Info_Data(Fram16CurrentSettingPaperAmount_n,&Fram16_OutPaper_SetLength_Data);			//获取Fram16中的纸长设置
				WriteFram16_QueueData.WriteFram16_MemoryID=Fram16CurrentSettingPaperAmountAddress;
				if((Fram16_OutPaper_SetLength_Data==ManualSet_PaperLength_Limits_Up)||(Fram16_OutPaper_SetLength_Data<=ManualSet_PaperLength_Limits_Down)){
					WriteFram16_QueueData.WriteFram16_MemoryData=ManualSet_PaperLength_Limits_Down+ManualSet_PaperLength_StepChange;
				}
				else if(Fram16_OutPaper_SetLength_Data+ManualSet_PaperLength_StepChange>=ManualSet_PaperLength_Limits_Up){
					WriteFram16_QueueData.WriteFram16_MemoryData=ManualSet_PaperLength_Limits_Up;
				}
				else{
					WriteFram16_QueueData.WriteFram16_MemoryData=Fram16_OutPaper_SetLength_Data+ManualSet_PaperLength_StepChange;
				}
				xQueueSend(Get_WriteFram16QueueHandle(),&WriteFram16_QueueData,(TickType_t)0 );				
			}
		}
		else if(HAL_GPIO_ReadPin(PaperMoveExtent_FourByOneKey2_GPIO_Port, PaperMoveExtent_FourByOneKey2_Pin)==GPIO_PIN_RESET){
			FourByOneKey2_key_up=1;
		}
    osDelay(20);
  }
  /* USER CODE END SwitchSetPaperLengthTask */
}

/* CheckOutPaperStateTask function */
void CheckOutPaperStateTask(void const * argument)
{
  /* USER CODE BEGIN CheckOutPaperStateTask */
  /* Infinite loop */
	uint8_t SingleOutPaper_State=0;
  for(;;)
  {
		Get_PaperDeviceInfo_Data(SingleOutPaper_Status_n,&SingleOutPaper_State);
		if(SingleOutPaper_State==1){
			if(OutPaper_State_Sum<0xFF){
				OutPaper_State_Sum++;
			}			
		}
    osDelay(5);
  }
  /* USER CODE END CheckOutPaperStateTask */
}

/* RunningLightTask function */
void RunningLightTask(void const * argument)
{
  /* USER CODE BEGIN RunningLightTask */
  /* Infinite loop */
  for(;;)
  {			
		HAL_GPIO_TogglePin(RunningLight_GPIO_Port, RunningLight_Pin);
    osDelay(200);
  }
  /* USER CODE END RunningLightTask */
}

/* USER CODE BEGIN Application */
/*
*****************************************************************************
*@brief		纸长与出纸电机开启时间换算函数
*@param		uint8_t PaperLen：输入出纸长度
*@retval	uint32_t Time_millisec：出纸电机开启时长，单位ms
*@par
*****************************************************************************
*/
uint32_t PaperLenAndTime_Convert(uint8_t PaperLen)   
{
	uint32_t Time_millisec=0;
	if((uint32_t)PaperLen*Ratio_PaperLenAndTimerDelay>=0xFFFFFFFF){
		Time_millisec=0xFFFFFFFF;
	}
	else if((uint32_t)PaperLen*Ratio_PaperLenAndTimerDelay<=1){
		Time_millisec=1;
	}
	else {
		Time_millisec=(uint32_t)PaperLen*Ratio_PaperLenAndTimerDelay;
	}
	return Time_millisec;
}

/*
*****************************************************************************
*@brief		控制切纸开启和关闭函数，含出纸前后开启、关闭出口
*@param		uint8_t CutPaperState：0关闭切纸，1开启切纸
*@retval	None
*@par
*****************************************************************************
*/
uint8_t PaperCuttingMachinePhotodiode_Value=0;
void CutPaperAndCtrlTheExit(uint8_t CutPaperState)
{	
//	AdjustSpeedAndDir_CutPaperMotor(500,1);
	PaperCuttingMachinePhotodiode_Value=HAL_GPIO_ReadPin(PaperCuttingMachinePhotodiode_Input_GPIO_Port, PaperCuttingMachinePhotodiode_Input_Pin);		
	osDelay(20);
}

/*
*****************************************************************************
*@brief		出纸过程函数
*@param		uint8_t OutPaper_SetLength_Data: 出纸长度变量
*@retval	None
*@par
*****************************************************************************
*/
void SendOutPaper_Action(uint8_t OutPaper_SetLength_Data)
{		
	uint8_t Clear_SingleOutPaper_State=0;
	uint8_t State_SET=1,State_RESET=0;
	Set_PaperDeviceInfo_Data(SingleOutPaper_Status_n,&Clear_SingleOutPaper_State);
	OutPaper_State_Sum=0;
	vTaskResume(CheckOutPaperStateTaskHandle);			
	OutPaperMotor_Ctrl(1);			
	osDelay(PaperLenAndTime_Convert(OutPaper_SetLength_Data));		
	OutPaperMotor_Ctrl(0);	
	vTaskSuspend(CheckOutPaperStateTaskHandle);
	if(OutPaper_State_Sum>0){
		Set_PaperDeviceInfo_Data(SingleOutPaper_FeedbackStatus_n,&State_SET);										//置位出纸成功状态
	}
	else{
		Set_PaperDeviceInfo_Data(SingleOutPaper_FeedbackStatus_n,&State_RESET);									//复位出纸成功状态
	}
	OutPaper_State_Sum=0;			
	xTaskNotifyGive(Get_Sim900A_GPRS_Feadback_TransmitTaskHandle());
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

