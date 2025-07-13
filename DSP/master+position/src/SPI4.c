#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "SPI4.h"

#define standSum 12540



int16 SlaveIdr=0;
int16 SlaveIqr=0;
char SlaveDataerror;
int32 Slavespeed;
Uint32 temp;
extern int32 speed;
extern int16 speed3;
extern int32 Avr100spd;

int32 IElimit=1000;
int32 syncPidMax=300;
int32 IError;

unsigned char SendEnconder=0;
int32 EncodrPidIElimit=50;
int32 EncodrPidMax=3000;
int32 EncodrIError;
Uint16 Slaveencoder=0;
//extern volatile int32 encoder ;
extern volatile int32 encoder;





void mcbsp_xmit(Uint16 a  /* , int b */)
{

    McbspaRegs.DXR1.all = a;
    while( McbspaRegs.SPCR1.bit.RRDY == 0 ) {}         // Master waits until RX data is ready                      // Read DRR2 first.
    McbspaRegs.DRR1.all = McbspaRegs.DRR1.all;


}
void mcbspb_xmit32(Uint16 a,Uint16 b)
{

    McbspbRegs.DXR1.all = a;  //32位发送时B在前。a在后
    McbspbRegs.DXR2.all = b;
    //while( McbspbRegs.SPCR1.bit.RRDY == 0 ) {}         // Master waits until RX data is ready                      // Read DRR2 first.
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
  //  FSX = 0;       // open CS
  //  DELAY_US(10);
    mcbspb_xmit(0x90);//鍙戦�佽鍙朓D 鍛戒护
    mcbspb_xmit(0x00);
    mcbspb_xmit(0x00);
    mcbspb_xmit(0x00);
    Temp = mcbspb_rmit()&0x00FF;
    Temp = Temp<<8;
    Temp|= mcbspb_rmit()&0x00FF;
    //FSX = 1;        // close CS
    DELAY_US(10);
    return Temp;

}


interrupt void Mcbspb_TxINTB_ISR(void)    //速度发送中断中发送encoder  想办法避免进入死循环发送
{


    ////////////////////发送主机encode///////////////////////////
    //if(SendEnconder)
    //{
   //     McbspbRegs.DXR1.all=encoder;         //发送本机 Slave encodr
  //     McbspbRegs.DXR2.all=0xAAAA;                    //高位发送encodr标志
 //       SendEnconder=0;           //发送一次后不能再发送。等speed发送后置1再发送）
//    }

    /////////////////////////////////////////////
    //McbspbRegs.DXR1.all= RDX1;
    //sdata = (sdata+1)& 0x00FF ;
    // To receive more interrupts from this PIE group, acknowledge this interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
}

interrupt void Mcbspb_RxINTB_ISR(void)
{
        //    temp=McbspbRegs.DRR2.all;
       //     temp=temp<<16;
                  //   Slavespeed=(int16)(McbspbRegs.DRR1.all);

                   //  speed3=(Slavespeed+Avr100spd)>>1;  //第三轴速度两者之和 。同步时速度为0  ;相加/ 2   主速为正，从速为负 均取的后16位。负值需要转换吗



        //    if(temp==0xaaaa)   //encoder
        //      {
                //  Slaveencoder=McbspbRegs.DRR1.all;
                //  McbspbRegs.DXR1.all=encoder;         //发送本机 Slave encodr 位于User_lsr.c enoder得出处
                  //McbspbRegs.DXR2.all=0xAAAA;                    //高位发送encodr标志
           //   }
           //   else      //speed
          //    {


        //      }
      // if((Slavespeed>130000)|(Slavespeed<-130000))//限130000
      // {
               //Slavespeed=speed;  //超限就=self
             //SlaveDataerror++;
     //  }


   // rdata=McbspbRegs.DRR1.all;
   // if (rdata != ( (rdata_point) & 0x00FF) ) error();
   // rdata_point = (rdata_point+1) & 0x00FF;
    // To receive more interrupts from this PIE group, acknowledge this interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
}



int32 AvrspdPid(int16 DeltaSpeed)
{
    int16 p=10;
    int16 i=2;
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
    return out/100;
}

int32 PIDencodr(int16  error  )
{
    int16 p=5;
    int16 i=2;
    int32 Current_Error;
    int32 out;
    Current_Error=error;
    EncodrIError=EncodrIError+Current_Error;
    if(EncodrIError>EncodrPidIElimit)EncodrIError=EncodrPidIElimit;
    if(EncodrIError<-EncodrPidIElimit)EncodrIError=-EncodrPidIElimit;
    out = p * Current_Error      //比例P
          + i * EncodrIError;//积分I
    if(out >EncodrPidMax)out =EncodrPidMax;
    if(out <-EncodrPidMax)out =-EncodrPidMax;
    return out/5;
}


//===========================================================================
// No more.
//===========================================================================
