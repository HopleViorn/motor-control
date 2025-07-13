// 子程序单元
#include "EDBclm.h"			// define and function declare
#include "globalvar.h"		//  
#include "DPRAM.h"

//#pragma CODE_SECTION(AnybusIsr, "ramfuncs");

void WriteAppIndBits( unsigned int CBits, unsigned int SBits, unsigned int Tbits );
unsigned long DPRAM_CO_OD_Write(const CO_objectDictionaryEntry* pODE, void* data, unsigned int length);
unsigned long DPRAM_CO_OD_Read(const CO_objectDictionaryEntry* pODE, void* pBuff, unsigned int sizeOfBuff);
void CheckModule(void);
void ClearMapInOutMatrix(void);
Uint16 AppWriteDPRAM(Uint16 *SourceAddr,Uint16 *DPRAMAddr);
Uint16 AppWriteDPRAMREG(Uint16 *SourceAddr,Uint16 *DPRAMAddr);
Uint16 AppWriteMapData(Uint16 *SourceAddr,Uint16 *DPRAMAddr);
void PowerUpInitHandshake(void);
Uint16 InitialHandshake(void);
void ParaProcess(void);

extern const Uint16 PnAttribute[][4];
/*******************************************************************************
**
** Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** AbIndCopy
**
**------------------------------------------------------------------------------
**
** Copy of the curren7t AnyBus Indication register [ABS-DGP 4.7] (updated by the
** interrupt routine).
**
**------------------------------------------------------------------------------
*/


/*------------------------------------------------------------------------------
**
** ProcessData
**
**------------------------------------------------------------------------------
**
** Holds the current process data.
**
**------------------------------------------------------------------------------
*/

//volatile Uint32 MAPINcnt = 0; 
volatile Uint16 WriteSuccess = 0;            // = 0: The Appwrite is completed.
                        			// > 1: The Appwrite failed.
volatile int16 DPRAMtmp;
typedef	struct{
	volatile Uint16	ParComm;			// parameter command
	volatile Uint16	ParIndex;			// parameter number
	volatile Uint16	ParDataL;			// parameter high 16bits
	volatile Uint16	ParDataH;			// parameter low  16bits
	//Uint16	IOLength;		// IO data length: 1 - 16bits; 2 - 32bits
}DPRAM_MAILBOX_Struct;

DPRAM_MAILBOX_Struct DPRAMBus = {0,0,0,0};
DPRAM_MAILBOX_Struct DPRAMApp = {0,0,0,0};
//DPRAM_MAILBOX_Struct DPRAMBusold = {0,0,0,0};

#define MaxIOnum	10			// Map IO data max number
typedef	struct{	
	volatile Uint32 Address[MaxIOnum];	// IO data address
	volatile Uint16 Length[MaxIOnum];	// IO data length : 1 - 16bits; 2 - 32bits
	volatile Uint16 MaxIndex;			// Map IO data number : Index <= MaxIOnum;
}MAP_IO_Struct;

MAP_IO_Struct MapIn;
MAP_IO_Struct MapOut;

#define MaxGuardnum	10			// Guard data max number
typedef	struct{	
	volatile Uint32 Address[MaxGuardnum];	// Guard data address
	volatile Uint16 GuardIndex[MaxGuardnum];	// Guard data index
	volatile Uint16 Length[MaxGuardnum];		// Guard data length : 1 - 16bits; 2 - 32bits
	volatile Uint32 GuardData[MaxGuardnum];	// Guard data
	volatile Uint16 Guarded[MaxGuardnum];	// 0: 无变化 1：变化 
	volatile Uint16 MaxIndex;				// Guard data number : Index <= MaxIOnum;
}Guard_Struct;

Guard_Struct GuardStack;

//Uint16 intoMapInBufMSGnum = 0;					// save into Mapinbuf msg number 
volatile Uint16 WrParFRAMFlg = 0;		// write para into FRAM
volatile Uint16 BusEditIndex = 0;		// 总线修改参数实际索引号
volatile Uint16 BusEditContent = 0;		// 总线修改参数值
volatile Uint16 BusGropNum = 0;			// 总线修改参数显示索引号
//volatile Uint16 ResParCommOK = 0;		// 非周期命令是否正确响应
										// 0：正确
										// 1：不正确
volatile Uint16 WrParStatus = 0;		// 写参数到FRAM进程 
										// 0：NOP
										// 1：wr ing 写进行中
										// 2：wr end 写完成
volatile Uint16 GuardedFlg = 0;			// Guarded数据发生变化标志
				
void DPRAM_init(void)
{Uint16 temp1;
	
	
	RST_DPRAM_H();
	RST_DPRAM_L();
	for(temp1=0;temp1<10;temp1++)
	{
		asm("	nop");
	}
	RST_DPRAM_H();
	PowerUpInitHandshake();
	
}
void DPRAM_Timers_process(void)
{
	if((memBusSel == 1) || (memBusSel == 2))
	{
		if((InitialShakehandLength == 0) && (Timers.DPRAM_HS_Delay < 60000))	// DPRAM 握手延时    5ms
		{
			Timers.DPRAM_HS_Delay++;
		}

		if(ModuleStatus.bit.HS)
		{
			Timers.DPRAM_HS_ScanCnt = 0;
			if(Timers.DPRAMISR_ScanCnt < 60000)		// DPRAM ISR扫描延时 200ms
			{
				Timers.DPRAMISR_ScanCnt++;
			}
		}
		else
		{
			if((InitialShakehandLength) && (Timers.DPRAM_HS_ScanCnt < 60000))		// DPRAM 握手扫描timer：如果握手没成功，则每2秒启动1次握手初始化，直到握手成功
			{
				Timers.DPRAM_HS_ScanCnt++;
			}
		}
	}
	//if(*EDBH_INDREG_ADD != AppIndCopy)
	//{
		//AppWriteDPRAM((void *)&AppIndCopy,(Uint16 *)EDBH_INDREG_ADD);
	//}
	
}
void DPRAM_main_process(void)
{	
	unsigned int * temp;
	if((memBusSel == 1) || (memBusSel == 2))
	{
		if(Timers.DPRAMISR_ScanCnt > TIME_50MS)	// 50ms
		{
			Timers.DPRAMISR_ScanCnt = 0;
			ModuleStatus.bit.SCANerr = 1;				// 50ms内没扫描到通信板
			if(RESETDPRAMISR == 0)
			{
				DPRAMtmp = *ABUS_INDREG_ADD;			// 读，reset中断脚电平
			}
		}
		if(RESETDPRAMISR == 0)
		{
			DPRAMtmp = *ABUS_INDREG_ADD;			// 读，reset中断脚电平
		}

		if(DPRAM_HS_StartCnt > 1)
		{
			DPRAM_HS_StartCnt = 0;
			ModuleStatus.bit.HSerr = 1;
		}
		else if(Timers.DPRAM_HS_ScanCnt > 400000)	// 40000ms
		{
			Timers.DPRAM_HS_ScanCnt = 0;
			DPRAM_HS_StartCnt++;
			DPRAM_init();	
		}			
	
		InitialHandshake();							// 握手扫描
		WrParFRAM();								// 总线写参数
		CheckGuard();								// 检测检测数据是否发生变化
		CheckModule();								// 检测通信板是否在线

		ParaDataExchange();
		temp=(unsigned int *)BusType_ADD;			//profibus bustype
		* temp=memBusSel;
		temp=(unsigned int *)VendorID_ADD;			//dp100 EDITOR
		//* temp;
	}
}

