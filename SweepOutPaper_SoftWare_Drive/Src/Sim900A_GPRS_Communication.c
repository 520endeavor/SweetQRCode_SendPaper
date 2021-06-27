#include "Sim900A_GPRS_Communication.h" 


volatile uint8_t USART1_TX_BUF[USART1_MAX_SEND_LEN]; 				//发送缓冲,最大USART1_MAX_SEND_LEN字节   	  	
volatile uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART1_MAX_RECV_LEN个字节.
volatile uint16_t USART1_RX_STA=0;													//接收缓存状态变量
volatile uint8_t Heartbeat_Sending_Flag=0;									//正在发送心跳标志位
volatile uint8_t Return_usartTransmit=0;										//辅助调试用变量
uint8_t Init_Flag=0;																				//辅助调试用变量


static uint8_t AT_QuerySimCardStatus[]="AT+CPIN?\r";																				//查询SIM卡状态			指令
static uint8_t AT_DialNumber[]="ATD17352890207;\r";																					//电话卡拨打电话    指令
static uint8_t AT_MobileStationCategory[]="AT+CGCLASS=\"B\"\r";															//设置移动台类别    指令
static uint8_t AT_ConnectionMethod[]="AT+CGDCONT=1,\"IP\",\"CMNET\"\r";											//设置连接方式      指令
static uint8_t AT_AccessPoint[]="AT+CGATT=1\r";																							//设置接入点        指令
static uint8_t AT_AttachedToGPRS[]="AT+CIPCSGP=1,\"CMNET\"\r";															//设置附着GPRS业务  指令
static uint8_t AT_TheMachine_Port[]="AT+CLPORT=\"TCP\",\"2000\"\r";													//设置本机端口号    指令
static uint8_t AT_SeverIP_And_Port[]="AT+CIPSTART=\"TCP\",\"120.27.121.73\",\"9501\"\r";		//连接TCP IP&Port   指令
static uint8_t AT_Send_cmd[]="AT+CIPSEND\r";																								//GPRS通讯时发送    指令
static uint8_t AT_InquireProductSerialer[]="AT+CGSN\r";																			//查询产品序列号		指令
static uint8_t AT_TestText[]="{\"type\":\"register\",\"device_id\":\"869668020850594\"}\r";	//发送注册信息


struct{
	volatile uint8_t OutPaper_Cmd;
	volatile uint8_t OutPaper_SetModeOfLength_Cmd;
	volatile uint8_t OutPaper_SetLength_Data;
}GPRS_Receive_Data;


osThreadId Sim900A_GPRS_ReceiveTaskHandle;
osThreadId Sim900A_GPRS_PowerOn_InitTaskHandle;
osThreadId Sim900A_GPRS_AnalysisTaskHandle;
osThreadId Sim900A_GPRS_Initiative_TransmitTaskHandle;
osThreadId Sim900A_GPRS_Feadback_TransmitTaskHandle;
osThreadId Sim900A_GPRS_Heartbeat_TransmitTaskHandle;
osThreadId Sim900A_GPRS_Heartbeat_GuardTaskHandle;
EventGroupHandle_t Sim900A_GPRS_ReceiveAndSwitchEventGroupHandle;


osThreadId Get_Sim900A_GPRS_ReceiveTaskHandle(void){
	return Sim900A_GPRS_ReceiveTaskHandle;
}
osThreadId Get_Sim900A_GPRS_Initiative_TransmitTaskHandle(void){
	return Sim900A_GPRS_Initiative_TransmitTaskHandle;
}
osThreadId Get_Sim900A_GPRS_Feadback_TransmitTaskHandle(void){
	return Sim900A_GPRS_Feadback_TransmitTaskHandle;
}
EventGroupHandle_t Get_Sim900A_GPRS_ReceiveAndSwitchEventGroupHandle(void){
	return Sim900A_GPRS_ReceiveAndSwitchEventGroupHandle;
}


