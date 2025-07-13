#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "AD2S.h"

extern Uint16    ResoverBit;
extern Uint16     ABLine;
int16 AngPosition;
int16 DltaPosition[16];
unsigned char daIndex;
unsigned char Wtemp;
unsigned char reg[10];

void delay_us(Uint16 n)
{
    Uint16  k;
    Uint16 m;
//  for(m=0;m<n;m++)
  //{
            for(k=0;k<1;k++)    //1us
            {
                  m=0;
            }
  //}
}



void SPI3Init(void)
{
    EALLOW;
    //���ÿ���
    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;   //GPBMUX2
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;

    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 0;          //����
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO30 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;

    GpioCtrlRegs.GPBPUD.bit.GPIO53 = 0;   // Enable pull-up on GPIO53 (SDI)  0 �򹤿�����
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;   // Enable pull-up on GPIO48 (SDO)
    GpioCtrlRegs.GPBPUD.bit.GPIO49 = 0;   // Enable pull-up on GPIO49 (SCLK)
    GpioCtrlRegs.GPAPUD.bit.GPIO30 = 0;   // Enable pull-up on GPIO30 (WR)
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;   // Enable pull-up on GPIO31 (SAMPLE)

    EDIS;
}

void NewAD2S1210_WRITE(unsigned char addr,unsigned char data)
{
  unsigned char Wtemp;
  unsigned char s;
    SCLK_H;
    SDI_H;
    NCS_L;
    Wtemp=addr;
    for(s=0;s<8;s++)
    {
      SCLK_H;
      delay_us(1);
      if(Wtemp&0x80){SDI_H;}
      else{SDI_L;}
      delay_us(1);
      SCLK_L;
      delay_us(1);
      Wtemp<<=1;
    }
  SCLK_H;
  delay_us(1);
  NCS_H;
  delay_us(1);
  SDI_H;
  delay_us(3);
  //SCLK_L;
  NCS_L;
  delay_us(1);
  Wtemp=data;

      for(s=0;s<8;s++)
      {
        SCLK_H;
        delay_us(1);
        if(Wtemp&0x80){SDI_H;}
        else{SDI_L;}
        delay_us(1);
        SCLK_L;
        delay_us(1);
        Wtemp<<=1;
      }
  SCLK_H;
  delay_us(1);
  NCS_H;
  delay_us(1);

}

unsigned char NewAD2S1210_READ(unsigned char addr)
{
  unsigned char Wtemp;
  unsigned char Rtemp;
  unsigned char s;
  SCLK_H;
  SDI_H;
  NCS_L;
  Wtemp=addr;
  for(s=0;s<8;s++)
  {
    SCLK_H;
    if(Wtemp&0x80){SDI_H;}
    else{SDI_L;}
    delay_us(1);
    SCLK_L;
    delay_us(1);
    Wtemp<<=1;
  }
  NCS_H;
  SCLK_H;
  SDI_H;
  delay_us(3);//
  //SCLK_L;
  NCS_L;

  for( s=0;s<8;s++)
  {
    SCLK_H;
    delay_us(1);
    SCLK_L;
    Rtemp<<=1;
    if(SDO)
        {
            Rtemp++;
        }
    //  delay_us(1);
  }
  SCLK_H;
  NCS_H;

  return Rtemp;
}
unsigned char NewAD2S1210_READnew(unsigned char addr)
{
  unsigned char Wtemp;
  unsigned char Rtemp;
  unsigned char s;
  SCLK_H;
  SDI_H;
  NCS_L;
  delay_us(1);
  Wtemp=addr;
  for(s=0;s<8;s++)
  {
      delay_us(1);
    SCLK_H;
    delay_us(1);
    if(Wtemp&0x80){SDI_H;}
    else{SDI_L;}
    delay_us(1);
    SCLK_L;
    delay_us(1);
    Wtemp<<=1;
  }
  delay_us(1);
  NCS_H;
  SCLK_H;
  SDI_H;
  delay_us(3);//
  //SCLK_L;
  NCS_L;
  delay_us(1);
  for( s=0;s<8;s++)
  {
    SCLK_H;
    delay_us(1);
    SCLK_L;
    Rtemp<<=1;
    if(SDO)
        {
            Rtemp++;
        }
      delay_us(1);
  }
  SCLK_H;
  delay_us(1);
  NCS_H;

  return Rtemp;
}

