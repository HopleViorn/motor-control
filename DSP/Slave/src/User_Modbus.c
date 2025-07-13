/*******************************************************************
 *    DESCRIPTION:	SCI COMMUNCIATION FILE 
 *
 *    AUTHOR:	yizhenhua	
 *	  MODBUSͨѶЭ���µĴ������
 *******************************************************************/

#include "EDBclm.h"			// define and function declare
#include "globalvar.h"		//	
#include "SCImodbus.h" 

extern int16 AsSpeed;
Uint16 ResoverBit;
Uint16 ABLine;

Uint16 xyz=0x0;     //  WrDataAdd==0x1062

extern Uint16 PowerOnInitAng;
extern Uint16 EncoderKp;
extern Uint16   sst;
Uint16 Pos485;
extern Uint16 standSum;
Uint16 WriteData[4];
//------------------------------------------------------------------
void CMD00(void);
void CMD03(void);
void CMD06(void);
void CMD10(void);
void CMD2B(void);


Uint16 Para_Maxmin(Uint16 Para_Address,int16 data);
//----------------------------------------------------
Uint16 GetSCIBRXstatus_MOD(void);
Uint16 WriteToSCIB_MOD(pTXBUFEER p);
//----------------------------------------------------
Uint16 ASCII_HEX(Uint16 High_Data,Uint16 Low_Data);
Uint16 HEX_ASCII(Uint16 Hex_Data);
Uint16 CRC_checkcode(Uint16 *data,Uint16 length);
//-------ASCII---------------------------------------------
void SciErrorProcess(void);
void DealSCI_MOD(void);
void SCIReceive_MOD(void);
void SCITransmit_MOD(void);
void SCI_ISR_Modbus(void);
void SCI_ISRnotModbus(void);
Uint16 PnSwitch(Uint16 Para_Address);
Uint32 SampleAddressMap(Uint16 SampleAddress);

extern const Uint16 PnAttribute[][4];
void	Write_Data(Uint16 WrDataAdd,Uint16 WrDataEdit,Uint16 temp,Uint16 * tmpPointer);

//-----------------------------------------------------------------------------
//=============================================================================
//	ͨѶ״̬�������
//	���ݲ���ʵʱ�ı�ͨѶЭ��
//=============================================================================

//=============================================================================
void SCI_COM_Process(void)
{		
		if(memSCIsel)
		{
			SCITransmit_MOD();
			SciErrorProcess();
			DealSCI_MOD();
			SCIReceive_MOD();
			if(int_flagx.bit.SciAnAutoWrFRAM)
			{
				int_flagx.bit.SciAnAutoWrFRAM = 0;
				SPI_WriteFRAM(Fn004Addr,&Pn[Fn004Addr],1);
				SPI_WriteFRAM(Fn004Addr+1,&Pn[Fn004Addr+1],1);
				SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
			}
		}
		else
		{
			CommTxData();
		}			
}
//-----------------------------------------------------------------------//
//  串行通讯口配置  及SCIA寄存器初始化
//  SCI通讯复位---->改变驱动器传输速率、改变通讯协议
//  通讯速率可调，通讯地址为1，通讯协议：ASCII，8,O,1//
//-----------------------------------------------------------------------//	
void SCIBRST_init(void)
{

    //初始化DSP-SCI
	InitSci();

	//通讯地址ַ
	sci_data.Address = memModbusAdd;

	//传输速率
	if(!memSCIBRT)
	{
		ScicRegs.SCIHBAUD = 0x0003;
		ScicRegs.SCILBAUD = 0x00D0;//4800 baud
		sci_data.SCIFourbyteTIME = (Uint32)8*11*1000*4/4800;//��250usΪһ������ 4���ֽ���
	}
	else if(memSCIBRT == 1)
	{
		ScicRegs.SCIHBAUD = 0x0001;//
		ScicRegs.SCILBAUD = 0x00E7;//9600 baud
		sci_data.SCIFourbyteTIME = (Uint32)8*11*1000*4/9600;//��250usΪһ������
	}
	else
	{
		//ScicRegs.SCIHBAUD = 0x0000;//
		//ScicRegs.SCILBAUD = 0x00F3;//19200 baud
		//sci_data.SCIFourbyteTIME = (Uint32)8*11*1000*4/19200;//��250usΪһ������
	    //---------------------------------------115200------
	            ScicRegs.SCIHBAUD = 0x0000;//
	            ScicRegs.SCILBAUD = 0x0027;//115200 baud
	            sci_data.SCIFourbyteTIME = (Uint32)8*11*1000*4/115200;//��250usΪһ������
	}

	//通讯协定
	if(!memSCIPTL)
	{// 7 N 2
		ScicRegs.SCICCR.bit.SCICHAR = 6;
		ScicRegs.SCICCR.bit.PARITYENA = 0;
		ScicRegs.SCICCR.bit.STOPBITS = 1;
//		ScicRegs.SCICCR.all = 0x0086;
	}
	else if(memSCIPTL == 1)
	{// 7 E 1
		ScicRegs.SCICCR.bit.SCICHAR = 6;
		ScicRegs.SCICCR.bit.PARITYENA = 1;
		ScicRegs.SCICCR.bit.PARITY = 1;
		ScicRegs.SCICCR.bit.STOPBITS = 0;
//		ScibRegs.SCICCR.all = 0x0066;
	}
	else if(memSCIPTL == 2)
	{// 7 O 1
		ScicRegs.SCICCR.bit.SCICHAR = 6;
		ScicRegs.SCICCR.bit.PARITYENA = 1;
		ScicRegs.SCICCR.bit.PARITY = 0;
		ScicRegs.SCICCR.bit.STOPBITS = 0;
//		ScibRegs.SCICCR.all = 0x0026;
	}
	else if((memSCIPTL == 3) || (memSCIPTL == 6))
	{// 8 N 2
		ScicRegs.SCICCR.bit.SCICHAR = 7;
		ScicRegs.SCICCR.bit.PARITYENA = 0;
		ScicRegs.SCICCR.bit.STOPBITS = 1;
//		ScibRegs.SCICCR.all = 0x0087;
	}
	else if((memSCIPTL == 4) || (memSCIPTL == 7))
	{// 8 E 1
		ScicRegs.SCICCR.bit.SCICHAR = 7;
		ScicRegs.SCICCR.bit.PARITYENA = 1;
		ScicRegs.SCICCR.bit.PARITY = 1;
		ScicRegs.SCICCR.bit.STOPBITS = 0;
//		ScibRegs.SCICCR.all = 0x0067;
	}
	else// if((memSCIPTL == 5) || (memSCIPTL == 8))
	{// 8 O 1
		ScicRegs.SCICCR.bit.SCICHAR = 7;
		ScicRegs.SCICCR.bit.PARITYENA = 1;
		ScicRegs.SCICCR.bit.PARITY = 0;
		ScicRegs.SCICCR.bit.STOPBITS = 0;
//		ScibRegs.SCICCR.all = 0x0027;
	}

    ScicRegs.SCICTL1.bit.SWRESET = 1;	// Relinquish SCI from Reset

}
//-----------------------------------------------------------------------------
//
//  改变通讯设置：波特率，协议，校验，轴号等
//
//-----------------------------------------------------------------------------
void SciErrorProcess(void)
{	
	if(GetSCIBRXstatus_MOD())
	{											//���յ��ֽڴ���
		sci_flag.bit.RX_end=1;					//���ս���
		sci_flag.bit.RX_valid=0;				//���յ���������Ч
		sci_flag.bit.Reply=0;					//��������Ӧ��
		RXbuffer.pcurrent=RXbuffer.paddress;							
		SCIBRST_init();							//��λSCI������
		sci_data.sci_state=0;					//RECEIVE_DATA_SAVE;
	}
}
//-----------------------------------------------------------------------------
//  通讯常用数据更新
//-----------------------------------------------------------------------------
//-------------------100us------------------------
void Com_Timer(void)
{
	if(sci_flag.bit.SCI_Change_Flag) 
	{
		sci_data.SCI_Changed++;
	}
	else 
	{
		sci_data.SCI_Changed=0;
	}
}


//-----------------------------------------------------------------------------
//	interrupt void SCI_ISR
//-----------------------------------------------------------------------------
interrupt void SCI_RX_ISR(void)
//void SCI_RX_ISR1(void)			
{
	Uint16 rxtmp;
	//rxtmp = ScicRegs.SCIRXBUF.all & 0x00FF;
	if(memSCIsel)
	{
		if(GpioDataRegs.GPBDAT.bit.GPIO61 == 0)
		{
			SCI_ISR_Modbus();
		}
		else
		{
			rxtmp = ScicRegs.SCIRXBUF.all & 0x00FF;
			/*
			if(ScicRegs.SCIRXST.bit.RXERROR)
			{
				//rxtmp = ScicRegs.SCIRXBUF.all & 0x00FF;
				SCIBRST_init();
			}
			else
			{
				rxtmp = ScicRegs.SCIRXBUF.all & 0x00FF;
			} 
			*/	
			rxtmp = rxtmp;
		}
	}
	else
	{
		SCI_ISRnotModbus();
	}	
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
	EINT;
}

//-----------------------------------------------------------------------------
// File:     static Uint16  StoreRam(Uint32 * pdata)
// 功能描述：存储数据子程序
// 调用模块
//-----------------------------------------------------------------------------					
Uint16 SaveSCIDataModbus(Uint16 * pdata)
{ 		     		    
	Uint16 SCI_data=0;
		SCI_data=ScicRegs.SCIRXBUF.all & 0x00FF;
		sci_int_flag.bit.RXbuffer_add = 1;		//���ӱ�־
//-----------------------------------------------------------------------------
		//  如果是ASCII码，接收时就把ASCII数据转化成HEX数据
//-----------------------------------------------------------------------------
	//ASCII
	if(memSCIPTL < 6)
	{
		if(SCI_data == 0x0D)
		{
			if(sci_int_flag.bit.ASCIIHead)
			{
				sci_int_flag.bit.ASCII = 0;		//���ASCIIͨѶ��ʼ��־
				sci_int_flag.bit.Save_Flag = 0;
				* pdata = SCI_data;	
			}
			else
			{
				sci_int_flag.bit.RXbuffer_add = 0;		//���ӱ�־
			}		
		}
		else if(SCI_data == 0x0A)
		{
			if(sci_int_flag.bit.ASCIIHead)
			{				
				sci_int_flag.bit.ASCIIHead = 0;
				sci_int_flag.bit.ASCII = 0;
				sci_int_flag.bit.Save_Flag = 0;	//���ASCIIͨѶ��ʼ��־
				* pdata = SCI_data;			
				return	TRUE;
			}
			else
			{
				sci_int_flag.bit.RXbuffer_add = 0;		//���ӱ�־
			}
		}
		else if(SCI_data == 0x3A)
		{
			sci_int_flag.bit.ASCII = 1;
			sci_int_flag.bit.ASCIIHead = 1;
			sci_int_flag.bit.Save_Flag = 0;
			RXbuffer.pcurrent = RXbuffer.paddress;
			* RXbuffer.pcurrent = SCI_data;			
			RXbuffer.pcurrent = RXbuffer.paddress;
		}
		else if(sci_int_flag.bit.ASCII && sci_int_flag.bit.Save_Flag)
		{
			SCI_data = ASCII_HEX(sci_data.RX_data_ASCII,SCI_data);
			sci_int_flag.bit.Save_Flag = 0;
			* pdata = SCI_data;
		}
		else if(sci_int_flag.bit.ASCII && (sci_int_flag.bit.Save_Flag==0))
		{
			sci_int_flag.bit.Save_Flag = 1;
			sci_data.RX_data_ASCII = SCI_data;
			sci_int_flag.bit.RXbuffer_add = 0;//���մ洢�������ӱ�־
		}
 		return FALSE;
	}
	// RTU
	else
	{
		* pdata = SCI_data;
		return FALSE;
	}
}
//-----------------------------------------------------------------------------
//������ASCIIת����1���ֽڵ�HEX����
//-----------------------------------------------------------------------------
Uint16 ASCII_HEX(Uint16 High_Data,Uint16 Low_Data)
{
	Uint16 HexData,temp;
	if(High_Data >= ASCII_0 && High_Data <= ASCII_9)
	{
		temp = High_Data - ASCII_0;
	}
	else if(High_Data >= ASCII_A && High_Data <= ASCII_F)
	{
		temp = High_Data - ASCII_A + 10;
	}
	else 
	{
		sci_data.sci_state = RECEIVE_SCI_ERROR;//����ͨѶ����
	}
	HexData = temp<<4;		//HEX��λ��λ����
	if(Low_Data >= ASCII_0 && Low_Data <= ASCII_9)
	{
		temp = Low_Data - ASCII_0;
	}
	else if(Low_Data >= ASCII_A && Low_Data <= ASCII_F)
	{
		temp = Low_Data - ASCII_A + 10;
	}
	else 
	{
		sci_data.sci_state = RECEIVE_SCI_ERROR;//����ͨѶ����
	}
	HexData += temp;			//����һ��16���Ƶ�1�ֽ�����
	return	HexData;
}
//-----------------------------------------------------------------------------
//��4bit��HEX���� ת����SCII
//-----------------------------------------------------------------------------
Uint16 HEX_ASCII(Uint16 Hex_Data)
{
	Uint16	ASCII;
	ASCII = Hex_Data;
	if(ASCII <= 9)
	{
		ASCII = ASCII + 48;
	}
	else if((ASCII >= 10) && (ASCII <= 15))
	{
		ASCII = ASCII+55;
	}
	return	ASCII;
}
//-----------------------------------------------------------------------------
//	��SCI�Ĵ���״̬
//-----------------------------------------------------------------------------
Uint16 GetSCIBRXstatus_MOD(void)
{
	return	(ScicRegs.SCIRXST.bit.RXERROR ? TRUE:FALSE);
}

