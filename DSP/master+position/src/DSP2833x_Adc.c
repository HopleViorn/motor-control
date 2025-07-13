// TI File $Revision: /main/4 $
// Checkin $Date: July 30, 2007   14:15:53 $
//###########################################################################
//
// FILE:	DSP2833x_Adc.c
//
// TITLE:	DSP2833x ADC Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "EDBclm.h"			// define and function declare
#include "globalvar.h"

#define ADC_usDELAY  5000L
//---------------------------------------------------------------------------
// InitAdc:
//---------------------------------------------------------------------------
// This function initializes ADC to a known state.
//
void InitAdc(void)
{
    extern void DSP28x_usDelay(Uint32 Count);


    // *IMPORTANT*
	// The ADC_cal function, which  copies the ADC calibration values from TI reserved
	// OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
	// Boot ROM. If the boot ROM code is bypassed during the debug process, the
	// following function MUST be called for the ADC to function according
	// to specification. The clocks to the ADC MUST be enabled before calling this
	// function.
	// See the device data manual and/or the ADC Reference
	// Manual for more information.

	    EALLOW;
		SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
		ADC_cal();
		EDIS;


    // To powerup the ADC the ADCENCLK bit should be set first to enable
    // clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
    // Before the first conversion is performed a 5ms delay must be observed
	// after power up to give all analog circuits time to power up and settle

    // Please note that for the delay function below to operate correctly the
	// CPU_CLOCK_SPEED define statement in the DSP2833x_Examples.h file must
	// contain the correct CPU clock period in nanoseconds.

    AdcRegs.ADCTRL3.all = 0x00E0;  // Power up bandgap/reference/ADC circuits
    DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels

	AdcRegs.ADCTRL1.bit.CONT_RUN = 0;	//Stop,Run mode
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;	//Cascaded mode
	AdcRegs.ADCTRL1.bit.ACQ_PS = 6;		//
	AdcRegs.ADCTRL1.bit.CPS = 1;		//12.5MHz
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 =1;	//Allow SEQ/SEQ1 to be started by ePWMx SOCA trigger
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 3;	// 150M/(2*3) = 25MHz ADCLK
	AdcRegs.ADCTRL3.bit.SMODE_SEL = 1;	//Simultaneous sampling mode
	AdcRegs.ADCREFSEL.bit.REF_SEL = 1;	//Extern 2.048v Reference
	AdcRegs.ADCMAXCONV.all = 7;	//8 Channels
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x00;	//Iu(Result0)	    Iv(Result1)
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x01;	//Iu(Result2)	    Iv(Result3)
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x02;	//Iu(Result4)	    Iv(Result5)
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x03;	//Iu(Result6)	    Iv(Result7)
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x00;	//Tref(Result8)	    Vref(Result9)
	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x01;	//Tref(Result10)	Vref(Result11)
	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x06;	//Tref(Result12)	Vref(Result13)
	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x07;	//Tref(Result14)	Vref(Result15)

	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;	//Reset SEQ/SEQ1
	AdcRegs.ADCTRL2.bit.RST_SEQ2 = 1;	//Reset SEQ/SEQ2
	DELAY_US(ADC_usDELAY);
}
//===========================================================================
// End of file.
//===========================================================================
