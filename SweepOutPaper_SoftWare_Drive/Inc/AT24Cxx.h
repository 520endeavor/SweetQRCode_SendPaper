#ifndef __AT24CXX_H
#define __AT24CXX_H

#include "i2c.h"  

#define AT24C01			127
#define AT24C02			255
#define AT24C04			511
#define AT24C08			1023
#define AT24C16			2047
#define AT24C32			4095
#define AT24C64	  	8191
#define AT24C128		16383
#define AT24C256		32767  

#define AT24Cxx_Address 0xA0

#define EE_TYPE AT24C16
#if EE_TYPE>AT24C16
#define AT24CXX_MEMADD_SIZE I2C_MEMADD_SIZE_16BIT		
#else 
#define AT24CXX_MEMADD_SIZE I2C_MEMADD_SIZE_8BIT
#endif

uint8_t AT24Cxx_ReadOneByte(uint16_t ReadAddr);//指定地址读取一个字节
void AT24Cxx_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);							//指定地址写入一个字节
void AT24Cxx_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len); //指定地址开始写入指定长度的数据
uint32_t AT24Cxx_ReadLenByte(uint16_t ReadAddr,uint8_t Len);										//指定地址开始读取指定长度数据
void AT24Cxx_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);		//从指定地址开始写入指定长度的数据
void AT24Cxx_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);   		//从指定地址开始读出指定长度的数据
uint8_t AT24Cxx_Check(void);//检查器件

#endif


