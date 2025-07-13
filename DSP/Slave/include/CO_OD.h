/*******************************************************************************

   CO_OD.h - Definitions for default values in Object Dictionary

   Copyright (C) 2005  Bluestar

********************************************************************************

   For short description of standard Object Dictionary entries see CO_OD.txt

*******************************************************************************/

#ifndef _CO_OD_H
#define _CO_OD_H

/*******************************************************************************
   Setup CANopen
*******************************************************************************/
   #define CO_NO_SYNC            1  //(0 or 1), is SYNC (producer and consumer) used or not.
   #define CO_NO_EMERGENCY       1  //(0 or 1), is Emergency message producer used or not.
   #define CO_NO_RPDO            4  //(0 to 512*), number of receive PDOs.
   #define CO_NO_TPDO            4  //(0 to 512*), number of transmit PDOs.
   #define CO_NO_SDO_SERVER      1  //(0 to 128*), number of SDO server channels.
   #define CO_NO_SDO_CLIENT      0  //(0 to 128*), number of SDO client channels.
   #define CO_NO_CONS_HEARTBEAT  1  //(0 to 255*), number of consumer heartbeat entries.
   #define CO_NO_USR_CAN_RX      0  //(0 to ...), number of user CAN RX messages.
   #define CO_NO_USR_CAN_TX      0  //(0 to ...), number of user CAN TX messages.
   //test 传输驱动器名字等信息需要比较长的数据，一个字母表示一个字 2007-6-20
   #define CO_MAX_OD_ENTRY_SIZE  40 //(4 to 256), max size of variable in Object Dictionary in bytes.
   #define CO_SDO_TIMEOUT_TIME   10 //[in 100*ms] Timeout in SDO communication.
   #define CO_NO_ERROR_FIELD     8  //(0 to 254*), size of Pre Defined Error Fields at index 0x1003.
   #define CO_PDO_PARAM_IN_OD       //if defined, PDO parameters will be in Object Dictionary.
   //2007-6-16
   #define CO_PDO_MAPPING_IN_OD     //if defined, PDO mapping will be in Object Dictionary. If not defined, PDO size will be fixed to 8 bytes.
   #define CO_TPDO_INH_EV_TIMER     //if defined, Inhibit and Event timer will be used for TPDO transmission.
   #define CO_VERIFY_OD_WRITE       //if defined, SDO write to Object Dictionary will be verified.
   #define CO_OD_IS_ORDERED         //enable this macro, if entries in CO_OD are ordered (from lowest to highest index, then subindex). For longer Object Dictionaries searching is much faster. If entries are not ordered, disable macro.
   #define CO_SAVE_EEPROM           //if defined, ODE_EEPROM data will be saved.
   #define CO_SAVE_ROM              //if defined, ROM variables will be writeable from outside.
// * For some macros have written only 1 or up to 8 entries in CO_OD.c/CO_OD.h.
//   If you get error message and if you need more than that, write additional entries.


/*******************************************************************************
   Device profile for Generic I/O
*******************************************************************************/
   //#define CO_IO_DIGITAL_INPUTS     	//4 * 8 digital inputs
   //#define CO_IO_DIGITAL_OUTPUTS    	//4 * 8 digital outputs
   #define CO_IO_ANALOG_INPUTS      	//8 * 16bit analog inputs
   #define SERVO_CONTROL		1		//伺服控制领域用到的字典
   //#define CO_IO_ANALOG_OUTPUTS     	//2 * 16bit analog outputs


/*******************************************************************************
   Default values for object dictionary
*******************************************************************************/
   #define ODD_DEVICE_TYPE       0x00020192L    /*index 0x1000, RO*/ //servo driver
   #define ODD_SYNC_COB_ID       0x00000080L    /*index 0x1005*/     //if bit30=1, node produces SYNC
   //2007-6-22
   #define ODD_COMM_CYCLE_PERIOD 4000L           /*index 0x1006*/     //in micro seconds
   #define ODD_SYNCHR_WINDOW_LEN 700L             /*index 0x1007*/     //in micro seconds
   #define ODD_MANUF_DEVICE_NAME "ESTUN ProNet Driver"  /*index 0x1008, RO*/
   #define ODD_MANUF_HW_VERSION  "1.050"         /*index 0x1009, RO*/
   #define ODD_MANUF_SW_VERSION  "1.050"         /*index 0x100A, RO*/
   #define ODD_INHIBIT_TIME_EMER 50             /*index 0x1015*/     //time in 100 micro seconds
                                                /*index 0x1016*/     //see below
   #define ODD_PROD_HEARTBEAT    1000           /*index 0x1017*/     //time in ms
   //EDS信息
   #define ODD_IDENT_VENDOR_ID   0x00000322L    /*index 0x1018, RO*/ //See standard
   #define ODD_IDENT_PROD_CODE   0x00000000L
   #define ODD_IDENT_REVISION_NR 0x00000100L
   #define ODD_IDENT_SERIAL_NR   0x00000000L

   #define ODD_ERROR_BEH_COMM    0x01           /*index 0x1029*/     //If Communication error is present in Operational state: 0x00-switch to pre-operational; 0x01-do nothing; 0x02-switch to stopped.
   #define ODD_NMT_STARTUP       0x00000004L    /*index 0x1F80*/     //only bit2 implemented, if bit2=1, Node will NOT start operational

   #define DRIVE_CATALOG_ADDRESS "http://www.estun.com"
