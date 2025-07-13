/*******************************************************************************
   Object Dictionary
*******************************************************************************/
   //One entry in Object Dictionary
   typedef struct {
      unsigned int   index;               // Index of OD entry
      unsigned int   subindex;            // Subindex of OD entry
      unsigned int   attribute;           // Attributes
      unsigned int   length;              // Data length in bytes
      const void*    pData;               // POINTER to data (RAM or ROM memory)
   } CO_objectDictionaryEntry;

   //access attributes for object dictionary
   #define ATTR_RW      0x00    //attribute: read/write
   #define ATTR_WO      0x01    //attribute: write/only
   #define ATTR_RO      0x02    //attribute: read/only (TPDO may read from that entry)
   #define ATTR_CO      0x03    //attribute: read/only, constant value
   #define ATTR_RWR     0x04    //attribute: read/write on process input (TPDO may read from that entry)
   #define ATTR_RWW     0x05    //attribute: read/write on process output (RPDO may write to that entry)
   #define ATTR_RES1    0x06    //attribute: Reserved 1
   #define ATTR_RES2    0x07    //attribute: Reserved 2
   //additional attributes, must be '|' with access attributes
   #define ATTR_RES3    0x80    //attribute: Reserved 3
   #define ATTR_ROM     0x10    //attribute: ROM variable is saved in retentive memory
   #define ATTR_ADD_ID  0x20    //attribute: add NODE-ID to variable value (sizeof(variable)<=4)

   //Object Dictionary
   extern const CO_objectDictionaryEntry DPRAM_CO_OD[];
   //Number of Elements in Object Dictionary
   extern const unsigned int DPRAM_CO_OD_NoOfElements;
   //Function for search Object Dictionary
   const CO_objectDictionaryEntry* DPRAM_CO_FindEntryInOD(unsigned int index, unsigned int  subindex);


#define DPRAM_BASE               0x100000
#define EDBH_INDREG_ADD			 (volatile unsigned int *)(DPRAM_BASE + 0x00FFE)
#define ABUS_INDREG_ADD			 (volatile unsigned int *)(DPRAM_BASE + 0x00FFF)
#define INPUT_ADD		 		 (volatile unsigned int *)(DPRAM_BASE + 0x00000)
#define OUTPUT_ADD			 	 (volatile unsigned int *)(DPRAM_BASE + 0x00200)
#define MIN_ADD			 		 (volatile unsigned int *)(DPRAM_BASE + 0x00400)
#define MOUT_ADD			 	 (volatile unsigned int *)(DPRAM_BASE + 0x00410)
#define ModuleSoftVer_ADD	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FEA)// 1 RO
#define ModuleErrReg_ADD	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FEE)// 1 RO
//以下3个空间是测试Module是否正确跟主板连接用
#define CheckModuleA_ADD		 (volatile unsigned int *)(DPRAM_BASE + 0x00900)// 1 RW
#define CheckModuleB_ADD		 (volatile unsigned int *)(DPRAM_BASE + 0x00A00)// 1 RW
#define CheckModuleC_ADD		 (volatile unsigned int *)(DPRAM_BASE + 0x00B00)// 1 RW
#define CheckModuleD_ADD		 (volatile unsigned int *)(DPRAM_BASE + 0x00C00)// 1 RW
// 未来扩展用
#define BootVer_ADD			 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE0)// 1 RO
#define AppSoftVer_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE1)// 1 RO
#define BusSoftVer_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE2)// 1 RO
#define ModuleSN_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE3)// 2 RO
#define VendorID_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE5)// 1 RO
#define BusType_ADD			 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE6)// 1 RO
//#define ModuleSoftVer_ADD	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE7)// 1 RO
#define Resvered1_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE8)// 1 
#define WDCntIn_ADD			 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FE9)// 1 RW
#define WDCntOut_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FEA)// 1 RO
#define Resvered2_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FEB)// 2 
#define LEDStatus_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FED)// 2 RO
#define Resvered3_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FEF)// 1 
#define ModuleType_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FF0)// 1 RO master,slave,other
#define ModuleStatus_ADD	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FF1)// 1 RO Bit information:freeze,clear etc
#define ChangedDataField_AD	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FF2)// 4 RO Bit Field
#define EventCause_ADD	 	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FF6)// 1 RW Event cause register
#define EventSource_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FF7)// 1 RO 
#define InIOLength_ADD		 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FF8)// 1 RO
#define InDPRAMLength_ADD	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FF9)// 1 RO
#define InTotalLength_ADD	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FFA)// 1 RO
#define OutIOLength_ADD		     (volatile unsigned int *)(DPRAM_BASE + 0x00FFB)// 1 RO
#define OutDPRAMLength_ADD	 	 (volatile unsigned int *)(DPRAM_BASE + 0x00FFC)// 1 RO
#define OutTotalLength_ADD		 (volatile unsigned int *)(DPRAM_BASE + 0x00FFD)// 1 RO

