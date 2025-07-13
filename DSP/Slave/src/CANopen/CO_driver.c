#include <string.h>
#include "CANopen.h"

extern const Uint16 PnAttribute[][4];
/*******************************************************************************
   Variables and functions
*******************************************************************************/
   //Variables used in inline assembly
	//#pragma udata access CO_udata_Assembly
      //variables used in CO_CANrxIsr inline assembly
      volatile near CO_DEFAULT_TYPE CO_IsrHighIndex;        //index of matched CANid
      volatile near CO_DEFAULT_TYPE CO_IsrHighErrorIndex;   //IsrHigh writes index of received message with wrong length
      volatile near unsigned char CO_IsrHighSignal;         //temp variable and IsrHigh signals to ProcessMain:
                                                               //0xFF: Error in length of received message, index is stored
                                                               //0xFE: Error - message arrives, but is inhibited
   //#pragma udata

   //Node-ID
   unsigned char CO_NodeID;                                 //Variable must be intialized here
   unsigned char CO_BitRate;                                //Variable must be intialized here

   //function for verifying values at write to Object Dictionary
   unsigned long CO_OD_VerifyWrite(ROM CO_objectDictionaryEntry* pODE, void* data);
   //other
   extern volatile CO_DEFAULT_TYPE CO_TXCANcount;
   #if CO_NO_SYNC > 0
      extern volatile unsigned int CO_SYNCwindow;
   #endif

   extern volatile unsigned long TPDO_Old_Data[CO_NO_TPDO][2];

struct ECAN_REGS ECanbShadow;
Uint16 Error_Servo(int);
void Can_Transmit_Data(void);
extern void Transmit_Type_Change(void);
void Recieve_data(volatile struct MBOX * pMBOX);//2011-09-20
//=============================================================================
//接收发送处理程序
//=============================================================================
//Copy message to CAN buffers

#pragma CODE_SECTION(ECAN_ISR, "ramfuncs2");
//#pragma CODE_SECTION(Recieve_data,"ramfuncs");  // Lf20160704
#pragma CODE_SECTION(CO_TXCAN_COPY_TO_BUFFERS, "ramfuncs1");
/*******************************************************************************
   CAN bit rates - Registers setup
*******************************************************************************/
ROM struct{
   unsigned char BRP;         //(1...64)Baud Rate Prescaler
   unsigned char TSeg1;       //(1...8) Phase Segment 1 time
   unsigned char TSeg2;       //(1...8) Phase Segment 2 time
}CO_BitRateData[6] =
{{99,10,2},	    // 50k
 {74,5,2},		// 100k
 {59,5,2},		// 125k
 {29,5,2},		// 250k
 {14,4,3},		// 500k
 {4,10,2}		// 1M
};
/*#if CPU_FRQ == CPU_100MHz
{{199,6,1},		// 50k
 {99,6,1},		// 100k
 {79,6,1},		// 125k
 {39,5,2},		// 250k
 {19,5,2},		// 500k
 {9,4,3}		// 1M
};
#else
{{199,10,2},	// 50k
 {99,10,2},		// 100k
 {119,6,1},		// 125k
 {59,5,2},		// 250k
 {29,5,2},		// 500k
 {14,4,3}		// 1M
};
#endif*/

extern volatile unsigned char CO_Timer1msIsr_executing;
void CO_TXCAN_COPY_TO_BUFFERS(int INDEX)
{Uint32 tmp1,tmp2;
 int temp;
 volatile struct MBOX * pMBOX;
 volatile unsigned char *pData;
   	temp = 1<<INDEX;// 16bit,because mailbox0~15 is transmit_box
	ECanbRegs.CANTA.all = temp;
   	//copy data Low-->High
	pData = & CO_TXCAN[INDEX].Data.BYTE[0];
	tmp1 = * pData++;
	tmp1 += (* pData++)<<8;
	tmp1 += (Uint32)(* pData++)<<16;
	tmp1 += (Uint32)(* pData++)<<24;

	tmp2 = * pData++;
	tmp2 += (* pData++)<<8;
	tmp2 += (Uint32)(* pData++)<<16;
	tmp2 += (Uint32)(* pData)<<24;

	pMBOX = & ECanbMboxes.MBOX0 + INDEX;
	if(!CO_Timer1msIsr_executing)
	{
		asm("	SETC	INTM");
	}
	pMBOX->MDL.all = tmp1;
	pMBOX->MDL.all = tmp1;
	pMBOX->MDH.all = tmp2;
	pMBOX->MDH.all = tmp2;
	if(!CO_Timer1msIsr_executing)
	{
		asm("	CLRC	INTM");
	}
	ECanbRegs.CANTRS.all = temp;//ECanbShadow.CANTRS.all;
	temp = INDEX - CO_TXCAN_TPDO;
   	if((temp >= 0) && (temp <= 3))
   	{
		TPDO_Old_Data[temp][0] = tmp1;
		TPDO_Old_Data[temp][1] = tmp2;
   	}                                                 
}

//------------------------------------------------------------
void Reset_SYS(void)
{
	EALLOW;
    SysCtrlRegs.WDCR= 0x000A;
    EDIS;
	return;
}