unsigned char NewNewAD2S1210_READ(unsigned char addr)
{
  unsigned char Wtemp;
  unsigned char Rtemp;
  unsigned char s;
  SCLK_H;
  SDI_H;
  NCS_L;
  delay_us(1);
  Wtemp=addr;
  for(s=0;s<8;s++)
  {
    SCLK_H;
    delay_us(1);
    if(Wtemp&0x80){SDI_H;}
    else{SDI_L;}
    delay_us(1);
    SCLK_L;
    delay_us(1);
    Wtemp<<=1;
  }
  NCS_H;
  SCLK_H;
  SDI_H;
  delay_us(3);//
  //SCLK_L;
  NCS_L;
  delay_us(1);
  for( s=0;s<8;s++)
  {
    SCLK_H;
    delay_us(1);
    SCLK_L;
    Rtemp<<=1;
    if(SDO)
        {
            Rtemp++;
        }
      delay_us(1);
  }
  SCLK_H;
  delay_us(1);
  NCS_H;

  return Rtemp;
}

Uint16 READ_Position(void)//��ȡλ��
{
    //int16 da,PrePosition;
    Uint16 REPosition;
  EXAMPLE_L;
  delay_us(1);
  EXAMPLE_H;
  AngPosition=NewAD2S1210_READ(0x80);
  AngPosition<<=8;
  delay_us(1);
  AngPosition|=NewAD2S1210_READ(0x81);
    AngPosition=(AngPosition&0xFFFC);
/*
    da=AngPosition-PrePosition;  //

        if(da>32767){
            da=-(65536-da);    //��daΪ��,����С�ķ���ת��
        }
        if(da<-32767)
        {
            da=65536+da;
        }



    DltaPosition[daIndex]=da;

        if(daIndex<9)
    {
        daIndex++;
    }
    else
    {
        daIndex=0;
    }

    PrePosition=AngPosition;
#if 0
                                if(RunFlag)
                                    {
                                            if(speedindex<2000)
                                            {

                                                        SpeedTemp[speedindex]=AngPosition;//elAngle;//mecAngle;
                                                        error1210[speedindex]=da;//AD2S1210_READ(0xff);
                                                        speedindex++;
                                            }
                                                else
                                                {
                                                            speedindex=0;
                                                }
                                    }
                                #endif



*/
    REPosition=AngPosition;//+32768;
    return REPosition;
}
unsigned char ReadA1210_Fault(void)
{
    unsigned char out;
    EXAMPLE_H;
       delay_us(5);
       EXAMPLE_L;
        delay_us(5);
        EXAMPLE_H;
        out=NewAD2S1210_READ(0xff);
        delay_us(2);
        EXAMPLE_L;
        delay_us(5);
        EXAMPLE_H;
     return out;
}

void A1210_INIT(void)
{
  Uint16 ValCon;
//Uint16 reg[10]={0,0,0,0,0,0,0,0,0,0};
  ValCon=0x70|(ResoverBit<<2)|ABLine;     //ResoverBit 0 10bit 1 14bit 2. .ABline:  0,10bit1024线 1，12bit 4096线 2
  NewAD2S1210_WRITE(0x92,ValCon);//Set 16Bit 0x7A->12Bit  0x75 14bit  0x7F 16bit

  NewAD2S1210_WRITE(0x91,0x28);//Set Driver freq      datasheet Page22    10K  250*40
  NewAD2S1210_WRITE(0x8b,0x7f);//DOS
  NewAD2S1210_WRITE(0x8c,0x01);//DOS
  NewAD2S1210_WRITE(0x88,0x01);//LOS
  NewAD2S1210_WRITE(0x89,0x7f);//DOS
  NewAD2S1210_WRITE(0x8a,0x70);//DOS
  reg[0]=NewAD2S1210_READ(0x88);//LOS
  reg[1]=NewAD2S1210_READ(0x89);//DOS
  reg[2]=NewAD2S1210_READ(0x8a);//DOS
  reg[3]=NewAD2S1210_READ(0x8b);//DOS
  reg[4]=NewAD2S1210_READ(0x8c);//DOS
  reg[5]=NewAD2S1210_READ(0x8d);//LOT
  reg[6]=NewAD2S1210_READ(0x8e);//LOT
  reg[7]=NewAD2S1210_READ(0x91);//
  reg[8]=NewAD2S1210_READ(0x92);//
  reg[9]=NewAD2S1210_READ(0xff);//Fault

}