/* 0x1016 Heartbeat consumer **************************************************/
   //00NNTTTT: N=NodeID, T=time in ms
   #define ODD_CONS_HEARTBEAT_0  0x00000000L
   #define ODD_CONS_HEARTBEAT_1  0x00000000L
   #define ODD_CONS_HEARTBEAT_2  0x00000000L
   #define ODD_CONS_HEARTBEAT_3  0x00000000L
   #define ODD_CONS_HEARTBEAT_4  0x00000000L
   #define ODD_CONS_HEARTBEAT_5  0x00000000L
   #define ODD_CONS_HEARTBEAT_6  0x00000000L
   #define ODD_CONS_HEARTBEAT_7  0x00000000L

/* 0x1400 Receive PDO parameters **********************************************/
   //COB-ID: if(bit31==1) PDO is not used; bit30=1; bits(10..0)=COB-ID;
   #define ODD_RPDO_PAR_COB_ID_0 1  //if 0, predefined value will be used (0x200+NODE-ID - read-only)
   #define ODD_RPDO_PAR_T_TYPE_0 255
   #define ODD_RPDO_PAR_COB_ID_1 1  //if 0, predefined value will be used (0x300+NODE-ID - read-only)
   #define ODD_RPDO_PAR_T_TYPE_1 255
   #define ODD_RPDO_PAR_COB_ID_2 1  //if 0, predefined value will be used (0x400+NODE-ID - read-only)
   #define ODD_RPDO_PAR_T_TYPE_2 255
   #define ODD_RPDO_PAR_COB_ID_3 1  //if 0, predefined value will be used (0x500+NODE-ID - read-only)
   #define ODD_RPDO_PAR_T_TYPE_3 255
   #define ODD_RPDO_PAR_COB_ID_4 0		//2007-5-10
   #define ODD_RPDO_PAR_T_TYPE_4 255
   #define ODD_RPDO_PAR_COB_ID_5 0
   #define ODD_RPDO_PAR_T_TYPE_5 255
   #define ODD_RPDO_PAR_COB_ID_6 0
   #define ODD_RPDO_PAR_T_TYPE_6 255
   #define ODD_RPDO_PAR_COB_ID_7 0
   #define ODD_RPDO_PAR_T_TYPE_7 255

