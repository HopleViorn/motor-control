// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:44 $
//###########################################################################
//
// FILE:   DSP2833x_Spi.c
//
// TITLE:  DSP2833x SPI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitSPI: 
//---------------------------------------------------------------------------
// This function initializes the SPI(s) to a known state.
//
void InitSpi(void)
{
	SpiaRegs.SPICCR.bit.SPICHAR = 7;	//8-bit char bits
	SpiaRegs.SPICCR.bit.SPILBK = 0;
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
	SpiaRegs.SPICCR.bit.SPISWRESET = 0;

	SpiaRegs.SPICTL.bit.SPIINTENA = 0;	//Disable SPI INT
	SpiaRegs.SPICTL.bit.TALK = 1;	//Enable TX
	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;	//Mater
	SpiaRegs.SPICTL.bit.CLK_PHASE = 1;
	SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;

	SpiaRegs.SPIBRR = 4;	// SPI Baud Rate = LSPCLK/(4+1)=37.5MHz/5=7.5MHz
	SpiaRegs.SPICCR.bit.SPISWRESET = 1;		// Relinquish SPI from Reset
    SpiaRegs.SPIPRI.bit.FREE = 1;	// Set so breakpoints don't disturb xmission
}

//---------------------------------------------------------------------------
//         [READ]
//  CS   -|___________________________|-
// SIMO  --<00000011b><A15-A0>----------
// SOMI  ---------------------<D15-D0>--
//---------------------------------------------------------------------------
void SPI_ReadFRAM(Uint16 address,Uint16 *data,Uint16 quantity)
{	Uint16 rxdata,i;
    GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;  // CS=0

    SpiaRegs.SPITXBUF = 0x0300;
	while (!SpiaRegs.SPISTS.bit.INT_FLAG) {}
    rxdata = SpiaRegs.SPIRXBUF;

	SpiaRegs.SPICCR.all = 0x000F;          // Reset on, CPOL=0, 16-bit char bits  
	SpiaRegs.SPICCR.all = 0x008F;	       // Relinquish SPI from Reset 

    SpiaRegs.SPITXBUF = address;
	while (!SpiaRegs.SPISTS.bit.INT_FLAG) {}
    rxdata = SpiaRegs.SPIRXBUF;

    for (i=0;i<quantity;i++)
	{
	    SpiaRegs.SPITXBUF = 0x0000;
	    while (!SpiaRegs.SPISTS.bit.INT_FLAG) {}
	    rxdata = SpiaRegs.SPIRXBUF;
		*data++ = rxdata;
    }
	SpiaRegs.SPICCR.all = 0x0007;	      // Reset on, CPOL=0, 8-bit char bits  
	SpiaRegs.SPICCR.all = 0x0087;	      // Relinquish SPI from Reset 

    GpioDataRegs.GPBSET.bit.GPIO57 = 1;   // CS=1
}

//---------------------------------------------------------------------------
//         [WREN]         [WRITE]
//  CS   -|___________|--|___________________________|-
// SIMO  --<00000110b>----<00000010b><A15-A0><D15-D0>--
// SOMI  ----------------------------------------------
//---------------------------------------------------------------------------
void SPI_WriteFRAM(Uint16 address,Uint16 *data,Uint16 quantity)	
{   Uint16 rxdata,i;
    GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;  // CS=0

    SpiaRegs.SPITXBUF = 0x0600;
	while (!SpiaRegs.SPISTS.bit.INT_FLAG) {}
    rxdata = SpiaRegs.SPIRXBUF;

    GpioDataRegs.GPBSET.bit.GPIO57 = 1;    // CS=1

    GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;  // CS=0

    SpiaRegs.SPITXBUF = 0x0200;
	while (!SpiaRegs.SPISTS.bit.INT_FLAG) {}
    rxdata = SpiaRegs.SPIRXBUF;

	SpiaRegs.SPICCR.all = 0x000F;          // Reset on, CPOL=0, 16-bit char bits  
	SpiaRegs.SPICCR.all = 0x008F;	       // Relinquish SPI from Reset 

    SpiaRegs.SPITXBUF = address*2;
	while (!SpiaRegs.SPISTS.bit.INT_FLAG) {}
    rxdata = SpiaRegs.SPIRXBUF;

    for (i=0;i<quantity;i++)
    {
	    SpiaRegs.SPITXBUF = *data++;
		while (!SpiaRegs.SPISTS.bit.INT_FLAG) {}
	    rxdata = SpiaRegs.SPIRXBUF;
    }

	SpiaRegs.SPICCR.all = 0x0007;	      // Reset on, CPOL=0, 8-bit char bits  
	SpiaRegs.SPICCR.all = 0x0087;	      // Relinquish SPI from Reset 

    GpioDataRegs.GPBSET.bit.GPIO57 = 1;   // CS=1
	rxdata = rxdata;
}


//===========================================================================
// End of file.
//===========================================================================
