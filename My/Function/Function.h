#ifndef __function_H
#define __function_H

#include "main.h"


#define LiJuL Pc485RtuReg[22]
#define LiJuR Pc485RtuReg[23]


uint8_t CheckBeforeRun(void);

uint8_t InitAngIfRight(uint8_t MotorNum);

void StopMoto(void) ;
int16_t DoFingInitAngVal(uint8_t MotorNum);
void ClearALMcom(void);
void ResoverBit_Line(uint8_t BitNum);
void ResetDSP(void);
int16_t CheckZDerror(void);
void AllZdFind(void);
void armReset(void);
void SaftyCheck(void);
void FindSaveInitAngle(void);

void HisSyncAction(void);

void readDSPerrorCode(void);

void BeginSystemSyncProcess(void);
uint16_t CalculatSpeed(uint16_t LiJu);
void init_PA(void);
void SelfCheck(void);
void DisError(uint8_t A,uint16_t Code);
void BengParaCheck(uint8_t BengType );
uint8_t WriteBengType(uint8_t Index);
#endif