/* 0x1600 Receive PDO mapping *************************************************/
   //0xIIIISSDD IIII = index from OD, SS = subindex, DD = data length in bits
   //DD must be byte aligned, max value 0x40 (8 bytes)

   #define ODD_RPDO_MAP_0_1      0x60400010L	//默认OD：6040，607A
   #define ODD_RPDO_MAP_0_2      0x607A0020L	//
   #define ODD_RPDO_MAP_0_3      0x00000000L
   #define ODD_RPDO_MAP_0_4      0x00000000L
   #define ODD_RPDO_MAP_0_5      0x00000000L
   #define ODD_RPDO_MAP_0_6      0x00000000L
   #define ODD_RPDO_MAP_0_7      0x00000000L
   #define ODD_RPDO_MAP_0_8      0x00000000L

   #define ODD_RPDO_MAP_1_1      0x60810020L	//
   #define ODD_RPDO_MAP_1_2      0x60600008L	//
   #define ODD_RPDO_MAP_1_3      0x00000000L
   #define ODD_RPDO_MAP_1_4      0x00000000L
   #define ODD_RPDO_MAP_1_5      0x00000000L
   #define ODD_RPDO_MAP_1_6      0x00000000L
   #define ODD_RPDO_MAP_1_7      0x00000000L
   #define ODD_RPDO_MAP_1_8      0x00000000L

   #define ODD_RPDO_MAP_2_1      0x60400010L
   #define ODD_RPDO_MAP_2_2      0x607A0020L
   #define ODD_RPDO_MAP_2_3      0x00000000L
   #define ODD_RPDO_MAP_2_4      0x00000000L
   #define ODD_RPDO_MAP_2_5      0x00000000L
   #define ODD_RPDO_MAP_2_6      0x00000000L
   #define ODD_RPDO_MAP_2_7      0x00000000L
   #define ODD_RPDO_MAP_2_8      0x00000000L

   #define ODD_RPDO_MAP_3_1      0x60400010L
   #define ODD_RPDO_MAP_3_2      0x60FF0020L
   #define ODD_RPDO_MAP_3_3      0x00000000L
   #define ODD_RPDO_MAP_3_4      0x00000000L
   #define ODD_RPDO_MAP_3_5      0x00000000L
   #define ODD_RPDO_MAP_3_6      0x00000000L
   #define ODD_RPDO_MAP_3_7      0x00000000L
   #define ODD_RPDO_MAP_3_8      0x00000000L

   #define ODD_RPDO_MAP_4_1      0x60400010L
   #define ODD_RPDO_MAP_4_2      0x60710010L
   #define ODD_RPDO_MAP_4_3      0x00000000L
   #define ODD_RPDO_MAP_4_4      0x00000000L
   #define ODD_RPDO_MAP_4_5      0x00000000L
   #define ODD_RPDO_MAP_4_6      0x00000000L
   #define ODD_RPDO_MAP_4_7      0x00000000L
   #define ODD_RPDO_MAP_4_8      0x00000000L

   #define ODD_RPDO_MAP_5_1      0x00000000L
   #define ODD_RPDO_MAP_5_2      0x00000000L
   #define ODD_RPDO_MAP_5_3      0x00000000L
   #define ODD_RPDO_MAP_5_4      0x00000000L
   #define ODD_RPDO_MAP_5_5      0x00000000L
   #define ODD_RPDO_MAP_5_6      0x00000000L
   #define ODD_RPDO_MAP_5_7      0x00000000L
   #define ODD_RPDO_MAP_5_8      0x00000000L

   #define ODD_RPDO_MAP_6_1      0x00000000L
   #define ODD_RPDO_MAP_6_2      0x00000000L
   #define ODD_RPDO_MAP_6_3      0x00000000L
   #define ODD_RPDO_MAP_6_4      0x00000000L
   #define ODD_RPDO_MAP_6_5      0x00000000L
   #define ODD_RPDO_MAP_6_6      0x00000000L
   #define ODD_RPDO_MAP_6_7      0x00000000L
   #define ODD_RPDO_MAP_6_8      0x00000000L

   #define ODD_RPDO_MAP_7_1      0x00000000L
   #define ODD_RPDO_MAP_7_2      0x00000000L
   #define ODD_RPDO_MAP_7_3      0x00000000L
   #define ODD_RPDO_MAP_7_4      0x00000000L
   #define ODD_RPDO_MAP_7_5      0x00000000L
   #define ODD_RPDO_MAP_7_6      0x00000000L
   #define ODD_RPDO_MAP_7_7      0x00000000L
   #define ODD_RPDO_MAP_7_8      0x00000000L

/* 0x1800 Transmit PDO parameters *********************************************/
   //COB-ID: if(bit31==1) PDO is not used; bit30=1; bits(10..0)=COB-ID;
   //T_TYPE: 1-240...transmission after every (T_TYPE)-th SYNC object;
   //        254...manufacturer specific
   //        255...Device Profile specific, default transmission is Change of State
   //I_TIME: 0...65535 Inhibit time in 100us is minimum time between PDO transmission
   //E_TIME: 0...65535 Event timer in ms - PDO is periodically transmitted (0 == disabled)
   #define ODD_TPDO_PAR_COB_ID_0 1  //if 0, predefined value will be used (0x180+NODE-ID - read-only)
   #define ODD_TPDO_PAR_T_TYPE_0 255
   #define ODD_TPDO_PAR_I_TIME_0 10//100	
   #define ODD_TPDO_PAR_E_TIME_0 10//0

   #define ODD_TPDO_PAR_COB_ID_1 1  //if 0, predefined value will be used (0x280+NODE-ID - read-only)
   #define ODD_TPDO_PAR_T_TYPE_1 255
   #define ODD_TPDO_PAR_I_TIME_1 10	//1000 100ms *T1
   #define ODD_TPDO_PAR_E_TIME_1 10	//60000 

   #define ODD_TPDO_PAR_COB_ID_2 1  //if 0, predefined value will be used (0x380+NODE-ID - read-only)
   #define ODD_TPDO_PAR_T_TYPE_2 255
   #define ODD_TPDO_PAR_I_TIME_2 10
   #define ODD_TPDO_PAR_E_TIME_2 10

   #define ODD_TPDO_PAR_COB_ID_3 1  //if 0, predefined value will be used (0x480+NODE-ID - read-only)
   #define ODD_TPDO_PAR_T_TYPE_3 255
   #define ODD_TPDO_PAR_I_TIME_3 10
   #define ODD_TPDO_PAR_E_TIME_3 10

   #define ODD_TPDO_PAR_COB_ID_4 0//0xC0000000L
   #define ODD_TPDO_PAR_T_TYPE_4 255
   #define ODD_TPDO_PAR_I_TIME_4 10
   #define ODD_TPDO_PAR_E_TIME_4 10

   #define ODD_TPDO_PAR_COB_ID_5 0//0xC0000000L
   #define ODD_TPDO_PAR_T_TYPE_5 254
   #define ODD_TPDO_PAR_I_TIME_5 0
   #define ODD_TPDO_PAR_E_TIME_5 0

   #define ODD_TPDO_PAR_COB_ID_6 0//0xC0000000L
   #define ODD_TPDO_PAR_T_TYPE_6 254
   #define ODD_TPDO_PAR_I_TIME_6 0
   #define ODD_TPDO_PAR_E_TIME_6 0

   #define ODD_TPDO_PAR_COB_ID_7 0//0xC0000000L
   #define ODD_TPDO_PAR_T_TYPE_7 254
   #define ODD_TPDO_PAR_I_TIME_7 0
   #define ODD_TPDO_PAR_E_TIME_7 0