//****************************************
//�ȷ��͸�λ
Uint16 WriteToSCIB_MOD(pTXBUFEER  p)					//дһ���ֽڵ�SCIB
{	
	if(ScicRegs.SCICTL2.bit.TXRDY)
	//if(ScicRegs.SCICTL2.bit.TXEMPTY)
	{				// SCI���Է���
		ScicRegs.SCITXBUF = (* p->pcurrent) & 0x00FF;// Hst_data_wrtten=1:���͸�8λ
		p->pcurrent++;
		p->Hst_data_wrtten++;
	}
	return ( p->Hst_data_wrtten >= p->data_length)? TRUE:FALSE;	
}

//*============================================================================			  
//*			ASCII	&&	RTU	
//**************************************************************************		
//只用于标识状态
//==============================================================================
//接收中断
//使用到的变量及其含义:RX_type,RX_end,RX_valid,Reply,Message_valid,
//==============================================================================
void SCI_ISR_Modbus(void)			
{
	Timers.SCIRxfreecnt = 0;			// SCI接受空闲计数器清0
	sci_flag.bit.RX_have_data = 1;		// 接受到数据
	sci_int_flag.bit.RX_type=1;			// ��ǰ��Ϣ�ṹ232��Ϣ
	if(GetSCIBRXstatus_MOD())							//���յ��ֽڴ���
	{
		sci_flag.bit.RX_end=1;							//���ս���
		sci_flag.bit.RX_valid=0;						//���յ���������Ч
		sci_flag.bit.Reply=0;							//��������Ӧ��
		RXbuffer.pcurrent=RXbuffer.paddress;							
		SCIBRST_init();									//��λSCI������
		sci_data.sci_state = RECEIVE_DATA_SAVE;
	}
	else if(sci_data.sci_state == RECEIVE_DATA_SAVE)
	{
		if(SaveSCIDataModbus(RXbuffer.pcurrent))
		{
			sci_data.sci_state = RECEIVE_SCI_END;			//���յ�֡β��־,�п��ܲ���������β��־
		}
		if(sci_int_flag.bit.RXbuffer_add) 
		{
			RXbuffer.pcurrent++;//���ݰ����
		}
		if(RXbuffer.pcurrent>(RXbuffer.paddress+DataLength_Max_MOD))//��һֱ��������û��β��־ʱ������255������Ϊû����Ϣ
		{
			sci_data.sci_state=RECEIVE_SCI_ERROR;
		}
	}
}
//-------------------------------------------------------------------------------
//SCI通讯处理程序
//用到的变量:
//分离出有效数据，判断是不是自己的数据
//-------------------------------------------------------------------------------
void DealSCI_MOD(void)
{	    	
	Uint16  datalength,Address,* DataAddress;
	Uint16  checkcode=0,checkcode_cal=0;

	if(sci_int_flag.bit.RX_type)				//232通讯接收处理
	{
		if(Timers.SCIRxfreecnt >= 20000)		//2s没有数据，以前也没有接收到数据尾标志־
		{
			//sci_flag.bit.RX_end = 1;			//接收结束
			//sci_flag.bit.RX_valid = 0;			//收到的数据无效
			//sci_flag.bit.Reply = 0;				//主机无需应答
			//sci_flag.bit.RX_have_data = 0;
			//RXbuffer.pcurrent = RXbuffer.paddress;
			//sci_data.sci_state = RECEIVE_DATA_SAVE;			//ͨ通讯处于接收状态ڽ���״̬
			sci_data.sci_state = RECEIVE_SCI_ERROR;
		}
		else if(Timers.SCIRxfreecnt >= sci_data.SCIFourbyteTIME)
		{
			if((memSCIPTL >= 6) && sci_flag.bit.RX_have_data)
			{
				sci_data.sci_state = RECEIVE_SCI_END;
				Timers.SCIRxfreecnt = 0;
			}
		}
		if(sci_data.sci_state == RECEIVE_SCI_ERROR)
		{
			sci_flag.bit.RX_end = 1;				//���ս���
			sci_flag.bit.RX_valid = 0;			//�յ�������Ч
			sci_flag.bit.Reply = 0;				//��������Ӧ��
			sci_flag.bit.RX_have_data = 0;
			RXbuffer.pcurrent = RXbuffer.paddress;
			sci_data.sci_state = RECEIVE_DATA_SAVE;
		}
//*********************************************************************************
		//  分离接收数据
		//  MESG.RX_mesg.RX_cmd----------->接收命令
		//  MESG.RX_mesg.RX_data_ADR------>有效数据起始地址
		//  MESG.RX_mesg.RX_data_number--->有效数据长度 字节
//*********************************************************************************

		if(sci_data.sci_state == RECEIVE_SCI_END)	  			//���ս���
		{
			checkcode_cal = 0;								
			if(memSCIPTL < 6)									//ASCII
			{
				RXbuffer.pcurrent--;							// addr(0AH)
				RXbuffer.pcurrent--;							// addr(0DH)
				checkcode =* --RXbuffer.pcurrent;				//��LRCУ����
				DataAddress = RXbuffer.pcurrent;
				RXbuffer.pcurrent = RXbuffer.paddress;			//addr(3AH)
				Address =* ++RXbuffer.pcurrent;					//ͨѶ��ַ
				sci_data.RX_cmd = * ++RXbuffer.pcurrent;		//��������ָ����
				sci_data.RX_data_ADR = ++RXbuffer.pcurrent;		//��ʼ���ϵ�ַ
				RXbuffer.pcurrent = RXbuffer.pcurrent-2;		//ָ��ͨѶ��ַ
				datalength = DataAddress-RXbuffer.pcurrent;
				if((datalength > DataLength_Max_MOD) || (datalength < 4))//���ݳ��ȳ���
				{
					sci_data.RX_cmd = 224;						//����һ����������
				}
				else
				{
					for(;RXbuffer.pcurrent < DataAddress;RXbuffer.pcurrent++)
					{
						checkcode_cal = (* RXbuffer.pcurrent + checkcode_cal) & 0x00FF;
					}
					checkcode_cal = (0x1000-checkcode_cal) & 0x00FF;
				}
				sci_flag.bit.RX_data_number = (Uint16)(DataAddress - sci_data.RX_data_ADR);//��Ч�����ֽڸ���
			}
			else//RTU
			{
				RXbuffer.pcurrent--;							// 
				checkcode =* RXbuffer.pcurrent--;				//��CRC_H
				checkcode = (checkcode << 8) + *RXbuffer.pcurrent;
				DataAddress = RXbuffer.pcurrent;
				RXbuffer.pcurrent = RXbuffer.paddress;			//
				Address =* RXbuffer.pcurrent++;					//ͨѶ��ַ
				sci_data.RX_cmd = * RXbuffer.pcurrent++;		//��������ָ����
				sci_data.RX_data_ADR = RXbuffer.pcurrent;		//��ʼ���ϵ�ַ
				RXbuffer.pcurrent = RXbuffer.pcurrent - 2;		//ָ��ͨѶ��ַ
				datalength = DataAddress - RXbuffer.pcurrent;
				if((datalength > DataLength_Max_MOD) || (datalength < 4))//���ݳ��ȳ���
				{
					sci_data.RX_cmd = 224;						//����һ����������
				}
				else
				{
					checkcode_cal = CRC_checkcode(RXbuffer.pcurrent,datalength);					
				}
				sci_flag.bit.RX_data_number = (Uint16)(DataAddress - sci_data.RX_data_ADR);//��Ч�����ֽڸ���
			}
			
	
			    					
			if((checkcode_cal != checkcode)					//У��Ͳ���
				||(sci_data.RX_data_number > 244))			//���յ���Ч���ݸ���������4��������
			{
				sci_flag.bit.RX_end = 1;					//���ս���
				sci_flag.bit.RX_valid = 0;					//�յ���������Ч
				sci_flag.bit.Reply = 0;						//�ӻ�����Ӧ��
				RXbuffer.pcurrent = RXbuffer.paddress;
				sci_data.sci_state = RECEIVE_DATA_SAVE;
				return;
			}
			else
			{
				if(!Address)			//�㲥���� 	//��Ҫ���ظ���λ��,����δ���
				{
					if(!sci_data.Address)
					{
						sci_flag.bit.Reply = 1;
					}
					else 
					{
						sci_flag.bit.Reply = 0;
					}
				}
				else if(Address != sci_data.Address)
				{
					sci_flag.bit.RX_end = 1;					//���ս���
					sci_flag.bit.RX_valid = 0;					//�յ�������Ч
					sci_flag.bit.Reply = 0;						//��������Ӧ��
					RXbuffer.pcurrent = RXbuffer.paddress;
					sci_data.sci_state = RECEIVE_DATA_SAVE;
					return;
				}
				else
				{
					sci_flag.bit.Reply=1;						//�ӻ�����Ӧ��
				}
				RXbuffer.pcurrent = RXbuffer.paddress;			//������һ�����ݺ�ָ��ص���ʼ��ַ
				sci_flag.bit.RX_end = 1;						//�������ݴ������
				sci_flag.bit.RX_valid = 1;						//����������Ч
				sci_data.sci_state = RECEIVE_DATA_SAVE;
			}
			if((RXbuffer.pcurrent > (RXbuffer.paddress+DataLength_Max_MOD))
			||(SXbuffer.pcurrent > (SXbuffer.paddress+2030)))	//120����δ�������ݣ�SCI��λ
			{
				sci_flag.bit.RX_end = 1;						//���ս���
				sci_flag.bit.RX_valid = 0;						//�յ�������Ч
				sci_flag.bit.Reply = 0;							//��������Ӧ��
				//init_sci_data();					
				SCIBRST_init();									//��λSCI������
				sci_data.sci_state = RECEIVE_DATA_SAVE;
			}
		}
	}
}
//-----------------------------------------------------------------------------
//  CRC校验计算
//  反馈CRC校验值
//  入口：（数据其始地址，数据长度）
//-----------------------------------------------------------------------------
Uint16 CRC_checkcode(Uint16 * data,Uint16 length){
	static volatile Uint16 i;
	Uint16	crc_reg=0xFFFF;	
	while(length--)
	{
		crc_reg^=* data++;
		for(i=0;i<8;i++)
		{
			if(crc_reg & 0x01)
			{
				crc_reg=(crc_reg>>1)^0xA001;
			}else
			{
				crc_reg=(crc_reg>>1);
			}
		}
	}
	return crc_reg;
}
//-----------------------------------------------------------------------------
// File:     void SCITransmit(void)
// 功能描述：发送数据子程序
// 调用模块：无
// TXbuffer.Data_length----------->发送字节总数
//-----------------------------------------------------------------------------				
void SCITransmit_MOD(void)
{	
	Uint16	temp;
	Uint16	checkcode_cal=0;
	
	if((!sci_flag.bit.TX_end)						//�����췢�����
			&& sci_flag.bit.TX_data_ready			//��������׼����
			&& sci_flag.bit.Reply) 					//��ҪDSPӦ��
	{
		checkcode_cal = 0;
		TXbuffer.pcurrent = TXbuffer.paddress;	//ָ���ʼλ��
		// ASCII
		if(memSCIPTL<6)
		{
			* TXbuffer.pcurrent++ = 0x003A;			//��ʼ�ַ�
			temp = sci_data.TX_data_number;			//����������Ч�ֽ���
			while(temp--)
			{
				* TXbuffer.pcurrent++ = HEX_ASCII(((* SXbuffer.pcurrent)>>4) & 0x0F);//�������ݴ��뷢�ͻ�����
				* TXbuffer.pcurrent++ = HEX_ASCII((* SXbuffer.pcurrent) & 0x0F);	//�������ݴ��뷢�ͻ�����
				checkcode_cal += (* SXbuffer.pcurrent) & 0x00FF;
				SXbuffer.pcurrent++;
				checkcode_cal &= 0x00FF;
			}
			checkcode_cal = (0x1000-checkcode_cal) & 0x00FF;				//LRC У��
			* TXbuffer.pcurrent++ = HEX_ASCII((checkcode_cal>>4) & 0x0F);	//�ȴ��λ
			* TXbuffer.pcurrent++ = HEX_ASCII(checkcode_cal & 0x0F);
			* TXbuffer.pcurrent++ = 0x0D;
			* TXbuffer.pcurrent++ = 0x0A;
			TXbuffer.data_length = (sci_data.TX_data_number<<1)+5;			//�ܵķ����ֽ���
		}
		// RTU
		else
		{
			temp = sci_data.TX_data_number;									//����������Ч�ֽ���
			while(temp--)
			{
				* TXbuffer.pcurrent++ = (* SXbuffer.pcurrent & 0x0FF);		//�������ݴ��뷢�ͻ�����
				SXbuffer.pcurrent++;
			}
			temp = CRC_checkcode(TXbuffer.paddress,sci_data.TX_data_number);
			* TXbuffer.pcurrent++ = temp & 0xFF;
			* TXbuffer.pcurrent++ = (temp>>8) & 0xFF;
			TXbuffer.data_length = sci_data.TX_data_number + 2;
		}
		//���ͻ�����ָ��,�˴�ŷ��͵���Ч�ֽ�������8λ��ǰ����У���루��8λ��
		TXbuffer.pcurrent=TXbuffer.paddress;
		sci_flag.bit.TX_end=1;				//�����췢��δ���
		TXbuffer.Hst_data_wrtten=0;			//DSP��д������������BYTE����
	}	

	if((sci_flag.bit.TX_end == 0) && ScicRegs.SCICTL2.bit.TXEMPTY && ScicRegs.SCICTL2.bit.TXRDY)
	{
		asm(" rpt #31 ||	nop");
		HVD33Rxd();
		asm(" rpt #31 ||	nop");
	}

	if(sci_flag.bit.TX_end							//������
		&& sci_flag.bit.Reply)						//��λ����Ҫ�ش�
	{
		HVD33Txd();
		asm(" rpt #31 ||	nop");
		if(WriteToSCIB_MOD(&TXbuffer))				//д���ͻ��������ݵ�SCI
		{
			//HVD33Rxd();
			sci_flag.bit.TX_end = 0;				//�����������
			sci_flag.bit.TX_data_ready = 0;			//��������δ׼����
			sci_flag.bit.Reply = 0;					//ȡ����λ����Ҫ�ش�
			TXbuffer.pcurrent = TXbuffer.paddress;	//�ָ����ͻ�����ָ��
		}
	}
	
}		
//-----------------------------------------------------------------------------
// File:     void SCIReceive(void)
// 功能描述：解析数据子程序
// 创建人：YJ
//-------------------------------
//-----------------------------------------------------------------------------
void SCIReceive_MOD(void)
{		
	if( sci_flag.bit.RX_end 				// 接收数据结束
		&& sci_int_flag.bit.RX_type			// 232回应标志־
		&& sci_flag.bit.RX_valid)			// 接收数据有效
	{	

		sci_flag.bit.RX_end =0;
		sci_int_flag.bit.RX_type=0;
		sci_flag.bit.RX_valid=0;
		sci_flag.bit.HOST_undone=0;//δִ�б�־����

		switch(sci_data.RX_cmd)
		{
//--------------------------------------------------------
//	读取N个字
//--------------------------------------------------------
			case 0x0003:	CMD03();
							break;
//--------------------------------------------------------
//	写一个字
//--------------------------------------------------------
			case 0x0006:	CMD06();
							break;
//--------------------------------------------------------
//	写多个字
//--------------------------------------------------------
			case 0x0010:	CMD10();
							break;	
//--------------------------------------------------------
//	读取驱动器信息数据
//--------------------------------------------------------
			case 0x002B:	CMD2B();
							break;	
		
			default:		CMD00();
							break;
		}
	}
}
//----------------------------------------------------------------
//	 
//----------------------------------------------------------------
void CMD00(void)
{
	//�����������
	sci_flag.bit.HOST_undone=1;
	sci_data.Error_mesg=01;
	SXbuffer.pcurrent =SXbuffer.paddress;			//���ݴ洢�ռ��׵�ַ
	* SXbuffer.pcurrent++=sci_data.Address;			//�ֺŵ�ַ
	* SXbuffer.pcurrent++=sci_data.RX_cmd+0x80;
	sci_data.TX_cmd=sci_data.RX_cmd+0x80;
	* SXbuffer.pcurrent++=sci_data.Error_mesg;		//�����־
	sci_data.TX_data_number=3;
	sci_flag.bit.TX_data_ready=1;					//��������׼�����
	SXbuffer.pcurrent =SXbuffer.paddress;			//���ݴ洢�ռ��׵�ַ
}
//----------------------------------------------------------------
//	 ���������жϵ�ַ
//----------------------------------------------------------------
void CMD03(void)
{
	Uint16 Datanums,DataFirstadd,DataEndadd,PnFactNum;
	Uint16 * addrtemp, * tmpPointer,pdata;	
	int16 temp;
	//------------------------------------------------------------
	//�Խ��������
	//------------------------------------------------------------	
	addrtemp = sci_data.RX_data_ADR;
	pdata=0;
	pdata += ((* addrtemp++)<<8) & 0xFF00;				//��ʼ���ϵ�ַ
	pdata += ((* addrtemp++) & 0x0FF);	
	
	DataFirstadd= pdata;								//��ʼ���ϵ�ַ
	//�����ݵ�ַ���жϣ���ʼ��ַ��
	Datanums = (((* addrtemp++)<<8) & 0xFF00);			//��ȡ���ݸ������֣�
	Datanums += ((* addrtemp++) & 0x0FF);				//

	DataEndadd = DataFirstadd + Datanums - 1;			//����ֹ��ַ��
	
	//�����ݵ�ַ�ж�
	/*debug
	if(DataFirstadd > 0x0200 && DataFirstadd < 0x1000)					//��Ч��ַ
	{
		sci_flag.bit.HOST_undone = 1;
		sci_data.Error_mesg = 2;
	}
	else if(DataFirstadd>0x3710)
	{
		sci_flag.bit.HOST_undone = 1;
		sci_data.Error_mesg = 2;
	}
	*/
	
	if(Datanums > 100)										//ֻ��������ȡ100�����ڵ���
	{
		sci_flag.bit.HOST_undone = 1;
		sci_data.Error_mesg = 2;
	}
	if(DataEndadd <= PnMaxNum)														//������
	{
		PnFactNum = PnSwitch(DataEndadd);
		if(PnFactNum == 0xFFFF)
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 2;
		}
		PnFactNum = PnFactNum + 1 - Datanums;
	}

	addrtemp = sci_data.RX_data_ADR;						//����������Ч��ַ
	SXbuffer.pcurrent = SXbuffer.paddress;				//���ݴ洢�ռ��׵�ַ
	SXbuffer.data_length = Datanums;							//���͵���Ч��������
	* SXbuffer.pcurrent++ = sci_data.Address;				//�ֺŵ�ַ
	if(!sci_flag.bit.HOST_undone)
	{
		* SXbuffer.pcurrent++=sci_data.RX_cmd;
		sci_data.TX_cmd=sci_data.RX_cmd;
		* SXbuffer.pcurrent++=SXbuffer.data_length<<1;	//��Ӧ���������ֽ���
		sci_data.TX_data_number = (SXbuffer.data_length<<1) + 3;		//���͵���Ч�ֽ���
			
		if(DataEndadd <= PnMaxNum)														//������
		{
			while(Datanums--)															//����Ч����
			{
				* SXbuffer.pcurrent++ = ((Pn[PnFactNum])>>8) & 0x00FF;					//��д��λ
				* SXbuffer.pcurrent++ = (Pn[PnFactNum]) & 0x00FF;						//��д��λ
				pdata++;
				PnFactNum++;
			}
		}
		else if(DataFirstadd >= 0x07F1 && DataEndadd <= 0x07FE)							//��ʷ���� + ADƫ����
		{
			while(Datanums--)															//����Ч����
			{
				if(pdata >= 0x07F1 && pdata <= 0x07FA)									// ��ʷ����
				{
					* SXbuffer.pcurrent++ = ((Pn[Fn000Addr + pdata - 0x07F1])>>8) & 0x00FF;	//��д��λ
					* SXbuffer.pcurrent++ = (Pn[Fn000Addr + pdata - 0x07F1]) & 0x00FF;		//��д��λ
				}
				else if(pdata >= 0x07FB && pdata <= 0x07FE)									// ADƫ����
				{
					* SXbuffer.pcurrent++ = ((Pn[Fn004Addr + pdata - 0x07FB])>>8) & 0x00FF;	//��д��λ
					* SXbuffer.pcurrent++ = (Pn[Fn004Addr + pdata - 0x07FB]) & 0x00FF;		//��д��λ
				}
				pdata++;
			}
		}
		else if(DataFirstadd >= 0x0806 && DataEndadd <= 0x0818)					// Un
		{
			while(Datanums--)													//����Ч����
			{
				if(pdata == 0x0817)
				{
					* SXbuffer.pcurrent++ = ((alarmno)>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = (alarmno) & 0x00FF;							//��д��λ
					pdata++;
				}
				else if (pdata == 0x0818)
				{
					* SXbuffer.pcurrent++ = ((Un[17])>>8) & 0x00FF;		//��д��λ
					* SXbuffer.pcurrent++ = (Un[17]) & 0x00FF;			//��д��λ
					pdata++;				
				}
				else
				{
					* SXbuffer.pcurrent++ = ((Un[pdata - 0x0806])>>8) & 0x00FF;		//��д��λ
					* SXbuffer.pcurrent++ = (Un[pdata - 0x0806]) & 0x00FF;			//��д��λ
					pdata++;
				}				
			}
		}
		else if(DataFirstadd >= 0x0850 && DataEndadd <= 0x0852)					// Gear
		{
			while(Datanums--)													//����Ч����
			{
				if(pdata == MemgearB)
				{
					* SXbuffer.pcurrent++ = ((Bgear)>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = (Bgear) & 0x00FF;						//��д��λ
				}
				else if(pdata == MemgearA1)
				{
					* SXbuffer.pcurrent++ = ((memAgear1)>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = (memAgear1) & 0x00FF;							//��д��λ
				}
				else if(pdata == MemgearA2)
				{
					* SXbuffer.pcurrent++ = ((memAgear2)>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = (memAgear2) & 0x00FF;							//��д��λ
				}	
				pdata++;			
			}
		}
		else if(DataFirstadd >= 0x0900 && DataEndadd <= 0x090A)					// 0900~0904
		{
			while(Datanums--)													//����Ч����
			{				
				if(pdata == MODBUSinputADD)
				{
					* SXbuffer.pcurrent++ = (Un[Un_Input]>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = Un[Un_Input] & 0x00FF;				//��д��λ
				}
				else if(pdata == DRVstatusADD)
				{
					DrvStatus.bit.ALM = state_flag2.bit.DispAlm;
					DrvStatus.bit.SvReady = state_flag2.bit.SvReady;
					DrvStatus.bit.svstate = servo_state.bit.svstate;					
					DrvStatus.bit.coin = servo_state.bit.coin;
					DrvStatus.bit.mainpower = servo_state.bit.mainpower;					
					if(StateBit[0] & StateBit0_InPuls)
					{
						DrvStatus.bit.ovVref_inPuls = 1;
					}
					else
					{
						DrvStatus.bit.ovVref_inPuls = 0;
					}
					if(StateBit[0] & StateBit0_InCLR)
					{
						DrvStatus.bit.TCR_CLRerr = 1;
					}
					else
					{
						DrvStatus.bit.TCR_CLRerr = 0;
					}
					DrvStatus.bit.TGON = state_flag2.bit.TGONvalid;
					DrvStatus.bit.n_ot = servo_state.bit.n_ot;
					DrvStatus.bit.p_ot = servo_state.bit.p_ot;
					
					* SXbuffer.pcurrent++ = (DrvStatus.all>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = DrvStatus.all & 0x00FF;					//��д��λ
				}
				else if(pdata == PhaseADD)
				{
					* SXbuffer.pcurrent++ = (ComminitPhase>>8) & 0x00FF;		//��д��λ
					* SXbuffer.pcurrent++ = ComminitPhase & 0x00FF;				//��д��λ
				}
				else if(pdata == InertiaADD)
				{
					* SXbuffer.pcurrent++ = (CommInertia>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = CommInertia & 0x00FF;				//��д��λ
				}
				else if(pdata == DRVruntimeADD)
				{					
					* SXbuffer.pcurrent++ = 0;										//��д��λ
					* SXbuffer.pcurrent++ = 0;										//��д��λ
				}
				else if(pdata == 0x0905)
				                                {
				                                    * SXbuffer.pcurrent++ = 0;                                      //��д��λ
				                                    * SXbuffer.pcurrent++ = int_flagx.bit.SciFindInitPhase;                                      //��д��λ
				                                }
				else if(pdata == 0x0906)
				                                  {
				                                                    * SXbuffer.pcurrent++ = 0;                                      //��д��λ
				                                                    * SXbuffer.pcurrent++ = alarmno;//int_flagx.bit.SciFindInitPhase;                                      //��д��λ
				                                 }
				else if(pdata == 0x0907)
				                                  {
				                                                     * SXbuffer.pcurrent++ =(PowerOnInitAng&0xff00)>>8;                                      //��д��λ
				                                                     * SXbuffer.pcurrent++ =PowerOnInitAng&0x00ff;                                     //��д��λ
				                                 }
				else if(pdata == 0x0908)
				                     {
				                                   * SXbuffer.pcurrent++ =(AsSpeed&0xff00)>>8;                                      //��д��λ
				                                   * SXbuffer.pcurrent++ =AsSpeed&0x00ff;                                     //��д��λ
				                     }
				pdata++;
			}
		}
		else if(DataFirstadd >= 0x090D && DataEndadd <= 0x0925)						// 090D~0925
		{
			while(Datanums--)														//����Ч����
			{
				if(pdata == UserPINADD)
				{
					* SXbuffer.pcurrent++ = (Commpassword>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = Commpassword & 0x00FF;				//��д��λ
				}
				else if(pdata == SoftDSPVarADD)
				{
					* SXbuffer.pcurrent++ = (Edition.all>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = Edition.all & 0x00FF;				//��д��λ
				}
				else if(pdata == SoftPLDVarADD)
				{
					* SXbuffer.pcurrent++ = (CpldEdition>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = CpldEdition & 0x00FF;				//��д��λ
				}
				else if(pdata == SoftMODVarADD)
				{
					* SXbuffer.pcurrent++ = (DP100Edition>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = DP100Edition & 0x00FF;				//��д��λ
				}
				else if(pdata == speedrminADD)
				{
					temp = (int16)Vfactcomm;
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;						//��д��λ
				}
				else if(pdata == VCMDrminADD)
				{
					temp = (int16)VCMDcomm;
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;						//��д��λ
				}
				else if(pdata == TCMDADD)
				{
					* SXbuffer.pcurrent++ = (TCMD>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = TCMD & 0x00FF;						//��д��λ
				}
				else if(pdata == TnADD)
				{
					* SXbuffer.pcurrent++ = (Tn>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = Tn & 0x00FF;						//��д��λ
				}
				else if(pdata == inputADD)
				{
					* SXbuffer.pcurrent++ = (input_state.all>>8) & 0x00FF;		//��д��λ
					* SXbuffer.pcurrent++ = input_state.all & 0x00FF;			//��д��λ
				}
				else if(pdata == outMemADD)
				{
					* SXbuffer.pcurrent++ = ((out_state.all & 0x0F)>>8) & 0x0FF;//��д��λ
					* SXbuffer.pcurrent++ = (out_state.all & 0x0F) & 0x0FF;		//��д��λ
				}
				else if(pdata == CurPos1ADD)
				{
					temp = (int16)(UnCurPos%10000);
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;						//��д��λ
				}
				else if(pdata == CurPosADD)
				{
					temp = (int16)(UnCurPos/10000);
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;						//��д��λ
				}
				else if(pdata == Ek4ADD)	// ƫ�����������λ
				{
					temp = (int16)(Ek & 0xFFFF);
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;						//��д��λ
				}
				else if(pdata == Ek3ADD)	// ƫ�����������λ
				{
					temp = (int16)((Ek>>16) & 0xFFFF);
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;						//��д��λ
				}
				else if(pdata == Pg1ADD)	// ���������λ
				{
					* SXbuffer.pcurrent++ = (UnPg_count>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = UnPg_count & 0x00FF;				//��д��λ
				}
				else if(pdata == PgADD)		// ���������λ
				{
					* SXbuffer.pcurrent++ = (UnPg_roate>>8) & 0x00FF;			//��д��λ
					* SXbuffer.pcurrent++ = UnPg_roate & 0x00FF;				//��д��λ
				}
				else if(pdata == IuADD)
				{
					* SXbuffer.pcurrent++ = (Iu>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = Iu & 0x00FF;						//��д��λ
				}
				else if(pdata == IvADD)
				{
					* SXbuffer.pcurrent++ = (Iv>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = Iv & 0x00FF;						//��д��λ
				}
				else if(pdata == IdrADD)
				{
					* SXbuffer.pcurrent++ = (Idr>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = Idr & 0x00FF;						//��д��λ
				}
				else if(pdata == IdADD)
				{
					* SXbuffer.pcurrent++ = (Id>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = Id & 0x00FF;						//��д��λ
				}
				else if(pdata == IqrADD)
				{
					* SXbuffer.pcurrent++ = (Iqr>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = Iqr & 0x00FF;						//��д��λ
				}
				else if(pdata == IqADD)
				{
					* SXbuffer.pcurrent++ = (Iq>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = Iq & 0x00FF;						//��д��λ
				}
				else if(pdata == UdADD)
				{
					* SXbuffer.pcurrent++ = (Ud>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = Ud & 0x00FF;						//��д��λ
				}
				else if(pdata == UqADD)
				{
					* SXbuffer.pcurrent++ = (Uq>>8) & 0x00FF;					//��λ
					* SXbuffer.pcurrent++ = Uq & 0x00FF;						//��д��λ
				}
				else if(pdata == speedADD)
				{
					temp = (int16)speed;
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;						//��д��λ
				}
				else if(pdata == VCMDADD)
				{
					temp = (int16)VCMD;
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;						//��д��λ
				}
				pdata++;
			}
		}
		else if(DataFirstadd >= 0x1000 && DataEndadd <= 0x1007)					// 1000~1006
		{
			while(Datanums--)													//����Ч����
			{
				if(pdata == DrvTypeADD)
				{
					temp = 200 + memDriveSel * 10 + memMotorSel;
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;
				}
				else if(pdata == KjPnnumADD)
				{
					temp = 704;
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;
				}
				else if(pdata == DrvFnStatusADD)
				{//
					* SXbuffer.pcurrent++ = (sci_oper_status.all>>8) & 0x00FF;	//��д��λ
					* SXbuffer.pcurrent++ = sci_oper_status.all & 0x00FF;
				}
				else if(pdata == HandOpPnnumADD)
				{
					temp = sci_data.SCIHandCurPnnum;
					* SXbuffer.pcurrent++ = (temp>>8) & 0x00FF;					//��д��λ
					* SXbuffer.pcurrent++ = temp & 0x00FF;
				}
				else if(pdata == HandPnMaxADD)
				{
					temp = PnSwitch(sci_data.SCIHandCurPnnum);
					if (sci_data.SCIHandCurPnnum >= 401 && sci_data.SCIHandCurPnnum <= 405)
					{
						* SXbuffer.pcurrent++ = (abl_load>>8) & 0x00FF;	//��д��λ
					    * SXbuffer.pcurrent++ = abl_load & 0x00FF;
					}
					else
					{
						* SXbuffer.pcurrent++ = (PnAttribute[temp][2]>>8) & 0x00FF;	//��д��λ
					    * SXbuffer.pcurrent++ = PnAttribute[temp][2] & 0x00FF;
					}
				}
				else if(pdata == HandPnMinADD)
				{
					temp = PnSwitch(sci_data.SCIHandCurPnnum);
					* SXbuffer.pcurrent++ = (PnAttribute[temp][1]>>8) & 0x00FF;	//��д��λ
					* SXbuffer.pcurrent++ = PnAttribute[temp][1] & 0x00FF;
				}
				else if(pdata == HandPnDataADD)
				{
					temp = PnSwitch(sci_data.SCIHandCurPnnum);
					* SXbuffer.pcurrent++ = (Pn[temp]>>8) & 0x00FF;				//��д��λ
					* SXbuffer.pcurrent++ = Pn[temp] & 0x00FF;
				}
				else// if(pdata == HandPnattributeADD)
				{					
					temp = PnSwitch(sci_data.SCIHandCurPnnum);
					* SXbuffer.pcurrent++ = (PnAttribute[temp][0]>>8) & 0x00FF;	//��д��λ
					* SXbuffer.pcurrent++ = PnAttribute[temp][0] & 0x00FF;
				}
				pdata++;
			}
		}
		else if(DataFirstadd >= 0x1010 && DataEndadd <= 0x1012)					// 1010~1012
		{
			while(Datanums--)													//����Ч����
			{				
				if(pdata == RotateADD)
				{					
					* SXbuffer.pcurrent++ = (Rotate>>8) & 0x00FF;	//��д��λ
					* SXbuffer.pcurrent++ = Rotate & 0x00FF;
				}
				else if(pdata == SinglePos_ADDL)
				{					
					* SXbuffer.pcurrent++ = (singlePos>>8) & 0x00FF;	//��д��λ
					* SXbuffer.pcurrent++ = singlePos & 0x00FF;
				}
				else// if(pdata == SinglePos_ADDH)
				{					
					* SXbuffer.pcurrent++ = (singlePos>>24) & 0x00FF;	//��д��λ
					* SXbuffer.pcurrent++ = (singlePos>>16) & 0x00FF;
				}
				pdata++;
			}
		}
		//test
		else if(DataFirstadd >= TESTWave && DataEndadd <= TESTMODE+5)				
		{
			tmpPointer = (Uint16 *)&TestPara;
			temp=DataFirstadd-TESTWave;
			while(Datanums--)													
			{
				* SXbuffer.pcurrent++ = ((*(tmpPointer+temp))>>8) & 0x00FF;
				* SXbuffer.pcurrent++ = (*(tmpPointer+temp)) & 0x00FF;
				tmpPointer++;				
			}
		}
        else if((DataFirstadd >= AdResultADD && DataEndadd <= AdResultADD+9) && (Commpassword == UserPassword))	//У����ʮ�����ݶ�ȡ,��Ҫ�û������Ȩ�޲ſɲ���
		{
			while(Datanums--)													
			{
				* SXbuffer.pcurrent++ = (Pn[SpdAddr+pdata-AdResultADD]>>8) & 0x00FF;
				* SXbuffer.pcurrent++ = (Pn[SpdAddr+pdata-AdResultADD]) & 0x00FF;
				pdata++;				
			}
		}
		else if(DataFirstadd >= 0x2000 && DataEndadd <= 0x3800)			// ��ͼ���� 2000~3800
		{
			while(Datanums--)											//����Ч����
			{
				tmpPointer = (Uint16 *)(SAMPLE_BUF + (pdata - 0x2000));
				* SXbuffer.pcurrent++ = ((*tmpPointer)>>8) & 0x00FF;	//��д��λ
				* SXbuffer.pcurrent++ = (*tmpPointer) & 0x00FF;			//��д��λ
				pdata++;
			}
		}
		else	// �Ƿ���ַ
		{// debug
			sci_flag.bit.HOST_undone = 1;
			* --SXbuffer.pcurrent;		// �ֽڳ���
			* --SXbuffer.pcurrent;		// ����
			* SXbuffer.pcurrent++ = sci_data.RX_cmd + 0x80;
			sci_data.TX_cmd = sci_data.RX_cmd + 0x80;
			* SXbuffer.pcurrent++ = 2;									//�����־
			sci_data.TX_data_number = 3;
		}		
	}
	else
	{
		* SXbuffer.pcurrent++ = sci_data.RX_cmd + 0x80;
		sci_data.TX_cmd = sci_data.RX_cmd + 0x80;
		* SXbuffer.pcurrent++ = sci_data.Error_mesg;					//�����־
		sci_data.TX_data_number = 3;
	}	
	//------------------------------------------------------------
	sci_flag.bit.TX_data_ready = 1;										//��������׼�����
	SXbuffer.pcurrent =SXbuffer.paddress;								//���ݴ洢�ռ��׵�ַ
}
//----------------------------------------------------------------
//	 //  写操作：判断地址、判断数据大小��ַ1B+������0x16 1B+RegaddrH1B+RegaddrL 1B+д�Ĵ�������1B+����H1B+���ݵ�1B+��������������+2B CRC
//----------------------------------------------------------------
void CMD06(void)
{
	Uint16	PnFactNum,WrDataEdit,WrDataAdd,temp,i;
	Uint16	* addrtemp,* tmpPointer;
	//------------------------------------------------------------
	// 对接收命令处理
	//------------------------------------------------------------
	addrtemp=sci_data.RX_data_ADR;
	temp = 0;
	temp +=(((*addrtemp++)<<8) & 0xFF00);			//数据地址ݵ�ַ
	temp +=((*addrtemp++) & 0x0FF);					//
	
	WrDataAdd = temp & 0xFFFF;						//要写的地址ַ

	WrDataEdit = (((*addrtemp++)<<8) & 0xFF00);		//要写的数据
	WrDataEdit +=((*addrtemp++) & 0x0FF);			
	if(WrDataAdd <= PnMaxNum)						//参数区
	{
		PnFactNum = PnSwitch(WrDataAdd);
		if((PnFactNum == 0xFFFF) || ((WrDataAdd >= 800) && (Commpassword != UserPassword) && (WrDataAdd != 840)))
		{// �Ƿ���ַ �� Ȩ�޲���
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 2;
		}
		else
		{
			if(WrDataEdit != Pn[PnFactNum])
			{
				if(Para_Maxmin(PnFactNum,WrDataEdit))
				{
					sci_flag.bit.HOST_undone=1;					//数据操作不能完成
					sci_data.Error_mesg=3;
				}
				else//���ݴ��
				{
					Pn[SumAddr] -= Pn[PnFactNum];	// 									
					CheckSum -= Pn[PnFactNum];
					Pn[PnFactNum] = WrDataEdit;
					Pn[SumAddr] += Pn[PnFactNum];
					CheckSum += Pn[PnFactNum];
				 if((WrDataAdd!=304)&&(WrDataAdd!=306))
				  //   if(WrDataAdd!=304)
				 {
					SPI_WriteFRAM(PnFactNum,&Pn[PnFactNum],1);
					SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
				 }
					sci_flag.bit.HOST_undone = 0;				////数据操作完成
				}								
			}
		}
	}
	else if(WrDataAdd == ZeroSpdADD)
	{
		Pn[SumAddr] -=  spdoffset;
		CheckSum -=  spdoffset;
		spdoffset = WrDataEdit;
		Pn[Fn004Addr] = spdoffset;
		Pn[SumAddr] +=  spdoffset;
		CheckSum +=  spdoffset;
		SPI_WriteFRAM(Fn004Addr,&Pn[Fn004Addr],1);
		SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		sci_flag.bit.HOST_undone = 0;				//数据操作完成
	}
	else if(WrDataAdd == ZeroTcrADD)
	{
		Pn[SumAddr] -=  TCRoffset;
		CheckSum -=  TCRoffset;
		TCRoffset = WrDataEdit;
		Pn[Fn004Addr+1] = TCRoffset;
		Pn[SumAddr] +=  TCRoffset;
		CheckSum += TCRoffset;
		SPI_WriteFRAM(Fn004Addr+1,&Pn[Fn004Addr+1],1);
		SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		sci_flag.bit.HOST_undone = 0;				//���ݲ������
	}
	else if(WrDataAdd == MemgearB)
	{
		if((WrDataEdit!=0) && (memBusSel!=3))
		{
			Bgear = WrDataEdit;
			sci_flag.bit.HOST_undone = 0;				//���ݲ������
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}
	else if(WrDataAdd == MemgearA1)
	{
		if((WrDataEdit!=0) && (memBusSel!=3))
		{
			memAgear1 = WrDataEdit;
			sci_flag.bit.HOST_undone = 0;				//���ݲ������
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}
	else if(WrDataAdd == MemgearA2)
	{
		if((WrDataEdit!=0) && (memBusSel!=3))
		{
			memAgear2 = WrDataEdit;
			sci_flag.bit.HOST_undone = 0;				//���ݲ������
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}
	else if(WrDataAdd == MODBUSinputADD)						//
	{
		sci_data.SCIInput = WrDataEdit;
		sci_flag.bit.HOST_undone = 0;							//���ݲ������
	}
	else if(WrDataAdd == UserPINADD)							//
	{
		Commpassword = WrDataEdit;
		sci_flag.bit.HOST_undone = 0;							//���ݲ������
	}
	else if(WrDataAdd == HandOpPnnumADD)						//
	{
		if(WrDataEdit <= PnMaxNum)						//������
		{
			PnFactNum = PnSwitch(WrDataEdit);
			if(PnFactNum == 0xFFFF)
			{// �Ƿ���ַ
				sci_flag.bit.HOST_undone = 1;
				sci_data.Error_mesg = 3;
			}
			else
			{
				sci_data.SCIHandCurPnnum = WrDataEdit;
				sci_flag.bit.HOST_undone = 0;
			}
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;					//���ݲ����������
			sci_data.Error_mesg = 3;
		}
	}
	else if((WrDataAdd >= CLRALMLOGcom) && (WrDataAdd <= JDPOSHaltcom))						//
	{
		if(WrDataAdd == CLRALMLOGcom)
		{
			if(WrDataEdit == 1)
			{
				for(i = 0; i < 10; i++)
				{
					Pn[SumAddr] -= Pn[Fn000Addr + i];
					CheckSum -= Pn[Fn000Addr + i];
					Pn[Fn000Addr + i] = 0;					
				}
				SPI_WriteFRAM(Fn000Addr,&Pn[Fn000Addr],10);
				SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == CLRALMcom)
		{
			if(WrDataEdit == 1)
			{
				// �������
				state_flag2.bit.HaveAlm = 0;
				// ���(��ʾ������־)
				state_flag2.bit.DispAlm = 0;
				// ����(��������ı�־)
				state_flag2.bit.ClrAlm = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == JOGSoncom)
		{
			if(WrDataEdit == 0)
			{
				asm("	SETC	INTM");
				sci_oper_flag.bit.sciJogson = 0;
				asm("	CLRC	INTM");
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if((WrDataEdit == 1) && (state_flag2.bit.SvReady) && (!key_flag1.bit.JOGrun))
			{
				asm("	SETC	INTM");
				sci_oper_flag.bit.sciJogson = 1;
				asm("	CLRC	INTM");
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == JOGPruncom)
		{
			if(WrDataEdit == 0)
			{
				key_flag1.bit.JOGpos = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if((WrDataEdit == 1) && (state_flag2.bit.SvReady) && (!key_flag1.bit.JOGrun))
			{
				key_flag1.bit.JOGpos = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == JOGNruncom)
		{
			if(WrDataEdit == 0)
			{
				key_flag1.bit.JOGneg = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if((WrDataEdit == 1) && (state_flag2.bit.SvReady) && (!key_flag1.bit.JOGrun))
			{
				key_flag1.bit.JOGneg = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == JDPOSJOGpcom)
		{
			if(WrDataEdit == 0)
			{
				sci_oper_flag.bit.sci_pos_jogp = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if((WrDataEdit == 1) && int_flagx.bit.pos_jogen)
			{
				sci_oper_flag.bit.sci_pos_jogp = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == JDPOSJOGncom)
		{
			if(WrDataEdit == 0)
			{
				sci_oper_flag.bit.sci_pos_jogn = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if((WrDataEdit == 1) && int_flagx.bit.pos_jogen)
			{
				sci_oper_flag.bit.sci_pos_jogn = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == JDPOSHaltcom)
		{
			if(WrDataEdit == 0)
			{
				sci_oper_flag.bit.sci_pos_halt = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if(WrDataEdit == 1)
			{
				sci_oper_flag.bit.sci_pos_halt = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
	}
	else if((WrDataAdd >= PHASEstartcom) && (WrDataAdd <= Inertiastartcom))						//
	{
		if(WrDataAdd == PHASEstartcom)
		{
			if(WrDataEdit == 0)
			{
				sci_oper_flag.bit.sciPhaseSON = 0;
				sci_oper_status.bit.sciPhaseStatus = 0;
				asm("	SETC	INTM");				
				int_flagx.bit.SciFindInitPhase = 0;
				int_flag3.bit.HaveFindInitPos = 0;
				int_flag3.bit.pass360 = 0;
				asm("	CLRC	INTM");				
				sci_flag.bit.HOST_undone = 0;						//���ݲ������
			}
			else if(WrDataEdit == 1)
			{
				if(state_flag2.bit.Son || (!state_flag2.bit.SvReady) || state_flag2.bit.HandPhase )
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
				else
				{
					sci_oper_flag.bit.sciPhaseSON = 1;
					sci_oper_status.bit.sciPhaseStatus = 1;
					asm("	SETC	INTM");					
					int_flagx.bit.SciFindInitPhase = 0;
					int_flag3.bit.HaveFindInitPos = 0;
					int_flag3.bit.pass360 = 0;
					asm("	CLRC	INTM");
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;						//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == Inertiastartcom)
		{
			if(WrDataEdit == 0)
			{				
				sci_oper_status.bit.sciInertiaStatus = 0;
				sci_oper_flag.bit.sciStartInertia = 0;
				asm("	SETC	INTM");	
				int_flag3.bit.J_initia = 0;
				int_flag3.bit.J_End = 0;
				asm("	CLRC	INTM");
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if((WrDataEdit == 1) && state_flag2.bit.SvReady && (!state_flag2.bit.Son))
			{
				sci_oper_status.bit.sciInertiaStatus = 1;
				sci_oper_flag.bit.sciStartInertia = 1;
				asm("	SETC	INTM");	
				int_flag3.bit.J_initia = 0;
				int_flag3.bit.J_End = 0;
				asm("	CLRC	INTM");	
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
	}
	else if((WrDataAdd >= ClrEncErrcom) && (WrDataAdd <= ClrEncMuticom))
	{
		if(WrDataAdd == ClrEncErrcom)
		{
			if(WrDataEdit == 0)
			{
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if((WrDataEdit == 1) && !state_flag2.bit.Son && memCurLoop00.hex.hex00 == Encoder_Abs17bit)
			{
				asm("	SETC	INTM");
				int_flagx.bit.rstEncErr = 1;
				asm("	CLRC	INTM");
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == ClrEncMuticom)
		{
			if(WrDataEdit == 0)
			{
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if((WrDataEdit == 1) && !state_flag2.bit.Son && memCurLoop00.hex.hex00 == Encoder_Abs17bit)
			{
				asm("	SETC	INTM");
				int_flagx.bit.rstEncMuti = 1;
				asm("	CLRC	INTM");
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
	}
	else if((WrDataAdd >= Samplestartcom) && (WrDataAdd <= SampleTimecom))						//
	{
		if(WrDataAdd == Samplestartcom)
		{
			if(WrDataEdit == 0)
			{
				sci_oper_status.bit.sciSampleStatus = 0;
				sci_oper_flag.bit.sciStartSample = 0;
				sci_oper_flag.bit.SciSampleing = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
				wComm = SAMPLE_BUF;						// ������׵�ַ
			}
			else if(WrDataEdit == 1)
			{
				savecnt = 0;
				sci_oper_status.bit.sciSampleStatus = 1;
				sci_oper_flag.bit.sciStartSample = 1;
				sci_oper_flag.bit.SciSampleing = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
				wComm = SAMPLE_BUF;						// ������׵�ַ
				Save32bit[0]=0;	//clear test flag
				Save32bit[1]=0;
				Save32bit[2]=0;
				Save32bit[3]=0;
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
				wComm = SAMPLE_BUF;						// ������׵�ַ
			}
		}
		else if(WrDataAdd == SampleADD1com)
		{
			Uint32 scitemp;
			scitemp = SampleAddressMap(WrDataEdit);
			if(scitemp)
			{
				commvar1 = scitemp;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
				if(TestPara.Test_Mode && 
					((scitemp==(Uint32)(&UnPg_count)) || (scitemp==(Uint32)(& test_abs_position_more))))
				{
					Save32bit[0]=1;
					if (scitemp==(Uint32)(&UnPg_count))		//2012-02-24
					{
						commvar1 = (Uint32)(&UnPg);
					}
				}
				else	Save32bit[0]=0;

			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == SampleADD2com)
		{Uint32 scitemp;
			scitemp = SampleAddressMap(WrDataEdit);
			if(scitemp)
			{
				commvar2 = scitemp;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
				if(TestPara.Test_Mode && 
					((scitemp==(Uint32)(&UnPg_count)) || (scitemp==(Uint32)(& test_abs_position_more))))
				{
					Save32bit[1]=1;
					if (scitemp==(Uint32)(&UnPg_count))
					{
						commvar2 = (Uint32)(&UnPg);
					}
				}
				else	Save32bit[1]=0;
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == SampleTrigcom)
		{
			if(WrDataEdit > 3)
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
			else
			{
				sci_data.SCISampleTrig = WrDataEdit;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
		}
		else if(WrDataAdd == SampleTrigV1com)
		{
			sci_data.SCISampleTrigV1 = WrDataEdit;
			sci_flag.bit.HOST_undone = 0;					//���ݲ������
		}
		else if(WrDataAdd == SampleTrigV2com)
		{
			sci_data.SCISampleTrigV2 = WrDataEdit;
			sci_flag.bit.HOST_undone = 0;					//���ݲ������
		}
		else//SampleTimecom
		{
			sci_data.SCISampleTime = WrDataEdit;
			sci_flag.bit.HOST_undone = 0;					//���ݲ������
		}
	}
	else if((WrDataAdd >= ZeroSpdstartcom) && (WrDataAdd <= ZeroTcrstartcom))						//
	{
		if(WrDataAdd == ZeroSpdstartcom)
		{
			if(WrDataEdit == 0)
			{
				sci_oper_flag.bit.sciStartZeroSpd = 0;
				sci_oper_status.bit.sciZeroSpdStatus = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if(WrDataEdit == 1)
			{
				asm("	SETC	INTM");
				Timers.cntdone = 0;
				Sspd = 0;
				Stcr = 0;
				asm("	CLRC	INTM");
				sci_oper_flag.bit.sciStartZeroSpd = 1;
				sci_oper_status.bit.sciZeroSpdStatus = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == ZeroTcrstartcom)
		{
			if(WrDataEdit == 0)
			{
				sci_oper_flag.bit.sciStartZeroTcr = 0;
				sci_oper_status.bit.sciZeroTcrStatus = 0;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if(WrDataEdit == 1)
			{
				asm("	SETC	INTM");
				Timers.cntdone = 0;
				Sspd = 0;
				Stcr = 0;
				asm("	CLRC	INTM");
				sci_oper_flag.bit.sciStartZeroTcr = 1;
				sci_oper_status.bit.sciZeroTcrStatus = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
	}
	else if((WrDataAdd >= LoadDefaultcom) && (WrDataAdd <= RESETDSPcom))						//
	{
		if(WrDataAdd == LoadDefaultcom)
		{
			if((WrDataEdit == 1) && (!state_flag2.bit.Son) && !((MainMenuPage == 3) && (FnNum == LoadDefault)))
			{
				ResumeIndex = 0;
				sci_oper_flag.bit.SciLoadDefault = 1;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
		else if(WrDataAdd == RESETDSPcom)
		{
			if(WrDataEdit == 1)
			{
				EALLOW;
    			SysCtrlRegs.WDKEY = 0x0000;
    			SysCtrlRegs.WDKEY = 0x0001;
				SysCtrlRegs.WDCR  = 0x0020;
    			EDIS;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
		}
	}
	//test
	else if(WrDataAdd >= TESTWave && WrDataAdd <= TESTMODE)
	{
		tmpPointer = (Uint16 *)&TestPara;
		temp=WrDataAdd-TESTWave;
		Write_Data(WrDataAdd,WrDataEdit,temp,tmpPointer);
	}
	else if(WrDataAdd == SampleADD3com)
	{
		Uint32 scitemp;
			scitemp = SampleAddressMap(WrDataEdit);
			if(scitemp)
			{
				commvar3 = scitemp;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
				if(TestPara.Test_Mode && 
					((scitemp==(Uint32)(&UnPg_count)) || (scitemp==(Uint32)(& test_abs_position_more))))
				{
					Save32bit[2]=1;
					if (scitemp==(Uint32)(&UnPg_count))
					{
						commvar3 = (Uint32)(&UnPg);
					}
				}
				else	Save32bit[2]=0;
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
	}
	else if(WrDataAdd == SampleADD4com)
	{
		Uint32 scitemp;
			scitemp = SampleAddressMap(WrDataEdit);
			if(scitemp)
			{
				commvar4 = scitemp;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
				if(TestPara.Test_Mode && 
					((scitemp==(Uint32)(&UnPg_count)) || (scitemp==(Uint32)(& test_abs_position_more))))
				{
					Save32bit[3]=1;
					if (scitemp==(Uint32)(&UnPg_count))
					{
						commvar4 = (Uint32)(&UnPg);
					}
				}
				else	Save32bit[3]=0;
			}
			else
			{
				sci_flag.bit.HOST_undone = 1;					//���ݲ����������
				sci_data.Error_mesg = 4;
			}
	}
    else if((WrDataAdd == AdjustSpdADD) && (Commpassword == UserPassword))//�ٶ�ģ����У��,��Ҫ�û������Ȩ�޲ſɲ���
	{
		
	    if(WrDataEdit == 1)//10V
		{
           	Pn[SumAddr] = Pn[SumAddr] - Pn[SpdAddr];
		   	SpdPos10 = (int16)Vref_Sample;
		   	Pn[SpdAddr] = (int16)Vref_Sample;
		   	Pn[SumAddr] = Pn[SumAddr] + Pn[SpdAddr];										
		   	CheckSum = Pn[SumAddr];
		   	SPI_WriteFRAM(SpdAddr,&Pn[SpdAddr],1);
		   	SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
		else if(WrDataEdit == 2)//5V
		{
           	Pn[SumAddr] = Pn[SumAddr] - Pn[SpdAddr+1];
		   	SpdPos5 = (int16)Vref_Sample;
		   	Pn[SpdAddr+1] = (int16)Vref_Sample;
		   	Pn[SumAddr] = Pn[SumAddr] + Pn[SpdAddr+1];										
		   	CheckSum = Pn[SumAddr];
		   	SPI_WriteFRAM(SpdAddr+1,&Pn[SpdAddr+1],1);
		  	SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
	    else if(WrDataEdit == 0)//0V
	  	{
        	Pn[SumAddr] = Pn[SumAddr] - Pn[SpdAddr+2];
		 	SpdZero = (int16)Vref_Sample;
			Pn[SpdAddr+2] = (int16)Vref_Sample;
			Pn[SumAddr] = Pn[SumAddr] + Pn[SpdAddr+2];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(SpdAddr+2,&Pn[SpdAddr+2],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);  
	  	}
        else if(WrDataEdit == 3)//-5V
		{
           	Pn[SumAddr] = Pn[SumAddr] - Pn[SpdAddr+3];	
			SpdNeg5 = (int16)Vref_Sample;
			Pn[SpdAddr+3] = (int16)Vref_Sample;
			Pn[SumAddr] = Pn[SumAddr] + Pn[SpdAddr+3];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(SpdAddr+3,&Pn[SpdAddr+3],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
		else if(WrDataEdit == 4)//-10V
		{
 			Pn[SumAddr] = Pn[SumAddr] - Pn[SpdAddr+4];	
			SpdNeg10 = (int16)Vref_Sample;
			Pn[SpdAddr+4] = (int16)Vref_Sample;
			Pn[SumAddr] = Pn[SumAddr] + Pn[SpdAddr+4];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(SpdAddr+4,&Pn[SpdAddr+4],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}

	}
	else if((WrDataAdd == AdjustSpdENDADD) && (Commpassword == UserPassword))//�ٶ�ģ����У����ɱ�־,��Ҫ�û������Ȩ�޲ſɲ���
	{
    	if(WrDataEdit == 0)//�ٶ�ģ����У����ɱ�־����
		{
        	Pn[SumAddr] = Pn[SumAddr] - Pn[SpdflagADD];	
			Pn[SpdflagADD] = 0;
			Pn[SumAddr] = Pn[SumAddr] + Pn[SpdflagADD];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(SpdflagADD,&Pn[SpdflagADD],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
		else if(WrDataEdit == 1)//�ٶ�ģ����У����ɱ�־��1
		{
        	Pn[SumAddr] = Pn[SumAddr] - Pn[SpdflagADD];	
			Pn[SpdflagADD] = 1;
			Pn[SumAddr] = Pn[SumAddr] + Pn[SpdflagADD];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(SpdflagADD,&Pn[SpdflagADD],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
	}
	else if((WrDataAdd == AdjustTcrADD) && (Commpassword == UserPassword))//����ģ����У��,��Ҫ�û������Ȩ�޲ſɲ���
	{
		if(WrDataEdit == 1)//10V
		{
           	Pn[SumAddr] = Pn[SumAddr] - Pn[TcrAddr];
		   	TcrPos10 = (int16)Tref_Sample;
		   	Pn[TcrAddr] = (int16)Tref_Sample;
		   	Pn[SumAddr] = Pn[SumAddr] + Pn[TcrAddr];										
		   	CheckSum = Pn[SumAddr];
		   	SPI_WriteFRAM(TcrAddr,&Pn[TcrAddr],1);
		   	SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
		else if(WrDataEdit == 2)//5V
		{
           	Pn[SumAddr] = Pn[SumAddr] - Pn[TcrAddr+1];
		   	TcrPos5 = (int16)Tref_Sample;
		   	Pn[TcrAddr+1] = (int16)Tref_Sample;
		   	Pn[SumAddr] = Pn[SumAddr] + Pn[TcrAddr+1];										
		   	CheckSum = Pn[SumAddr];
		   	SPI_WriteFRAM(TcrAddr+1,&Pn[TcrAddr+1],1);
		  	SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
		else if(WrDataEdit == 0)//0V
	  	{
        	Pn[SumAddr] = Pn[SumAddr] - Pn[TcrAddr+2];
		 	TcrZero = (int16)Tref_Sample;
			Pn[TcrAddr+2] = (int16)Tref_Sample;
			Pn[SumAddr] = Pn[SumAddr] + Pn[TcrAddr+2];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(TcrAddr+2,&Pn[TcrAddr+2],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);  
	  	}
        else if(WrDataEdit == 3)//-5V
		{
           	Pn[SumAddr] = Pn[SumAddr] - Pn[TcrAddr+3];	
			TcrNeg5 = (int16)Tref_Sample;
			Pn[TcrAddr+3] = (int16)Tref_Sample;
			Pn[SumAddr] = Pn[SumAddr] + Pn[TcrAddr+3];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(TcrAddr+3,&Pn[TcrAddr+3],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
		else if(WrDataEdit == 4)//-10V
		{
 			Pn[SumAddr] = Pn[SumAddr] - Pn[TcrAddr+4];	
			TcrNeg10 = (int16)Tref_Sample;
			Pn[TcrAddr+4] = (int16)Tref_Sample;
			Pn[SumAddr] = Pn[SumAddr] + Pn[TcrAddr+4];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(TcrAddr+4,&Pn[TcrAddr+4],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}

	}
	else if((WrDataAdd == AdjustTcrENDADD) && (Commpassword == UserPassword))//����ģ����У����ɱ�־,��Ҫ�û������Ȩ�޲ſɲ���
	{
    	if(WrDataEdit == 0)//����ģ����У����ɱ�־����
		{
        	Pn[SumAddr] = Pn[SumAddr] - Pn[TcrflagADD];	
			Pn[TcrflagADD] = 0;
			Pn[SumAddr] = Pn[SumAddr] + Pn[TcrflagADD];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(TcrflagADD,&Pn[TcrflagADD],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
		else if(WrDataEdit == 1)//����ģ����У����ɱ�־��1
		{
        	Pn[SumAddr] = Pn[SumAddr] - Pn[TcrflagADD];	
			Pn[TcrflagADD] = 1;
			Pn[SumAddr] = Pn[SumAddr] + Pn[TcrflagADD];										
			CheckSum = Pn[SumAddr];
			SPI_WriteFRAM(TcrflagADD,&Pn[TcrflagADD],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		}
	}
	else if(WrDataAdd==0x1059)
	{
	    standSum=WrDataEdit;
	    SPI_WriteFRAM(56,& standSum,1);

	}
	else if(WrDataAdd==0x1060)
	    {
	    EncoderKp=WrDataEdit;
	   //     SPI_WriteFRAM(56,& standSum,1);

	    }
	else if(WrDataAdd==0x1061)
	{
	       Pos485=WrDataEdit;
	}
	else if(WrDataAdd==0x1062)
	    {
	          xyz=WrDataEdit;
	    }
	else
	{
		sci_flag.bit.HOST_undone=1;
		sci_data.Error_mesg=2;
	}
	//------------------------------------------------------------							//
	if(sci_flag.bit.Reply)
	{
		addrtemp=sci_data.RX_data_ADR;						//����������Ч��ַ
		SXbuffer.pcurrent =SXbuffer.paddress;				//���ݴ洢�ռ��׵�ַ
		SXbuffer.data_length=2;								//���͵���Ч������
		* SXbuffer.pcurrent++=sci_data.Address;				//�ֺŵ�ַ
		if(sci_flag.bit.HOST_undone)						//�����Ӧ
		{
			* SXbuffer.pcurrent++=sci_data.RX_cmd+0x80;
			sci_data.TX_cmd=sci_data.RX_cmd+0x80;
			* SXbuffer.pcurrent++=sci_data.Error_mesg;		//
			sci_data.TX_data_number=3;
		}
		else
		{
			* SXbuffer.pcurrent++=sci_data.RX_cmd;
			sci_data.TX_cmd=sci_data.RX_cmd;
			while(SXbuffer.data_length--)						//����Ч���ݣ�1���֣�
			{
				* SXbuffer.pcurrent++=(* addrtemp++) & 0x00FF;	//д��������ֵ
				* SXbuffer.pcurrent++=(* addrtemp++) & 0x00FF;	//д��������
			}
			sci_data.TX_data_number=(2+1)<<1;
			//MoreData.Pn_Address=temp1-PNADDR;	//������ŵ�ַ
		}				
		sci_flag.bit.TX_data_ready=1;		//��������׼�����
	}
	SXbuffer.pcurrent =SXbuffer.paddress;		//���ݴ洢�ռ��׵�ַ
}

void CMD10(void)
{
	//Uint16 temp1,temp2,Sumtemp,Data_Length;
	Uint16 Datanums,DataFirstadd,DataEndadd,PnFactNum,PnFactNumFirst,WrDataEdit,temp,temp1,temp2,i;
	Uint16 * addrtemp, * tmpPointer;
	//------------------------------------------------------------
	// �Խ��������
	//------------------------------------------------------------
	addrtemp=sci_data.RX_data_ADR;
	DataFirstadd=0;
	DataFirstadd += ((* addrtemp++)<<8) & 0xFF00;		//������ʼ��ַ
	DataFirstadd += ((* addrtemp++) & 0x0FF);			//
	Datanums	= (((* addrtemp++)<<8) & 0xFF00);		//Ҫд�ļĴ�������
	Datanums	+=((* addrtemp++) & 0x0FF);				//
	*addrtemp++;										//Ҫд��bytes
	//DataFirstadd = pdata;								

	DataEndadd = DataFirstadd + Datanums - 1;			//������ֹ��ַ-->
	//------------------------------------------------------------
	// ��ַ�ж�
	//------------------------------------------------------------
	//�����ݵ�ַ���жϣ���ֹ��ַ��
	if(DataEndadd <= PnMaxNum)														//������
	{
		PnFactNumFirst = PnSwitch(DataFirstadd);
		PnFactNum = PnSwitch(DataEndadd);
		if((PnFactNum == 0xFFFF) || (PnFactNumFirst == 0xFFFF) || ((DataEndadd >= 800) && (Commpassword != UserPassword) && (DataEndadd != 840)))
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 2;
		}
		else
		{
			PnFactNum = PnFactNum + 1 - Datanums;
			//�жϲ�����Χ
			temp2=Datanums;
			while(temp2--)
			{
				WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
				WrDataEdit +=((* addrtemp++) & 0x0FF);
				if(Para_Maxmin(PnFactNum++,WrDataEdit))		//�ж������С��ֵ
				{
					sci_flag.bit.HOST_undone=1;				//д��Ĳ�����ֵ���ڷ�Χ��
					sci_data.Error_mesg=3;
					break;
				}
			}
			if(!sci_flag.bit.HOST_undone)					//�洢����
			{
				temp2=Datanums;
				addrtemp = addrtemp - (Datanums<<1);		//ָ���������
				PnFactNum = PnFactNum - Datanums;
				while(temp2--)
				{
					WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);	//����
					WrDataEdit +=((* addrtemp++) & 0x0FF);	

					Pn[SumAddr] -= Pn[PnFactNum];	// 									
					CheckSum -= Pn[PnFactNum];
					Pn[PnFactNum] = WrDataEdit;
					Pn[SumAddr] += Pn[PnFactNum];
					CheckSum += Pn[PnFactNum];
					SPI_WriteFRAM(PnFactNum,&Pn[PnFactNum],1);
					SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
					PnFactNum++;
				 }
			}
		}	
	}
	else if((DataFirstadd >= ZeroSpdADD) && DataEndadd <= ZeroTcrADD)
	{
		temp1 = DataFirstadd;
		temp2=Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);
			if(temp1 == ZeroSpdADD)
			{
				Pn[SumAddr] -=  spdoffset;
				CheckSum -=  spdoffset;
				spdoffset = WrDataEdit;
				Pn[Fn004Addr] = spdoffset;
				Pn[SumAddr] +=  spdoffset;
				CheckSum +=  spdoffset;
				SPI_WriteFRAM(Fn004Addr,&Pn[Fn004Addr],1);
				SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
				sci_flag.bit.HOST_undone = 0;				//���ݲ������
			}
			else//ZeroTcrADD
			{
				Pn[SumAddr] -=  TCRoffset;
				CheckSum -=  TCRoffset;
				TCRoffset = WrDataEdit;
				Pn[Fn004Addr+1] = TCRoffset;
				Pn[SumAddr] +=  TCRoffset;
				CheckSum += TCRoffset;
				SPI_WriteFRAM(Fn004Addr+1,&Pn[Fn004Addr+1],1);
				SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
				sci_flag.bit.HOST_undone = 0;				//���ݲ������
			}
			temp1++;
		}
	}
	else if((DataFirstadd >= MemgearB) && DataEndadd <= MemgearA2)
	{
		temp1 = DataFirstadd;
		temp2 = Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);
			if(temp1 == MemgearB)
			{
				if((WrDataEdit!=0) && (memBusSel!=3))
				{
					Bgear = WrDataEdit;
					sci_flag.bit.HOST_undone = 0;				//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;
					sci_data.Error_mesg = 3;
					break;
				}
			}
			else if(temp1 == MemgearA1)
			{
				if((WrDataEdit!=0) && (memBusSel!=3))
				{
					memAgear1 = WrDataEdit;
					sci_flag.bit.HOST_undone = 0;				//���������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;
					sci_data.Error_mesg = 3;
					break;
				}
			}
			else if(temp1 == MemgearA2)
			{
				if((WrDataEdit!=0) && (memBusSel!=3))
				{
					memAgear2 = WrDataEdit;
					sci_flag.bit.HOST_undone = 0;				//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;
					sci_data.Error_mesg = 3;
					break;
				}
			}
			temp1++;
		}
	}
	else if(DataEndadd == MODBUSinputADD)					//
	{
		WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);		//����
		WrDataEdit +=((* addrtemp++) & 0x0FF);	
		sci_data.SCIInput = WrDataEdit;
		sci_flag.bit.HOST_undone = 0;
	}
	else if(DataEndadd == UserPINADD)						//
	{
		WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);		//����
		WrDataEdit +=((* addrtemp++) & 0x0FF);			
		Commpassword = WrDataEdit;
		sci_flag.bit.HOST_undone = 0;
	}
	else if(DataEndadd == HandOpPnnumADD)					//
	{
		WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);		//����
		WrDataEdit +=((* addrtemp++) & 0x0FF);	
		if(WrDataEdit <= PnMaxNum)							//������
		{
			PnFactNum = PnSwitch(WrDataEdit);
			if(PnFactNum == 0xFFFF)
			{// �Ƿ���ַ
				sci_flag.bit.HOST_undone = 1;
				sci_data.Error_mesg = 3;
			}
			else
			{
				sci_data.SCIHandCurPnnum = WrDataEdit;
				sci_flag.bit.HOST_undone = 0;
			}
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;					//���ݲ����������
			sci_data.Error_mesg = 3;
		}
	}
	else if((DataFirstadd >= CLRALMLOGcom) && (DataEndadd <= JDPOSHaltcom))						//
	{
		temp1 = DataFirstadd;
		temp2=Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);

			if(temp1 == CLRALMLOGcom)
			{
				if(WrDataEdit == 1)
				{
					for(i = 0; i < 10; i++)
					{
						Pn[SumAddr] -= Pn[Fn000Addr + i];
						CheckSum -= Pn[Fn000Addr + i];
						Pn[Fn000Addr + i] = 0;							
					}
					SPI_WriteFRAM(Fn000Addr,&Pn[Fn000Addr],10);
					SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == CLRALMcom)
			{
				if(WrDataEdit == 1)
				{
					state_flag2.bit.HaveAlm = 0;
					state_flag2.bit.DispAlm = 0;
					state_flag2.bit.ClrAlm = 1;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == JOGSoncom)
			{
				if(WrDataEdit == 0)
				{
					asm("	SETC	INTM");
					sci_oper_flag.bit.sciJogson = 0;
					asm("	CLRC	INTM");
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if((WrDataEdit == 1) && (state_flag2.bit.SvReady) && (!key_flag1.bit.JOGrun))
				{
					asm("	SETC	INTM");
					sci_oper_flag.bit.sciJogson = 1;
					asm("	CLRC	INTM");
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == JOGPruncom)
			{
				if(WrDataEdit == 0)
				{
					key_flag1.bit.JOGpos = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if((WrDataEdit == 1) && (state_flag2.bit.SvReady) && (!key_flag1.bit.JOGrun))
				{
					key_flag1.bit.JOGpos = 1;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == JOGNruncom)
			{
				if(WrDataEdit == 0)
				{
					key_flag1.bit.JOGneg = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if((WrDataEdit == 1) && (state_flag2.bit.SvReady) && (!key_flag1.bit.JOGrun))
				{
					key_flag1.bit.JOGneg = 1;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == JDPOSJOGpcom)
			{
				if(WrDataEdit == 0)
				{
					sci_oper_flag.bit.sci_pos_jogp = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if((WrDataEdit == 1) && int_flagx.bit.pos_jogen)
				{
					sci_oper_flag.bit.sci_pos_jogp = 1;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == JDPOSJOGncom)
			{
				if(WrDataEdit == 0)
				{
					sci_oper_flag.bit.sci_pos_jogn = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if((WrDataEdit == 1) && int_flagx.bit.pos_jogen)
				{
					sci_oper_flag.bit.sci_pos_jogn = 1;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == JDPOSHaltcom)
			{
				if(WrDataEdit == 0)
				{
					sci_oper_flag.bit.sci_pos_halt = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if(WrDataEdit == 1)
				{
					sci_oper_flag.bit.sci_pos_halt = 1;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			temp1++;
		}
	}
	else if((DataFirstadd >= PHASEstartcom) && (DataEndadd <= Inertiastartcom))						//
	{
		temp1 = DataFirstadd;
		temp2=Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);				

			if(temp1 == PHASEstartcom)
			{
				if(WrDataEdit == 0)
				{
					sci_oper_flag.bit.sciPhaseSON = 0;
					sci_oper_status.bit.sciPhaseStatus = 0;
					asm("	SETC	INTM");				
					int_flagx.bit.SciFindInitPhase = 0;
					int_flag3.bit.HaveFindInitPos = 0;
					int_flag3.bit.pass360 = 0;
					asm("	CLRC	INTM");				
					sci_flag.bit.HOST_undone = 0;						//���ݲ������
				}
				else if(WrDataEdit == 1)
				{
					if(state_flag2.bit.Son || (!state_flag2.bit.SvReady) || state_flag2.bit.HandPhase )
					{
						sci_flag.bit.HOST_undone = 1;					//���ݲ����������
						sci_data.Error_mesg = 4;
					}
					else
					{
						sci_oper_flag.bit.sciPhaseSON = 1;
						sci_oper_status.bit.sciPhaseStatus = 1;
						asm("	SETC	INTM");					
						int_flagx.bit.SciFindInitPhase = 0;
						int_flag3.bit.HaveFindInitPos = 0;
						int_flag3.bit.pass360 = 0;
						asm("	CLRC	INTM");
						sci_flag.bit.HOST_undone = 0;					//���ݲ������
					}
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;						//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == Inertiastartcom)
			{
				if(WrDataEdit == 0)
				{				
					sci_oper_status.bit.sciInertiaStatus = 0;
					sci_oper_flag.bit.sciStartInertia = 0;
					asm("	SETC	INTM");	
					int_flag3.bit.J_initia = 0;
					int_flag3.bit.J_End = 0;
					asm("	CLRC	INTM");
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if((WrDataEdit == 1) && state_flag2.bit.SvReady && (!state_flag2.bit.Son))
				{
					sci_oper_status.bit.sciInertiaStatus = 1;
					sci_oper_flag.bit.sciStartInertia = 1;
					asm("	SETC	INTM");	
					int_flag3.bit.J_initia = 0;
					int_flag3.bit.J_End = 0;
					asm("	CLRC	INTM");	
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			temp1++;
		}
	}
	else if((DataFirstadd >= ClrEncErrcom) && (DataEndadd <= ClrEncMuticom))
	{
		temp1 = DataFirstadd;
		temp2 = Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);	
			if(temp1 == ClrEncErrcom)
			{
				if(WrDataEdit == 0)
				{
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if((WrDataEdit == 1) && !state_flag2.bit.Son && memCurLoop00.hex.hex00 == Encoder_Abs17bit)
				{
					asm("	SETC	INTM");
					int_flagx.bit.rstEncErr = 1;
					asm("	CLRC	INTM");
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == ClrEncMuticom)
			{
				if(WrDataEdit == 0)
				{
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if((WrDataEdit == 1) && !state_flag2.bit.Son && memCurLoop00.hex.hex00 == Encoder_Abs17bit)
				{
					asm("	SETC	INTM");
					int_flagx.bit.rstEncMuti = 1;
					asm("	CLRC	INTM");
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			temp1++;
		}			
	}
	else if((DataFirstadd >= Samplestartcom) && (DataEndadd <= SampleTimecom))						//
	{
		temp1 = DataFirstadd;
		temp2=Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);
			
			if(temp1 == Samplestartcom)
			{
				if(WrDataEdit == 0)
				{
					sci_oper_status.bit.sciSampleStatus = 0;
					sci_oper_flag.bit.sciStartSample = 0;
					sci_oper_flag.bit.SciSampleing = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
					wComm = SAMPLE_BUF;						// ������׵�ַ
				}
				else if(WrDataEdit == 1)
				{
					savecnt = 0;
					sci_oper_status.bit.sciSampleStatus = 1;
					sci_oper_flag.bit.sciStartSample = 1;
					sci_oper_flag.bit.SciSampleing = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
					wComm = SAMPLE_BUF;						// ������׵�ַ
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
					wComm = SAMPLE_BUF;						// ������׵�ַ
				}
			}
			else if(temp1 == SampleADD1com)
			{Uint32 scitemp;
				scitemp = SampleAddressMap(WrDataEdit);
				if(scitemp)
				{
					commvar1 = scitemp;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
					if(TestPara.Test_Mode && 
						((scitemp==(Uint32)(&UnPg_count)) || (scitemp==(Uint32)(& test_abs_position_more))))
					{
						Save32bit[0]=1;
						if (scitemp==(Uint32)(&UnPg_count))
						{
							commvar1 = (Uint32)(&UnPg);
						}
					}
					else	Save32bit[0]=0;
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == SampleADD2com)
			{Uint32 scitemp;
				scitemp = SampleAddressMap(WrDataEdit);
				if(scitemp)
				{
					commvar2 = scitemp;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
					if(TestPara.Test_Mode && 
						((scitemp==(Uint32)(&UnPg_count)) || (scitemp==(Uint32)(& test_abs_position_more))))
					{
						Save32bit[1]=1;
						if (scitemp==(Uint32)(&UnPg_count))
						{
							commvar2 = (Uint32)(&UnPg);
						}
					}
					else	Save32bit[1]=0;
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == SampleTrigcom)
			{
				if(WrDataEdit > 3)
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
				else
				{
					sci_data.SCISampleTrig = WrDataEdit;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
			}
			else if(temp1 == SampleTrigV1com)
			{
				sci_data.SCISampleTrigV1 = WrDataEdit;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else if(temp1 == SampleTrigV2com)
			{
				sci_data.SCISampleTrigV2 = WrDataEdit;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			else//SampleTimecom
			{
				sci_data.SCISampleTime = WrDataEdit;
				sci_flag.bit.HOST_undone = 0;					//���ݲ������
			}
			temp1++;
		}
	}
	else if((DataFirstadd >= ZeroSpdstartcom) && (DataEndadd <= ZeroTcrstartcom))						//
	{
		temp1 = DataFirstadd;
		temp2=Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);
			
			if(temp1 == ZeroSpdstartcom)
			{
				if(WrDataEdit == 0)
				{
					sci_oper_flag.bit.sciStartZeroSpd = 0;
					sci_oper_status.bit.sciZeroSpdStatus = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if(WrDataEdit == 1)
				{
					asm("	SETC	INTM");
					Timers.cntdone = 0;
					Sspd = 0;
					Stcr = 0;
					asm("	CLRC	INTM");
					sci_oper_flag.bit.sciStartZeroSpd = 1;
					sci_oper_status.bit.sciZeroSpdStatus = 1;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == ZeroTcrstartcom)
			{
				if(WrDataEdit == 0)
				{
					sci_oper_flag.bit.sciStartZeroTcr = 0;
					sci_oper_status.bit.sciZeroTcrStatus = 0;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else if(WrDataEdit == 1)
				{
					asm("	SETC	INTM");
					Timers.cntdone = 0;
					Sspd = 0;
					Stcr = 0;
					asm("	CLRC	INTM");
					sci_oper_flag.bit.sciStartZeroTcr = 1;
					sci_oper_status.bit.sciZeroTcrStatus = 1;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
		}
			temp1++;
		}
	}
	else if((DataFirstadd >= LoadDefaultcom) && (DataEndadd <= RESETDSPcom))						//
	{
		temp1 = DataFirstadd;
		temp2 = Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);
			if(temp1 == LoadDefaultcom)
			{
				if((WrDataEdit == 1) && (!state_flag2.bit.Son))
				{
					LoadDefaultPar();
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == RESETDSPcom)
			{
				if(WrDataEdit == 1)
				{
					EALLOW;
	    			SysCtrlRegs.WDKEY = 0x0000;
	    			SysCtrlRegs.WDKEY = 0x0001;
					SysCtrlRegs.WDCR  = 0x0020;
	    			EDIS;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
			temp1++;
		}
	}
	//test data
	else if((DataFirstadd >= TESTWave) && (DataEndadd <= TESTMODE))						//
	{
		//temp1 = DataFirstadd;
		tmpPointer = (Uint16 *)&TestPara;
		temp = DataFirstadd - TESTWave;
		temp2 = Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);
			//WrDataAdd = DataFirstadd;
			Write_Data(DataFirstadd,WrDataEdit,temp,tmpPointer);
			temp++;
			DataFirstadd++;
		}
	}
	else if((DataFirstadd >= SampleADD3com) && (DataEndadd <= SampleADD4com))						//
	{
		temp1 = DataFirstadd;
		temp2=Datanums;
		while(temp2--)
		{
			WrDataEdit = (((* addrtemp++)<<8) & 0xFF00);//����
			WrDataEdit +=((* addrtemp++) & 0x0FF);
			if(temp1 == SampleADD3com)
			{Uint32 scitemp;
				scitemp = SampleAddressMap(WrDataEdit);
				if(scitemp)
				{
					commvar3 = scitemp;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
					if(TestPara.Test_Mode && 
						((scitemp==(Uint32)(&UnPg_count)) || (scitemp==(Uint32)(& test_abs_position_more))))
					{
						Save32bit[2]=1;
						if (scitemp==(Uint32)(&UnPg_count))
						{
							commvar3 = (Uint32)(&UnPg);
						}
					}
					else	Save32bit[2]=0;
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ���������?
					sci_data.Error_mesg = 4;
				}
			}
			else if(temp1 == SampleADD4com)
			{Uint32 scitemp;
				scitemp = SampleAddressMap(WrDataEdit);
				if(scitemp)
				{
					commvar4 = scitemp;
					sci_flag.bit.HOST_undone = 0;					//���ݲ������
					if(TestPara.Test_Mode && 
						((scitemp==(Uint32)(&UnPg_count)) || (scitemp==(Uint32)(& test_abs_position_more))))
					{
						Save32bit[3]=1;
						if (scitemp==(Uint32)(&UnPg_count))
						{
							commvar4 = (Uint32)(&UnPg);
						}
					}
					else	Save32bit[3]=0;
				}
				else
				{
					sci_flag.bit.HOST_undone = 1;					//���ݲ����������
					sci_data.Error_mesg = 4;
				}
			}
		}
	}
	else
	{
		sci_flag.bit.HOST_undone=1;	//������������ַ�ڣ����ַ����
		sci_data.Error_mesg=2;
	}
	//------------------------------------------------------------							//
	if(sci_flag.bit.Reply)
	{
		addrtemp=sci_data.RX_data_ADR;					//����������Ч��ַ
		SXbuffer.pcurrent =SXbuffer.paddress;				//���ݴ洢�ռ��׵�ַ
		SXbuffer.data_length=1;								//���͵���Ч��������
		* SXbuffer.pcurrent++=sci_data.Address;//�ֺŵ�ַ
		if(sci_flag.bit.HOST_undone)					//�����Ӧ
		{
			* SXbuffer.pcurrent++=sci_data.RX_cmd+0x80;
			sci_data.TX_cmd=sci_data.RX_cmd+0x80;
			* SXbuffer.pcurrent++=sci_data.Error_mesg;			//�����־
			sci_data.TX_data_number=3;
		}
		else
		{
			* SXbuffer.pcurrent++=sci_data.RX_cmd;
			sci_data.TX_cmd=sci_data.RX_cmd;
			//}
			while(SXbuffer.data_length--)						//����Ч����
			{
				* SXbuffer.pcurrent++=(* addrtemp++) & 0x00FF;	//д��������ֵ
				* SXbuffer.pcurrent++=(* addrtemp++) & 0x00FF;	//д��������ֵ
			}
			* SXbuffer.pcurrent++=(Datanums & 0xFF00)>>8;	//д�ļĴ�������
			* SXbuffer.pcurrent++=Datanums & 0x00FF;			//
			sci_data.TX_data_number=(2+1)<<1;
		}
		sci_flag.bit.TX_data_ready=1;		//��������׼�����
	}
	SXbuffer.pcurrent =SXbuffer.paddress;				//���ݴ洢�ռ��׵�ַ
}
//=============================================================================
// ��ȡ�������Ţ�?�����ݣ�
// 1����˾��ESTUN;
// 2������ƣ�EDC SERVO��
// 3������汾��200
//=============================================================================
void CMD2B(void){
	Uint16	i;
	//------------------------------------------------------------							//
	if(sci_flag.bit.Reply){
		SXbuffer.pcurrent =SXbuffer.paddress;				//���ݴ洢�ռ��׵�ַ
		//SXbuffer.data_length=1;							//��͵���Ч�������?
		* SXbuffer.pcurrent++=sci_data.Address;	//�ֺŵ�ַ
		* SXbuffer.pcurrent++=sci_data.RX_cmd;

		* SXbuffer.pcurrent++=0x0E;	//MEI
		* SXbuffer.pcurrent++=0x01;
		* SXbuffer.pcurrent++=0x02;
		* SXbuffer.pcurrent++=0x00;
		* SXbuffer.pcurrent++=0x00;
		* SXbuffer.pcurrent++=0x03;
		* SXbuffer.pcurrent++=0x05;
		//ESTUN
		for(i=0;i<5;i++)
		{
			* SXbuffer.pcurrent++=ESTUN[i];
		}
		
		//EDC Driver
		* SXbuffer.pcurrent++=0x01;
		* SXbuffer.pcurrent++=0x09;	
		for(i=0;i<9;i++)
		* SXbuffer.pcurrent++=EDB_Servo[i];

		//V200
		* SXbuffer.pcurrent++=0x02;
		* SXbuffer.pcurrent++=0x06;
		for(i=0;i<6;i++)
		* SXbuffer.pcurrent++=Speed_Software[i];

		sci_data.TX_cmd=sci_data.RX_cmd;				
		sci_data.TX_data_number=2+7+5+11+8;		//�ֽ���
		sci_flag.bit.TX_data_ready=1;		//��������׼�����
		sci_flag.bit.Reply=1;				//��ҪDSPӦ��
	}
	SXbuffer.pcurrent =SXbuffer.paddress;				//���ݴ洢�ռ��׵�ַ

}
//=============================================================================
//比较参数的最大最小值
//输入：参数地址（0x8000～0x80f7），数据
//输出：0：数据在参数范围内；1：数据不再此参数范围内
//=============================================================================

Uint16 Para_Maxmin(Uint16 PnFactNum,int16 WrDataEdit)
{HEX_DISP hexEdit,hexEditMax,hexEditMin;
	if(PnAttribute[PnFactNum][0] == 1)
	{
		if(((int16)WrDataEdit < (int16)PnAttribute[PnFactNum][1]) 
		 ||((int16)WrDataEdit > (int16)PnAttribute[PnFactNum][2]))
		{
			return 1;
		}
	}
	else if(PnAttribute[PnFactNum][0] == 3)
	{
		hexEdit.all = WrDataEdit;
		hexEditMin.all = PnAttribute[PnFactNum][1];	
		hexEditMax.all = PnAttribute[PnFactNum][2];										
		if((hexEdit.hex.hex00 < hexEditMin.hex.hex00)
		 ||(hexEdit.hex.hex01 < hexEditMin.hex.hex01)
		 ||(hexEdit.hex.hex02 < hexEditMin.hex.hex02)
		 ||(hexEdit.hex.hex03 < hexEditMin.hex.hex03))
		{
			return 1;
		}
		else if((hexEdit.hex.hex00 > hexEditMax.hex.hex00)
			 ||(hexEdit.hex.hex01 > hexEditMax.hex.hex01)
			 ||(hexEdit.hex.hex02 > hexEditMax.hex.hex02)
		 	 ||(hexEdit.hex.hex03 > hexEditMax.hex.hex03))
		{
			return 1;
		}
	}	
	else
	{
		if((WrDataEdit < PnAttribute[PnFactNum][1]) 
		 ||(WrDataEdit > PnAttribute[PnFactNum][2]))
		{
			return 1;
		}
	}
	return 0;			
}

Uint16 PnSwitch(Uint16 Para_Address)
{
	Uint16 PnGropBeg,PnGropBegNum,PnFactNum;
	PnGropBeg = Para_Address/100;
	PnGropBegNum = Para_Address%100;
		switch(PnGropBeg)
		{
			case 0:
				PnFactNum = Pn1FactBeg + PnGropBegNum;
				break;
			case 1:
				PnFactNum = Pn2FactBeg + PnGropBegNum;
				break;
			case 2:
				PnFactNum = Pn3FactBeg + PnGropBegNum;
				break;
			case 3:
				PnFactNum = Pn4FactBeg + PnGropBegNum;
				break;
			case 4:
				PnFactNum = Pn5FactBeg + PnGropBegNum;
				break;
			case 5:
				PnFactNum = Pn6FactBeg + PnGropBegNum;
				break;
			case 6:
				PnFactNum = Pn7FactBeg + PnGropBegNum;
				break;
			case 7:
				PnFactNum = Pn8FactBeg + PnGropBegNum;
				break;
			case 8:
				PnFactNum = Pn9FactBeg + PnGropBegNum;
				break;
			default:
				PnFactNum = 0xFFFF;	// �Ƿ���ַ
				break;
		}
		return PnFactNum;
}


Uint32 SampleAddressMap(Uint16 SampleAddress)
{
	if(SampleAddress == speedrminADD)
	{
		return  (Uint32)(& Vfactcomm);
	}
	else if(SampleAddress == VCMDrminADD)
	{
		return  (Uint32)(& VCMDcomm);
	}
	else if(SampleAddress == TCMDADD)
	{
		return  (Uint32)(& TCMD);
	}
	else if(SampleAddress == TnADD)
	{
		return  (Uint32)(& Tn);
	}
	else if(SampleAddress == inputADD)
	{
		return  (Uint32)(& input_state.all);
	}
	else if(SampleAddress == outMemADD)
	{
		return  (Uint32)(& out_state.all);
	}
	else if(SampleAddress == CurPos1ADD)
	{
		if(TestPara.Test_Mode)
		{
			return  (Uint32)(& test_abs_position_more);
		}
		else if(memBusSel == 3)
		{
			return  (Uint32)(& co_abs_count);
		}
		else
		{
			return  (Uint32)(& Pfcount);
		}
	}
	else if(SampleAddress == CurPosADD)
	{
		if(memBusSel == 3)
		{
			return  (Uint32)(& co_abs_roate);
		}
		else
		{
			return  (Uint32)(& Pfroate);
		}
	}
	else if(SampleAddress == Ek4ADD)	// ƫ�����������16λ
	{
		return  (Uint32)(& Ek);
	}
	else if(SampleAddress == Ek3ADD)	// ƫ�����������16λ
	{
		return  ((Uint32)(& Ek) + 1);
	}
	else if(SampleAddress == Pg1ADD)	// ���������λ
	{
		return  (Uint32)(& UnPg_count);
	}
	else if(SampleAddress == PgADD)		// ���������λ
	{
		return  (Uint32)(& UnPg_roate);
	}
	else if(SampleAddress == IuADD)
	{
		return  (Uint32)(& Iu);
	}
	else if(SampleAddress == IvADD)
	{
		return  (Uint32)(& Iv);
	}
	else if(SampleAddress == IdrADD)
	{
		return  (Uint32)(& Idr);
	}
	else if(SampleAddress == IdADD)
	{
		return  (Uint32)(& Id);
	}
	else if(SampleAddress == IqrADD)
	{
		return  (Uint32)(& Iqr);
	}
	else if(SampleAddress == IqADD)
	{
		return  (Uint32)(& Iq);
	}
	else if(SampleAddress == UdADD)
	{
		return  (Uint32)(& Ud);
	}
	else if(SampleAddress == UqADD)
	{
		return  (Uint32)(& Uq);
	}
	else if(SampleAddress == speedADD)
	{
		return  (Uint32)(& speed);
	}
	else if(SampleAddress == VCMDADD)
	{
		return  (Uint32)(& VCMD);
	}
	else if(SampleAddress == SPEEDREF)
	{
		return  (Uint32)(& SpeedRef);
	}
	else if(SampleAddress == wAnPressFeed_outADD)
	{
		return  (Uint32)(& wAnPressFeed_out);
	}
	else if(SampleAddress == wAnPressBack_outADD)
	{
		return  (Uint32)(& wAnPressBack_out);
	}
	else if(SampleAddress == 0x0000){
		return 0x123;
	}
	else if(SampleAddress < 0x001000)
	{
		return (0x00F000 + SampleAddress);
	}
	else
	{
		return 0;
	}
}
void	Write_Data(Uint16 WrDataAdd,Uint16 WrDataEdit,Uint16 temp,Uint16 * tmpPointer){
	if(WrDataAdd == TESTWave)
	{
		if(WrDataEdit==1 || WrDataEdit==2 || WrDataEdit==10 || WrDataEdit==11)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTSpeed){
		if(WrDataEdit>0 && WrDataEdit<=3000)//1rpm
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTAcc){
		if(WrDataEdit>0)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTFreq){
		if(WrDataEdit>0 && WrDataEdit<=50)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTRepeat){
		if(WrDataEdit>0 && WrDataEdit<=10)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTRuntime){
		if(WrDataEdit>0 && WrDataEdit<=32767)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTStopServo){		//not used
		if(WrDataEdit<2)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTCWCCW){
		if(WrDataEdit<2)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTStoptime){
		if(WrDataEdit<=32767)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 3;
		}
	}else if(WrDataAdd == TESTStart){
		if(WrDataEdit<2 && TestPara.Test_Mode==1)
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 4;
		}
	}else if(WrDataAdd == TESTMODE){
		if(WrDataEdit==1 && (!state_flag2.bit.Son))			//s-off
		{
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;					//1
		}else if(!WrDataEdit){
			* (tmpPointer+temp)=WrDataEdit;
			sci_flag.bit.HOST_undone = 0;					//0
		}
		else
		{
			sci_flag.bit.HOST_undone = 1;
			sci_data.Error_mesg = 4;
		}
	}else{
		* (tmpPointer+temp)=WrDataEdit;
	}
}
//=============================================================================
