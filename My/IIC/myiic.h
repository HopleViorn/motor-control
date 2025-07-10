#ifndef __MYIIC_H
#define __MYIIC_H

#include "main.h"

#define TM_DAT_Pin GPIO_PIN_12

#define TM_DAT_GPIO_Port GPIOE

#define TM_CLK_Pin GPIO_PIN_13

#define TM_CLK_GPIO_Port GPIOE

void delay_us(uint16_t numb);


void SDA_IN(void);
									
void SDA_OUT(void);

//IO操作函数	 
#define IIC_SCL_L    HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_RESET);
#define IIC_SCL_H		 HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);									
												
#define IIC_SDA_L  	 HAL_GPIO_WritePin(TM_DAT_GPIO_Port, TM_DAT_Pin, GPIO_PIN_RESET);		
#define IIC_SDA_H  	 HAL_GPIO_WritePin(TM_DAT_GPIO_Port, TM_DAT_Pin, GPIO_PIN_SET);	
												
 
#define READ_SDA   HAL_GPIO_ReadPin(TM_DAT_GPIO_Port, TM_DAT_Pin)  //输入SDA 

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t  IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t  IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号


uint8_t  IIC_Read_One_Byte(uint8_t  daddr,uint8_t  addr);
uint8_t IIC_Read_len_Byte(uint8_t daddr,uint8_t addr,uint8_t  len,uint8_t  *data);
void IIC_Write_one_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
void IIC_Write_len_Byte(uint8_t daddr,uint8_t addr,uint8_t  *data,uint8_t  len) ;


uint8_t  AT24C01_Check(void);

#endif
