/* 0x1A00 Transmit PDO mapping ************************************************/
   //0xIIIISSDD IIII = index from OD, SS = subindex, DD = data length in bits
   //DD must be byte aligned, max value 0x40 (8 bytes)

   #define ODD_TPDO_MAP_0_1      0x60410010L//默认：6041，607A   0x60000108L
   #define ODD_TPDO_MAP_0_2      0x60640020L//60640020L//0x60000208L
   #define ODD_TPDO_MAP_0_3      0x00000000L//0x60000308L
   #define ODD_TPDO_MAP_0_4      0x00000000L//0x60000408L
   #define ODD_TPDO_MAP_0_5      0x00000000L
   #define ODD_TPDO_MAP_0_6      0x00000000L
   #define ODD_TPDO_MAP_0_7      0x00000000L
   #define ODD_TPDO_MAP_0_8      0x00000000L

   #define ODD_TPDO_MAP_1_1      0x60640020L//0x64010110L
   #define ODD_TPDO_MAP_1_2      0x60610010L//0x64010200L
   #define ODD_TPDO_MAP_1_3      0x00000000L//0x64010300L
   #define ODD_TPDO_MAP_1_4      0x00000000L//0x64010400L
   #define ODD_TPDO_MAP_1_5      0x00000000L
   #define ODD_TPDO_MAP_1_6      0x00000000L
   #define ODD_TPDO_MAP_1_7      0x00000000L
   #define ODD_TPDO_MAP_1_8      0x00000000L

   #define ODD_TPDO_MAP_2_1      0x60410010L//0x64010510L
   #define ODD_TPDO_MAP_2_2      0x60640020L//0x64010610L
   #define ODD_TPDO_MAP_2_3      0x00000000L//0x64010710L
   #define ODD_TPDO_MAP_2_4      0x00000000L//0x64010810L
   #define ODD_TPDO_MAP_2_5      0x00000000L
   #define ODD_TPDO_MAP_2_6      0x00000000L
   #define ODD_TPDO_MAP_2_7      0x00000000L
   #define ODD_TPDO_MAP_2_8      0x00000000L

   #define ODD_TPDO_MAP_3_1      0x60410010L
   #define ODD_TPDO_MAP_3_2      0x606C0020L
   #define ODD_TPDO_MAP_3_3      0x00000000L
   #define ODD_TPDO_MAP_3_4      0x00000000L
   #define ODD_TPDO_MAP_3_5      0x00000000L
   #define ODD_TPDO_MAP_3_6      0x00000000L
   #define ODD_TPDO_MAP_3_7      0x00000000L
   #define ODD_TPDO_MAP_3_8      0x00000000L

   #define ODD_TPDO_MAP_4_1      0x60410010L
   #define ODD_TPDO_MAP_4_2      0x60770010L
   #define ODD_TPDO_MAP_4_3      0x00000000L
   #define ODD_TPDO_MAP_4_4      0x00000000L
   #define ODD_TPDO_MAP_4_5      0x00000000L
   #define ODD_TPDO_MAP_4_6      0x00000000L
   #define ODD_TPDO_MAP_4_7      0x00000000L
   #define ODD_TPDO_MAP_4_8      0x00000000L

   #define ODD_TPDO_MAP_5_1      0x00000000L
   #define ODD_TPDO_MAP_5_2      0x00000000L
   #define ODD_TPDO_MAP_5_3      0x00000000L
   #define ODD_TPDO_MAP_5_4      0x00000000L
   #define ODD_TPDO_MAP_5_5      0x00000000L
   #define ODD_TPDO_MAP_5_6      0x00000000L
   #define ODD_TPDO_MAP_5_7      0x00000000L
   #define ODD_TPDO_MAP_5_8      0x00000000L

   #define ODD_TPDO_MAP_6_1      0x00000000L
   #define ODD_TPDO_MAP_6_2      0x00000000L
   #define ODD_TPDO_MAP_6_3      0x00000000L
   #define ODD_TPDO_MAP_6_4      0x00000000L
   #define ODD_TPDO_MAP_6_5      0x00000000L
   #define ODD_TPDO_MAP_6_6      0x00000000L
   #define ODD_TPDO_MAP_6_7      0x00000000L
   #define ODD_TPDO_MAP_6_8      0x00000000L

   #define ODD_TPDO_MAP_7_1      0x00000000L
   #define ODD_TPDO_MAP_7_2      0x00000000L
   #define ODD_TPDO_MAP_7_3      0x00000000L
   #define ODD_TPDO_MAP_7_4      0x00000000L
   #define ODD_TPDO_MAP_7_5      0x00000000L
   #define ODD_TPDO_MAP_7_6      0x00000000L
   #define ODD_TPDO_MAP_7_7      0x00000000L
   #define ODD_TPDO_MAP_7_8      0x00000000L


