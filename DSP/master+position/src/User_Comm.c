#include "EDBclm.h"			// define and function declare
#include "globalvar.h"		// 

void SCI_ISRnotModbus(void)
{Uint16 ComCheckSum,CalCheckSum,ComLength,i;
 Uint32 RxbufferIndexAdd;
	oldsci_flag.bit.notTrail = 0;
	if(GpioDataRegs.GPBDAT.bit.GPIO61)
	{
		i = ScicRegs.SCIRXBUF.all;
	}
	else if(ScicRegs.SCIRXST.bit.RXERROR)
	{// rewrong
		i = ScicRegs.SCIRXBUF.all;
		oldsci_flag.bit.resend = 1;
		RxbufferIndex = & Rxbuffer[0];
	}
	else
	{
		if(oldsci_flag.bit.SaveSingleComData)
		{
		}
		else
		{
			* RxbufferIndex = ScicRegs.SCIRXBUF.all & 0x00FF;
			RxbufferIndexAdd = (Uint32)(RxbufferIndex+1);
			if(* RxbufferIndex++ == 0x58)	
			{
				oldsci_flag.bit.notTrail = 0;
				if((RxbufferIndexAdd - (Uint32)&Rxbuffer[0]) >= 4)
				{				
					* RxbufferIndex--;
					* RxbufferIndex--;					// trail
					ComCheckSum = * RxbufferIndex--;	// checksum
					ComLength   = * RxbufferIndex--;	// length
					i = ComLength;
					if(ComLength > 0)
					{	CalCheckSum = 0;
						for(;i>0;i--)
						{
							CalCheckSum += * RxbufferIndex--;
						}
						CalCheckSum &= 0x00FF;
						if(CalCheckSum != ComCheckSum)
						{// rewrong2
					 		if((RxbufferIndexAdd - (Uint32)&Rxbuffer[0]) > 256)
							{
								RxbufferIndex = & Rxbuffer[0];
								oldsci_flag.bit.resend = 1;
							}
							else
							{
								RxbufferIndex = (Uint16 *)RxbufferIndexAdd;;
								oldsci_flag.bit.notTrail = 1;
							}
						}
						else
						{
							if(* RxbufferIndex++ != 0x17)
							{// rewrong2
						 		if((RxbufferIndexAdd - (Uint32)&Rxbuffer[0]) > 256)
								{
									RxbufferIndex = & Rxbuffer[0];
									oldsci_flag.bit.resend = 1;
								}
								else
								{
									RxbufferIndex = (Uint16 *)RxbufferIndexAdd;//& Rxbuffer[0];
									oldsci_flag.bit.notTrail = 1;
								}
							}
							else
							{
								if(ComLength > 251)		// 有效信息长度不超过252字节
								{// rewrong
									oldsci_flag.bit.resend = 1;
									RxbufferIndex = & Rxbuffer[0];
								}
								else
								{
									if(* RxbufferIndex++ == 0xFF)
									{// singleCOM
										oldsci_flag.bit.SaveSingleComData = 1;
										commvar1 = (* RxbufferIndex++)<<8;
										commvar1 |= (* RxbufferIndex++);
										commvar2 = (* RxbufferIndex++)<<8;
										commvar2 |= (* RxbufferIndex++);
										commprd = (* RxbufferIndex++)<<8;
										commprd |= (* RxbufferIndex++);
										commvar1 += ((Uint32)0x8000);
										commvar2 += ((Uint32)0x8000);
										RxbufferIndex = & Rxbuffer[0];
										wComm = SAMPLE_BUF;			// 保存的首地址
										rComm = SAMPLE_BUF;
									}
									else if(* RxbufferIndex++ == 0x0F)
									{	// doubleCOM
								 		oldsci_flag.bit.SaveDoubleComData = 1;
										// CLRB	xflag2,bit11 sci_flag.bit.
										// ...
										// ...
										commvar1 = (* RxbufferIndex++)<<8;
										commvar1 |= (* RxbufferIndex++);
										commvar2 = (* RxbufferIndex++)<<8;
										commvar2 |= (* RxbufferIndex++);
										commprd = (* RxbufferIndex++)<<8;
										commprd |= (* RxbufferIndex++);
										commvar1 += ((Uint32)0x8000);
										commvar2 += ((Uint32)0x8000);
										RxbufferIndex = & Rxbuffer[0];
									}
									else
									{	// commdowith
								 	// ...
									}
								}
							}
						}					
					}
					else
					{// rewrong2
				 		if((RxbufferIndexAdd - (Uint32)&Rxbuffer[0]) > 256)
						{
							RxbufferIndex = & Rxbuffer[0];
							oldsci_flag.bit.resend = 1;
						}
						else
						{
							RxbufferIndex = (Uint16 *)RxbufferIndexAdd;;
							oldsci_flag.bit.notTrail = 1;
						}
					}
				}
			}
			else
			{
				oldsci_flag.bit.notTrail = 1;			// if 10ms not occur trail , err
			}					
		}
	}
//	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

//save 1000 组数据
void SaveCommData(void)
{Uint16 * temp;
 Uint32 * temp1;
	if(memSCIsel && sci_oper_flag.bit.sciStartSample)
	{
		if(!sci_oper_flag.bit.SciSampleing)
		{
			if(sci_data.SCISampleTrig == 0)
			{
				sci_oper_flag.bit.SciSampleing = 1;
			}
			else if(sci_data.SCISampleTrig == 1)
			{
				if (commvar1 == (Uint32)(& co_abs_count) || commvar1 == (Uint32)(& Pfcount))
				{
					if (UnCurPos > sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else if (commvar1 == (Uint32)(& Ek))
				{
					if (Ek > sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else if (commvar1 == (Uint32)(& UnPg_count))
				{
					if (UnPg > sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else
				{
					temp = (Uint16 *)commvar1;
					if((int16)(* temp) > sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
			}
			else if(sci_data.SCISampleTrig == 2)
			{
				if (commvar1 == (Uint32)(& co_abs_count) || commvar1 == (Uint32)(& Pfcount))
				{
					if (UnCurPos < sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else if (commvar1 == (Uint32)(& Ek))
				{
					if (Ek < sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else if (commvar1 == (Uint32)(& UnPg_count))
				{
					if (UnPg < sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else
				{
					temp = (Uint16 *)commvar1;
					if((int16)(* temp) < sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
			}
			else if(sci_data.SCISampleTrig == 3)
			{
				if (commvar1 == (Uint32)(& co_abs_count) || commvar1 == (Uint32)(& Pfcount))
				{
					if (UnCurPos != sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else if (commvar1 == (Uint32)(& Ek))
				{
					if (Ek != sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else if (commvar1 == (Uint32)(& UnPg_count))
				{
					if (UnPg != sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
				else
				{
					temp = (Uint16 *)commvar1;
					if((int16)(* temp) != sci_data.SCISampleTrigV1)
					{
						sci_oper_flag.bit.SciSampleing = 1;
					}
				}
			}
		}
		if(sci_oper_flag.bit.SciSampleing)
		{
			if(++Timers.SCI_commprd >= sci_data.SCISampleTime)
			{
				Timers.SCI_commprd = 0;
				if(!TestPara.Test_Mode)
				{
					if(savecnt < 4000)
					{
						temp = (Uint16 *)commvar1;
						//Comm->data[savecnt++] = * temp;
						*wComm++ = * temp;
						temp = (Uint16 *)commvar2;
						//Comm->data[savecnt++] = * temp;
						*wComm++ = * temp;
						temp = (Uint16 *)commvar3;
						//Comm->data[savecnt++] = * temp;
						*wComm++ = * temp;
						temp = (Uint16 *)commvar4;
						//Comm->data[savecnt++] = * temp;
						*wComm++ = * temp;
						savecnt += 4;

					}
					if(savecnt >= 4000)
					{
						TestData.ScanFinish=0;
						sci_oper_status.bit.sciSampleStatus = 2;
						sci_oper_flag.bit.sciStartSample=0;//
						sci_oper_flag.bit.SciSampleing=0;
					}
				}
				else
				{
					if(savecnt < 1000)
					{
						savecnt++;
						if(commvar1 != 0x123)
						{
							if(Save32bit[0])
							{
								temp1 = (Uint32 *)commvar1;
								*wComm++ = (* temp1) & 0xFFFF;
								*wComm++ = ((* temp1)>>16) & 0xFFFF;
							}
							else
							{
								temp = (Uint16 *)commvar1;
								*wComm++ = * temp;
							}
						}
						if(commvar2 != 0x123)
						{
							if(Save32bit[1])
							{
								temp1 = (Uint32 *)commvar2;
								*wComm++ = (* temp1) & 0xFFFF;
								*wComm++ = ((* temp1)>>16) & 0xFFFF;
							}
							else
							{
								temp = (Uint16 *)commvar2;
								*wComm++ = * temp;
							}
						}
						if(commvar3 != 0x123)
						{
							if(Save32bit[2])
							{
								temp1 = (Uint32 *)commvar3;
								*wComm++ = (* temp1) & 0xFFFF;
								*wComm++ = ((* temp1)>>16) & 0xFFFF;
							}
							else
							{
								temp = (Uint16 *)commvar3;
								*wComm++ = * temp;
							}
						}
						if(commvar4 != 0x123)
						{
							if(Save32bit[3])
							{
								temp1 = (Uint32 *)commvar4;
								*wComm++ = (* temp1) & 0xFFFF;
								*wComm++ = ((* temp1)>>16) & 0xFFFF;
							}
							else
							{
								temp = (Uint16 *)commvar4;
								*wComm++ = * temp;
							}
						}
					}
					if(savecnt >= 1000 || TestData.ScanFinish)
					{
						TestData.ScanFinish=0;
						sci_oper_status.bit.sciSampleStatus = 2;
						sci_oper_flag.bit.sciStartSample=0;//
						sci_oper_flag.bit.SciSampleing=0;
						TestPara.Scan_Number=savecnt;
					}
				}
			}
		}
	}
	else if(memSCIsel == 0)
	{	
		if(oldsci_flag.bit.SaveSingleComData || oldsci_flag.bit.SaveDoubleComData)
		{		
			if(++Timers.SCI_commprd >= commprd)
			{
				Timers.SCI_commprd = 0;
				if(savecnt < 2000)
				{
					temp = (Uint16 *)commvar1;
					//Comm->data[savecnt++] = * temp;
					*wComm++ = * temp;
					temp = (Uint16 *)commvar2;
					//Comm->data[savecnt++] = * temp;
					*wComm++ = * temp;
					savecnt += 2;
				}
			}
		}
	}
}

void CommTxData(void)
{
	if(ScicRegs.SCIRXST.bit.RXERROR)
	{
		InitSci();
	}
	if(savecnt == 2000)
	{
		HVD33Txd();								// HVD33发送状态
		if(oldsci_flag.bit.SaveDoubleComData)
		{
			asm("	SETC	INTM");
			oldsci_flag.bit.SaveDoubleComData = 0;
			oldsci_flag.bit.UPdrawdata = 1;
			asm("	CLRC	INTM");
			savecnt = 0;
			drawcount = 0;
			// Dcomm16
			// ...
		}
		else
		{// singleSEND
			asm("	SETC	INTM");
			oldsci_flag.bit.SaveSingleComData = 0;
			asm("	CLRC	INTM");
			if(loadcnt == 4000)
			{
				savecnt = 0;
				loadcnt = 0;
				// end_Comm
				// ...
				state_flag2.bit.CommDelay = 1;
			}
			else
			{
				if(ScicRegs.SCICTL2.bit.TXEMPTY)
				{
					if((loadcnt & 0x0003) == 0)
					{
						/*Xbuffer[0] = ((Comm->data[loadcnt>>1])>>8) & 0x00FF;
						Xbuffer[1] = (Comm->data[loadcnt>>1]) & 0x00FF;
						Xbuffer[2] = ((Comm->data[(loadcnt>>1) + 1])>>8) & 0x00FF;
						Xbuffer[3] = (Comm->data[(loadcnt>>1) + 1]) & 0x00FF;*/
						Xbuffer[0] = (* rComm >> 8) & 0x00FF;
						Xbuffer[1] = (* rComm++)  & 0x00FF;
						Xbuffer[2] = (* rComm >> 8) & 0x00FF;
						Xbuffer[3] = (* rComm++)  & 0x00FF;
					}					
					ScicRegs.SCITXBUF = Xbuffer[(loadcnt & 0x0003)];
					loadcnt ++;
				}
				// end_Comm
				// ...
			}
		}
	}
	// end_Comm
	// ...
//	if (state_flag2.bit.CommDelay  &&  Timers.CommDelayTime >= 20)
	if (state_flag2.bit.CommDelay  && ScicRegs.SCICTL2.bit.TXEMPTY && ScicRegs.SCICTL2.bit.TXRDY)
	{
			HVD33Rxd();								// HVD33接受状态
			state_flag2.bit.CommDelay = 0;
		
	}
}
