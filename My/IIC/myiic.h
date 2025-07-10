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

//IO��������	 
#define IIC_SCL_L    HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_RESET);
#define IIC_SCL_H		 HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);									
												
#define IIC_SDA_L  	 HAL_GPIO_WritePin(TM_DAT_GPIO_Port, TM_DAT_Pin, GPIO_PIN_RESET);		
#define IIC_SDA_H  	 HAL_GPIO_WritePin(TM_DAT_GPIO_Port, TM_DAT_Pin, GPIO_PIN_SET);	
												
 
#define READ_SDA   HAL_GPIO_ReadPin(TM_DAT_GPIO_Port, TM_DAT_Pin)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t  IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t  IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�


uint8_t  IIC_Read_One_Byte(uint8_t  daddr,uint8_t  addr);
uint8_t IIC_Read_len_Byte(uint8_t daddr,uint8_t addr,uint8_t  len,uint8_t  *data);
void IIC_Write_one_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
void IIC_Write_len_Byte(uint8_t daddr,uint8_t addr,uint8_t  *data,uint8_t  len) ;


uint8_t  AT24C01_Check(void);

#endif
















