/*******************************************************************************

   CO_stack.c - Functions for CANopenNode - processor independent

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*******************************************************************************/

#include "CANopen.h"
extern struct ECAN_REGS ECanbShadow;
#define CHANGE_PDO_TIME 100//10000L // ms
/*******************************************************************************
   VERIFY SIZES
*******************************************************************************/
   #if CO_NO_SYNC != 0 && CO_NO_SYNC != 1
      #error defineCO_NO_SYNC (CO_NO_SYNC) not correct!
   #endif
   #if CO_NO_EMERGENCY != 0 && CO_NO_EMERGENCY != 1
      #error defineCO_NO_EMERGENCY (CO_NO_EMERGENCY) not correct!
   #endif
   #if CO_NO_RPDO < 0 || CO_NO_RPDO > 512
      #error define_CO_NO_RPDO (CO_NO_RPDO) not valid
   #endif
   #if CO_NO_TPDO < 0 || CO_NO_TPDO > 512
      #error define_CO_NO_TPDO (CO_NO_TPDO) not valid
   #endif
   #if CO_NO_SDO_SERVER < 0 || CO_NO_SDO_SERVER > 128
      #error define_CO_NO_SDO_SERVER (CO_NO_SDO_SERVER) not valid
   #endif
   #if CO_NO_SDO_CLIENT < 0 || CO_NO_SDO_CLIENT > 128
      #error define_CO_NO_SDO_CLIENT (CO_NO_SDO_CLIENT) not valid
   #endif
   #if CO_NO_CONS_HEARTBEAT < 0 || CO_NO_CONS_HEARTBEAT > 255
      #error define_CO_NO_CONS_HEARTBEAT (CO_NO_CONS_HEARTBEAT) not valid
   #endif
   #if CO_NO_USR_CAN_RX < 0
      #error define_CO_NO_USR_CAN_RX (CO_NO_USR_CAN_RX) not valid
   #endif
   #if CO_NO_USR_CAN_TX < 0
      #error define_CO_NO_USR_CAN_TX (CO_NO_USR_CAN_TX) not valid
   #endif
   #if CO_RXCAN_NO_MSGS > CO_DEFAULT_TYPE_SIZE
      #error number of receiving messages CO_RXCAN_NO_MSGS too large
   #endif
   #if CO_TXCAN_NO_MSGS > CO_DEFAULT_TYPE_SIZE
      #error number of transmiting messages CO_TXCAN_NO_MSGS too large
   #endif
   #if CO_NO_ERROR_FIELD < 0 || CO_NO_ERROR_FIELD > 254
      #error define_CO_NO_ERROR_FIELD (CO_NO_ERROR_FIELD) not valid
   #endif
   #if CO_MAX_OD_ENTRY_SIZE < 4 || CO_MAX_OD_ENTRY_SIZE > 256
      #error define_CO_MAX_OD_ENTRY_SIZE (CO_MAX_OD_ENTRY_SIZE) not valid
   #endif


/*******************************************************************************
   Functions, implemented by user, used in CO_stack.c
*******************************************************************************/
   void User_ResetComm(void);
   void User_Process01msIsr(void);


void CO_Find_RPDOmapData(ROM CO_objectDictionaryEntry* pODE,int RPDOindex,int i);
void CO_Find_TPDOmapData(ROM CO_objectDictionaryEntry* pODE,int TPDOindex,int i);
void Dynamic_PDO_Maping(void);
void Transmit_Type_Change(void);

/*******************************************************************************
   Functions from CO_driver.c used in CO_stack.c
*******************************************************************************/
   void CO_Read_NodeId_BitRate(void);  //determine NodeId and BitRate
   void CO_SetupCAN(void);             //setup CAN controller
   void CO_ProcessDriver(void);        //process microcontroller specific code
   
   void Change_CAN_MOD(void);			//CANͨѶ��ַ�����ʵȸı�	

   void CO_TXCAN_COPY_TO_BUFFERS(int INDEX);
   void memcpy1(unsigned int *DestAddr, unsigned int *SourceAddr, unsigned int memsize);
   void Reset_SYS(void);
   //object dictionary read/write functions
   unsigned long CO_OD_Read(ROM CO_objectDictionaryEntry* pODE, void* pBuff, unsigned char BuffMaxSize);
   unsigned long CO_OD_Write(ROM CO_objectDictionaryEntry* pODE, void* pBuff, unsigned char dataSize);


/*******************************************************************************
   Variables
*******************************************************************************/
//timer variables and macros (immune on overflow)
	unsigned int CO_SYNCtime1=0;
   volatile unsigned int CO_Timer16bit1ms = 0;   //16-bit variable, increments every 1ms
   volatile unsigned char CO_Timer8bit100ms = 0; //8-bit variable, increments every 100ms

   volatile unsigned int Following_error_Timer = 0;
   
   #define TMR8Z(ttimerVariable)   (unsigned char) (ttimerVariable = CO_Timer8bit100ms)
   #define TMR8(ttimerVariable)    (unsigned char) (CO_Timer8bit100ms - ttimerVariable)

//when CO_Timer1msIsr is executing, this variable is set to 1. This way some functions
//distinguish between being called from mainline or timer functions
   volatile unsigned char CO_Timer1msIsr_executing = 0;

//Status LEDs
//   volatile CO_StatusLED_struct CO_StatusLED = {1, 0};

//CANopen nodeID
   
   extern unsigned char CO_NodeID;

   extern volatile UNSIGNED8 ODE_RPDO_Mapping_NoOfEntries[8];
   extern volatile UNSIGNED8 ODE_TPDO_Mapping_NoOfEntries[8];
   extern volatile unsigned long TPDO_Old_Data[CO_NO_TPDO][2];

	CO_objectDictionaryEntry CO_TPDO_MAPing[CO_NO_TPDO][4]={
		#if CO_NO_TPDO > 0
		{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}},	
		#endif	
		#if CO_NO_TPDO > 1
		{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}},
		#endif
		#if CO_NO_TPDO > 2
		{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}},
		#endif
		#if CO_NO_TPDO > 3
		{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}},
		#endif
	};

	CO_objectDictionaryEntry CO_RPDO_MAPing[CO_NO_RPDO][4]={
		#if CO_NO_RPDO > 0
		{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}},	
		#endif	
		#if CO_NO_RPDO > 1
		{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}},
		#endif
		#if CO_NO_RPDO > 2
		{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}},
		#endif
		#if CO_NO_RPDO > 3
		{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}},
		#endif
	};

	volatile Uint16 TPDO_MAPPing_Flag[4] ={0,0,0,0};
	volatile Uint16 RPDO_MAPPing_Flag[4] ={0,0,0,0};


//CAN transmit
   volatile CO_DEFAULT_TYPE CO_TXCANcount;

//main variables for storing CAN messages, filled at CanInit, see CANopen.h for indexes
//   #pragma udata CO_udata_RXCAN
      volatile CO_CanMessage CO_RXCAN[CO_RXCAN_NO_MSGS];
//   #pragma udata

//   #pragma udata CO_udata_TXCAN
      volatile CO_CanMessage CO_TXCAN[CO_TXCAN_NO_MSGS];
//   #pragma udata

//NMT operating state of node
   volatile unsigned char CO_NMToperatingState;

//SYNC
   #if CO_NO_SYNC > 0
      volatile unsigned int CO_SYNCcounter;  //variable is incremented after SYNC message
      volatile unsigned int CO_SYNCtime;     //variable is incremented every 1ms, after SYNC message it is set to 0
      volatile unsigned int CO_SYNCwindow;   //window [ms]
      //(CO_RXCAN[CO_RXCAN_SYNC].Data is not used by CAN receive). It is used here to save space
      //EDC used SYNC
      //#define CO_SYNCperiod         CO_RXCAN[CO_RXCAN_SYNC].Data.WORD[0]   //period [ms]
      //#define CO_SYNCperiodTimeout  CO_RXCAN[CO_RXCAN_SYNC].Data.WORD[1]   //1,5 * CO_SYNCperiod [ms]
      //#define CO_SYNCperiodEarly    CO_RXCAN[CO_RXCAN_SYNC].Data.WORD[2]   //0,5 * CO_SYNCperiod [ms]
      //#define CO_SYNCproducer       CO_RXCAN[CO_RXCAN_SYNC].Data.BYTEbits[6].bit0   //if 1, node is SYNC producer
   	  volatile UNSIGNED16 CO_SYNCperiod;
	  volatile UNSIGNED16 CO_SYNCperiod_producer;
	  volatile UNSIGNED16 CO_SYNCperiodTimeout;
	  volatile UNSIGNED16 CO_SYNCperiodEarly;
	  volatile UNSIGNED16 CO_SYNCproducer;
   #endif

//Emergency message control and Error codes (see CO_errors.h)
   volatile struct{
      unsigned int CheckErrors     :1;
      unsigned int EmergencyToSend :1;
      unsigned int EmergencyErrorCode;
      unsigned char ErrorBit;
      unsigned int CodeVal;
   }ErrorControl;
   unsigned char CO_ErrorStatusBits[ERROR_NO_OF_BYTES];
   ROM unsigned int ErrorCodesTable[] = ERROR_CODES;

//SDO Server
   #if CO_NO_SDO_SERVER > 0
      struct{
         ROM CO_objectDictionaryEntry* pODE;          //Location of curent object dictionary entry
         unsigned char State;                         //state of SDO server
                     //bit1: 1=segmented download in progress
                     //bit2: 1=segmented upload in progress
                     //bit4: toggle bit in previous object
         #if CO_MAX_OD_ENTRY_SIZE > 0x04
            unsigned char Buff[CO_MAX_OD_ENTRY_SIZE]; //buffer for segmented transfer
            unsigned char BuffSize;                   //position in Buff of next data segment being read/written
            unsigned char tTimeout;                   //timeout timer
         #endif
      }CO_SDOserverVar[CO_NO_SDO_SERVER];
   #endif

//SDO Client
   #if CO_NO_SDO_CLIENT > 0
      struct{
         unsigned char State;                     //state of SDO client
                     //bit1: 1=segmented download in progress
                     //bit2: 1=segmented upload in progress
                     //bit4: toggle bit in previous object
                     //bit6: 1=download initiated
                     //bit7: 1=upload initiated
         unsigned char* pBuff;                     //pointer to data buffer supplied by user
         unsigned char BuffMaxSize;                //by download: size of data in buffer; by upload: size of buffer
         unsigned char BuffSize;                   //position in Buff of next data segment being read/written
         unsigned char tTimeout;                   //timeout timer
      }CO_SDOclientVar[CO_NO_SDO_CLIENT];
   #endif

//PDOs
   #if CO_NO_TPDO > 0
      #if CO_NO_SYNC > 0
         volatile unsigned char CO_TPDO_SyncTimer[CO_NO_TPDO];    //SYNC timer used for trigger PDO sending
      #endif
      //for test
	  volatile unsigned int CO_TPDO_InhibitTimer[CO_NO_TPDO];
      #ifdef CO_TPDO_INH_EV_TIMER
         volatile unsigned int CO_TPDO_InhibitTimer[CO_NO_TPDO];  //Inhibit timer used for inhibit PDO sending
         volatile unsigned int CO_TPDO_EventTimer[CO_NO_TPDO];    //Event timer used for trigger PDO sending
      #endif
   #endif

