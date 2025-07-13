
#ifndef AD2S1210_H
#define AD2S1210_H



#define SCLK_H  GpioDataRegs.GPBDAT.bit.GPIO49=1
#define SCLK_L  GpioDataRegs.GPBDAT.bit.GPIO49=0

#define SDI_H   GpioDataRegs.GPBDAT.bit.GPIO48=1
#define SDI_L   GpioDataRegs.GPBDAT.bit.GPIO48=0



#define EXAMPLE_H    GpioDataRegs.GPADAT.bit.GPIO31=1
#define EXAMPLE_L    GpioDataRegs.GPADAT.bit.GPIO31=0

///#define SET_A0() GpioDataRegs.GPBDAT.bit.GPIO58=1
//#define CLR_A0()  GpioDataRegs.GPBDAT.bit.GPIO58=0

//#define SET_A1()  GpioDataRegs.GPBDAT.bit.GPIO53=1
//#define CLR_A1()  GpioDataRegs.GPBDAT.bit.GPIO53=0

#define NCS_H    GpioDataRegs.GPADAT.bit.GPIO30=1    //(WR)
#define NCS_L    GpioDataRegs.GPADAT.bit.GPIO30=0

#define SDO      GpioDataRegs.GPBDAT.bit.GPIO53

/*
#define SET_SDI()	GpioDataRegs.GPBDAT.bit.GPIO48=1
#define CLR_SDI()	GpioDataRegs.GPBDAT.bit.GPIO48=0

#define	SET_SCLK() 	GpioDataRegs.GPBDAT.bit.GPIO49=1
#define	CLR_SCLK()	GpioDataRegs.GPBDAT.bit.GPIO49=0

#define SET_SAMPLE()	GpioDataRegs.GPADAT.bit.GPIO31=1
#define CLR_SAMPLE()	GpioDataRegs.GPADAT.bit.GPIO31=0

///#define SET_A0()	GpioDataRegs.GPBDAT.bit.GPIO58=1
//#define CLR_A0()	GpioDataRegs.GPBDAT.bit.GPIO58=0

//#define SET_A1()	GpioDataRegs.GPBDAT.bit.GPIO53=1
//#define CLR_A1()	GpioDataRegs.GPBDAT.bit.GPIO53=0

#define SET_WR()	GpioDataRegs.GPADAT.bit.GPIO30=1
#define CLR_WR()	GpioDataRegs.GPADAT.bit.GPIO30=0

//#define SET_CS()	GpioDataRegs.GPFDAT.bit.GPIOF12=1
//#define CLR_CS()	GpioDataRegs.GPFDAT.bit.GPIOF12=0

#define POSITION    1
#define VELOCITY    2
#define CONFIG      0

*/

#define POSITIONMSB		0x80
#define POSITIONLSB		0x81
#define VELOCITYMSB		0x82
#define VELOCITYLSB		0x83
#define LOSTTHRES		0x88
#define DOSORTHRES		0x89
#define DOSMISTHRES		0x8A
#define DOSRSTMXTHRES	0x8B
#define DOSRSTMITHRES	0x8C
#define LOTHITHRES		0x8D
#define LOTLOTHRES		0x8E
#define EXFREQUENCY		0x91
#define CONTROL			0x92
#define SOFTRESET		0xF0
#define FAULT			0xFF
#define POS_VEL			0x00


void delay_us(Uint16 n);
void SPI3Init(void);
void NewAD2S1210_WRITE(unsigned char addr,unsigned char data);
unsigned char NewAD2S1210_READ(unsigned char addr);
void A1210_INIT(void);

unsigned char ReadA1210_Fault(void);

Uint16 READ_Position(void);//∂¡»°Œª÷√
























/*

void delay();

void AD2S1210Initiate();
void AD2S1210SelectMode(unsigned char);
void ReadFromAD2S1210(unsigned char,unsigned char,unsigned char*);
void SPI3Write(unsigned char,unsigned char*);
void SPI3Read(unsigned char , unsigned char *);
void SPI3Init(void);
void configtestled(void);
void WriteToAD2S1210(unsigned char address, unsigned char data);

*/








#endif

