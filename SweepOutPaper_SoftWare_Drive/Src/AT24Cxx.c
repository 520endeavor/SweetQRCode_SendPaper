#include "AT24Cxx.h" 

/*
*****************************************************************************
*@brief		在AT24Cxx指定地址读出一个数据
*@param		ReadAddr:开始读数的地址  		
*@retval	返回值  :读到的数据
*@par
*****************************************************************************
*/
uint8_t AT24Cxx_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;	
	if(EE_TYPE>AT24C16){
		HAL_I2C_Mem_Read(&hi2c1, AT24Cxx_Address, ReadAddr, AT24CXX_MEMADD_SIZE, &temp, 1, 100);
//		HAL_I2C_Mem_Read_DMA(&hi2c1, AT24Cxx_Address, ReadAddr, AT24CXX_MEMADD_SIZE, &temp, 1);
//		HAL_I2C_Mem_Read_IT(&hi2c1, AT24Cxx_Address, ReadAddr, AT24CXX_MEMADD_SIZE, &temp, 1);
	}
	else {		
		HAL_I2C_Mem_Read(&hi2c1, (AT24Cxx_Address+((ReadAddr/256)<<1)), (ReadAddr%256), AT24CXX_MEMADD_SIZE, &temp, 1, 100);
//		HAL_I2C_Mem_Read_DMA(&hi2c1, (AT24Cxx_Address+((ReadAddr/256)<<1)), (ReadAddr%256), AT24CXX_MEMADD_SIZE, &temp, 1);
//		HAL_I2C_Mem_Read_IT(&hi2c1, (AT24Cxx_Address+((ReadAddr/256)<<1)), (ReadAddr%256), AT24CXX_MEMADD_SIZE, &temp, 1);
	}
	return temp;
}

/*
*****************************************************************************
*@brief		在AT24Cxx指定地址写入一个字节数据
*@param		WriteAddr:写入数据的目的地址 
*@param		DataToWrite:要写入的数据
*@retval	None
*@par
*****************************************************************************
*/
void AT24Cxx_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{			
	if(EE_TYPE>AT24C16){
		HAL_I2C_Mem_Write(&hi2c1, AT24Cxx_Address, WriteAddr, AT24CXX_MEMADD_SIZE, &DataToWrite,  1, 100);		
//		HAL_I2C_Mem_Write_DMA(&hi2c1, AT24Cxx_Address, WriteAddr, AT24CXX_MEMADD_SIZE, &DataToWrite,  1);
//		HAL_I2C_Mem_Write_IT(&hi2c1, AT24Cxx_Address, WriteAddr, AT24CXX_MEMADD_SIZE, &DataToWrite,  1);
	}
	else {		
		HAL_I2C_Mem_Write(&hi2c1, (AT24Cxx_Address+((WriteAddr/256)<<1)), (WriteAddr%256), AT24CXX_MEMADD_SIZE, &DataToWrite,  1, 100);
//		HAL_I2C_Mem_Write_DMA(&hi2c1, (AT24Cxx_Address+((WriteAddr/256)<<1)), (WriteAddr%256), AT24CXX_MEMADD_SIZE, &DataToWrite,  1);
//		HAL_I2C_Mem_Write_IT(&hi2c1, (AT24Cxx_Address+((WriteAddr/256)<<1)), (WriteAddr%256), AT24CXX_MEMADD_SIZE, &DataToWrite,  1);
	}	
//	HAL_Delay(2);		//AT24C02需要延时，FRAM16不需要延时
}

/*
*****************************************************************************
*@brief		在AT24Cxx里面的指定地址开始写入长度为Len的数据,小端模式存储数据
					该函数用于写入16bit或者32bit的数据.
*@param		WriteAddr:开始写入的地址  
*@param		DataToWrite:数据数组首地址
*@param		Len:要写入数据的长度2,4
*@retval	None
*@par
*****************************************************************************
*/
void AT24Cxx_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{  	
	uint8_t t;
	for(t=0;t<Len;t++)
	{
		AT24Cxx_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/*
*****************************************************************************
*@brief		在AT24Cxx里面的指定地址开始读出长度为Len的数据,小端模式存储数据
					该函数用于读出16bit或者32bit的数据，先读大地址的高字节左移
*@param		ReadAddr:开始读出的地址 
*@param		DataToWrite:数据数组首地址
*@param		Len:要读出数据的长度2,4
*@retval	返回值：读出的数据
*@par
*****************************************************************************
*/
uint32_t AT24Cxx_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{  	
	uint8_t t;
	uint32_t temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24Cxx_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/*
*****************************************************************************
*@brief		检查AT24Cxx是否正常，这里用了AT24Cxx的最后一个地址(255)来存储标志字
					如果用其AT24Cxx系列,这个地址要修改
*@retval	返回值：1、检测失败，0、检测成功
*@par
*****************************************************************************
*/
uint8_t AT24Cxx_Check(void)
{
	uint8_t temp;
	temp=AT24Cxx_ReadOneByte(EE_TYPE);//避免每次开机都写AT24Cxx			   
	if(temp==0X55){
		return 0;	
	}		
	else			//排除第一次初始化的情况
	{
		AT24Cxx_WriteOneByte(EE_TYPE,0X55);
		temp=AT24Cxx_ReadOneByte(EE_TYPE);	  
		if(temp==0X55){
			return 0;
		}
	}
	return 1;											  
}

/*
*****************************************************************************
*@brief		在AT24CXX里面的指定地址开始读出指定个数的数据,连续读数
*@param		ReadAddr:开始读出的地址 对24c02为0~255
*@param		pBuffer:数据数组首地址
*@param		NumToRead:要读出数据的个数
*@retval	None
*@par
*****************************************************************************
*/
void AT24Cxx_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24Cxx_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  

/*
*****************************************************************************
*@brief		在AT24Cxx里面的指定地址开始写入指定个数的数据,连续写入
*@param		WriteAddr:开始写入的地址 对24c02为0~255
*@param		pBuffer:数据数组首地址
*@param		NumToWrite:要写入数据的个数
*@retval	None
*@par
*****************************************************************************
*/
void AT24Cxx_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		AT24Cxx_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








