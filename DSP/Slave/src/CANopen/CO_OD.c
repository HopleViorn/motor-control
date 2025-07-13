
/*******************************************************************************

   CO_OD.c - Variables and Object Dictionary for CANopenNode

   Copyright (C) 2004  yzhua

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


   Author: yzhua

********************************************************************************

   For short description of standard Object Dictionary entries see CO_OD.txt

*******************************************************************************/

#include "CANopen.h"

#define	POSITION_LIMIT		0x7FFFFFFFL	//3000rpm,5min
#define	SOFT_POSITION_LIMIT	0x7FFFFFFFL	//3000rpm,5min

#define FOLLOWING_ERROR_WINDOW	 0x7530 //30000 pulse
#define FOLLOWING_ERROR_TIME_OUT 0x0C8 //200 ms
//CO_MAX_ENTRY_NR is maximum number of PDO and Heartbeat entries
//defined in this file. If more entries are neeed, they can be added
#define CO_MAX_ENTRY_NR 8

//check defines
#if CO_NO_RPDO > CO_MAX_ENTRY_NR
   #error defineCO_NO_RPDO (CO_NO_RPDO) not correct!
#endif
#if CO_NO_TPDO > CO_MAX_ENTRY_NR
   #error defineCO_NO_TPDO (CO_NO_TPDO) not correct!
#endif
#if CO_NO_SDO_SERVER > 1
   #error defineCO_NO_SDO_SERVER (CO_NO_SDO_SERVER) not correct!
#endif
#if CO_NO_SDO_CLIENT > 1
   #error defineCO_NO_SDO_CLIENT (CO_NO_SDO_CLIENT) not correct!
#endif
#if CO_NO_CONS_HEARTBEAT > CO_MAX_ENTRY_NR
   #error defineCO_NO_CONS_HEARTBEAT (CO_NO_CONS_HEARTBEAT) not correct!
#endif
#if CO_NO_ERROR_FIELD != 0 && CO_NO_ERROR_FIELD != 8
   #error defineCO_NO_ERROR_FIELD (CO_NO_ERROR_FIELD) not correct!
#endif

/******************************************************************************/
/*     VARIABLES      *********************************************************/
/******************************************************************************/
//#ifdef __18CXX
//   #pragma romdata ODE_CO_RomVariables=0x1000 //ROM variables in PIC18fxxx must be above address 0x1000
//#endif

/*0x1000*/ ROM UNSIGNED32     ODE_Device_Type = ODD_DEVICE_TYPE;
/*0x1001*/     VOLATILE UNSIGNED8      ODE_Error_Register = 0;
/*0x1002*/     VOLATILE UNSIGNED32     ODE_Manufacturer_Status_Register = 0L;

           #if CO_NO_ERROR_FIELD > 0
/*0x1003*/     VOLATILE UNSIGNED8      ODE_Pre_Defined_Error_Field_NoOfErrors = 0;
               VOLATILE UNSIGNED32     ODE_Pre_Defined_Error_Field[CO_NO_ERROR_FIELD];
           #endif

           #if CO_NO_SYNC > 0
		   //2007-6-22
/*0x1005*/ //ROM UNSIGNED32     ODE_SYNC_COB_ID = ODD_SYNC_COB_ID;
/*0x1006*/ //ROM UNSIGNED32     ODE_Communication_Cycle_Period = ODD_COMM_CYCLE_PERIOD;
/*0x1007*/ //ROM UNSIGNED32     ODE_Synchronous_Window_Length = ODD_SYNCHR_WINDOW_LEN;
/*0x1005*/ ROM UNSIGNED32     ODE_SYNC_COB_ID = ODD_SYNC_COB_ID;
/*0x1006*/ VOLATILE UNSIGNED32     ODE_Communication_Cycle_Period = ODD_COMM_CYCLE_PERIOD;
		   VOLATILE UNSIGNED32     ODE_Communication_Cycle_Period1 = ODD_COMM_CYCLE_PERIOD;
/*0x1007*/ VOLATILE UNSIGNED32     ODE_Synchronous_Window_Length = ODD_SYNCHR_WINDOW_LEN;

           #endif

/*0x1008*/ ROM char           ODE_Manufacturer_Device_Name[] = ODD_MANUF_DEVICE_NAME;
/*0x1009*/ ROM char           ODE_Manufacturer_Hardware_Version[] = ODD_MANUF_HW_VERSION;
/*0x100A*/ ROM char           ODE_Manufacturer_Software_Version[] = ODD_MANUF_SW_VERSION;

           #if CO_NO_EMERGENCY > 0
/*0x1014*/ ROM UNSIGNED32     ODE_Emergency_COB_ID = 0x00000080L; //NODE-ID is added when used
/*0x1015*/ VOLATILE UNSIGNED16     ODE_Inhibit_Time_Emergency = ODD_INHIBIT_TIME_EMER;
           #endif

           #if CO_NO_CONS_HEARTBEAT > 0
/*0x1016*/ VOLATILE UNSIGNED8      ODE_Consumer_Heartbeat_Time_NoOfEntries = CO_NO_CONS_HEARTBEAT;
           VOLATILE UNSIGNED32     ODE_Consumer_Heartbeat_Time[CO_NO_CONS_HEARTBEAT] = {
                  ODD_CONS_HEARTBEAT_0,
               #if CO_NO_CONS_HEARTBEAT > 1
                  ODD_CONS_HEARTBEAT_1,
               #endif
               #if CO_NO_CONS_HEARTBEAT > 2
                  ODD_CONS_HEARTBEAT_2,
               #endif
               #if CO_NO_CONS_HEARTBEAT > 3
                  ODD_CONS_HEARTBEAT_3,
               #endif
               #if CO_NO_CONS_HEARTBEAT > 4
                  ODD_CONS_HEARTBEAT_4,
               #endif
               #if CO_NO_CONS_HEARTBEAT > 5
                  ODD_CONS_HEARTBEAT_5,
               #endif
               #if CO_NO_CONS_HEARTBEAT > 6
                  ODD_CONS_HEARTBEAT_6,
               #endif
               #if CO_NO_CONS_HEARTBEAT > 7
                  ODD_CONS_HEARTBEAT_7,
               #endif
               };
           #endif

/*0x1017*/ VOLATILE UNSIGNED16     ODE_Producer_Heartbeat_Time = ODD_PROD_HEARTBEAT;
/*0x1018*/ ROM struct{
               UNSIGNED8      NoOfEntries;
               UNSIGNED32     Vendor_ID;
               UNSIGNED32     Product_Code;
               UNSIGNED32     Revision_Number;
               UNSIGNED32     Serial_Number;
               }              ODE_Identity = {
                                 4,
                                 ODD_IDENT_VENDOR_ID,
                                 ODD_IDENT_PROD_CODE,
                                 ODD_IDENT_REVISION_NR,
                                 ODD_IDENT_SERIAL_NR
                              };

/*0x1029*/ ROM ODEs_Error_Behavior  ODE_Error_Behavior = {
                                 1,
                                 ODD_ERROR_BEH_COMM,
                              };

           #if CO_NO_SDO_SERVER > 0
/*0x1200+*/ROM UNSIGNED8      ODE_Server_SDO_Parameter_NoOfEntries = 2;
           ROM CO_sSDO_Param  ODE_Server_SDO_Parameter[CO_NO_SDO_SERVER] = {
                  {CAN_ID_RSDO, CAN_ID_TSDO, 0}
               };
           #endif

           #if CO_NO_SDO_CLIENT > 0
/*0x1280+*/ROM UNSIGNED8      ODE_Client_SDO_Parameter_NoOfEntries = 3;
               CO_sSDO_Param  ODE_Client_SDO_Parameter[CO_NO_SDO_CLIENT] = {
                  {0x80000000L, 0x80000000L, 0}
               };
           #endif

           #if CO_NO_RPDO > 0
		   //2007-6-22
/*0x1400+*///ROM UNSIGNED8      ODE_RPDO_Parameter_NoOfEntries = 2;
           //ROM CO_sRPDO_param ODE_RPDO_Parameter[CO_NO_RPDO] = {
		   VOLATILE UNSIGNED8      ODE_RPDO_Parameter_NoOfEntries = 2;
           VOLATILE CO_sRPDO_param ODE_RPDO_Parameter[CO_NO_RPDO] = {
                  #if ODD_RPDO_PAR_COB_ID_0 == 0	//使用默认COB-ID
                     {CAN_ID_RPDO0, ODD_RPDO_PAR_T_TYPE_0},//{200+NODE ID,255}
                  #else
                     {ODD_RPDO_PAR_COB_ID_0, ODD_RPDO_PAR_T_TYPE_0},
                  #endif
               #if CO_NO_RPDO > 1
                  #if ODD_RPDO_PAR_COB_ID_1 == 0
                     {CAN_ID_RPDO1, ODD_RPDO_PAR_T_TYPE_1},
                  #else
                     {ODD_RPDO_PAR_COB_ID_1, ODD_RPDO_PAR_T_TYPE_1},
                  #endif
               #endif
               #if CO_NO_RPDO > 2
                  #if ODD_RPDO_PAR_COB_ID_2 == 0
                     {CAN_ID_RPDO2, ODD_RPDO_PAR_T_TYPE_2},
                  #else
                     {ODD_RPDO_PAR_COB_ID_2, ODD_RPDO_PAR_T_TYPE_2},
                  #endif
               #endif
               #if CO_NO_RPDO > 3
                  #if ODD_RPDO_PAR_COB_ID_3 == 0
                     {CAN_ID_RPDO3, ODD_RPDO_PAR_T_TYPE_3},
                  #else
                     {ODD_RPDO_PAR_COB_ID_3, ODD_RPDO_PAR_T_TYPE_3},
                  #endif
               #endif
               #if CO_NO_RPDO > 4
                  {ODD_RPDO_PAR_COB_ID_4, ODD_RPDO_PAR_T_TYPE_4},
               #endif
               #if CO_NO_RPDO > 5
                  {ODD_RPDO_PAR_COB_ID_5, ODD_RPDO_PAR_T_TYPE_5},
               #endif
               #if CO_NO_RPDO > 6
                  {ODD_RPDO_PAR_COB_ID_6, ODD_RPDO_PAR_T_TYPE_6},
               #endif
               #if CO_NO_RPDO > 7
                  {ODD_RPDO_PAR_COB_ID_7, ODD_RPDO_PAR_T_TYPE_7},
               #endif
               };
           #endif

           #ifdef CO_PDO_MAPPING_IN_OD
           #if CO_NO_RPDO > 0
		   //2007-6-20
/*0x1600+*///ROM UNSIGNED8      ODE_RPDO_Mapping_NoOfEntries = 8;//默认为8个映射，实际最多2个，4个字
           //ROM UNSIGNED32     ODE_RPDO_Mapping[CO_NO_RPDO][8] = {
		   //UNSIGNED8      ODE_RPDO1_Mapping_NoOfEntries = 1;///2007-7-6/默认为8个映射，实际最多2个，4个字
           //UNSIGNED8      ODE_RPDO2_Mapping_NoOfEntries = 0;//2007-7-11
		   VOLATILE UNSIGNED8      ODE_RPDO_Mapping_NoOfEntries[8] = {2,2,0,0,0,0,0,0};
           VOLATILE UNSIGNED32     ODE_RPDO_Mapping[CO_NO_RPDO][8] = {
                  {ODD_RPDO_MAP_0_1, ODD_RPDO_MAP_0_2, ODD_RPDO_MAP_0_3, ODD_RPDO_MAP_0_4, ODD_RPDO_MAP_0_5, ODD_RPDO_MAP_0_6, ODD_RPDO_MAP_0_7, ODD_RPDO_MAP_0_8},
               #if CO_NO_RPDO > 1
                  {ODD_RPDO_MAP_1_1, ODD_RPDO_MAP_1_2, ODD_RPDO_MAP_1_3, ODD_RPDO_MAP_1_4, ODD_RPDO_MAP_1_5, ODD_RPDO_MAP_1_6, ODD_RPDO_MAP_1_7, ODD_RPDO_MAP_1_8},
               #endif
               #if CO_NO_RPDO > 2
                  {ODD_RPDO_MAP_2_1, ODD_RPDO_MAP_2_2, ODD_RPDO_MAP_2_3, ODD_RPDO_MAP_2_4, ODD_RPDO_MAP_2_5, ODD_RPDO_MAP_2_6, ODD_RPDO_MAP_2_7, ODD_RPDO_MAP_2_8},
               #endif
               #if CO_NO_RPDO > 3
                  {ODD_RPDO_MAP_3_1, ODD_RPDO_MAP_3_2, ODD_RPDO_MAP_3_3, ODD_RPDO_MAP_3_4, ODD_RPDO_MAP_3_5, ODD_RPDO_MAP_3_6, ODD_RPDO_MAP_3_7, ODD_RPDO_MAP_3_8},
               #endif
               #if CO_NO_RPDO > 4
                  {ODD_RPDO_MAP_4_1, ODD_RPDO_MAP_4_2, ODD_RPDO_MAP_4_3, ODD_RPDO_MAP_4_4, ODD_RPDO_MAP_4_5, ODD_RPDO_MAP_4_6, ODD_RPDO_MAP_4_7, ODD_RPDO_MAP_4_8},
               #endif
               #if CO_NO_RPDO > 5
                  {ODD_RPDO_MAP_5_1, ODD_RPDO_MAP_5_2, ODD_RPDO_MAP_5_3, ODD_RPDO_MAP_5_4, ODD_RPDO_MAP_5_5, ODD_RPDO_MAP_5_6, ODD_RPDO_MAP_5_7, ODD_RPDO_MAP_5_8},
               #endif
               #if CO_NO_RPDO > 6
                  {ODD_RPDO_MAP_6_1, ODD_RPDO_MAP_6_2, ODD_RPDO_MAP_6_3, ODD_RPDO_MAP_6_4, ODD_RPDO_MAP_6_5, ODD_RPDO_MAP_6_6, ODD_RPDO_MAP_6_7, ODD_RPDO_MAP_6_8},
               #endif
               #if CO_NO_RPDO > 7
                  {ODD_RPDO_MAP_7_1, ODD_RPDO_MAP_7_2, ODD_RPDO_MAP_7_3, ODD_RPDO_MAP_7_4, ODD_RPDO_MAP_7_5, ODD_RPDO_MAP_7_6, ODD_RPDO_MAP_7_7, ODD_RPDO_MAP_7_8},
               #endif
               };
           #endif
           #endif		   

           #if CO_NO_TPDO > 0
		   //2007-6-22
/*0x1800+*///ROM UNSIGNED8      ODE_TPDO_Parameter_NoOfEntries =
			VOLATILE UNSIGNED8      ODE_TPDO_Parameter_NoOfEntries =
           #ifdef CO_TPDO_INH_EV_TIMER	//根据需要规划数组
                     5;
           #else
                     2;
           #endif
		   //2007-6-22
           //ROM CO_sTPDO_param ODE_TPDO_Parameter[CO_NO_TPDO] = {
		   VOLATILE CO_sTPDO_param ODE_TPDO_Parameter[CO_NO_TPDO] = {
                  #if ODD_TPDO_PAR_COB_ID_0 == 0
                     {CAN_ID_TPDO0, ODD_TPDO_PAR_T_TYPE_0, ODD_TPDO_PAR_I_TIME_0, ODD_TPDO_PAR_E_TIME_0},
                  #else
                     {ODD_TPDO_PAR_COB_ID_0, ODD_TPDO_PAR_T_TYPE_0, ODD_TPDO_PAR_I_TIME_0, ODD_TPDO_PAR_E_TIME_0},
                  #endif
               #if CO_NO_TPDO > 1
                  #if ODD_TPDO_PAR_COB_ID_1 == 0
                     {CAN_ID_TPDO1, ODD_TPDO_PAR_T_TYPE_1, ODD_TPDO_PAR_I_TIME_1, ODD_TPDO_PAR_E_TIME_1},
                  #else
                     {ODD_TPDO_PAR_COB_ID_1, ODD_TPDO_PAR_T_TYPE_1, ODD_TPDO_PAR_I_TIME_1, ODD_TPDO_PAR_E_TIME_1},
                  #endif
               #endif
               #if CO_NO_TPDO > 2
                  #if ODD_TPDO_PAR_COB_ID_2 == 0
                     {CAN_ID_TPDO2, ODD_TPDO_PAR_T_TYPE_2, ODD_TPDO_PAR_I_TIME_2, ODD_TPDO_PAR_E_TIME_2},
                  #else
                     {ODD_TPDO_PAR_COB_ID_2, ODD_TPDO_PAR_T_TYPE_2, ODD_TPDO_PAR_I_TIME_2, ODD_TPDO_PAR_E_TIME_2},
                  #endif
               #endif
               #if CO_NO_TPDO > 3
                  #if ODD_TPDO_PAR_COB_ID_3 == 0
                     {CAN_ID_TPDO3, ODD_TPDO_PAR_T_TYPE_3, ODD_TPDO_PAR_I_TIME_3, ODD_TPDO_PAR_E_TIME_3},
                  #else
                     {ODD_TPDO_PAR_COB_ID_3, ODD_TPDO_PAR_T_TYPE_3, ODD_TPDO_PAR_I_TIME_3, ODD_TPDO_PAR_E_TIME_3},
                  #endif
               #endif
               #if CO_NO_TPDO > 4
                  {ODD_TPDO_PAR_COB_ID_4, ODD_TPDO_PAR_T_TYPE_4, ODD_TPDO_PAR_I_TIME_4, ODD_TPDO_PAR_E_TIME_4},
               #endif
               #if CO_NO_TPDO > 5
                  {ODD_TPDO_PAR_COB_ID_5, ODD_TPDO_PAR_T_TYPE_5, ODD_TPDO_PAR_I_TIME_5, ODD_TPDO_PAR_E_TIME_5},
               #endif
               #if CO_NO_TPDO > 6
                  {ODD_TPDO_PAR_COB_ID_6, ODD_TPDO_PAR_T_TYPE_6, ODD_TPDO_PAR_I_TIME_6, ODD_TPDO_PAR_E_TIME_6},
               #endif
               #if CO_NO_TPDO > 7
                  {ODD_TPDO_PAR_COB_ID_7, ODD_TPDO_PAR_T_TYPE_7, ODD_TPDO_PAR_I_TIME_7, ODD_TPDO_PAR_E_TIME_7},
               #endif
               };
           #endif

           #ifdef CO_PDO_MAPPING_IN_OD
           #if CO_NO_TPDO > 0
		   //2007-6-22