uint8_t* Sim900A_Check_cmd(uint8_t *str);																	//检测接收Sim900A应答 函数
uint8_t Sim900A_WaitAck(uint8_t *ack,uint16_t waittime);									//控制器发送AT指令后等待应答函数
uint8_t Sim900A_Send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);		//向Sim900AS发送命令 函数
void Sim900A_RX_STA_Ctrl(uint8_t mode);																		//操作USART1_RX_BUF缓存区函数
uint8_t SIM900A_GPRS_PROTOCOL_Analysis(void);															//接收到Sim900A数据后，进行GPRS协议解析
void Sim900A_TCP_Close(void);																							//控制Sim900A关闭TCP通讯
uint8_t Sim900A_TCP_Init(void);																						//开机时初始化Sim900A的TCP通讯


void Sim900A_GPRS_ReceiveTask(void const * argument);
void Sim900A_GPRS_PowerOn_InitTask(void const * argument);
void Sim900A_GPRS_AnalysisTask(void const * argument);
void Sim900A_GPRS_Initiative_TransmitTask(void const * argument);
void Sim900A_GPRS_Feadback_TransmitTask(void const * argument);
void Sim900A_GPRS_Heartbeat_TransmitTask(void const * argument);
void Sim900A_GPRS_Heartbeat_GuardTask(void const * argument);


void Sim900A_GPRS_FREERTOS_Init(void){
	/* definition and creation of Sim900A_GPRS_ReceiveTask */
	osThreadDef(Sim900A_GPRS_ReceiveTask, Sim900A_GPRS_ReceiveTask, osPriorityNormal, 0, 128);
	Sim900A_GPRS_ReceiveTaskHandle = osThreadCreate(osThread(Sim900A_GPRS_ReceiveTask), NULL);
	/* definition and creation of Sim900A_GPRS_PowerOn_InitTask */
	osThreadDef(Sim900A_GPRS_PowerOn_InitTask, Sim900A_GPRS_PowerOn_InitTask, osPriorityNormal, 0, 128);
	Sim900A_GPRS_PowerOn_InitTaskHandle = osThreadCreate(osThread(Sim900A_GPRS_PowerOn_InitTask), NULL);
	/* definition and creation of Sim900A_GPRS_Heartbeat_GuardTask */
	osThreadDef(Sim900A_GPRS_Heartbeat_GuardTask, Sim900A_GPRS_Heartbeat_GuardTask, osPriorityNormal, 0, 128);
	Sim900A_GPRS_Heartbeat_GuardTaskHandle = osThreadCreate(osThread(Sim900A_GPRS_Heartbeat_GuardTask), NULL);	
	/* definition and creation of Sim900A_GPRS_ReceiveEventGroup */
	Sim900A_GPRS_ReceiveAndSwitchEventGroupHandle=xEventGroupCreate();
}


/* Sim900A_GPRS_ReceiveTask function */
void Sim900A_GPRS_ReceiveTask(void const * argument)
{
  /* USER CODE BEGIN Sim900A_GPRS_ReceiveTask */
  /* Infinite loop */
	for(;;)
  { 
		if(USART1_RX_STA==0){
			HAL_UART_Receive_DMA(&huart1,USART1_RX_BUF, USART1_MAX_RECV_LEN);
		}
		osDelay(1);
	}
	/* USER CODE END Sim900A_GPRS_ReceiveTask */
}