/*******************************************************************************
   Default values for user Object Dictionary Entries
*******************************************************************************/
/*0x2101*/
//   #define ODD_CANnodeID    0x06 //(1 to 127), default node ID
/*0x2102*/
//   #define ODD_CANbitRate   4    //(0 to 5), default CAN bit rate
                                    // 0 = 50 kbps     1 = 100 kbps
                                    // 2 = 125 kbps    3 = 250 kbps
                                    // 4 = 500 kbps    5 = 1000 kbps
/*******************************************************************************
   CANopen basic Data Types
*******************************************************************************/
   #define UNSIGNED8  unsigned char
   #define UNSIGNED16 unsigned int
   #define UNSIGNED32 unsigned long
   #define INTEGER8   char
   #define INTEGER16  int
   #define INTEGER32  long


/*******************************************************************************
   CANopen Variables from Object Dictionary
*******************************************************************************/
/*0x1001*/ extern       volatile UNSIGNED8      ODE_Error_Register;
/*0x1002*/ extern       volatile UNSIGNED32     ODE_Manufacturer_Status_Register;

           #if CO_NO_ERROR_FIELD > 0
/*0x1003*/ extern       volatile UNSIGNED8      ODE_Pre_Defined_Error_Field_NoOfErrors;
           extern       volatile UNSIGNED32     ODE_Pre_Defined_Error_Field[];
           #endif

           #if CO_NO_SYNC > 0
		   //2007-6-22
/*0x1005*/ //extern ROM   UNSIGNED32     ODE_SYNC_COB_ID;
/*0x1006*/ //extern ROM   UNSIGNED32     ODE_Communication_Cycle_Period;
/*0x1007*/ //extern ROM   UNSIGNED32     ODE_Synchronous_Window_Length;
/*0x1005*/ extern ROM  UNSIGNED32     ODE_SYNC_COB_ID;
/*0x1006*/ extern   volatile UNSIGNED32     ODE_Communication_Cycle_Period;
		   extern   volatile UNSIGNED32     ODE_Communication_Cycle_Period1;
/*0x1007*/ extern   volatile UNSIGNED32     ODE_Synchronous_Window_Length;
           #endif

           #if CO_NO_EMERGENCY > 0
/*0x1014*/ extern ROM  UNSIGNED32     ODE_Emergency_COB_ID;
/*0x1015*/ extern   volatile UNSIGNED16     ODE_Inhibit_Time_Emergency;
           #endif

           #if CO_NO_CONS_HEARTBEAT > 0
/*0x1016*/ extern   volatile UNSIGNED32     ODE_Consumer_Heartbeat_Time[];
           #endif

/*0x1017*/ extern   volatile UNSIGNED16     ODE_Producer_Heartbeat_Time;

           typedef struct{
              UNSIGNED8    NoOfEntries;
              UNSIGNED8    Communication_Error;
           }               ODEs_Error_Behavior;
/*0x1029*/ extern ROM   ODEs_Error_Behavior  ODE_Error_Behavior;

           #if CO_NO_SDO_SERVER > 0 || CO_NO_SDO_CLIENT > 0
           typedef struct{
              UNSIGNED32   COB_ID_Client_to_Server;
              UNSIGNED32   COB_ID_Server_to_Client;
              UNSIGNED8    NODE_ID_of_SDO_Client_or_Server;
           }               CO_sSDO_Param;
           #endif

           #if CO_NO_SDO_SERVER > 0
/*0x1200*/ extern  ROM CO_sSDO_Param  ODE_Server_SDO_Parameter[];
           #endif

           #if CO_NO_SDO_CLIENT > 0
/*0x1280*/ extern  volatile CO_sSDO_Param  ODE_Client_SDO_Parameter[];
           #endif

           #if CO_NO_RPDO > 0
           typedef struct{
              UNSIGNED32   COB_ID;
              UNSIGNED8    Transmission_type;
			  //UNSIGNED8    Compatibility_Entry; - not needed
			  UNSIGNED16    Inhibit_Time;//F2808 servo
			  UNSIGNED16    Event_Timer;//F2808 servo

           }               CO_sRPDO_param;
		   //2007-6-22
/*0x1400*/ //extern ROM   CO_sRPDO_param ODE_RPDO_Parameter[];
/*0x1400*/ extern   volatile CO_sRPDO_param ODE_RPDO_Parameter[];
           #ifdef CO_PDO_MAPPING_IN_OD
