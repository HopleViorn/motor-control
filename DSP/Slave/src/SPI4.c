#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "SPI4.h"

int16 CommandSpeed;
extern int16 encincr;
//#include "globalvar.h"
extern Uint16 eaEncoder;
extern int16  standSum;
extern Uint16 xyz;
int16 PositionSpeed;
unsigned char index;
int32 SPulse;
extern  volatile int16 Pferr ;
int32 MasterPulse;
int32 SlavePulse;
extern int16 speed3;
extern int32 Avr5spd;

#define N 100
int32 testA[N];
int32 testB[N];
int32 testD[N];
long long testE[N];
int32 lasterror=0;
int32 istop=0;

int32 IElimit=1000;
int32 syncPidMax=9000;
int32 IError;


Uint16 EncoderKp=1;
int32 EncodrPidIElimit=1000000000;
int32 EncodrPidMax=1000;
int32 EncodrIError;

//extern volatile int32 encoder ;
extern volatile int32 encoder;

extern volatile int32 encoder;
extern int32 EncodrPidOut;
extern int16 TogetherEncodr;
Uint16 Masterencoder;
extern int16 PositionCurrentError;
int32 encodererror[10];
int32 Sumencodererror;
int16 AvrEncoderError;

extern int32 syncOut;
//Uint16 CountSpeedLoop;  //5次速度环（10K)做一次同步
extern int32 SyncSpeed;
extern int16 speed3;

char  SlaveDataerror;


int16 Masterspeed=0;
extern int32 speed;


int32 temp;
void mcbsp_xmit(Uint16 a  /* , int b */)
{

    McbspaRegs.DXR1.all = a;
    while( McbspaRegs.SPCR1.bit.RRDY == 0 ) {}         // Master waits until RX data is ready                      // Read DRR2 first.
    McbspaRegs.DRR1.all = McbspaRegs.DRR1.all;


}
void mcbspb_xmit(Uint16 a  /* , int b */)
{

    McbspbRegs.DXR1.all = a;
    while( McbspbRegs.SPCR1.bit.RRDY == 0 ) {}         // Master waits until RX data is ready                      // Read DRR2 first.
   // McbspbRegs.DRR1.all = McbspbRegs.DRR1.all;

}
Uint16 mcbspb_rmit(void)
{
    Uint16 Temp = 0;
    McbspbRegs.DXR1.all= 0x00;
    while( McbspbRegs.SPCR1.bit.RRDY == 0 ) {}         // Master waits until RX data is ready                      // Read DRR2 first.
    Temp = McbspbRegs.DRR1.all;                      // Then read DRR1 to complete receiving of data
    return(Temp);                                           // Check that correct data is received.
}

Uint16 mcbsp_rmit(void)
{
    Uint16 Temp = 0;
    McbspaRegs.DXR1.all= 0x00;
    while( McbspaRegs.SPCR1.bit.RRDY == 0 ) {}         // Master waits until RX data is ready                      // Read DRR2 first.
    Temp = McbspaRegs.DRR1.all;                      // Then read DRR1 to complete receiving of data
    return(Temp);                                           // Check that correct data is received.
}


Uint16 SPI_Flash_ReadIDb(void)  //璇诲彇 FLASH ID
{
    Uint16 Temp = 0;

  //  DELAY_US(10);
    mcbspb_xmit(0x90);//鍙戦�佽鍙朓D 鍛戒护
    mcbspb_xmit(0x00);
    mcbspb_xmit(0x00);
    mcbspb_xmit(0x00);
    Temp = mcbspb_rmit()&0x00FF;
    Temp = Temp<<8;
    Temp|= mcbspb_rmit()&0x00FF;

    DELAY_US(10);
    return Temp;

}

/////////////中断/////////////////////////////////////
interrupt void Mcbspb_TxINTB_ISR(void)
{
    //McbspbRegs.DXR1.all= RDX1;
    //sdata = (sdata+1)& 0x00FF ;
    // To receive more interrupts from this PIE group, acknowledge this interrupt

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
}

