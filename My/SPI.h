#ifndef __SPI_H
#define __SPI_H

#include "main.h"
#define SPI_CS_1  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,1);
#define SPI_CS_0  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,0);


void MX_SPI1_Init(void);
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);
void Send_Iab(void);

#endif
