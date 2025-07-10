#ifndef __TM6510_H
#define __TM6510_H

#include "myiic.h"
#define u8 uint8_t


void TM1650_Init();
void TM1650_Set(u8 addr, u8 dt);
u8 TM1650_Key();
void TM1650_Key_Demo2(void);

void TM1650_Set_Value(u8 addr, u8 value);
void DisRun(void);
void DisSpeed(uint16_t speed);


#endif