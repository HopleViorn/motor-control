#ifndef __pid_H
#define __pid_H

#include "main.h"

int32_t PIDencodr(int16_t  error);
float ZL_PIDPower(int32_t  SetPower,int32_t FactPower);
float ZL_PIDTorque(int32_t SetTorquePercent, int32_t FactTorquePercent);


float PID_WZ(int32_t  SetPower,int32_t FactPower);
#endif