//HeartBeat Consumer (CO_RXCAN[CO_RXCAN_CONS_HB].Data (except first byte) is not used by CAN receive)
   volatile unsigned char CO_HBcons_AllMonitoredOperational = 0;
   unsigned char CO_HBcons_AllMonitoredOperationalCpy;
   #if CO_NO_CONS_HEARTBEAT > 0
      //#define CO_HBcons_NMTstate(i)    CO_RXCAN[CO_RXCAN_CONS_HB+i].Data.BYTE[1]//defined in CANopen.h
	  #define CO_HBcons_TimerValue_L(i)	(CO_RXCAN[CO_RXCAN_CONS_HB+i].Data.BYTE[2])
	  #define CO_HBcons_TimerValue_H(i)	(CO_RXCAN[CO_RXCAN_CONS_HB+i].Data.BYTE[3])
      #define CO_HBcons_MonStarted(i)  	CO_RXCAN[CO_RXCAN_CONS_HB+i].Data.BYTE[4]
   #endif

/*******************************************************************************
   CO_ResetComm - RESET COMMUNICATION
   Function is called after start of program and after CANopen NMT command: Reset
   Communication. It resets CAN interface and majority of CANopen variables.
   It also calls User_ResetComm() function.
*******************************************************************************/
void CO_ResetComm(void)
{
   CO_DEFAULT_SPEC CO_DEFAULT_TYPE i;//static unsigned char
   unsigned int index,subindex,j;
   unsigned int ii;

//   PCB_RUN_LED(CO_StatusLED.Off);
   CO_NMToperatingState = NMT_INITIALIZING;//=0
/* �ı�CAN�ڵ����� */
   CO_Read_NodeId_BitRate();
/* Clear arrays */
   for(ii=0; ii<sizeof(CO_RXCAN); ii++)
      *(((unsigned char*)CO_RXCAN)+ii) = 0;
   for(ii=0; ii<sizeof(CO_TXCAN); ii++)
      *(((unsigned char*)CO_TXCAN)+ii) = 0;

//   ODE_Communication_Cycle_Period = Pn[CANSyncPeriodADD];
/* Setup variables */
   //Sync		���Ը���OD�ĸı���ı�
   #if CO_NO_SYNC > 0
      CO_SYNCperiod = (unsigned int)(ODE_Communication_Cycle_Period / 100);//100us 1000);
	  CO_SYNCperiod_producer = (unsigned int)(ODE_Communication_Cycle_Period1 / 100);
      //CO_SYNCperiodTimeout = (unsigned int)((ODE_Communication_Cycle_Period * 3) / 2);//000);
	  CO_SYNCperiodTimeout = (unsigned int)((ODE_Communication_Cycle_Period * 2) / 100);//000);
      CO_SYNCperiodEarly = (unsigned int)((ODE_Communication_Cycle_Period) / 200);//100us 2000);
      CO_SYNCproducer = (ODE_SYNC_COB_ID & 0x40000000L)?1:0;
      CO_SYNCwindow = (unsigned int)(ODE_Synchronous_Window_Length / 100);//100us 1000);
      CO_SYNCcounter = 0;
      CO_SYNCtime = 0;
   
   //test SYNC 2007-6-25
//   ODE_TPDO_Parameter[0].Transmission_type=1;
//   CO_TXCAN[1].Inhibit=1;
   #endif

   //Errors and Emergency
   ErrorControl.EmergencyToSend = 0;
   ErrorControl.CheckErrors = 0;//1;
   for(i=0; i<ERROR_NO_OF_BYTES; i++)
      CO_ErrorStatusBits[i] = 0;
   ODE_Error_Register = 0;
   #if CO_NO_ERROR_FIELD > 0
      ODE_Pre_Defined_Error_Field_NoOfErrors = 0;
      for(i=0; i<CO_NO_ERROR_FIELD; i++)
         ODE_Pre_Defined_Error_Field[i] = 0;
   #endif

   //CAN transmit
   CO_TXCANcount = 0;

   //SDO Server
   #if CO_NO_SDO_SERVER > 0
      for(i=0; i<CO_NO_SDO_SERVER; i++) CO_SDOserverVar[i].State = 0;
   #endif

   //SDO Client
   #if CO_NO_SDO_CLIENT > 0
      for(i=0; i<CO_NO_SDO_CLIENT; i++) CO_SDOclientVar[i].State = 0;
   #endif

   //PDOs
   #if CO_NO_TPDO > 0
      for(i=0; i<CO_NO_TPDO; i++){
         #if CO_NO_SYNC > 0
            CO_TPDO_SyncTimer[i] = ODE_TPDO_Parameter[i].Transmission_type;
         #endif
         #ifdef CO_TPDO_INH_EV_TIMER
            CO_TPDO_InhibitTimer[i] = 0;
            CO_TPDO_EventTimer[i] = ODE_TPDO_Parameter[i].Event_Timer * 10;
         #endif
      }
   #endif

//User function
//   User_ResetComm();

//Setup CO_RXCAN array	//NMT	COB-ID
   CO_RXCAN[CO_RXCAN_NMT].Ident.BYTE[0] = CO_IDENT_WRITE(0, 0);
   CO_RXCAN[CO_RXCAN_NMT].NoOfBytes = 2;
   #if CO_NO_SYNC > 0	//SYNC	COB-ID
      CO_RXCAN[CO_RXCAN_SYNC].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_SYNC_COB_ID, 0);
      CO_RXCAN[CO_RXCAN_SYNC].NoOfBytes = 0;
      CO_RXCAN[CO_RXCAN_SYNC].Inhibit = 1;
   #endif
   
   
   //added by Liu Yan 2011-08-31
	ODE_RPDO_Parameter[0].COB_ID = CAN_ID_RPDO0 + CO_NodeID;
	#if CO_NO_RPDO > 1
		ODE_RPDO_Parameter[1].COB_ID = CAN_ID_RPDO1 + CO_NodeID;
	#endif
	#if CO_NO_RPDO > 2
		ODE_RPDO_Parameter[2].COB_ID = CAN_ID_RPDO2 + CO_NodeID;
	#endif
	#if CO_NO_RPDO > 3
		ODE_RPDO_Parameter[3].COB_ID = CAN_ID_RPDO3 + CO_NodeID;
	#endif	
				
   #if CO_NO_RPDO > 0	//RPDO COB-ID
      for(i=CO_RXCAN_RPDO; i<CO_RXCAN_RPDO+CO_NO_RPDO; i++){
         unsigned char length;
         #ifdef CO_PDO_MAPPING_IN_OD	//Enable map
            //unsigned char j;
            //calculate length from mapping
            length = 7;  //round up to use whole byte
            for(j=0; j<ODE_RPDO_Mapping_NoOfEntries[i-CO_RXCAN_RPDO]; j++)//���һ���ֽڱ�ʾ���ݳ���
               length += (ODE_RPDO_Mapping[i-CO_RXCAN_RPDO][j] & 0xFF);
            length >>= 3;
            if(length > 8){
               length = 8;
               ErrorReport(ERROR_wrong_PDO_mapping, i-CO_RXCAN_RPDO);
            }
         #else
            length = 9; //size of RPDO is not important
         #endif
         //if((ODE_RPDO_Parameter[i-CO_RXCAN_RPDO].COB_ID & 0x80000000L) == 0 && length){//is RPDO used
	
         if((ODE_RPDO_Parameter[i-CO_RXCAN_RPDO].COB_ID & 0x80000000L) == 0){
            CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_RPDO_Parameter[i-CO_RXCAN_RPDO].COB_ID, 0);
            CO_RXCAN[i].NoOfBytes = length;
            switch(i-CO_RXCAN_RPDO){
               case 0:  //First RPDO: is used standard predefined COB-ID?
                  #if ODD_RPDO_PAR_COB_ID_0 == 0
                     CO_RXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
				  #else
					 CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_RPDO_Parameter[i-CO_RXCAN_RPDO].COB_ID, 0);
                  #endif
                  break;
               case 1:  //Second RPDO: is used standard predefined COB-ID?
                  #if ODD_RPDO_PAR_COB_ID_1 == 0
                     CO_RXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
				  #else
					 CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_RPDO_Parameter[i-CO_RXCAN_RPDO].COB_ID, 0);
                  #endif
                  break;
               case 2:  //Third RPDO: is used standard predefined COB-ID?
                  #if ODD_RPDO_PAR_COB_ID_2 == 0
                     CO_RXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
                  #else
					 CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_RPDO_Parameter[i-CO_RXCAN_RPDO].COB_ID, 0);
                  #endif
                  break;
               case 3:  //Fourth RPDO: is used standard predefined COB-ID?
                  #if ODD_RPDO_PAR_COB_ID_3 == 0
                     CO_RXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
                  #else
					 CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_RPDO_Parameter[i-CO_RXCAN_RPDO].COB_ID, 0);
                  #endif
                  break;
            }
         }
      }
	  //���ֵ��е�����
	  for(i=0; i<CO_NO_RPDO; i++)
	  {//unsigned char j;
		for(j=0;j<ODE_RPDO_Mapping_NoOfEntries[i];j++)
		{
			//unsigned int index,subindex;
			ROM CO_objectDictionaryEntry  * pODE1;
			index = (ODE_RPDO_Mapping[i][j]>>16) & 0xFFFF;
			subindex = (ODE_RPDO_Mapping[i][j]>>8) & 0xFF;
			pODE1 = CO_FindEntryInOD(index, subindex);
			CO_Find_RPDOmapData(pODE1,i,j);
         }
	  }
   #endif
   #if CO_NO_SDO_SERVER > 0
      for(i=CO_RXCAN_SDO_SRV; i<CO_RXCAN_SDO_SRV+CO_NO_SDO_SERVER; i++){
         if((ODE_Server_SDO_Parameter[i-CO_RXCAN_SDO_SRV].COB_ID_Client_to_Server & 0x80000000L) == 0 &&
            (ODE_Server_SDO_Parameter[i-CO_RXCAN_SDO_SRV].COB_ID_Server_to_Client & 0x80000000L) == 0){//is SDO used
            CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_Server_SDO_Parameter[i-CO_RXCAN_SDO_SRV].COB_ID_Client_to_Server, 0);
            CO_RXCAN[i].NoOfBytes = 8;
            CO_RXCAN[i].Inhibit = 1;
            if((i-CO_RXCAN_SDO_SRV) == 0) //first SDO? -> standard predefined COB-ID is used.
               CO_RXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
         }
      }
   #endif
   #if CO_NO_SDO_CLIENT > 0
      for(i=CO_RXCAN_SDO_CLI; i<CO_RXCAN_SDO_CLI+CO_NO_SDO_CLIENT; i++){
         if((ODE_Client_SDO_Parameter[i-CO_RXCAN_SDO_CLI].COB_ID_Client_to_Server & 0x80000000L) == 0 &&
            (ODE_Client_SDO_Parameter[i-CO_RXCAN_SDO_CLI].COB_ID_Server_to_Client & 0x80000000L) == 0){//is SDO used
            CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_Client_SDO_Parameter[i-CO_RXCAN_SDO_CLI].COB_ID_Server_to_Client, 0);
            CO_RXCAN[i].NoOfBytes = 8;
            CO_RXCAN[i].Inhibit = 1;
         }
      }
   #endif
   #if CO_NO_CONS_HEARTBEAT > 0
      for(i=CO_RXCAN_CONS_HB; i<CO_RXCAN_CONS_HB+CO_NO_CONS_HEARTBEAT; i++){
         //if(ODE_Consumer_Heartbeat_Time[i-CO_RXCAN_CONS_HB] & 0x00FF0000L)
            CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)(((ODE_Consumer_Heartbeat_Time[i-CO_RXCAN_CONS_HB]>>16)&0xFF) | CAN_ID_HEARTBEAT), 0);
         //else CO_RXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE(0, 0);
         CO_RXCAN[i].NoOfBytes = 1;
         CO_RXCAN[i].Inhibit = 1;
      }
   #endif