/*0x1600*/ //extern ROM   UNSIGNED32     ODE_RPDO_Mapping[][8];
/*0x1600*/ extern   volatile UNSIGNED32     ODE_RPDO_Mapping[][8];
           #endif
           #endif

           #if CO_NO_TPDO > 0
           typedef struct{
              UNSIGNED32   COB_ID;
              UNSIGNED8    Transmission_type;
              UNSIGNED16   Inhibit_Time;
              //UNSIGNED8    Compatibility_Entry; - not needed
              UNSIGNED16   Event_Timer;
           }               CO_sTPDO_param;
		   //2007-6-22
/*0x1800*/ //extern ROM   CO_sTPDO_param ODE_TPDO_Parameter[];
/*0x1800*/ extern   volatile CO_sTPDO_param ODE_TPDO_Parameter[];
           #ifdef CO_PDO_MAPPING_IN_OD
/*0x1A00*/ //extern ROM   UNSIGNED32     ODE_TPDO_Mapping[][8];
/*0x1A00*/ extern   volatile UNSIGNED32     ODE_TPDO_Mapping[][8];
           #endif
           #endif
/***** Device profile for Generic I/O *****************************************/
/*0x6040*/ #ifdef SERVO_CONTROL		//伺服控制领域用到的字典
      //paraToFram.Control_Word
	  //paraToFram.System_Status
//#define SINE_PM_Motor	10
//#define DRIVE_MODE	0x2D	//支持pp,pv,tq,hm控制方式

	extern  volatile INTEGER16 Connection_Option;//Abort connection option code
    extern  volatile UNSIGNED16 Error_Code;//Error code
//Motor manufacturer data	  
	extern  volatile UNSIGNED16 Motor_Type;//Motor_Type:Sinusoidal PM brushless motor
	  //UNSIGNED16
	extern  volatile char Motor_Catalog;//Motor catalog number
	  //UNSIGNED16
	extern  volatile char Motor_Manufacturer;//Motor manufacturer
	extern  volatile UNSIGNED16 Motor_Address;//motor catalog address
	extern  volatile UNSIGNED16 Motor_Data;//motor calibration date
	extern  volatile UNSIGNED32 Motor_Service_Period;//Motor service period,hours
//Motor data
	extern  volatile UNSIGNED8 Motor_NoOfEntries;//Motor data     
    extern  volatile UNSIGNED32 Motor_Current;//Motor data
	extern  volatile UNSIGNED32 Motor_Torque;//Motor data
	extern  volatile UNSIGNED32 Motor_Speed;//Motor data
	extern  volatile UNSIGNED32 Motor_Encoder;//Motor data
	extern  volatile UNSIGNED32 Motor_Angel;//Motor data
	extern  volatile UNSIGNED32 Motor_R;//Motor data
//Drive manufacturer data	
	extern  volatile UNSIGNED16 Drive_Mode;//Supported drive modes
//----------------------------------------------------------------------------
//		DATA DESCRIPTION
//31 					16 15 	7 	6 	5 	4 		 3  2  1  0
//manufacturer specific reserved 	ip 	hm 	reserved tq pv vl pp
//MSB													LSB
//----------------------------------------------------------------------------
	extern  volatile UNSIGNED16  Drive_Nameplate_Number;//Drive catalog number
	  //UNSIGNED16
	extern  volatile char  Drive_Manufacturer[];//Drive manufacturer
	  //UNSIGNED16
	extern  volatile char  Drive_Catalog_Address[];//drive catalog address
//drive data
	extern  volatile UNSIGNED8  Drive_NoOfEntries;//drive data
	extern  volatile UNSIGNED32 Drive_Input_Current;//drive data
	extern  volatile UNSIGNED32 Drive_Output_Current;//drive data
	extern  volatile UNSIGNED32 Drive_Output_Power;//drive data
//
	extern  volatile UNSIGNED32 Digital_Inputs;//Digital inputs
//------------------------------------------------------------------------------
//			DATA DESCRIPTION
//31 				16 15 		4 		3 		2 			1				 0
//manufacturer specific|reserved |interlock |home switch| positive limit | negative limit 
//															switch	  		switch
//MSB																		 LSB
//The switch have to be "active high".
//------------------------------------------------------------------------------      
    extern  volatile UNSIGNED8  Digital_NoOfEntries;//Digital Outputs
	extern  volatile UNSIGNED32 Physical_Outputs;//Physical outputs
	extern  volatile UNSIGNED32 Bit_Mask;//Bit mask

//------------------------------------------------------------------------------
      //UNSIGNED16 paraToFram.Control_Word;
	  //UNSIGNED16 paraToFram.System_Status;