/*------------------------------------------------------------------------------
**
** DPRAM_ProcessDataExchange()
**
** DPRAM周期数据（过程数据）交换
**
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**    ISR
**
**------------------------------------------------------------------------------
*/
void DPRAM_DirectDataExchange(void)
{	Uint16 ProcessDataOut[6];
	Uint16 CaulSum,DRAM_CRC,RDcnt;
	Uint16 * pProcessDataOut;
	Uint16 PosHTime,SyncHTime;
	int32 TMP;
	int16 tmpPG,SyncInStatustmp;	
	//rd output
  if((*ABUS_INDREG_ADD & INDREG_HS) && ModuleStatus.bit.HS) //Handshake OK
  {	
	if(memBusSel == 2) // DP-v2
	{
		tmpPG = *SyncQPCNT;
		syncprofiTime = *SyncTime;
		if(memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit)
		{
			DeltSyncTime = 0;
			syncPGout = tmpPG;
		}
		else
		{
			SyncInStatustmp = (*TOTAL_IN) & 0x0400;	// read sync input	
			if(SyncInStatus != SyncInStatustmp)
			{
				//PosHTime = *PosHTimeADD;
				PosHTime = spgTime[0];
				SyncHTime = *SyncHTimeADD;		
				if(PosHTime > SyncHTime)
				{
					DeltSyncTime = (65535 - PosHTime) + SyncHTime + 1;
				}
				else
				{
					DeltSyncTime = SyncHTime - PosHTime;
				}
				TMP = (int32)ErrPos;
				TMP = (TMP * (int32)DeltSyncTime) / spgTimeDelt[0];
				DeltSyncPos = (int16)TMP;

				profibus_buf.pbus_PGout = profibus_buf.pbus_PGout + DeltSyncPos;

				syncPGout = profibus_buf.pbus_PGout;
			}
			SyncInStatus = SyncInStatustmp;
		}	
		Profitime = syncprofiTime;
		profibus_buf.pbus_PGout = syncPGout;
	}
//	else	//DP-v0/v1
//	{
//	}
	

	RDcnt = 0;
	do
	{
		pProcessDataOut = (Uint16 *)OUTPUT_ADD;
		ProcessDataOut[0] = *pProcessDataOut++;	// stw1
		ProcessDataOut[1] = *pProcessDataOut++;	// speed_low
		ProcessDataOut[2] = *pProcessDataOut++;	// speed_high
		ProcessDataOut[3] = *pProcessDataOut++;	// stw2
		ProcessDataOut[4] = *pProcessDataOut++;	// g1_stw1
		ProcessDataOut[5] = *pProcessDataOut++;	// sumcheck
		CaulSum = ProcessDataOut[0]+ProcessDataOut[1]+ProcessDataOut[2]+ProcessDataOut[3]+ProcessDataOut[4];
		if(++RDcnt > 5)
		{
			break;		// rd data error.
		}
	}while(ProcessDataOut[5] != CaulSum);

	if(RDcnt<6)
	{
		profibus_buf.pbus_STWnew = ProcessDataOut[0];
		profibus_buf.pbus_SPEEDorIqr = ((Uint32)ProcessDataOut[2]<<16)+ProcessDataOut[1];
		//profibus_buf.pbus_Iqr = ProcessDataOut[4];
	}

	ModuleStatus.all = *ModuleErrReg_ADD;
	if(ModuleStatus.bit.DPRAM_EX_Dataerr)
	{
		ModuleStatus.bit.DPRAM_EX_Dataerr = 1;
	}

	// wr input	
	pProcessDataOut = (Uint16 *)INPUT_ADD;	
	DRAM_CRC = profibus_buf.pbus_ZSW;						// zsw1
	DRAM_CRC += (Uint16) (((Uint32)Unspd) & 0x0FFFF);		// speed_l
	DRAM_CRC += (Uint16) ((((Uint32)Unspd)>>16) & 0x0FFFF);	// speed_h
	DRAM_CRC += 0;											// zsw2
	DRAM_CRC += 0;											// g1_zsw
	DRAM_CRC += (Uint16) (((Uint32)profibus_buf.pbus_PGout) & 0x0FFFF);		// xist1_low
	DRAM_CRC += (Uint16) ((((Uint32)profibus_buf.pbus_PGout)>>16) & 0x0FFFF);// xist1_high
	DRAM_CRC += Profitime;													// sumcheck_l
	DRAM_CRC += 0;													// sumcheck_h
	//profibus_back->ZSW1 = profibus_buf.pbus_ZSW;
	*pProcessDataOut++ = profibus_buf.pbus_ZSW;

	//profibus_back->NIST_B = speed;
	*pProcessDataOut++ =(Uint16) (((Uint32)Unspd) & 0xFFFF);
	*pProcessDataOut++ =(Uint16) ((((Uint32)Unspd)>>16) & 0xFFFF);

	//profibus_back->ZSW2;
	*pProcessDataOut++ = 0;
	// g1zsw
	*pProcessDataOut++ = 0;

	//profibus_back->G1_XIST1 = profibus_buf.pbus_PGout;	
	*pProcessDataOut++ =(Uint16) (((Uint32)profibus_buf.pbus_PGout) & 0xFFFF);
	*pProcessDataOut++ =(Uint16) ((((Uint32)profibus_buf.pbus_PGout)>>16) & 0xFFFF);

	//profibus_back->G1_XIST2 = Profitime;	
	*pProcessDataOut++ = Profitime;
	*pProcessDataOut++ = 0;
	*pProcessDataOut = DRAM_CRC;

	if((profibus_buf.pbus_STWnew & 0x0080) && ((profibus_buf.pbus_STWold & 0x0080) == 0x0000))
	{
		profibus_buf.pbus_STW |= 0x0080;
	}
	profibus_buf.pbus_STWold = profibus_buf.pbus_STWnew;
	profibus_buf.pbus_STW &= 0x0080;
	profibus_buf.pbus_STW |= (profibus_buf.pbus_STWnew & 0xFF7F);

	if(RDcnt > 5)
	{
		ModuleStatus.bit.DPRAM_EX_Dataerr = 1;
	}
	ModuleStatus.bit.SCANerr = 0;
	ModuleStatus.bit.HS =1;
  }  
}
/*------------------------------------------------------------------------------
**
** AnybusIsr()
**
** AnyBus interrupt routine (EXTERNAL INT 1).
**
** Activated by a low level of the AnyBus signal /IRQ (DPRAM signal /INT)
** [ABS-DGP 3.2.1 and 5.1].
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**    ISR
**
**------------------------------------------------------------------------------
*/
//interrupt void AnybusIsr( void )
void ParaDataExchange(void)
{
 Uint16 *pMailBox,*Point16;
// shakehand
   /*
   ** We're here,
   ** so the AnyBus has written something to the AnyBus Indication register.
   ** Read the register. The interrupt signal is thereby restored [ABS-DGP 5.1].
   */

	AbIndCopy  = *ABUS_INDREG_ADD;
  	AppIndCopy = *EDBH_INDREG_ADD;
//  Timers.DPRAMISR_ScanCnt = 0;
//  ModuleStatus.bit.SCANerr = 0;
// shakehanded
// check MAILBOX OUT area new message
  
  	if(ModuleStatus.bit.HS && (AbIndCopy & INDREG_HS) && ((AbIndCopy & INDREG_MOUT) != (AppIndCopy & INDREG_MOUT)))		// shake hand ok
  	{
  	 //	MAPINcnt ++;	 	
	 // receive new message
	 // read new message
		pMailBox = (Uint16 *)MOUT_ADD;
		DPRAMBus.ParComm  = *pMailBox++;			// parameter command
		DPRAMBus.ParIndex = *pMailBox++;			// parameter number
		DPRAMBus.ParDataL = *pMailBox++;			// parameter high 16bits
		DPRAMBus.ParDataH = *pMailBox++;			// parameter low  16bits
		// readed all data from map out
		AppIndCopy ^= INDREG_MOUT;

		// para process
		if(DPRAMBus.ParComm)
		{
		// idea 1:
			/*
			if((DPRAMBus.ParComm == DPRAMBusold.ParComm) 
			 &&(DPRAMBus.ParIndex == DPRAMBusold.ParIndex)
			 &&(DPRAMBus.ParDataL == DPRAMBusold.ParDataL)
			 &&(DPRAMBus.ParDataH == DPRAMBusold.ParDataH)
			 &&(WrParStatus == 2))
			{
				WrParStatus = 0;
				DPRAMApp.ParComm  = WROneWordOK;
				DPRAMApp.ParIndex = BusGropNum;
				DPRAMApp.ParDataL = 0;
				DPRAMApp.ParDataH = 0;
			}
			else
			{
				ParaProcess();
				DPRAMBusold.ParComm = DPRAMBus.ParComm ;
			 	DPRAMBusold.ParIndex = DPRAMBus.ParIndex;
			 	DPRAMBusold.ParDataL = DPRAMBus.ParDataL;
			 	DPRAMBusold.ParDataH = DPRAMBus.ParDataH;
			}*/
		// idea 2:
			///*
			if(WrParStatus == 2)
			{
				WrParStatus = 0;
				DPRAMApp.ParComm  = WROneWordOK;
				DPRAMApp.ParIndex = BusGropNum;
				DPRAMApp.ParDataL = 0;
				DPRAMApp.ParDataH = 0;
			}
			else
			{
				ParaProcess();
			}
			//*/
		// idea 3:
			/*
			if((DPRAMBus.ParComm == DPRAMBusold.ParComm) 
			 &&(DPRAMBus.ParIndex == DPRAMBusold.ParIndex)
			 &&(DPRAMBus.ParDataL == DPRAMBusold.ParDataL)
			 &&(DPRAMBus.ParDataH == DPRAMBusold.ParDataH)
			 //&&(ResParCommOK == 0)
			 )
			{
				if(WrParStatus == 2)
				{
					WrParStatus = 0;
					DPRAMApp.ParComm  = WROneWordOK;
					DPRAMApp.ParIndex = BusGropNum;
					DPRAMApp.ParDataL = 0;
					DPRAMApp.ParDataH = 0;
				}
				DPRAMBusold.ParComm = 0;
			 	DPRAMBusold.ParIndex = 0;
			 	DPRAMBusold.ParDataL = 0;
			 	DPRAMBusold.ParDataH = 0;
			}
			else
			{									
				DPRAMBusold.ParComm = DPRAMBus.ParComm ;
			 	DPRAMBusold.ParIndex = DPRAMBus.ParIndex;
			 	DPRAMBusold.ParDataL = DPRAMBus.ParDataL;
			 	DPRAMBusold.ParDataH = DPRAMBus.ParDataH;
				ParaProcess();
			}	
			*/		
		}
		else
		{// IDLECOMM			
			if(WrParStatus == 2)
			{
				WrParStatus = 0;
				DPRAMApp.ParComm  = WROneWordOK;
				DPRAMApp.ParIndex = BusGropNum;				
			}
			else
			{
				DPRAMApp.ParComm  = 0;
				DPRAMApp.ParIndex = 0;
			}	
			DPRAMApp.ParDataL = 0;
			DPRAMApp.ParDataH = 0;			
		}
		if(GuardedFlg == 1)
		{
			GuardedFlg = 2;
			DPRAMApp.ParComm |= 0x0080;
		}
		
		if((AbIndCopy & INDREG_MIN) == (AppIndCopy & INDREG_MIN))
		{// write respond message			
			pMailBox = (Uint16 *)MIN_ADD;
			Point16 = (Uint16 *)((Uint32)&DPRAMApp.ParComm);
			*pMailBox++ = *Point16++;
			*pMailBox++ = *Point16++;
			*pMailBox++ = *Point16++;
			*pMailBox++ = *Point16++;			
			AppIndCopy ^= INDREG_MIN;			
	 		//WriteAppIndBits(0,0,INDREG_MIN);	 			
	 	}	 	
		*EDBH_INDREG_ADD = AppIndCopy;			
	}
//	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
} /* end of AnybusIsr() */

