#include "Masterrtu.h"
#include "pc485slave.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "tm1650.h"
#include "function.h"
#define u16 uint16_t
#define u8 uint8_t

		extern uint8_t				WriteDSPBusy;
		extern uint8_t				ReadDSPBusy;

extern uint32_t FactSpeed;
extern int16_t Pc485RtuReg[100]; 
u8 MyPC485addr=0x01;
u16 ReadAddr;
u8 FunctionCode;
extern uint8_t HaveError;
extern void MX_USART1_UART_Init(void);    
extern void MX_USART2_UART_Init(void);

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;

extern uint8_t Dsp485RX2Buff[8];
extern uint8_t Dsp485TX2Buff[8];

extern uint8_t Pc485RX2Buff[8];
extern uint8_t Pc485TX2Buff[8];

uint16_t  DspRetrunTimeout;
uint16_t  ErrorCode; //错误码
uint8_t DSP485Received;
uint8_t Pc485Received;
uint8_t ReturnAdr;
uint8_t ByteCount;

extern u8 CODE7_180[17];



extern uint8_t iic_WRtemp[200];
extern u16 RtuReg[83];
char modbus_t=4;
char modbus_t_temp=4;

u16 modbus_addr;
char bus_index;
char CrcH;
char CrcL;
char RegIndex;


	
void setSendCRC(uint8_t *buffer,uint8_t Length)
{
	char i,j;
	char sh,sl,cl,ch;
	
	CrcH=0xff;
	CrcL=0xff;
	cl=0x1;
	ch=0xa0;
	for(i=0;i<Length;i++)  //2*length+adr+function+length
	{ 
		CrcL=CrcL ^ *(buffer+i);//uart_sendbf[i];
		for(j=0;j<8;j++)
		{
			sh=CrcH;
			sl=CrcL;
			
			CrcH=CrcH>>1;
			CrcL=CrcL>>1;
			if(sh&0x1)CrcL|=0x80;
			if(sl&0x1){
				CrcH=CrcH^ch;
				CrcL=CrcL^cl;            //注意：CrcL在前
			}
		}
	}			
	//uart_sendbf[Length-2]=CrcH;	
	//uart_sendbf[Length-1]=CrcL;	
}
void setRceiveCRC(uint8_t *Pdata,char Length)
{
	//char Length;
	char i,j;
	char sh,sl,cl,ch;
	
	CrcH=0xff;
	CrcL=0xff;
	cl=0x1;
	ch=0xa0;
	for(i=0;i<Length;i++)
	{
		CrcL=CrcL^* (Pdata+i);//UART_RX_BUF4[i];
		for(j=0;j<8;j++)
		{
			sh=CrcH;
			sl=CrcL;
			
			CrcH=CrcH>>1;
			CrcL=CrcL>>1;
			if(sh&0x1)CrcL|=0x80;
			if(sl&0x1){
				CrcH=CrcH^ch;
				CrcL=CrcL^cl;
			}
		}
	}			
	//uart_sendbf[Length-2]=CrcH;	
	//uart_sendbf[Length-1]=CrcL;	
}
u16 crc16bitbybit(uint8_t *ptr, uint16_t len)
{
	const uint16_t polynom = 0xA001;
	uint8_t i;
	uint16_t crc = 0xffff;
	//CrcH=0xff;
	//CrcL=0xff;
 
	if (len == 0) {
		len = 1;
	}
	while (len--) {
		crc ^= *ptr;
		for (i = 0; i<8; i++)
		{
			if (crc & 1) {
				crc >>= 1;
				crc ^= polynom;
			}
			else {
				crc >>= 1;
			}
		}
		ptr++;
	}
	return(crc);
}
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart == &huart1)
//		{
//			Pc485Received=1;
//			HAL_UART_Receive_IT(&huart1,Pc485RX2Buff,5);
//		}