/* Sim900A_GPRS_PowerOn_InitTask function */
void Sim900A_GPRS_PowerOn_InitTask(void const * argument)
{
  /* USER CODE BEGIN Sim900A_GPRS_PowerOn_InitTask */
  /* Infinite loop */
	osDelay(20000);
	Init_Flag=Sim900A_TCP_Init();
	for(;;)
  { 
		/* definition and creation of Sim900A_GPRS_AnalysisTask */
		osThreadDef(Sim900A_GPRS_AnalysisTask, Sim900A_GPRS_AnalysisTask, osPriorityNormal, 0, 128);
		Sim900A_GPRS_AnalysisTaskHandle = osThreadCreate(osThread(Sim900A_GPRS_AnalysisTask), NULL);
		/* definition and creation of Sim900A_GPRS_Initiative_TransmitTask */
		osThreadDef(Sim900A_GPRS_Initiative_TransmitTask, Sim900A_GPRS_Initiative_TransmitTask, osPriorityNormal, 0, 128);
		Sim900A_GPRS_Initiative_TransmitTaskHandle = osThreadCreate(osThread(Sim900A_GPRS_Initiative_TransmitTask), NULL);
		/* definition and creation of Sim900A_GPRS_Feadback_TransmitTask */
		osThreadDef(Sim900A_GPRS_Feadback_TransmitTask, Sim900A_GPRS_Feadback_TransmitTask, osPriorityNormal, 0, 128);
		Sim900A_GPRS_Feadback_TransmitTaskHandle = osThreadCreate(osThread(Sim900A_GPRS_Feadback_TransmitTask), NULL);
		/* definition and creation of Sim900A_GPRS_Heartbeat_TransmitTask */
		osThreadDef(Sim900A_GPRS_Heartbeat_TransmitTask, Sim900A_GPRS_Heartbeat_TransmitTask, osPriorityNormal, 0, 128);
		Sim900A_GPRS_Heartbeat_TransmitTaskHandle = osThreadCreate(osThread(Sim900A_GPRS_Heartbeat_TransmitTask), NULL);
		
		vTaskDelete( NULL );	
		osDelay(2);
	}
	/* USER CODE END Sim900A_GPRS_PowerOn_InitTask */
}

/* Sim900A_GPRS_AnalysisTask function */
void Sim900A_GPRS_AnalysisTask(void const * argument)
{
  /* USER CODE BEGIN Sim900A_GPRS_AnalysisTask */
  /* Infinite loop */
	for(;;)
  { 
		if(SIM900A_GPRS_PROTOCOL_Analysis()){
			for(uint16_t i=0;i<(USART1_RX_STA&0x7FFF);i++)
			{
				USART1_RX_BUF[i]=0x00;
				USART1_RX_STA=0;
				xEventGroupSetBits(Sim900A_GPRS_ReceiveAndSwitchEventGroupHandle,0x01);
			}			
			xTaskNotifyGive(Sim900A_GPRS_Feadback_TransmitTaskHandle);
		}
		osDelay(1);
	}
	/* USER CODE END Sim900A_GPRS_AnalysisTask */
}