/* Setup CO_TXCAN array */
   #if CO_NO_SYNC > 0
      CO_TXCAN[CO_TXCAN_SYNC].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_SYNC_COB_ID, 0);
      CO_TXCAN[CO_TXCAN_SYNC].NoOfBytes = 0;
   #endif
   #if CO_NO_EMERGENCY > 0
      CO_TXCAN[CO_TXCAN_EMERG].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_Emergency_COB_ID+CO_NodeID, 0);
      CO_TXCAN[CO_TXCAN_EMERG].NoOfBytes = 8;
   #endif

	//added by Liu Yan 2100-08-31
	 ODE_TPDO_Parameter[0].COB_ID = CAN_ID_TPDO0 + CO_NodeID;
	 #if CO_NO_TPDO > 1
	 	ODE_TPDO_Parameter[1].COB_ID = CAN_ID_TPDO1 + CO_NodeID;
	 #endif
	 #if CO_NO_RPDO > 2
	 	ODE_TPDO_Parameter[2].COB_ID = CAN_ID_TPDO2 + CO_NodeID;
	 #endif
	 #if CO_NO_TPDO > 3
	 	ODE_TPDO_Parameter[3].COB_ID = CAN_ID_TPDO3 + CO_NodeID;
	 #endif

   #if CO_NO_TPDO > 0
      for(i=CO_TXCAN_TPDO; i<CO_TXCAN_TPDO+CO_NO_TPDO; i++){
         unsigned char length;
         #ifdef CO_PDO_MAPPING_IN_OD
            //unsigned char j;
            //calculate length from mapping
            length = 7;  //round up to use whole byte
            for(j=0; j<ODE_TPDO_Mapping_NoOfEntries[i-CO_TXCAN_TPDO]; j++)
               length += (ODE_TPDO_Mapping[i-CO_TXCAN_TPDO][j] & 0xFF);
            length >>= 3;
            if(length > 8){
               length = 8;
               ErrorReport(ERROR_wrong_PDO_mapping, i-CO_TXCAN_TPDO);
            }
         #else
            length = 8;    //8 bytes long TPDO will be transmitted
         #endif
         //if((ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].COB_ID & 0x80000000L) == 0 && length){//is TPDO used
              
         if((ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].COB_ID & 0x80000000L) == 0)
         {
            CO_TXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].COB_ID, 0);
            CO_TXCAN[i].NoOfBytes = length;
            CO_TXCAN[i].Inhibit = 0;
            #if CO_NO_SYNC > 0
               if((ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].Transmission_type-1) <= 239)
                  CO_TXCAN[i].Inhibit = 1; //mark Synchronous TPDOs
            #endif
            switch(i-CO_TXCAN_TPDO){
               case 0:  //First TPDO: is used standard predefined COB-ID?
                  #if ODD_TPDO_PAR_COB_ID_0 == 0
                     CO_TXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
				  #else
					 CO_TXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].COB_ID, 0);
                  #endif
                  break;
               case 1:  //Second TPDO: is used standard predefined COB-ID?
                  #if ODD_TPDO_PAR_COB_ID_1 == 0
                     CO_TXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
				  #else
					 CO_TXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].COB_ID, 0);
                  #endif
                  break;
               case 2:  //Third TPDO: is used standard predefined COB-ID?
                  #if ODD_TPDO_PAR_COB_ID_2 == 0
                     CO_TXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
                  #else
					 CO_TXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].COB_ID, 0);
                  #endif
                  break;
               case 3:  //Fourth TPDO: is used standard predefined COB-ID?
                  #if ODD_TPDO_PAR_COB_ID_3 == 0
                     CO_TXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
                  #else
					 CO_TXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].COB_ID, 0);
                  #endif
                  break;
            }
         }
      }
	   //���ֵ��е�����
	  for(i = 0; i<CO_NO_TPDO; i++)
	  {
		for(j=0;j<ODE_TPDO_Mapping_NoOfEntries[i];j++)
		{
			ROM CO_objectDictionaryEntry  * pODE1;
			index = (ODE_TPDO_Mapping[i][j]>>16) & 0xFFFF;
			subindex = (ODE_TPDO_Mapping[i][j]>>8) & 0xFF;
			pODE1 = CO_FindEntryInOD(index, subindex);
			CO_Find_TPDOmapData(pODE1,i,j);
         }
	  }
   #endif
   #if CO_NO_SDO_SERVER > 0
      for(i=CO_TXCAN_SDO_SRV; i<CO_TXCAN_SDO_SRV+CO_NO_SDO_SERVER; i++){
         if((ODE_Server_SDO_Parameter[i-CO_TXCAN_SDO_SRV].COB_ID_Client_to_Server & 0x80000000L) == 0 &&
            (ODE_Server_SDO_Parameter[i-CO_TXCAN_SDO_SRV].COB_ID_Server_to_Client & 0x80000000L) == 0){//is SDO used
            CO_TXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_Server_SDO_Parameter[i-CO_TXCAN_SDO_SRV].COB_ID_Server_to_Client, 0);
            CO_TXCAN[i].NoOfBytes = 8;
            if((i-CO_TXCAN_SDO_SRV) == 0) //first SDO? -> standard predefined COB-ID is used.
               CO_TXCAN[i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
         }
      }
   #endif
   #if CO_NO_SDO_CLIENT > 0
      for(i=CO_TXCAN_SDO_CLI; i<CO_TXCAN_SDO_CLI+CO_NO_SDO_CLIENT; i++){
         if((ODE_Client_SDO_Parameter[i-CO_TXCAN_SDO_CLI].COB_ID_Client_to_Server & 0x80000000L) == 0 &&
            (ODE_Client_SDO_Parameter[i-CO_TXCAN_SDO_CLI].COB_ID_Server_to_Client & 0x80000000L) == 0){//is SDO used
            CO_TXCAN[i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_Client_SDO_Parameter[i-CO_TXCAN_SDO_CLI].COB_ID_Client_to_Server, 0);
            CO_TXCAN[i].NoOfBytes = 8;
         }
      }
   #endif
   CO_TXCAN[CO_TXCAN_HB].Ident.BYTE[0] = CO_IDENT_WRITE(CAN_ID_HEARTBEAT + CO_NodeID, 0);
   CO_TXCAN[CO_TXCAN_HB].NoOfBytes = 1;

/* Setup CAN bus */
   CO_SetupCAN();
}


/*******************************************************************************
   CO_TXCANsend - MARK CAN MESSAGE FOR SENDING
   Function can be called from mainline or timer function. When message is prepared,
   it marks it for sending and switch on CANTX interrupt, which will send the
   message when buffers are ready.

   PARAM Index:   index of CO_TXCAN array to be send
   RETURN:        0 = success
                  1 = error, previous message was not sent, buffer full
*******************************************************************************/
char CO_TXCANsend(unsigned int index)
{Uint16 temp;
 Uint32 tmp;
   if(CO_TXCAN[index].NewMsg){
      ErrorReport(ERROR_CAN_OVERFLOW, index);
      return 1;
   }
   temp = (1<<index);
   tmp = ECanbRegs.CANTA.all;
   if((tmp & temp) || !(CANinit & temp))
   {
      #if CO_NO_SYNC > 0
         //messages with Inhibit flag set (synchronous PDOs) must be transmited inside preset window
         if(CO_TXCAN[index].Inhibit && CO_SYNCwindow && CO_SYNCtime > CO_SYNCwindow)
            ErrorReport(ERROR_TPDO_OUTSIDE_WINDOW, index);
         else
      #endif
         CO_TXCAN_COPY_TO_BUFFERS(index);
         CANinit |= temp;
   }
   else
   {                         //interrupt will send it
      CO_TXCAN[index].NewMsg = 1;
      CO_TXCANcount++;
   }
   return 0;
}

/*******************************************************************************
   CO_TPDOsend - MARK TPDO FOR SENDING
   Function can be called from timer function. it marks TPDO for
   sending. If TPDO has to be prepared, this can be done here.

   PARAM Index:   index of PDO to be send (0 = first PDO)
   RETURN:        0 = success
                  1 = error, previous TPDO was not sent, buffer full
                  2 = error, TPDO was inhibited
*******************************************************************************/
char CO_TPDOsend(unsigned int index)
{
   	#ifdef CO_TPDO_INH_EV_TIMER
   	if(CO_TXCAN[CO_TXCAN_TPDO+index].Inhibit == 0) // not sync TPDO
   	{
      	if(CO_TPDO_InhibitTimer[index]) 
      	{
      		return 2;
		}
      	CO_TPDO_InhibitTimer[index] = ODE_TPDO_Parameter[index].Inhibit_Time;//100us
   	}
   	#endif

   	return CO_TXCANsend(CO_TXCAN_TPDO+index);
}


