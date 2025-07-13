/*******************************************************************************/
#include "CANopen.h"
/*******************************************************************************
   User_Process1msIsr - 1 ms USER TIMER FUNCTION
   Function is executed every 1 ms. It is deterministic and has priority over
   mainline functions.

   If TPDO is configured for Change of State transmission (Transmission_type >= 254), then consider next
   situation: On network startup one node is started first and send PDO of curent state, which will not
   change soon. Another node is started later and missed PDO from first node??? Solution might be
   Event timer.
*******************************************************************************/
extern UNSIGNED8 ODE_RPDO_Mapping_NoOfEntries[8];
extern UNSIGNED8 ODE_TPDO_Mapping_NoOfEntries[8];
extern volatile Uint16 TPDO_MAPPing_Flag[4];
extern volatile Uint16 RPDO_MAPPing_Flag[4];

volatile unsigned long TPDO_Old_Data[CO_NO_TPDO][2];
#if RAM_RUN
#pragma CODE_SECTION(User_Process01msIsr, "ramfuncs2");
#endif
//---------------------------------------------------------------------
void User_Process01msIsr(void){
//   static unsigned char LastStateOperationalGradePrev = 0;
//   unsigned char LastStateOperationalGrade = 0;
   extern volatile unsigned int CO_TPDO_InhibitTimer[CO_NO_TPDO];  //Inhibit timer used for inhibit PDO sending - if 0, TPDO is not inhibited

	unsigned int temp,data_changed,i,j;
	unsigned long temp1,newtmp1,newtmp2;
	volatile unsigned char * pData;
	#if CO_NO_SYNC > 0
	static Uint16 CO_SYNCcounter_old[4] = {0,0,0,0};
	#endif
   //PDO Communication
   if(CO_NMToperatingState == NMT_OPERATIONAL){
//      LastStateOperationalGrade ++;
      //verify operating state of monitored nodes
      #if CO_NO_CONS_HEARTBEAT > 0
      if(CO_HBcons_AllMonitoredOperational == NMT_OPERATIONAL)
      {
      #endif		
//       LastStateOperationalGrade ++;
         //RPDO 
		 for(j = 0; j < CO_NO_RPDO; j++)
		 {
			if(CO_RPDO_New(j) && ((ODE_RPDO_Parameter[j].Transmission_type == 255) || (CO_SYNCcounter_old[j] != CO_SYNCcounter)) 
               && (!RPDO_MAPPing_Flag[j]) && (ODE_RPDO_Mapping_NoOfEntries[j]))
			{		
				  pData = & CO_RXCAN[CO_RXCAN_RPDO+j].Data.BYTE[0];
				  for(i = 0; i< ODE_RPDO_Mapping_NoOfEntries[j]; i++)
				  {
					 if(CO_RPDO_MAPing[j][i].length == 1)
					 {//word
                 	 	temp =(unsigned int)(* pData++);	
						*((Uint16 *)CO_RPDO_MAPing[j][i].pData) = temp;
					 }
					 else if(CO_RPDO_MAPing[j][i].length == 2)
					 {//word
                 	 	temp =(unsigned int)(* pData++);
						temp +=((unsigned int)(* pData++)<<8);
                 	 	//temp =(unsigned int)((CO_RXCAN[CO_RXCAN_RPDO+j].Data.BYTE[Word_Count + 1]<<8)+CO_RXCAN[CO_RXCAN_RPDO+j].Data.BYTE[Word_Count]);	
						*((Uint16 *)CO_RPDO_MAPing[j][i].pData) = temp;
					 }
					 else// if(CO_RPDO_MAPing[j][i].length == 4)
					 {//dword
                 	 	temp1 = (unsigned int)(* pData++);
						temp1 +=((unsigned int)(* pData++)<<8);
						temp1 +=((unsigned long)(* pData++)<<16);
						temp1 +=((unsigned long)(* pData++)<<24);
                 	 	//temp1=(((unsigned long)CO_RXCAN[CO_RXCAN_RPDO+j].Data.BYTE[Word_Count + 3]<<24)+((unsigned long)CO_RXCAN[CO_RXCAN_RPDO+j].Data.BYTE[Word_Count + 2]<<16)
                 	 	//		+(CO_RXCAN[CO_RXCAN_RPDO+j].Data.BYTE[Word_Count+1]<<8)+CO_RXCAN[CO_RXCAN_RPDO+j].Data.BYTE[Word_Count]);
						*((Uint32 *)CO_RPDO_MAPing[j][i].pData) = temp1;
					 }
				  }	 
                  CO_RPDO_New(j) = 0;
				  //CO_SYNCcounter_old[j] = CO_SYNCcounter;
			}
		 }
      #if CO_NO_CONS_HEARTBEAT > 0
	  }
	  else
	  {
		  CO_RPDO_New(0)=0;
		  CO_RPDO_New(1)=0;
		  CO_RPDO_New(2)=0;
		  CO_RPDO_New(3)=0;
      }// end if(CO_HBcons_AllMonitoredOperational == NMT_OPERATIONAL)
      #endif
		//
		CO_SYNCcounter_old[0] = CO_SYNCcounter;
		CO_SYNCcounter_old[1] = CO_SYNCcounter;
		CO_SYNCcounter_old[2] = CO_SYNCcounter;
		CO_SYNCcounter_old[3] = CO_SYNCcounter;
	  //TPDO
	 for(j = 0; j < CO_NO_TPDO; j++)
	 {		
		if(ODE_TPDO_Mapping_NoOfEntries[j] && (!TPDO_MAPPing_Flag[j]) 
		    && (ODE_TPDO_Parameter[j].COB_ID & 0x80000000L) == 0)
        {
           //TPDO×ÖµÄ¸öÊý 
		   pData = & CO_TPDO(j).BYTE[0];
		   for(i = 0; i < ODE_TPDO_Mapping_NoOfEntries[j];i++)
		   {			   
				if(CO_TPDO_MAPing[j][i].length == 1)
				{//word
					temp = *((Uint16 *)CO_TPDO_MAPing[j][i].pData);
					//CO_TPDO(j).BYTE[Word_Count++] = temp & 0xFF;
					*pData++ = temp & 0xFF;
				}				
				else if(CO_TPDO_MAPing[j][i].length == 2)
				{//word
					temp = *((Uint16 *)CO_TPDO_MAPing[j][i].pData);
					*pData++ = temp & 0xFF;	
           			*pData++ = (temp>>8) & 0xFF;
				}				
				else// if(CO_TPDO_MAPing[j][i].length == 4)
				{//dword
					temp1 = *((Uint32 *)CO_TPDO_MAPing[j][i].pData);
					*pData++ = temp1 & 0xFF;	
           			*pData++ = (temp1>>8) & 0xFF;
					*pData++ = (temp1>>16) & 0xFF;	
           			*pData++ = (temp1>>24) & 0xFF;
           									
           		}
		   }
		   data_changed = 0;
		   if(ODE_TPDO_Parameter[j].Transmission_type == 254)
		   {	
			   	pData = & CO_TPDO(j).BYTE[0];
			   	newtmp1 = * pData++;
			   	newtmp1 += (* pData++)<<8;
				newtmp1 += (Uint32)(* pData++)<<16;
				newtmp1 += (Uint32)(* pData++)<<24;

				newtmp2 = * pData++;
				newtmp2 += (* pData++)<<8;
				newtmp2 += (Uint32)(* pData++)<<16;
				newtmp2 += (Uint32)(* pData)<<24;
			    if((newtmp1 & ODE_TPDO_MASK[j][0]) != (TPDO_Old_Data[j][0] & ODE_TPDO_MASK[j][0]) 
			    || (newtmp2 & ODE_TPDO_MASK[j][1]) != (TPDO_Old_Data[j][1] & ODE_TPDO_MASK[j][1]))
			    {
					data_changed = 1;
			    }
		   }
		   //if((CO_TPDO_InhibitTimer[j] == 0) && (data_changed || (ODE_TPDO_Parameter[j].Transmission_type == 255)))
		   if((CO_TPDO_InhibitTimer[j] == 0) && (data_changed))
		   {
              CO_TPDOsend(j);
           }
        }
	 }
   } //end if(CO_NMToperatingState == NMT_OPERATIONAL)


//   if(LastStateOperationalGrade < LastStateOperationalGradePrev){ //NMT_OPERATIONAL (this or monitored nodes) was just lost
//      SwitchOffNode();
//   }
//   LastStateOperationalGradePrev = LastStateOperationalGrade;
}

//=============================================================================
// End of file.
//=============================================================================