/* Sim900A_GPRS_Initiative_TransmitTask function */
void Sim900A_GPRS_Initiative_TransmitTask(void const * argument)
{
  /* USER CODE BEGIN Sim900A_GPRS_Initiative_TransmitTask */
  /* Infinite loop */	
	uint8_t Initiative_Transmit_Content[20];
	uint8_t PaperRemainingState=0,Fram16State=0, CurrentSettingPaperAmount=0;
	uint8_t* Password_value;
	Initiative_Transmit_Content[0]=0xA6;
	Initiative_Transmit_Content[1]=0xA6;
	Initiative_Transmit_Content[2]=0x00;
	Initiative_Transmit_Content[4]=0x01;
	Initiative_Transmit_Content[6]=0x02;
	Initiative_Transmit_Content[8]=0x03;
	Initiative_Transmit_Content[15]=0x09;
	Initiative_Transmit_Content[17]=0xA0;	
	Initiative_Transmit_Content[19]=0x1A;	
	for(;;)
  { 	
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		vTaskSuspend(Sim900A_GPRS_Heartbeat_TransmitTaskHandle);
		Password_value=Get_PasswordInfo_Pointer();
		Get_PaperDeviceInfo_Data(PaperRemaining_Status_n,&PaperRemainingState);
		Get_Fram16Info_Data(Fram16Status_n,&Fram16State);
		Get_Fram16Info_Data(Fram16CurrentSettingPaperAmount_n,&CurrentSettingPaperAmount);
		Initiative_Transmit_Content[3]=PaperRemainingState;
		Initiative_Transmit_Content[5]=Fram16State;
		Initiative_Transmit_Content[7]=0x00;												//主动发送不检测出纸状态
		for(uint8_t i=0;i<6;i++)
		{
			Initiative_Transmit_Content[9+i]=Password_value[i];
		}
		if(CurrentSettingPaperAmount==0x1A){
			Initiative_Transmit_Content[16]=CurrentSettingPaperAmount+1;
		}
		else{
			Initiative_Transmit_Content[16]=CurrentSettingPaperAmount;
		}
		Initiative_Transmit_Content[18]=0x01;												//主动发送
		Sim900A_Send_cmd(AT_Send_cmd,">",100);
		HAL_UART_Transmit_DMA(&huart1, Initiative_Transmit_Content,20);			
		xTaskNotifyGive(Sim900A_GPRS_Heartbeat_GuardTaskHandle);		
	}
	/* USER CODE END Sim900A_GPRS_Initiative_TransmitTask */
}

/* Sim900A_GPRS_Feadback_TransmitTask function */
void Sim900A_GPRS_Feadback_TransmitTask(void const * argument)
{
  /* USER CODE BEGIN Sim900A_GPRS_Feadback_TransmitTask */
  /* Infinite loop */	
	uint8_t Feadback_Transmit_Content[20];
	uint8_t PaperRemainingState=0,Fram16State=0, SingleRoll_FeedbackState=0,CurrentSettingPaperAmount=0;
	uint8_t* Password_value;
	Feadback_Transmit_Content[0]=0xA6;
	Feadback_Transmit_Content[1]=0xA6;
	Feadback_Transmit_Content[2]=0x00;
	Feadback_Transmit_Content[4]=0x01;
	Feadback_Transmit_Content[6]=0x02;
	Feadback_Transmit_Content[8]=0x03;
	for(uint8_t i=0;i<6;i++)
	{
		Feadback_Transmit_Content[9+i]=0;
	}
	Feadback_Transmit_Content[15]=0x09;
	Feadback_Transmit_Content[17]=0xA0;	
	Feadback_Transmit_Content[19]=0x1A;
	for(;;)
  { 	
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		vTaskSuspend(Sim900A_GPRS_Heartbeat_TransmitTaskHandle);
		Get_PaperDeviceInfo_Data(PaperRemaining_Status_n,&PaperRemainingState);
		Get_PaperDeviceInfo_Data(SingleOutPaper_FeedbackStatus_n,&SingleRoll_FeedbackState);
		Get_Fram16Info_Data(Fram16Status_n,&Fram16State);
		Get_Fram16Info_Data(Fram16CurrentSettingPaperAmount_n,&CurrentSettingPaperAmount);
		Feadback_Transmit_Content[3]=PaperRemainingState;
		Feadback_Transmit_Content[5]=Fram16State;
		Feadback_Transmit_Content[7]=SingleRoll_FeedbackState;		
		if(CurrentSettingPaperAmount==0x1A){
			Feadback_Transmit_Content[16]=CurrentSettingPaperAmount+1;
		}
		else{
			Feadback_Transmit_Content[16]=CurrentSettingPaperAmount;
		}
		Feadback_Transmit_Content[18]=0x00;													//反馈发送
		Sim900A_Send_cmd(AT_Send_cmd,">",100);
		HAL_UART_Transmit_DMA(&huart1, Feadback_Transmit_Content,20);		
		vTaskResume(Sim900A_GPRS_Heartbeat_TransmitTaskHandle);
	}
	/* USER CODE END Sim900A_GPRS_Feadback_TransmitTask */
}