/*******************************************************************************
   CO_Timer1msIsr - 1 ms TIMER FUNCTION
   Function is executed every 1 ms. It is deterministic and has priority over
   mainline functions.
*******************************************************************************/
void CO_Timer01msIsr(void)
{
   //int test;
   CO_DEFAULT_SPEC CO_DEFAULT_TYPE i;
   static Uint16 timer100ms = 0,timer1ms = 0;
   //Heartbeat producer timer variable

//information for some functions (when variable = 1, timer is executing and mainline is waiting)
   CO_Timer1msIsr_executing = 1;

//Increment timer variables 
   if(++timer1ms>=10)
   {
   		CO_Timer16bit1ms++;
		timer1ms = 0;
   }
   if(++timer100ms >= 1000)
   {
      CO_Timer8bit100ms++;
      timer100ms = 0;
   }

//SYNC timer/counter variables, SYNC producer 
   #if CO_NO_SYNC > 0	//û�� SYNC producer
      //test=CO_SYNCperiod; 2007-7-9 ��"Operation"״̬�¼��ͬ���ź�
      //if(CO_SYNCperiod && (CO_NMToperatingState==NMT_PRE_OPERATIONAL || CO_NMToperatingState==NMT_OPERATIONAL)){
      if(CO_SYNCperiod && (CO_NMToperatingState==NMT_OPERATIONAL)){
         //increment SYNC timer variable
         CO_SYNCtime++;
         if(CO_SYNCtime>65500)
         {
         	CO_SYNCtime=65500;//2008-9-17
		 }
         //Verify if new SYNC message received
         if(CO_RXCAN[CO_RXCAN_SYNC].NewMsg){
            //Verify is SYNC is to early (two producers)
            if(CO_SYNCtime<CO_SYNCperiodEarly && CO_NMToperatingState == NMT_OPERATIONAL){
               ErrorReport(ERROR_SYNC_EARLY, CO_SYNCtime/10);//�������ֻ�Ե�һ�ε�ͨѶ�ź���Ӱ�죬�費��Ҫ��
            }
            CO_SYNCcounter++;
			CO_SYNCtime1 = CO_SYNCtime;
            CO_SYNCtime = 0;
            CO_RXCAN[CO_RXCAN_SYNC].NewMsg = 0;
			Syncflag = 1;
         }

         //SYNC producer (On CAN bus must NOT exist two equal SYNC producers)
      }

	  if(CO_SYNCproducer)
	  {	//û�� SYNC producer
			static unsigned int tSYNCperiod = 0;
			if(++tSYNCperiod >= CO_SYNCperiod_producer)
			{
				CO_TXCANsend(CO_TXCAN_SYNC);
				//               CO_SYNCcounter++;	//deleted on 2011-11-05
				//               CO_SYNCtime = 0;	//deleted on 2011-11-05
				tSYNCperiod = 0;
			}
	  }//end of SYNC producer
   #endif

//Erase RPDOs if not operational PDO����
   #if CO_NO_RPDO > 0
      if(CO_NMToperatingState != NMT_OPERATIONAL){
		CO_RPDO_New(0) = 0;
		#if CO_NO_RPDO > 1
		CO_RPDO_New(1) = 0;
		#endif
		#if CO_NO_RPDO > 2
		CO_RPDO_New(2) = 0;
		#endif
		#if CO_NO_RPDO > 3
		CO_RPDO_New(3) = 0;
		#endif
      }
   #endif

//User timer procedure
   User_Process01msIsr();   

//Send Synchronous PDOs and verify timeout of SYNC
   #if CO_NO_SYNC > 0
      if(CO_SYNCperiod && CO_NMToperatingState == NMT_OPERATIONAL)
      {
         #if CO_NO_TPDO > 0
            if(CO_SYNCtime == 0)
            {
               for(i=0; i<CO_NO_TPDO; i++)
               {
                  //is TPDO Synchronous (Inhibit flag) and is time to be send
                  if(CO_TXCAN[CO_TXCAN_TPDO+i].Inhibit && --CO_TPDO_SyncTimer[i] == 0
                  	&& (ODE_TPDO_Parameter[i].COB_ID & 0x80000000L) == 0)
                  {
                     CO_TPDO_SyncTimer[i] = ODE_TPDO_Parameter[i].Transmission_type;
                     CO_TPDOsend(i);
                  }
               }
            }
         #endif

         //Verify timeout of SYNC
         if(CO_SYNCtime>CO_SYNCperiodTimeout){
            ErrorReport(ERROR_SYNC_TIME_OUT, CO_SYNCtime/10);
		}
      }
   #endif

//PDO transmission - Inhibit and Event timer 
   #ifdef CO_TPDO_INH_EV_TIMER	//test sync 2007-6-25
      for(i=0; i<CO_NO_TPDO; i++)
      {
         if(ODE_TPDO_Mapping_NoOfEntries[i] && (CO_TXCAN[CO_TXCAN_TPDO+i].Inhibit == 0)
         	&& (ODE_TPDO_Parameter[i].COB_ID & 0x80000000L) == 0)
		 {
         	if(CO_TPDO_InhibitTimer[i])
         	{ 
         		CO_TPDO_InhibitTimer[i]--;
			}
			if(ODE_TPDO_Parameter[i].Transmission_type == 255)
			{
	         	if(ODE_TPDO_Parameter[i].Event_Timer && --CO_TPDO_EventTimer[i] == 0)
	         	{
	            	CO_TPDO_EventTimer[i] = ODE_TPDO_Parameter[i].Event_Timer * 10;
	            	if(CO_NMToperatingState == NMT_OPERATIONAL)
					{
	               		CO_TPDOsend(i);
					}
	         	}
			}
		 }
      }
   #endif
/* information for some functions */
   CO_Timer1msIsr_executing = 0;

}

void Error_Servo(int alarmno)
{
	if(!state_flag2.bit.DispAlm)
	{
		alarmno = 0;
		return;
	}

	if(alarmno == 12)
	{
		ErrorReport(ERROR_OVER_CURRENT,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_OVER_CURRENT))
	{
    	ErrorReset(ERROR_OVER_CURRENT, alarmno);
	}

	if(alarmno == 13)
	{
		ErrorReport(ERROR_MAIN_OVER_VOLTAGE,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_MAIN_OVER_VOLTAGE))
	{
    	ErrorReset(ERROR_MAIN_OVER_VOLTAGE, alarmno);
	}
 
	if(alarmno == 14)
	{
		ErrorReport(ERROR_MAIN_UNDER_VOLTAGE,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_MAIN_UNDER_VOLTAGE))
	{
    	ErrorReset(ERROR_MAIN_UNDER_VOLTAGE, alarmno);
	}
 
	if(alarmno == 21)
	{
		ErrorReport(ERROR_MAIN_VOLTAGE_STOP,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_MAIN_VOLTAGE_STOP))
	{
    	ErrorReset(ERROR_MAIN_VOLTAGE_STOP, alarmno);
	}
 
	if(alarmno == 80)
	{
		ErrorReport(ERROR_RAM,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_RAM))
	{
    	ErrorReset(ERROR_RAM, alarmno);
	}
 
	if((alarmno == 21) || (alarmno == 8) || (alarmno == 9))
	{
		ErrorReport(ERROR_ADC,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_ADC))
	{
    	ErrorReset(ERROR_ADC, alarmno);
	}
 
	if(alarmno == 15)
	{
		ErrorReport(ERROR_XIEFANG,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_XIEFANG))
	{
    	ErrorReset(ERROR_XIEFANG, alarmno);
	}

	if(alarmno == 16)
	{
		ErrorReport(ERROR_ZAISHENG,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_ZAISHENG))
	{
    	ErrorReset(ERROR_ZAISHENG, alarmno);
	}
 
	if(alarmno == 2)
	{
		ErrorReport(ERROR_SUMCHECK,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_SUMCHECK))
	{
    	ErrorReset(ERROR_SUMCHECK, alarmno);
	}
 
	if(alarmno == 7)
	{
		ErrorReport(ERROR_GEAR,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_GEAR))
	{
    	ErrorReset(ERROR_GEAR, alarmno);
	}
 
	if((alarmno == 42)||(alarmno == 43))
	{
		ErrorReport(ERROR_MOTORSEL,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_MOTORSEL))
	{
    	ErrorReset(ERROR_MOTORSEL, alarmno);
	}
 
	if((alarmno == 10) || (alarmno == 11) || (alarmno == 30) || (alarmno == 31) || (alarmno == 32))
	{
		ErrorReport(ERROR_INCREAME_PG,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_INCREAME_PG))
	{
    	ErrorReset(ERROR_INCREAME_PG, alarmno);
	}
 
	if((alarmno >= 50) && (alarmno <= 59))
	{
		ErrorReport(ERROR_SPG,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_SPG))
	{
    	ErrorReset(ERROR_SPG, alarmno);
	}

	if((alarmno >= 22) && (alarmno <= 25))
	{
		ErrorReport(ERROR_RESOLVER,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_RESOLVER))
	{
    	ErrorReset(ERROR_RESOLVER, alarmno);
	}
 
	if(alarmno == 4)
	{
		ErrorReport(ERROR_OVER_TORQE,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_OVER_TORQE))
	{
    	ErrorReset(ERROR_OVER_TORQE, alarmno);
	}
 
	if(alarmno == 3)
	{
		ErrorReport(ERROR_OVER_SPEED,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_OVER_SPEED))
	{
    	ErrorReset(ERROR_OVER_SPEED, alarmno);
	}

	if(alarmno)
	{
		ErrorReport(ERROR_USER_0_IMPORTANT,alarmno);
		return;
	}
	else if(ERROR_BIT_READ(ERROR_USER_0_IMPORTANT))
	{
    	ErrorReset(ERROR_USER_0_IMPORTANT, alarmno);
	}
}
/*******************************************************************************
   ErrorReport - REPORT ERROR CONDITION
   Function is used to report any error occurred in program. It can be used from
   mainline or interrupt. It sets appropriate error bit and sends emergency if
   it is the first time for that bit. If critical bits are set, node will not be
   able to enter operational state. For detailed description see CO_errors.h
   PARAM ErrorBit: specific error bit, use defined constants
   PARAM Code: informative value, send as 4-th and 5-th byte in Emergency message
*******************************************************************************/
void ErrorReport(unsigned char ErrorBit, unsigned int Code){
   unsigned char index = ErrorBit >> 3;
   unsigned char bitmask = 1 << (ErrorBit & 0x7);

   //if ErrorBit value not supported, set ERROR_ErrorReport_ParametersNotSupp
   if(index > (ERROR_NO_OF_BYTES-1)){		//4-1
      index = 0; bitmask = 2;
   }
   if((CO_ErrorStatusBits[index]&bitmask) == 0){
      if(ErrorBit) CO_ErrorStatusBits[index] |= bitmask; //set bit, if NO_ERROR just send emergency
      ErrorControl.CheckErrors = 1;
      if(!ErrorControl.EmergencyToSend){  //free
         ErrorControl.EmergencyToSend = 1;
         ErrorControl.EmergencyErrorCode = ErrorCodesTable[ErrorBit];
         ErrorControl.ErrorBit = ErrorBit;
         ErrorControl.CodeVal = Code;
      }
   }
}


/*******************************************************************************
   ErrorReset - RESET ERROR CONDITION
   Function is used to report any if error condition is no more present. It can
   be used from mainline or interrupt function. It resets appropriate error bit
   and sends emergency 'no error'.
   For detailed description see CO_errors.h
   PARAM ErrorBit: specific error bit, use defined constants
   PARAM Code: informative value, send as 4-th and 5-th byte in Emergency message
*******************************************************************************/
void ErrorReset(unsigned char ErrorBit, unsigned int Code){
   unsigned char index = ErrorBit >> 3;
   unsigned char bitmask = 1 << (ErrorBit & 0x7);

   if(index > (ERROR_NO_OF_BYTES-1)){
      ErrorReport(ERROR_ErrorReport_ParametersNotSupp, ErrorBit);
      return;
   }
   if((CO_ErrorStatusBits[index]&bitmask) != 0){
      CO_ErrorStatusBits[index] &= ~bitmask; //erase bit
      ErrorControl.CheckErrors = 1;
      if(!ErrorControl.EmergencyToSend){  //free
         ErrorControl.EmergencyToSend = 1;
         ErrorControl.EmergencyErrorCode = 0x0000; //no error
         ErrorControl.ErrorBit = ErrorBit;
         ErrorControl.CodeVal = Code;
      }
   }
}


/*******************************************************************************
********************************************************************************

   MAINLINE FUNCTIONS

********************************************************************************
*******************************************************************************/