/*******************************************************************************
**
** Functions
**
********************************************************************************
*/
/*------------------------------------------------------------------------------
**
** ParaProcess()
**
** 
** DPRAM 非周期通道处理
** 
** 
**
**------------------------------------------------------------------------------
**
** Inputs:
**    
**    
** Outputs:
**    
** 
** Usage:
**    ParaProcess()
**
**------------------------------------------------------------------------------
*/
void ParaProcess(void)
{Uint16 ret,userpassword,PnGropBeg,PnGropBegNum,PnFactNum;
 Uint16 *pGuard,*tmp_pt;
 int16 tmp_Index;
 Uint32 Addresstmp;
 HEX_DISP hexEdit,hexEditMax,hexEditMin;
	const CO_objectDictionaryEntry* pODE;
			if(DPRAMBus.ParIndex < 900)	// 900以下不需查表，参数区
			{
				PnGropBeg = DPRAMBus.ParIndex/100;
				PnGropBegNum = DPRAMBus.ParIndex%100;
				if((PnGropBeg < 8) || (password == PROFIBUSPASSWORD))
				{
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
							PnFactNum = 0xFFFF;	// 非法地址
							break;
					}
					if(PnFactNum > PnMax)		// 非法地址
					{
						userpassword = 1;		// 非法地址 非法访问
					}
					else if(PnAttribute[PnFactNum][2])
					{						
						userpassword = 0;		// 合法地址 合法访问
					}
					else//  最大值== 0
					{
						userpassword = 2;		// 保留地址 非法访问
					}
				}
				else 				// 权限不够，非法访问
				{					
					userpassword = 3;
				}
			}
			else	// 900以上查表
			{
				userpassword = 5;
				pODE = DPRAM_CO_FindEntryInOD(DPRAMBus.ParIndex,0x00);
				if(!pODE)
				{
					userpassword = 1;
				}
				else
				{
					userpassword = 0;
				}
			}/*
			if(userpassword)
			{
				// error: object does not exist in OD
				DPRAMApp.ParComm  = MapInErr;
				DPRAMApp.ParComm |= ErrInvalidPara;	
				DPRAMApp.ParDataL = 0x0000;
				DPRAMApp.ParDataH = 0x0000;	
				DPRAMApp.ParIndex = DPRAMBus.ParIndex;
			}
			else
			{*/
				switch(DPRAMBus.ParComm)
				{
					case RDdata:
						if(userpassword)
						{
							DPRAMApp.ParComm = RDErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn
						{
							DPRAMApp.ParDataL = Pn[PnFactNum];
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParComm = RDOneWordOK;				
						}
						else // common para
						{	
							DPRAMApp.ParDataH = 0x0000;
							ret = DPRAM_CO_OD_Read(pODE, (void*)&DPRAMApp.ParDataL, 1);
							if(!ret)
							{// ok
								DPRAMApp.ParComm = RDOneWordOK;		
								DPRAMApp.ParDataH = 0x0000;						
							}
							else 
							{// err
								ret = DPRAM_CO_OD_Read(pODE, (void*)&DPRAMApp.ParDataL, 2);
								if(!ret)
								{// ok
									DPRAMApp.ParComm = RDTwoWordOK;								
								}
								else
								{
									DPRAMApp.ParComm = RDErr;
									DPRAMApp.ParDataL = 0x0000;
									DPRAMApp.ParDataH = 0x0000;
									DPRAMApp.ParComm |= ErrRD_WR_Para;	// error: ErrRD_WR_Para (attribute)																	
								}
							}							
						}
						DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						break;
					case RDOneWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = RDErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn
						{
							DPRAMApp.ParDataL = Pn[PnFactNum];
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParComm = RDOneWordOK;				
						}
						else // common para
						{	
							DPRAMApp.ParDataH = 0x0000;
							ret = DPRAM_CO_OD_Read(pODE, (void*)&DPRAMApp.ParDataL, 1);
							if(!ret)
							{// ok
								DPRAMApp.ParComm = RDOneWordOK;								
							}
							else 
							{// err
								DPRAMApp.ParComm = RDErr;
								DPRAMApp.ParDataL = 0x0000;
								DPRAMApp.ParDataH = 0x0000;
								if(ret == 1)	// error: ErrRD_WR_Para (attribute)
								{
									DPRAMApp.ParComm |= ErrRD_WR_Para;
								}
								else if(ret == 2)	// error: ErrLessData
								{
									DPRAMApp.ParComm |= ErrLessData;
								}
								else				// error: ErrMuchData
								{
									DPRAMApp.ParComm |= ErrMuchData;
								}
							}							
						}
						DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						//DPRAMApp.IOLength = 1;
						break;
					case RDTwoWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = RDErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn
						{
							DPRAMApp.ParComm = RDErr;
							DPRAMApp.ParComm |= ErrLessData;
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;					
						}
						else // common para
						{
							ret = DPRAM_CO_OD_Read(pODE, (void*)&DPRAMApp.ParDataL, 2);
							if(!ret)
							{
							// ok
								DPRAMApp.ParComm = RDTwoWordOK;								
							}
							else 
							{// err
								DPRAMApp.ParComm = RDErr;
								DPRAMApp.ParDataL = 0x0000;
								DPRAMApp.ParDataH = 0x0000;
								if(ret == 1)	// error: ErrRD_WR_Para (attribute)
								{
									DPRAMApp.ParComm |= ErrRD_WR_Para;
								}
								else if(ret == 2)	// error: ErrLessData
								{
									DPRAMApp.ParComm |= ErrLessData;
								}
								else				// error: ErrMuchData
								{
									DPRAMApp.ParComm |= ErrMuchData;
								}
							}
						}
						DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						//DPRAMApp.IOLength = 2;
						break;
					case WROneWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = WRErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn
						{
							if((WrParStatus)									// <>0，上次WRING未完成，invalid wr err
							 ||((MainMenuPage==1) && MenuLayer))				// 参数在编辑状态禁止通讯修改参数
							{
								DPRAMApp.ParComm = WRErr;	
								DPRAMApp.ParComm |= ErrInvalidComm;
							}
							else
							{
								if(DPRAMBus.ParDataL == Pn[PnFactNum])
								{
									DPRAMApp.ParComm = WROneWordOK;										
								}
								else if(PnAttribute[PnFactNum][0] == 1)
								{
									if((int16)DPRAMBus.ParDataL < (int16)PnAttribute[PnFactNum][1])
									{// error: ErrLowValue 
										DPRAMApp.ParComm = WRErr;
										DPRAMApp.ParComm |= ErrLowValue;
									}
									else if((int16)DPRAMBus.ParDataL > (int16)PnAttribute[PnFactNum][2])
									{// error: ErrHighValue 
										DPRAMApp.ParComm = WRErr;	
										DPRAMApp.ParComm |= ErrHighValue; 
									}
									else if((int16)DPRAMBus.ParDataL == (int16)Pn[PnFactNum])
									{
										DPRAMApp.ParComm = WROneWordOK;
									}
									else
									{// ok
										DPRAMApp.ParComm = WRing;										
										WrParStatus = 1;
										BusEditIndex = PnFactNum;
										BusEditContent = DPRAMBus.ParDataL;	
										BusGropNum = DPRAMBus.ParIndex;
									}
								}
								else if(PnAttribute[PnFactNum][0] == 3)
								{
									hexEdit.all = DPRAMBus.ParDataL;
									hexEditMin.all = PnAttribute[PnFactNum][1];	
									hexEditMax.all = PnAttribute[PnFactNum][2];										
									if((hexEdit.hex.hex00 < hexEditMin.hex.hex00)
									 ||(hexEdit.hex.hex01 < hexEditMin.hex.hex01)
									 ||(hexEdit.hex.hex02 < hexEditMin.hex.hex02)
									 ||(hexEdit.hex.hex03 < hexEditMin.hex.hex03))
									{// error: ErrLowValue 
										DPRAMApp.ParComm = WRErr;
										DPRAMApp.ParComm |= ErrLowValue;
									}
									else if((hexEdit.hex.hex00 > hexEditMax.hex.hex00)
									 	 ||(hexEdit.hex.hex01 > hexEditMax.hex.hex01)
									 	 ||(hexEdit.hex.hex02 > hexEditMax.hex.hex02)
									 	 ||(hexEdit.hex.hex03 > hexEditMax.hex.hex03))
									{// error: ErrHighValue 
										DPRAMApp.ParComm = WRErr;	
										DPRAMApp.ParComm |= ErrHighValue; 
									}
									else
									{// ok
										DPRAMApp.ParComm = WRing;										
										WrParStatus = 1;
										BusEditIndex = PnFactNum;
										BusEditContent = DPRAMBus.ParDataL;	
										BusGropNum = DPRAMBus.ParIndex;
									}
								}
								else //if(PnAttribute[PnFactNum][0] == 0 \\ == 2)
								{
									if(DPRAMBus.ParDataL < PnAttribute[PnFactNum][1] )
									{// error: ErrLowValue 
										DPRAMApp.ParComm = WRErr;	
										DPRAMApp.ParComm |= ErrLowValue;
									}
									else if(DPRAMBus.ParDataL > PnAttribute[PnFactNum][2])
									{// error: ErrHighValue 
										DPRAMApp.ParComm = WRErr;			
										DPRAMApp.ParComm |= ErrHighValue;
									}
									else if(DPRAMBus.ParDataL == Pn[PnFactNum])
									{
										DPRAMApp.ParComm = WROneWordOK;
									}
									else
									{// ok
										DPRAMApp.ParComm = WRing;
										WrParStatus = 1;
										BusEditIndex = PnFactNum;
										BusEditContent = DPRAMBus.ParDataL;	
										BusGropNum = DPRAMBus.ParIndex;	
									}
								}
							
								DPRAMApp.ParIndex = DPRAMBus.ParIndex;
								DPRAMApp.ParDataH = 0x0000;
								DPRAMApp.ParDataL = 0x0000;	
							}											
						}
						else // common para
						{
							ret = DPRAM_CO_OD_Write(pODE, (void*)&DPRAMBus.ParDataL, 1);
							if(!ret)
							{
							// ok
								DPRAMApp.ParComm = WROneWordOK;																
							}
							else 
							{// err
								DPRAMApp.ParComm = WRErr;
								if(ret == 1)	// error: ErrRD_WR_Para (attribute)
								{
									DPRAMApp.ParComm |= ErrRD_WR_Para;
								}
								else if(ret == 2)	// error: ErrLessData
								{
									DPRAMApp.ParComm |= ErrLessData;
								}
								else				// error: ErrMuchData
								{
									DPRAMApp.ParComm |= ErrMuchData;
								}
							}
						}
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;
						DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						//DPRAMApp.IOLength = 1;
						break;
					case WRTwoWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = WRErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn
						{
							// error: 16bits
							// error: ErrMuchData
							DPRAMApp.ParComm = WRErr;
							DPRAMApp.ParComm |= ErrMuchData;					
						}
						else // common para
						{
							ret = DPRAM_CO_OD_Write(pODE, (void*)&DPRAMBus.ParDataL, 2);
							if(!ret)
							{
							// ok
								DPRAMApp.ParComm = WRTwoWordOK;						
							}
							else 
							{// err
								DPRAMApp.ParComm = WRErr;
								if(ret == 1)	// error: ErrRD_WR_Para (attribute)
								{
									DPRAMApp.ParComm |= ErrRD_WR_Para;
								}
								else if(ret == 2)	// error: ErrLessData
								{
									DPRAMApp.ParComm |= ErrLessData;
								}
								else				// error: ErrMuchData
								{
									DPRAMApp.ParComm |= ErrMuchData;
								}
							}
						}
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;
						DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						//DPRAMApp.IOLength = 2;	
						break;
					case UpdateGuard:
						for(ret=0,tmp_Index=0;tmp_Index < GuardStack.MaxIndex; tmp_Index++)
						{
							if(GuardStack.Guarded[tmp_Index])
							{
								GuardStack.Guarded[tmp_Index] = 0;
								GuardedFlg = 0;
								ret = 1;
								break;
							}
						}
						if(ret)
						{
							if(GuardStack.Length[tmp_Index] == 1)		// one word
							{
								DPRAMApp.ParComm  = TrGuardOneWord;	
								DPRAMApp.ParIndex = GuardStack.GuardIndex[tmp_Index];
								pGuard = (Uint16 *)GuardStack.Address[tmp_Index];
								DPRAMApp.ParDataL = *pGuard;								
								DPRAMApp.ParDataH = 0;
								GuardStack.GuardData[GuardStack.MaxIndex] = DPRAMApp.ParDataL;
							}
							else										// two word
							{
								DPRAMApp.ParComm  = TrGuardTwoWord;	
								DPRAMApp.ParIndex = GuardStack.GuardIndex[tmp_Index];									
								pGuard = (Uint16 *)GuardStack.Address[tmp_Index];
								DPRAMApp.ParDataL = *pGuard++;
								DPRAMApp.ParDataH = *pGuard;	
								GuardStack.GuardData[GuardStack.MaxIndex] = DPRAMApp.ParDataH;
								GuardStack.GuardData[GuardStack.MaxIndex] = GuardStack.GuardData[GuardStack.MaxIndex]<<16 + DPRAMApp.ParDataL;
							}
						}
						else
						{
							// ...
						}
						break;
					case SetGuardOneWord:					// GuardStack
						if(userpassword)
						{
							DPRAMApp.ParComm = GuardReqErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn 不能MAP IN/OUT
						{									
							if(GuardStack.MaxIndex < MaxGuardnum)
							{
								GuardStack.Address[GuardStack.MaxIndex] = (Uint32)&Pn[PnFactNum];
								GuardStack.GuardIndex[GuardStack.MaxIndex] = DPRAMBus.ParIndex;
								GuardStack.Length[GuardStack.MaxIndex] = 1;
								GuardStack.Guarded[GuardStack.MaxIndex] = 0;								
								DPRAMApp.ParComm  = GuardReqOK;	
								DPRAMApp.ParIndex = DPRAMBus.ParIndex;
								DPRAMApp.ParDataL = Pn[PnFactNum];
								DPRAMApp.ParDataH = 0;
								GuardStack.GuardData[GuardStack.MaxIndex] = DPRAMApp.ParDataL;
								GuardStack.MaxIndex++;
								//DPRAMApp.IOLength = pODE->length;
							}
							else
							{
								DPRAMApp.ParComm  = GuardReqErr;
								DPRAMApp.ParComm |= ErrVender;	
								DPRAMApp.ParIndex = DPRAMBus.ParIndex;
								DPRAMApp.ParDataL = 0;
								DPRAMApp.ParDataH = 0;
							}									
						}
						else if((pODE->attribute&0x07) == ATTR_WO)
						{
							// error : invalid read
							DPRAMApp.ParComm  = GuardReqErr;
							DPRAMApp.ParComm |= ErrRD_WR_Para;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							DPRAMApp.ParDataL = 0;
							DPRAMApp.ParDataH = 0;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(pODE->length != 1)
						{
							// error : invalid read
							DPRAMApp.ParComm  = GuardReqErr;
							DPRAMApp.ParComm |= ErrMuchData;								
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							DPRAMApp.ParDataL = 0;
							DPRAMApp.ParDataH = 0;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(MapIn.MaxIndex < MaxIOnum)
						{
							GuardStack.Address[GuardStack.MaxIndex] = (Uint32)pODE->pData;
							GuardStack.GuardIndex[GuardStack.MaxIndex] = DPRAMBus.ParIndex;
							GuardStack.Length[GuardStack.MaxIndex] = 1;
							GuardStack.Guarded[GuardStack.MaxIndex] = 0;							
							DPRAMApp.ParComm  = GuardReqOK;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							pGuard = (Uint16 *)GuardStack.Address[GuardStack.MaxIndex];
							DPRAMApp.ParDataL = *pGuard;
							//DPRAMApp.ParDataL = *((void *)((Uint32)pODE->pData));
							DPRAMApp.ParDataH = 0;
							//DPRAMApp.IOLength = pODE->length;
							GuardStack.GuardData[GuardStack.MaxIndex] = DPRAMApp.ParDataL;
							GuardStack.MaxIndex++;
						}
						else
						{
							// error : map in is ov
							DPRAMApp.ParComm  = GuardReqErr;
							DPRAMApp.ParComm |= ErrVender;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							DPRAMApp.ParDataL = 0;
							DPRAMApp.ParDataH = 0;
							//DPRAMApp.IOLength = pODE->length;
						}
						break;
					case SetGuardTwoWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = GuardReqErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn 不能MAP IN/OUT
						{
							DPRAMApp.ParComm  = GuardReqErr;
							DPRAMApp.ParComm |= ErrLessData;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.ParComm = MapOutErr;
							//DPRAMApp.ParComm |= ErrLessData;
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;										
						}
						else if((pODE->attribute&0x07) == ATTR_WO)
						{
							// error : invalid read
							DPRAMApp.ParComm  = GuardReqErr;
							DPRAMApp.ParComm |= ErrRD_WR_Para;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							DPRAMApp.ParDataL = 0;
							DPRAMApp.ParDataH = 0;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(pODE->length != 2)
						{
							// error : invalid read
							DPRAMApp.ParComm  = GuardReqErr;
							DPRAMApp.ParComm |= ErrLessData;								
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							DPRAMApp.ParDataL = 0;
							DPRAMApp.ParDataH = 0;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(MapIn.MaxIndex < MaxIOnum)
						{
							GuardStack.Address[GuardStack.MaxIndex] = (Uint32)pODE->pData;
							GuardStack.GuardIndex[GuardStack.MaxIndex] = DPRAMBus.ParIndex;
							GuardStack.Length[GuardStack.MaxIndex] = 2;
							GuardStack.Guarded[GuardStack.MaxIndex] = 0;
							DPRAMApp.ParComm  = GuardReqOK;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							pGuard = (Uint16 *)GuardStack.Address[GuardStack.MaxIndex];
							DPRAMApp.ParDataL = *pGuard++;
							DPRAMApp.ParDataH = *pGuard;
							//DPRAMApp.IOLength = pODE->length;
							GuardStack.GuardData[GuardStack.MaxIndex] = DPRAMApp.ParDataH;
							GuardStack.GuardData[GuardStack.MaxIndex] = GuardStack.GuardData[GuardStack.MaxIndex]<<16 + DPRAMApp.ParDataL;
							GuardStack.MaxIndex++;
						}
						else
						{
							// error : map in is ov
							DPRAMApp.ParComm  = GuardReqErr;
							DPRAMApp.ParComm |= ErrVender;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							DPRAMApp.ParDataL = 0;
							DPRAMApp.ParDataH = 0;
							//DPRAMApp.IOLength = pODE->length;
						}
						break;
					case RemoveIn:
						if(userpassword)
						{
							DPRAMApp.ParComm = MapInErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)		// Pn不能MAP IN/OUT
						{
							ret = 0;
						}
						else
						{
							Addresstmp = (Uint32)pODE->pData;
							for(ret=0,tmp_Index=0;tmp_Index < MapIn.MaxIndex; tmp_Index++)
							{
								if(Addresstmp == MapIn.Address[tmp_Index])
								{
									ret = 1;
									break;
								}
							}
						}
						if(ret)
						{							
							MapIn.MaxIndex--;// delete one
							for(;tmp_Index < MapIn.MaxIndex;tmp_Index++)
							{
								MapIn.Address[tmp_Index] = MapIn.Address[tmp_Index+1];
								MapIn.Length[tmp_Index] = MapIn.Length[tmp_Index+1]; 
							}
							MapIn.Length[tmp_Index] = 0;
							DPRAMApp.ParComm  = MapInOK;
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;				
						}
						else
						{
							// error: invalid remove
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;						
						}
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;
						break;
					case MapInOneWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = MapInErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn 不能MAP IN/OUT
						{									
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//MapIn.Address[MapIn.MaxIndex] = (Uint32)&Pn[DPRAMBus.ParIndex];
							//MapIn.Length[MapIn.MaxIndex] = 1;
							//MapIn.MaxIndex++;
							//DPRAMApp.ParComm  = MapInOK;	
							//DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;		
						}
						else if((pODE->attribute&0x07) == ATTR_WO)
						{
							// error : invalid read
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrRD_WR_Para;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(pODE->length != 1)
						{
							// error : invalid read
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrMuchData;								
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(MapIn.MaxIndex < MaxIOnum)
						{
							MapIn.Address[MapIn.MaxIndex] = (Uint32)pODE->pData;
							MapIn.Length[MapIn.MaxIndex] = 1;
							MapIn.MaxIndex++;
							DPRAMApp.ParComm  = MapInOK;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else
						{
							// error : map in is ov
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrVender;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;
						break;
					case MapInTwoWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = MapInErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn 不能MAP IN/OUT
						{
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.ParComm = MapInErr;
						}
						else if((pODE->attribute&0x07) == ATTR_WO)
						{
							// error : invalid read
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrRD_WR_Para;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;

						}
						else if(pODE->length != 2)
						{
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrLessData;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(MapIn.MaxIndex < MaxIOnum)
						{
							MapIn.Address[MapIn.MaxIndex] = (Uint32)pODE->pData;
							MapIn.Length[MapIn.MaxIndex] = 2;
							MapIn.MaxIndex++;
							DPRAMApp.ParComm  = MapInOK;
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else
						{
							// error : map in is ov
							DPRAMApp.ParComm  = MapInErr;
							DPRAMApp.ParComm |= ErrVender;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;
						break;
					case ClrMapIn:
						MapIn.MaxIndex = 0;
						DPRAMApp.ParComm  = MapInOK;	
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;	
						DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						//DPRAMApp.IOLength = pODE->length;
						tmp_Index = sizeof(MapIn);
						tmp_pt = (void *)&MapIn;
						while(tmp_Index--)
						{
							*tmp_pt++ = 0;
						}
						break;
					case RemoveOut:
						if(userpassword)
						{
							DPRAMApp.ParComm = MapOutErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn 不能MAP IN/OUT
						{
							ret = 0;
						}
						else
						{
							Addresstmp = (Uint32)pODE->pData;
							for(ret=0,tmp_Index=0;tmp_Index < MapOut.MaxIndex; tmp_Index++)
							{
								if(Addresstmp == MapOut.Address[tmp_Index])
								{
									ret = 1;
									break;
								}
							}
						}						
						if(ret)
						{							
							MapOut.MaxIndex--;// delete one
							for(;tmp_Index < MapOut.MaxIndex;tmp_Index++)
							{
								MapOut.Address[tmp_Index] = MapOut.Address[tmp_Index+1];
								MapOut.Length[tmp_Index] = MapOut.Length[tmp_Index+1];
							}
							MapOut.Length[tmp_Index] = 0;
							DPRAMApp.ParComm  = MapOutOK;
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;						
						}
						else
						{
							// error: invalid remove
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;
						break;
					case MapOutOneWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = MapOutErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn 不能MAP IN/OUT
						{									
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//MapOut.Address[MapOut.MaxIndex] = (Uint32)&Pn[DPRAMBus.ParIndex];
							//MapOut.Length[MapOut.MaxIndex] = 1;
							//MapOut.MaxIndex++;
							//DPRAMApp.ParComm  = MapOutOK;	
							//DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;		
						}
						else if((pODE->attribute&0x07) == ATTR_RO || (pODE->attribute&0x07) == ATTR_CO)
						{
							// error : invalid read
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrRD_WR_Para;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(pODE->length != 1)
						{
							// error : invalid read
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrMuchData;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(MapOut.MaxIndex < MaxIOnum)
						{
							MapOut.Address[MapOut.MaxIndex] = (Uint32)pODE->pData;
							MapOut.Length[MapOut.MaxIndex] = 1;
							MapOut.MaxIndex++;
							DPRAMApp.ParComm  = MapOutOK;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else
						{
							// error : map out is ov
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrVender;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;						
						}
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;
						break;
					case MapOutTwoWord:
						if(userpassword)
						{
							DPRAMApp.ParComm = MapOutErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						}
						else if(DPRAMBus.ParIndex < 900)							// Pn 不能MAP IN/OUT
						{
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrInvalidPara;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.ParComm = MapOutErr;
							//DPRAMApp.ParComm |= ErrLessData;
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
						}
						else if((pODE->attribute&0x07) == ATTR_RO || (pODE->attribute&0x07) == ATTR_CO)
						{
							// error : invalid read
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrRD_WR_Para;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else if(pODE->length != 2)
						{
							// error : invalid read
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrLessData;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;						
						}
						else if(MapOut.MaxIndex < MaxIOnum)
						{
							MapOut.Address[MapOut.MaxIndex] = (Uint32)pODE->pData;
							MapOut.Length[MapOut.MaxIndex] = 2;
							MapOut.MaxIndex++;
							DPRAMApp.ParComm  = MapOutOK;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						else
						{
							// error : map out is ov
							DPRAMApp.ParComm  = MapOutErr;
							DPRAMApp.ParComm |= ErrVender;	
							DPRAMApp.ParDataL = 0x0000;
							DPRAMApp.ParDataH = 0x0000;	
							DPRAMApp.ParIndex = DPRAMBus.ParIndex;
							//DPRAMApp.IOLength = pODE->length;
						}
						break;
					case ClrMapOut:
						MapOut.MaxIndex = 0;
						DPRAMApp.ParComm  = MapOutOK;	
						DPRAMApp.ParDataL = 0x0000;
						DPRAMApp.ParDataH = 0x0000;	
						DPRAMApp.ParIndex = DPRAMBus.ParIndex;
						//DPRAMApp.IOLength = pODE->length;
						tmp_Index = sizeof(MapOut);
						tmp_pt = (void *)&MapOut;
						while(tmp_Index--)
						{
							*tmp_pt++ = 0;
						}
						break;
					case AbortReq:
						break;
					case WRBusStatus:
						FieldbusStatus.all = DPRAMBus.ParDataL;
						DPRAMApp.ParComm  = WRBusStatusOK;	
						DPRAMApp.ParIndex = 0;
						DPRAMApp.ParDataL = FieldbusStatus.all;
						DPRAMApp.ParDataH = 0x0000;	
						if(FieldbusStatus.bit.DPRAM_EX_Dataerr)	
						{// DPRAM交换错
							ModuleStatus.bit.DPRAM_EX_Dataerr = 1;
						}				
						break;
					default:
						break;
				}
			//}
}
/*******************************************************************************
**
** Functions
**
********************************************************************************
*/
/*------------------------------------------------------------------------------
**
** WrParFRAM()
**
** 
** write para into FRAM
** 
** 
**
**------------------------------------------------------------------------------
**
** Inputs:
**    
**    
** Outputs:
**    
** 
** Usage:
**    WrParFRAM()
**
**------------------------------------------------------------------------------
*/
void WrParFRAM(void)
{
//	if(WrParFRAMFlg == 0x8000)
	if(WrParStatus == 1)
	{
		//WrParFRAMFlg &= 0x7FFF;			
		Pn[SumAddr] -= Pn[BusEditIndex];	// 									
		CheckSum -= Pn[BusEditIndex];
		Pn[BusEditIndex] = BusEditContent;
		WrParStatus = 2;
		Pn[SumAddr] += Pn[BusEditIndex];
		CheckSum += Pn[BusEditIndex];
		SPI_WriteFRAM(BusEditIndex,&Pn[BusEditIndex],1);
		SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
	}
}

/*------------------------------------------------------------------------------
**
** CheckGuard()
**
** 
** Check Guarded Data change
** 
** 
**
**------------------------------------------------------------------------------
**
** Inputs:
**    
**    
** Outputs:
**    
** 
** Usage:
**    CheckGuard()
**
**------------------------------------------------------------------------------
*/
void CheckGuard(void)
{Uint16 i;
 Uint16 *pGuard1;
 Uint32 *pGuard2;
 Uint32 tmp;
	if(GuardedFlg == 0)
	{
		for(i=0; i<GuardStack.MaxIndex; i++)
		{
			if(GuardStack.Length[i] == 1)	// one word
			{
				pGuard1 = (Uint16 *)GuardStack.Address[i];
				tmp = *pGuard1;
			}
			else							// two word
			{
				pGuard2 = (Uint32 *)GuardStack.Address[i];
				tmp = *pGuard2;
			}
			if(tmp != GuardStack.GuardData[i])
			{
				GuardStack.Guarded[i] = 1;
				GuardedFlg = 1;
				break;
			}		
		}
	}
}

/*------------------------------------------------------------------------------
**
** AppWriteDPRAM()
**
** App(EDBH-F2812) write DPRAM.
**
** if return 0, AppWrite ok
** if return 1, AppWrite error
**
**------------------------------------------------------------------------------
**
** Inputs:
**    SourceAddr
**    DPRAMAddr
** Outputs:
**    WriteSuccess            = 0: The Appwrite is completed.
**                            > 1: The Appwrite failed.
** Usage:
**    AppWriteDPRAM(SourceAddr,DPRAMAddr)
**
**------------------------------------------------------------------------------
*/
Uint16 AppWriteDPRAM(Uint16 *SourceAddr,Uint16 *DPRAMAddr)
{volatile Uint16 WriteDPRAMcnt,temp1,temp2;
 volatile Uint16 *pTemp = CheckModuleD_ADD;
	WriteDPRAMcnt = 0;
	do
	{
		temp2 = *SourceAddr;
		*DPRAMAddr = temp2;
		asm("	rpt #31||	NOP");
		*pTemp = *SourceAddr ^ 0xFFFF;
		asm("	rpt #31||	NOP");
		temp1 = *DPRAMAddr;
		if(++WriteDPRAMcnt > 9)
		{
			return 1;
		}
	}while(temp1 != temp2);
	return 0;
}

Uint16 AppWriteDPRAMREG(Uint16 *SourceAddr,Uint16 *DPRAMAddr)
{Uint16 WriteDPRAMcnt;
	WriteDPRAMcnt = 0;
	do
	{
		*DPRAMAddr = *SourceAddr;
		if(++WriteDPRAMcnt > 9)
		{
			return 1;
		}
	}while(*DPRAMAddr != *SourceAddr);
	return 0;
}

Uint16 AppWriteMapData(Uint16 *SourceAddr,Uint16 *DPRAMAddr)
{Uint16 WriteDPRAMcnt;
	WriteDPRAMcnt = 0;
	do
	{
		*DPRAMAddr = *SourceAddr;
		if(++WriteDPRAMcnt > 2)
		{
			return 1;
		}
	}while(*DPRAMAddr != *SourceAddr);
	return 0;
}

/*------------------------------------------------------------------------------
**
** CheckModule()
**
** 
** 检测通信板是否正确插上
** 
** 
**
**------------------------------------------------------------------------------
**
** Inputs:
**    
**    
** Outputs:
** 							  = 0: 通信板正确连接   
**                            = 1: 通信板未正确连接 
** Usage:
**    CheckModule()
**
**------------------------------------------------------------------------------
*/
void CheckModule(void)
{Uint16 WRErrCnt,tmp;
	WRErrCnt = 0;

	tmp = 0x5555;
	WRErrCnt += AppWriteDPRAM((void*)&tmp,(Uint16 *)CheckModuleA_ADD);
	tmp = 0xAAAA;
	WRErrCnt += AppWriteDPRAM((void*)&tmp,(Uint16 *)CheckModuleB_ADD);
	tmp = 0x0000;
	WRErrCnt += AppWriteDPRAM((void*)&tmp,(Uint16 *)CheckModuleC_ADD);
	if(WRErrCnt > 1)
	{
		ModuleStatus.bit.DPRAMerr = 1;
	}
	else
	{
	//	ModuleStatus.bit.DPRAMerr = 0;
	}
}
/*------------------------------------------------------------------------------
**
** ClearMapInOutMatrix()
**
** clear MapIn,MapOut,MapInBuf,GuardStack space
**
** 
** 
**
**------------------------------------------------------------------------------
**
** Inputs:
**    
**    
** Outputs:
**                			
**                          
** Usage:
**    ClearMapInOutMatrix();
**
**------------------------------------------------------------------------------
*/
void ClearMapInOutMatrix(void)
{Uint16 tmp1;
 Uint16 *tmp_pt;

	tmp1 = sizeof(MapIn);
	tmp_pt = (void *)&MapIn;
	while(tmp1--)
	{
		*tmp_pt++ = 0;
	}

	tmp1 = sizeof(MapOut);
	tmp_pt = (void *)&MapOut;
	while(tmp1--)
	{
		*tmp_pt++ = 0;
	}

	tmp1 = sizeof(GuardStack);
	tmp_pt = (void *)&GuardStack;
	while(tmp1--)
	{
		*tmp_pt++ = 0;
	}
}
void InitialMapInOutMatrix(void)
{Uint16 tmp1;
	MapIn.MaxIndex = 4;
	tmp1 = MapIn.MaxIndex;
	for(tmp1 = 0; tmp1 < MapIn.MaxIndex; tmp1++)
	{
		MapIn.Address[tmp1] = 0x003F8198+tmp1;
		MapIn.Length[tmp1] = 1;
	}
	MapOut.MaxIndex = 3;
	tmp1 = MapOut.MaxIndex;
	for(tmp1 = 0; tmp1 < MapOut.MaxIndex; tmp1++)
	{
		MapOut.Address[tmp1] = 0x003F8200+tmp1;
		MapOut.Length[tmp1] = 1;
	}
}
void PowerUpInitHandshake(void)
{
// clear MapIn MapOut
	ClearMapInOutMatrix();
	
// Initial MapIn MapOut
// InitialMapInOutMatrix();

// InitialHandshake
	InitialShakehandLength = 0;
	AbIndCopy  = *ABUS_INDREG_ADD;
	AppIndCopy = *EDBH_INDREG_ADD;
	Timers.DPRAM_HS_Delay = 0;
	Timers.DPRAM_HS_ScanCnt = 0;
	Timers.DPRAMISR_ScanCnt = 0;
	ModuleStatus.all = 0;
	FieldbusStatus.all = 0;
}
Uint16 InitialHandshake(void)
{Uint16 WriteDPRAMcnt,I;
 Uint16 *pTest;
// InitialHandshake
  //if(!(*ABUS_INDREG_ADD & INDREG_HS))
  if(ModuleStatus.bit.HS == 0)
  {
	// appIND = 0x00	
	if((InitialShakehandLength == 0) && (Timers.DPRAM_HS_Delay > TIME_200MS))	// delay 200ms
	{		
		WriteDPRAMcnt = 0;
		do
		{
			AppIndCopy = 0x00;
			*EDBH_INDREG_ADD = 0x00;
			if(++WriteDPRAMcnt > 10)
			{
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}while(*EDBH_INDREG_ADD != 0x00);
		InitialShakehandLength = 1;	
		ModuleStatus.bit.DPRAMerr = 0;	
	}

	if((InitialShakehandLength == 1) && (*ABUS_INDREG_ADD == 0x80))
	{
		InitialShakehandLength = 2;
	}

	if(InitialShakehandLength == 2)
	{		
		// appIND = 0x80
		WriteDPRAMcnt = 0;
		do
		{
			AppIndCopy = 0x80;
			*EDBH_INDREG_ADD = 0x80;
			if(++WriteDPRAMcnt > 10)
			{
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}while(*EDBH_INDREG_ADD != 0x80);
		InitialShakehandLength = 3;
		ModuleStatus.bit.DPRAMerr = 0;
	}
	
	if((InitialShakehandLength == 3) && (*ABUS_INDREG_ADD  == 0xC0))
	{		
	// test RAM 1
		//WR
		pTest = (Uint16 *)INPUT_ADD;
		for(I=0; I<0x000A; I++)
		{
			*pTest++ = 0x5555;		
		}
		pTest = (Uint16 *)OUTPUT_ADD;
		for(I=0; I<0x000A; I++)
		{
			*pTest++ = 0x5555;		
		}
		pTest = (Uint16 *)MIN_ADD;
		for(I=0; I<0x000F; I++)
		{
			*pTest++ = 0x5555;		
		}
		pTest = (Uint16 *)MOUT_ADD;
		for(I=0; I<0x000F; I++)
		{
			*pTest++ = 0x5555;		
		}
		pTest = (Uint16 *)ModuleSoftVer_ADD;
		*pTest= 0x5555;	
		pTest = (Uint16 *)ModuleErrReg_ADD;
		*pTest= 0x5555;	
		//RD
		pTest = (Uint16 *)INPUT_ADD;		
		for(I=0; I<0x000A; I++)
		{
			if(*pTest++ != 0x5555)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)OUTPUT_ADD;		
		for(I=0; I<0x000A; I++)
		{
			if(*pTest++ != 0x5555)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)MIN_ADD;		
		for(I=0; I<0x000F; I++)
		{
			if(*pTest++ != 0x5555)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)MOUT_ADD;		
		for(I=0; I<0x000F; I++)
		{
			if(*pTest++ != 0x5555)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)ModuleSoftVer_ADD;
		if(*pTest != 0x5555)
		{
			// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
		}
		pTest = (Uint16 *)ModuleErrReg_ADD;
		if(*pTest != 0x5555)
		{
			// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
		}
	// test RAM 2
		//WR
		pTest = (Uint16 *)INPUT_ADD;
		for(I=0; I<0x000A; I++)
		{
			*pTest++ = 0xAAAA;		
		}
		pTest = (Uint16 *)OUTPUT_ADD;
		for(I=0; I<0x000A; I++)
		{
			*pTest++ = 0xAAAA;		
		}
		pTest = (Uint16 *)MIN_ADD;
		for(I=0; I<0x000F; I++)
		{
			*pTest++ = 0xAAAA;		
		}
		pTest = (Uint16 *)MOUT_ADD;
		for(I=0; I<0x000F; I++)
		{
			*pTest++ = 0xAAAA;		
		}
		pTest = (Uint16 *)ModuleSoftVer_ADD;
		*pTest= 0xAAAA;	
		pTest = (Uint16 *)ModuleErrReg_ADD;
		*pTest= 0xAAAA;	
		//RD
		pTest = (Uint16 *)INPUT_ADD;		
		for(I=0; I<0x000A; I++)
		{
			if(*pTest++ != 0xAAAA)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)OUTPUT_ADD;		
		for(I=0; I<0x000A; I++)
		{
			if(*pTest++ != 0xAAAA)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)MIN_ADD;		
		for(I=0; I<0x000F; I++)
		{
			if(*pTest++ != 0xAAAA)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)MOUT_ADD;		
		for(I=0; I<0x000F; I++)
		{
			if(*pTest++ != 0xAAAA)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)ModuleSoftVer_ADD;
		if(*pTest != 0xAAAA)
		{
			// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
		}
		pTest = (Uint16 *)ModuleErrReg_ADD;
		if(*pTest != 0xAAAA)
		{
			// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
		}
	// test RAM 3
		//WR
		pTest = (Uint16 *)INPUT_ADD;
		for(I=0; I<0x000A; I++)
		{
			*pTest++ = I;		
		}
		pTest = (Uint16 *)OUTPUT_ADD;
		for(I=0; I<0x000A; I++)
		{
			*pTest++ = I+0x000A;		
		}
		pTest = (Uint16 *)MIN_ADD;
		for(I=0; I<0x000F; I++)
		{
			*pTest++ = I+0x000A+0x000A;		
		}
		pTest = (Uint16 *)MOUT_ADD;
		for(I=0; I<0x000F; I++)
		{
			*pTest++ = I+0x000A+0x000A+0x000F;		
		}
		pTest = (Uint16 *)ModuleSoftVer_ADD;
		*pTest= I+0x000A+0x000A+0x000F;	
		pTest = (Uint16 *)ModuleErrReg_ADD;
		*pTest= I+0x000A+0x000A+0x000F+1;	
		//RD
		pTest = (Uint16 *)INPUT_ADD;		
		for(I=0; I<0x000A; I++)
		{
			if(*pTest++ != I)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)OUTPUT_ADD;		
		for(I=0; I<0x000A; I++)
		{
			if(*pTest++ != I+0x000A)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)MIN_ADD;		
		for(I=0; I<0x000F; I++)
		{
			if(*pTest++ != I+0x000A+0x000A)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)MOUT_ADD;		
		for(I=0; I<0x000F; I++)
		{
			if(*pTest++ != I+0x000A+0x000A+0x000F)
			{
				// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}
		}
		pTest = (Uint16 *)ModuleSoftVer_ADD;
		if(*pTest != I+0x000A+0x000A+0x000F)
		{
			// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
		}
		pTest = (Uint16 *)ModuleErrReg_ADD;
		if(*pTest != I+0x000A+0x000A+0x000F+1)
		{
			// DPRAM test err
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
		}
		ModuleStatus.bit.DPRAMerr = 0;
		InitialShakehandLength = 4;
	}
	// appIND = 0xC0
	if(InitialShakehandLength == 4)
	{		
		WriteDPRAMcnt = 0;
		do
		{
			*EDBH_INDREG_ADD = 0xC0;
			if(++WriteDPRAMcnt > 10)
			{
				ModuleStatus.bit.DPRAMerr = 1;
				return 1;
			}							
		}while(*EDBH_INDREG_ADD != 0xC0);
		//ModuleStatus.bit.HS = 1;
		//DPRAM_HS_StartCnt = 0;
		InitialShakehandLength = 5;
		ModuleStatus.bit.DPRAMerr = 0;
	}
	if(InitialShakehandLength == 5)
	{
		if((*ABUS_INDREG_ADD & INDREG_HS))
		{
			DPRAM_HS_StartCnt = 0;
			ModuleStatus.bit.HS = 1;
			ModuleStatus.bit.HSerr = 0;
			ModuleStatus.bit.DPRAMerr = 0;
			DP100Edition = *VendorID_ADD;
		}
	}
  }
  return 0;
}

/*------------------------------------------------------------------------------
**
** WriteAppIndBits()
**
** Updates the Application Indication register according to the values given.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    CBits                   - The bits that are to be cleared.
**    SBits                   - The bits that are to be set.
**    Tbits                   - The bits that are to be toggled.
**
** Outputs:
**    None
**
** Usage:
**     WriteAppIndBits( CBits, SBits, Tbits );
**
**------------------------------------------------------------------------------
*/
void WriteAppIndBits( unsigned int CBits, unsigned int SBits, unsigned int Tbits )
{
   unsigned int AppIndCopy;

   /*
   ** Get a copy of the Application Indication register.
   ** Update the copy according to the values given.
   **
   ** Note: The bits are manipulated in a copy of the register to avoid
   **       multiple writing to the Application Indication register as we only
   **       want to write ONE value to the Application Indication register
   **       [ABS-DGP 5.1].
   */

   AppIndCopy  = *EDBH_INDREG_ADD;
   AppIndCopy &= ~CBits;
   AppIndCopy |=  SBits;
   AppIndCopy ^=  Tbits;

   /*
   ** Write the contents of the copy to the Application Indication register.
   ** Repeat until success in case the write fails due to a DPRAM collision.
   ** For further information regarding the Cypress DPRAM CY7C136, please see
   ** the Cypress webpage www.cypress.com.
   */

   /*do
   {
	  *EDBH_INDREG_ADD = AppIndCopy;
   }
   while( *EDBH_INDREG_ADD != AppIndCopy );*/
   WriteSuccess += AppWriteDPRAM((void*)&AppIndCopy,(Uint16 *)EDBH_INDREG_ADD);
} /* end of WriteAppIndBits() */


/*******************************************************************************
**
** End of ANYBUS.C
**
********************************************************************************
*/