/* Sim900A_GPRS_Heartbeat_TransmitTask function */
void Sim900A_GPRS_Heartbeat_TransmitTask(void const * argument)
{
  /* USER CODE BEGIN Sim900A_GPRS_Heartbeat_TransmitTask */
  /* Infinite loop */
	uint8_t Heartbeat_Count=0,Heartbeat_Value[2]={0x01,0x1A};
	for(;;)
  { 
		Sim900A_Send_cmd(AT_Send_cmd,">",100);		
		HAL_UART_Transmit_DMA(&huart1, Heartbeat_Value,2);
		Sim900A_WaitAck(&Heartbeat_Value[1],100);
		USART1_RX_STA=0;
		if(Sim900A_WaitAck("SEND OK",100)==0){
			Heartbeat_Count++;
		}
		else{
			Heartbeat_Count=0;
		}
		if(Heartbeat_Count==8){
			Heartbeat_Count=0;
			Init_Flag=Sim900A_TCP_Init();
		}
		USART1_RX_STA=0;
		HAL_UART_Transmit_DMA(&huart2, USART1_RX_BUF,10);		
		osDelay(6000);
	}
	/* USER CODE END Sim900A_GPRS_Heartbeat_TransmitTask */
}

/* Sim900A_GPRS_Heartbeat_GuardTask function */
void Sim900A_GPRS_Heartbeat_GuardTask(void const * argument)
{
  /* USER CODE BEGIN Sim900A_GPRS_Heartbeat_GuardTask */
  /* Infinite loop */
	for(;;)
  { 
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		osDelay(6000);
		vTaskResume(Sim900A_GPRS_Heartbeat_TransmitTaskHandle);	
	}
	/* USER CODE END Sim900A_GPRS_Heartbeat_GuardTask */
}