/*0x1A00+*///ROM UNSIGNED8      ODE_TPDO_Mapping_NoOfEntries = 8;
           //ROM UNSIGNED32     ODE_TPDO_Mapping[CO_NO_TPDO][8] = {
		   //UNSIGNED8     ODE_TPDO1_Mapping_NoOfEntries = 1;//CONST_
		   //UNSIGNED8     ODE_TPDO2_Mapping_NoOfEntries = 0;//	
		   VOLATILE UNSIGNED8     ODE_TPDO_Mapping_NoOfEntries[8] = {2,2,0,0,0,0,0,0};		   
           VOLATILE UNSIGNED32     ODE_TPDO_Mapping[CO_NO_TPDO][8] = {
                  {ODD_TPDO_MAP_0_1, ODD_TPDO_MAP_0_2, ODD_TPDO_MAP_0_3, ODD_TPDO_MAP_0_4, ODD_TPDO_MAP_0_5, ODD_TPDO_MAP_0_6, ODD_TPDO_MAP_0_7, ODD_TPDO_MAP_0_8},
               #if CO_NO_TPDO > 1
                  {ODD_TPDO_MAP_1_1, ODD_TPDO_MAP_1_2, ODD_TPDO_MAP_1_3, ODD_TPDO_MAP_1_4, ODD_TPDO_MAP_1_5, ODD_TPDO_MAP_1_6, ODD_TPDO_MAP_1_7, ODD_TPDO_MAP_1_8},
               #endif
               #if CO_NO_TPDO > 2
                  {ODD_TPDO_MAP_2_1, ODD_TPDO_MAP_2_2, ODD_TPDO_MAP_2_3, ODD_TPDO_MAP_2_4, ODD_TPDO_MAP_2_5, ODD_TPDO_MAP_2_6, ODD_TPDO_MAP_2_7, ODD_TPDO_MAP_2_8},
               #endif
               #if CO_NO_TPDO > 3
                  {ODD_TPDO_MAP_3_1, ODD_TPDO_MAP_3_2, ODD_TPDO_MAP_3_3, ODD_TPDO_MAP_3_4, ODD_TPDO_MAP_3_5, ODD_TPDO_MAP_3_6, ODD_TPDO_MAP_3_7, ODD_TPDO_MAP_3_8},
               #endif
               #if CO_NO_TPDO > 4
                  {ODD_TPDO_MAP_4_1, ODD_TPDO_MAP_4_2, ODD_TPDO_MAP_4_3, ODD_TPDO_MAP_4_4, ODD_TPDO_MAP_4_5, ODD_TPDO_MAP_4_6, ODD_TPDO_MAP_4_7, ODD_TPDO_MAP_4_8},
               #endif
               #if CO_NO_TPDO > 5
                  {ODD_TPDO_MAP_5_1, ODD_TPDO_MAP_5_2, ODD_TPDO_MAP_5_3, ODD_TPDO_MAP_5_4, ODD_TPDO_MAP_5_5, ODD_TPDO_MAP_5_6, ODD_TPDO_MAP_5_7, ODD_TPDO_MAP_5_8},
               #endif
               #if CO_NO_TPDO > 6
                  {ODD_TPDO_MAP_6_1, ODD_TPDO_MAP_6_2, ODD_TPDO_MAP_6_3, ODD_TPDO_MAP_6_4, ODD_TPDO_MAP_6_5, ODD_TPDO_MAP_6_6, ODD_TPDO_MAP_6_7, ODD_TPDO_MAP_6_8},
               #endif
               #if CO_NO_TPDO > 7
                  {ODD_TPDO_MAP_7_1, ODD_TPDO_MAP_7_2, ODD_TPDO_MAP_7_3, ODD_TPDO_MAP_7_4, ODD_TPDO_MAP_7_5, ODD_TPDO_MAP_7_6, ODD_TPDO_MAP_7_7, ODD_TPDO_MAP_7_8},
               #endif
               };
           #endif
           #endif

/*0x1F80*/ ROM UNSIGNED32     ODE_NMT_Startup = ODD_NMT_STARTUP;//only bit2 implemented, if bit2=1, Node will NOT start operational
/*0x2000*/ ROM UNSIGNED8 	  ODE_TPDO1_MASK_NoOfEntries = 2;
/*0x2001*/ ROM UNSIGNED8 	  ODE_TPDO2_MASK_NoOfEntries = 2;
/*0x2002*/ ROM UNSIGNED8 	  ODE_TPDO3_MASK_NoOfEntries = 2;
/*0x2003*/ ROM UNSIGNED8 	  ODE_TPDO4_MASK_NoOfEntries = 2;
		   volatile UNSIGNED32 ODE_TPDO_MASK[4][2] = {0xFFFFFFFF,0xFFFFFFFF,
													  0xFFFFFFFF,0xFFFFFFFF,
													  0xFFFFFFFF,0xFFFFFFFF,
													  0xFFFFFFFF,0xFFFFFFFF};

/***** Manufacturer specific variables ****************************************/
/*0x2100*/ extern unsigned char CO_ErrorStatusBits[];
/*0x2101*/ //ROM UNSIGNED8      ODE_CANnodeID = ODD_CANnodeID;
			volatile unsigned int ODE_CANnodeID;
                //this entery can be replaced with DIP switches on printed board
/*0x2102*/ //ROM UNSIGNED8      ODE_CANbitRate = ODD_CANbitRate;
            volatile unsigned int ODE_CANbitRate;
                //this entery can be replaced with DIP switches on printed board
           #if CO_NO_SYNC > 0
/*0x2103*/ extern volatile unsigned int CO_SYNCcounter;
                //variable is incremented after SYNC message
/*0x2104*/ extern volatile unsigned int CO_SYNCtime;
		   //extern volatile unsigned long CO_SYNCtime;
                //variable is incremented every 1ms, after SYNC message it is set to 0
           #endif


/***** Manufacturer specific EEPROM DATA **************************************/
//Data in following structure are read from EEPROM at microcontroller intialisation
//and is written automatically to EEPROM when changed. Structure is defined in CO_OD.h
//               sODE_EEPROM    ODE_EEPROM;
//               unsigned char* CO_EEPROM_pointer = (unsigned char*) &ODE_EEPROM;
//           ROM unsigned int   CO_EEPROM_size = sizeof(ODE_EEPROM);


/***** Device profile for Generic I/O *****************************************/
/*0x6040*/ #ifdef SERVO_CONTROL
      //paraToFram.Control_Word
	  //paraToFram.System_Status
#define SINE_PM_Motor	0x0003
#define DRIVE_MODE	0x25	//支持pp,pv,hm控制方式

	  VOLATILE INTEGER16 Connection_Option=2;//Abort connection option code
      VOLATILE UNSIGNED16 Error_Code=0;//Error code
//Motor manufacturer data	  
	  VOLATILE UNSIGNED16 Motor_Type=SINE_PM_Motor;//Motor_Type:Sinusoidal PM brushless motor
	  //UNSIGNED16
	  VOLATILE char Motor_Catalog;//Motor catalog number
	  //UNSIGNED16
	  VOLATILE char Motor_Manufacturer;//Motor manufacturer
	  VOLATILE UNSIGNED16 Motor_Address;//motor catalog address
	  VOLATILE UNSIGNED16 Motor_Data;//motor calibration date
	  VOLATILE UNSIGNED32 Motor_Service_Period;//Motor service period,hours
//Motor data
	  VOLATILE UNSIGNED8 Motor_NoOfEntries;//Motor data     
      VOLATILE UNSIGNED32 Motor_Current;//Motor data
	  VOLATILE UNSIGNED32 Motor_Torque;//Motor data
	  VOLATILE UNSIGNED32 Motor_Speed;//Motor data
	  VOLATILE UNSIGNED32 Motor_Encoder;//Motor data
	  VOLATILE UNSIGNED32 Motor_Angel;//Motor data
	  VOLATILE UNSIGNED32 Motor_R;//Motor data
//Drive manufacturer data	
	  VOLATILE UNSIGNED16 Drive_Mode=DRIVE_MODE;//Supported drive modes
//----------------------------------------------------------------------------
//		DATA DESCRIPTION
//31 					16 15 	7 	6 	5 	4 		 3  2  1  0
//manufacturer specific reserved 	ip 	hm 	reserved tq pv vl pp
//MSB													LSB
//----------------------------------------------------------------------------
	  VOLATILE UNSIGNED16  Drive_Nameplate_Number;//Drive catalog number
	  //UNSIGNED16
	  VOLATILE char  Drive_Manufacturer[]=ODD_MANUF_DEVICE_NAME;//Drive manufacturer
	  //UNSIGNED16
	  VOLATILE char  Drive_Catalog_Address[]=DRIVE_CATALOG_ADDRESS;//drive catalog address
//drive data
	  VOLATILE UNSIGNED8  Drive_NoOfEntries;//drive data
	  VOLATILE UNSIGNED32 Drive_Input_Current;//drive data
	  VOLATILE UNSIGNED32 Drive_Output_Current;//drive data
	  VOLATILE UNSIGNED32 Drive_Output_Power;//drive data
//
	  VOLATILE UNSIGNED32 Digital_Inputs;//Digital inputs
//------------------------------------------------------------------------------
//			DATA DESCRIPTION
//31 				16 15 		4 		3 		2 			1				 0
//manufacturer specific|reserved |interlock |home switch| positive limit | negative limit 
//															switch	  		switch
//MSB																		 LSB
//The switch have to be "active high".
//------------------------------------------------------------------------------      
      VOLATILE UNSIGNED8 Digital_NoOfEntries=2;//Digital Outputs
	  VOLATILE UNSIGNED32 Physical_Outputs = 0;//Physical outputs
	  VOLATILE UNSIGNED32 Bit_Mask=0;//Bit mask

//---------------------停止处理方式---------------------------------------------------------
	  VOLATILE INTEGER16 Shutdown_Option			=0;//Shutdown option code
	  VOLATILE INTEGER16 Disable_Operation_Option	=0;//Disable operation option code
	  VOLATILE INTEGER16 Quickstop_Option_Code		=2;//Quick stop option code
	  VOLATILE INTEGER16 Halt_Option_Code			=1;//Halt option code
	  VOLATILE INTEGER16 Falt_Reaction_Option_Code	=0;//Fault reaction option code
//---------------------控制方式及其显示--------------------------------------------------------
	  VOLATILE INTEGER8 Operation_Mode				=1;//
	  VOLATILE INTEGER8 Operation_Mode_Display 	=0;
//------------------------------------------------------------------------------
	  VOLATILE UNSIGNED16 Polarity					=0;		//Polarity
//---------------------定义Factor---------------------------------------------------------
	  VOLATILE UNSIGNED8  Position_Encoder_NoOfEntries 	=2;	//Position encoder resolution
	  VOLATILE UNSIGNED32 Position_Encoder_Increments	=0;	//Position encoder resolution
	  VOLATILE UNSIGNED32 Position_Encoder_Revolutions	=0;	//Position encoder resolution

	  VOLATILE UNSIGNED8  Velocity_Encoder_NoOfEntries	=2;	//Velocity encoder resolution
	  VOLATILE UNSIGNED32 Velocity_Encoder_Increments	=0;	//Velocity encoder resolution
	  VOLATILE UNSIGNED32 Velocity_Encoder_Revolutions	=0;	//Velocity encoder resolution
  
	  VOLATILE UNSIGNED8  Gear_Ratio_NoOfEntries 		=2;	//Gear ratio
	  VOLATILE UNSIGNED32 Motor_Revolutions				=0;	//Motor revolutions
	  VOLATILE UNSIGNED32 Shaft_Revolutions				=0;	//Shaft revolutions
	  
	  VOLATILE UNSIGNED8  Feed_Constant_NoOfEntries 	=2;	//Feed constant
	  VOLATILE UNSIGNED32 Feed							=0;	//Feed
	  VOLATILE UNSIGNED32 Feed_Shaft_Revolutions		=0;	//Shaft revolutions

	  VOLATILE UNSIGNED8  Position_Factor_NoOfEntries 	=2;	//Position factor
	  VOLATILE UNSIGNED32 Position_Numerator			=1;	//Numerator
	  VOLATILE UNSIGNED32 Position_Divisor				=1;	//Feed constant

	  VOLATILE UNSIGNED8  Velocity_Factor_NoOfEntries 	=2;	//Velocity encoder factor
	  VOLATILE UNSIGNED32 Velocity_Numerator				=1;	//Numerator
	  VOLATILE UNSIGNED32 Velocity_Divisor				=1;	//Velocity_Divisor

	  VOLATILE UNSIGNED8  Velocity_Factor1_NoOfEntries 	=2;	//Velocity factor 1
	  VOLATILE UNSIGNED32 Velocity_Numerator1			=1;	//Numerator
	  VOLATILE UNSIGNED32 Velocity_Divisor1				=1;	//Velocity_Divisor

	  VOLATILE UNSIGNED8  Velocity_Factor2_NoOfEntries 	=2;	//Velocity factor 1
	  VOLATILE UNSIGNED32 Velocity_Numerator2			=1;	//Numerator
	  VOLATILE UNSIGNED32 Velocity_Divisor2				=1;	//Velocity_Divisor

	  VOLATILE UNSIGNED8  Acceleration_Factor_NoOfEntries=2;	//Velocity factor 1
	  VOLATILE UNSIGNED32 Acceleration_Numerator		=1;	//Numerator
	  VOLATILE UNSIGNED32 Acceleration_Divisor			=1;	//Velocity_Divisor
	  
//----------------------------------------------------------------------------
//------------------------ POSITION CONTROL FUNCTION--------------------------------------------------------
	  VOLATILE INTEGER32 Position_Demand_Value		=0;
	  VOLATILE INTEGER32 Position_Demand_Value_aster=0;
	  VOLATILE INTEGER32 Position_Acture_Value		=0;
	  VOLATILE INTEGER32 Position_Acture_Value_aster=0;
	  VOLATILE UNSIGNED32 Following_Error_Window	=FOLLOWING_ERROR_WINDOW;
	  VOLATILE UNSIGNED16 Following_Error_Time_Out	=FOLLOWING_ERROR_TIME_OUT;
	  VOLATILE UNSIGNED32 Position_Window 			=10;
	  VOLATILE UNSIGNED16 Position_Window_Time 		=50;
	  VOLATILE INTEGER32 Following_Error_Actual_Value=0;
	  VOLATILE INTEGER32 Control_Effort				=0;