/*******************************************************************************
   CO_FindEntryInOD - SEARCH OBJECT DICTIONARY
   Function is used for searching object dictionary for entry with specified
   index and subindex. It searches OD from beginning to end and if matched, returns
   poiner to entry. It is usually called from SDO server.
   If Object Dictionary exist in multiple arrays, this function must search all.

   PARAM index, subindex: address of entry in object dictionary
   RETURN: if found, pointer to entry, othervise 0
   �ֵ���Ҫ����˳�������� ���ֵ�ʱ���ǱȽ������Ĵ�С
*******************************************************************************/
ROM CO_objectDictionaryEntry* CO_FindEntryInOD(unsigned int index, unsigned char subindex)
{
   CO_DEFAULT_SPEC  unsigned int Index;
   CO_DEFAULT_SPEC unsigned char SubIndex;
   #ifdef CO_OD_IS_ORDERED
      //Fast search in ordered Object Dictionary. If indexes or subindexes are mixed, this won't work.
      //If CO_OD has up to 2^N entries, then N is max number of loop passes.
      CO_DEFAULT_SPEC unsigned int cur, min, max, CurIndex;
      CO_DEFAULT_SPEC unsigned char CurSubIndex;
      Index = index;
      SubIndex = subindex;
      min = 0;
      max = CO_OD_NoOfElements - 1;
      while(min < max){
         cur = (min + max) / 2;
         CurIndex = CO_OD[cur].index;
         CurSubIndex = CO_OD[cur].subindex;
         if(Index == CurIndex && SubIndex == CurSubIndex)
            return &CO_OD[cur];			//--------
         else if(Index < CurIndex || (Index == CurIndex && SubIndex < CurSubIndex)){
            max = cur;
            if(max) max--;
         }
         else
            min = cur + 1;
      }
      if(min == max){
         cur = min;
         CurIndex = CO_OD[cur].index;
         CurSubIndex = CO_OD[cur].subindex;
         if(Index == CurIndex && SubIndex == CurSubIndex)
            return &CO_OD[cur];
      }
   #else
      //search OD from first to last entry
      CO_DEFAULT_SPEC unsigned int i;
      ROM CO_objectDictionaryEntry* pODE;
      Index = index;
      SubIndex = subindex;
      pODE = &CO_OD[0];
      for(i = CO_OD_NoOfElements; i>0; i--){
         if(Index == pODE->index && SubIndex == pODE->subindex) return pODE;
         pODE++;
      }
   #endif
   return 0;  //object does not exist in OD
}

