// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:19 $
//###########################################################################
//
// FILE:   DSP2833x_EPwm.c
//
// TITLE:  DSP2833x ePWM Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define	PWM_PRD	7500	//100us
#define	DB_TIME	540		//3.6us
//---------------------------------------------------------------------------
// InitEPwm: 
//---------------------------------------------------------------------------
// This function initializes the ePWM(s) to a known state.
//
void InitEPwm(void)
{
extern	void Disable_PWM(void);

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;	// Stop all the TB clocks
   EDIS;

// ePWM1
//TB
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;	//Master Mode
   EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
   EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
   EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
   EPwm1Regs.TBPRD = PWM_PRD;
   EPwm1Regs.TBCTR = 0;
//CC
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPA.half.CMPA = 0;
   EPwm1Regs.CMPB = 0;
//AQ
   EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
   EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
   EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;
   EPwm1Regs.AQCTLB.bit.CAD = AQ_SET;
//DB
   EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
   EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;	//ALC
   EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
   EPwm1Regs.DBFED = DB_TIME;
   EPwm1Regs.DBRED = DB_TIME;
//PC
   EPwm1Regs.PCCTL.bit.CHPEN = CHP_DISABLE;
//TZ
   EALLOW;
   EPwm1Regs.TZCTL.bit.TZA = TZ_HIZ;	//High impedance
   EPwm1Regs.TZCTL.bit.TZB = TZ_HIZ;	//High impedance
   EDIS;
//ET
   EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;
//   EPwm1Regs.ETSEL.bit.INTEN = 1;
   EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;
   EPwm1Regs.ETSEL.bit.SOCAEN = 1;
//   EPwm1Regs.ETSEL.bit.SOCBSEL = ET_CTR_ZERO;
//   EPwm1Regs.ETSEL.bit.SOCBEN = 1;
   EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;
   EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;
//   EPwm1Regs.ETPS.bit.SOCBPRD = ET_1ST;

// ePWM2
//TB
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
   EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;	//Slave Mode
   EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;
   EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
   EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
   EPwm2Regs.TBPRD = PWM_PRD;
   EPwm2Regs.TBPHS.half.TBPHS = 0;
   EPwm2Regs.TBCTR = 0;
//CC
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
   EPwm2Regs.CMPA.half.CMPA = 0;
   EPwm2Regs.CMPB = 0;
//AQ
   EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;
   EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
   EPwm2Regs.AQCTLB.bit.CAU = AQ_CLEAR;
   EPwm2Regs.AQCTLB.bit.CAD = AQ_SET;
//DB
   EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
   EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;	//ALC
   EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
   EPwm2Regs.DBFED = DB_TIME;
   EPwm2Regs.DBRED = DB_TIME;
//PC
   EPwm2Regs.PCCTL.bit.CHPEN = CHP_DISABLE;
//TZ
   EALLOW;
   EPwm2Regs.TZCTL.bit.TZA = TZ_HIZ;	//High impedance
   EPwm2Regs.TZCTL.bit.TZB = TZ_HIZ;	//High impedance
   EDIS;

// ePWM3
//TB
   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
   EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;	//Slave Mode
   EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;
   EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
   EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
   EPwm3Regs.TBPRD = PWM_PRD;
   EPwm3Regs.TBPHS.half.TBPHS = 0;
   EPwm3Regs.TBCTR = 0;
//CC
   EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
   EPwm3Regs.CMPA.half.CMPA = 0;
   EPwm3Regs.CMPB = 0;
//AQ
   EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;
   EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;
   EPwm3Regs.AQCTLB.bit.CAU = AQ_CLEAR;
   EPwm3Regs.AQCTLB.bit.CAD = AQ_SET;
//DB
   EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
   EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;	//ALC
   EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
   EPwm3Regs.DBFED = DB_TIME;
   EPwm3Regs.DBRED = DB_TIME;
//PC
   EPwm3Regs.PCCTL.bit.CHPEN = CHP_DISABLE;
//TZ
   EALLOW;
   EPwm3Regs.TZCTL.bit.TZA = TZ_HIZ;	//High impedance
   EPwm3Regs.TZCTL.bit.TZB = TZ_HIZ;	//High impedance
   EDIS;

   EPwm1Regs.ETCLR.bit.SOCA = 1;
   EPwm1Regs.ETCLR.bit.SOCB = 1;
   EPwm1Regs.ETCLR.bit.INT = 1;

   EPwm1Regs.ETSEL.bit.INTEN = 1;	//Enable ePWM1_INT

   Disable_PWM();		//初始上电封锁PWM输出


}
//===========================================================================
// End of file.
//===========================================================================
