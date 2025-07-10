#ifndef __Masterrtu_H
#define __Masterrtu_H	
#include "main.h"

#define C485L HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);
#define C485H HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_SET);

#define Pc485L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
#define Pc485H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);


void RtuProcessMaster(void);
void setSendCRC(uint8_t *buffer,uint8_t Length);
void setRceiveCRC(uint8_t *Pdata,char Length);
void RtuProcess(void);

uint16_t FindInitAng(uint8_t MotoNob);
uint16_t AdjSpeed(uint8_t MotoNb,uint16_t ComSpeed);
int16_t ReadSpeed(uint8_t MotoNb);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
uint16_t crc16bitbybit(uint8_t *ptr, uint16_t len);
uint8_t CheckFindOver(uint8_t MotoNob);
//----------------------------------------------------------------------------
uint16_t ReadDsp1RegITReiver(uint8_t MotoNb,uint16_t regaddr);
uint16_t ReadDsp1Reg(uint8_t MotoNb,uint16_t regaddr) ;
uint8_t SendDSPCommand(uint8_t MotoNb,uint16_t regaddr,uint16_t FunData);

void AdjAddTime(uint8_t MotoNb,uint32_t time);
void AdjDecTime(uint8_t MotoNb,uint32_t time);


#endif