/*******************************************************************************
   	CO_ProcessMain - PROCESS CANOPEN MAINLINE
   	This function is cyclycally called from main(). It is non blocking function.
   	It is asynchronous. Here is longer and time consuming code.
	CANͨѶ������
*******************************************************************************/
void CO_ProcessMain(void)
{
	unsigned int index,subindex,tmp;
	unsigned long longtmp;
//variables
   //multipurpose usage
   CO_DEFAULT_SPEC CO_DEFAULT_TYPE i, j;
   //SDO server
   #if CO_NO_SDO_SERVER > 1
      static unsigned char SDOserverChannel = 0;
   #endif
   //Heartbeat producer timer variable
   static unsigned int tProducerHeartbeatTime = 0;

//16 bit mainline timer variable
   unsigned int CO_Timer16bit1msCopy;

   //Can_Transmit_Data();
   CO_DrvStateMachine();

   CO_DISABLE_TMR();
   CO_Timer16bit1msCopy = CO_Timer16bit1ms;
   CO_ENABLE_TMR();
   #define TMR16Z(ttimerVariable)  (unsigned int)(ttimerVariable = CO_Timer16bit1msCopy)
   #define TMR16(ttimerVariable)   (unsigned int)(CO_Timer16bit1msCopy - ttimerVariable)
//CAN�ڵ㡢�ٶȸı䴦��
//    Change_CAN_MOD();
//Process microcontroller specific code 
    CO_ProcessDriver();
//Verify if new NMT message received 
   if(CO_RXCAN[CO_RXCAN_NMT].NewMsg){
      if((CO_RXCAN[CO_RXCAN_NMT].Data.BYTE[1] & 0xFF) == 0 || (CO_RXCAN[CO_RXCAN_NMT].Data.BYTE[1] & 0xFF) == CO_NodeID){
         switch((CO_RXCAN[CO_RXCAN_NMT].Data.BYTE[0]) & 0xFF){//switch NMT command
            //1:Start Remote Node
            case NMT_ENTER_OPERATIONAL:      CO_NMToperatingState = NMT_OPERATIONAL;      break;
            //2:Stop Remote Node
            case NMT_ENTER_STOPPED:          CO_NMToperatingState = NMT_STOPPED;          break;
            //128:Enter Pre-operational State
            case NMT_ENTER_PRE_OPERATIONAL:  CO_NMToperatingState = NMT_PRE_OPERATIONAL;  break;
            //129:Reset Node				2007-5-8
            case NMT_RESET_NODE:             CO_NMToperatingState = NMT_RESET_NODE;
            								 CO_Reset();    
            								 EALLOW;
	    									 SysCtrlRegs.WDKEY = 0x0000;
	    									 SysCtrlRegs.WDKEY = 0x0001;
	    									 SysCtrlRegs.WDCR  = 0x0020;
	    									 EDIS;   
            								 break;
            //130:Reset Communication
            case NMT_RESET_COMMUNICATION:    CO_DISABLE_ALL();
											 CO_NMToperatingState = NMT_RESET_COMMUNICATION;
											 CO_DrvState = NotReadyToSwitchOnState;	
    										 CO_Statusword.all &= 0xFFB0;
                                             CO_ResetComm();
											 CO_DrvState = SwitchOnDisabledState;	
    										 CO_Statusword.bit.SwitchednOn = 1;
                                             CO_ENABLE_ALL();  
                                             break;
            default: ErrorReport(ERROR_CO_RXMSG_NMTcmd, 0);      //Error in NMT Command
         }
      }
      CO_RXCAN[CO_RXCAN_NMT].NewMsg = 0;
   }
//PDO mapping
	Dynamic_PDO_Maping();

	if(CO_NMToperatingState == NMT_PRE_OPERATIONAL)
	{
		#if CO_NO_SYNC > 0
	      CO_SYNCperiod = (unsigned int)(ODE_Communication_Cycle_Period / 100);//100us 1000);
		  CO_SYNCperiod_producer = (unsigned int)(ODE_Communication_Cycle_Period1 / 100);
	      //CO_SYNCperiodTimeout = (unsigned int)((ODE_Communication_Cycle_Period * 3) / 2);//000);
		  CO_SYNCperiodTimeout = (unsigned int)((ODE_Communication_Cycle_Period * 2) / 100);//000);
	      CO_SYNCperiodEarly = (unsigned int)((ODE_Communication_Cycle_Period) / 200);//100us 2000);
	      CO_SYNCproducer = (ODE_SYNC_COB_ID & 0x40000000L)?1:0;
	      CO_SYNCwindow = (unsigned int)(ODE_Synchronous_Window_Length / 100);//100us 1000);
	      CO_SYNCcounter = 0;
	      CO_SYNCtime = 0;
	   
	   //test SYNC 2007-6-25
	//   ODE_TPDO_Parameter[0].Transmission_type=1;
	//   CO_TXCAN[1].Inhibit=1;
	   #endif
   }
//TPDO transmit type
//	Transmit_Type_Change();
//Error handling and Emergency message sending
   //calculate Error register
   if(ErrorControl.CheckErrors){
      ErrorControl.CheckErrors = 0;
      //generic error
      if(ERROR_REGISTER_BIT0_CONDITION) ODE_Error_Register |= 0x01;
      else                              ODE_Error_Register &= 0xFE;
      //current
      if(ERROR_REGISTER_BIT1_CONDITION) ODE_Error_Register |= 0x02;
      else                              ODE_Error_Register &= 0xFD;
      //voltage
      if(ERROR_REGISTER_BIT2_CONDITION) ODE_Error_Register |= 0x04;
      else                              ODE_Error_Register &= 0xFB;
      //temperature
      if(ERROR_REGISTER_BIT3_CONDITION) ODE_Error_Register |= 0x08;
      else                              ODE_Error_Register &= 0xF7;
      //communication error (overrun, error state)
      if(ERROR_REGISTER_BIT4_CONDITION) ODE_Error_Register |= 0x10;
      else                              ODE_Error_Register &= 0xEF;
      //device profile specific error
      if(ERROR_REGISTER_BIT5_CONDITION) ODE_Error_Register |= 0x20;
      else                              ODE_Error_Register &= 0xDF;
      //manufacturer specific error
      if(ERROR_REGISTER_BIT7_CONDITION) ODE_Error_Register |= 0x80;
      else                              ODE_Error_Register &= 0x7F;

      //send emergency message
      if(CO_NMToperatingState==NMT_PRE_OPERATIONAL || CO_NMToperatingState==NMT_OPERATIONAL){
         //is new emergency, buffer free and no inhibit?
         if(ErrorControl.EmergencyToSend){
            #if CO_NO_EMERGENCY > 0
               static unsigned int tInhibitEmergency = 0;
               if(!CO_TXCAN[CO_TXCAN_EMERG].NewMsg && (TMR16(tInhibitEmergency) > (ODE_Inhibit_Time_Emergency/10))){
				  CO_TXCAN[CO_TXCAN_EMERG].Data.BYTE[0] = (ErrorControl.EmergencyErrorCode) & 0xFF;
				  CO_TXCAN[CO_TXCAN_EMERG].Data.BYTE[1] = ((ErrorControl.EmergencyErrorCode)>>8) & 0xFF;
                  CO_TXCAN[CO_TXCAN_EMERG].Data.BYTE[2] = ODE_Error_Register;
                  CO_TXCAN[CO_TXCAN_EMERG].Data.BYTE[3] = ErrorControl.ErrorBit;
				  CO_TXCAN[CO_TXCAN_EMERG].Data.BYTE[4] = (ErrorControl.CodeVal) & 0xFF;
				  CO_TXCAN[CO_TXCAN_EMERG].Data.BYTE[5] = ((ErrorControl.CodeVal)>>8) & 0xFF;
                  CO_TXCAN[CO_TXCAN_EMERG].Data.BYTE[6] = 0;
                  CO_TXCAN[CO_TXCAN_EMERG].Data.BYTE[7] = 0;
                  CO_TXCANsend(CO_TXCAN_EMERG);
				  ErrorControl.CheckErrors = 0;
				  ErrorControl.EmergencyToSend = 0;//2007-7-9
               }
			   //if(!ErrorControl.CheckErrors)
               TMR16Z(tInhibitEmergency);
            #endif
            //ErrorControl.EmergencyToSend = 0;//2007-7-9
            //write to history
            #if CO_NO_ERROR_FIELD > 0	//���´�������
               if(ODE_Pre_Defined_Error_Field_NoOfErrors < CO_NO_ERROR_FIELD)
                  ODE_Pre_Defined_Error_Field_NoOfErrors++;
               for(i=ODE_Pre_Defined_Error_Field_NoOfErrors-1; i>0; i--)
                  ODE_Pre_Defined_Error_Field[i] = ODE_Pre_Defined_Error_Field[i-1];
               ODE_Pre_Defined_Error_Field[0] = ErrorControl.EmergencyErrorCode | (((unsigned long)ErrorControl.CodeVal)<<16);
            #endif
         }
      }
   }

   //in case of error enter pre-operational state
   if(ODE_Error_Register && (CO_NMToperatingState == NMT_OPERATIONAL)){
      if(ODE_Error_Register&0xEF){  //all, except communication error
         //test sync 2007-6-25
         //CO_NMToperatingState = NMT_PRE_OPERATIONAL;
      }
      if(ODE_Error_Register&0x10){  //communication error
         switch(ODE_Error_Behavior.Communication_Error){
            case 0x01:
               break;
            case 0x02:
               CO_NMToperatingState = NMT_STOPPED;
               break;
            default:
               CO_NMToperatingState = NMT_PRE_OPERATIONAL;
         }
      }
   }
///*
#if CO_NO_SDO_SERVER > 0
//SDO SERVER
   //SDO server makes Object Dictionary of this node available to SDO client (Master on CAN bus)
   //SDO download means, that SDO client wants to WRITE to Object Dictionary of this node
   //SDO upload means, that SDO client wants to READ from Object Dictionary of this node

   if(CO_NMToperatingState==NMT_PRE_OPERATIONAL || CO_NMToperatingState==NMT_OPERATIONAL){
      #if CO_NO_SDO_SERVER == 1
         #define pODE      CO_SDOserverVar[0].pODE
         #define STATE     CO_SDOserverVar[0].State
         #define BUFF      CO_SDOserverVar[0].Buff
         #define BUFFSIZE  CO_SDOserverVar[0].BuffSize
         #define tTIMEOUT  CO_SDOserverVar[0].tTimeout
         #define RXC_INDEX CO_RXCAN_SDO_SRV
         #define TXC_INDEX CO_TXCAN_SDO_SRV
      #else
         #define pODE      CO_SDOserverVar[SDOserverChannel].pODE
         #define STATE     CO_SDOserverVar[SDOserverChannel].State
         #define BUFF      CO_SDOserverVar[SDOserverChannel].Buff
         #define BUFFSIZE  CO_SDOserverVar[SDOserverChannel].BuffSize
         #define tTIMEOUT  CO_SDOserverVar[SDOserverChannel].tTimeout
         #define RXC_INDEX (CO_RXCAN_SDO_SRV + SDOserverChannel)
         #define TXC_INDEX (CO_TXCAN_SDO_SRV + SDOserverChannel)
         i = SDOserverChannel;
         do{
            if(++SDOserverChannel == CO_NO_SDO_SERVER) SDOserverChannel = 0;
            if(CO_RXCAN[RXC_INDEX].NewMsg) break;
         }while(SDOserverChannel != i);
      #endif

      //Abort macro:

      #define SDO_ABORT(Code){                      \
         CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x80;   \
		 CO_TXCAN[TXC_INDEX].Data.BYTE[4] = Code & 0xFF;  \
		 CO_TXCAN[TXC_INDEX].Data.BYTE[5] = ((Code>>8) & 0xFF );  \
		 CO_TXCAN[TXC_INDEX].Data.BYTE[6] = ((Code>>16) & 0xFF );  \
		 CO_TXCAN[TXC_INDEX].Data.BYTE[7] = ((Code>>24) & 0xFF );  \
         STATE = 0;                                 \
         CO_TXCANsend(TXC_INDEX);                   \
      }



      if(CO_RXCAN[RXC_INDEX].NewMsg && !CO_TXCAN[TXC_INDEX].NewMsg){  //New SDO object has to be processed and SDO CAN send buffer is free
         //setup variables for default response
         CO_TXCAN[TXC_INDEX].Data.BYTE[1] = CO_RXCAN[RXC_INDEX].Data.BYTE[1];
         CO_TXCAN[TXC_INDEX].Data.BYTE[2] = CO_RXCAN[RXC_INDEX].Data.BYTE[2];
         CO_TXCAN[TXC_INDEX].Data.BYTE[3] = CO_RXCAN[RXC_INDEX].Data.BYTE[3];
		 //CO_TXCAN[TXC_INDEX].Data.DWORD[1] = 0;
         CO_TXCAN[TXC_INDEX].Data.BYTE[4] = 0;
		 CO_TXCAN[TXC_INDEX].Data.BYTE[5] = 0;
		 CO_TXCAN[TXC_INDEX].Data.BYTE[6] = 0;
		 CO_TXCAN[TXC_INDEX].Data.BYTE[7] = 0;
/**********************************************************************/
//SDOͨѶЭ�鴦�����
//���������� ��Initiate Domain Download��	001
//��ֶ����أ�Download Domain Segment�� 	000
//�������ϴ� ��Initiate Domain Upload��		010
//��ֶ��ϴ� ��Upload Domain Segment�� 		011
//������ֹ��Abort Domain Transfer����		100
/**********************************************************************/
         switch(CO_RXCAN[RXC_INDEX].Data.BYTE[0]>>5){  //Switch Client Command Specifier
            case 1:   //Initiate SDO Download request
               //find pointer to object dictionary entry
			   index = (CO_RXCAN[RXC_INDEX].Data.BYTE[1] & 0xFF);
			   index |= (((CO_RXCAN[RXC_INDEX].Data.BYTE[2]) & 0xFF)<<8);
			   subindex = CO_RXCAN[RXC_INDEX].Data.BYTE[3];
			   pODE = CO_FindEntryInOD(index, subindex);  
             
               //pODE = CO_FindEntryInOD(*((unsigned int*)&CO_RXCAN[RXC_INDEX].Data.BYTE[1]), CO_RXCAN[RXC_INDEX].Data.BYTE[3]);
               if(!pODE){
                  SDO_ABORT(0x06020000L)  	//object does not exist in OD
                  break;					//�����ֵ��ж��󲻴���
               }
               if(pODE->length > CO_MAX_OD_ENTRY_SIZE){  //length of ODE is not valid
                  SDO_ABORT(0x06040047L)   	//general internal incompatibility in the device
                  break;					//һ�����豸�ڲ������
               }
               if((pODE->attribute&0x07) == ATTR_RO || (pODE->attribute&0x07) == ATTR_CO){
                  SDO_ABORT(0x06010002L)  	//attempt to write a read-only object
                  break;					//��ͼдֻ������
               }
               if(CO_RXCAN[RXC_INDEX].Data.BYTE[0] & 0x02){//0 = �������ͣ�1 = ���ٴ���
                  //Expedited transfer		0 = ���ݳ���δָ����1 = ���ݳ���ָ��
                  if(CO_RXCAN[RXC_INDEX].Data.BYTE[0] & 0x01)//is size indicated
				  {
                     i = 4 - ((CO_RXCAN[RXC_INDEX].Data.BYTE[0]>>2)&0x03);   //size
					 if(i==1) i=2;			//sizeof DSPֻ�ܼ������ĸ��?
				  }
                  else 
                  {
                     i = 4;
				  }
                  //write to memory
                  longtmp = CO_OD_Write(pODE, (void*)&CO_RXCAN[RXC_INDEX].Data.BYTE[4], i);
				  CO_TXCAN[TXC_INDEX].Data.BYTE[4] = longtmp & 0xFF;
				  CO_TXCAN[TXC_INDEX].Data.BYTE[5] = (longtmp>>8) & 0xFF;
				  CO_TXCAN[TXC_INDEX].Data.BYTE[6] = (longtmp>>16) & 0xFF;
				  CO_TXCAN[TXC_INDEX].Data.BYTE[7] = (longtmp>>24) & 0xFF;
                  if(longtmp != 0)//SDO_ABORT
				  {
                     CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x80;
				  }
                  else
				  {
                     CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x60;
				  }
                  STATE = 0;
                  CO_TXCANsend(TXC_INDEX);
               }else{
#if CO_MAX_OD_ENTRY_SIZE == 0x04
                  SDO_ABORT(0x06010000L)  	//unsupported access to an object
               }							//����֧�ַ���
               break;
#else
                  //segmented transfer
               if(CO_RXCAN[RXC_INDEX].Data.BYTE[0] & 0x01)
                  {//is size indicated
                     //if(*((unsigned long*)&CO_RXCAN[RXC_INDEX].Data.BYTE[4]) != (unsigned long)pODE->length){
					 longtmp = ((unsigned long)CO_RXCAN[RXC_INDEX].Data.BYTE[7]<<24);
					 longtmp += ((unsigned long)CO_RXCAN[RXC_INDEX].Data.BYTE[6]<<16);
					 longtmp += ((unsigned long)CO_RXCAN[RXC_INDEX].Data.BYTE[5]<<8);
					 longtmp += CO_RXCAN[RXC_INDEX].Data.BYTE[4];
					 if(longtmp != (unsigned long)pODE->length){
                        SDO_ABORT(0x06070010L)   //Length of service parameter does not match
                        break;
                     }
                  BUFFSIZE = 0;
                  TMR8Z(tTIMEOUT);
                  STATE = 0x02;
                  CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x60;
                  CO_TXCANsend(TXC_INDEX);
                  }
               }
               break;

            case 0:   //Download SDO segment
               if(!(STATE&0x02)){//download SDO segment was not initiated
                  SDO_ABORT(0x05040001L) //command specifier not valid
                  break;
               }
               //verify toggle bit
               if((CO_RXCAN[RXC_INDEX].Data.BYTE[0]&0x10) != (STATE&0x10)){
                  SDO_ABORT(0x05030000L) //toggle bit not alternated
                  break;
               }
               //get size
			   tmp = CO_RXCAN[RXC_INDEX].Data.BYTE[0];
               i = 7 - ((tmp>>1)&0x07);   //size
               //verify length
               if((BUFFSIZE + i) > pODE->length){
                  SDO_ABORT(0x06070012L)   //Length of service parameter too high
                  break;
               }
               //copy data to buffer
               memcpy1((void*)&BUFF[BUFFSIZE], (void*)&CO_RXCAN[RXC_INDEX].Data.BYTE[1], i);
               //memcpy1(&BUFF[BUFFSIZE], &CO_RXCAN[RXC_INDEX].Data.BYTE[1], i);
               BUFFSIZE += i;
               //write response data (partial)
               CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x20 | (STATE&0x10);
               //If no more segments to be downloaded, copy data to variable
               if(CO_RXCAN[RXC_INDEX].Data.BYTE[0] & 0x01){
                  longtmp = CO_OD_Write(pODE, (void*)&BUFF[0], BUFFSIZE);
				  CO_TXCAN[TXC_INDEX].Data.BYTE[4] = longtmp & 0xFF;
				  CO_TXCAN[TXC_INDEX].Data.BYTE[5] = (longtmp>>8) & 0xFF;
				  CO_TXCAN[TXC_INDEX].Data.BYTE[6] = (longtmp>>16) & 0xFF;
				  CO_TXCAN[TXC_INDEX].Data.BYTE[7] = (longtmp>>24) & 0xFF;
                  if(longtmp){//send SDO_ABORT
                     CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x80;
                     STATE = 0;
                     CO_TXCANsend(TXC_INDEX);
                     break;
                  }
                  STATE = 0;
               }
               else{
                  //reset timeout timer, alternate toggle bit
                  TMR8Z(tTIMEOUT);
                  if(STATE&0x10) STATE &= 0xEF;
                  else STATE |= 0x10;
               }
               //write other response data
               CO_TXCAN[TXC_INDEX].Data.BYTE[1] = 0;
               CO_TXCAN[TXC_INDEX].Data.BYTE[2] = 0;
               CO_TXCAN[TXC_INDEX].Data.BYTE[3] = 0;
               //download segment response
               CO_TXCANsend(TXC_INDEX);
               break;
#endif
            case 2:   //Initiate SDO Upload request
               //find pointer to object dictionary entry
               index = (CO_RXCAN[RXC_INDEX].Data.BYTE[1] & 0xFF);
			   index |= (((CO_RXCAN[RXC_INDEX].Data.BYTE[2]) & 0xFF)<<8);
			   subindex = CO_RXCAN[RXC_INDEX].Data.BYTE[3] & 0xFF;
			   pODE = CO_FindEntryInOD(index, subindex);  
            
               //pODE = CO_FindEntryInOD(*((unsigned int*)&CO_RXCAN[RXC_INDEX].Data.BYTE[1]), CO_RXCAN[RXC_INDEX].Data.BYTE[3]);
               if(!pODE){
                  SDO_ABORT(0x06020000L)  //object does not exist in OD
                  break;
               }
               else if(pODE->length > CO_MAX_OD_ENTRY_SIZE){  //length of ODE is not valid
                  //index=pODE->length;
                  SDO_ABORT(0x06040047L)   //general internal incompatibility in the device
                  break;
               }
               if((pODE->attribute&0x07) == ATTR_WO){
                  SDO_ABORT(0x06010001L)   //attempt to read a write-only object
                  break;
               }
               if(pODE->length <= 4){
                  unsigned long ret;
                  //expedited transfer
                  //ret = CO_OD_Read(pODE, (void*)&CO_TXCAN[TXC_INDEX].Data.DWORD[1], 4);
                  ret = CO_OD_Read(pODE, (void*)&CO_TXCAN[TXC_INDEX].Data.BYTE[4], 4);
                  if(ret) SDO_ABORT(ret)
                  else{
                     //if(pODE->attribute & ATTR_ADD_ID) CO_TXCAN[TXC_INDEX].Data.DWORD[1] += CO_NodeID;
                     if(pODE->attribute & ATTR_ADD_ID)
                     {
                     	CO_TXCAN[TXC_INDEX].Data.BYTE[4] += CO_NodeID;
					 }

					 if(Find_char_OD(pODE->index,pODE->subindex,pODE->length))
					 {
                     	CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x43 | ((4-(pODE->length>>1)) << 2);
					 }
					 else
					 {
                     	CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x43 | ((4-pODE->length) << 2);
					 }

                     STATE = 0;
                     CO_TXCANsend(TXC_INDEX);
                  }
               }
#if CO_MAX_OD_ENTRY_SIZE == 0x04
               break;
#else
               else{
                  unsigned long ret;
                  //segmented transfer
                  ret = CO_OD_Read(pODE, (void*)&BUFF[0], CO_MAX_OD_ENTRY_SIZE);
                  if(ret) SDO_ABORT(ret)
                  else{
                     BUFFSIZE = 0;  //indicates pointer to next data to be send
                     TMR8Z(tTIMEOUT);
                     STATE = 0x04;
                     CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x41;
                     //CO_TXCAN[TXC_INDEX].Data.DWORD[1] = (unsigned long)pODE->length;
                     CO_TXCAN[TXC_INDEX].Data.BYTE[4] = (pODE->length & 0xFF);
					 CO_TXCAN[TXC_INDEX].Data.BYTE[5] = (((pODE->length)>>8) & 0xFF);
					 CO_TXCAN[TXC_INDEX].Data.BYTE[6] = 0;
					 CO_TXCAN[TXC_INDEX].Data.BYTE[7] = 0;
                     CO_TXCANsend(TXC_INDEX);
                  }
               }
               break;

            case 3:   //Upload SDO segment
               if(!(STATE&0x04)){//upload SDO segment was not initiated
                  SDO_ABORT(0x05040001L) //command specifier not valid
                  break;
               }
               //verify toggle bit
               if((CO_RXCAN[RXC_INDEX].Data.BYTE[0]&0x10) != (STATE&0x10)){
                  SDO_ABORT(0x05030000L) //toggle bit not alternated
                  break;
               }
               //calculate length to be sent
               j = pODE->length - BUFFSIZE;
               if(j > 7) j = 7;
               //fill data bytes
               for(i=0; i<j; i++)
                  CO_TXCAN[TXC_INDEX].Data.BYTE[i+1] = BUFF[BUFFSIZE+i];
               for(; i<7; i++)
                  CO_TXCAN[TXC_INDEX].Data.BYTE[i+1] = 0;
               BUFFSIZE += j;
               CO_TXCAN[TXC_INDEX].Data.BYTE[0] = 0x00 | (STATE&0x10) | ((7-j)<<1);
               //is end of transfer?
               if(BUFFSIZE==pODE->length){
                  CO_TXCAN[TXC_INDEX].Data.BYTE[0] |= 1;
                  STATE = 0;
               }
               else{
                  //reset timeout timer, alternate toggle bit
                  TMR8Z(tTIMEOUT);
                  if(STATE&0x10) STATE &= 0xEF;
                  else STATE |= 0x10;
               }
               CO_TXCANsend(TXC_INDEX);
               break;
#endif
            case 4:   //Abort SDO transfer by client
#if CO_MAX_OD_ENTRY_SIZE > 0x04
               STATE = 0;
#endif
               break;

            default:
               SDO_ABORT(0x05040001L) //command specifier not valid
         }//end switch
         CO_RXCAN[RXC_INDEX].NewMsg = 0;//release
      }//end process new SDO object

   //verify timeout of segmented transfer
