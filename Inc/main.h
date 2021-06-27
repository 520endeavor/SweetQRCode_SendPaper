/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define OutPaperMot_Pin GPIO_PIN_2
#define OutPaperMot_GPIO_Port GPIOE
#define BackUp_Usart2_TX_Pin GPIO_PIN_2
#define BackUp_Usart2_TX_GPIO_Port GPIOA
#define BackUp_Usart2_RX_Pin GPIO_PIN_3
#define BackUp_Usart2_RX_GPIO_Port GPIOA
#define PaperConsumptionDetectionPhotodiode_ADC_Pin GPIO_PIN_4
#define PaperConsumptionDetectionPhotodiode_ADC_GPIO_Port GPIOA
#define OutPaperSucessfulPhotodiode_ADC_Pin GPIO_PIN_5
#define OutPaperSucessfulPhotodiode_ADC_GPIO_Port GPIOA
#define PaperCuttingMot_Pwm_Tim3CH3_IA_Pin GPIO_PIN_0
#define PaperCuttingMot_Pwm_Tim3CH3_IA_GPIO_Port GPIOB
#define PaperCuttingMot_Pwm_Tim3CH4_IB_Pin GPIO_PIN_1
#define PaperCuttingMot_Pwm_Tim3CH4_IB_GPIO_Port GPIOB
#define PaperCuttingMachinePhotodiode_Input_Pin GPIO_PIN_0
#define PaperCuttingMachinePhotodiode_Input_GPIO_Port GPIOD
#define PasswordButton_Input4_Pin GPIO_PIN_8
#define PasswordButton_Input4_GPIO_Port GPIOD
#define PasswordButton_Input3_Pin GPIO_PIN_9
#define PasswordButton_Input3_GPIO_Port GPIOD
#define PasswordButton_Input2_Pin GPIO_PIN_10
#define PasswordButton_Input2_GPIO_Port GPIOD
#define PasswordButton_Input1_Pin GPIO_PIN_11
#define PasswordButton_Input1_GPIO_Port GPIOD
#define PasswordButton_Output4_Pin GPIO_PIN_12
#define PasswordButton_Output4_GPIO_Port GPIOD
#define PasswordButton_Output3_Pin GPIO_PIN_13
#define PasswordButton_Output3_GPIO_Port GPIOD
#define PasswordButton_Output2_Pin GPIO_PIN_14
#define PasswordButton_Output2_GPIO_Port GPIOD
#define PasswordButton_Output1_Pin GPIO_PIN_15
#define PasswordButton_Output1_GPIO_Port GPIOD
#define SIM900A_Usart1_TX_Pin GPIO_PIN_9
#define SIM900A_Usart1_TX_GPIO_Port GPIOA
#define SIM900A_Usart1_RX_Pin GPIO_PIN_10
#define SIM900A_Usart1_RX_GPIO_Port GPIOA
#define SIM900A_PWRKEY_Pin GPIO_PIN_11
#define SIM900A_PWRKEY_GPIO_Port GPIOC
#define RunningLight_Pin GPIO_PIN_12
#define RunningLight_GPIO_Port GPIOC
#define FourByOneKey4_Pin GPIO_PIN_1
#define FourByOneKey4_GPIO_Port GPIOD
#define FourByOneKey3_Pin GPIO_PIN_2
#define FourByOneKey3_GPIO_Port GPIOD
#define PaperMoveExtent_FourByOneKey2_Pin GPIO_PIN_3
#define PaperMoveExtent_FourByOneKey2_GPIO_Port GPIOD
#define ManualOutPaper_FourByOneKey1_Pin GPIO_PIN_4
#define ManualOutPaper_FourByOneKey1_GPIO_Port GPIOD
#define ManualOutPaperAloneLED_Pin GPIO_PIN_5
#define ManualOutPaperAloneLED_GPIO_Port GPIOD
#define ManualOutPaperAloneKey_Pin GPIO_PIN_6
#define ManualOutPaperAloneKey_GPIO_Port GPIOD
#define SweepingCodeAndOutPaperKey_Pin GPIO_PIN_7
#define SweepingCodeAndOutPaperKey_GPIO_Port GPIOD
#define FRAM16_IIC_SCL_Pin GPIO_PIN_6
#define FRAM16_IIC_SCL_GPIO_Port GPIOB
#define FRAM16_IIC_SDA_Pin GPIO_PIN_7
#define FRAM16_IIC_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
