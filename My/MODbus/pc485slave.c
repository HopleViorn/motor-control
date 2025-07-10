#include "pc485slave.h"
#include "Masterrtu.h"
#include <string.h>
#include "24cxx.h"
#include "function.h"
#include "tm1650.h"
extern uint8_t BigError;
uint8_t WriteUseTimeTOFlash;
extern uint8_t HaveError;
extern u8 CODE7_180[17];
uint16_t inputpawd;
uint16_t syspassword=2008;
extern uint8_t  iic_WRtemp[200];
extern UART_HandleTypeDef huart1;
extern uint8_t Pc485RX2Buff[8];
extern uint8_t Pc485TX2Buff[8];
int16_t Pc485RtuReg[100];    
extern uint8_t MyPC485addr;
extern char CrcH, CrcL;

void PC485SlaveProcess03(void)     //01 03 00 00 00 01 xx xx  回答：01 03 02  d1 d2 xx xx 
{
	 uint8_t i,RegIndex;
	//	RegIndex=Pc485RX2Buff[3];	//Reg地址L
   Pc485TX2Buff[0]=MyPC485addr;   //地址
   Pc485TX2Buff[1]=0x03;          //命令
   Pc485TX2Buff[2]=Pc485RX2Buff[5]*2;    //  回答数据长度每个寄存器2个字节

  // memcpy(&Pc485TX2Buff[3],&Pc485RtuReg[Pc485RX2Buff[2]*256+Pc485RX2Buff[3]],Pc485RX2Buff[5]*2);
	 for(i=0;i<Pc485RX2Buff[5];i++)
	 {
				Pc485TX2Buff[3+i*2]=(Pc485RtuReg[Pc485RX2Buff[2]*256+Pc485RX2Buff[3]+i]&0xff00)>>8;
        Pc485TX2Buff[4+i*2]=(Pc485RtuReg[Pc485RX2Buff[2]*256+Pc485RX2Buff[3]+i]&0x00ff);
	 
		 
	 }
	
   setSendCRC(Pc485TX2Buff,Pc485TX2Buff[2]+3 );
   Pc485TX2Buff[Pc485TX2Buff[2]+3]=CrcL;
   Pc485TX2Buff[Pc485TX2Buff[2]+3+1]=CrcH;
   Pc485H;
   HAL_UART_Transmit_IT(&huart1,Pc485TX2Buff,Pc485TX2Buff[2]+5);
  
 }
void PC485SlaveProcess06(void)     //01 06 00 00 00 01 xx xx  回答：01 06 0
{
	if(Pc485RX2Buff[3]==30)//需要密码
   {
		 if(Pc485RtuReg[32]==syspassword)//需要密码
	   {
       if((Pc485RX2Buff[5]!=Pc485RtuReg[30]) ) 
       {
           ResoverBit_Line(Pc485RX2Buff[5]);
					 Pc485RtuReg[Pc485RX2Buff[3]]=Pc485RX2Buff[4]*256+Pc485RX2Buff[5];
           memcpy(iic_WRtemp,&Pc485RtuReg[Pc485RX2Buff[3]],2);
           eeprom_WriteBytes(iic_WRtemp,Pc485RX2Buff[3]*2,2);
					 Pc485RtuReg[32]=0;//执行完一次清除密码
       }
		 }
   }
	 else if(Pc485RX2Buff[3]==4)// && (Pc485RtuReg[32]==syspassword))  // 同步，需要密码
	 {
		 if(Pc485RtuReg[32]==syspassword)//需要密码
	   {
				Pc485RtuReg[4]=Pc485RX2Buff[4]*256+Pc485RX2Buff[5];    
				
		 }
	 }
//	 else if(Pc485RX2Buff[3]==22)// &&(Pc485RtuReg[32]==syspassword))  //
//	 {
//		 if(Pc485RtuReg[32]==syspassword)//需要密码
//	   {
//				Pc485RtuReg[22]=Pc485RX2Buff[4]*256+Pc485RX2Buff[5];
//				
//		 }			 
//	 }
	 else if(Pc485RX2Buff[3]==31)//DSP复位
	 {
		 if(Pc485RtuReg[32]==syspassword)//需要密码
	   {
			  HAL_Delay(500);
        ResetDSP();
			 Pc485RtuReg[32]=0;//执行完一次清除密码
	   }

	 }
	  else if(Pc485RX2Buff[3]==27)   //清除arm报警和DSP报警
		{  
								Pc485RtuReg[12]=0;  //清code
								TM1650_Set(0x6E,CODE7_180[0]); 
								TM1650_Set(0x6C,CODE7_180[0]); 
								TM1650_Set(0x6A,CODE7_180[0]);   
								TM1650_Set(0x68,CODE7_180[0]);  
								ClearALMcom();//清DSP报警
								HaveError=0;//清自已报警 
								armReset(); 
		}
		else if(Pc485RX2Buff[3]==3)   //开机
		{  
				if((!HaveError)&&(!BigError))		Pc485RtuReg[Pc485RX2Buff[3]]=Pc485RX2Buff[4]*256+Pc485RX2Buff[5];
		}
	 else                    //不需要密码的参数直接写
	 {
		  Pc485RtuReg[Pc485RX2Buff[3]]=Pc485RX2Buff[4]*256+Pc485RX2Buff[5];
	 }
	 
	 if((Pc485RX2Buff[3]==3)&&(Pc485RX2Buff[5]==0))//停机命令写使用时间进flash;
	 {
		  WriteUseTimeTOFlash=1;
	 }
	 
	 	 ////////////////////////////////命令回传//////////////////////////////////////
   Pc485H;
   HAL_UART_Transmit_IT(&huart1,Pc485RX2Buff,8);
	 
	 /////////////////////////////////////////保存Flash///////////////////////////////////////////
	 if((Pc485RX2Buff[3]==0)||(Pc485RX2Buff[3]==1)||(Pc485RX2Buff[3]==7)||(Pc485RX2Buff[3]==8)
		 ||(Pc485RX2Buff[3]==9)||(Pc485RX2Buff[3]==10)||(Pc485RX2Buff[3]==11)) // (Pc485RX2Buff[3]==2)  指令速度不再存储
	 {
		 memcpy(iic_WRtemp,&Pc485RtuReg[Pc485RX2Buff[3]],2);
     eeprom_WriteBytes(iic_WRtemp,Pc485RX2Buff[3]*2,2);
	 }
  
		
	 
   //if(Pc485RX2Buff[3]==32)//DSP复位   上面赋值时已经完成
	// {
	  //  inputpawd=s
	   //}
   
 }