#define CntrlMax				50
// Para command
#define IDLECOMM				0x0000
#define RDdata					0x1000
#define RDOneWord				0x1200
#define RDTwoWord				0x1400
#define WROneWord				0x2200
#define WRTwoWord				0x2400
#define UpdateGuard				0x3000
#define SetGuardOneWord			0x3200
#define SetGuardTwoWord			0x3400
#define RemoveIn				0x4000
#define MapInOneWord			0x4200
#define MapInTwoWord			0x4400
#define ClrMapIn				0x4F00
#define RemoveOut				0x5000
#define MapOutOneWord			0x5200
#define MapOutTwoWord			0x5400
#define ClrMapOut				0x5F00
#define AbortReq				0x6000
#define WRBusStatus				0x6200

// Response command
#define IDLE					0x0000
#define RDing					0x1000
#define RDOneWordOK				0x1200
#define RDTwoWordOK				0x1400
#define RDErr					0x1F00
#define WRing					0x2000
#define WROneWordOK				0x2200
#define WRTwoWordOK				0x2400
#define WRErr					0x2F00
#define Guarding				0x3000
#define TrGuardOneWord			0x3200
#define TrGuardTwoWord			0x3400
#define GuardReqOK				0x3500
#define GuardReqErr				0x3F00
#define MapINing				0x4000
#define MapInOK					0x4100
#define MapInErr				0x4F00
#define MapOUTing				0x5000
#define MapOutOK				0x5100
#define MapOutErr				0x5F00
#define RsBusReq				0x6000
#define WRBusStatusOK			0x6200

// Err code
#define ErrInvalidComm			0x08
#define ErrInvalidData			0x09
#define ErrRD_WR_Para			0x0E
#define ErrDeviceState			0x10
#define ErrLessData				0x13
#define ErrInvalidPara			0x14
#define ErrMuchData				0x15
#define ErrStore				0x19
#define ErrHighValue			0xD0
#define ErrLowValue				0xD1
#define ErrVender				0x1F

// Common Mailbox bits.
#define INDREG_MIN               0x80
#define INDREG_MOUT              0x40
#define INDREG_HS                0x10

// Common Area control bits.
#define INDREG_FB_CTRL           0x01
#define INDREG_OUT               0x02
#define INDREG_IN                0x04


// Application Indication register:
// Request/release and Locked bits.
#define APPINDREG_R_R            0x10
#define APPINDREG_L              0x08

// Area request/release control bit mask (bits R/R, L, IN, OUT and FB/CTRL).
#define APPIND_REQ_BITS          0x1F


// Anybus Indication Register:
// Initialisation indication bit.
#define ABINDREG_INIT            0x10

/*------------------------------------------------------------------------------
**
** Mailbox Header Message Information bitmasks [ABS-DGP 6.2.2].
**
**------------------------------------------------------------------------------
*/
#define MBH_INFO_ERR_MASK        0x80
