#ifndef __pid_H
#define __pid_H

#include "main.h"

int32_t PIDencodr(int16_t  error);
float ZL_PIDPower(int32_t  SetPower,int32_t FactPower);

float PID_WZ(int32_t  SetPower,int32_t FactPower);
#endif