/*******************************************************************************
   CO_ProcessDriver - PROCESS MICROCONTROLLER SPECIFIC CODE
   This is mainline function and is called cyclically from CO_ProcessMain().
*******************************************************************************/
void CO_ProcessDriver(void)//主程序中的CAN数据处理
{
   static unsigned char COMSTATold = 0;
   /* Verify CAN Errors *****************/
   	//CANerr_REG.bit.EW = ECanbRegs.CANES.bit.EW;
	CANerr_REG.bit.EW = 0;
	if(ECanbRegs.CANES.bit.EW)
	{
		ECanbRegs.CANES.bit.EW = 1;
	}
	CANerr_REG.bit.EP = ECanbRegs.CANES.bit.EP;
	if(ECanbRegs.CANES.bit.EP)
	{
		ECanbRegs.CANES.bit.EP = 1;
	}			
	CANerr_REG.bit.BO = ECanbRegs.CANES.bit.BO;
	if(ECanbRegs.CANES.bit.BO) 
	{
		ECanbRegs.CANES.bit.BO = 1;
	}
	CANerr_REG.bit.ACKE = ECanbRegs.CANES.bit.ACKE;
	if(ECanbRegs.CANES.bit.ACKE)
	{
		ECanbRegs.CANES.bit.ACKE = 1;
	}
	//CANerr_REG.bit.SE = ECanbRegs.CANES.bit.SE;
	CANerr_REG.bit.SE = 0;
	if(ECanbRegs.CANES.bit.SE)
	{
		ECanbRegs.CANES.bit.SE = 1;
	}
	//CANerr_REG.bit.CRCE = ECanbRegs.CANES.bit.CRCE;
	CANerr_REG.bit.CRCE = 0;
	if(ECanbRegs.CANES.bit.CRCE)
	{
		ECanbRegs.CANES.bit.CRCE = 1;
	}
	//CANerr_REG.bit.BE = ECanbRegs.CANES.bit.BE;
	CANerr_REG.bit.BE = 0;
	if(ECanbRegs.CANES.bit.BE)
	{
		ECanbRegs.CANES.bit.BE = 1;
	}
	//CANerr_REG.bit.FE = ECanbRegs.CANES.bit.FE;
	CANerr_REG.bit.FE = 0;
	if(ECanbRegs.CANES.bit.FE)
	{
		ECanbRegs.CANES.bit.FE = 1;
	}	
   	if(CANerr_REG.all != COMSTATold)
   	{
      if(CANerr_REG.bit.FE || CANerr_REG.bit.BE || CANerr_REG.bit.CRCE || CANerr_REG.bit.SE || CANerr_REG.bit.ACKE)
	  {
		 ErrorReport(ERROR_CAN_FE_BE_CRCE_SE_ACKE, CANerr_REG.all);
	  }
	  else if(ERROR_BIT_READ(ERROR_CAN_FE_BE_CRCE_SE_ACKE))
	  {		 
         ErrorReset(ERROR_CAN_FE_BE_CRCE_SE_ACKE, CANerr_REG.all);
	  }

      if(CANerr_REG.bit.BO)
         ErrorReport(ERROR_CAN_BUS_OFF, CANerr_REG.all);
      else if(ERROR_BIT_READ(ERROR_CAN_BUS_OFF))
         ErrorReset(ERROR_CAN_BUS_OFF, CANerr_REG.all);

      if(CANerr_REG.bit.EP)
         ErrorReport(ERROR_CAN_BUS_PASSIVE, CANerr_REG.all);
      else if(ERROR_BIT_READ(ERROR_CAN_BUS_PASSIVE))
      {
         ErrorReset(ERROR_CAN_BUS_PASSIVE, CANerr_REG.all);
      }

      if(CANerr_REG.bit.EW)
         ErrorReport(ERROR_CAN_BUS_WARNING, CANerr_REG.all);
      else if(ERROR_BIT_READ(ERROR_CAN_BUS_WARNING))
         ErrorReset(ERROR_CAN_BUS_WARNING, CANerr_REG.all);

      COMSTATold = CANerr_REG.all;
   }   
   Error_Servo(alarmno);   
   if(CO_NMToperatingState==NMT_PRE_OPERATIONAL ||CO_NMToperatingState==NMT_OPERATIONAL)
   {
   		if(CO_IsrHighSignal == 0xFF)
   		{
	      	if(CO_IsrHighErrorIndex >= CO_RXCAN_RPDO && CO_IsrHighErrorIndex < (CO_RXCAN_RPDO+CO_NO_RPDO))
	           ErrorReport(ERROR_CO_RXMSG_PDOlength, CO_IsrHighErrorIndex - CO_RXCAN_RPDO);
	      	else ErrorReport(ERROR_CO_RXMSG_Length, CO_IsrHighErrorIndex);
	      	CO_IsrHighSignal = 0;
   		}
   		else if (CO_IsrHighSignal == 0xFE)
   		{
	      	ErrorReport(ERROR_CO_RXMSG_Inhibit, CO_IsrHighErrorIndex);
	     	CO_IsrHighSignal = 0;
   		}
	}
}