interrupt void Mcbspb_RxINTB_ISR(void)
{
        Uint16 RXtemp;
    //   McbspbRegs.DXR2.all=0x0;

                    CommandSpeed=(int16)McbspbRegs.DRR1.all;  //由arm的高位发送 DSP 1高位接收
                //   speed3=(Masterspeed+Avr5spd%;5536)>>1;
                //     RXtemp=McbspbRegs.DRR1.all;
                //     RXtemp=RXtemp<<8;
                 //    PositionCurrentError=((int16)RXtemp)>>8;
                     PositionCurrentError=((int16)McbspbRegs.DRR2.all);   //arm的低位发送
                  if( PositionCurrentError>200)PositionCurrentError=200;
                  if( PositionCurrentError<-200)PositionCurrentError=-200;
                 // if(speed<4000)PositionCurrentError=0;  //400RMPM
                  //PositionCurrentError=0;
                 // PositionSpeed=PositionCurrentError;
                 // if(xyz==0)  PositionCurrentError=0;   //MODBUS 1062



                 /////////////////////////////////////////////////////////////////////////////

                 /////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
                              //-------------------------计算---------------------------------------------
                                 speed3=(Avr5spd+Masterspeed)>>1;

                                 if((speed3>500)|(speed3<-500)){
                                     W5=Avr5spd;
                                     Wspi=Masterspeed;
                                 }
                                 syncOut= PIDsync(speed3);
                                  WatchLine=1;
#endif
                         ////////////////////////////////////////////////////////////////////////////////////////////




    // To receive more interrupts from this PIE group, acknowledge this interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
}


int32 AvrspdPid(int16 DeltaSpeed)
{
    int16 p=10;
    int16 i=0;
    int32 Current_Error;
    int32 out;
    Current_Error=DeltaSpeed;
    IError=IError+Current_Error;
    if(IError>IElimit)IError=IElimit;
    if(IError<-IElimit)IError=-IElimit;
    out = p * Current_Error      //比例P
          + i * IError;//积分I
    if(out >syncPidMax)out =syncPidMax;
    if(out <-syncPidMax)out =-syncPidMax;
    return out/20;
}

int32 PIDencodr(int16  error)
{
    //EncoderKp
    int32  i=1;
    int32 Current_Error;
    int32 out;
    Current_Error=error;
    //if(Current_Error>500)Current_Error=00;
   // if(Current_Error<-100)Current_Error=-100;
    EncodrIError=EncodrIError+Current_Error;
    if(EncodrIError>EncodrPidIElimit)EncodrIError=EncodrPidIElimit;
    if(EncodrIError<-EncodrPidIElimit)EncodrIError=-EncodrPidIElimit;
    out= 1* Current_Error      //比例P
          + 0 * EncodrIError/10000;//积分I

//    EncodrIError=EncodrIError%100;



    if(out >EncodrPidMax)out =EncodrPidMax;
    if(out <-EncodrPidMax)out =-EncodrPidMax;

//    if(error - lasterror > 30 || error - lasterror < -30)  out = 0;
//     if(error - lasterror > 30 || error - lasterror < -30)  out = 0;
//    if(abs(error)-lasterror > 20) istop=100;
//    if(abs(error)-lasterror < -20) istop=0;
//    if(istop>0) istop--;
//    lasterror=abs(error);


//    if(istop) out=0;

//    if(error > 30 || error <30) out=out / 4;

//    if(abs(error) > xyz) istop=1;
//    else istop=0;

//    if(istop) out=0;

    out=out/4;
   // if(xyz==0) out=0;

//    testA[currentpos]=++lasterror;
//    testB[currentpos]=Masterencoder;
//
//    testD[currentpos]=error;
//    testE[currentpos]=(long long) Masterencoder*10000000+encoder*1000+error;


//    out=0;
    return out;      ///3;
}
# if 0
  void Ecoder5Prog(void )
  {

                                     TogetherEncodr=Masterencoder+encoder;
                                      if (TogetherEncodr<8192)TogetherEncodr+=8192;

                                 //   if(index<10)
                                  //       {
                                  //               encodererror[index]=TogetherEncodr;
                                   //              index++;
                                   //      }
                                   //  else
                                  //   {
                                   //              index=0;
                                   //            Sumencodererror=encodererror[0]+encodererror[1]+encodererror[2]+encodererror[3]+encodererror[4]
                                    //                 +  encodererror[5]+encodererror[6]+encodererror[7]+encodererror[8]+encodererror[9];
                                                 AvrEncoderError= TogetherEncodr;//Sumencodererror/10;
                                   //  }
                                             PositionCurrentError=AvrEncoderError-standSum;
                                          ///////////////////////////同步位置补㑽PID
                                             EncodrPidOut=  PIDencodr(PositionCurrentError );
}
#endif
//===========================================================================
// No more.
//===========================================================================