//---------------------停止处理方式---------------------------------------------------------
	extern  volatile INTEGER16 Shutdown_Option;//Shutdown option code
	extern  volatile INTEGER16 Disable_Operation_Option;//Disable operation option code
	extern  volatile INTEGER16 Quickstop_Option_Code;//Quick stop option code
	extern  volatile INTEGER16 Halt_Option_Code;//Halt option code
	extern  volatile INTEGER16 Falt_Reaction_Option_Code;//Fault reaction option code
//---------------------控制方式及其显示--------------------------------------------------------
	  //UNSIGNED8 paraToFram.Operation_Mode;//
	extern  volatile INTEGER8 Operation_Mode;
	extern  volatile INTEGER8 Operation_Mode_Display;
//------------------------------------------------------------------------------
//	extern  UNSIGNED16 Polarity;	//Polarity
//---------------------没有用到,保留---------------------------------------------------------
	  //OD_ENTRY(0x6089, 0x00, ATTR_RW, Position_Notation_Index),	//Position notation index
	  //OD_ENTRY(0x608A, 0x00, ATTR_RW, Position_Dimension_Index),	//Position dimension index
	  //OD_ENTRY(0x608B, 0x00, ATTR_RW, Velocity_Notation_Index),	//Velocity notation index
	  //OD_ENTRY(0x608C, 0x00, ATTR_RW, Velocity_Dimension_Index),	//Velocity dimension index
	  //OD_ENTRY(0x608D, 0x00, ATTR_RW, Acceleration_Notation_Index),	//Acceleration notation index
	  //OD_ENTRY(0x608E, 0x00, ATTR_RW, Acceleration_Dimension_Index),	//Acceleration dimension index	  
//---------------------定义Factor---------------------------------------------------------
	extern  volatile UNSIGNED8  Position_Encoder_NoOfEntries;	//Position encoder resolution
	extern  volatile UNSIGNED32 Position_Encoder_Increments;		//Position encoder resolution
	extern  volatile UNSIGNED32 Position_Encoder_Revolutions;	//Position encoder resolution

	extern  volatile UNSIGNED8  Velocity_Encoder_NoOfEntries;	//Velocity encoder resolution
	extern  volatile UNSIGNED32 Velocity_Encoder_Increments;		//Velocity encoder resolution
	extern  volatile UNSIGNED32 Velocity_Encoder_Revolutions;	//Velocity encoder resolution
  
	extern  volatile UNSIGNED8 Gear_Ratio_NoOfEntries;	//Gear ratio
	extern  volatile UNSIGNED32 Motor_Revolutions;		//Motor revolutions
	extern  volatile UNSIGNED32 Shaft_Revolutions;	//Shaft revolutions
	  
	extern  volatile UNSIGNED8 Feed_Constant_NoOfEntries;	//Feed constant
	extern  volatile UNSIGNED32 Feed;		//Feed
	extern  volatile UNSIGNED32 Feed_Shaft_Revolutions;	//Shaft revolutions

	extern  volatile UNSIGNED8 Position_Factor_NoOfEntries;	//Position factor
	extern  volatile UNSIGNED32 Position_Numerator;		//Numerator
	extern  volatile UNSIGNED32 Position_Divisor;	//Feed constant

	extern  volatile UNSIGNED8 Velocity_Factor1_NoOfEntries;	//Velocity encoder factor
	extern  volatile UNSIGNED32 Velocity_Numerator1;		//Numerator
	extern  volatile UNSIGNED32 Velocity_Divisor1;	//Velocity_Divisor

	extern  volatile UNSIGNED8 Velocity_Factor_NoOfEntries;	//Velocity factor 1
	extern  volatile UNSIGNED32 Velocity_Numerator;		//Numerator
	extern  volatile UNSIGNED32 Velocity_Divisor;	//Velocity_Divisor

	extern  volatile UNSIGNED8 Velocity_Factor2_NoOfEntries;	//Velocity factor 1
	extern  volatile UNSIGNED32 Velocity_Numerator2;		//Numerator
	extern  volatile UNSIGNED32 Velocity_Divisor2;	//Velocity_Divisor

	extern  volatile UNSIGNED8 Acceleration_Factor_NoOfEntries;	//Velocity factor 1
	extern  volatile UNSIGNED32 Acceleration_Numerator;		//Numerator
	extern  volatile UNSIGNED32 Acceleration_Divisor;	//Velocity_Divisor
	  
//----------------------------------------------------------------------------
//------------------------ POSITION CONTROL FUNCTION--------------------------------------------------------
	extern  volatile INTEGER32 Position_Demand_Value;
	extern  volatile INTEGER32 Position_Demand_Value_aster;
	extern  volatile INTEGER32 Position_Acture_Value;
	extern  volatile INTEGER32 Position_Acture_Value_aster;
	extern  volatile UNSIGNED32 Following_Error_Window;
	extern  volatile UNSIGNED16 Following_Error_Time_Out;
	extern  volatile UNSIGNED32 Position_Window;
	extern  volatile UNSIGNED16 Position_Window_Time;
	extern  volatile INTEGER32 Following_Error_Actual_Value;
	extern  volatile INTEGER32 Control_Effort;