#if CO_MAX_OD_ENTRY_SIZE > 0x04
      if(STATE){ //Segmented SDO transfer in progress
		 //n*100ms-tTIMEOUT
         if(TMR8(tTIMEOUT) >= CO_SDO_TIMEOUT_TIME){//2S
            SDO_ABORT(0x05040000L)  //SDO protocol timed out
         }
      }
#endif
   }
   else{ //not in (pre)operational state
      STATE = 0;
      CO_RXCAN[RXC_INDEX].NewMsg = 0;//release buffer
   }

   #undef pODE
   #undef STATE
   #undef BUFF
   #undef BUFFSIZE
   #undef tTIMEOUT
   #undef RXC_INDEX
   #undef TXC_INDEX

#endif //end of SDO server
//*/

//Heartbeat consumer message handling
   //DS 301: "Monitoring starts after the reception of the first HeartBeat. (Not bootup)"
   CO_HBcons_AllMonitoredOperationalCpy = 5;
#if CO_NO_CONS_HEARTBEAT > 0
   if(CO_NMToperatingState==NMT_PRE_OPERATIONAL || CO_NMToperatingState==NMT_OPERATIONAL){
      i = 0;
      #if CO_NO_CONS_HEARTBEAT > 1
      for(i=0; i<CO_NO_CONS_HEARTBEAT; i++){
	  #endif
         if((unsigned int)ODE_Consumer_Heartbeat_Time[i])//is node monitored
		 {
            //Verify if new Consumer Heartbeat message received
            if(CO_RXCAN[CO_RXCAN_CONS_HB+i].NewMsg)
            {
               CO_HBcons_NMTstate(i) = CO_RXCAN[CO_RXCAN_CONS_HB+i].Data.BYTE[0];
               if(CO_HBcons_NMTstate(i))//must not be a bootup message
			   {
                  //TMR16Z(CO_HBcons_TimerValue_L(i));  //reset timer
				  CO_HBcons_TimerValue_L(i) = (CO_Timer16bit1msCopy) & 0xFF;
				  CO_HBcons_TimerValue_H(i) = (CO_Timer16bit1msCopy>>8) & 0xFF;
			   }
               CO_RXCAN[CO_RXCAN_CONS_HB+i].NewMsg = 0;
			   CO_HBcons_error_bit &= (0xFFFF ^ (1<<i));
            }
            //Verify timeout
            else
            {
               if(CO_HBcons_MonStarted(i))//Monitoring starts after the reception of the first heartbeat (not Bootup)
			   {
                  tmp = (CO_HBcons_TimerValue_H(i)<<8) + CO_HBcons_TimerValue_L(i);
                  if(TMR16(tmp) > (unsigned int)ODE_Consumer_Heartbeat_Time[i])
                  {
                     ErrorReport(ERROR_HEARTBEAT_CONSUMER, 0);
                     CO_HBcons_NMTstate(i) = 0;
					 CO_HBcons_error_bit |= (1<<i);
                  }
               }
               else //monitoring did not yet sterted
			   {
                  CO_HBcons_error_bit &= (0xFFFF ^ (1<<i));
                  if(CO_HBcons_NMTstate(i)) CO_HBcons_MonStarted(i) = 1;
               }
            }
            if(CO_HBcons_NMTstate(i) != NMT_OPERATIONAL)
               CO_HBcons_AllMonitoredOperationalCpy = 0;
         }
		 else
		 {
			CO_HBcons_error_bit &= (0xFFFF ^ (1<<i));
		 }
      #if CO_NO_CONS_HEARTBEAT > 1
      }
	  #endif
   }
   else{ //not in (pre)operational state
      for(i=0; i<CO_NO_CONS_HEARTBEAT; i++){
         CO_HBcons_NMTstate(i) = 0;
         CO_RXCAN[CO_RXCAN_CONS_HB+i].NewMsg = 0;
         CO_HBcons_MonStarted(i) = 0;
      }
      CO_HBcons_AllMonitoredOperationalCpy = 0;
   }
#endif
   CO_HBcons_AllMonitoredOperational = CO_HBcons_AllMonitoredOperationalCpy;