/*
void AD2S1210Initiate()
{
    SET_SAMPLE();
    delay(3);
    CLR_SAMPLE();
    delay(1);
    SET_SAMPLE();
}

void delay (int length)
{
    while (length>0)
        length--;
}

void SPI3Init(void)
{

    EALLOW;

    //���ÿ���
    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;   //GPBMUX2
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;

    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 0;          //����
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;
       GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;
       GpioCtrlRegs.GPADIR.bit.GPIO30 = 1;
       GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;

    GpioCtrlRegs.GPBPUD.bit.GPIO53 = 0;   // Enable pull-up on GPIO53 (SDI)  0 �򹤿�����
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;   // Enable pull-up on GPIO48 (SDO)
    GpioCtrlRegs.GPBPUD.bit.GPIO49 = 1;   // Enable pull-up on GPIO49 (SCLK)
    GpioCtrlRegs.GPAPUD.bit.GPIO30 = 0;   // Enable pull-up on GPIO30 (WR)
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;   // Enable pull-up on GPIO31 (SAMPLE)



    EDIS;
}


void SPI3Read(unsigned char count, unsigned char *buf)
{
    unsigned    char    i = 0;
    unsigned    char    j = 0;
    unsigned    int     iTemp = 0;
    unsigned    char    RotateData = 0;
    for(j=count; j>0; j--)
    {
        for(i=0; i<8; i++)
        {
            SET_SCLK();
            RotateData <<= 1;       //Rotate data
            delay(1);
            iTemp = GpioDataRegs.GPBDAT.bit.GPIO53;         //Read SDO of AD2S
            CLR_SCLK();

            if(iTemp==1)
            {
                RotateData |= 1;
            }
            delay(1);
    //      RotateData <<= 1;       //Rotate data
        }
        *(buf + j - 1)= RotateData;
    }
}
void ReadFromAD2S1210(unsigned char mode,unsigned char address,unsigned char*buf)
{
    if(mode==CONFIG)
    {
        buf[0]=address;

        SET_SCLK();
        delay(1);
//      SET_CS();
        delay(1);
//      CLR_CS();
        delay(1);


        SET_WR();
        delay(1);
        CLR_WR();
        delay(1);

        SPI3Write(1,buf);
        SET_WR();
        delay(1);
//      SET_CS();

        SET_SCLK();
//      SET_CS();
        SET_WR();
        delay(1);

//      CLR_CS();
        delay(1);
        CLR_SCLK();
        delay(1);

        CLR_WR();
        delay(1);

        SPI3Read(1,buf);

        SET_WR();
        delay(1);

//      SET_CS();

    }
    else if (mode==POSITION||mode==VELOCITY)
    {
        SET_SAMPLE();
        delay(1);
        CLR_SAMPLE();
        delay(1);

        SET_SCLK();
//      SET_CS();
        SET_WR();
        delay(1);

//      CLR_CS();
        delay(1);
        CLR_SCLK();
        delay(1);

        CLR_WR();
        delay(1);

        SPI3Read(3,buf);

        SET_WR();
        delay(1);

//      SET_CS();

    }


}


void SPI3Write(unsigned char count, unsigned char *buf)
{
        unsigned    char    ValueToWrite = 0;
        unsigned    char    i = 0;
        unsigned    char    j = 0;



        for(i=count;i>0;i--)
        {
            ValueToWrite = *(buf + i - 1);
            for(j=0; j<8; j++)
            {
                SET_SCLK();
                if(0x80 == (ValueToWrite & 0x80))
                {
                    SET_SDI();    //Send one to SDI pin
                }
                else
                {
                    CLR_SDI();    //Send zero to SDI pin
                }
                delay(1);
                CLR_SCLK();
                delay(1);
                ValueToWrite <<= 1; //Rotate data
            }



        }

 }



void WriteToAD2S1210(unsigned char address, unsigned char data)
{
    unsigned    char    buf;

    //write control register address
    buf = address;

    SET_SCLK();
    delay(1);
//  SET_CS();
    delay(1);
//  CLR_CS();
    delay(1);

    SET_WR();
    delay(1);
    CLR_WR();
    delay(1);

    SPI3Write(1,&buf);

    SET_WR();
    delay(1);
//  SET_CS();
    //write control register address

    //write control register data
    buf = data;

    SET_SCLK();
    delay(1);
//  SET_CS();
    delay(1);
//  CLR_CS();
    delay(1);

    SET_WR();
    delay(1);
    CLR_WR();
    delay(1);

    SPI3Write(1,&buf);

    SET_WR();
    delay(1);
//  SET_CS();
    //write control register data
}


int16 READ_Position(void)//
{
    int16 temp;
    CLR_SAMPLE();//SET_SAMPLE();//EXAMPLE_L;
    delay_us(1);
    SET_SAMPLE();//EXAMPLE_H;
  temp=AD2S1210_READ(0x80);
  temp<<=8;
  temp|=AD2S1210_READ(0x81);
    temp=(temp&0xFFFC);
    return temp;

}

*/



//===========================================================================
// No more.
//===========================================================================