/*******************************************************************************
   CO_Read_NodeId_BitRate - READ NODE-ID AND CAN BITRATE
   This is mainline function and is called from Communication reset. Usually
   NodeID and BitRate are read from DIP switches.
*******************************************************************************/
void CO_Read_NodeId_BitRate(void)
{   
   	ODE_CANbitRate = Pn[CANParasADD] & 0x000F;	//CAN通讯速率
	ODE_CANnodeID = Pn[CANAdderADD];			//CAN通讯节点
   	CO_NodeID = ODE_CANnodeID;    //range 1 to 127 CAN通讯节点
   	CO_BitRate = ODE_CANbitRate;  //range 0 to 5
                                    // 0 = 50 kbps    1 = 100 kbps
                                    // 2 = 125 kbps   3 = 250 kbps
                                    // 4 = 500 kbps   5 = 1000 kbps
   	if(CO_NodeID==0 || CO_NodeID>127 || CO_BitRate > 5){
      ErrorReport(ERROR_WrongNodeIDorBitRate, (CO_NodeID<<8)|CO_BitRate);
      CO_NodeID = 1;
      if(CO_BitRate > 5) CO_BitRate = 2;
   }
}
//*
/*数据区copy到buffer区，字到字*/
void memcpy1(unsigned int *DestAddr, unsigned int *SourceAddr, int memsize)
{
    //memsize = (memsize+1)>1;
    while(memsize--)
	{
		*DestAddr++ = *SourceAddr++;
	}
    return;
}
/*对字典读，字节到字*/
void memcpy_read_od(unsigned int *DestAddr, unsigned int *SourceAddr, int memsize)
{
    if(!memsize)
	{
		*DestAddr++ = (*SourceAddr) & 0xFF;//先读低位后高位
		return;
	}
    while(memsize--)
	{
		*DestAddr++ = (*SourceAddr) & 0xFF;//先读低位后高位
		*DestAddr++ = (((*SourceAddr++) & 0xFF00)>>8);
	}
    return;
}
/*对字典写，字节到字*/
void memcpy_write_od(unsigned int *DestAddr, unsigned int *SourceAddr, int memsize)
{
    if(!memsize)
	{
		*DestAddr++ = (*SourceAddr) & 0xFF;//先读低位后高位
		return;
	}
	while(memsize--)
	{
		*DestAddr = (*SourceAddr++ );
		*DestAddr++ += ((*SourceAddr++)<<8);
	}
    return;
}
void CO_SetupCAN(void)
{Uint16 i;
 volatile struct MBOX * pMBOX;
	//struct ECAN_REGS ECanbShadow;

/* Setup driver variables */
	CO_IsrHighSignal = 0;
	ECanbShadow.CANME.all = ECanbRegs.CANME.all;	

	ECanbShadow.CANME.all = 0;		//不使能邮箱

	ECanbRegs.CANME.all = ECanbShadow.CANME.all; 
/* Setup CAN bus */
	EALLOW;		// EALLOW enables access to protected bits

/* Configure eCAN RX and TX pins for eCAN transmissions using eCAN regs*/  
    
    ECanbRegs.CANTIOC.bit.TXFUNC = 1;
    ECanbRegs.CANRIOC.bit.RXFUNC = 1;  

/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
									// HECC mode also enables time-stamping feature
	
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.SCB = 1;		//		
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
	
/* Initialize all bits of 'Master Control Field' to zero */
	ECanbMboxes.MBOX0.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX1.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX2.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX3.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX4.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX5.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX6.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX7.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX8.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX9.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX10.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX11.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX12.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX13.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX14.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX15.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX16.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX17.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX18.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX19.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX20.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX21.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX22.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX23.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX24.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX25.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX26.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX27.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX28.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX29.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX30.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX31.MSGCTRL.all = 0x00000000;
    
// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//	as a matter of precaution. 

	ECanbRegs.CANTA.all	= 0xFFFFFFFF;	/* Clear all TAn bits 清除所有邮箱的发送成功标志*/ 	
	ECanbRegs.CANRMP.all = 0xFFFFFFFF;	/* Clear all RMPn bits 清除所有邮箱的接收成功标志*/ 	
	ECanbRegs.CANGIF0.all = 0xFFFFFFFF;	/* Clear all interrupt flag bits 清除所有中断标志*/ 
	ECanbRegs.CANGIF1.all = 0xFFFFFFFF;	/* Clear all interrupt flag bits 清除所有中断标志*/
	
/* Configure bit timing parameters for eCANB*/
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1 waiting CCE bit to be set
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    
    while(ECanbRegs.CANES.bit.CCE != 1 ) {}   // Wait for CCE bit to be set..
    
    ECanbShadow.CANBTC.all = 0;
    ECanbShadow.CANBTC.bit.BRPREG = CO_BitRateData[CO_BitRate].BRP;//通讯速率79;	
    ECanbShadow.CANBTC.bit.TSEG1REG = CO_BitRateData[CO_BitRate].TSeg1;
    ECanbShadow.CANBTC.bit.TSEG2REG = CO_BitRateData[CO_BitRate].TSeg2; 
    if((Pn[CANParasADD] & 0x000F) > 4)
    {
	    ECanbShadow.CANBTC.bit.SAM = 0;///////
		ECanbShadow.CANBTC.bit.SJWREG = 1;
	}
	else
	{
		ECanbShadow.CANBTC.bit.SAM = 1;///////
		ECanbShadow.CANBTC.bit.SJWREG = 0;
	}
    ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;

	ECanbShadow.CANGAM.all = ECanbRegs.CANGAM.all;
	ECanbShadow.CANGAM.bit.AMI = 1;
	ECanbRegs.CANGAM.all = ECanbShadow.CANGAM.all;
	//ECanbRegs.CANGAM.all = 0xFFFFFFFF;
    
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 0 ;            // Set CCR = 0
	ECanbShadow.CANMC.bit.ABO = 1 ;            // bus open
	ECanbShadow.CANMC.bit.DBO = 1 ;            // LSB
	//ECanbShadow.CANMC.bit.STM = 1;				// self test mode
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;
    
    while(ECanbRegs.CANES.bit.CCE == !0 ) {}   // Wait for CCE bit to be cleared..

/* Disable all Mailboxes  */
 	ECanbRegs.CANME.all = 0;		// Required before writing the MSGIDs
    EDIS;

	//init rx mail
	EALLOW;
/***********************************************************************/
/*	NMT:	mailbox 	16;	*/
/*	SYNC:	mailbox 	17;	*/
/*	RPDO0:	mailbox 	18;	*/
/*	RPDO1:	mailbox 	19;	*/
/*	RPDO2:	mailbox 	20;	*/
/*	RPDO3:	mailbox 	21;	*/
/*	SDO:	mailbox 	22;	*/
/*	LIFE:	mailbox 	23;	*/
/***********************************************************************/
	ECanbLAMRegs.LAM16.all = 0x0;
	ECanbLAMRegs.LAM17.all = 0x0;
	ECanbLAMRegs.LAM18.all = 0x0;
	ECanbLAMRegs.LAM19.all = 0x0;
	ECanbLAMRegs.LAM20.all = 0x0;
	ECanbLAMRegs.LAM21.all = 0x0;
	ECanbLAMRegs.LAM22.all = 0x0;
	ECanbLAMRegs.LAM23.all = 0x0;
	ECanbLAMRegs.LAM24.all = 0x0;
	ECanbLAMRegs.LAM25.all = 0x0;
	ECanbLAMRegs.LAM26.all = 0x0;
	ECanbLAMRegs.LAM27.all = 0x0;
	ECanbLAMRegs.LAM28.all = 0x0;
	ECanbLAMRegs.LAM29.all = 0x0;
	ECanbLAMRegs.LAM30.all = 0x0;
	ECanbLAMRegs.LAM31.all = 0x0;
	ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;
	ECanbShadow.CANMD.all |= 0xFFFF0000;	// MBOX16~31 is recieve_box	
   	ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;
	ECanbShadow.CANME.all = ECanbRegs.CANME.all;	
   	ECanbShadow.CANME.all &= 0x0000FFFF;	// MBOX16~31 is disabled
   	ECanbRegs.CANME.all = ECanbShadow.CANME.all;
	pMBOX = & ECanbMboxes.MBOX16;
	for(i = 0; i < CO_RXCAN_NO_MSGS;i++)
	{		
		pMBOX->MSGID.bit.IDE = 0;
		pMBOX->MSGID.bit.AME = 1;
		pMBOX->MSGID.bit.AAM = 0;
		pMBOX->MSGID.bit.STDMSGID = ((CO_RXCAN[i].Ident.BYTE[0])>>5);
		pMBOX ++;
		ECanbShadow.CANME.all = ECanbRegs.CANME.all;	
	   	ECanbShadow.CANME.all |= ((Uint32)1<<(16+i));
	   	ECanbRegs.CANME.all = ECanbShadow.CANME.all;
	}

/***********************************************************************/
/*	HB:			mailbox 	7;	*/
/*	SDO:		mailbox 	6;	*/
/*	TPDO3:		mailbox 	5;	*/
/*	TPDO2:		mailbox 	4;	*/
/*	TPDO1:		mailbox 	3;	*/
/*	TPDO0:		mailbox 	2;	*/
/*	EMERGENCY:	mailbox 	1;	*/
/*	SYNC:		mailbox 	0;	*/
/***********************************************************************/
	ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;
	ECanbShadow.CANMD.all &= 0xFFFF0000;	// MBOX0~15 is transmit_box	
   	ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;
	ECanbShadow.CANME.all = ECanbRegs.CANME.all;	
   	ECanbShadow.CANME.all &= 0xFFFF0000;	// MBOX0~15 is disabled
   	ECanbRegs.CANME.all = ECanbShadow.CANME.all;
	pMBOX = & ECanbMboxes.MBOX0;
	for(i = 0; i < CO_TXCAN_NO_MSGS;i++)
	{		
		pMBOX->MSGID.bit.IDE = 0;
		pMBOX->MSGID.bit.AME = 0;
		pMBOX->MSGID.bit.AAM = 0;
		pMBOX->MSGID.bit.STDMSGID = (CO_TXCAN[i].Ident.BYTE[0]>>5);	
		if(CO_TXCAN[i].NoOfBytes > 8) 
	   	{
			CO_TXCAN[i].NoOfBytes = 8;
	   	}
		pMBOX->MSGCTRL.bit.DLC = CO_TXCAN[i].NoOfBytes;
		pMBOX ++;
		ECanbShadow.CANME.all = ECanbRegs.CANME.all;	
   		ECanbShadow.CANME.all |= ((Uint32)1<<i);	// MBOX enable
   		ECanbRegs.CANME.all = ECanbShadow.CANME.all;
	}

	// Configure CAN interrupts  在中断线1上产生邮箱邢
	ECanbShadow.CANMIL.all = 0xFFFFFFFF ; // Interrupts asserted on eCAN1INT
	//ECanbShadow.CANMIL.all  = 0x00000000 ; // Interrupts asserted on eCAN0INT	
	ECanbRegs.CANMIL.all = ECanbShadow.CANMIL.all;
	ECanbRegs.CANOPC.all=0;//0xFFFFFFFF; 2008-9-23
	//邮箱中断屏蔽位
	//ECanbShadow.CANMIM.all  = 0x0062181;// Enable interrupts for all mailboxes
    //ECanbShadow.CANMIM.all  = 0x00621C2;//发送中断打开
	ECanbShadow.CANMIM.all  = 0;//发送中断不打开
	ECanbShadow.CANMIM.bit.MIM23 = 1;		// RX LIFE
	ECanbShadow.CANMIM.bit.MIM22 = 1;		// RX SDO
	ECanbShadow.CANMIM.bit.MIM21 = 1;		// RPDO3
	ECanbShadow.CANMIM.bit.MIM20 = 1;		// RPDO2
	ECanbShadow.CANMIM.bit.MIM19 = 1;		// RPDO1 
	ECanbShadow.CANMIM.bit.MIM18 = 1;		// RPDO0
	ECanbShadow.CANMIM.bit.MIM17 = 1;		// RX SYNC	
	ECanbShadow.CANMIM.bit.MIM16 = 1;		// RX NMT
    ECanbRegs.CANMIM.all = ECanbShadow.CANMIM.all;
    
    //全局中断屏蔽寄存器	
    ECanbShadow.CANGIM.all=0;
    ECanbShadow.CANGIM.bit.I0EN = 0;   	// Enable eCAN1INT 
    ECanbShadow.CANGIM.bit.I1EN = 1;	// Disable eCAN0INT
	ECanbShadow.CANGIM.bit.GIL = 1;		//所有全局中断都映射到1INT中断线上
    ECanbRegs.CANGIM.all = ECanbShadow.CANGIM.all;		
	EDIS;

	CANinit = 0;
	//set filters
}
 /*******************************************************************************
   CO_CANrxIsr - CAN RECEIVE INTERRUPT
   This function is invoked by interrupt, when new can message receive from CAN
   network. It searches CO_RXCAN[] array and compares COB-ID. If found, copy data
   to appropriate buffer.
*******************************************************************************/
interrupt void ECAN_ISR(void)
{
	Uint16 MIV;
	volatile struct MBOX * pMBOX;	
	asm (" NOP");
	if(ECanbRegs.CANGIF1.bit.GMIF1)
	{
		MIV = ECanbRegs.CANGIF1.bit.MIV1;
//		tmp = (Uint32)1<<MIV;
//		pMBOX = & ECanbMboxes.MBOX0 + MIV;
		if(MIV > 15)
		{// receive

			//LIFE 接收数据				
			if(ECanbRegs.CANRMP.bit.RMP23)
			{
				ECanbRegs.CANRMP.bit.RMP23 = 1;
				if(!ECanbMboxes.MBOX23.MSGCTRL.bit.RTR)
				{
					pMBOX = & ECanbMboxes.MBOX0 + 23;
					CO_IsrHighIndex = 7;
					Recieve_data(pMBOX);
				}	
			}
			//SDO 接收数据
			if(ECanbRegs.CANRMP.bit.RMP22)
			{
				ECanbRegs.CANRMP.bit.RMP22 = 1;
				if(!ECanbMboxes.MBOX22.MSGCTRL.bit.RTR)
				{
					pMBOX = & ECanbMboxes.MBOX0 + 22;
					CO_IsrHighIndex = 6;
					Recieve_data(pMBOX);
				}	
			}	
	    	//RPDO3 接收数据
			if(ECanbRegs.CANRMP.bit.RMP21) 
			{
				ECanbRegs.CANRMP.bit.RMP21 = 1;
				if(!ECanbMboxes.MBOX21.MSGCTRL.bit.RTR)
				{
					pMBOX = & ECanbMboxes.MBOX0 + 21;
					CO_IsrHighIndex = 5;
					Recieve_data(pMBOX);
				}	
			}	
			//RPDO2 接收数据
			if(ECanbRegs.CANRMP.bit.RMP20)
			{
				ECanbRegs.CANRMP.bit.RMP20 = 1;
				if (!ECanbMboxes.MBOX20.MSGCTRL.bit.RTR)
				{
					pMBOX = & ECanbMboxes.MBOX0 + 20;
					CO_IsrHighIndex = 4;
					Recieve_data(pMBOX);
				}	
			}	
			//RPDO1 接收数据
			if(ECanbRegs.CANRMP.bit.RMP19)
			{
				ECanbRegs.CANRMP.bit.RMP19 = 1;
				if(!ECanbMboxes.MBOX19.MSGCTRL.bit.RTR)
				{
					pMBOX = & ECanbMboxes.MBOX0 + 19;
					CO_IsrHighIndex = 3;
					Recieve_data(pMBOX);
				}	
			}	
			//RPDO0 接收数据
			if(ECanbRegs.CANRMP.bit.RMP18)
			{
				ECanbRegs.CANRMP.bit.RMP18 = 1;
				if(!ECanbMboxes.MBOX18.MSGCTRL.bit.RTR)
				{
					pMBOX = & ECanbMboxes.MBOX0 + 18;
					CO_IsrHighIndex = 2;
					Recieve_data(pMBOX);
				}	
			}	
			//SYNC 接收数据
			if(ECanbRegs.CANRMP.bit.RMP17)
			{
				ECanbRegs.CANRMP.bit.RMP17 = 1;
				if(!ECanbMboxes.MBOX17.MSGCTRL.bit.RTR)
				{
					pMBOX = & ECanbMboxes.MBOX0 + 17;
					CO_IsrHighIndex = 1;
					Recieve_data(pMBOX);
				}	
			}	
			//NMT 接收数据
			if(ECanbRegs.CANRMP.bit.RMP16)
			{
				ECanbRegs.CANRMP.bit.RMP16 = 1;
				if(!ECanbMboxes.MBOX16.MSGCTRL.bit.RTR)
				{
					pMBOX = & ECanbMboxes.MBOX0 + 16;
					CO_IsrHighIndex = 0;
					Recieve_data(pMBOX);
				}	
			}	
		}
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	EINT;//开全局中断
}

void Recieve_data(volatile struct MBOX * pMBOX)
{
	volatile unsigned char *pData;
	if(CO_IsrHighIndex < CO_RXCAN_NO_MSGS)
	{
			//Verify length
        if((CO_RXCAN[CO_IsrHighIndex].NoOfBytes < 8) && (CO_RXCAN[CO_IsrHighIndex].NoOfBytes != pMBOX->MSGCTRL.bit.DLC))
        {
         	CO_IsrHighSignal = 0xFF;
         	CO_IsrHighErrorIndex = CO_IsrHighIndex;
        }
			//Verify inhibit
		else if(CO_RXCAN[CO_IsrHighIndex].NewMsg &&
			CO_RXCAN[CO_IsrHighIndex].Inhibit)
		{
			CO_IsrHighSignal = 0xFE;
			CO_IsrHighErrorIndex = CO_IsrHighIndex;
		}
		else
		{
				//Set flag and copy data
			CO_RXCAN[CO_IsrHighIndex].NewMsg = 1;
//			pMBOX.MDL.all = pMBOX->MDL.all;	
//			pMBOX.MDL.all = pMBOX->MDL.all;							
//			pMBOX.MDH.all = pMBOX->MDH.all;						
//			pMBOX.MDH.all = pMBOX->MDH.all;
			pData = & CO_RXCAN[CO_IsrHighIndex].Data.BYTE[0];
			if(CO_RXCAN[CO_IsrHighIndex].NoOfBytes == 8)
			{
				*pData++ = pMBOX->MDL.byte.BYTE1;
				*pData++ = pMBOX->MDL.byte.BYTE2;
				*pData++ = pMBOX->MDL.byte.BYTE3;
				*pData++ = pMBOX->MDL.byte.BYTE4;
				*pData++ = pMBOX->MDH.byte.BYTE5;
				*pData++ = pMBOX->MDH.byte.BYTE6;
				*pData++ = pMBOX->MDH.byte.BYTE7;
				*pData = pMBOX->MDH.byte.BYTE8;
			}
			else if(CO_RXCAN[CO_IsrHighIndex].NoOfBytes == 7)
			{
				*pData++ = pMBOX->MDL.byte.BYTE1;
				*pData++ = pMBOX->MDL.byte.BYTE2;
				*pData++ = pMBOX->MDL.byte.BYTE3;
				*pData++ = pMBOX->MDL.byte.BYTE4;
				*pData++ = pMBOX->MDH.byte.BYTE5;
				*pData++ = pMBOX->MDH.byte.BYTE6;
				*pData = pMBOX->MDH.byte.BYTE7;
			}
			else if(CO_RXCAN[CO_IsrHighIndex].NoOfBytes == 6)
			{
				*pData++ = pMBOX->MDL.byte.BYTE1;
				*pData++ = pMBOX->MDL.byte.BYTE2;
				*pData++ = pMBOX->MDL.byte.BYTE3;
				*pData++ = pMBOX->MDL.byte.BYTE4;
				*pData++ = pMBOX->MDH.byte.BYTE5;
				*pData = pMBOX->MDH.byte.BYTE6;
			}
			else if(CO_RXCAN[CO_IsrHighIndex].NoOfBytes == 5)
			{
				*pData++ = pMBOX->MDL.byte.BYTE1;
				*pData++ = pMBOX->MDL.byte.BYTE2;
				*pData++ = pMBOX->MDL.byte.BYTE3;
				*pData++ = pMBOX->MDL.byte.BYTE4;
				*pData = pMBOX->MDH.byte.BYTE5;
			}
			else if(CO_RXCAN[CO_IsrHighIndex].NoOfBytes == 4)
			{
				*pData++ = pMBOX->MDL.byte.BYTE1;
				*pData++ = pMBOX->MDL.byte.BYTE2;
				*pData++ = pMBOX->MDL.byte.BYTE3;
				*pData = pMBOX->MDL.byte.BYTE4;
			}
			else if(CO_RXCAN[CO_IsrHighIndex].NoOfBytes == 3)
			{
				*pData++ = pMBOX->MDL.byte.BYTE1;
				*pData++ = pMBOX->MDL.byte.BYTE2;
				*pData = pMBOX->MDL.byte.BYTE3;
			}
			else if(CO_RXCAN[CO_IsrHighIndex].NoOfBytes == 2)
			{
				*pData++ = pMBOX->MDL.byte.BYTE1;
				*pData = pMBOX->MDL.byte.BYTE2;
			}
			else if(CO_RXCAN[CO_IsrHighIndex].NoOfBytes == 1)
			{
				*pData = pMBOX->MDL.byte.BYTE1;
			}
		}
	}
}
//=============================================================================
// 用查询的方式判断发送数据成功与否
//=============================================================================
void Can_Transmit_Data(void)
{Uint16 temp,i;	
 Uint16 tmp;	
	//if(ECanbRegs.CANTA.bit.TA8 || !CANinit)
	if(CO_CANTX_BUFFER_FREE())
	{
		if((CO_TXCANcount > 0))//are new messages
		{		
			for(i=0; i<CO_TXCAN_NO_MSGS; i++)
			{
				temp = (1<<i);
				tmp = ECanbRegs.CANTA.all;
				if(CO_TXCAN[i].NewMsg && ((tmp & temp) || !(CANinit & temp)))
				{
					#if CO_NO_SYNC > 0
	              		//messages with Inhibit flag set (synchronous PDOs) must be transmited inside preset window
	           			if(CO_TXCAN[i].Inhibit && CO_SYNCwindow && CO_SYNCtime > CO_SYNCwindow)
	              			ErrorReport(ERROR_TPDO_OUTSIDE_WINDOW, i);
	           			else
	           		#endif
	           			CO_TXCAN_COPY_TO_BUFFERS(i);
	       			//release buffer
	           		CO_TXCAN[i].NewMsg = 0;
					CO_TXCANcount--;
					CANinit |= temp;
					break;
				}
			}
		}
	}	
}
/*******************************************************************************
   CO_OD_Read - OBJECT DICTIONARY READ
   This is mainline function. It is called from SDO server and reads data from
   object dictionary. Since object dictionary variables can use many types of
   memory, some code is processor specific.

   PARAM pODE: pointer to object dictionary entry, found by CO_FindEntryInOD().
   PARAM pBuff: pointer to buffer where data will be written.
   PARAM BuffMaxSize: Size of buffer (to prevent overflow).
   RETURN: 0 if success, otherwise abort code
*******************************************************************************/
unsigned long CO_OD_Read(ROM CO_objectDictionaryEntry* pODE, void* pBuff, unsigned char sizeOfBuff){
   if(pODE->length > sizeOfBuff) return 0x05040005L;  //Out of memory
   if((pODE->attribute&0x07) == ATTR_WO) return 0x06010001L;  //attempt to read a write-only object

   CO_DISABLE_ALL();
   memcpy_read_od(pBuff, (void*)pODE->pData, ((pODE->length)>>1));
   CO_ENABLE_ALL();
   return 0;
}

/*******************************************************************************
   CO_OD_Write - OBJECT DICTIONARY WRITE
   This is mainline function. It is called from SDO server and writes data to
   object dictionary. Since object dictionary variables can use many types of
   memory, some code is processor specific.

   PARAM pODE: pointer to object dictionary entry, found by CO_FindEntryInOD().
   PARAM pBuff: pointer to buffer which contains data to be written.
   PARAM dataSize: Size of data in buffer.
   RETURN: 0 if success, otherwise abort code
*******************************************************************************/
unsigned long CO_OD_Write(ROM CO_objectDictionaryEntry* pODE, void* data, unsigned char length)
{
   	#ifdef CO_VERIFY_OD_WRITE
      	Uint32 datatmp;
   	#endif
   	#ifdef CO_VERIFY_OD_WRITE
      	Uint32 AbortCode;
   	#endif
   	if((pODE->attribute&0x07) == ATTR_RO || (pODE->attribute&0x07) == ATTR_CO) return 0x06010002L;   //attempt to write a read-only object 试图写只读对象
   	if(length != pODE->length) return 0x06070010L;   //Length of service parameter does not match	数据类型不匹配，服务参数长度不匹配
   	#ifdef CO_VERIFY_OD_WRITE
      	//verify value
	  	datatmp = *((unsigned char*)data) & 0x00FF;
	  	datatmp |= (Uint32)(*((unsigned char*)data+1) & 0x00FF) << 8;
	  	datatmp |= (Uint32)(*((unsigned char*)data+2) & 0x00FF) << 16;
	  	datatmp |= (Uint32)(*((unsigned char*)data+3) & 0x00FF) << 24;
      	AbortCode = CO_OD_VerifyWrite(pODE, &datatmp);
      	if(AbortCode) return AbortCode;
   	#endif

   //write data to memory (processor specific code)
   //RAM data
   //all RAM data are above address 0x3000~3100 in EDC
	if((unsigned long)pODE->index >= 0x3000 && (unsigned long)pODE->index <= 0x3159)
  	{//检验写入参数的值是否在范围内     	
		Uint16 Pnsigns,Min_Data,Max_Data,Write_Data,PnIndex;
		int * temp;
		temp = data;
		PnIndex = (Uint16)((unsigned long)pODE->pData - (unsigned long)&Pn[0]);
//		Write_Data=(* temp++&0xFF)+((* temp&0xFF)<<8);
		Write_Data=* temp&0xFF;
		temp++;
		Write_Data += (* temp&0xFF)<<8;
		Pnsigns = (* ((int16 *)((Uint32)&PnAttribute + PnIndex*4)));
		Min_Data= (* ((int16 *)((Uint32)&PnAttribute + PnIndex*4 + 1)));
		Max_Data= (* ((int16 *)((Uint32)&PnAttribute + PnIndex*4 + 2)));
		if(Pnsigns == 1)		//int
		{
			if((int16)Write_Data>(int16)Max_Data ||  (int16)Write_Data<(int16)Min_Data)
			{
				return 0x06090030L;//超出参数的值范围(写访问时)
	  		}
	  		else if(Write_Data != Pn[PnIndex])//进行驱动器系牟问慈氩僮?
	  		{
				Pn[SumAddr] -= Pn[PnIndex];								
				CheckSum -= Pn[PnIndex];
				Pn[PnIndex] = Write_Data;
				Pn[SumAddr] += Pn[PnIndex];
				CheckSum += Pn[PnIndex];
				SPI_WriteFRAM(PnIndex,&Pn[PnIndex],1);
				SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
			}
		}
		else if(Pnsigns == 3)	//hex
		{
			if(((Write_Data & 0xF000) > (Max_Data & 0xF000)) 
			 ||((Write_Data & 0x0F00) > (Max_Data & 0x0F00))
			 ||((Write_Data & 0x00F0) > (Max_Data & 0x00F0))
			 ||((Write_Data & 0x000F) > (Max_Data & 0x000F))
			 ||((Write_Data & 0xF000) < (Min_Data & 0xF000)) 
			 ||((Write_Data & 0x0F00) < (Min_Data & 0x0F00))
			 ||((Write_Data & 0x00F0) < (Min_Data & 0x00F0))
			 ||((Write_Data & 0x000F) < (Min_Data & 0x000F)))
			{
				return 0x06090030L;//超出参数的值范围(写访问时)
	  		}
	  		else if(Write_Data != Pn[PnIndex])//进行驱动器上的参数写入操作
	  		{
				Pn[SumAddr] -= Pn[PnIndex];								
				CheckSum -= Pn[PnIndex];
				Pn[PnIndex] = Write_Data;
				Pn[SumAddr] += Pn[PnIndex];
				CheckSum += Pn[PnIndex];
				SPI_WriteFRAM(PnIndex,&Pn[PnIndex],1);
				SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
			}				
		}
		else					//unint,bit
		{
			if((Write_Data > Max_Data) || (Write_Data < Min_Data))
			{
				return 0x06090030L;//超出参数的值范围(写访问时)
	  		}
	  		else if(Write_Data != Pn[PnIndex])//进行驱动器上的参数写入操作
	  		{
				Pn[SumAddr] -= Pn[PnIndex];								
				CheckSum -= Pn[PnIndex];
				Pn[PnIndex] = Write_Data;
				Pn[SumAddr] += Pn[PnIndex];
				CheckSum += Pn[PnIndex];
				SPI_WriteFRAM(PnIndex,&Pn[PnIndex],1);
				SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
			}
		}	  		
	}
	else//进行其他设置操作
	{		
    	CO_DISABLE_ALL();
    	//memcpy1((void*)pODE->pData, data, length);
		memcpy_write_od((void*)pODE->pData, data, ((pODE->length)>>1));
    	CO_ENABLE_ALL();
		Transmit_Type_Change();
	}
   	return 0;
}
//=============================================================================
// End of file.
//=============================================================================
