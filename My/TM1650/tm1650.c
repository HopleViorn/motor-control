#include "tm1650.h"
#define u16 uint16_t

//#include "MyTIM2.h"
//7����ʾ
u8 CODE7[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40,0x79};  //0B=��E��0x79

//8����ʾ
u8 CODE1[10] = {0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef};

u8 CODE7_180[17] = {0x3f, 0x30, 0x5b, 0x79, 0x74,   0x6d, 0x6f,   0x38, 0x7f, 0x7d, 0x7e,0x67,0x0f, 0x73 , 0x4f,   0x4e,0x40 };  //0B=��E��0x79
//			0		1			2			3			4			5        6      7     8    9    A    b      c     d     14 =e   f    -

//ȫ�ֱ�����ǣ�Ĭ��Ϊ1
	
	extern uint8_t RunFlag;
u16 flag = 1;
extern u16 speed_temp;
extern u16 ll;
extern u16 angel;
extern u16 speed;
extern u16 start_dir;
extern u16 zsb;
extern u16 mode;
extern u16 yk_index;
extern u16 Jjkw_Set;

extern u16 addr485;
extern u16 btr485;
extern u16 USART_BTR;
extern char Run_flag;
extern u16 RtuReg[30];

uint8_t DisRunStep;
uint16_t DisRunStepCount;
void TM1650_Init()
{
		IIC_Init();
	delay_us(4);
    TM1650_Set(0x48,0x25);
   delay_us(4);
    TM1650_Set(0x6E,CODE7_180[0]);
    TM1650_Set(0x6C,CODE7_180[0]);
    TM1650_Set(0x6A,CODE7_180[0]);
    TM1650_Set(0x68,CODE7_180[0]);
}

void TM1650_Set_Value(u8 addr, u8 value)
{

    TM1650_Set(addr,CODE7[value]);

}

void TM1650_Set(u8 addr, u8 dt)
{
   // I2C_Start();
    IIC_Start();
	
    IIC_Send_Byte(addr);
    IIC_Wait_Ack();
    
    IIC_Send_Byte(dt);
    IIC_Wait_Ack();
    
   // I2C_Stop();
		IIC_Stop();
}


u8 TM1650_Key()
{
    u8 key = 0;
    
    IIC_Start();
    IIC_Send_Byte(0x49);
    IIC_Wait_Ack();
    key = IIC_Read_Byte(0);
    IIC_Stop();
    
    return key;
}

void TM1650_Key_Demo2(void)
{
    u8 key=0;
    u8 key2=0;
    key = TM1650_Key();
    HAL_Delay(10);
    key2= TM1650_Key();
    
if(key==key2)
{
    switch(key)
    {
        //S1 ����;
        case 0x44:
        {
							
        }
        break;
        //S2 �Ϸ�;
        case 0x4C:
        {

        }
        break;
        //S3 �·�;
        
        case 0x54:
        {
   
        }  
        break;
     
          

    }   
    
}
}



void DisSpeed(uint16_t speed)
{
	uint8_t W,Q,B,S;
	
	W=speed/10000;
	Q=speed%10000/1000;
	B=speed%1000/100;
	S=speed%100/10;
							TM1650_Set(0x6E,CODE7_180[W]);
							TM1650_Set(0x6C,CODE7_180[Q]);
							TM1650_Set(0x6A,CODE7_180[B]);
							TM1650_Set(0x68,CODE7_180[S]);
	
	
	
	
}


void DisRun(void)
{
		if(DisRunStepCount<70)DisRunStepCount++;
	  else 
		{
			DisRunStepCount=0;
			if(DisRunStep<12)DisRunStep++;
			else
			{
					DisRunStep=0;
			}
				switch(DisRunStep)
			{
				case 0:
				{
							TM1650_Set(0x6E,0x08);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x0);
				}break;
				case 1:
				{
							TM1650_Set(0x6E,0x0);
							TM1650_Set(0x6C,0x08);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x0);
				}break;
				case 2:
				{
							TM1650_Set(0x6E,0x0);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x08);
							TM1650_Set(0x68,0x0);
				}break;
				case 3:
				{
							TM1650_Set(0x6E,0x0);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x08);
				}break;
				case 4:
				{
							TM1650_Set(0x6E,0x0);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x10);
				}break;
				case 5:
				{
							TM1650_Set(0x6E,0x0);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x20);
				}break;
				case 6:
				{
							TM1650_Set(0x6E,0x0);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x01);
				}break;
				case 7:
				{
							TM1650_Set(0x6E,0x0);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x01);
							TM1650_Set(0x68,0x0);
				}break;
				case 8:
				{
							TM1650_Set(0x6E,0x0);
							TM1650_Set(0x6C,0x01);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x0);
					
				}break;
				case 9:
				{
							TM1650_Set(0x6E,0x01);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x0);
				}break;
				case 10:
				{
							TM1650_Set(0x6E,0x02);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x0);
				}break;
				case 11:
				{
							TM1650_Set(0x6E,0x4);
							TM1650_Set(0x6C,0x0);
							TM1650_Set(0x6A,0x0);
							TM1650_Set(0x68,0x0);
				}break;
			}
	}
}

