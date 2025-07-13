// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:25 $
//###########################################################################
//
// FILE:	DSP2833x_Gpio.c
//
// TITLE:	DSP2833x General Purpose I/O Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitGpio: 
//---------------------------------------------------------------------------
// This function initializes the Gpio to a known (default) state.
//
// For more details on configuring GPIO's as peripheral functions,
// refer to the individual peripheral examples and/or GPIO setup example. 
void InitGpio(void)
{
   EALLOW;
   
   // Each GPIO pin can be: 
   // a) a GPIO input/output
   // b) peripheral function 1
   // c) peripheral function 2
   // d) peripheral function 3
   // By default, all are GPIO Inputs

//	Enable pull-up on GPIO0~GPIO5(ePWM1~ePWM6)
   GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;
   GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;
   GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
   GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;
   GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;
   GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;
   

//	ePWM1~ePWM6
   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;		//ePWM1A	H->L
   GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;		//ePWM1B	H->L
   GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;		//ePWM2A	H->L
   GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;		//ePWM2B	H->L
   GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;		//ePWM3A	H->L
   GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;		//ePWM3B	H->L
   


//	SPI
/* Set qualification for selected pins to asynch only */
   GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)

   GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1;		//SPISIMOA
   GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1;		//SPISOMIA
   GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1;		//SPICLKA

   GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;		//SPISTEA
   GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;		//Output(CS)

//	CAN
   GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 3;		//CANTXB
   GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 3;	    //CANRXB

//	SCI
/* Set qualification for selected pins to asynch only */
   GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO62 (SCIRXDC)

   GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;		//SCIRXDC
   GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;		//SCITXDC

   //SCIEN
   GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;		//GPIO61, 485 SCI enable 
   GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;      //Output

//	Extern Data Line(XD0~XD15)
   GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 2;		//XD0
   GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 2;		//XD1
   GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 2;		//XD2
   GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 2;		//XD3
   GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 2;		//XD4
   GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 2;		//XD5
   GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 2;		//XD6
   GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 2;		//XD7
   GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 2;		//XD8
   GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 2;		//XD9
   GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 2;		//XD10
   GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 2;		//XD11
   GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 2;		//XD12
   GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 2;		//XD13
   GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 2;		//XD14
   GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 2;		//XD15

//	Extern Address Line(XA0~XA16)
   GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 2;     //XA0
   GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 2;		//XA1
   GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 2;     //XA2
   GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 2;		//XA3
   GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 2;     //XA4
   GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 2;		//XA5
   GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 2;     //XA6
   GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 2;		//XA7
   GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 2;		//XA8
   GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 2;		//XA9
   GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 2;		//XA10
   GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 2;		//XA11
   GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 2;		//XA12
   GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 2;		//XA13
   GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 2;		//XA14
   GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 2;		//XA15
   GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 2;		//XA16

//	Extern Chip Select && WE
   GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 2;		//XZCS6
   GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 2;		//XZCS7
   GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 2;		//XWE0
   GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 2;		//XZCS0 

//	XINT1/XINT2/XINT3
   GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 12;	//XINT1:GPIO12
   //GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 13;	//XINT2:GPIO13
   //GpioIntRegs.GPIOXNMISEL.bit.GPIOSEL = 16;	//XNMI:GPIO16
   
/*   XIntruptRegs.XINT1CR.bit.ENABLE = 1;
   XIntruptRegs.XINT1CR.bit.POLARITY = 1;	//Rising Egde
   XIntruptRegs.XINT2CR.bit.ENABLE = 1;
   XIntruptRegs.XINT2CR.bit.POLARITY = 1;	//Rising Egde
*/

//	KEY1~KEY4
   GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;		//KEY1
   GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;		//KEY2
   GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;		//KEY3
   GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;		//KEY4

   GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0;		//Input
   GpioCtrlRegs.GPBDIR.bit.GPIO59 = 0;		//Input
   GpioCtrlRegs.GPBDIR.bit.GPIO51 = 0;		//Input
   GpioCtrlRegs.GPBDIR.bit.GPIO50 = 0;		//Input



   // Each input can have different qualification
   // a) input synchronized to SYSCLKOUT
   // b) input qualified by a sampling window
   // c) input sent asynchronously (valid for peripheral inputs only)

   GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 0;	//Synchronize to SYSCLKOUT
   GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 0;	//Synchronize to SYSCLKOUT
   GpioCtrlRegs.GPBQSEL2.bit.GPIO51 = 0;	//Synchronize to SYSCLKOUT
   GpioCtrlRegs.GPBQSEL2.bit.GPIO50 = 0;	//Synchronize to SYSCLKOUT

   EDIS;

}

//===========================================================================
// End of file.
//===========================================================================