/*
*****************************************************************************
*@brief		Sim900A发送命令后,检测接收到的应答
*@param		uint8_t *str:期待的应答结果		
*@retval	0:没有得到期待的应答结果  其他,期待应答结果的位置(str的位置)
*@par
*****************************************************************************
*/
uint8_t* Sim900A_Check_cmd(uint8_t *str)
{
	char *strx=0;
	if(USART1_RX_STA&0x8000){																		//接收到一次数据了	 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;										//添加结束符
		strx=strstr((const char*)USART1_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}

/*
*****************************************************************************
*@brief		控制器发送AT指令后等待应答函数
*@param		uint8_t *ack:期待的应答结果	
*@param		uint16_t waittime:应答最长等待时间，乘以10，单位ms
*@retval	0:没有得到期待的应答结果  1：得到期待的应答结果 
*@par
*****************************************************************************
*/
uint8_t Sim900A_WaitAck(uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0;
	while(--waittime)
	{	
		osDelay(10);		
		if((USART1_RX_STA&(1<<15))){
			if(Sim900A_Check_cmd(ack)){
				USART1_RX_STA=0;
				res=1;
			}			 
			break;
		}
	}
	 return	res;
}

/*
*****************************************************************************
*@brief		向Sim900AS发送命令
*@param		uint8_t *cmd:发送的命令字符串
*@param		uint8_t *ack:期待的应答结果	
*@param		uint16_t waittime:应答最长等待时间，乘以10，单位ms
*@retval	0:未接收到正确应答  1：接收到正确应答
*@par
*****************************************************************************
*/
uint8_t Sim900A_Send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	if(strstr((const char*)cmd,(const char*)"AT+CIPSTART")||(*cmd==0x1A&&strlen(cmd)==1)){
		Return_usartTransmit=HAL_UART_Transmit_DMA(&huart1, cmd, strlen(cmd));
		if(Return_usartTransmit==HAL_OK){
			if(strstr((const char*)cmd,(const char*)"AT+CIPSTART")){
				osDelay(1000);
			}
			else if(*cmd==0x1A&&strlen(cmd)==1){
				osDelay(600);
			}
			USART1_RX_STA=0;
		}
	}
	else{
		USART1_RX_STA=0;		
		if(strlen(cmd)<5){
			Return_usartTransmit=HAL_UART_Transmit(&huart1, cmd, strlen(cmd),100);	
		}
		else{
			Return_usartTransmit=HAL_UART_Transmit_DMA(&huart1, cmd, strlen(cmd));
		}
	}
	return Sim900A_WaitAck(ack,waittime);
} 

/*
*****************************************************************************
*@brief		接收到Sim900A数据后，操作USART1_RX_BUF缓存区
*@param		uint8_t mode:操作模式，0：不清零USART1_RX_STA 1:清零USART1_RX_STA
*@retval	None
*@par
*****************************************************************************
*/
void Sim900A_RX_STA_Ctrl(uint8_t mode)
{
	if(USART1_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//添加结束符
		if(mode){
			USART1_RX_STA=0;
		}
	} 
}

/*
*****************************************************************************
*@brief		接收到Sim900A数据后，进行GPRS协议解析
*@param		uint8_t*receive_word:接收数据首地址
*@retval	0:接收的数据不符合协议，1：接受的数据符合协议并解析成功
*@par
*****************************************************************************
*/
uint8_t SIM900A_GPRS_PROTOCOL_Analysis(void)
{
	uint8_t Gprs_Protocol_Flag=0,Header_Index=0;	
	if(USART1_RX_STA&(1<<15)){
		if((USART1_RX_STA&0x7FFF)>=GPRS_PROTOCOL_RECV_LEN){
			for(uint16_t i=0;i<(USART1_RX_STA&0x7FFF)-1;i++)
			{
				if((USART1_RX_BUF[i]==0xA7)&&(USART1_RX_BUF[i+1]==0xA7)){
					if((USART1_RX_STA&0x7FFF)-i>=GPRS_PROTOCOL_RECV_LEN){
						Gprs_Protocol_Flag=1;
						Header_Index=i;
						break;
					}
				}			
			}
			if(Gprs_Protocol_Flag==0){
				return 0;
			}
			else{
				if((USART1_RX_BUF[Header_Index+2]==0x50)&&(USART1_RX_BUF[Header_Index+4]==0x51)&&(USART1_RX_BUF[Header_Index+6]==0x52)){
					GPRS_Receive_Data.OutPaper_Cmd=USART1_RX_BUF[Header_Index+3];
					GPRS_Receive_Data.OutPaper_SetLength_Data=(USART1_RX_BUF[Header_Index+5]>>5)&0x03;
					GPRS_Receive_Data.OutPaper_SetModeOfLength_Cmd=USART1_RX_BUF[Header_Index+7];
					return 1;
				}				
			}			
		}
		return 0;
	}
}

/*
*****************************************************************************
*@brief		控制Sim900A关闭TCP通讯
*@param		None
*@retval	None
*@par
*****************************************************************************
*/
void Sim900A_TCP_Close(void)
{
	uint8_t Transmit_Close=0x1B;
	Sim900A_Send_cmd(&Transmit_Close,"OK",100);
	Sim900A_Send_cmd(&Transmit_Close,"OK",100);
	Sim900A_Send_cmd("AT+CIPCLOSE=1\r","CLOSE OK",100);	//关闭连接
	Sim900A_Send_cmd("AT+CIPSHUT\r","SHUT OK",100);		//关闭移动场景 	
}

/*
*****************************************************************************
*@brief		上电时，串口同步函数
*@param		None
*@retval	None
*@par
*****************************************************************************
*/
static void UsartSync_PowerON(void)
{
	uint8_t FirstTransmit_PowerON[]="PowerON\r";
	uint8_t Transmit_Close=0x1B;
	Sim900A_Send_cmd(FirstTransmit_PowerON,"OK",100);
	Sim900A_Send_cmd(&Transmit_Close,"OK",100);	
}

/*
*****************************************************************************
*@brief		开机时初始化Sim900A的TCP通讯
*@param		None
*@retval	0:初始化失败  1：注册成功  2:注册失败，但连接成功
*@par
*****************************************************************************
*/
uint8_t Sim900A_TCP_Init(void)
{
	uint8_t Transmit_Enter=0x1A;
	Sim900A_TCP_Close();
	Sim900A_Send_cmd(AT_MobileStationCategory,"OK",100);	
	Sim900A_Send_cmd(AT_ConnectionMethod,"OK",100);
	Sim900A_Send_cmd(AT_AccessPoint,"OK",100);
	Sim900A_Send_cmd(AT_AttachedToGPRS,"OK",100);
	Sim900A_Send_cmd(AT_TheMachine_Port,"OK",100);
	if(Sim900A_Send_cmd(AT_SeverIP_And_Port,"CONNECT OK",100)){
		Sim900A_Send_cmd(AT_Send_cmd,"OK",100);
		Sim900A_Send_cmd(AT_TestText,"OK",100);	
		if(Sim900A_Send_cmd(&Transmit_Enter,"successful",500)){
			return 1;
		}
		else return 2;
	}
	else{
		osDelay(8000);
		Sim900A_Send_cmd(AT_Send_cmd,">",100);
		Sim900A_Send_cmd(AT_TestText,"OK",100);	
		if(Sim900A_Send_cmd(&Transmit_Enter,"successful",500)){
			return 1;
		}
		else return 2;
	}
	return 0;
}

/*
*****************************************************************************
*@brief		取值函数，搬运GPRS接收的数据
*@param		GPRS_Receive_Data_n name_num：索取数据对应的枚举变量@GPRS_Receive_Data_n
*@param		void*extern_data：存放传递出来的数据
*@retval	None
*@par
*****************************************************************************
*/
void Get_GPRS_Receive_Data(GPRS_Receive_Data_n name_num,void*extern_data)//*extern_data用来存放get到的值
{
	if(name_num==OutPaper_Cmd_n){
		*(uint8_t*)extern_data=GPRS_Receive_Data.OutPaper_Cmd;
		GPRS_Receive_Data.OutPaper_Cmd=0x00;
	} 
	else if(name_num==OutPaper_SetModeOfLength_Cmd_n){
		*(uint8_t*)extern_data=GPRS_Receive_Data.OutPaper_SetModeOfLength_Cmd;
		GPRS_Receive_Data.OutPaper_SetModeOfLength_Cmd=0x00;
	}	
	else if(name_num==OutPaper_SetLength_Data_n){
		*(uint8_t*)extern_data=GPRS_Receive_Data.OutPaper_SetLength_Data;
		GPRS_Receive_Data.OutPaper_SetLength_Data=0x00;
	}	
}

/*
*****************************************************************************
*@brief		修改GPRS接受的数据
*@param		GPRS_Receive_Data_n name_num：修改数据对应的枚举变量@GPRS_Receive_Data_n
*@param		void* extern_data：传递修改的目标值
*@retval	None
*@par
*****************************************************************************
*/
void Set_GPRS_Receive_Data(GPRS_Receive_Data_n name_num,void*extern_data)//*extern_data用来存放get到的值
{
	if(name_num==OutPaper_Cmd_n){
		GPRS_Receive_Data.OutPaper_Cmd=*(uint8_t*)extern_data;
	} 
	else if(name_num==OutPaper_SetModeOfLength_Cmd_n){
		GPRS_Receive_Data.OutPaper_SetModeOfLength_Cmd=*(uint8_t*)extern_data;
	}	
	else if(name_num==OutPaper_SetLength_Data_n){
		GPRS_Receive_Data.OutPaper_SetLength_Data=*(uint8_t*)extern_data;
	}	
}

