#include "EDBclm.h"			// define and function declare
#include "globalvar.h"		//  
#include "DPRAM.h"
	
//if entries in DPRAM_CO_OD are ordered (from lowest to highest index, then subindex). 
//For longer Object Dictionaries searching is much faster. 
//If entries are not ordered, disable macro.

//const Uint16	ODE_RW_SetProdcnt_NoOfEntries = 1;
//const Uint16	ODE_RW_Prodcnt_NoOfEntries = 1;

//const Uint16	ODE_RW_CAM_NoOfEntries = 2;
//const Uint16	ODE_RW_PN_NoOfEntries = 1;
//const Uint16	ODE_RW_UN_NoOfEntries1 = 4;
//const Uint16	ODE_RW_UN_NoOfEntries2 = 3;
//const Uint16	ODE_RW_UN_NoOfEntries3 = 1;

// debug
int resver1 = 0;
int resver2 = 0;
int resver3 = 0;
int resver4 = 0;
int32 resver5 = 0;
/******************************************************************************/
/*     OBJECT DICTIONARY                                                      */
/* Entries in DPRAM_CO_OD[] must be ordered. If not, disable macro DPRAM_CO_OD_IS_ORDERED */
/******************************************************************************/
//macro for add entry to object dictionary
#define DPRAM_OD_ENTRY(Index, Subindex, Attribute, Variable) \
     {Index, Subindex, Attribute, (sizeof(Variable)), (const void*)&Variable}

const CO_objectDictionaryEntry DPRAM_CO_OD[] = {

/***** Manufacturer specific **************************************************/

/***** Manufacturer profile****************************************************/
// Pn	  
// Un 
	  DPRAM_OD_ENTRY(1010, 0x00, ATTR_RW, profibus_buf.pbus_SPEEDorIqr),
	  DPRAM_OD_ENTRY(1011, 0x00, ATTR_RO, Unspd),
	  DPRAM_OD_ENTRY(1400, 0x00, ATTR_RW, profibus_buf.pbus_STWnew),
	  DPRAM_OD_ENTRY(1401, 0x00, ATTR_RW, resver1),
	  DPRAM_OD_ENTRY(1402, 0x00, ATTR_RW, resver2),
	  DPRAM_OD_ENTRY(1403, 0x00, ATTR_RO, profibus_buf.pbus_ZSW),
	  DPRAM_OD_ENTRY(1404, 0x00, ATTR_RW, resver3),
	  DPRAM_OD_ENTRY(1405, 0x00, ATTR_RW, resver4),
	  DPRAM_OD_ENTRY(1406, 0x00, ATTR_RO, profibus_buf.pbus_PGout),
	  DPRAM_OD_ENTRY(1407, 0x00, ATTR_RW, resver5),

	  DPRAM_OD_ENTRY(1500, 0x00, ATTR_RO, Un[0]), 
      DPRAM_OD_ENTRY(1501, 0x00, ATTR_RO, Un[1]),
      DPRAM_OD_ENTRY(1502, 0x00, ATTR_RO, Un[2]),
      DPRAM_OD_ENTRY(1503, 0x00, ATTR_RO, Un[3]),
      DPRAM_OD_ENTRY(1504, 0x00, ATTR_RO, Un[4]),
      DPRAM_OD_ENTRY(1505, 0x00, ATTR_RO, Un[5]),
	  DPRAM_OD_ENTRY(1506, 0x00, ATTR_RO, Un[6]),
	  DPRAM_OD_ENTRY(1507, 0x00, ATTR_RO, Un[7]),
	  DPRAM_OD_ENTRY(1508, 0x00, ATTR_RO, Un[8]),
	  DPRAM_OD_ENTRY(1509, 0x00, ATTR_RO, alarmno),

	  DPRAM_OD_ENTRY(1600, 0x00, ATTR_RW, profibus_buf.pbus_input),
	  DPRAM_OD_ENTRY(1601, 0x00, ATTR_RO, Torque),
	  DPRAM_OD_ENTRY(1602, 0x00, ATTR_RO, servo_state.all),
	  	  
	  DPRAM_OD_ENTRY(2000, 0x00, ATTR_RW, password),
// DPRAM 
	  //DPRAM_OD_ENTRY(963, 0x00, ATTR_RW, Un[0]), 


/***** Device profile for Generic I/O *****************************************/
};

/***** Number of Elements in Object Dictionary ********************************/
const unsigned int DPRAM_CO_OD_NoOfElements = sizeof(DPRAM_CO_OD) / sizeof(DPRAM_CO_OD[0]);

