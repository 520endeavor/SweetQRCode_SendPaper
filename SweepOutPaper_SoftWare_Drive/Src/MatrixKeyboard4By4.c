#include "MatrixKeyboard4By4.h" 

uint8_t MatrixKeyboard_CorrespondingNumber[16]={1,2,3,Password_Error_Num,4,5,6,Password_Error_Num,7,8,9,Password_Error_Num,Password_Error_Num,0,Password_Error_Num,Password_Error_Num};
volatile uint8_t Password_CurrentInput[Password_Length]={0};
volatile uint8_t Password_Input[Password_Length];

osThreadId Password_IdentifyTaskHandle;									//���������������


void Password_IdentifyTask(void const * argument);


void MatrixKeyboard4By4_FREERTOS_Init(void){
	/* definition and creation of Password_IdentifyTask */
	osThreadDef(Password_IdentifyTask, Password_IdentifyTask, osPriorityNormal, 0, 128);
	Password_IdentifyTaskHandle = osThreadCreate(osThread(Password_IdentifyTask), NULL);
		
}

/*
*****************************************************************************
*@brief		ɨ������󰴼�ֵ
*@param		uint8_t mode:0,��֧��������;1,֧��������;
*@param		uint8_t *MatrixKey_Value:�洢4*4�������ֵ0~15
*@retval	����ֵ:1�а�������,0�ް������»��߼�⵽������δ�ɿ�
*@par
*****************************************************************************
*/
static uint8_t MatrixKeyboard4By4_Scan(uint8_t mode,uint8_t *MatrixKey_Value)
{	
	uint8_t Column_Value_Sum[4]={0};	
	static uint8_t key_up[4]={1,1,1,1};				//�������ɿ���־	
	if(mode){
		for (uint8_t i=0;i<4;i++){
			key_up[i]=1;  												//֧������		
		}		
	}	
	for(uint8_t i=0;i<4;i++)
	{
		if(i==0){			
			HAL_GPIO_WritePin(GPIOD, PasswordButton_Output2_Pin|PasswordButton_Output3_Pin|PasswordButton_Output4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, PasswordButton_Output1_Pin, GPIO_PIN_SET);
		}
		if(i==1){			
			HAL_GPIO_WritePin(GPIOD, PasswordButton_Output1_Pin|PasswordButton_Output3_Pin|PasswordButton_Output4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, PasswordButton_Output2_Pin, GPIO_PIN_SET);
		}
		if(i==2){
			HAL_GPIO_WritePin(GPIOD, PasswordButton_Output1_Pin|PasswordButton_Output2_Pin|PasswordButton_Output4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, PasswordButton_Output3_Pin, GPIO_PIN_SET);
		}
		if(i==3){
			HAL_GPIO_WritePin(GPIOD, PasswordButton_Output1_Pin|PasswordButton_Output2_Pin|PasswordButton_Output3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, PasswordButton_Output4_Pin, GPIO_PIN_SET);
		}			
		Column_Value_Sum[i]=HAL_GPIO_ReadPin(GPIOD,PasswordButton_Input1_Pin)+HAL_GPIO_ReadPin(GPIOD,PasswordButton_Input2_Pin)+HAL_GPIO_ReadPin(GPIOD,PasswordButton_Input3_Pin)+HAL_GPIO_ReadPin(GPIOD,PasswordButton_Input4_Pin);
		if(key_up[i]&&Column_Value_Sum[i]>0){
			osDelay(10);
			key_up[i]=0;
			if(HAL_GPIO_ReadPin(GPIOD,PasswordButton_Input1_Pin)==1){
				*MatrixKey_Value=0x00+4*i;
				return 1;
			}
			else if(HAL_GPIO_ReadPin(GPIOD,PasswordButton_Input2_Pin)==1){
				*MatrixKey_Value=0x01+4*i;
				return 1;
			}
			else if(HAL_GPIO_ReadPin(GPIOD,PasswordButton_Input3_Pin)==1){
				*MatrixKey_Value=0x02+4*i;
				return 1;
			}
			else if(HAL_GPIO_ReadPin(GPIOD,PasswordButton_Input4_Pin)==1){
				*MatrixKey_Value=0x03+4*i;
				return 1;
			}
		}
		else if(Column_Value_Sum[i]==0){
			key_up[i]=1; 
		}
		if(i==3){			
			return 0;				
		}
	}					//end of for
}

/*
*****************************************************************************
*@brief		������������������
*@param		uint8_t *MatrixKey_Value:����4*4�������ֵ�����׵�ַ
*@param		uint8_t *Password_Value:�洢�����������
*@retval	None
*@par
*****************************************************************************
*/
void Password_Analysis(uint8_t *MatrixKey_Value,uint8_t *Password_Value)
{
	for(uint8_t i=0;i<Password_Length;i++)
	{
		Password_Value[i]=MatrixKeyboard_CorrespondingNumber[MatrixKey_Value[i]];
	}
}

/* Password_IdentifyTask function */
void Password_IdentifyTask(void const * argument)
{
  /* USER CODE BEGIN Password_IdentifyTask */
  /* Infinite loop */
	uint8_t i=0;
	uint8_t MatrixKey_Value=0;
  for(;;)
  {
		if(MatrixKeyboard4By4_Scan(0,&MatrixKey_Value)){	
			if(MatrixKey_Value==Delet_Button){
				if(i!=0){																								//ɾ����һ������
					i--;
				}
			}
			else if(i<Password_Length){				
				if(MatrixKey_Value==Enter_Button){
					i=0;																									//�����������
				}
				else if(MatrixKey_Value!=Password_Keyboard_A&&MatrixKey_Value!=Password_Keyboard_B&&MatrixKey_Value!=Password_Keyboard_C&&MatrixKey_Value!=Password_Keyboard_D){
					Password_CurrentInput[i]=MatrixKey_Value;							//�ų������ϵ�ABCD�������ư����������
					i++;																			
				}				
			}				
			else if(i==Password_Length){
				if(MatrixKey_Value==Enter_Button){
					Password_Analysis((uint8_t *)Password_CurrentInput,(uint8_t *)Password_Input);		//�ɹ���������		
					xTaskNotifyGive(Get_Sim900A_GPRS_Initiative_TransmitTaskHandle());				
				}
				i=0;																										//�����������				
			}			
		}
    osDelay(20);
  }
  /* USER CODE END Password_IdentifyTask */
}

/*
*****************************************************************************
*@brief		��ȡ��������ָ��
*@param		None
*@retval	None
*@par
*****************************************************************************
*/
uint8_t* Get_PasswordInfo_Pointer(void)//*extern_data�������get����ֵ
{
	return 	Password_Input;
}








