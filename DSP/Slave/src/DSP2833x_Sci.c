// TI File $Revision: /main/2 $
// Checkin $Date: March 1, 2007   16:06:07 $
//###########################################################################
//
// FILE:	DSP2833x_Sci.c
//
// TITLE:	DSP2833x SCI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitSci: 
//---------------------------------------------------------------------------
// This function initializes the SCI(s) to a known state.
//
void InitSci(void)
{
	ScicRegs.SCICTL1.bit.RXENA = 1;	//Transmitter enable
	ScicRegs.SCICTL1.bit.TXENA = 1;	//Receiver enable
	ScicRegs.SCICTL1.bit.TXWAKE = 0;
	ScicRegs.SCICTL1.bit.SLEEP = 0;
	ScicRegs.SCICTL1.bit.RXERRINTENA = 0;
	ScicRegs.SCICTL1.bit.SWRESET = 0; //SoftWare Reset

	ScicRegs.SCICTL2.bit.TXINTENA = 0;	//Disable TXRDY interrupt
	ScicRegs.SCICTL2.bit.RXBKINTENA = 1;//Enalbe RXRDY/BRKDT interrupt

	ScicRegs.SCICCR.bit.LOOPBKENA = 0;	//Loop Back Test Mode Disabled
	ScicRegs.SCICCR.bit.ADDRIDLE_MODE = 0;//Compatible with RS-232 type Communications

}


//===========================================================================
// End of file.
//===========================================================================