//}
 
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)  //接收事件中断
{
	uint16_t crcval;
    if(huart == &huart2)      //DSP485
		{
			if(Dsp485RX2Buff[1]==0x03)
			{
				crcval=crc16bitbybit(Dsp485RX2Buff,5);//01 03 02 d1 d2 crc1 crc2
				if(crcval==(Dsp485RX2Buff[6]*256+Dsp485RX2Buff[5]))
				{
						ReadDSPBusy=0; 
						DSP485Received=1;
						if(FunctionCode ==3)
						{		
							if(ReadAddr==0x0809)Pc485RtuReg[22]=(Dsp485RX2Buff[3])<<8|Dsp485RX2Buff[4];  //读左电机 Tn
							else if(ReadAddr==0x0806)
							{
									Pc485RtuReg[17]=(Dsp485RX2Buff[3])<<8|Dsp485RX2Buff[4];
									if(Pc485RtuReg[17]<0)Pc485RtuReg[17]=0;
																		FactSpeed=Pc485RtuReg[17];
							} //读Speed
						}
					}
			}
			else if(Dsp485RX2Buff[1]==0x06)
			{
				crcval=crc16bitbybit(Dsp485RX2Buff,6);//01 06 00 01 d1 d2 crc1 crc2 
				if(crcval==(Dsp485RX2Buff[7]*256+Dsp485RX2Buff[6]))	
					DSP485Received=1;		
					
			}	
			HAL_UARTEx_ReceiveToIdle_IT(&huart2, Dsp485RX2Buff, 16);  //
		}
	else if (huart == &huart1)  //PC485
	{
	  if((Pc485RX2Buff[0]==MyPC485addr) || (Pc485RX2Buff[0]==0x00)) //0为广播地址
	  {
			if(Pc485RX2Buff[1]==0x03)
			{
				crcval=crc16bitbybit(Pc485RX2Buff,6);//01 03 00 00 00 01 crc1 crc2
				if(crcval==(Pc485RX2Buff[7]*256+Pc485RX2Buff[6]))
				{
					Pc485Received=1;
					PC485SlaveProcess03();
				}					
			}
			else if(Pc485RX2Buff[1]==0x06)
			{
				crcval=crc16bitbybit(Pc485RX2Buff,6);//01 06 00 01 d1 d2 crc1 crc2 
				if(crcval==(Pc485RX2Buff[7]*256+Pc485RX2Buff[6]))		
				{
					Pc485Received=1;		
					PC485SlaveProcess06();
				}
			}	
		
	  }	
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, Pc485RX2Buff, 11);  //	 超出一个保险，但只能空闲中断了 10个容易发生误码
	}
}

#if 0
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)   //返回DSP 485数据
{
		if(huart == &huart2)
		{
			
			DSP485Received=1;
			HAL_UART_Receive_IT(&huart2,Dsp485RX2Buff,8);
		}
}
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)   //发送DSP 485数据中断
{
		if(huart == &huart2)    //DSP485
		{
			C485L;	//发送完成，关闭485发送
		}
		else if(huart==&huart1)    //Pc485
		{
			Pc485L;   
			
		}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)   
{
	if(huart == &huart2)
		{
//				HAL_UART_DeInit(&huart2);
//				 MX_USART2_UART_Init();      //DSP485
//					__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
//					HAL_UARTEx_ReceiveToIdle_IT(&huart2, Dsp485RX2Buff, 16);  //
//			
		}
	else if(huart == &huart1)
	{
//				HAL_UART_DeInit(&huart1);
//				MX_USART1_UART_Init();     //PC485//串口 出错处理
//			__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
//					HAL_UARTEx_ReceiveToIdle_IT(&huart1, Dsp485RX2Buff, 16);  //
	}
}