#define DPRAM_CO_OD_IS_ORDERED
/*******************************************************************************
   CO_FindEntryInOD - SEARCH OBJECT DICTIONARY
   Function is used for searching object dictionary for entry with specified
   index and subindex. It searches OD from beginning to end and if matched, returns
   poiner to entry. It is usually called from SDO server.
   If Object Dictionary exist in multiple arrays, this function must search all.

   PARAM index, subindex: address of entry in object dictionary
   RETURN: if found, pointer to entry, othervise 0
*******************************************************************************/
const CO_objectDictionaryEntry* DPRAM_CO_FindEntryInOD(unsigned int index, unsigned int subindex){
   static unsigned int Index;
   static unsigned int SubIndex;
   #ifdef DPRAM_CO_OD_IS_ORDERED
      //Fast search in ordered Object Dictionary. If indexes or subindexes are mixed, this won't work.
      //If DPRAM_CO_OD has up to 2^N entries, then N is max number of loop passes.
      static unsigned int cur, min, max, CurIndex;
      static unsigned int CurSubIndex;
      Index = index;
      SubIndex = subindex;
      min = 0;
      max = DPRAM_CO_OD_NoOfElements - 1;
      while(min < max)
      {		
    	 cur = (min + max) / 2;
         CurIndex = DPRAM_CO_OD[cur].index;
         CurSubIndex = DPRAM_CO_OD[cur].subindex;
         if(Index == CurIndex && SubIndex == CurSubIndex)
		 {
            return &DPRAM_CO_OD[cur];         
         }
         else if(Index < CurIndex || (Index == CurIndex && SubIndex < CurSubIndex))
         {
            max = cur;
            if(max) max--;
         }
         else
		 {
            min = cur + 1;
		 }
      }
	  if(max > 15)
	  {
		asm("	nop");
	  }
      if(min == max)
      {
         cur = min;
         CurIndex = DPRAM_CO_OD[cur].index;
         CurSubIndex = DPRAM_CO_OD[cur].subindex;
         if(Index == CurIndex && SubIndex == CurSubIndex)
         {
         	return &DPRAM_CO_OD[cur];
		 }
      }
   #else
      //search OD from first to last entry
      static unsigned int i;
      const CO_objectDictionaryEntry* pODE;
      Index = index;
      SubIndex = subindex;
      pODE = &DPRAM_CO_OD[0];
      for(i = DPRAM_CO_OD_NoOfElements; i>0; i--){
         if(Index == pODE->index && SubIndex == pODE->subindex) 
         {
         	return pODE;
         }
         pODE++;
      }
   #endif
   return 0;  //object does not exist in OD
}



void DPRAM_memcpy1(unsigned int *DestAddr, unsigned int *SourceAddr, unsigned int memsize)
{
    while(memsize--)
	{
		*DestAddr++ = *SourceAddr++;
	}
    return;
}

/*******************************************************************************
   DPRAM_CO_OD_Read - OBJECT DICTIONARY READ
   This is mainline function. It is called from SDO server and reads data from
   object dictionary. Since object dictionary variables can use many types of
   memory, some code is processor specific.

   PARAM pODE: pointer to object dictionary entry, found by CO_FindEntryInOD().
   PARAM pBuff: pointer to buffer where data will be written.
   PARAM BuffMaxSize: Size of buffer (to prevent overflow).
   RETURN: 0 if success, otherwise abort code
*******************************************************************************/
unsigned long DPRAM_CO_OD_Read(const CO_objectDictionaryEntry* pODE, void* pBuff, unsigned int sizeOfBuff){
   
   	if((pODE->attribute&0x07) == ATTR_WO) return 1;  //attempt to read a write-only object
   	if(sizeOfBuff < pODE->length)
   	{
   		return 2;  // less data
	}
	else if(sizeOfBuff > pODE->length)
	{
   		return 3;  // Much data
	}

   	//read data from memory (processor specific code)
   	if(!(pODE->attribute&ATTR_ROM)){
      	//CO_DISABLE_ALL();
      	DPRAM_memcpy1(pBuff, (void*)pODE->pData, pODE->length);
      	//CO_ENABLE_ALL();
   	}
   	return 0;
}

/*******************************************************************************
   DPRAM_CO_OD_Write - OBJECT DICTIONARY WRITE
   This is mainline function. It is called from SDO server and writes data to
   object dictionary. Since object dictionary variables can use many types of
   memory, some code is processor specific.

   PARAM pODE: pointer to object dictionary entry, found by CO_FindEntryInOD().
   PARAM pBuff: pointer to buffer which contains data to be written.
   PARAM dataSize: Size of data in buffer.
   RETURN: 0 if success, otherwise abort code
*******************************************************************************/
unsigned long DPRAM_CO_OD_Write(const CO_objectDictionaryEntry* pODE, void* data, unsigned int length)
{   
   	if((pODE->attribute&0x07) == ATTR_RO || (pODE->attribute&0x07) == ATTR_CO) return 1;   //attempt to write a read-only object
   	if(length < pODE->length)   //Length of service parameter does not match
   	{
	 	return 2;  // less data
   	}
   	else if(length > pODE->length)
   	{
	 	return 3;  // Much data
   	}
   //write data to memory (processor specific code)
   //RAM data
   if((pODE->attribute&ATTR_ROM) == 0){
      //CO_DISABLE_ALL();
      DPRAM_memcpy1((void*)pODE->pData, data, pODE->length);
      //CO_ENABLE_ALL();
   }
   return 0;
}

