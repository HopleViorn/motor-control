

#ifndef SPI4_H
#define SPI4_H

//#include "stm32f10x_lib.h"
//#include "DSP2833x_Spi.h"

//#define FSX   GpioDataRegs.GPADAT.bit.GPIO27

#define WatchLine   GpioDataRegs.GPADAT.bit.GPIO16
  void Ecoder5Prog(void );
int32 PIDencodr(int16  error  );

int32 AvrspdPid(int16 DeltaSpeed);

void mcbsp_xmit(Uint16 a ); //���ͺ���
Uint16 mcbsp_rmit(void);//���ܺ���

void mcbspb_xmit(Uint16 a ); //���ͺ���
Uint16 mcbspb_rmit(void);//���ܺ���


interrupt void Mcbspb_TxINTB_ISR(void);
interrupt void Mcbspb_RxINTB_ISR(void);


#endif  // end of DSP2833x_SPI_H definition