//----------------------PROFILE POSITION MODE------------------------------------------------------
	  VOLATILE INTEGER32 Target_Position			=0;
	  VOLATILE UNSIGNED8 Position_Range_NoOfEntries =2;
	  VOLATILE INTEGER32 Min_Position_Range_Limit	=-POSITION_LIMIT;		//初始化位置限制
	  VOLATILE INTEGER32 Max_Position_Range_Limit	=POSITION_LIMIT;

	  VOLATILE UNSIGNED8 Software_Position_Limit	=2;
	  VOLATILE INTEGER32 Min_Position_Limit			=-SOFT_POSITION_LIMIT;	//初始化软件位置限制
	  VOLATILE INTEGER32 Max_Position_Limit			=SOFT_POSITION_LIMIT;

	  VOLATILE UNSIGNED32 Max_Profile_Velocity		=0;
	  VOLATILE UNSIGNED32 Max_Motor_Speed			=0;	
	  VOLATILE UNSIGNED32 Profile_Velocity			=0;
	  VOLATILE UNSIGNED32 END_Velocity				=0;
	  VOLATILE UNSIGNED32 Profile_Acceleration		=100000;
	  VOLATILE UNSIGNED32 Profile_Decelaration		=100000;
	  VOLATILE UNSIGNED32 Quick_Stop_Deceleration	=200000;
	  VOLATILE UNSIGNED32 Profile_Acceleration_aster=0;
	  VOLATILE UNSIGNED32 Profile_Decelaration_aster=0;
	  VOLATILE UNSIGNED32 Quick_Stop_Deceleration_aster=0;
	  VOLATILE INTEGER16  Motion_Profile_Type 		=0;	
	  VOLATILE UNSIGNED8  Profile_Jerk_Use 		    =1;	   //added by niekefu for s shape speed 2012.6.11
	  VOLATILE INTEGER8   Profile_Jerk_NoOfEntries = 1;
	  VOLATILE UNSIGNED32 Profile_Jerk1 = 5;
	  VOLATILE UNSIGNED32 Max_Acceleration			=200000000;
	  VOLATILE UNSIGNED32 Max_Deceleration			=200000000;
//------------------------- Profile Velocity control-------------------------------------------------------
	  VOLATILE INTEGER32 Velocity_Sensor_Actual_Value=0;
	  VOLATILE INTEGER16 Sensor_Selection_Code		=0;
	  VOLATILE INTEGER32 Velocity_Demand_Value		=0;
	  VOLATILE INTEGER32 Velocity_Actual_Value		=0;
	  VOLATILE UNSIGNED16 Velocity_Window 			=20;
	  VOLATILE UNSIGNED16 Velocity_Window_Time 		=0;
	  VOLATILE UNSIGNED16 Velocity_Threshold		=50;
	  VOLATILE UNSIGNED16 Velocity_Threshold_Time	=0;
	  VOLATILE INTEGER32 Target_Velocity			=0;
//-------------------------Interpolate Position-------------------------------------------------------
	  VOLATILE INTEGER16 Interpolatio_Sub_Mode_Select = 0;
	  VOLATILE UNSIGNED8 Interpolation_Data_Record_NoOfEntries = 2;	//*********还有子索引************
	  VOLATILE INTEGER32 Interpolation_Data_Record1 = 0;
	  VOLATILE INTEGER32 Interpolation_Data_Record2 = 0;
	  //VOLATILE UNSIGNED8 Interpolation_Time_Period = 250;			//*********************
	  VOLATILE UNSIGNED8 Interpolation_Time_NoOfEntries = 2;
	  VOLATILE UNSIGNED8 Interpolation_Time_Unit =  1;		//default 1ms
	  VOLATILE INTEGER8 Interpolation_Time_Index = -3;		//1000 us

	  VOLATILE UNSIGNED8 Interpolation_Sync_Definition;		//*******还有子索引**************
	  VOLATILE UNSIGNED32 Interpolation_Data_Configuration;	//*******还有子索引**************
//-------------------------Profile Torque Control-------------------------------------------------------
	  VOLATILE INTEGER16 Target_Torque				=0;
	  VOLATILE UNSIGNED16 Max_Torque				=0;
	  VOLATILE UNSIGNED16 Max_Current				=0;
	  VOLATILE INTEGER16 Torque_Demand_Value		=0;
	  VOLATILE UNSIGNED32 Motor_Rated_Current		=0;
	  VOLATILE UNSIGNED32 Motor_Rated_Torque		=0;
	  VOLATILE INTEGER16 Torque_Actual_Value		=0;
	  VOLATILE INTEGER16 Current_Actual_Value		=0;
	  VOLATILE UNSIGNED32 Torque_Slope				=0;
	  VOLATILE INTEGER16 Torque_Profile_Type		=0;

//------------------------HOMING MODE--------------------------------------------------------
	  VOLATILE INTEGER32 Home_Offset 				=0;
	  VOLATILE INTEGER8 Homing_Method 				=1;

	  VOLATILE UNSIGNED8  Homing_Speeds_NoOfEntries =2;
	  VOLATILE UNSIGNED32 Switch_Speed				=5000;
	  VOLATILE UNSIGNED32 Zero_Speed				=100;
	  VOLATILE UNSIGNED32 Homing_Acceleration		=200000;
	  VOLATILE UNSIGNED32 Homing_Acceleration_aster	=0;
   #endif

           #ifdef CO_IO_DIGITAL_INPUTS
/*0x6000*/ ROM UNSIGNED8      ODE_Read_Digital_Input_NoOfEntries = 4;
               tData4bytes    ODE_Read_Digital_Input;
           #endif

           #ifdef CO_IO_DIGITAL_OUTPUTS
/*0x6200*/ ROM UNSIGNED8      ODE_Write_Digital_Output_NoOfEntries = 4;
               tData4bytes    ODE_Write_Digital_Output;
           #endif

           #ifdef CO_IO_ANALOG_INPUTS
/*0x6401*/ ROM UNSIGNED8      ODE_Read_Analog_Input_NoOfEntries = 8;
               INTEGER16      ODE_Read_Analog_Input[8];
           #endif

           #ifdef CO_IO_ANALOG_OUTPUTS
/*0x6411*/ ROM UNSIGNED8      ODE_Write_Analog_Output_NoOfEntries = 2;
               INTEGER16      ODE_Write_Analog_Output[2];
           #endif

/*0x3001*/ 	ROM UNSIGNED8      ODE_RW_SetProdcnt_NoOfEntries = 1;
/*0x3001*/ ROM UNSIGNED8      ODE_RW_Prodcnt_NoOfEntries = 1;

/*0x3011~0x301A*/ ROM UNSIGNED8      ODE_RW_CAM_NoOfEntries = 2;
/*0x3041~0x304D*/ ROM UNSIGNED8      ODE_RW_PN_NoOfEntries = 1;
/*0x30A1~0x304D*/ ROM UNSIGNED8      ODE_RW_UN_NoOfEntries1 = 4;
/*0x30A1~0x304D*/ ROM UNSIGNED8      ODE_RW_UN_NoOfEntries2 = 3;
/*0x30A1~0x304D*/ ROM UNSIGNED8      ODE_RW_UN_NoOfEntries3 = 1;
extern struct ECAN_REGS ECanbShadow;//2011-08-17

typedef	struct{	 
				volatile Uint32 start_v;	
				volatile Uint32 target_v; 
				volatile Uint32 end_v;				   		
				volatile Uint32 acc;
				volatile Uint32 dec;
				volatile Uint32 quickdec;
				volatile int32  target_position;
				volatile int32  idea_actual_position;
				volatile int32  idea_start_position;
				volatile int32  increase_pulse;
				volatile int32  dec_position;
				volatile int32	dec_length;

}CO_PP_VELOCITY_PLAN_PARS;
extern volatile CO_PP_VELOCITY_PLAN_PARS CO_PP_velocity_plan_pars;

//#ifdef __18CXX
//   #pragma romdata //return to the default section

//#endif


/******************************************************************************/
/*     VERIFY FUNCTION      ***************************************************/
/******************************************************************************/
//SDO server verifies new values, before they are written to Object Dictionary
//Typical Error codes:
//       0x06090030L   Value range of parameter exceeded 	超出参数的值范围(写访问时)
//       0x06090031L   Value of parameter written too high 	写入参数数值太大
//       0x06090032L   Value of parameter written too low	写入参数数值太小

#ifdef CO_VERIFY_OD_WRITE
unsigned long CO_OD_VerifyWrite(ROM CO_objectDictionaryEntry* pODE, void* data)
{
   unsigned int index = pODE->index;
   unsigned int PDOindex = 0;
   unsigned int PDOsubindex = 0;
   unsigned int PDOlength = 0;
   unsigned int tmp = 0, i = 0, MAPlength = 0;
   int	tmp1= 0;
   ROM CO_objectDictionaryEntry  * pODEtmp;
   unsigned long temp = 0;
   unsigned long cob_id = 0;
   
   if(index > 0x1200 && index <= 0x12FF) index &= 0xFF80;//All SDO
   if(index > 0x1400 && index <= 0x1BFF) index &= 0xFE00;//All PDO

   switch(index){
   //   unsigned char i;

   #if CO_NO_ERROR_FIELD > 0
   case 0x1003://Pre Defined Error Field
               if(*((unsigned char*)data) > ODE_Pre_Defined_Error_Field_NoOfErrors)
                  //写入参数数值太大
                  return 0x06090031L;  //Value of parameter written too high
               break;
   #endif

   #if CO_NO_SYNC > 0
   case 0x1005://SYNC COB ID
               if(*((unsigned long*)data) & 0xBFFFF800L)
                  return 0x08000020L;  //Data can not be transferred or stored to the application
               if((ODE_SYNC_COB_ID & 0x40000000L) &&
                  (*((unsigned int*)data) != (ODE_SYNC_COB_ID&0x7FF)))
                  return 0x08000022L;  //Data can not be transferred or stored to the application because of the present device state
               break;

   case 0x1006:   //Communication Cycle Period
               if(*((unsigned int*)data) != 0 && *((unsigned int*)data) < 300)//3000)
                  //由于当前设备状态导致数据不能传送或保存到应用
                  return 0x08000022L;  //Data can not be transferred or stored to the application because of the present device state
   				break;
   #endif

   #if CO_NO_CONS_HEARTBEAT > 0
   case 0x1016://Consumer Heartbeat Time
               if(*((unsigned long*)data) & 0xFF800000L)
                  return 0x08000020L;  //Data can not be transferred or stored to the application
               for(i = 0; i<CO_NO_CONS_HEARTBEAT; i++){
                  if((*((unsigned long*)data)>>16) == (ODE_Consumer_Heartbeat_Time[i]>>16) &&   //same NodeID
                     (pODE->subindex-1) != i && //different subindex
                     *((unsigned int*)(data)) != 0 &&    //time nonzero
                     (ODE_Consumer_Heartbeat_Time[i]&0xFFFF) != 0) //time nonzero
                     return 0x08000022L;  //Data can not be transferred or stored to the application because of the present device state
               }
               break;
   #endif
/**/
   #if CO_NO_SDO_SERVER > 0
   case 0x1200://Server SDO Parameter
               if(pODE->subindex == 1 || pODE->subindex == 2){
                  //unsigned long COB_ID;
                  //if(pODE->subindex == 1) COB_ID = ODE_Server_SDO_Parameter[pODE->index&0x7F].COB_ID_Client_to_Server;
                  //else                    COB_ID = ODE_Server_SDO_Parameter[pODE->index&0x7F].COB_ID_Server_to_Client;
                  if((*((unsigned long*)data) & 0x7FFFF800L))
                     return 0x08000020L;  //Data can not be transferred or stored to the application
                  //if(*((unsigned int*)(data)) != (COB_ID&0xFFFF) &&   //Different COB ID
                  //   !(COB_ID&0x80000000L))   //bit 31 is 0 (SDO in use)
                  //   return 0x08000022L;  //Data can not be transferred or stored to the application because of the present device state
               }
               break;
   #endif

   #if CO_NO_SDO_CLIENT > 0
   case 0x1280://Client SDO Parameter
               if(pODE->subindex == 1 || pODE->subindex == 2){
                  //unsigned long COB_ID;
                  //if(pODE->subindex == 1) COB_ID = ODE_Client_SDO_Parameter[pODE->index&0x7F].COB_ID_Client_to_Server;
                  //else                    COB_ID = ODE_Client_SDO_Parameter[pODE->index&0x7F].COB_ID_Server_to_Client;
                  if((*((unsigned long*)data) & 0x7FFFF800L))
                     return 0x08000020L;  //Data can not be transferred or stored to the application
                  //if(*((unsigned int*)(data)) != (COB_ID&0xFFFF) &&   //Different COB ID
                  //   !(COB_ID&0x80000000L))   //bit 31 is 0 (SDO in use)
                  //   return 0x08000022L;  //Data can not be transferred or stored to the application because of the present device state
               }
               break;
   #endif

   #if CO_NO_RPDO > 0
	case 0x1400://Receive PDO Parameter
		if(pODE->subindex == 1)		
		{//unsigned long COB_ID = ODE_RPDO_Parameter[pODE->index&0x1FF].COB_ID;
			volatile struct MBOX * pMBOX;
			temp = *((unsigned long*)pODE->pData);
			cob_id = *((unsigned long*)data);
			tmp = pODE->index - 0x1400;
			pMBOX = & ECanbMboxes.MBOX18 + tmp;
			if(cob_id & 0x80000000L)//already in disable status
			{
				if(cob_id & 0x3FFFF800L)
				{
					return 0x06090030L;	
				}
				else
				{
					ECanbShadow.CANME.all = ECanbRegs.CANME.all;
					temp = (ECanbShadow.CANME.all>>(tmp+18)) & 0x00000001;

					if(temp)//close mailbox
					{
						ECanbShadow.CANME.all &= (0xFFFFFFFFL ^((Uint32)1<<(18+tmp)));
						ECanbRegs.CANME.all = ECanbShadow.CANME.all;
					}

					pMBOX->MSGID.bit.IDE = 0;
					pMBOX->MSGID.bit.AME = 1;
					pMBOX->MSGID.bit.AAM = 0;
					pMBOX->MSGID.bit.STDMSGID = (*((unsigned long*)data)) & 0x000007FFL;
				}
			}
			else
			{
				if(((cob_id & 0x80000000L) == 0) && ((temp & 0x80000000L) != 0) && ((cob_id & 0x3FFFF800L) == 0))
				{
					if((cob_id & 0x000007FFL)  == pMBOX->MSGID.bit.STDMSGID)
					{
						ECanbShadow.CANME.all |= ((Uint32)1<<(18+tmp));
						ECanbRegs.CANME.all = ECanbShadow.CANME.all; //open the relevant mailbox
						CO_RXCAN[2+tmp].NewMsg = 0;
					}
					else
					{
						return 0x06090030L;
					}
				}
				else
				{
					return 0x06090030L;
				}						
			}
		}
		break;
   case 0x1600://Receive PDO Parameter
               if(pODE->subindex == 0)
			   {	
			   		MAPlength = *((unsigned long*)data);
					if( MAPlength > 4)
					{
						return 0x06090031L;  //Value of parameter written too high
					}
					else if(MAPlength && ODE_RPDO_Mapping_NoOfEntries[pODE->index & 0x00FF])
					{
						return 0x08000020L;
					}
					else
					{
						tmp = 0;
						for(i = 0; i < MAPlength; i++)
						{
							tmp += ((ODE_RPDO_Mapping[pODE->index & 0x00FF][i] & 0x00FF)>>3);
						}
						if(tmp > 8)
						{
							return 0x06040042L;
						}
						tmp = 0;
						for(i = 0; i < MAPlength; i++)
						{
							if(ODE_RPDO_Mapping[pODE->index & 0x00FF][i])
							{
								++tmp;
							}
						}
						if(tmp != MAPlength)
						{
							return 0x06090031L;
						}
					}
			   }
			   else if(pODE->subindex > 4)
			   {
					return 0x06020000L;  //object does not exist in OD
			   }
			   else if (*((unsigned long*)data) == 0)
			   {
					break;
			   }
			   else
               {  
               		PDOlength = *((unsigned int*)data);
					PDOsubindex = (PDOlength & 0xFF00)>>8;
               		PDOindex  = *((unsigned int*)data+1); 
               		PDOlength = (PDOlength & 0x00FF)>>3;  
               		pODEtmp = CO_FindEntryInOD(PDOindex, PDOsubindex);   
               		if(Find_char_OD(PDOindex,PDOsubindex,pODEtmp->length))
					{
						if(pODEtmp->length != (PDOlength<<1))
						{// length error
							return 0x06070010L;
						} 
					}
               		else if(PDOlength != pODEtmp->length)
               		{// length error
						return 0x06070010L;
               		} 
               		else if((pODEtmp->attribute & 0x07) != ATTR_RWW)
               		{						
						return 0x06040041L;
               		}			        		                  
               }
               break;
   #endif
/**/
   #if CO_NO_TPDO > 0
   case 0x1800://Transmit PDO Parameter
        if(pODE->subindex == 1)		
		{//unsigned long COB_ID = ODE_RPDO_Parameter[pODE->index&0x1FF].COB_ID;
			volatile struct MBOX * pMBOX;
			cob_id = *((unsigned long*)data);;
			temp = *((unsigned long*)pODE->pData);
			tmp = pODE->index - 0x1800;
			pMBOX = & ECanbMboxes.MBOX2 + tmp;
			if(cob_id & 0x80000000L)//already in disable status
			{
				if((*((unsigned long*)data) & 0x3FFFF800L))
				{
					return 0x06090030L;	
				}
				else if((cob_id & 0x000007FFL) != 0)
				{
					ECanbShadow.CANME.all = ECanbRegs.CANME.all;
					temp = (ECanbShadow.CANME.all>>(2 + tmp)) & 0x00000001;
					if(temp)
					{
						ECanbShadow.CANME.all &= (0xFFFFFFFFL ^((Uint32)1<<(2 + tmp)));
						ECanbRegs.CANME.all = ECanbShadow.CANME.all;
					}						
					pMBOX->MSGID.bit.IDE = 0;
					pMBOX->MSGID.bit.AME = 1;
					pMBOX->MSGID.bit.AAM = 0;
					pMBOX->MSGID.bit.STDMSGID = (*((unsigned long*)data)) & 0x000007FFL;
				}
				else
				{
					return 0x06090030L;
				}
				
			}
			else//already in enable status
			{
				if(((cob_id & 0x80000000L) == 0) && ((temp & 0x80000000L) != 0) && ((cob_id & 0x3FFFF800L) == 0))
				{
					if((cob_id & 0x000007FFL) == pMBOX->MSGID.bit.STDMSGID)
					{
						ECanbShadow.CANME.all = ECanbRegs.CANME.all;
						temp = (ECanbShadow.CANME.all>>(2+tmp)) & 0x00000001;

						ECanbShadow.CANME.all |= ((Uint32)1<<(2+tmp));
						ECanbRegs.CANME.all = ECanbShadow.CANME.all; //open the relevant mailbox
						CO_TXCAN[2+tmp].NewMsg = 0;	
					}
					else
					{
						return 0x06090030L;	
					}	
				}
				else
				{
					return 0x06090030L;
				}
						
			}
			//if(*((unsigned int*)(data)) != (COB_ID&0xFFFF) &&   //Different COB ID
			//   !(COB_ID&0x80000000L))   //bit 31 is 0 (PDO in use)
			//   return 0x08000022L;  //Data can not be transferred or stored to the application because of the present device state
		}
        break;
   case 0x1A00://Transmit PDO Parameter
               if(pODE->subindex == 0)
			   {
					MAPlength = *((unsigned long*)data);
					if((*((unsigned long*)data) > 4))
					{
						return 0x06090031L;  //Value of parameter written too high
					}
					else if(MAPlength && ODE_TPDO_Mapping_NoOfEntries[pODE->index & 0x00FF])
					{
						return 0x08000020L;
					}
					else
					{
						tmp = 0;
						for(i = 0; i < MAPlength; i++)
						{
							tmp += ((ODE_TPDO_Mapping[pODE->index & 0x00FF][i] & 0x00FF)>>3);
						}
						if(tmp > 8)
						{
							return 0x06040042L;
						}
						tmp = 0;
						for(i = 0; i < MAPlength; i++)
						{
							if(ODE_TPDO_Mapping[pODE->index & 0x00FF][i])
							{
								++tmp;
							}
						}
						if(tmp != MAPlength)
						{
							return 0x06090031L;
						}
					}
			   }
			   else if(pODE->subindex > 4)
			   {
					return 0x06020000L;  //object does not exist in OD
			   }
			   else if (*((unsigned long*)data) == 0)
			   {
					break;
			   }
			   else
               {  
               		PDOlength = *((unsigned int*)data);
					PDOsubindex = (PDOlength & 0xFF00)>>8;
               		PDOindex  = *((unsigned int*)data+1); 
               		PDOlength = (PDOlength & 0x00FF)>>3;  
               		pODEtmp = CO_FindEntryInOD(PDOindex, PDOsubindex);   
					if(Find_char_OD(PDOindex,PDOsubindex,pODEtmp->length))
					{
						if(pODEtmp->length != (PDOlength<<1))
						{// length error
							return 0x06070010L;
						} 
					}
               		else if(PDOlength != pODEtmp->length)
               		{// length error
						return 0x06070010L;
               		} 
               		else if(((pODEtmp->attribute & 0x07) != ATTR_RO) && ((pODEtmp->attribute & 0x07) != ATTR_RWR))
					{
						return 0x06040041L;					
               		}			        		                  
               }
               break;
   #endif

   case 0x1F80://NMT Startup
               if(*((unsigned long*)data) & 0xFFFFFFFBL)
                  return 0x08000020L;  //Data can not be transferred or stored to the application
               break;

   case 0x2101://CAN Node ID
               if(*((unsigned char*)data) < 1)
                  //
                  return 0x06090032L;  //Value of parameter written too low
               else if(*((unsigned char*)data) > 127)
                  return 0x06090031L;  //Value of parameter written too high
               break;

   case 0x2102://CAN Bit RAte
               if(*((unsigned char*)data) > 7)
                  return 0x06090031L;  //Value of parameter written too high
               break;

   case 0x6007://Abort connection 
               if(*((unsigned int*)data) != 2)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x605A://Quick stop option code
               tmp = *((unsigned int*)data);
               if((tmp != 0) && (tmp != 1) && (tmp != 2) && (tmp != 5) && (tmp != 6))
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x605B://Shutdown option code
               tmp = *((unsigned int*)data);
               if((tmp != 0) && (tmp != 1))
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x605C://Disable operation option code 
               tmp = *((unsigned int*)data);
               if((tmp != 0) && (tmp != 1))
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x605D://Halt option code 
               tmp = *((unsigned int*)data);
               if((tmp != 1) && (tmp != 2))
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x605E://Fault reaction option code
               tmp = *((unsigned int*)data);
               if((tmp != 0))
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x6060://Operation_Mode
               tmp = *((unsigned int*)data);
               if((tmp != 1) && (tmp != 3) && (tmp != 6) && (tmp != 7))
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x606A://Sensor_Selection_Code
               if(*((unsigned int*)data) != 0)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x6083://acc
               if(*((unsigned long*)data) == 0)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x6084://dec
               if(*((unsigned long*)data) == 0)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x6085://quick dec
               if(*((unsigned long*)data) == 0)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x6086://motion_profile_type
               if((*((unsigned int*)data) != 0)&& (*((unsigned int*)data) != 2)) //只支持两种速度曲线方式
			   //if(*((unsigned int*)data) != 0)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;
	case 0x60C0:
		   		if(*((unsigned int*)data) != 0)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
		   		break;
	case 0x60C2:
			   if(pODE->subindex == 2)
			   {
				  tmp1 = *((int*)data);
			   	  if(tmp1>=128)
				  {
				  	tmp1 = tmp1-256;
				  }
			   	  if(tmp1 < -4 || tmp1 > 0)//100us~1s
				  {
				  	 return 0x08000021L;
				  }

				  *((int*)data) = tmp1;
			   }
			   else if(pODE->subindex == 1)
			   {
			   	  tmp1 = *((unsigned int*)data);
				  if(tmp1>=256)
				  {
				  		return 0x06090031L;
				  }
				  /*else if((Interpolation_Time_Index==-1 && tmp>1) || (Interpolation_Time_Index==-2 && tmp>10)//must be lower than 100ms
				  		||(Interpolation_Time_Index==-5 && tmp1<25) || (Interpolation_Time_Index==-6 && tmp<256))
				  {
				  		return 0x08000021L;
				  }*/
			   }
			   break;
   
   case 0x60C5://max acc
               if(*((unsigned long*)data) == 0)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x60C6://max dec
               if(*((unsigned long*)data) == 0)
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;

   case 0x6098://Homing_Method 
               tmp = *((unsigned int*)data);
               if((tmp != 1) && (tmp != 2) && (tmp != 3) && (tmp != 4) && (tmp != 17) && (tmp != 18) && (tmp != 19) && (tmp != 20)&& (tmp != 35))
			   {
                  return 0x08000020L;  //Data can not be transferred or stored to the application
			   }
			   break;
  
   }//end switch
   return 0L;
}
#endif

