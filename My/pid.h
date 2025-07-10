#ifndef __pid_H
#define __pid_H

#include "main.h"

int32_t PIDencodr(int16_t  error);
int32_t PIDPowerSpd(int32_t  UserSpd,int32_t NowSpd);
#endif