//Heartbeat producer message & Bootup message
   //Sent only if not in TX passive, bootup send always
   	if((ODE_Producer_Heartbeat_Time && (TMR16(tProducerHeartbeatTime) >= ODE_Producer_Heartbeat_Time))
     ||(CO_NMToperatingState == NMT_INITIALIZING))
	{
      TMR16Z(tProducerHeartbeatTime);
      CO_TXCAN[CO_TXCAN_HB].Data.BYTE[0] = CO_NMToperatingState;
      CO_TXCANsend(CO_TXCAN_HB);
      if(CO_NMToperatingState == NMT_INITIALIZING)
      {
         //CO_NMToperatingState = NMT_PRE_OPERATIONAL;
         if((ODE_NMT_Startup & 0x04) == 0) CO_NMToperatingState = NMT_OPERATIONAL;//only bit2 implemented, if bit2=1, Node will NOT start operational
         else                              CO_NMToperatingState = NMT_PRE_OPERATIONAL;
      }
   }

}
//-----------------------------------------------------------------------------
//	�ı�CANͨѶ���ã������ʣ����
//-----------------------------------------------------------------------------
void Change_CAN_MOD(void)
{	
	unsigned long temp;

	temp = (Pn[CANAdderADD]<<8) + (Pn[CANParasADD] & 0x000F);;

	if(temp != CANParasSET)
	{			//���øı���
		CANParasSET = temp;
		ODE_CANbitRate = Pn[CANParasADD] & 0x000F;	//CANͨѶ����
		ODE_CANnodeID = Pn[CANAdderADD];			//CANͨѶ�ڵ�
		//CO_ResetComm();							//����CAN�Ĵ���
		CO_RXCAN[CO_RXCAN_NMT].NewMsg = 1;
		CO_RXCAN[CO_RXCAN_NMT].Data.BYTE[1] = 0;
		CO_RXCAN[CO_RXCAN_NMT].Data.BYTE[0] = 130;
	}
}
//-----------------------------------------------------------------------------
//	dynamic PDO
//	PDOӳ��ı����һ��ʱ���ڣ�PDOӳ����Ч
//-----------------------------------------------------------------------------
void Dynamic_PDO_Maping(void)
{volatile struct MBOX * pMBOX;
	unsigned int i,j,k,index,subindex;
	//	START MAP:	���MAP					ODE_RPDO_Mapping_NoOfEntries[0] == 0
	//	MAPing1:	����дPDO				ODE_RPDO_Mapping_NoOfEntries[0] == 0 && RPDO1_MAPPing_Flag == 1
	//	MAPing2:	���дPDO,���ó�ʼ��	ODE_RPDO_Mapping_NoOfEntries[0] != 0 && RPDO1_MAPPing_Flag == 1
	//	MAPend:		���MAP��ʼ��,����PDO	ODE_RPDO_Mapping_NoOfEntries[0] != 0 && RPDO1_MAPPing_Flag == 0
	//RPDO1
	#if CO_NO_RPDO > 0
	if(ODE_RPDO_Mapping_NoOfEntries[0] == 0)
	{// START MAP
		RPDO_MAPPing_Flag[0] = 1;
	}
	#endif
	#if CO_NO_RPDO > 1
	if(ODE_RPDO_Mapping_NoOfEntries[1] == 0)
	{// START MAP
		RPDO_MAPPing_Flag[1] = 1;
	}
	#endif
	#if CO_NO_RPDO > 2
	if(ODE_RPDO_Mapping_NoOfEntries[2] == 0)
	{// START MAP
		RPDO_MAPPing_Flag[2] = 1;
	}
	#endif
	#if CO_NO_RPDO > 3
	if(ODE_RPDO_Mapping_NoOfEntries[3] == 0)
	{// START MAP
		RPDO_MAPPing_Flag[3] = 1;
	}
	#endif

	for(i = 0; i < CO_NO_RPDO; i++)
	{
		if(RPDO_MAPPing_Flag[i] && ODE_RPDO_Mapping_NoOfEntries[i])
		{
			// MAPing ��ʼ��RPDO����	
			unsigned char length;
	        #ifdef CO_PDO_MAPPING_IN_OD	//Enable map
	        //unsigned char j;
	        //calculate length from mapping
	        length = 7;  //round up to use whole byte
	        for(j=0; j<ODE_RPDO_Mapping_NoOfEntries[i]; j++)
			{
	        	length += (ODE_RPDO_Mapping[i][j] & 0xFF);
			}
	        length >>= 3;
	        if(length > 8)
	        {
	        	length = 8;
	        	ErrorReport(ERROR_wrong_PDO_mapping, i);
	        }
	        #else
	        length = 9; //size of RPDO is not important
	        #endif
	        if(length)//if((ODE_RPDO_Parameter[i].COB_ID & 0x80000000L) == 0 && length)//is RPDO used
			{
	        	CO_RXCAN[CO_RXCAN_RPDO+i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_RPDO_Parameter[i].COB_ID, 0);
	            CO_RXCAN[CO_RXCAN_RPDO+i].NoOfBytes = length;
	            switch(i)
	            {
	              	case 0:  //First RPDO: is used standard predefined COB-ID?
	                 	#if ODD_RPDO_PAR_COB_ID_0 == 0
	                    CO_RXCAN[CO_RXCAN_RPDO+i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
	                 	#endif
	                 break;
	              	case 1:  //Second RPDO: is used standard predefined COB-ID?
	                 	#if ODD_RPDO_PAR_COB_ID_1 == 0
	                 	CO_RXCAN[CO_RXCAN_RPDO+i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
	                 	#endif
	                 break;
	              	case 2:  //Third RPDO: is used standard predefined COB-ID?
	                 	#if ODD_RPDO_PAR_COB_ID_2 == 0
	                    CO_RXCAN[CO_RXCAN_RPDO+i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
	                 	#endif
	                 break;
	              	case 3:  //Fourth RPDO: is used standard predefined COB-ID?
	                 	#if ODD_RPDO_PAR_COB_ID_3 == 0
	                    CO_RXCAN[CO_RXCAN_RPDO+i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
	                 	#endif
	                 	break;
	           	}
	        }
			for(k = 0; k < ODE_RPDO_Mapping_NoOfEntries[i]; k++)
			{
				//unsigned int index,subindex;
				ROM CO_objectDictionaryEntry  * pODE1;
				index = (ODE_RPDO_Mapping[i][k]>>16) & 0xFFFF;
				subindex = (ODE_RPDO_Mapping[i][k]>>8) & 0xFF;
				pODE1 = CO_FindEntryInOD(index, subindex);
				CO_Find_RPDOmapData(pODE1,i,k);
         	}
		 	RPDO_MAPPing_Flag[i] = 0;
		}
	}
//********************************************************************//
//
//********************************************************************//
	// TPDO1
	#if CO_NO_TPDO > 0
	if(ODE_TPDO_Mapping_NoOfEntries[0] == 0)
	{// START MAP
		TPDO_MAPPing_Flag[0] = 1;
	}
	#endif
	#if CO_NO_TPDO > 0
	if(ODE_TPDO_Mapping_NoOfEntries[1] == 0)
	{// START MAP
		TPDO_MAPPing_Flag[1] = 1;
	}
	#endif
	#if CO_NO_TPDO > 0
	if(ODE_TPDO_Mapping_NoOfEntries[2] == 0)
	{// START MAP
		TPDO_MAPPing_Flag[2] = 1;
	}
	#endif
	#if CO_NO_TPDO > 0
	if(ODE_TPDO_Mapping_NoOfEntries[3] == 0)
	{// START MAP
		TPDO_MAPPing_Flag[3] = 1;
	}
	#endif


	for(i = 0; i < CO_NO_TPDO; i++)
	{
		if(TPDO_MAPPing_Flag[i] && ODE_TPDO_Mapping_NoOfEntries[i])
		{// MAPing2 ��ʼ��TPDO����	
			unsigned char length;
			#ifdef CO_PDO_MAPPING_IN_OD
            //unsigned char j;
            //calculate length from mapping
            length = 7;  //round up to use whole byte
            for(j=0; j<ODE_TPDO_Mapping_NoOfEntries[0]; j++)
               length += (ODE_TPDO_Mapping[i][j] & 0xFF);
            length >>= 3;
            if(length > 8){
               length = 8;
               ErrorReport(ERROR_wrong_PDO_mapping, i);
            }
         	#else
            length = 8;    //8 bytes long TPDO will be transmitted
         	#endif
			pMBOX = & ECanbMboxes.MBOX0;
			if(length)//is TPDO used
			{
            	CO_TXCAN[CO_TXCAN_TPDO+i].Ident.BYTE[0] = CO_IDENT_WRITE((unsigned int)ODE_TPDO_Parameter[i].COB_ID, 0);
            	CO_TXCAN[CO_TXCAN_TPDO+i].NoOfBytes = length;
            	CO_TXCAN[CO_TXCAN_TPDO+i].Inhibit = 0;
				pMBOX += (CO_TXCAN_TPDO+i);
				if(length > 8) 
		   		{
					length = 8;
		   		}
				ECanbShadow.CANME.all = ECanbRegs.CANME.all;	
   				ECanbShadow.CANME.all &= (0xFFFFFFFF ^ ((Uint32)1<<i));	// MBOX disable
   				ECanbRegs.CANME.all = ECanbShadow.CANME.all;
				asm("	SETC INTM");
				pMBOX->MSGCTRL.bit.DLC = length;
				pMBOX->MSGCTRL.bit.DLC = length;
				asm("	CLRC INTM");
				ECanbShadow.CANME.all = ECanbRegs.CANME.all;	
   				ECanbShadow.CANME.all |= ((Uint32)1<<i);	// MBOX enable
   				ECanbRegs.CANME.all = ECanbShadow.CANME.all;

            	#if CO_NO_SYNC > 0
               	if(ODE_TPDO_Parameter[i].Transmission_type < 240)
                  	CO_TXCAN[CO_TXCAN_TPDO+i].Inhibit = 1; //mark Synchronous TPDOs
            	#endif
            	switch(i)
            	{
               		case 0:  //First TPDO: is used standard predefined COB-ID?
                  		#if ODD_TPDO_PAR_COB_ID_0 == 0
                    	CO_TXCAN[CO_TXCAN_TPDO+i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
                  		#endif
                  		break;
               		case 1:  //Second TPDO: is used standard predefined COB-ID?
                  		#if ODD_TPDO_PAR_COB_ID_1 == 0
                     	CO_TXCAN[CO_TXCAN_TPDO+i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
                  		#endif
                  		break;
               		case 2:  //Third TPDO: is used standard predefined COB-ID?
                  		#if ODD_TPDO_PAR_COB_ID_2 == 0
                     	CO_TXCAN[CO_TXCAN_TPDO+i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
                  		#endif
                  		break;
               		case 3:  //Fourth TPDO: is used standard predefined COB-ID?
                  		#if ODD_TPDO_PAR_COB_ID_3 == 0
                     	CO_TXCAN[CO_TXCAN_TPDO+i].Ident.BYTE[0] += CO_IDENT_WRITE((unsigned int)CO_NodeID, 0);
                  		#endif
                  		break;
            	}
         	}
			//���ֵ��е�����
			for(k = 0; k < ODE_TPDO_Mapping_NoOfEntries[i]; k++)
			{
				//unsigned int index,subindex;
				ROM CO_objectDictionaryEntry  * pODE1;
				index = (ODE_TPDO_Mapping[i][k]>>16) & 0xFFFF;
				subindex = (ODE_TPDO_Mapping[i][k]>>8) & 0xFF;
				pODE1 = CO_FindEntryInOD(index, subindex); 
				CO_Find_TPDOmapData(pODE1,i,k);		
         	}
			TPDO_MAPPing_Flag[i] = 0;
		}
	}
}
//****************************************************************//
//	�����ҵ��ĵ�ַָ�룬������������
//****************************************************************//
void CO_Find_RPDOmapData(ROM CO_objectDictionaryEntry* pODE,int RPDOindex,int i)
{Uint16 tmp;
	tmp = Find_char_OD(pODE->index,pODE->subindex,pODE->length);
	CO_RPDO_MAPing[RPDOindex][i].attribute=pODE->attribute;
	CO_RPDO_MAPing[RPDOindex][i].index=pODE->index;
	CO_RPDO_MAPing[RPDOindex][i].length=pODE->length;
	CO_RPDO_MAPing[RPDOindex][i].pData=pODE->pData;
	CO_RPDO_MAPing[RPDOindex][i].subindex=pODE->subindex;
	if(tmp)
	{
		CO_RPDO_MAPing[RPDOindex][i].length >>= 1;
	}
}

void CO_Find_TPDOmapData(ROM CO_objectDictionaryEntry* pODE,int TPDOindex,int i)
{Uint16 tmp;
	tmp = Find_char_OD(pODE->index,pODE->subindex,pODE->length);
	CO_TPDO_MAPing[TPDOindex][i].attribute=pODE->attribute;
	CO_TPDO_MAPing[TPDOindex][i].index=pODE->index;
	CO_TPDO_MAPing[TPDOindex][i].length=pODE->length;
	CO_TPDO_MAPing[TPDOindex][i].pData=pODE->pData;
	CO_TPDO_MAPing[TPDOindex][i].subindex=pODE->subindex;
	if(tmp)
	{
		CO_TPDO_MAPing[TPDOindex][i].length >>= 1;
	}
}

//=============================================================================
// TPDO���ͣʽ�仯
//=============================================================================

void Transmit_Type_Change(void)
{//���ı�ͬ��ͨѶ����Ҫʵʱ�ı�
	int i;
	#if CO_NO_SYNC > 0
	for(i=CO_TXCAN_TPDO; i<CO_TXCAN_TPDO+CO_NO_TPDO; i++)
	{		
		if(ODE_TPDO_Mapping_NoOfEntries[i-CO_TXCAN_TPDO])
		{
			if((ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].Transmission_type-1) <= 239)
				CO_TXCAN[i].Inhibit = 1; //mark Synchronous TPDOs
			else CO_TXCAN[i].Inhibit = 0; //mark Synchronous TPDOs 
				CO_TPDO_SyncTimer[i-CO_TXCAN_TPDO] = ODE_TPDO_Parameter[i-CO_TXCAN_TPDO].Transmission_type;       
		}
	}
	#endif
}

//=============================================================================
// End of file.
//=============================================================================