Uint16 Find_char_OD(Uint16 Index,Uint16 Subindex,Uint16 length)
{
	if(length != 2) return 0;
	if(Index > 0x1400 && Index <= 0x1BFF) Index &= 0xFE00;//All PDO
	switch(Index)
	{
		case 0x1001:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x1003:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x1016:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x1018:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x1029:
			return 1;
			//break;
		case 0x1200:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x1280:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x1400:   //mark
			if(!Subindex || (Subindex == 0x02))
			{
				return 1;
			}
			break;
		case 0x1600:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x1800:  //mark
			if(!Subindex || (Subindex == 0x02))
			{
				return 1;
			}
			break;
		case 0x1A00:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x604D:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6060:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6061:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x607E:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6089:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x608A:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x608B:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x608C:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x608D:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x608E:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x608F:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6090:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6091:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6092:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6093:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6094:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6095:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6096:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6097:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6098:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6099:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x60C2:
			if((Subindex == 0x01) || (Subindex == 0x02))
			{
				return 1;
			}
			//break;
		case 0x60C3:
			return 1;
			//break;
		case 0x60C4:
			if(!Subindex ||(Subindex == 0x03) || (Subindex == 0x05) || (Subindex == 0x06))
			{
				return 1;
			}
			break;
		case 0x60F6:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x60F9:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x60FB:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x60FE:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6410:
			if(!Subindex)
			{
				return 1;
			}
			break;
		case 0x6510:
			if(!Subindex)
			{
				return 1;
			}
			break;
		default:;
	}
	return 0;
}
/******************************************************************************/
/*     OBJECT DICTIONARY                                                      */
/* Entries in CO_OD[] must be ordered. If not, disable macro CO_OD_IS_ORDERED */
/******************************************************************************/
//macro for add entry to object dictionary
#define OD_ENTRY(Index, Subindex, Attribute, Variable) \
     {Index, Subindex, Attribute, ((sizeof(Variable))<<1), (ROM void*)&Variable}

