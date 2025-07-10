#ifndef __MYIIC24C64_H
#define __MYIIC24C64_H

#include "main.h"

#define at24_TM_DAT_Pin GPIO_PIN_13

#define at24_TM_DAT_GPIO_Port GPIOD

#define at24_TM_CLK_Pin GPIO_PIN_14

#define at24_TM_CLK_GPIO_Port GPIOD

void delay_us(uint16_t numb);


void at24_SDA_IN(void);
									
void at24_SDA_OUT(void);

//IO��������	 
#define at24_IIC_SCL_L    HAL_GPIO_WritePin(at24_TM_CLK_GPIO_Port, at24_TM_CLK_Pin, GPIO_PIN_RESET);
#define at24_IIC_SCL_H		 HAL_GPIO_WritePin(at24_TM_CLK_GPIO_Port, at24_TM_CLK_Pin, GPIO_PIN_SET);									
												
#define at24_IIC_SDA_L  	 HAL_GPIO_WritePin(at24_TM_DAT_GPIO_Port, at24_TM_DAT_Pin, GPIO_PIN_RESET);		
#define at24_IIC_SDA_H  	 HAL_GPIO_WritePin(at24_TM_DAT_GPIO_Port, at24_TM_DAT_Pin, GPIO_PIN_SET);	
												
 
#define at24_READ_SDA   HAL_GPIO_ReadPin(at24_TM_DAT_GPIO_Port, at24_TM_DAT_Pin)  //����SDA 

//IIC���в�������
void at24_IIC_Init(void);                //��ʼ��IIC��IO��				 
void at24_IIC_Start(void);				//����IIC��ʼ�ź�
void at24_IIC_Stop(void);	  			//����IICֹͣ�ź�
void at24_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t  at24_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t  at24_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void at24_IIC_Ack(void);					//IIC����ACK�ź�
void at24_IIC_NAck(void);				//IIC������ACK�ź�


uint8_t  at24_IIC_Read_One_Byte(uint8_t  daddr,uint8_t  addr);
uint8_t at24_IIC_Read_len_Byte(uint8_t daddr,uint8_t addr,uint8_t  len,uint8_t  *data);
void at24_IIC_Write_one_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
void at24_IIC_Write_len_Byte(uint8_t daddr,uint8_t addr,uint8_t  *data,uint8_t  len) ;


uint8_t  at24_AT24C01_Check(void);

#endif
















