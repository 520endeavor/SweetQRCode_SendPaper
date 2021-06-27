#ifndef __MATRIXKEYBOARD4BY4_H
#define __MATRIXKEYBOARD4BY4_H

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "gpio.h"
#include "Sim900A_GPRS_Communication.h" 


#define Password_Length							(uint8_t)6
#define	Password_Keyboard_A					(uint8_t)3	
#define	Password_Keyboard_B					(uint8_t)7
#define Password_Keyboard_C					(uint8_t)11
#define Password_Keyboard_D					(uint8_t)15
#define Delet_Button								(uint8_t)12
#define Enter_Button								(uint8_t)14
#define Password_Error_Num					(uint8_t)255


void MatrixKeyboard4By4_FREERTOS_Init(void); 					//矩阵键盘中建立任务的函数

uint8_t* Get_PasswordInfo_Pointer(void);							//获取密码数据指针



#endif