ROM CO_objectDictionaryEntry CO_OD[] = {
   OD_ENTRY(0x1000, 0x00, ATTR_RO|ATTR_ROM, ODE_Device_Type),
   OD_ENTRY(0x1001, 0x00, ATTR_RO, ODE_Error_Register),
   OD_ENTRY(0x1002, 0x00, ATTR_RO, ODE_Manufacturer_Status_Register),

   #if CO_NO_ERROR_FIELD > 0
      OD_ENTRY(0x1003, 0x00, ATTR_RW, ODE_Pre_Defined_Error_Field_NoOfErrors),
      OD_ENTRY(0x1003, 0x01, ATTR_RO, ODE_Pre_Defined_Error_Field[0]),
      OD_ENTRY(0x1003, 0x02, ATTR_RO, ODE_Pre_Defined_Error_Field[1]),
      OD_ENTRY(0x1003, 0x03, ATTR_RO, ODE_Pre_Defined_Error_Field[2]),
      OD_ENTRY(0x1003, 0x04, ATTR_RO, ODE_Pre_Defined_Error_Field[3]),
      OD_ENTRY(0x1003, 0x05, ATTR_RO, ODE_Pre_Defined_Error_Field[4]),
      OD_ENTRY(0x1003, 0x06, ATTR_RO, ODE_Pre_Defined_Error_Field[5]),
      OD_ENTRY(0x1003, 0x07, ATTR_RO, ODE_Pre_Defined_Error_Field[6]),
      OD_ENTRY(0x1003, 0x08, ATTR_RO, ODE_Pre_Defined_Error_Field[7]),
   #endif

   #if CO_NO_SYNC > 0
      OD_ENTRY(0x1005, 0x00, ATTR_RW|ATTR_ROM, ODE_SYNC_COB_ID),
      OD_ENTRY(0x1006, 0x00, ATTR_RW|ATTR_ROM, ODE_Communication_Cycle_Period1),
      OD_ENTRY(0x1007, 0x00, ATTR_RW|ATTR_ROM, ODE_Synchronous_Window_Length),
   #endif

   OD_ENTRY(0x1008, 0x00, ATTR_RO|ATTR_ROM, ODE_Manufacturer_Device_Name),
   OD_ENTRY(0x1009, 0x00, ATTR_RO|ATTR_ROM, ODE_Manufacturer_Hardware_Version),
   OD_ENTRY(0x100A, 0x00, ATTR_RO|ATTR_ROM, ODE_Manufacturer_Software_Version),

   #if CO_NO_EMERGENCY > 0
      OD_ENTRY(0x1014, 0x00, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_Emergency_COB_ID),
      OD_ENTRY(0x1015, 0x00, ATTR_RW|ATTR_ROM, ODE_Inhibit_Time_Emergency),
   #endif

   #if CO_NO_CONS_HEARTBEAT > 0
      OD_ENTRY(0x1016, 0x00, ATTR_RO|ATTR_ROM, ODE_Consumer_Heartbeat_Time_NoOfEntries),
      OD_ENTRY(0x1016, 0x01, ATTR_RW|ATTR_ROM, ODE_Consumer_Heartbeat_Time[0]),
      #if CO_NO_CONS_HEARTBEAT > 1
         OD_ENTRY(0x1016, 0x02, ATTR_RW|ATTR_ROM, ODE_Consumer_Heartbeat_Time[1]),
      #endif
      #if CO_NO_CONS_HEARTBEAT > 2
         OD_ENTRY(0x1016, 0x03, ATTR_RW|ATTR_ROM, ODE_Consumer_Heartbeat_Time[2]),
      #endif
      #if CO_NO_CONS_HEARTBEAT > 3
         OD_ENTRY(0x1016, 0x04, ATTR_RW|ATTR_ROM, ODE_Consumer_Heartbeat_Time[3]),
      #endif
      #if CO_NO_CONS_HEARTBEAT > 4
         OD_ENTRY(0x1016, 0x05, ATTR_RW|ATTR_ROM, ODE_Consumer_Heartbeat_Time[4]),
      #endif
      #if CO_NO_CONS_HEARTBEAT > 5
         OD_ENTRY(0x1016, 0x06, ATTR_RW|ATTR_ROM, ODE_Consumer_Heartbeat_Time[5]),
      #endif
      #if CO_NO_CONS_HEARTBEAT > 6
         OD_ENTRY(0x1016, 0x07, ATTR_RW|ATTR_ROM, ODE_Consumer_Heartbeat_Time[6]),
      #endif
      #if CO_NO_CONS_HEARTBEAT > 7
         OD_ENTRY(0x1016, 0x08, ATTR_RW|ATTR_ROM, ODE_Consumer_Heartbeat_Time[7]),
      #endif
   #endif

   OD_ENTRY(0x1017, 0x00, ATTR_RW|ATTR_ROM, ODE_Producer_Heartbeat_Time),
   OD_ENTRY(0x1018, 0x00, ATTR_RO|ATTR_ROM, ODE_Identity.NoOfEntries),
   OD_ENTRY(0x1018, 0x01, ATTR_RO|ATTR_ROM, ODE_Identity.Vendor_ID),
   OD_ENTRY(0x1018, 0x02, ATTR_RO|ATTR_ROM, ODE_Identity.Product_Code),
   OD_ENTRY(0x1018, 0x03, ATTR_RO|ATTR_ROM, ODE_Identity.Revision_Number),
   OD_ENTRY(0x1018, 0x04, ATTR_RO|ATTR_ROM, ODE_Identity.Serial_Number),
   OD_ENTRY(0x1029, 0x00, ATTR_RO|ATTR_ROM, ODE_Error_Behavior.NoOfEntries),
   OD_ENTRY(0x1029, 0x01, ATTR_RW|ATTR_ROM, ODE_Error_Behavior.Communication_Error),

   #if CO_NO_SDO_SERVER > 0
      OD_ENTRY(0x1200, 0x00, ATTR_RO|ATTR_ROM, ODE_Server_SDO_Parameter_NoOfEntries),
      OD_ENTRY(0x1200, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_Server_SDO_Parameter[0].COB_ID_Client_to_Server),
      OD_ENTRY(0x1200, 0x02, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_Server_SDO_Parameter[0].COB_ID_Server_to_Client),
   #endif

   #if CO_NO_SDO_CLIENT > 0
      OD_ENTRY(0x1280, 0x00, ATTR_RO|ATTR_ROM, ODE_Client_SDO_Parameter_NoOfEntries),
      OD_ENTRY(0x1280, 0x01, ATTR_RW, ODE_Client_SDO_Parameter[0].COB_ID_Client_to_Server),
      OD_ENTRY(0x1280, 0x02, ATTR_RW, ODE_Client_SDO_Parameter[0].COB_ID_Server_to_Client),
      OD_ENTRY(0x1280, 0x03, ATTR_RW, ODE_Client_SDO_Parameter[0].NODE_ID_of_SDO_Client_or_Server),
   #endif

   #ifdef CO_PDO_PARAM_IN_OD
   #if CO_NO_RPDO > 0
      OD_ENTRY(0x1400, 0x00, ATTR_RO|ATTR_ROM, ODE_RPDO_Parameter_NoOfEntries),
      #if ODD_RPDO_PAR_COB_ID_0 == 0
         OD_ENTRY(0x1400, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_RPDO_Parameter[0].COB_ID),
      #else
         OD_ENTRY(0x1400, 0x01, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[0].COB_ID),
      #endif
      //2007-7-11
      //OD_ENTRY(0x1400, 0x02, ATTR_RO|ATTR_ROM, ODE_RPDO_Parameter[0].Transmission_type),
   	  OD_ENTRY(0x1400, 0x02, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[0].Transmission_type),
   	  #if SERVO_CONTROL==1
   	  OD_ENTRY(0x1400, 0x03, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[0].Inhibit_Time),//RPDO Inhibit time
   	  OD_ENTRY(0x1400, 0x05, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[0].Event_Timer),//RPDO Event time
   	  #endif
   #endif
   #if CO_NO_RPDO > 1
      OD_ENTRY(0x1401, 0x00, ATTR_RO|ATTR_ROM, ODE_RPDO_Parameter_NoOfEntries),
      #if ODD_RPDO_PAR_COB_ID_1 == 0
         OD_ENTRY(0x1401, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_RPDO_Parameter[1].COB_ID),
      #else
         OD_ENTRY(0x1401, 0x01, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[1].COB_ID),
      #endif
      //OD_ENTRY(0x1401, 0x02, ATTR_RO|ATTR_ROM, ODE_RPDO_Parameter[1].Transmission_type),
   	  OD_ENTRY(0x1401, 0x02, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[1].Transmission_type),
   	  
   	  #if SERVO_CONTROL==1
   	  OD_ENTRY(0x1401, 0x03, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[1].Inhibit_Time),//RPDO Inhibit time
   	  OD_ENTRY(0x1401, 0x05, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[1].Event_Timer),//RPDO Event time
   	  #endif
   #endif
   #if CO_NO_RPDO > 2
      OD_ENTRY(0x1402, 0x00, ATTR_RO|ATTR_ROM, ODE_RPDO_Parameter_NoOfEntries),
      #if ODD_RPDO_PAR_COB_ID_2 == 0
         OD_ENTRY(0x1402, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_RPDO_Parameter[2].COB_ID),
      #else
         OD_ENTRY(0x1402, 0x01, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[2].COB_ID),
      #endif
      OD_ENTRY(0x1402, 0x02, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[2].Transmission_type),
   	  #if SERVO_CONTROL == 1
   	  OD_ENTRY(0x1402, 0x03, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[2].Inhibit_Time),//RPDO Inhibit time
   	  OD_ENTRY(0x1402, 0x05, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[2].Event_Timer),//RPDO Event time
   	  #endif
   #endif
   #if CO_NO_RPDO > 3
      OD_ENTRY(0x1403, 0x00, ATTR_RO|ATTR_ROM, ODE_RPDO_Parameter_NoOfEntries),
      #if ODD_RPDO_PAR_COB_ID_3 == 0
         OD_ENTRY(0x1403, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_RPDO_Parameter[3].COB_ID),
      #else
         OD_ENTRY(0x1403, 0x01, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[3].COB_ID),
      #endif
      OD_ENTRY(0x1403, 0x02, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[3].Transmission_type),
   	  #if SERVO_CONTROL
   	  OD_ENTRY(0x1403, 0x03, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[3].Inhibit_Time),//RPDO Inhibit time
   	  OD_ENTRY(0x1403, 0x05, ATTR_RW|ATTR_ROM, ODE_RPDO_Parameter[3].Event_Timer),//RPDO Event time
   	  #endif
   #endif
   #endif

   #ifdef CO_PDO_MAPPING_IN_OD
   #if CO_NO_RPDO > 0	//ro;	rw: if dynamic mapping is supported
      OD_ENTRY(0x1600, 0x00, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping_NoOfEntries[0]),
      OD_ENTRY(0x1600, 0x01, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[0][0]),
      OD_ENTRY(0x1600, 0x02, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[0][1]),
      OD_ENTRY(0x1600, 0x03, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[0][2]),
      OD_ENTRY(0x1600, 0x04, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[0][3]),
      OD_ENTRY(0x1600, 0x05, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[0][4]),
      OD_ENTRY(0x1600, 0x06, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[0][5]),
      OD_ENTRY(0x1600, 0x07, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[0][6]),
      OD_ENTRY(0x1600, 0x08, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[0][7]),
   #endif
   #if CO_NO_RPDO > 1
      OD_ENTRY(0x1601, 0x00, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping_NoOfEntries[1]),
      OD_ENTRY(0x1601, 0x01, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[1][0]),
      OD_ENTRY(0x1601, 0x02, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[1][1]),
      OD_ENTRY(0x1601, 0x03, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[1][2]),
      OD_ENTRY(0x1601, 0x04, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[1][3]),
      OD_ENTRY(0x1601, 0x05, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[1][4]),
      OD_ENTRY(0x1601, 0x06, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[1][5]),
      OD_ENTRY(0x1601, 0x07, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[1][6]),
      OD_ENTRY(0x1601, 0x08, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[1][7]),
   #endif
   #if CO_NO_RPDO > 2
      OD_ENTRY(0x1602, 0x00, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping_NoOfEntries[2]),
      OD_ENTRY(0x1602, 0x01, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[2][0]),
      OD_ENTRY(0x1602, 0x02, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[2][1]),
      OD_ENTRY(0x1602, 0x03, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[2][2]),
      OD_ENTRY(0x1602, 0x04, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[2][3]),
      OD_ENTRY(0x1602, 0x05, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[2][4]),
      OD_ENTRY(0x1602, 0x06, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[2][5]),
      OD_ENTRY(0x1602, 0x07, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[2][6]),
      OD_ENTRY(0x1602, 0x08, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[2][7]),
   #endif
   #if CO_NO_RPDO > 3
      OD_ENTRY(0x1603, 0x00, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping_NoOfEntries[3]),
      OD_ENTRY(0x1603, 0x01, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[3][0]),
      OD_ENTRY(0x1603, 0x02, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[3][1]),
      OD_ENTRY(0x1603, 0x03, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[3][2]),
      OD_ENTRY(0x1603, 0x04, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[3][3]),
      OD_ENTRY(0x1603, 0x05, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[3][4]),
      OD_ENTRY(0x1603, 0x06, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[3][5]),
      OD_ENTRY(0x1603, 0x07, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[3][6]),
      OD_ENTRY(0x1603, 0x08, ATTR_RW|ATTR_ROM, ODE_RPDO_Mapping[3][7]),
   #endif
   #endif

   #ifdef CO_PDO_PARAM_IN_OD
   #if CO_NO_TPDO > 0
      OD_ENTRY(0x1800, 0x00, ATTR_RO|ATTR_ROM, ODE_TPDO_Parameter_NoOfEntries),
      #if ODD_TPDO_PAR_COB_ID_0 == 0
         OD_ENTRY(0x1800, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_TPDO_Parameter[0].COB_ID),
      #else
         OD_ENTRY(0x1800, 0x01, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[0].COB_ID),
      #endif
	  OD_ENTRY(0x1800, 0x02, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[0].Transmission_type),
      #ifdef CO_TPDO_INH_EV_TIMER
         OD_ENTRY(0x1800, 0x03, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[0].Inhibit_Time),
         OD_ENTRY(0x1800, 0x05, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[0].Event_Timer),
      #endif
   #endif
   #if CO_NO_TPDO > 1
      OD_ENTRY(0x1801, 0x00, ATTR_RO|ATTR_ROM, ODE_TPDO_Parameter_NoOfEntries),
      #if ODD_TPDO_PAR_COB_ID_1 == 0
         OD_ENTRY(0x1801, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_TPDO_Parameter[1].COB_ID),
      #else
         OD_ENTRY(0x1801, 0x01, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[1].COB_ID),
      #endif
      OD_ENTRY(0x1801, 0x02, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[1].Transmission_type),
      #ifdef CO_TPDO_INH_EV_TIMER
         OD_ENTRY(0x1801, 0x03, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[1].Inhibit_Time),
         OD_ENTRY(0x1801, 0x05, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[1].Event_Timer),
      #endif
   #endif
   #if CO_NO_TPDO > 2
      OD_ENTRY(0x1802, 0x00, ATTR_RO|ATTR_ROM, ODE_TPDO_Parameter_NoOfEntries),
      #if ODD_TPDO_PAR_COB_ID_2 == 0
         OD_ENTRY(0x1802, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_TPDO_Parameter[2].COB_ID),
      #else
         OD_ENTRY(0x1802, 0x01, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[2].COB_ID),
      #endif
      OD_ENTRY(0x1802, 0x02, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[2].Transmission_type),
      #ifdef CO_TPDO_INH_EV_TIMER
         OD_ENTRY(0x1802, 0x03, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[2].Inhibit_Time),
         OD_ENTRY(0x1802, 0x05, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[2].Event_Timer),
      #endif
   #endif
   #if CO_NO_TPDO > 3
      OD_ENTRY(0x1803, 0x00, ATTR_RO|ATTR_ROM, ODE_TPDO_Parameter_NoOfEntries),
      #if ODD_TPDO_PAR_COB_ID_3 == 0
         OD_ENTRY(0x1803, 0x01, ATTR_RO|ATTR_ROM|ATTR_ADD_ID, ODE_TPDO_Parameter[3].COB_ID),
      #else
         OD_ENTRY(0x1803, 0x01, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[3].COB_ID),
      #endif
      OD_ENTRY(0x1803, 0x02, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[3].Transmission_type),
      #ifdef CO_TPDO_INH_EV_TIMER
         OD_ENTRY(0x1803, 0x03, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[3].Inhibit_Time),
         OD_ENTRY(0x1803, 0x05, ATTR_RW|ATTR_ROM, ODE_TPDO_Parameter[3].Event_Timer),
      #endif
   #endif
   #endif

   #ifdef CO_PDO_MAPPING_IN_OD
   #if CO_NO_TPDO > 0		//ro;	rw: if dynamic mapping is supported
      OD_ENTRY(0x1A00, 0x00, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping_NoOfEntries[0]),
      OD_ENTRY(0x1A00, 0x01, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[0][0]),
      OD_ENTRY(0x1A00, 0x02, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[0][1]),
      OD_ENTRY(0x1A00, 0x03, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[0][2]),
      OD_ENTRY(0x1A00, 0x04, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[0][3]),
      OD_ENTRY(0x1A00, 0x05, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[0][4]),
      OD_ENTRY(0x1A00, 0x06, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[0][5]),
      OD_ENTRY(0x1A00, 0x07, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[0][6]),
      OD_ENTRY(0x1A00, 0x08, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[0][7]),
   #endif
   #if CO_NO_TPDO > 1
      OD_ENTRY(0x1A01, 0x00, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping_NoOfEntries[1]),
      OD_ENTRY(0x1A01, 0x01, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[1][0]),
      OD_ENTRY(0x1A01, 0x02, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[1][1]),
      OD_ENTRY(0x1A01, 0x03, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[1][2]),
      OD_ENTRY(0x1A01, 0x04, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[1][3]),
      OD_ENTRY(0x1A01, 0x05, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[1][4]),
      OD_ENTRY(0x1A01, 0x06, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[1][5]),
      OD_ENTRY(0x1A01, 0x07, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[1][6]),
      OD_ENTRY(0x1A01, 0x08, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[1][7]),
   #endif
   #if CO_NO_TPDO > 2
      OD_ENTRY(0x1A02, 0x00, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping_NoOfEntries[2]),
      OD_ENTRY(0x1A02, 0x01, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[2][0]),
      OD_ENTRY(0x1A02, 0x02, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[2][1]),
      OD_ENTRY(0x1A02, 0x03, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[2][2]),
      OD_ENTRY(0x1A02, 0x04, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[2][3]),
      OD_ENTRY(0x1A02, 0x05, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[2][4]),
      OD_ENTRY(0x1A02, 0x06, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[2][5]),
      OD_ENTRY(0x1A02, 0x07, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[2][6]),
      OD_ENTRY(0x1A02, 0x08, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[2][7]),
   #endif
   #if CO_NO_TPDO > 3
      OD_ENTRY(0x1A03, 0x00, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping_NoOfEntries[3]),
      OD_ENTRY(0x1A03, 0x01, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[3][0]),
      OD_ENTRY(0x1A03, 0x02, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[3][1]),
      OD_ENTRY(0x1A03, 0x03, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[3][2]),
      OD_ENTRY(0x1A03, 0x04, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[3][3]),
      OD_ENTRY(0x1A03, 0x05, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[3][4]),
      OD_ENTRY(0x1A03, 0x06, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[3][5]),
      OD_ENTRY(0x1A03, 0x07, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[3][6]),
      OD_ENTRY(0x1A03, 0x08, ATTR_RW|ATTR_ROM, ODE_TPDO_Mapping[3][7]),
   #endif
   #endif

   OD_ENTRY(0x1F80, 0x00, ATTR_RW|ATTR_ROM, ODE_NMT_Startup),

/***** Manufacturer specific **************************************************/
   OD_ENTRY(0x2000, 0x00, ATTR_RO|ATTR_ROM, ODE_TPDO1_MASK_NoOfEntries),
   OD_ENTRY(0x2000, 0x01, ATTR_RW, ODE_TPDO_MASK[0][0]),
   OD_ENTRY(0x2000, 0x02, ATTR_RW, ODE_TPDO_MASK[0][1]),

   OD_ENTRY(0x2001, 0x00, ATTR_RO|ATTR_ROM, ODE_TPDO2_MASK_NoOfEntries),
   OD_ENTRY(0x2001, 0x01, ATTR_RW, ODE_TPDO_MASK[1][0]),
   OD_ENTRY(0x2001, 0x02, ATTR_RW, ODE_TPDO_MASK[1][1]),

   OD_ENTRY(0x2002, 0x00, ATTR_RO|ATTR_ROM, ODE_TPDO3_MASK_NoOfEntries),
   OD_ENTRY(0x2002, 0x01, ATTR_RW, ODE_TPDO_MASK[2][0]),
   OD_ENTRY(0x2002, 0x02, ATTR_RW, ODE_TPDO_MASK[2][1]),

   OD_ENTRY(0x2003, 0x00, ATTR_RO|ATTR_ROM, ODE_TPDO4_MASK_NoOfEntries),
   OD_ENTRY(0x2003, 0x01, ATTR_RW, ODE_TPDO_MASK[3][0]),
   OD_ENTRY(0x2003, 0x02, ATTR_RW, ODE_TPDO_MASK[3][1]),

   OD_ENTRY(0x2045, 0x00, ATTR_RW, homing_timeout),//

   #if CO_NO_SYNC > 0
      OD_ENTRY(0x2103, 0x00, ATTR_RW, CO_SYNCcounter),
      OD_ENTRY(0x2104, 0x00, ATTR_RO, CO_SYNCtime),
	  OD_ENTRY(0x2105, 0x00, ATTR_RW, ODE_Communication_Cycle_Period),
   #endif

   

/***** Manufacturer profile****************************************************/
/* 参数数据（读写）*/
	OD_ENTRY(0x3000, 0x00, ATTR_RW, Pn[0]),      
	OD_ENTRY(0x3001, 0x00, ATTR_RW, Pn[1]),
	OD_ENTRY(0x3002, 0x00, ATTR_RW, Pn[2]),
	OD_ENTRY(0x3003, 0x00, ATTR_RW, Pn[3]),      
	OD_ENTRY(0x3004, 0x00, ATTR_RW, Pn[4]),
	OD_ENTRY(0x3005, 0x00, ATTR_RW, Pn[5]),
	OD_ENTRY(0x3006, 0x00, ATTR_RW, Pn[6]),      
	OD_ENTRY(0x3007, 0x00, ATTR_RW, Pn[7]),
	OD_ENTRY(0x3008, 0x00, ATTR_RW, Pn[8]),
	OD_ENTRY(0x3009, 0x00, ATTR_RW, Pn[9]),

	OD_ENTRY(0x300A, 0x00, ATTR_RW, Pn[10]),
	OD_ENTRY(0x300B, 0x00, ATTR_RW, Pn[11]),      
	OD_ENTRY(0x300C, 0x00, ATTR_RW, Pn[12]),
	OD_ENTRY(0x300D, 0x00, ATTR_RW, Pn[13]),
	OD_ENTRY(0x300E, 0x00, ATTR_RW, Pn[14]),      
	OD_ENTRY(0x300F, 0x00, ATTR_RW, Pn[15]),
	OD_ENTRY(0x3010, 0x00, ATTR_RW, Pn[16]),
	OD_ENTRY(0x3011, 0x00, ATTR_RW, Pn[17]),      
	OD_ENTRY(0x3012, 0x00, ATTR_RW, Pn[18]),
	OD_ENTRY(0x3013, 0x00, ATTR_RW, Pn[19]),

	OD_ENTRY(0x3014, 0x00, ATTR_RW, Pn[20]),
	OD_ENTRY(0x3015, 0x00, ATTR_RW, Pn[21]),      
	OD_ENTRY(0x3016, 0x00, ATTR_RW, Pn[22]),
	OD_ENTRY(0x3017, 0x00, ATTR_RW, Pn[23]),
	OD_ENTRY(0x3018, 0x00, ATTR_RW, Pn[24]),      
	OD_ENTRY(0x3019, 0x00, ATTR_RW, Pn[25]),
	OD_ENTRY(0x301A, 0x00, ATTR_RW, Pn[26]),
	OD_ENTRY(0x301B, 0x00, ATTR_RW, Pn[27]),      
	OD_ENTRY(0x301C, 0x00, ATTR_RW, Pn[28]),
	OD_ENTRY(0x301D, 0x00, ATTR_RW, Pn[29]),

	OD_ENTRY(0x301E, 0x00, ATTR_RW, Pn[30]),
	OD_ENTRY(0x301F, 0x00, ATTR_RW, Pn[31]),      
	OD_ENTRY(0x3020, 0x00, ATTR_RW, Pn[32]),
	OD_ENTRY(0x3021, 0x00, ATTR_RW, Pn[33]),
	OD_ENTRY(0x3022, 0x00, ATTR_RW, Pn[34]),      
	OD_ENTRY(0x3023, 0x00, ATTR_RW, Pn[35]),
	OD_ENTRY(0x3024, 0x00, ATTR_RW, Pn[36]),
	OD_ENTRY(0x3025, 0x00, ATTR_RW, Pn[37]),      
	OD_ENTRY(0x3026, 0x00, ATTR_RW, Pn[38]),
	OD_ENTRY(0x3027, 0x00, ATTR_RW, Pn[39]),

	OD_ENTRY(0x3028, 0x00, ATTR_RW, Pn[40]),
	OD_ENTRY(0x3029, 0x00, ATTR_RW, Pn[41]),      
	OD_ENTRY(0x302A, 0x00, ATTR_RW, Pn[42]),
	OD_ENTRY(0x302B, 0x00, ATTR_RW, Pn[43]),
	OD_ENTRY(0x302C, 0x00, ATTR_RW, Pn[44]),      
	OD_ENTRY(0x302D, 0x00, ATTR_RW, Pn[45]),
	OD_ENTRY(0x302E, 0x00, ATTR_RW, Pn[46]),
	OD_ENTRY(0x302F, 0x00, ATTR_RW, Pn[47]),      
	OD_ENTRY(0x3030, 0x00, ATTR_RW, Pn[48]),
	OD_ENTRY(0x3031, 0x00, ATTR_RW, Pn[49]),

	OD_ENTRY(0x3032, 0x00, ATTR_RW, Pn[50]),
	OD_ENTRY(0x3033, 0x00, ATTR_RW, Pn[51]),      
	OD_ENTRY(0x3034, 0x00, ATTR_RW, Pn[52]),
	OD_ENTRY(0x3035, 0x00, ATTR_RW, Pn[53]),
	OD_ENTRY(0x3036, 0x00, ATTR_RW, Pn[54]),      
	OD_ENTRY(0x3037, 0x00, ATTR_RW, Pn[55]),
	OD_ENTRY(0x3038, 0x00, ATTR_RW, Pn[56]),
	OD_ENTRY(0x3039, 0x00, ATTR_RW, Pn[57]),      
	OD_ENTRY(0x303A, 0x00, ATTR_RW, Pn[58]),
	OD_ENTRY(0x303B, 0x00, ATTR_RW, Pn[59]),

	OD_ENTRY(0x303C, 0x00, ATTR_RW, Pn[60]),
	OD_ENTRY(0x303D, 0x00, ATTR_RW, Pn[61]),      
	OD_ENTRY(0x303E, 0x00, ATTR_RW, Pn[62]),
	OD_ENTRY(0x303F, 0x00, ATTR_RW, Pn[63]),
	OD_ENTRY(0x3040, 0x00, ATTR_RW, Pn[64]),      
	OD_ENTRY(0x3041, 0x00, ATTR_RW, Pn[65]),
	OD_ENTRY(0x3042, 0x00, ATTR_RW, Pn[66]),
	OD_ENTRY(0x3043, 0x00, ATTR_RW, Pn[67]),      
	OD_ENTRY(0x3044, 0x00, ATTR_RW, Pn[68]),
	OD_ENTRY(0x3045, 0x00, ATTR_RW, Pn[69]),

	OD_ENTRY(0x3046, 0x00, ATTR_RW, Pn[70]),
	OD_ENTRY(0x3047, 0x00, ATTR_RW, Pn[71]),      
	OD_ENTRY(0x3048, 0x00, ATTR_RW, Pn[72]),
	OD_ENTRY(0x3049, 0x00, ATTR_RW, Pn[73]),
	OD_ENTRY(0x304A, 0x00, ATTR_RW, Pn[74]),      
	OD_ENTRY(0x304B, 0x00, ATTR_RW, Pn[75]),
	OD_ENTRY(0x304C, 0x00, ATTR_RW, Pn[76]),
	OD_ENTRY(0x304D, 0x00, ATTR_RW, Pn[77]),      
	OD_ENTRY(0x304E, 0x00, ATTR_RW, Pn[78]),
	OD_ENTRY(0x304F, 0x00, ATTR_RW, Pn[79]),

	OD_ENTRY(0x3050, 0x00, ATTR_RW, Pn[80]),
	OD_ENTRY(0x3051, 0x00, ATTR_RW, Pn[81]),      
	OD_ENTRY(0x3052, 0x00, ATTR_RW, Pn[82]),
	OD_ENTRY(0x3053, 0x00, ATTR_RW, Pn[83]),
	OD_ENTRY(0x3054, 0x00, ATTR_RW, Pn[84]),      
	OD_ENTRY(0x3055, 0x00, ATTR_RW, Pn[85]),
	OD_ENTRY(0x3056, 0x00, ATTR_RW, Pn[86]),
	OD_ENTRY(0x3057, 0x00, ATTR_RW, Pn[87]),      
	OD_ENTRY(0x3058, 0x00, ATTR_RW, Pn[88]),
	OD_ENTRY(0x3059, 0x00, ATTR_RW, Pn[89]),

	OD_ENTRY(0x305A, 0x00, ATTR_RW, Pn[90]),
	OD_ENTRY(0x305B, 0x00, ATTR_RW, Pn[91]),      
	OD_ENTRY(0x305C, 0x00, ATTR_RW, Pn[92]),
	OD_ENTRY(0x305D, 0x00, ATTR_RW, Pn[93]),
	OD_ENTRY(0x305E, 0x00, ATTR_RW, Pn[94]),      
	OD_ENTRY(0x305F, 0x00, ATTR_RW, Pn[95]),
	OD_ENTRY(0x3060, 0x00, ATTR_RW, Pn[96]),
	OD_ENTRY(0x3061, 0x00, ATTR_RW, Pn[97]),      
	OD_ENTRY(0x3062, 0x00, ATTR_RW, Pn[98]),
	OD_ENTRY(0x3063, 0x00, ATTR_RW, Pn[99]),

	OD_ENTRY(0x3064, 0x00, ATTR_RW, Pn[100]),
	OD_ENTRY(0x3065, 0x00, ATTR_RW, Pn[101]),      
	OD_ENTRY(0x3066, 0x00, ATTR_RW, Pn[102]),
	OD_ENTRY(0x3067, 0x00, ATTR_RW, Pn[103]),
	OD_ENTRY(0x3068, 0x00, ATTR_RW, Pn[104]),      
	OD_ENTRY(0x3069, 0x00, ATTR_RW, Pn[105]),
	OD_ENTRY(0x306A, 0x00, ATTR_RW, Pn[106]),
	OD_ENTRY(0x306B, 0x00, ATTR_RW, Pn[107]),      
	OD_ENTRY(0x306C, 0x00, ATTR_RW, Pn[108]),
	OD_ENTRY(0x306D, 0x00, ATTR_RW, Pn[109]),

	OD_ENTRY(0x306E, 0x00, ATTR_RW, Pn[110]),
	OD_ENTRY(0x306F, 0x00, ATTR_RW, Pn[111]),      
	OD_ENTRY(0x3070, 0x00, ATTR_RW, Pn[112]),
	OD_ENTRY(0x3071, 0x00, ATTR_RW, Pn[113]),
	OD_ENTRY(0x3072, 0x00, ATTR_RW, Pn[114]),      
	OD_ENTRY(0x3073, 0x00, ATTR_RW, Pn[115]),
	OD_ENTRY(0x3074, 0x00, ATTR_RW, Pn[116]),
	OD_ENTRY(0x3075, 0x00, ATTR_RW, Pn[117]),      
	OD_ENTRY(0x3076, 0x00, ATTR_RW, Pn[118]),
	OD_ENTRY(0x3077, 0x00, ATTR_RW, Pn[119]),

	OD_ENTRY(0x3078, 0x00, ATTR_RW, Pn[120]),
	OD_ENTRY(0x3079, 0x00, ATTR_RW, Pn[121]),      
	OD_ENTRY(0x307A, 0x00, ATTR_RW, Pn[122]),
	OD_ENTRY(0x307B, 0x00, ATTR_RW, Pn[123]),
	OD_ENTRY(0x307C, 0x00, ATTR_RW, Pn[124]),      
	OD_ENTRY(0x307D, 0x00, ATTR_RW, Pn[125]),
	OD_ENTRY(0x307E, 0x00, ATTR_RW, Pn[126]),
	OD_ENTRY(0x307F, 0x00, ATTR_RW, Pn[127]),      
	OD_ENTRY(0x3080, 0x00, ATTR_RW, Pn[128]),
	OD_ENTRY(0x3081, 0x00, ATTR_RW, Pn[129]),

	OD_ENTRY(0x3082, 0x00, ATTR_RW, Pn[130]),
	OD_ENTRY(0x3083, 0x00, ATTR_RW, Pn[131]),      
	OD_ENTRY(0x3084, 0x00, ATTR_RW, Pn[132]),
	OD_ENTRY(0x3085, 0x00, ATTR_RW, Pn[133]),
	OD_ENTRY(0x3086, 0x00, ATTR_RW, Pn[134]),      
	OD_ENTRY(0x3087, 0x00, ATTR_RW, Pn[135]),
	OD_ENTRY(0x3088, 0x00, ATTR_RW, Pn[136]),
	OD_ENTRY(0x3089, 0x00, ATTR_RW, Pn[137]),      
	OD_ENTRY(0x308A, 0x00, ATTR_RW, Pn[138]),
	OD_ENTRY(0x308B, 0x00, ATTR_RW, Pn[139]),

	OD_ENTRY(0x308C, 0x00, ATTR_RW, Pn[140]),
	OD_ENTRY(0x308D, 0x00, ATTR_RW, Pn[141]),      
	OD_ENTRY(0x308E, 0x00, ATTR_RW, Pn[142]),
	OD_ENTRY(0x308F, 0x00, ATTR_RW, Pn[143]),
	OD_ENTRY(0x3090, 0x00, ATTR_RW, Pn[144]),      
	OD_ENTRY(0x3091, 0x00, ATTR_RW, Pn[145]),
	OD_ENTRY(0x3092, 0x00, ATTR_RW, Pn[146]),
	OD_ENTRY(0x3093, 0x00, ATTR_RW, Pn[147]),      
	OD_ENTRY(0x3094, 0x00, ATTR_RW, Pn[148]),
	OD_ENTRY(0x3095, 0x00, ATTR_RW, Pn[149]),

	OD_ENTRY(0x3096, 0x00, ATTR_RW, Pn[150]),
	OD_ENTRY(0x3097, 0x00, ATTR_RW, Pn[151]),      
	OD_ENTRY(0x3098, 0x00, ATTR_RW, Pn[152]),
	OD_ENTRY(0x3099, 0x00, ATTR_RW, Pn[153]),
	OD_ENTRY(0x309A, 0x00, ATTR_RW, Pn[154]),      
	OD_ENTRY(0x309B, 0x00, ATTR_RW, Pn[155]),
	OD_ENTRY(0x309C, 0x00, ATTR_RW, Pn[156]),
	OD_ENTRY(0x309D, 0x00, ATTR_RW, Pn[157]),      
	OD_ENTRY(0x309E, 0x00, ATTR_RW, Pn[158]),
	OD_ENTRY(0x309F, 0x00, ATTR_RW, Pn[159]),

	OD_ENTRY(0x30A0, 0x00, ATTR_RW, Pn[160]),
	OD_ENTRY(0x30A1, 0x00, ATTR_RW, Pn[161]),      
	OD_ENTRY(0x30A2, 0x00, ATTR_RW, Pn[162]),
	OD_ENTRY(0x30A3, 0x00, ATTR_RW, Pn[163]),
	OD_ENTRY(0x30A4, 0x00, ATTR_RW, Pn[164]),      
	OD_ENTRY(0x30A5, 0x00, ATTR_RW, Pn[165]),
	OD_ENTRY(0x30A6, 0x00, ATTR_RW, Pn[166]),
	OD_ENTRY(0x30A7, 0x00, ATTR_RW, Pn[167]),      
	OD_ENTRY(0x30A8, 0x00, ATTR_RW, Pn[168]),
	OD_ENTRY(0x30A9, 0x00, ATTR_RW, Pn[169]),

	OD_ENTRY(0x30AA, 0x00, ATTR_RW, Pn[170]),
	OD_ENTRY(0x30AB, 0x00, ATTR_RW, Pn[171]),      
	OD_ENTRY(0x30AC, 0x00, ATTR_RW, Pn[172]),
	OD_ENTRY(0x30AD, 0x00, ATTR_RW, Pn[173]),
	OD_ENTRY(0x30AE, 0x00, ATTR_RW, Pn[174]),      
	OD_ENTRY(0x30AF, 0x00, ATTR_RW, Pn[175]),
	OD_ENTRY(0x30B0, 0x00, ATTR_RW, Pn[176]),
	OD_ENTRY(0x30B1, 0x00, ATTR_RW, Pn[177]),      
	OD_ENTRY(0x30B2, 0x00, ATTR_RW, Pn[178]),
	OD_ENTRY(0x30B3, 0x00, ATTR_RW, Pn[179]),

	OD_ENTRY(0x30B4, 0x00, ATTR_RW, Pn[180]),
	OD_ENTRY(0x30B5, 0x00, ATTR_RW, Pn[181]),      
	OD_ENTRY(0x30B6, 0x00, ATTR_RW, Pn[182]),
	OD_ENTRY(0x30B7, 0x00, ATTR_RW, Pn[183]),
	OD_ENTRY(0x30B8, 0x00, ATTR_RW, Pn[184]),      
	OD_ENTRY(0x30B9, 0x00, ATTR_RW, Pn[185]),
	OD_ENTRY(0x30BA, 0x00, ATTR_RW, Pn[186]),
	OD_ENTRY(0x30BB, 0x00, ATTR_RW, Pn[187]),      
	OD_ENTRY(0x30BC, 0x00, ATTR_RW, Pn[188]),
	OD_ENTRY(0x30BD, 0x00, ATTR_RW, Pn[189]),

	OD_ENTRY(0x30BE, 0x00, ATTR_RW, Pn[190]),
	OD_ENTRY(0x30BF, 0x00, ATTR_RW, Pn[191]),      
	OD_ENTRY(0x30C0, 0x00, ATTR_RW, Pn[192]),
	OD_ENTRY(0x30C1, 0x00, ATTR_RW, Pn[193]),
	OD_ENTRY(0x30C2, 0x00, ATTR_RW, Pn[194]),      
	OD_ENTRY(0x30C3, 0x00, ATTR_RW, Pn[195]),
	OD_ENTRY(0x30C4, 0x00, ATTR_RW, Pn[196]),
	OD_ENTRY(0x30C5, 0x00, ATTR_RW, Pn[197]),      
	OD_ENTRY(0x30C6, 0x00, ATTR_RW, Pn[198]),
	OD_ENTRY(0x30C7, 0x00, ATTR_RW, Pn[199]),

	OD_ENTRY(0x30C8, 0x00, ATTR_RW, Pn[200]),
	OD_ENTRY(0x30C9, 0x00, ATTR_RW, Pn[201]),      
	OD_ENTRY(0x30CA, 0x00, ATTR_RW, Pn[202]),
	OD_ENTRY(0x30CB, 0x00, ATTR_RW, Pn[203]),
	OD_ENTRY(0x30CC, 0x00, ATTR_RW, Pn[204]),      
	OD_ENTRY(0x30CD, 0x00, ATTR_RW, Pn[205]),
	OD_ENTRY(0x30CE, 0x00, ATTR_RW, Pn[206]),
	OD_ENTRY(0x30CF, 0x00, ATTR_RW, Pn[207]),      
	OD_ENTRY(0x30D0, 0x00, ATTR_RW, Pn[208]),
	OD_ENTRY(0x30D1, 0x00, ATTR_RW, Pn[209]),

	OD_ENTRY(0x30D2, 0x00, ATTR_RW, Pn[210]),
	OD_ENTRY(0x30D3, 0x00, ATTR_RW, Pn[211]),      
	OD_ENTRY(0x30D4, 0x00, ATTR_RW, Pn[212]),
	OD_ENTRY(0x30D5, 0x00, ATTR_RW, Pn[213]),
	OD_ENTRY(0x30D6, 0x00, ATTR_RW, Pn[214]),      
	OD_ENTRY(0x30D7, 0x00, ATTR_RW, Pn[215]),
	OD_ENTRY(0x30D8, 0x00, ATTR_RW, Pn[216]),
	OD_ENTRY(0x30D9, 0x00, ATTR_RW, Pn[217]),      
	OD_ENTRY(0x30DA, 0x00, ATTR_RW, Pn[218]),
	OD_ENTRY(0x30DB, 0x00, ATTR_RW, Pn[219]),

	OD_ENTRY(0x30DC, 0x00, ATTR_RW, Pn[220]),
	OD_ENTRY(0x30DD, 0x00, ATTR_RW, Pn[221]),      
	OD_ENTRY(0x30DE, 0x00, ATTR_RW, Pn[222]),
	OD_ENTRY(0x30DF, 0x00, ATTR_RW, Pn[223]),
	OD_ENTRY(0x30E0, 0x00, ATTR_RW, Pn[224]),      
	OD_ENTRY(0x30E1, 0x00, ATTR_RW, Pn[225]),
	OD_ENTRY(0x30E2, 0x00, ATTR_RW, Pn[226]),
	OD_ENTRY(0x30E3, 0x00, ATTR_RW, Pn[227]),      
	OD_ENTRY(0x30E4, 0x00, ATTR_RW, Pn[228]),
	OD_ENTRY(0x30E5, 0x00, ATTR_RW, Pn[229]),

	OD_ENTRY(0x30E6, 0x00, ATTR_RW, Pn[230]),
	OD_ENTRY(0x30E7, 0x00, ATTR_RW, Pn[231]),      
	OD_ENTRY(0x30E8, 0x00, ATTR_RW, Pn[232]),
	OD_ENTRY(0x30E9, 0x00, ATTR_RW, Pn[233]),
	OD_ENTRY(0x30EA, 0x00, ATTR_RW, Pn[234]),      
	OD_ENTRY(0x30EB, 0x00, ATTR_RW, Pn[235]),
	OD_ENTRY(0x30EC, 0x00, ATTR_RW, Pn[236]),
	OD_ENTRY(0x30ED, 0x00, ATTR_RW, Pn[237]),      
	OD_ENTRY(0x30EE, 0x00, ATTR_RW, Pn[238]),
	OD_ENTRY(0x30EF, 0x00, ATTR_RW, Pn[239]),

	OD_ENTRY(0x30F0, 0x00, ATTR_RW, Pn[240]),
	OD_ENTRY(0x30F1, 0x00, ATTR_RW, Pn[241]),
	OD_ENTRY(0x30F2, 0x00, ATTR_RW, Pn[242]),
	OD_ENTRY(0x30F3, 0x00, ATTR_RW, Pn[243]),
	OD_ENTRY(0x30F4, 0x00, ATTR_RW, Pn[244]),
	OD_ENTRY(0x30F5, 0x00, ATTR_RW, Pn[245]),
	OD_ENTRY(0x30F6, 0x00, ATTR_RW, Pn[246]),
	OD_ENTRY(0x30F7, 0x00, ATTR_RW, Pn[247]),
	OD_ENTRY(0x30F8, 0x00, ATTR_RW, Pn[248]),
	OD_ENTRY(0x30F9, 0x00, ATTR_RW, Pn[249]),

	OD_ENTRY(0x30FA, 0x00, ATTR_RW, Pn[250]),
	OD_ENTRY(0x30FB, 0x00, ATTR_RW, Pn[251]),
	OD_ENTRY(0x30FC, 0x00, ATTR_RW, Pn[252]),
	OD_ENTRY(0x30FD, 0x00, ATTR_RW, Pn[253]),
	OD_ENTRY(0x30FE, 0x00, ATTR_RW, Pn[254]),
	OD_ENTRY(0x30FF, 0x00, ATTR_RW, Pn[255]),
	OD_ENTRY(0x3100, 0x00, ATTR_RW, Pn[256]),
	OD_ENTRY(0x3101, 0x00, ATTR_RW, Pn[257]),
	OD_ENTRY(0x3102, 0x00, ATTR_RW, Pn[258]),
	OD_ENTRY(0x3103, 0x00, ATTR_RW, Pn[259]),

	OD_ENTRY(0x3104, 0x00, ATTR_RW, Pn[260]),
	OD_ENTRY(0x3105, 0x00, ATTR_RW, Pn[261]),
	OD_ENTRY(0x3106, 0x00, ATTR_RW, Pn[262]),
	OD_ENTRY(0x3107, 0x00, ATTR_RW, Pn[263]),
	OD_ENTRY(0x3108, 0x00, ATTR_RW, Pn[264]),
	OD_ENTRY(0x3109, 0x00, ATTR_RW, Pn[265]),
	OD_ENTRY(0x310A, 0x00, ATTR_RW, Pn[266]),
	OD_ENTRY(0x310B, 0x00, ATTR_RW, Pn[267]),
	OD_ENTRY(0x310C, 0x00, ATTR_RW, Pn[268]),
	OD_ENTRY(0x310D, 0x00, ATTR_RW, Pn[269]),

	OD_ENTRY(0x310E, 0x00, ATTR_RW, Pn[270]),
	OD_ENTRY(0x310F, 0x00, ATTR_RW, Pn[271]),
	OD_ENTRY(0x3110, 0x00, ATTR_RW, Pn[272]),
	OD_ENTRY(0x3111, 0x00, ATTR_RW, Pn[273]),
	OD_ENTRY(0x3112, 0x00, ATTR_RW, Pn[274]),
	OD_ENTRY(0x3113, 0x00, ATTR_RW, Pn[275]),
	OD_ENTRY(0x3114, 0x00, ATTR_RW, Pn[276]),
	OD_ENTRY(0x3115, 0x00, ATTR_RW, Pn[277]),
	OD_ENTRY(0x3116, 0x00, ATTR_RW, Pn[278]),
	OD_ENTRY(0x3117, 0x00, ATTR_RW, Pn[279]),

	OD_ENTRY(0x3118, 0x00, ATTR_RW, Pn[280]),
	OD_ENTRY(0x3119, 0x00, ATTR_RW, Pn[281]),
	OD_ENTRY(0x311A, 0x00, ATTR_RW, Pn[282]),
	OD_ENTRY(0x311B, 0x00, ATTR_RW, Pn[283]),
	OD_ENTRY(0x311C, 0x00, ATTR_RW, Pn[284]),
	OD_ENTRY(0x311D, 0x00, ATTR_RW, Pn[285]),
	OD_ENTRY(0x311E, 0x00, ATTR_RW, Pn[286]),
	OD_ENTRY(0x311F, 0x00, ATTR_RW, Pn[287]),
	OD_ENTRY(0x3120, 0x00, ATTR_RW, Pn[288]),
	OD_ENTRY(0x3121, 0x00, ATTR_RW, Pn[289]),

	OD_ENTRY(0x3122, 0x00, ATTR_RW, Pn[290]),
	OD_ENTRY(0x3123, 0x00, ATTR_RW, Pn[291]),
	OD_ENTRY(0x3124, 0x00, ATTR_RW, Pn[292]),
	OD_ENTRY(0x3125, 0x00, ATTR_RW, Pn[293]),
	OD_ENTRY(0x3126, 0x00, ATTR_RW, Pn[294]),
	OD_ENTRY(0x3127, 0x00, ATTR_RW, Pn[295]),
	OD_ENTRY(0x3128, 0x00, ATTR_RW, Pn[296]),
	OD_ENTRY(0x3129, 0x00, ATTR_RW, Pn[297]),
	OD_ENTRY(0x312A, 0x00, ATTR_RW, Pn[298]),
	OD_ENTRY(0x312B, 0x00, ATTR_RW, Pn[299]),

	OD_ENTRY(0x312C, 0x00, ATTR_RW, Pn[300]),
	OD_ENTRY(0x312D, 0x00, ATTR_RW, Pn[301]),
	OD_ENTRY(0x312E, 0x00, ATTR_RW, Pn[302]),
	OD_ENTRY(0x312F, 0x00, ATTR_RW, Pn[303]),
	OD_ENTRY(0x3130, 0x00, ATTR_RW, Pn[304]),
	OD_ENTRY(0x3131, 0x00, ATTR_RW, Pn[305]),
	OD_ENTRY(0x3132, 0x00, ATTR_RW, Pn[306]),
	OD_ENTRY(0x3133, 0x00, ATTR_RW, Pn[307]),
	OD_ENTRY(0x3134, 0x00, ATTR_RW, Pn[308]),
	OD_ENTRY(0x3135, 0x00, ATTR_RW, Pn[309]),

	OD_ENTRY(0x3136, 0x00, ATTR_RW, Pn[310]),
	OD_ENTRY(0x3137, 0x00, ATTR_RW, Pn[311]),
	OD_ENTRY(0x3138, 0x00, ATTR_RW, Pn[312]),
	OD_ENTRY(0x3139, 0x00, ATTR_RW, Pn[313]),
	OD_ENTRY(0x313A, 0x00, ATTR_RW, Pn[314]),
	OD_ENTRY(0x313B, 0x00, ATTR_RW, Pn[315]),
	OD_ENTRY(0x313C, 0x00, ATTR_RW, Pn[316]),
	OD_ENTRY(0x313D, 0x00, ATTR_RW, Pn[317]),
	OD_ENTRY(0x313E, 0x00, ATTR_RW, Pn[318]),
	OD_ENTRY(0x313F, 0x00, ATTR_RW, Pn[319]),

	OD_ENTRY(0x3140, 0x00, ATTR_RW, Pn[320]),
	OD_ENTRY(0x3141, 0x00, ATTR_RW, Pn[321]),
	OD_ENTRY(0x3142, 0x00, ATTR_RW, Pn[322]),
	OD_ENTRY(0x3143, 0x00, ATTR_RW, Pn[323]),
	OD_ENTRY(0x3144, 0x00, ATTR_RW, Pn[324]),
	OD_ENTRY(0x3145, 0x00, ATTR_RW, Pn[325]),
	OD_ENTRY(0x3146, 0x00, ATTR_RW, Pn[326]),
	OD_ENTRY(0x3147, 0x00, ATTR_RW, Pn[327]),
	OD_ENTRY(0x3148, 0x00, ATTR_RW, Pn[328]),
	OD_ENTRY(0x3149, 0x00, ATTR_RW, Pn[329]),

	OD_ENTRY(0x314A, 0x00, ATTR_RW, Pn[330]),
	OD_ENTRY(0x314B, 0x00, ATTR_RW, Pn[331]),
	OD_ENTRY(0x314C, 0x00, ATTR_RW, Pn[332]),
	OD_ENTRY(0x314D, 0x00, ATTR_RW, Pn[333]),
	OD_ENTRY(0x314E, 0x00, ATTR_RW, Pn[334]),
	OD_ENTRY(0x314F, 0x00, ATTR_RW, Pn[335]),
	OD_ENTRY(0x3150, 0x00, ATTR_RW, Pn[336]),
	OD_ENTRY(0x3151, 0x00, ATTR_RW, Pn[337]),
	OD_ENTRY(0x3152, 0x00, ATTR_RW, Pn[338]),
	OD_ENTRY(0x3153, 0x00, ATTR_RW, Pn[339]),

//	OD_ENTRY(0x3154, 0x00, ATTR_RW, Pn[340]),
//	OD_ENTRY(0x3155, 0x00, ATTR_RW, Pn[341]),
//	OD_ENTRY(0x3156, 0x00, ATTR_RW, Pn[342]),
//	OD_ENTRY(0x3157, 0x00, ATTR_RW, Pn[343]),
//	OD_ENTRY(0x3158, 0x00, ATTR_RW, Pn[344]),
//	OD_ENTRY(0x3159, 0x00, ATTR_RW, Pn[345]),

/*	报警数据ALM0～ALM9 (只读)*/
    OD_ENTRY(0x3200, 0x00, ATTR_RO, Pn[Fn000Addr+0]),//NEW
	OD_ENTRY(0x3201, 0x00, ATTR_RO, Pn[Fn000Addr+1]),      
	OD_ENTRY(0x3202, 0x00, ATTR_RO, Pn[Fn000Addr+2]),
	OD_ENTRY(0x3203, 0x00, ATTR_RO, Pn[Fn000Addr+3]),
	OD_ENTRY(0x3204, 0x00, ATTR_RO, Pn[Fn000Addr+4]),      
	OD_ENTRY(0x3205, 0x00, ATTR_RO, Pn[Fn000Addr+5]),
	OD_ENTRY(0x3206, 0x00, ATTR_RO, Pn[Fn000Addr+6]),
	OD_ENTRY(0x3207, 0x00, ATTR_RO, Pn[Fn000Addr+7]),
	OD_ENTRY(0x3208, 0x00, ATTR_RO, Pn[Fn000Addr+8]),
	OD_ENTRY(0x3209, 0x00, ATTR_RO, Pn[Fn000Addr+9]),

/* 监控数据Un000～Un013(只读) */
	OD_ENTRY(0x320A, 0x00, ATTR_RO, Un[0]),
	OD_ENTRY(0x320B, 0x00, ATTR_RO, Un[1]),      
	OD_ENTRY(0x320C, 0x00, ATTR_RO, Un[2]),
	OD_ENTRY(0x320D, 0x00, ATTR_RO, Un[3]),
	OD_ENTRY(0x320E, 0x00, ATTR_RO, Un[4]),      
	OD_ENTRY(0x320F, 0x00, ATTR_RO, Un[5]),
	OD_ENTRY(0x3210, 0x00, ATTR_RO, Un[6]),
	OD_ENTRY(0x3211, 0x00, ATTR_RO, Un[7]),      
	OD_ENTRY(0x3212, 0x00, ATTR_RO, Un[8]),
	OD_ENTRY(0x3213, 0x00, ATTR_RO, Un[9]),
	OD_ENTRY(0x3214, 0x00, ATTR_RO, Un[10]),
	OD_ENTRY(0x3215, 0x00, ATTR_RO, Un[11]),      
	OD_ENTRY(0x3216, 0x00, ATTR_RO, Un[12]),
	OD_ENTRY(0x3217, 0x00, ATTR_RO, Un[13]),
	OD_ENTRY(0x3218, 0x00, ATTR_RO, Un[14]),

	OD_ENTRY(0x321E, 0x00, ATTR_RO, Edition.all),
	OD_ENTRY(0x3300, 0x00, ATTR_RO, Rotate),
	OD_ENTRY(0x3301, 0x00, ATTR_RO, singlePos),
/* 其他信息 */

/***** Device profile for drive *****************************************/
	#ifdef SERVO_CONTROL
      OD_ENTRY(0x6007, 0x00, ATTR_RW, Connection_Option),			//Abort connection option code
      OD_ENTRY(0x603F, 0x00, ATTR_RO, Error_Code),					//Error code
//------------------------------------------------------------------------------
      OD_ENTRY(0x6040, 0x00, ATTR_RWW, CO_Controlword.all),
	  OD_ENTRY(0x6041, 0x00, ATTR_RO, CO_Statusword.all),
//---------------------停止处理方式---------------------------------------------------------
	  OD_ENTRY(0x605A, 0x00, ATTR_RW, Quickstop_Option_Code),		//Quick stop option code
	  OD_ENTRY(0x605B, 0x00, ATTR_RW, Shutdown_Option),				//Shutdown option code
	  OD_ENTRY(0x605C, 0x00, ATTR_RW, Disable_Operation_Option),	//Disable operation option code
	  OD_ENTRY(0x605D, 0x00, ATTR_RW, Halt_Option_Code),			//Halt option code
	  OD_ENTRY(0x605E, 0x00, ATTR_RW, Falt_Reaction_Option_Code),	//Fault reaction option code
//---------------------控制方式及其显示--------------------------------------------------------
	  OD_ENTRY(0x6060, 0x00, ATTR_RWW, Operation_Mode),//
	  OD_ENTRY(0x6061, 0x00, ATTR_RO, Operation_Mode_Display),
//------------------------ POSITION CONTROL FUNCTION--------------------------------------------------------
	  OD_ENTRY(0x6062, 0x00, ATTR_RO, Position_Demand_Value),
	  OD_ENTRY(0x6063, 0x00, ATTR_RO, Position_Acture_Value_aster),
	  OD_ENTRY(0x6064, 0x00, ATTR_RO, Position_Acture_Value),
	  OD_ENTRY(0x6065, 0x00, ATTR_RWW, Following_Error_Window),
	  OD_ENTRY(0x6066, 0x00, ATTR_RWW, Following_Error_Time_Out),
	  OD_ENTRY(0x6067, 0x00, ATTR_RWW, Position_Window),
	  OD_ENTRY(0x6068, 0x00, ATTR_RWW, Position_Window_Time),
//------------------------- Profile Velocity control-------------------------------------------------------
	  OD_ENTRY(0x6069, 0x00, ATTR_RO, Velocity_Sensor_Actual_Value),
	  OD_ENTRY(0x606A, 0x00, ATTR_RWW, Sensor_Selection_Code),
	  OD_ENTRY(0x606B, 0x00, ATTR_RO, Velocity_Demand_Value),
	  OD_ENTRY(0x606C, 0x00, ATTR_RO, Velocity_Actual_Value),
	  OD_ENTRY(0x606D, 0x00, ATTR_RWW, Velocity_Window),
	  OD_ENTRY(0x606E, 0x00, ATTR_RWW, Velocity_Window_Time),
	  OD_ENTRY(0x606F, 0x00, ATTR_RWW, Velocity_Threshold),
	  OD_ENTRY(0x6070, 0x00, ATTR_RWW, Velocity_Threshold_Time),
//-------------------------Profile Torque Control-------------------------------------------------------
	  OD_ENTRY(0x6071, 0x00, ATTR_RWW, Target_Torque),
	  OD_ENTRY(0x6072, 0x00, ATTR_RWW, Max_Torque),
	  OD_ENTRY(0x6073, 0x00, ATTR_RWW, Max_Current),
	  OD_ENTRY(0x6074, 0x00, ATTR_RO, Torque_Demand_Value),
	  OD_ENTRY(0x6075, 0x00, ATTR_RWW, Motor_Rated_Current),
	  OD_ENTRY(0x6076, 0x00, ATTR_RWW, Motor_Rated_Torque),
	  OD_ENTRY(0x6077, 0x00, ATTR_RO, Torque_Actual_Value),
	  OD_ENTRY(0x6078, 0x00, ATTR_RO, Current_Actual_Value),
//----------------------PROFILE POSITION MODE------------------------------------------------------
	  OD_ENTRY(0x607A, 0x00, ATTR_RWW, Target_Position),//pp
	  OD_ENTRY(0x607B, 0x00, ATTR_RO|ATTR_ROM, Position_Range_NoOfEntries),
	  OD_ENTRY(0x607B, 0x01, ATTR_RW, Min_Position_Range_Limit),
	  OD_ENTRY(0x607B, 0x02, ATTR_RW, Max_Position_Range_Limit),

	  OD_ENTRY(0x607C, 0x00, ATTR_RWW, Home_Offset),

	  OD_ENTRY(0x607D, 0x00, ATTR_RO|ATTR_ROM, Software_Position_Limit),
	  OD_ENTRY(0x607D, 0x01, ATTR_RW, Min_Position_Limit),
	  OD_ENTRY(0x607D, 0x02, ATTR_RW, Max_Position_Limit),
//------------------------------------------------------------------------------
 	  OD_ENTRY(0x607E, 0x00, ATTR_RWW, Polarity),

	  OD_ENTRY(0x607F, 0x00, ATTR_RWW, Max_Profile_Velocity),			//Max profile velocity
	  OD_ENTRY(0x6080, 0x00, ATTR_RWW, Max_Motor_Speed),	
	  OD_ENTRY(0x6081, 0x00, ATTR_RWW, Profile_Velocity),				//pp
	  OD_ENTRY(0x6082, 0x00, ATTR_RWW, END_Velocity),
	  OD_ENTRY(0x6083, 0x00, ATTR_RWW, Profile_Acceleration),			//pp pv
	  OD_ENTRY(0x6084, 0x00, ATTR_RWW, Profile_Decelaration),			//pp pv
	  OD_ENTRY(0x6085, 0x00, ATTR_RWW, Quick_Stop_Deceleration),		//pp pv
	  OD_ENTRY(0x6086, 0x00, ATTR_RWW, Motion_Profile_Type),	
		  
	  OD_ENTRY(0x6087, 0x00, ATTR_RWW, Torque_Slope),
	  OD_ENTRY(0x6088, 0x00, ATTR_RWW, Torque_Profile_Type),
//---------------------定义Factor---------------------------------------------------------
	  OD_ENTRY(0x608F, 0x00, ATTR_RO|ATTR_ROM, Position_Encoder_NoOfEntries),	//Position encoder resolution
	  OD_ENTRY(0x608F, 0x01, ATTR_RW, Position_Encoder_Increments),		//Position encoder resolution
	  OD_ENTRY(0x608F, 0x02, ATTR_RW, Position_Encoder_Revolutions),	//Position encoder resolution

	  OD_ENTRY(0x6090, 0x00, ATTR_RO|ATTR_ROM, Velocity_Encoder_NoOfEntries),	//Velocity encoder resolution
	  OD_ENTRY(0x6090, 0x01, ATTR_RW, Velocity_Encoder_Increments),		//Velocity encoder resolution
	  OD_ENTRY(0x6090, 0x02, ATTR_RW, Velocity_Encoder_Revolutions),	//Velocity encoder resolution
  
	  OD_ENTRY(0x6091, 0x00, ATTR_RO|ATTR_ROM, Gear_Ratio_NoOfEntries),			//Gear ratio
	  OD_ENTRY(0x6091, 0x01, ATTR_RW, Motor_Revolutions),				//Motor revolutions
	  OD_ENTRY(0x6091, 0x02, ATTR_RW, Shaft_Revolutions),				//Shaft revolutions
	  
	  OD_ENTRY(0x6092, 0x00, ATTR_RO|ATTR_ROM, Feed_Constant_NoOfEntries),		//Feed constant
	  OD_ENTRY(0x6092, 0x01, ATTR_RW, Feed),
	  OD_ENTRY(0x6092, 0x02, ATTR_RW, Feed_Shaft_Revolutions),			//Shaft revolutions

	  OD_ENTRY(0x6093, 0x00, ATTR_RO|ATTR_ROM, Position_Factor_NoOfEntries),		//Position factor
	  OD_ENTRY(0x6093, 0x01, ATTR_RW, Position_Numerator),				//Numerator
	  OD_ENTRY(0x6093, 0x02, ATTR_RW, Position_Divisor),				//Feed constant

	  OD_ENTRY(0x6094, 0x00, ATTR_RO|ATTR_ROM, Velocity_Factor_NoOfEntries),		//Velocity encoder factor
	  OD_ENTRY(0x6094, 0x01, ATTR_RW, Velocity_Numerator),				//Numerator
	  OD_ENTRY(0x6094, 0x02, ATTR_RW, Velocity_Divisor),				//Velocity_Divisor

	  OD_ENTRY(0x6095, 0x00, ATTR_RO|ATTR_ROM, Velocity_Factor1_NoOfEntries),	//Velocity factor 1
	  OD_ENTRY(0x6095, 0x01, ATTR_RW, Velocity_Numerator1),				//Numerator
	  OD_ENTRY(0x6095, 0x02, ATTR_RW, Velocity_Divisor1),				//Velocity_Divisor

	  OD_ENTRY(0x6096, 0x00, ATTR_RO|ATTR_ROM, Velocity_Factor2_NoOfEntries),	//Velocity factor 1
	  OD_ENTRY(0x6096, 0x01, ATTR_RW, Velocity_Numerator2),				//Numerator
	  OD_ENTRY(0x6096, 0x02, ATTR_RW, Velocity_Divisor2),				//Velocity_Divisor

	  OD_ENTRY(0x6097, 0x00, ATTR_RO|ATTR_ROM, Acceleration_Factor_NoOfEntries),	//Velocity factor 1
	  OD_ENTRY(0x6097, 0x01, ATTR_RW, Acceleration_Numerator),			//Numerator
	  OD_ENTRY(0x6097, 0x02, ATTR_RW, Acceleration_Divisor),			//Velocity_Divisor

//------------------------HOMING MODE--------------------------------------------------------
	  OD_ENTRY(0x6098, 0x00, ATTR_RWW, Homing_Method),
	  OD_ENTRY(0x6099, 0x00, ATTR_RO|ATTR_ROM, Homing_Speeds_NoOfEntries),
	  OD_ENTRY(0x6099, 0x01, ATTR_RWW, Switch_Speed),
	  OD_ENTRY(0x6099, 0x02, ATTR_RWW, Zero_Speed),
	  OD_ENTRY(0x609A, 0x00, ATTR_RWW, Homing_Acceleration),

	  OD_ENTRY(0x60A3, 0x00, ATTR_RO|ATTR_ROM, Profile_Jerk_Use),               //BY niekefu	
	  OD_ENTRY(0x60A4, 0x00, ATTR_RWW, Profile_Jerk_NoOfEntries),
	  OD_ENTRY(0x60A4, 0x01, ATTR_RWW, Profile_Jerk1),
//-------------------------Interpolate Position-------------------------------------------------------
	  OD_ENTRY(0x60C0, 0x00, ATTR_RW, Interpolatio_Sub_Mode_Select),
	  OD_ENTRY(0x60C1, 0x00, ATTR_RO, Interpolation_Data_Record_NoOfEntries),
	  OD_ENTRY(0x60C1, 0x01, ATTR_RWW, Interpolation_Data_Record1),	//data low
	  OD_ENTRY(0x60C1, 0x02, ATTR_RWW, Interpolation_Data_Record2),

	  OD_ENTRY(0x60C2, 0x00, ATTR_RO, Interpolation_Time_NoOfEntries),
	  OD_ENTRY(0x60C2, 0x01, ATTR_RW, Interpolation_Time_Unit),
	  OD_ENTRY(0x60C2, 0x02, ATTR_RW, Interpolation_Time_Index),

	  OD_ENTRY(0x60C3, 0x00, ATTR_RO, Interpolation_Sync_Definition),
	  OD_ENTRY(0x60C4, 0x00, ATTR_RW, Interpolation_Data_Configuration),
	  OD_ENTRY(0x60C5, 0x00, ATTR_RWW, Max_Acceleration),
	  OD_ENTRY(0x60C6, 0x00, ATTR_RWW, Max_Deceleration),
//	  OD_ENTRY(0x60F4, 0x00, ATTR_RO, CO_PP_velocity_plan_pars.dec_length),	//mark								   
	  OD_ENTRY(0x60F4, 0x00, ATTR_RO, Following_Error_Actual_Value),
	  OD_ENTRY(0x60FA, 0x00, ATTR_RO, Control_Effort), //mark
//	  OD_ENTRY(0x60FA, 0x00, ATTR_RO, inPgcnt),

	  //OD_ENTRY(0x60FB, 0x00, ATTR_RW, Following_Error_Actual_Value),
	  OD_ENTRY(0x60FC, 0x00, ATTR_RO, Position_Demand_Value_aster),
	  OD_ENTRY(0x60FD, 0x00, ATTR_RO, Digital_Inputs),		//Digital inputs
//------------------------------------------------------------------------------
//			DATA DESCRIPTION
//31 				16 15 		4 		3 		2 			1	 0
//manufacturer specific|reserved |interlock |home switch| POT | NOT 
//The switch have to be "active high".
//------------------------------------------------------------------------------      
      OD_ENTRY(0x60FE, 0x00, ATTR_RO|ATTR_ROM, Digital_NoOfEntries),	//Digital Outputs
	  OD_ENTRY(0x60FE, 0x01, ATTR_RWW, Physical_Outputs),	//Physical outputs
	  OD_ENTRY(0x60FE, 0x02, ATTR_RW, Bit_Mask),			//Bit mask
 	  OD_ENTRY(0x60FF, 0x00, ATTR_RWW, Target_Velocity),

//--------------------------------------------------------------------------------
   	  //Motor manufacturer data	  
	  OD_ENTRY(0x6402, 0x00, ATTR_RWW, Motor_Type),//Motor_Type
	  OD_ENTRY(0x6403, 0x00, ATTR_RW, Motor_Catalog),//Motor catalog number
	  OD_ENTRY(0x6404, 0x00, ATTR_RW, Motor_Manufacturer),//Motor manufacturer
	  OD_ENTRY(0x6405, 0x00, ATTR_RW, Motor_Address),//motor catalog address
	  OD_ENTRY(0x6406, 0x00, ATTR_RWW, Motor_Data),//motor calibration date
	  OD_ENTRY(0x6407, 0x00, ATTR_RWW, Motor_Service_Period),//Motor service period
//Motor data
	  OD_ENTRY(0x6410, 0x00, ATTR_RO|ATTR_ROM, Motor_NoOfEntries),//Motor data     
      OD_ENTRY(0x6410, 0x01, ATTR_RW, Motor_Current),//Motor data
	  OD_ENTRY(0x6410, 0x02, ATTR_RW, Motor_Torque),//Motor data
	  OD_ENTRY(0x6410, 0x03, ATTR_RW, Motor_Speed),//Motor data
	  OD_ENTRY(0x6410, 0x04, ATTR_RW, Motor_Encoder),//Motor data
	  OD_ENTRY(0x6410, 0x05, ATTR_RW, Motor_Angel),//Motor data
	  OD_ENTRY(0x6410, 0x06, ATTR_RW, Motor_R),//Motor data
//Drive manufacturer data	
	  OD_ENTRY(0x6502, 0x00, ATTR_RO, Drive_Mode),//Supported drive modes
//----------------------------------------------------------------------------
//31 					16 15 	7 	6 	5 	4 		 3  2  1  0
//manufacturer specific reserved 	ip 	hm 	reserved tq pv vl pp
//----------------------------------------------------------------------------
	  OD_ENTRY(0x6503, 0x00, ATTR_RW|ATTR_ROM, Drive_Nameplate_Number),//Drive catalog number
	  OD_ENTRY(0x6504, 0x00, ATTR_RW, Drive_Manufacturer),//Drive manufacturer
	  OD_ENTRY(0x6505, 0x00, ATTR_RW, Drive_Catalog_Address),//drive catalog address
//drive data
	  OD_ENTRY(0x6510, 0x00, ATTR_RO|ATTR_ROM, Drive_NoOfEntries),	//drive data
	  OD_ENTRY(0x6510, 0x01, ATTR_RW, Drive_Input_Current),			//drive data
	  OD_ENTRY(0x6510, 0x02, ATTR_RW, Drive_Output_Current),		//drive data
	  OD_ENTRY(0x6510, 0x03, ATTR_RW, Drive_Output_Power),			//drive data
//
   #endif
};

/***** Number of Elements in Object Dictionary ********************************/
ROM unsigned int CO_OD_NoOfElements = sizeof(CO_OD) / sizeof(CO_OD[0]);

//=============================================================================
// End of file.
//=============================================================================