/*此函数只包含发送命令及判断是否超时。不包含整个查出具体电角度数值。整个过程函数为DoFingInitAngVal()*/
uint16_t FindInitAng(uint8_t MotoNob) //发送485命令固定8字节，查找电机角度    01 06 10 63 00 01 C5 78 MotoNob=1 or 2
{ 
     Dsp485TX2Buff[0]=MotoNob;
     Dsp485TX2Buff[1]=0x06;
     Dsp485TX2Buff[2]=0x10;        //寄存器地址102B 串口对相角度
	 Dsp485TX2Buff[3]=0x2B;
     Dsp485TX2Buff[4]=0x00;
     Dsp485TX2Buff[5]=0x01;
     setSendCRC((Dsp485TX2Buff),6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;
     Dsp485TX2Buff[7]=CrcH;

	 DSP485Received=0;
     C485H;
     //HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);
	// HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);  //HAL_UART_Transmit_IT
		HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//
//	 C485L;
     while((!DSP485Received)&(DspRetrunTimeout<100))
     {
     	DspRetrunTimeout++;
		HAL_Delay(20);//等待DSP返回数据
     }
	 if((DspRetrunTimeout>=100)|(Dsp485RX2Buff[0]!=MotoNob)|(Dsp485RX2Buff[2]!=0x10)|(Dsp485RX2Buff[3]!=0x2B))
	 {
	 	 ErrorCode=0xff;   //超时,返回错误代码
		 TM1650_Set(0x6E,CODE7_180[0x0e]);
		 TM1650_Set(0x6C,CODE7_180[0]);
		 TM1650_Set(0x6A,CODE7_180[0x0F]);
		 TM1650_Set(0x68,CODE7_180[0x0F]);
		 
		 while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
								armReset();
		 
		 
	 }
	 else
	 {
			DSP485Received=0;
			return 0;
	 }

}   

uint8_t CheckFindOver(uint8_t MotoNob)  //发送485命令固定8字节，确定是否完成 找电机角度  1 完成 0 未完成  01 03 09 05 00 01 0A 0A
{
    
     Dsp485TX2Buff[0]=MotoNob;
     Dsp485TX2Buff[1]=0x03;
     Dsp485TX2Buff[2]=0x09;        //寄存器地址09 05 电机角度查找状态
	 Dsp485TX2Buff[3]=0x05;
     Dsp485TX2Buff[4]=0x00;
     Dsp485TX2Buff[5]=0x01;
     setSendCRC((Dsp485TX2Buff),6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;
     Dsp485TX2Buff[7]=CrcH;
	 DSP485Received=0;
     C485H;
   //  HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);
	HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//,20);
	// C485L;
	
     while((!DSP485Received)&(DspRetrunTimeout<100))
     {
     	DspRetrunTimeout++;
			HAL_Delay(200);//等待DSP返回数据	
     }
	 if((DspRetrunTimeout>=100)|(Dsp485RX2Buff[0]!=MotoNob))
	 {
	     
	 	ErrorCode=0xff;   //超时,返回错误代码
		 TM1650_Set(0x6E,CODE7_180[0x0e]);
			TM1650_Set(0x6C,CODE7_180[0]);
			TM1650_Set(0x6A,CODE7_180[0x0F]);
			TM1650_Set(0x68,CODE7_180[0x0F]);
		return ErrorCode;   //超时,返回错误
	 }
	 else
	 {
		return Dsp485RX2Buff[4];//返回0  or 1     1表示完成
	 }

}


uint16_t AdjSpeed(uint8_t MotoNb,uint16_t ComSpeed)//发送485命令固定8字节，设置单电机速度 x  06  01 30 xx xx crcl crch
{ 

     Dsp485TX2Buff[0]=MotoNb;  //电机编号
     Dsp485TX2Buff[1]=0x06;    //功能码
     Dsp485TX2Buff[2]=0x01;        // 01 30   //3040 速度寄存器
	 Dsp485TX2Buff[3]=0x30;    //

     Dsp485TX2Buff[4]=(ComSpeed&0xff00)>>8;  //速度高8位
     Dsp485TX2Buff[5]=ComSpeed&0x00ff;      //速度低8位

     setSendCRC(Dsp485TX2Buff,6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;          //CRC校验低8位
     Dsp485TX2Buff[7]=CrcH;          //CRC校验高8位

	 DSP485Received=0;               //接收标志位清零
     C485H;                          //485总线高电平
    // HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);  //发送8字节数据
			HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//  

}   

void AdjAddTime(uint8_t MotoNb,uint32_t time)
{
	

     Dsp485TX2Buff[0]=MotoNb;  //电机编号
     Dsp485TX2Buff[1]=0x06;    //功能码
     Dsp485TX2Buff[2]=0x01;        // 01 30   //3040 速度寄存器
			Dsp485TX2Buff[3]=0x32;    //     加速时间306

     Dsp485TX2Buff[4]=(time&0xff00)>>8;  //速度高8位
     Dsp485TX2Buff[5]=time&0x00ff;      //速度低8位

     setSendCRC(Dsp485TX2Buff,6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;          //CRC校验低8位
     Dsp485TX2Buff[7]=CrcH;          //CRC校验高8位

			DSP485Received=0;               //接收标志位清零
     C485H;                          //485总线高电平
    // HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);  //发送8字节数据
		HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//,20);  //发送8字节数据
//			C485L;
//     while((!DSP485Received)&(DspRetrunTimeout<100))
//     {
//     	DspRetrunTimeout++;
//			HAL_Delay(2);//等待DSP返回数据
//     }
//		if((DspRetrunTimeout>=100)|(Dsp485RX2Buff[0]!=MotoNb)|(Dsp485RX2Buff[4]!=Dsp485TX2Buff[4])|(Dsp485RX2Buff[5]!=Dsp485TX2Buff[5]))
//		{
//			ErrorCode=0xff;   //超时,返回错误代码
//			TM1650_Set(0x6E,CODE7_180[0x0e]);
//			TM1650_Set(0x6C,CODE7_180[0]);
//			TM1650_Set(0x6A,CODE7_180[0x0F]);
//			TM1650_Set(0x68,CODE7_180[0x0F]);
//			return ErrorCode;   //超时,返回错误
//		}
//		else
//	 {
//		
//			DSP485Received=0;
//			return 0;
//		}

   
}

void AdjDecTime(uint8_t MotoNb,uint32_t time)
{
	

     Dsp485TX2Buff[0]=MotoNb;  //电机编号
     Dsp485TX2Buff[1]=0x06;    //功能码
     Dsp485TX2Buff[2]=0x01;        // 01 30   //3040 速度寄存器
			Dsp485TX2Buff[3]=0x33;    //     加速时间307

     Dsp485TX2Buff[4]=(time&0xff00)>>8;  //速度高8位
     Dsp485TX2Buff[5]=time&0x00ff;      //速度低8位

     setSendCRC(Dsp485TX2Buff,6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;          //CRC校验低8位
     Dsp485TX2Buff[7]=CrcH;          //CRC校验高8位

			DSP485Received=0;               //接收标志位清零
     C485H;                          //485总线高电平
    // HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);  //发送8字节数据
		HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//,20);  //发送8字节数据
		//	C485L;
//     while((!DSP485Received)&(DspRetrunTimeout<100))
//     {
//     	DspRetrunTimeout++;
//			HAL_Delay(2);//等待DSP返回数据
//     }
//		if((DspRetrunTimeout>=100)|(Dsp485RX2Buff[0]!=MotoNb)|(Dsp485RX2Buff[4]!=Dsp485TX2Buff[4])|(Dsp485RX2Buff[5]!=Dsp485TX2Buff[5]))
//		{
//			ErrorCode=0xff;   //超时,返回错误代码
//			TM1650_Set(0x6E,CODE7_180[0x0e]);
//			TM1650_Set(0x6C,CODE7_180[0]);
//			TM1650_Set(0x6A,CODE7_180[0x0F]);
//			TM1650_Set(0x68,CODE7_180[0x0F]);
//			return ErrorCode;   //超时,返回错误
//		}
//		else
//	 {
//		
//			DSP485Received=0;
//			return 0;
//		}

   
}


int16_t ReadSpeed(uint8_t MotoNb) //发送485命令固定8字节，读取单电机速度 x  03  02 d1 d2 crcl crch
{
		uint16_t speed;
     Dsp485TX2Buff[0]=MotoNb;  //电机编号
     Dsp485TX2Buff[1]=0x03;    //功能码
     Dsp485TX2Buff[2]=0x01;        // 01 30   //3040 速度寄存器
	 Dsp485TX2Buff[3]=0x30;    //
     Dsp485TX2Buff[4]=0x00;      //读取1reg
     Dsp485TX2Buff[5]=0x01;

	setSendCRC(Dsp485TX2Buff,6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;          //CRC校验低8位
     Dsp485TX2Buff[7]=CrcH;          //CRC校验高8位

	 DSP485Received=0;               //接收标志位清零
     C485H;                          //485总线高电平
    // HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);  //发送8字节数据
		HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//  //发送8字节数据 中断方式
		
	// C485L;
	DspRetrunTimeout=0;
	while((!DSP485Received)&(DspRetrunTimeout<100))
   {
     	DspRetrunTimeout++;
			HAL_Delay(2);//等待DSP返回数据
    }
	 if((DspRetrunTimeout>=100)|(Dsp485RX2Buff[0]!=MotoNb)|(Dsp485RX2Buff[1]!=0x03))
	 {
	 	ErrorCode=0xff;   //超时,返回错误代码
		 TM1650_Set(0x6E,CODE7_180[0x0e]);
			TM1650_Set(0x6C,CODE7_180[0]);
			TM1650_Set(0x6A,CODE7_180[0x0F]);
			TM1650_Set(0x68,CODE7_180[0x0F]);
		return ErrorCode;   //超时,返回错误
	 }
	 else
	 {
		speed=Dsp485RX2Buff[3]*256+Dsp485RX2Buff[4];
	 	DSP485Received=0;
		return speed;
		}
	 
}



//--------------------------------------------------------------------------------------------
 uint8_t SendDSPCommand(uint8_t MotoNb,uint16_t regaddr,uint16_t FunData)
{
	//清除报警  1022  //0904运行时间 
	 Dsp485TX2Buff[0]=MotoNb;
     Dsp485TX2Buff[1]=0x06;
     Dsp485TX2Buff[2]=(regaddr&0xff00)>>8; //寄存器地址高位       //寄存器地址102B 串口对相角度
	 Dsp485TX2Buff[3]=regaddr&0x00ff;  //寄存器地址低位
     Dsp485TX2Buff[4]=(FunData&0xff00)>>8; 
     Dsp485TX2Buff[5]=FunData&0x00ff;
     setSendCRC((Dsp485TX2Buff),6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;
     Dsp485TX2Buff[7]=CrcH;
	 DSP485Received=0;
     C485H;
     //HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);
	// HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);
	HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//20);
	// C485L;
	 if(MotoNb!=0)//非广播地址
	 {
    	while((!DSP485Received)&(DspRetrunTimeout<100))
     	{
     		DspRetrunTimeout++;
			HAL_Delay(20);//等待DSP返回数据
     	}
	}
	else DspRetrunTimeout=0;  //广播地址不等待
	
	if(MotoNb!=0)//非广播地址才检查
	{
		if((DspRetrunTimeout>=100)|(Dsp485RX2Buff[0]!=MotoNb))
		{
	 	 ErrorCode=0xff;   //超时,返回错误代码
		 TM1650_Set(0x6E,CODE7_180[0x0e]);
		 TM1650_Set(0x6C,CODE7_180[0]);
		 TM1650_Set(0x6A,CODE7_180[0x0F]);
		 TM1650_Set(0x68,CODE7_180[0x0F]);
		 return ErrorCode;   //超时,返回错误
	 	}
	 	else
	 	{
				DSP485Received=0;
			return 0;
	 	}
	}
	else//广播地址延时100ms
		{
			HAL_Delay(100);
	}
}
//---------------------------------------------------------------------------------
uint16_t ReadDsp1Reg(uint8_t MotoNb,uint16_t regaddr) 
{
		uint16_t out;
     Dsp485TX2Buff[0]=MotoNb;  //电机编号
     Dsp485TX2Buff[1]=0x03;    //功能码
     Dsp485TX2Buff[2]=(regaddr&0xff00)>>8;  
	 Dsp485TX2Buff[3]=regaddr&0x00ff;    //
     Dsp485TX2Buff[4]=0x00;      //读取1reg
     Dsp485TX2Buff[5]=0x01;

	setSendCRC(Dsp485TX2Buff,6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;          //CRC校验低8位
     Dsp485TX2Buff[7]=CrcH;          //CRC校验高8位

	 DSP485Received=0;               //接收标志位清零
     C485H;                          //485总线高电平
    // HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);  //发送8字节数据
		HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//,20);  //发送8字节数据
	// C485L;
	DspRetrunTimeout=0;
	while((!DSP485Received)&(DspRetrunTimeout<100))
   {
     	DspRetrunTimeout++;
			HAL_Delay(2);//等待DSP返回数据
    }
	 if((DspRetrunTimeout>=100)|(Dsp485RX2Buff[0]!=MotoNb)|(Dsp485RX2Buff[1]!=0x03))
	 {
				ErrorCode=0xff;   //超时,返回错误代码
			//TM1650_Set(0x6E,CODE7_180[0x0e]);
			//TM1650_Set(0x6C,CODE7_180[0]);
			//TM1650_Set(0x6A,CODE7_180[0x0F]);
			//TM1650_Set(0x68,CODE7_180[0x0F]);
		//return ErrorCode;   //超时,返回错误
	 }
	 else
	 {
		out=Dsp485RX2Buff[3]*256+Dsp485RX2Buff[4];
	 	DSP485Received=0;
		return out;
		}
	 
}

//---------------------------------------------------------------------------------
uint16_t ReadDsp1RegITReiver(uint8_t MotoNb,uint16_t regaddr) 
{
		uint16_t out;
     Dsp485TX2Buff[0]=MotoNb;  //电机编号
     Dsp485TX2Buff[1]=0x03;    //功能码
     Dsp485TX2Buff[2]=(regaddr&0xff00)>>8;  
	 Dsp485TX2Buff[3]=regaddr&0x00ff;    //
     Dsp485TX2Buff[4]=0x00;      //读取1reg
     Dsp485TX2Buff[5]=0x01;

	setSendCRC(Dsp485TX2Buff,6 );   //数据长度在buf[5]+前面3个里;          响应为0地址+1功能码+2字节长度+34.。字节内容+CRC
     Dsp485TX2Buff[6]=CrcL;          //CRC校验低8位
     Dsp485TX2Buff[7]=CrcH;          //CRC校验高8位

	 DSP485Received=0;               //接收标志位清零
     C485H;                          //485总线高电平
    // HAL_UART_Transmit(&huart2,Dsp485TX2Buff,8,20);  //发送8字节数据
		ReadAddr=Dsp485TX2Buff[2]<<8|Dsp485TX2Buff[3];
		FunctionCode=3;
		HAL_UART_Transmit_IT(&huart2,Dsp485TX2Buff,8);//,20);  //发送8字节数据
	// C485L;	 
}