//----------------------PROFILE POSITION MODE------------------------------------------------------
	extern  volatile INTEGER32 Target_Position;//pp
	extern  volatile UNSIGNED8 Position_Range_NoOfEntries;
	extern  volatile INTEGER32 Min_Position_Range_Limit;
	extern  volatile INTEGER32 Max_Position_Range_Limit;

	extern  volatile UNSIGNED8 Software_Position_Limit;
	extern  volatile INTEGER32 Min_Position_Limit;
	extern  volatile INTEGER32 Max_Position_Limit;

	extern  volatile UNSIGNED32 Max_Profile_Velocity;//Max profile velocity
	extern  volatile UNSIGNED32 Max_Motor_Speed;	
	extern  volatile UNSIGNED32 Profile_Velocity;	//pp
	extern  volatile UNSIGNED32 END_Velocity;
	extern  volatile UNSIGNED32 Profile_Acceleration;	//pp pv
	extern  volatile UNSIGNED32 Profile_Decelaration;	//pp pv
	extern  volatile UNSIGNED32 Quick_Stop_Deceleration;	//pp pv
	extern  volatile UNSIGNED32 Profile_Acceleration_aster;				// 6083h
	extern  volatile UNSIGNED32 Profile_Decelaration_aster;				// 6084h
	extern  volatile UNSIGNED32 Quick_Stop_Deceleration_aster;			// 6085h 
	extern  volatile INTEGER16  Motion_Profile_Type;
	extern  volatile UNSIGNED32 Profile_Jerk1;	
	extern  volatile UNSIGNED32 Max_Acceleration;
	extern  volatile UNSIGNED32 Max_Deceleration;
//------------------------- Profile Velocity control-------------------------------------------------------
	extern  volatile INTEGER32 Velocity_Sensor_Actual_Value;
	extern  volatile INTEGER16 Sensor_Selection_Code;
	extern  volatile INTEGER32 Velocity_Demand_Value;
	extern  volatile INTEGER32 Velocity_Actual_Value;
	extern  volatile UNSIGNED16 Velocity_Window;
	extern  volatile UNSIGNED16 Velocity_Window_Time;
	extern  volatile UNSIGNED16 Velocity_Threshold;
	extern  volatile UNSIGNED16 Velocity_Threshold_Time;
	extern  volatile INTEGER32 Target_Velocity;
//-------------------------Interpolate Position-------------------------------------------------------
	extern  volatile INTEGER16 Interpolatio_Sub_Mode_Select;
	extern  volatile UNSIGNED8 Interpolation_Data_Record_NoOfEntries;
	extern  volatile INTEGER32 Interpolation_Data_Record1;	//*********还有子索引************
	extern  volatile INTEGER32 Interpolation_Data_Record2;
	extern  volatile UNSIGNED8 Interpolation_Time_Period;	//*********还有子索引************
	extern  volatile UNSIGNED8 Interpolation_Sync_Definition;//*******还有子索引**************
	extern  volatile UNSIGNED32 Interpolation_Data_Configuration;//*******还有子索引**************
//-------------------------Profile Torque Control-------------------------------------------------------
	extern  volatile INTEGER16 Target_Torque;
	extern  volatile UNSIGNED16 Max_Torque;
	extern  volatile UNSIGNED16 Max_Current;
	extern  volatile INTEGER16 Torque_Demand_Value;
	extern  volatile UNSIGNED32 Motor_Rated_Current;
	extern  volatile UNSIGNED32 Motor_Rated_Torque;
	extern  volatile INTEGER16 Torque_Actual_Value;
	extern  volatile INTEGER16 Current_Actual_Value;
	extern  volatile UNSIGNED32 Torque_Slope;
	extern  volatile INTEGER16 Torque_Profile_Type;

//------------------------HOMING MODE--------------------------------------------------------
	extern  volatile INTEGER32 Home_Offset;
	extern  volatile INTEGER8 Homing_Method;

	extern  volatile UNSIGNED8 Homing_Speeds_NoOfEntries;
	extern  volatile UNSIGNED32 Switch_Speed;
	extern  volatile UNSIGNED32 Zero_Speed;

	extern  volatile UNSIGNED32 Homing_Acceleration;
	extern  volatile UNSIGNED32 Homing_Acceleration_aster;


   #endif
/*0x1F80*/ extern ROM   UNSIGNED32     ODE_NMT_Startup;


/***** Manufacturer specific variables ****************************************/
		   extern volatile UNSIGNED32 ODE_TPDO_MASK[4][2];
/*0x2101*/ extern volatile unsigned int ODE_CANnodeID;
/*0x2102*/ extern volatile unsigned int ODE_CANbitRate;

/*0x6064*/	//extern INTEGER32 Position_Acture_Value_aster;
#endif
