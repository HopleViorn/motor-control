#include "VerInfo.h"
#include "EDBclm.h"			// define and function declare
#include "globalvar.h"		//  
#include "Table.h"

//=================================================================================
// �˵����������̴������
//=================================================================================
void Menu(void)
{
int16 temp1,temp2,i;
	if(sci_oper_flag.bit.SciLoadDefault)
	{
		LoadDefaultPar();
	}
	if(state_flag2.bit.DispAlm && (state_flag2.bit.ClrMenu == 0) && !key_flag1.bit.loadDefault)
	{
		MainMenuPage = 0;
		KeyPressFlag = 0;
		KeyUndoFlag = 0;
		MenuLayer = 0;
		DoneTimer = 0;
		FinishFlag = 0;
		StartFlag = 0;
		SHIFT_PnNum = 0;
		lSHIFT_PnNum = 0;
		SHIFTPNbits = 0;
		EditFlag = 0;
		EditBit = 0;
		key_flag1.bit.JOGneg = 0;
		key_flag1.bit.JOGpos = 0;
		key_flag1.bit.JOGrun = 0;
		key_flag1.bit.DSP_PLD_Var = 0;
		key_flag1.bit.AnAuto = 0;
		key_flag1.bit.CurAuto = 0;
		key_flag1.bit.JInertia = 0;
		state_flag2.bit.HandPhase = 0;
	}
//---------------------------------------------------------------------------------
// һ���˵������˵���
//---------------------------------------------------------------------------------
	if (MenuLayer==0)
	{
		if (KeyUndoFlag==MODE ) // ��MODE���л����˵�ҳ��
		{
			if(!SHIFT_PnNum && !lSHIFT_PnNum)
			{
				MainMenuPage++;
				if (MainMenuPage>3)
				{
					MainMenuPage=0;
				}
			}
			lSHIFT_PnNum = 0;
		}
		switch(MainMenuPage)
		{
            //------------------------------------------------
			// ״̬��ʾҳ��
            //------------------------------------------------
			case 0:	
				LedSegment[4] = StateBit[1];
				LedSegment[3] = StateBit[0];
				if(state_flag2.bit.DispAlm)
				{
					state_flag2.bit.ClrMenu = 1;
					// ��λflag0���˼��������ɿ���������־
					// ��λflag1����дFRAM��������־
					state_flag2.bit.JOGsv_on = 0;	// ���JOGservo-on(����)
					KeyPressFlag &= DATA;			// ���KeyPressFlag,����DATA
					if(KeyPressFlag==DATA) 			// ��DATA�������ǰ������ʾ
					{
						state_flag2.bit.DispAlm = 0;
						state_flag2.bit.HaveAlm = 0;
						state_flag2.bit.ClrAlm = 1;
					}
					if(key_flag1.bit.AlmSaved == 0)
					{
						key_flag1.bit.AlmSaved = 1;
						Pn[SumAddr] -= Pn[Fn000Addr+9];
						CheckSum -= Pn[Fn000Addr+9];
						for (i=9;i>0;i--)
						{
							Pn[Fn000Addr+i]=Pn[Fn000Addr+i-1];
						}
						Pn[Fn000Addr] = alarmno;
						Pn[SumAddr] += alarmno;//Pn[Fn000Addr];
						CheckSum += alarmno;
						SPI_WriteFRAM(Fn000Addr,&Pn[Fn000Addr],10);
						SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
					}
					HexToBcd(alarmno);
					LedSegment[2] = 0x00BF; // A.
					LedSegment[1] = LedCode[BcdCode[1]]; // AlarmNo.
					LedSegment[0] = LedCode[BcdCode[0]]; // 
				}
				else
				{
					state_flag2.bit.ClrMenu = 0;
					key_flag1.bit.AlmSaved = 0;
					alarmno = 0;
					if (state_flag2.bit.Pot || state_flag2.bit.Not)
					{
						if (state_flag2.bit.Pot && state_flag2.bit.Not)
						{
							if(TimeComp2)
							{
								LedSegment[2] = 0x00AB; // P
								LedSegment[1] = 0x00E4; // o
								LedSegment[0] = 0x00E8;	// t
							}
							else
							{
								LedSegment[2] = 0x00A4;	// n
								LedSegment[1] = 0x00E4;	// o
								LedSegment[0] = 0x00E8;	// t
							}
						}
						else if (state_flag2.bit.Pot)
						{
							LedSegment[2] = 0x00AB;	// P
							LedSegment[1] = 0x00E4;	// o
							LedSegment[0] = 0x00E8;	// t
						}
						else if (state_flag2.bit.Not)
						{
							LedSegment[2] = 0x00A4;	// n
							LedSegment[1] = 0x00E4;	// o
							LedSegment[0] = 0x00E8;	// t
						}
						/*else
						{
							LedSegment[2] = 0x0000;
							LedSegment[1] = 0x0000;
							LedSegment[0] = 0x0000;
						}*/
					}
					//else if (SonFlag==1)
					else if (state_flag2.bit.Son)
					{
						LedSegment[2] = 0x00A0;	// r	
						LedSegment[1] = 0x00C4;	// u
						LedSegment[0] = 0x00A4;	// n
					}
					else
					{
						LedSegment[2] = 0x0000;	  
						LedSegment[1] = 0x00EC;	// b 
						LedSegment[0] = 0x00EC; // b
					}
				}
				break;
            //------------------------------------------------
			// ����ѡ��ҳ��
            //------------------------------------------------
			case 1:	
				//----������Χ----
				if (backcode == 9)
				{
					PnNumRange = PnMax;   //345   =Pn873
				}
				else
				{
					PnNumRange = PnSee;  //260
				}
				//----��������----
				if (KeyPressFlag == MODAT)
				{
					SHIFT_PnNum ^= 0x0001;
					SHIFTPNbits = 0;
					lSHIFT_PnNum = 1;
				}
				else if (KeyUndoFlag == MODAT)
				{
					lSHIFT_PnNum = 0;
				}
				else if (KeyUndoFlag==UP || (KeyHoldFlag==UP && KeyTimer>5 && TimeFlag1==1)) // ��UP�����Ӻ���
				{
					temp2 = PnNum; // �жϼ�֮ǰԭ�����������
					if(!SHIFT_PnNum)
					{
						if (KeyTimer>10)
						{
							PnNum += 1;
						}
						else if (KeyTimer>15)
						{
							PnNum += 5;
						}
						else if (KeyTimer>20)
						{
							PnNum += 10;
						}
						else 
						{
							PnNum++;
						}
					}
					else if(TimeFlag1!=1)
					{
						switch(SHIFTPNbits)
						{
							case 0:
								PnNum++;
								break;
							case 1:
								PnNum += 10;
								break;
							case 2:
								PnNum += 100;
								break;
							default:
								break;
						}
					
					}
					if(PnNum >= Pn9GropBeg && backcode == 15)
					{
						if(temp2 < Pn9ModelBeg)
						{
							PnNum = Pn9ModelBeg;			// �����������
						}
						else
						{
							if(PnNum > Pn9ModelBeg + Pn9ModelLen - 1)
							{
								PnNum = 0;
							}
						}
					}
					else if(PnNum >= Pn9GropBeg && backcode != 9)
					{
						PnNum = 0;
					}
					else if(PnNum >= Pn8GropBeg + Pn8GropLen && PnNum < Pn9GropBeg)
					{
						if(temp2 < Pn8GropBeg)
						{
							PnNum = Pn8GropBeg;
						}
						else if(backcode == 9)
						{
							PnNum = Pn9GropBeg;
						}
						else if(backcode == 15)
						{
							PnNum = Pn9ModelBeg;
						}
						else
						{
							PnNum = 0;
						}
					}
					else if(PnNum >= Pn7GropBeg + Pn7GropLen && PnNum < Pn8GropBeg)
					{
						if(temp2 < Pn7GropBeg)
						{
							PnNum = Pn7GropBeg;
						}
						else
						{
							PnNum = Pn8GropBeg;
						}
					}
					else if(PnNum >= Pn6GropBeg + Pn6GropLen && PnNum < Pn7GropBeg)
					{
						if(temp2 < Pn6GropBeg)
						{
							PnNum = Pn6GropBeg;
						}
						else
						{
							PnNum = Pn7GropBeg;
						}
					}
					else if(PnNum >= Pn5GropBeg + Pn5GropLen && PnNum < Pn6GropBeg)
					{
						if(temp2 < Pn5GropBeg)
						{
							PnNum = Pn5GropBeg;
						}
						else
						{
							PnNum = Pn6GropBeg;
						}
					}
					else if(PnNum >= Pn4GropBeg + Pn4GropLen && PnNum < Pn5GropBeg)
					{
						if(temp2 < Pn4GropBeg)
						{
							PnNum = Pn4GropBeg;
						}
						else
						{
							PnNum = Pn5GropBeg;
						}
					}
					else if(PnNum >= Pn3GropBeg + Pn3GropLen && PnNum < Pn4GropBeg)
					{
						if(temp2 < Pn3GropBeg)
						{
							PnNum = Pn3GropBeg;
						}
						else
						{
							PnNum = Pn4GropBeg;
						}
					}
					else if(PnNum >= Pn2GropBeg + Pn2GropLen && PnNum < Pn3GropBeg)
					{
						if(temp2 < Pn2GropBeg)
						{
							PnNum = Pn2GropBeg;
						}
						else
						{
							PnNum = Pn3GropBeg;
						}
					}
					else if(PnNum >= Pn1GropBeg + Pn1GropLen && PnNum < Pn2GropBeg)
					{
						if(temp2 > Pn8GropBeg)
						{
							PnNum = 0;
						}
						else
						{
							PnNum = Pn2GropBeg;
						}
					}										 	
					if (PnNum >= Pn9GropBeg + Pn9GropLen)
					{
						if(temp2 < Pn9GropBeg)
						{
							PnNum = Pn9GropBeg;
						}
						else
						{
							PnNum = 0;
						}
					}
				}
				else if (KeyUndoFlag==DOWN || (KeyHoldFlag==DOWN && KeyTimer>5 && TimeFlag1==1)) // ��DOWN����С����
				{
					if(!SHIFT_PnNum)
					{
						if (KeyTimer>10)
						{
							PnNum -= 2;
						}
						else if (KeyTimer>15)
						{
							PnNum -= 5;
						}
						else if (KeyTimer>20)
						{
							PnNum -= 10;
						}
						else 
						{
							PnNum--;
						}
					}
					else if(TimeFlag1!=1)
					{
						switch(SHIFTPNbits)
						{
							case 0:
								PnNum--;
								break;
							case 1:
								PnNum -= 10;
								break;
							case 2:
								PnNum -= 100;
								break;
							default:
								break;
						}
					}

					if(PnNum < Pn9ModelBeg && PnNum >= Pn9GropBeg && backcode == 15)
					{
						PnNum = Pn8GropBeg + Pn8GropLen - 1;
					}
					else if(PnNum >= Pn8GropBeg + Pn8GropLen && PnNum < Pn9GropBeg)
					{
						PnNum = Pn8GropBeg + Pn8GropLen - 1;
					}
					else if(PnNum >= Pn7GropBeg + Pn7GropLen && PnNum < Pn8GropBeg)
					{
						PnNum = Pn7GropBeg + Pn7GropLen - 1;
					}
					else if(PnNum >= Pn6GropBeg + Pn6GropLen && PnNum < Pn7GropBeg)
					{
						PnNum = Pn6GropBeg + Pn6GropLen - 1;
					}
					else if(PnNum >= Pn5GropBeg + Pn5GropLen && PnNum < Pn6GropBeg)
					{
						PnNum = Pn5GropBeg + Pn5GropLen - 1;
					}
					else if(PnNum >= Pn4GropBeg + Pn4GropLen && PnNum < Pn5GropBeg)
					{
						PnNum = Pn4GropBeg + Pn4GropLen - 1;
					}
					else if(PnNum >= Pn3GropBeg + Pn3GropLen && PnNum < Pn4GropBeg)
					{
						PnNum = Pn3GropBeg + Pn3GropLen - 1;
					}
					else if(PnNum >= Pn2GropBeg + Pn2GropLen && PnNum < Pn3GropBeg)
					{
						PnNum = Pn2GropBeg + Pn2GropLen - 1;
					}
					else if(PnNum >= Pn1GropBeg + Pn1GropLen && PnNum < Pn2GropBeg)
					{
						PnNum = Pn1GropBeg + Pn1GropLen - 1;
					}

					if (PnNum<0)
					{
						if(backcode == 9)
						{
							//PnNum = Pn9GropBeg + Pn9GropLen - 1;
						    PnNum = Pn9GropBeg + 61;//Pn9GropLen - 1;
						}
						else if(backcode == 15)
						{
							PnNum = Pn9ModelBeg + Pn9ModelLen - 1;
						}
						else
						{
							PnNum = Pn8GropBeg + Pn8GropLen - 1;
						}
					}
				}
				else if (KeyUndoFlag==DATA) // ��DATA����������˵�			//wxw if (KeyPressFlag==DATA)
				{
					temp1 = PnNum / 100;
					temp2 = PnNum % 100;
					switch(temp1)
					{
						case 0:
							EditIndex = Pn1FactBeg + temp2;
							break;
						case 1:
							EditIndex = Pn2FactBeg + temp2;
							break;
						case 2:
							EditIndex = Pn3FactBeg + temp2;
							break;
						case 3:
							EditIndex = Pn4FactBeg + temp2;
							break;
						case 4:
							EditIndex = Pn5FactBeg + temp2;
							break;
						case 5:
							EditIndex = Pn6FactBeg + temp2;
							break;
						case 6:
							EditIndex = Pn7FactBeg + temp2;
							break;
						case 7:
							EditIndex = Pn8FactBeg + temp2;
							break;
						case 8:
							EditIndex = Pn9FactBeg + temp2;
							break;
						default:
							break;
					}					
					if(!SHIFT_PnNum && !lSHIFT_PnNum)
					{
						MenuLayer = 1;
						if (PnAttribute[EditIndex][0]==0)
						{
							EditContent = Pn[EditIndex];
						}
						else
						{
							EditContent = (int16)Pn[EditIndex];
						}
					}
					else if (!lSHIFT_PnNum)
					{	
						SHIFTPNbits++;
						if(SHIFTPNbits >= 3)
						{
							SHIFTPNbits = 0;
						}
					}
					lSHIFT_PnNum = 0;
				}
				else if(KeyIn == 0)
				{
					lSHIFT_PnNum = 0;
				}
				//----��ʾ����----
				HexToBcd(PnNum);
				LedSegment[4] = 0x00AB;	// P
				LedSegment[3] = 0x00A4;	// n
				LedSegment[2] = LedCode[BcdCode[2]];
				LedSegment[1] = LedCode[BcdCode[1]];
				LedSegment[0] = LedCode[BcdCode[0]];
				if(SHIFT_PnNum && TimeComp3==1)
				{
					LedSegment[SHIFTPNbits] |= 0x0010;
				}
				break;
            //------------------------------------------------
			// ����ѡ��ҳ��
            //------------------------------------------------
			case 2:	
				//----��ط�Χ----
				if (backcode == 9)
				{
					UnNumRange = UnMax;
				}
				else
				{
					UnNumRange = UnSee;
				}
				//----��������----
				if (KeyUndoFlag==UP || (KeyHoldFlag==UP && KeyTimer>5 && TimeFlag1==1)) // ��UP�����Ӻ���
				{
					UnNum++;	
					if (UnNum>UnNumRange) 
					{
						UnNum = 0;
					}
				}
				if (KeyUndoFlag==DOWN || (KeyHoldFlag==DOWN && KeyTimer>5 && TimeFlag1==1)) // ��DOWN����С����
				{	
					UnNum--;
					if (UnNum<0)
					{
						UnNum = UnNumRange;
					}
				}
				if (KeyUndoFlag==DATA) // ��DATA����������˵�
				{	
					MenuLayer = 1;
				}
				//----��ʾ����----
				HexToBcd(UnNum);
				LedSegment[4] = 0x00CD;	// U
				LedSegment[3] = 0x00A4;	// n
				LedSegment[2] = LedCode[BcdCode[2]];
				LedSegment[1] = LedCode[BcdCode[1]];
				LedSegment[0] = LedCode[BcdCode[0]];
				break;
            //------------------------------------------------
			// ��������ѡ��ҳ��
            //------------------------------------------------
			case 3:
				//----Fn��Χ----
				if (backcode == 9)
				{
					FnNumRange = FnMax;
				}
				else
				{
					FnNumRange = FnSee;
				}				
				//----��������----
				key_flag1.bit.JOGrun = 0;
				state_flag2.bit.JOGsv_on = 0;
				state_flag2.bit.HandPhase = 0;
				asm("	SETC	INTM");					
				int_flag3.bit.HaveFindInitPos = 0;
				asm("	CLRC	INTM");
				if (KeyPressFlag==UP || (KeyHoldFlag==UP && KeyTimer>5 && TimeFlag1==1)) // ��UP�����Ӻ���
				{
					if (FnNum == 9)
					{
						#if ENC_TYPE == ENC17BIT
					    FnNum++;
						#else 
						FnNum += 3;	  //2500��û��Fn010��Fn011����
						#endif
					}
					else FnNum++;						
					if(FnNum > FnNumRange) 
					{
						FnNum = 0;
					}
				}
				if (KeyPressFlag==DOWN || (KeyHoldFlag==DOWN && KeyTimer>5 && TimeFlag1==1)) // ��DOWN����С����
				{
					if (FnNum == 12)
					{
						#if ENC_TYPE == ENC17BIT
					    FnNum--;
						#else 
						FnNum -= 3;	  //2500��û��Fn010��Fn011����
						#endif
					}
					else FnNum--;	
					if(FnNum < 0)
					{
						FnNum = FnNumRange;
					}
				}
				if (KeyUndoFlag==DATA) // ��DATA����������˵�
				{	
					MenuLayer = 1;
				}
				//----��ʾ����----
				HexToBcd(FnNum);
				LedSegment[4] = 0x00AA;	// F
				LedSegment[3] = 0x00A4;	// n
				LedSegment[2] = LedCode[BcdCode[2]];
				LedSegment[1] = LedCode[BcdCode[1]];
				LedSegment[0] = LedCode[BcdCode[0]];
				break;
			default:
				break;
		}
	}
//---------------------------------------------------------------------------------
// �����˵�
//---------------------------------------------------------------------------------
	else if (MenuLayer==1)
	{
		switch(MainMenuPage)
		{
            //------------------------------------------------
			// ����������ʾ
            //------------------------------------------------
			case 1:	
				//----��������----
				if (KeyHoldFlag==DATA && KeyTimer>10 && KeyIntoFlag==0) // ��DATA 1s�����������˵����༭ҳ�棩	// wxw if (KeyPressFlag==MODE)
				{	
					KeyIntoFlag = 1;									// ��ֹ�ڶ����͵������ڳ���ʱ�����л�
					MenuLayer = 2;
					KeepEbitFlag = 1;
				}
				if (KeyUndoFlag==MODE || KeyUndoFlag==DATA && HoldCmpLostFlag==0) // ��DATA������һ���˵�			//wxw if (KeyPressFlag==DATA)
				{	
					Pn[SumAddr] -= Pn[EditIndex];				// HoldCmpLostFlag �ɵ������˵ڶ���ʱ��������ɿ���Ч
					CheckSum -= Pn[EditIndex];
					Pn[EditIndex] = EditContent;
					Pn[SumAddr] += Pn[EditIndex];
					CheckSum += Pn[EditIndex];
					SPI_WriteFRAM(EditIndex,&Pn[EditIndex],1);
					SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
					MenuLayer = 0;
					EditFlag = 0;
					EditBit = 0;
				}
				ModifyContent();
				break;
            //------------------------------------------------
			// ��������
            //------------------------------------------------
			case 2:	
				//----��������----
				if (KeyUndoFlag==DATA) // ��DATA������һ����?
				{	
					MenuLayer = 0;
				}
				//----��ʾ����----
				Undisplay(Un[UnNum],UnAttribute[UnNum]);
				break;
            //------------------------------------------------
			// ��������ҳ��
            //------------------------------------------------
			case 3:	
				switch(FnNum)
				{	
            		//------------------------------------------------
					// ��������Fn000����ʾ������ʷ���ݣ���10����
            		//------------------------------------------------
					case AlmLog: 
						//----��������----
						if (KeyHoldFlag==DATA && KeyTimer>10 && StartFlag==0) // ��סMODE��1��������б�����ʷ����
						{
							StartFlag = 1;
							ResumeIndex = 0;
						}
						if (KeyPressFlag==UP || (KeyHoldFlag==UP && KeyTimer>5 && TimeFlag1==1)) // ��UP�����Ӻ���
						{	
							if (Fn000Index<9)
							{
								Fn000Index++;
							}
						}
						if (KeyPressFlag==DOWN || (KeyHoldFlag==DOWN && KeyTimer>5 && TimeFlag1==1)) // ��DOWN����С����
						{	
							if (Fn000Index>0)
							{
								Fn000Index--;
							}
						}
						if (KeyUndoFlag==DATA) // ��DATA������һ����
						{	
							MenuLayer = 0;
						}
						//----�������----
						if (StartFlag==1)
						{
							if (ResumeIndex<10)
							{
								Pn[SumAddr] -= Pn[Fn000Addr+ResumeIndex];
								CheckSum -= Pn[Fn000Addr+ResumeIndex];
								Pn[Fn000Addr+ResumeIndex] = 0;
								SPI_WriteFRAM(Fn000Addr+ResumeIndex,&Pn[Fn000Addr+ResumeIndex],1);
								SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
								ResumeIndex++; 
							}
							else 
							{
								//SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
								FinishFlag = 1;
							}
						}	
						//----��ʾ����----
						HexToBcd(Pn[Fn000Addr+Fn000Index]);
						LedSegment[4] = LedCode[Fn000Index];
						LedSegment[3] = 0x0020;	// -
						LedSegment[2] = 0x00BF; // A.
						LedSegment[1] = LedCode[BcdCode[1]];
						LedSegment[0] = LedCode[BcdCode[0]];
						break;
            		//------------------------------------------------
					// ��������Fn001���ָ���������ֵ
            		//------------------------------------------------
					case LoadDefault:
						//----��������----
						if (KeyHoldFlag==DATA && KeyTimer>10 && StartFlag==0 && (!state_flag2.bit.Son)) // ��סMODE��1��ָ���������ֵ
						{	
							StartFlag = 1;
							ResumeIndex = 0;
							key_flag1.bit.loadDefault = 1;
						}
						if (KeyUndoFlag==DATA) // ��DATA������һ���˵�
						{	
							MenuLayer = 0;
						}
						//----�ָ�����----
						if (StartFlag==1)
						{
							if(!FinishFlag)
							{
								LoadDefaultPar();									
							}						
						}
 						//----��ʾ����----
						if (DoneTimer>0 && DoneTimer<12)
						{							
							if (TimeComp3==1)
							{
								LedSegment[4] = 0x0000;
								LedSegment[3] = 0x00E5; // d
								LedSegment[2] = 0x00E4; // o
								LedSegment[1] = 0x00A4; // n
								LedSegment[0] = 0x00EA; // E
							}
							else
							{
								LedSegment[4] = 0x0000;
								LedSegment[3] = 0x0000;
								LedSegment[2] = 0x0000;
								LedSegment[1] = 0x0000;
								LedSegment[0] = 0x0000;
							}
						}
						else 
						{	
							LedSegment[4] = 0x0000;
							LedSegment[3] = 0x00C8; // L
							LedSegment[2] = 0x00CF; // O
							LedSegment[1] = 0x00AF; // A
							LedSegment[0] = 0x00E5; // d
						}
						break;
            		//------------------------------------------------
					// ��������Fn002���㶯��JOG������ģʽ
            		//------------------------------------------------
					case JOGope:
						//----��������----
						if (KeyUndoFlag == MODE) // ��MODE���л�JOG����
						{	
							if(state_flag2.bit.SvReady)
							{
								state_flag2.bit.JOGsv_on ^= 1;
							}
						}
						if(state_flag2.bit.JOGsv_on)
						{
							if((KeyPressFlag == UP) || (KeyHoldFlag == UP))
							{
								key_flag1.bit.JOGpos = 1;
							}
							else
							{
								key_flag1.bit.JOGpos = 0;
							}
							if((KeyPressFlag == DOWN) || (KeyHoldFlag == DOWN))
							{
								key_flag1.bit.JOGneg = 1;
							}
							else
							{
								key_flag1.bit.JOGneg = 0;
							}
						}
						key_flag1.bit.JOGrun = 1;
						if (KeyUndoFlag==DATA) // ��DATA������һ���˵�
						{	
							MenuLayer = 0;
						}						
						//----��ʾ����----
						LedSegment[4] = StateBit[1];
						LedSegment[3] = StateBit[0];
						LedSegment[2] = 0x00C5; // J
						LedSegment[1] = 0x00CF; // O
						LedSegment[0] = 0x00CE; // G
						break;
            		//------------------------------------------------
					// ��������Fn003��ģ��ָ���Զ�����
            		//------------------------------------------------
					case SpdErrAutoAdjust:
						//----��������----
						if (KeyUndoFlag==MODE)
						{	
							key_flag1.bit.AnAuto = 1;
							asm("	SETC	INTM");
							Timers.cntdone = 0;
							Sspd = 0;
							Stcr = 0;
							asm("	CLRC	INTM");
						}
						if (KeyUndoFlag==DATA) // ��DATA������һ���˵�
						{	
							key_flag1.bit.AnAuto = 0;
							asm("	SETC	INTM");
							Timers.cntdone = 0;
							Sspd = 0;
							Stcr = 0;
							asm("	CLRC	INTM");
							MenuLayer = 0;
						}
						if (Timers.cntdone>=2000)
						{
							temp1 = Sspd/Timers.cntdone;
							temp2 = Stcr/Timers.cntdone;
							Pn[SumAddr] = Pn[SumAddr] - spdoffset -TCRoffset;
							if(labs(temp1) <= 1024)
							{
								spdoffset = temp1;
								Pn[Fn004Addr] = temp1;
							}
							else
							{
								spdoffset = 0;
								Pn[Fn004Addr] = 0;
							}
							if(labs(temp2) <= 1024)
							{
								TCRoffset = temp2;
								Pn[Fn004Addr+1] = temp2;
							}
							else
							{
								TCRoffset = 0;
								Pn[Fn004Addr+1] = 0;
							}
							Pn[SumAddr] = Pn[SumAddr] + spdoffset + TCRoffset;
							CheckSum = Pn[SumAddr];
							key_flag1.bit.AnAuto = 0;
							asm("	SETC	INTM");
							Timers.cntdone = 0;
							Sspd = 0;
							Stcr = 0;
							asm("	CLRC	INTM");
							SPI_WriteFRAM(Fn004Addr,&Pn[Fn004Addr],1);
							SPI_WriteFRAM(Fn004Addr+1,&Pn[Fn004Addr+1],1);
							SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
						}
 						//----��ʾ����----
						if (key_flag1.bit.AnAuto)
						{							
							if (TimeComp3==1)
							{
								LedSegment[4] = 0x0000;
								LedSegment[3] = 0x00E5; // d
								LedSegment[2] = 0x00E4; // o
								LedSegment[1] = 0x00A4; // n
								LedSegment[0] = 0x00EA; // E
							}
							else
							{
								LedSegment[4] = 0x0000;
								LedSegment[3] = 0x0000;
								LedSegment[2] = 0x0000;
								LedSegment[1] = 0x0000;
								LedSegment[0] = 0x0000;
							}
						}
						else 
						{	
							LedSegment[4] = 0x00A0; // r
							LedSegment[3] = 0x00EA; // E
							LedSegment[2] = 0x00AA; // F
							LedSegment[1] = 0x0040; // _
							LedSegment[0] = 0x00E4; // o
							//...
						}
						break;
            		//------------------------------------------------
					// ��������Fn004��ģ��ָ���ֶ�����
            		//------------------------------------------------
					case SpdErrManuAdjust:
						//----��������----
						if (KeyHoldFlag==DATA && KeyTimer>10 && KeyIntoFlag==0) // ��DATA 1s�����������˵����༭ҳ�棩
						{	
							KeyIntoFlag = 1;									// ��ֹ�ڶ����͵������ڤ��ʱ�����л?
							MenuLayer = 2;
							KeepEbitFlag = 1;
							EditIndex = Fn004Addr+Fn004Index;
							EditContent = (int16)Pn[EditIndex];
						}
						if (KeyUndoFlag==MODE)
						{	
							Fn004Index = ~Fn004Index&0x0001;
						}
						if (KeyUndoFlag==DATA && HoldCmpLostFlag==0) // ��DATA������һ���˵�
						{	
							MenuLayer = 0;
						}
						//----��ʾ����----
						LedSegment[4] = LedCode[Fn004Index];
						LedSegment[3] = 0x0020; // -
						if (Fn004Index==0)
						{
							LedSegment[2] = 0x006E; // S
							LedSegment[1] = 0x00AB; // P
							LedSegment[0] = 0x00E5; // d
						}
						else
						{
							LedSegment[2] = 0x008A; // T
							LedSegment[1] = 0x00E0; // c
							LedSegment[0] = 0x00A0; // r
						}
						break;
            		//------------------------------------------------
					// ��������Fn005����������Զ�����
            		//------------------------------------------------
					case MotorIerrAutoAdjust:
						//----��������----
						if (KeyUndoFlag==MODE && int_flag3.bit.adjCurZero && !state_flag2.bit.Son) // �ڲ��������
						{
							key_flag1.bit.CurAuto = 1;
							asm("	SETC	INTM");
							Timers.cntdone = 0;
							Scur1 = 0;
							Scur2 = 0;
							#if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
							int_flag3.bit.PWM_Charge = 1;		//���������Զ����㣬��ҪPWM���
							int_flag3.bit.Volt_Reached = 0;
							Timers.ChargeTimer = 0;
							#endif
							asm("	CLRC	INTM");
						}
						if (KeyUndoFlag==DATA) // ��DATA������һ���˵�
						{
							key_flag1.bit.CurAuto = 0;
							asm("	SETC	INTM");
							Timers.cntdone = 0;
							Scur1 = 0;
							Scur2 = 0;
							#if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
							int_flag3.bit.PWM_Charge = 0;		//ȡ�����㣬����PWM���
							int_flag3.bit.Volt_Reached = 0;
							Timers.ChargeTimer = 0;
							#endif
							asm("	CLRC	INTM");
							MenuLayer = 0;
						}
						if (Timers.cntdone>=2000)
						{
							temp1 = Scur1/Timers.cntdone;
							temp2 = Scur2/Timers.cntdone;
							Pn[SumAddr] = Pn[SumAddr] - Iuoffset -Ivoffset;
							if(labs(temp1) <= 1024)
							{
								Iuoffset = temp1;
								Pn[Fn006Addr] = temp1;
							}
							else
							{
								Iuoffset = 0;
								Pn[Fn006Addr] = 0;
							}
							if(labs(temp2) <= 1024)
							{
								Ivoffset = temp2;
								Pn[Fn006Addr+1] = temp2;
							}
							else
							{
								Ivoffset = 0;
								Pn[Fn006Addr+1] = 0;
							}
							Pn[SumAddr] = Pn[SumAddr] + Iuoffset + Ivoffset;
							CheckSum = Pn[SumAddr];
							key_flag1.bit.CurAuto = 0;
							asm("	SETC	INTM");
							Timers.cntdone = 0;
							Scur1 = 0;
							Scur2 = 0;
							#if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
							int_flag3.bit.PWM_Charge = 0;		//������ɺ󣬳���PWM���
							int_flag3.bit.Volt_Reached = 0;
							Timers.ChargeTimer = 0;
							#endif
							asm("	CLRC	INTM");
							SPI_WriteFRAM(Fn006Addr,&Pn[Fn006Addr],1);
							SPI_WriteFRAM(Fn006Addr+1,&Pn[Fn006Addr+1],1);
							SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
						}
 						//----��ʾ����----
						if (key_flag1.bit.CurAuto)
						{							
							if (TimeComp3==1)
							{
								LedSegment[4] = 0x0000;
								LedSegment[3] = 0x00E5; // d
								LedSegment[2] = 0x00E4; // o
								LedSegment[1] = 0x00A4; // n
								LedSegment[0] = 0x00EA; // E
							}
							else
							{
								LedSegment[4] = 0x0000;
								LedSegment[3] = 0x0000;
								LedSegment[2] = 0x0000;
								LedSegment[1] = 0x0000;
								LedSegment[0] = 0x0000;
							}
						}
						else 
						{	
							LedSegment[4] = 0x00CA; // C
							LedSegment[3] = 0x00C4; // u
							LedSegment[2] = 0x00A0; // r
							LedSegment[1] = 0x0040; // _
							LedSegment[0] = 0x00E4; // o
							//...
						}
						break;
            		//------------------------------------------------
					// ��������Fn006����������ֶ�����
            		//------------------------------------------------
					case MotorIerrManuAdjust:
						//----��������----
						if (KeyHoldFlag==DATA && KeyTimer>10 && KeyIntoFlag==0 
							&& int_flag3.bit.adjCurZero) // ��DATA 1s�����������˵����༭ҳ�棩
						{	
							KeyIntoFlag = 1;									// ��ֹ�ڶ����͵������ڳ���ʱ�����л�
							MenuLayer = 2;
							KeepEbitFlag = 1;
							EditIndex = Fn006Addr+Fn006Index;
							EditContent = (int16)Pn[EditIndex];
						}
						if (KeyUndoFlag==MODE)
						{	
							Fn006Index = ~Fn006Index&0x0001;
						}
						if (KeyUndoFlag==DATA && HoldCmpLostFlag==0) // ��DATA������һ���˵�
						{	
							MenuLayer = 0;
						}
						//----��ʾ����----
						LedSegment[4] = LedCode[Fn006Index];
						LedSegment[3] = 0x0020; // -
						if (Fn006Index==0)
						{
							LedSegment[2] = 0x00CA; // C
							LedSegment[1] = 0x00C4; // u
							LedSegment[0] = 0x00AF; // A
						}
						else
						{
							LedSegment[2] = 0x00CA; // C 
							LedSegment[1] = 0x00C4; // u
							LedSegment[0] = 0x00EC; // b
						}
						break;
            		//------------------------------------------------
					// ��������Fn007���ŷ�����汾��
            		//------------------------------------------------
					case SoftVar:
						//----��������----
						if (KeyUndoFlag==DATA) // ��DATA������һ���˵�
						{	
							MenuLayer = 0;
							back_cnt = 0;
							backcode &= 0x000F;
						}
						else if(KeyUndoFlag == MODE)
						{
						Uint16 numVendor;
							if(ModuleStatus.bit.HS || state_flag2.bit.EXrignt)
							{
								numVendor = 4;								
							}
							else
							{
								numVendor = 3;
							}
							vendor_cnt++;
						  	if(vendor_cnt >= numVendor)
						  	{
								vendor_cnt = 0;
						  	}
						}
						else if(KeyUndoFlag==UP)
						{
							switch(back_cnt)
							{
								case 0:
									backcode |= 0x0001;
									break;
								case 1:
									backcode |= 0x0002;
									break;
								case 2:
									backcode |= 0x0004;
									break;
								case 3:
									backcode |= 0x0008;
									break;
								default:
									break;
							}
							back_cnt++;
						}
						else if(KeyUndoFlag==DOWN)
						{
							switch(back_cnt)
							{
								case 0:
									backcode &= 0xFFFE;
									break;
								case 1:
									backcode &= 0xFFFD;
									break;
								case 2:
									backcode &= 0xFFFB;
									break;
								case 3:
									backcode &= 0xFFF7;
									break;
								default:
									break;
							}
							back_cnt++;
						}
						//----��ʾ����----
						switch(vendor_cnt)
						{
							case 0:		//��ѹ�ȼ�
								#if AC_VOLT == AC_200V
								LedSegment[4] = 0x0020;	// -
								LedSegment[3] = LedCode[2];	// 2
								LedSegment[2] = LedCode[0];	// 0
								LedSegment[1] = LedCode[0];	// 0
								LedSegment[0] = 0x0020;	// -
								#elif AC_VOLT == AC_400V
								LedSegment[4] = 0x0020; // -
								LedSegment[3] = LedCode[4];	// 4
								LedSegment[2] = LedCode[0];	// 0
								LedSegment[1] = LedCode[0];	// 0
								LedSegment[0] = 0x0020;	// -
								#endif
								break;
							case 1:
								LedSegment[4] = LedCode[Edition.hex.hex03]; // A:1.5KW
								LedSegment[3] = 0x0020; // -
								LedSegment[2] = LedCode[Edition.hex.hex02];
								LedSegment[1] = LedCode[Edition.hex.hex01];
								LedSegment[0] = LedCode[Edition.hex.hex00];
								LedSegment[2] |= 0x0010; //.
								break;
							case 2:
								HexToBcd(CpldEdition);
								LedSegment[4] = 0x00AB; // P:CPLD
								LedSegment[3] = 0x0020; // -
								LedSegment[2] = LedCode[BcdCode[2]];
								LedSegment[1] = LedCode[BcdCode[1]];
								LedSegment[0] = LedCode[BcdCode[0]];
								LedSegment[2] |= 0x0010; //.
								break;
							case 3:
								if(state_flag2.bit.EXrignt)
								{
									HexToBcd(EXAnaEdition);
								}
								else if(ModuleStatus.bit.HS)
								{
									HexToBcd(DP100Edition);
								}
								else
								{
									HexToBcd(0);
								}
								LedSegment[4] = 0x00E4; // o:DP100 or EXAna
								LedSegment[3] = 0x0020; // -
								LedSegment[2] = LedCode[BcdCode[2]];
								LedSegment[1] = LedCode[BcdCode[1]];
								LedSegment[0] = LedCode[BcdCode[0]];
								LedSegment[2] |= 0x0010; //.
								break;
							default:
								break;
						}
						break;
            		//------------------------------------------------
					// ��������Fn008��λ��ʾ��
            		//------------------------------------------------
					case PosTeach:
						//----��������----
						if (KeyHoldFlag==DATA && KeyTimer>10 && StartFlag==0) // ��סMODE��1��λ��ʾ��
						{	
							StartFlag = 1;
						}
						if (KeyUndoFlag==DATA) // ��DATA������һ���˵�
						{
							MenuLayer = 0;
						}
						//----λ��ʾ��----
						if (StartFlag==1 && speed == 0)
						{

							Pfcount = Pn[PositionTeach+1];
							Pfroate = Pn[PositionTeach];
							Pgplus = 0;

							FinishFlag = 1;
						}
 						//----��ʾ����----
						if (DoneTimer>0 && DoneTimer<12)
						{
							if (TimeComp3==1)
							{
								LedSegment[4] = 0x0000;
								LedSegment[3] = 0x00E5; // d
								LedSegment[2] = 0x00E4; // o
								LedSegment[1] = 0x00A4; // n
								LedSegment[0] = 0x00EA; // E
							}
							else
							{
								LedSegment[4] = 0x0000;
								LedSegment[3] = 0x0000;
								LedSegment[2] = 0x0000;
								LedSegment[1] = 0x0000;
								LedSegment[0] = 0x0000;
							}
						}
						else
						{
							LedSegment[4] = 0x00E8; // t
							LedSegment[3] = 0x00EA; // E
							LedSegment[2] = 0x00AF; // A
							LedSegment[1] = 0x00CA; // C
							LedSegment[0] = 0x00AD; // H
						}
						break;
            		//------------------------------------------------
					// ��������Fn009���Զ�����
            		//------------------------------------------------
					case AutoPhase:
						//----��������----
						if (KeyUndoFlag==DATA) // ��DATA������һ���˵�
						{	
							MenuLayer = 0;
							key_flag1.bit.wEncState = 0;
						}
						if (KeyUndoFlag == MODE)
						{
						  	state_flag2.bit.HandPhase = 1;
						}

 						//----��ʾ����----
						if(key_flag1.bit.wEncState)
						{
							HexToBcd(encoder);
							LedSegment[4] = 0x0020;
							LedSegment[3] = 0x0020;
							LedSegment[2] = 0x0020;
							LedSegment[1] = 0x0020;
							LedSegment[0] = 0x0020;
						}
						else if (state_flag2.bit.HandPhase)
						{							
							HexToBcd(encoder);
							LedSegment[4] = LedCode[BcdCode[4]];
							LedSegment[3] = LedCode[BcdCode[3]];
							LedSegment[2] = LedCode[BcdCode[2]];
							LedSegment[1] = LedCode[BcdCode[1]];
							LedSegment[0] = LedCode[BcdCode[0]];
							if(AsTeta == 90 && KeyUndoFlag == UP && !int_flagx.bit.wEEPROM)
							{
								EncData[0] = memDriveSel;
								EncData[1] = memMotorSel;
								EncData[2] = encoder & 0x00FF;
								EncData[3] = (encoder & 0xFF00)>> 8;
								EncData[4] = memPgSel;
								EncData[5] = 0xAA;
								EncData[6] = (EncData[0] + EncData[1] + EncData[2]+ EncData[3] + EncData[4] + EncData[5]) & 0x00FF;
								asm("	SETC	INTM");
								int_flagx.bit.wEEPROM = 1;
								asm("	CLRC	INTM");
								key_flag1.bit.wEncState = 1;

							}
						}
						else 
						{	
							LedSegment[4] = 0x00AB; // P
							LedSegment[3] = 0x00AD; // H
							LedSegment[2] = 0x00AF; // A
							LedSegment[1] = 0x006E; // S
							LedSegment[0] = 0x00EA; // E
						}
						break;
            		//------------------------------------------------
					// ��������Fn010������ʶ��
            		//------------------------------------------------
					case Inertia:
						if (KeyUndoFlag == DATA)
						{
							MenuLayer = 0;
							key_flag1.bit.JInertia = 0;
							asm("	SETC	INTM");
							int_flag3.bit.J_initia = 0;
							int_flag3.bit.J_End = 0;
							asm("	CLRC	INTM");
						}
						else if (KeyUndoFlag == MODE && !key_flag1.bit.JInertia)
						{
						  	asm("	SETC	INTM");
							int_flag3.bit.J_initia = 0;
							int_flag3.bit.J_End = 0;
							asm("	CLRC	INTM");
						  	key_flag1.bit.JInertia = 1;						  	
						}
						if (int_flag3.bit.J_End)
						{
							key_flag1.bit.JInertia = 0;
							Undisplay((Uint16)J_inertia,0);
							LedSegment[1] |= 0x0010;
							if(J_inertia == 0)
							{
								LedSegment[1] &= 0xFFEF;
							}
							else if(J_inertia<10)
							{
								LedSegment[1] |= LedCode[0];
							}
							
						}
						else if (state_flag2.bit.Son && key_flag1.bit.JInertia)
						{
							Undisplay(Un[0],UnAttribute[0]);
						}
						else
						{
							LedSegment[4] = 0x0020;
							LedSegment[3] = 0x0020;
							LedSegment[2] = 0x00C5;
							LedSegment[1] = 0x0020;
							LedSegment[0] = 0x0020;
						}
						break;
					//---------------------------------------------------
					// �����Ȧ���ݼ�����
					//---------------------------------------------------
					case ClrEncMuti:
						if(KeyUndoFlag == DATA)
						{
							MenuLayer = 0;
							key_flag1.bit.rstEncMuti = 0;
						}


						if(key_flag1.bit.rstEncMuti)
						{
							LedSegment[4] = 0x00E0; // c
							LedSegment[3] = 0x00C8; // L
							LedSegment[2] = 0x00A0;	// r
							LedSegment[1] = 0x0020;	// -
							LedSegment[0] = 0x0020;	// -							
						}
						else
						{
							LedSegment[4] = 0x00E0; // c
							LedSegment[3] = 0x0020; // -
							LedSegment[2] = 0x00AB;	// P
							LedSegment[1] = 0x00E4;	// o
							LedSegment[0] = 0x006E;	// S
							if(KeyUndoFlag == MODE && !state_flag2.bit.Son && memCurLoop00.hex.hex00 == Encoder_Abs17bit)
							{
								key_flag1.bit.rstEncMuti = 1;
								asm("	SETC	INTM");
								int_flagx.bit.rstEncMuti = 1;
								asm("	CLRC	INTM");
							}
						}
						break;
					//---------------------------------------------------
					// �������ֵ��������ش���
					//---------------------------------------------------
					case ClrEncErr:
						if(KeyUndoFlag == DATA)
						{
							MenuLayer = 0;
							key_flag1.bit.rstEncErr = 0;
						}


						if(key_flag1.bit.rstEncErr)
						{
							LedSegment[4] = 0x00E0; // c
							LedSegment[3] = 0x00C8; // L
							LedSegment[2] = 0x00A0;	// r
							LedSegment[1] = 0x0020;	// -
							LedSegment[0] = 0x0020;	// -							
						}
						else
						{
							LedSegment[4] = 0x00E0; // c
							LedSegment[3] = 0x0020; // -
							LedSegment[2] = 0x00EA;	// E
							LedSegment[1] = 0x00A0;	// r
							LedSegment[0] = 0x00A0;	// r
							if(KeyUndoFlag == MODE && !state_flag2.bit.Son && memCurLoop00.hex.hex00 == Encoder_Abs17bit)
							{
								key_flag1.bit.rstEncErr = 1;
								asm("	SETC	INTM");
								int_flagx.bit.rstEncErr = 1;
								asm("	CLRC	INTM");
							}
						}
						break;
					//------------------------------------------------
					// ��������Fn013��ADУ��ֵ
            		//------------------------------------------------
					case Adjustview:
                        if (KeyPressFlag==UP || (KeyHoldFlag==UP && KeyTimer>5 && TimeFlag1==1)) // ��UP�����Ӻ���
						{	
							if (Fn013Index<9)
							{
								Fn013Index++;
							}
						}
						if (KeyPressFlag==DOWN || (KeyHoldFlag==DOWN && KeyTimer>5 && TimeFlag1==1)) // ��DOWN����С����
						{	
							if (Fn013Index>0)
							{
								Fn013Index--;
							}
						}
						if (KeyUndoFlag==DATA) // ��DATA������һ����
						{	
							MenuLayer = 0;
						}	
						//----��ʾ����----
						HexToBcd(Pn[SpdAddr+Fn013Index]);
						LedSegment[4] = LedCode[Fn013Index];
						LedSegment[3] = LedCode[BcdCode[3]];
						LedSegment[2] = LedCode[BcdCode[2]];
						LedSegment[1] = LedCode[BcdCode[1]];
						LedSegment[0] = LedCode[BcdCode[0]];
						LedSegment[4] |= 0x0010; //.
						break;
					default:					
						break;
				}
				break;
			default:
				break;
		}
	}
//---------------------------------------------------------------------------------
// �����˵�(�༭ҳ��)
//---------------------------------------------------------------------------------
	else if (MenuLayer==2)
	{
		ModifyContent();
		if(MainMenuPage == 3)
		{
			if(FnNum==4)
			{
				if(Fn004Index)
				{
					TCRoffset = EditContent;
				}
				else
				{
					spdoffset = EditContent;	
				}
			}
			else if(FnNum==6)
			{
				if(Fn006Index)
				{
					Ivoffset = EditContent;
				}
				else
				{
					Iuoffset = EditContent;
				}
			}
		}
		HoldCmpLostFlag = 1;
		if ((KeyUndoFlag==MODE) || 
			(KeyHoldFlag==DATA && KeyTimer>10 && KeyIntoFlag==0) )// ��MODE�����沢���ض�����?
		{	
			KeyIntoFlag = 1;
			Pn[SumAddr] -= Pn[EditIndex];
			CheckSum -= Pn[EditIndex];
			Pn[EditIndex] = EditContent;
			Pn[SumAddr] += Pn[EditIndex];
			CheckSum += Pn[EditIndex];
			SPI_WriteFRAM(EditIndex,&Pn[EditIndex],1);
			SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
			if (KeyUndoFlag == MODE && MainMenuPage == 1)
				MenuLayer = 0;
			else
				MenuLayer = 1;
			EditFlag = 0;
			EditBit = 0;
		}
		if (TimeComp3==1 && MenuLayer==2) // �༭λ��˸
		{
			LedSegment[EditBit] |= 0x0010;
		}
	}
//---------------------------------------------------------------------------------
// ��ʱ����
//---------------------------------------------------------------------------------
	//------------------------------------------------
	// 1ms��ʱ
	//------------------------------------------------
	if ((Timer0%20)>=10) 
	{
		TimeComp0 = 1;
	}
	else 
	{
		TimeComp0 = 0;
	}
	//------------------------------------------------
	// 100ms��ʱ
	//------------------------------------------------
	if ((Timer0%2000)>=1000)
	{
		TimeComp1 = 1;
	}
	else 
	{
		TimeComp1 = 0;
	}
	// ��ס����ʱ����ɲ�����ʱ
	TimeFlag1 = 0;
	if (TimeBuff1!=TimeComp1) // �����ػ��½���
	{
		TimeFlag1 = 1;
		if (KeyHoldFlag!=0)
		{
			if (KeyTimer<1000)
			{
				KeyTimer++; // �а�ס��־�Ϳ�ʼ��ʱ����ʱ��಻����100��
			}
		}
		else 
		{
			KeyTimer = 0; // �ް�ס��־�͸�λ��ʱ
		}
		if (FinishFlag==1) // ��ɲ�������ʱ1����˸
		{
			if (DoneTimer<12)
			{
				DoneTimer++;
			}
			else if (KeyHoldFlag==0)
			{
				DoneTimer = 0;
				FinishFlag = 0;
				StartFlag = 0;
			} 
		}
	}
	TimeBuff1 = TimeComp1;
	//------------------------------------------------
	// 0.5s��ʱ
	//------------------------------------------------
	if ((Timer0%10000)>=5000)
	{
		TimeComp2 = 1;
	}
	else 
	{
		TimeComp2 = 0;
	}
	//------------------------------------------------
	// 0.2s��ʱ
	//------------------------------------------------
	if ((Timer0%4000)>=2000)
	{
		TimeComp3 = 1;
	}
	else 
	{
		TimeComp3 = 0;
	}
	// ����������ʱ
	if (TimeBuff0!=TimeComp0) // �����ػ��½���
	{	
		if(TestPara.Test_Mode)KeyIn = 0;
		//else KeyIn = (~(GpioDataRegs.GPADAT.all)>>12)&0x000F; // ÿ��1ms��ȡһ�μ�ֵ
		else
	    {
		    KeyIn = 0;			// ÿ��1ms��ȡһ�μ�ֵ
		    KeyIn |= GpioDataRegs.GPBDAT.bit.GPIO50;	//DATA (Key1)
		    KeyIn ^= 0x0001;
		    KeyIn <<= 1;
		    KeyIn |= GpioDataRegs.GPBDAT.bit.GPIO51;	//DOWN (Key2)
		    KeyIn ^= 0x0001;
		    KeyIn <<= 1;
		    KeyIn |= GpioDataRegs.GPBDAT.bit.GPIO59;	//UP (Key3)
		    KeyIn ^= 0x0001;
		    KeyIn <<= 1;
		    KeyIn |= GpioDataRegs.GPBDAT.bit.GPIO60;	//MODE (Key4)
		    KeyIn ^= 0x0001;
		    KeyIn &= 0x000F;
        }
		if (KeyInBuff==KeyIn)
		{
			if (KeyDelay<20)
			{
				KeyDelay++; 
			}
			else 
			{
				KeyAssignments = KeyIn;
			}
		} 	 
		else 
		{
			KeyDelay = 0;
			KeyPressFlag = 0; 		// �����а�����־
			KeyHoldFlag = 0; 		// �����а�ס��־
			KeyIntoFlag = 0;
			KeyAssignments = 0;		//
		}
		KeyInBuff = KeyIn;
	}
	TimeBuff0 = TimeComp0;

	// ������־����
	if (KeyAssignments!=KeyBuff && KeyAssignments!=0)  
	{
		KeyPressFlag = KeyAssignments;	// �ð�����־
		KeyHoldFlag = KeyAssignments;	// �ð�ס��־
	}	
	else if (KeyAssignments!=KeyBuff && KeyIn==0)		
	{
		KeyUndoFlag = KeyBuff;	// ���ɿ���־
	}
	else  
	{
		if (KeyUndoFlag != 0)
		{
			HoldCmpLostFlag = 0;
			KeepEbitFlag = 0;
		}
		KeyPressFlag = 0; 		// �����а�����־
		KeyUndoFlag = 0;		// �������ɿ���־
	}
	KeyBuff = KeyAssignments;
}
//==== End void MENU(void) ====


//=================================================================================
// ʮ������תʮ����
//=================================================================================
void HexToBcd(Uint16 data)
{
	Uint16 i,divisor; 
	divisor = data;
	for (i=0;i<5;i++)
	{
		BcdCode[i] = divisor%10;
		divisor = divisor/10;
	}	
}

//=================================================================================
// ʮ������ת������
//=================================================================================
void HexToBit(Uint16 data)
{
	Uint16 i,divisor; 
	divisor = data;
	for (i=0;i<4;i++)
	{
		BcdCode[i] = divisor%2; 
		divisor = divisor/2;
	}	
}
void setWayExu(void)						// �Ӽ�����ʱ�ٶȼӿ촦��
{
	if ( MenuLayer==1 )
	{
	  	if (KeyTimer > 155)
	    {
		   WayExu = 3;  
		}
		else 
		{
			if(KeyTimer > 105)
			{
				WayExu = 2;
			}
			else
			{
				if (KeyTimer > 55) 
				{
					WayExu = 1;
				}
				else
				{
					WayExu = 0;
				}
			}
		}
	}
	else if (MenuLayer == 2)
	{
	  WayExu = EditBit;
	}
}
void ModifyContent(void)					// �����޸�����ʾ
{
	int16 bitNum;
	int16 IncData[5];
	HEX_DISP hexEdit,hexEditMax,hexEditMin;
	if (PnAttribute[EditIndex][0]==0)		// ���ݲ������Ծ�����λ����
	{
		bitNum = 4;
	}
	else
	{
		bitNum = 3;
	}
	if (PnAttribute[EditIndex][0]==2)		// ���ݲ������Ծ�����ʮ���ƻ��Ƕ���������
	{
		IncData[0] = 1;
		IncData[1] = 2;
		IncData[2] = 4;
		IncData[3] = 8;
	}
	else if(PnAttribute[EditIndex][0]==3)
	{
		IncData[0] = 1;
		IncData[1] = 16;
		IncData[2] = 256;
		IncData[3] = 4096;		
	}
	else
	{
		IncData[0] = 1;
		IncData[1] = 10;
		IncData[2] = 100;
		IncData[3] = 1000;
		IncData[4] = 10000;
	}
	if (KeyUndoFlag==DATA  && MenuLayer==2 && KeepEbitFlag==0) // ��DATA����λKeepEbitFlag��ֹ�������������ʱ�ɿ���Ч
	{	
		if (EditBit<bitNum)
		{
			EditBit++;
		}
		else 
		{
			EditBit = 0;
		}
	}	
	if (KeyUndoFlag==UP || (KeyHoldFlag==UP && KeyTimer>5 && TimeFlag1==1))
	{	
		setWayExu();
		if(PnAttribute[EditIndex][0]!=3)
		{
			switch(WayExu)
			{
				case 0:
					EditContent += IncData[0];
					break;
				case 1:
					EditContent += IncData[1];
					break;
				case 2:
					EditContent += IncData[2];
					break;
				case 3:
					EditContent += IncData[3];
					break;
				case 4:
					EditContent += IncData[4];
					break;
				default:
					break;
			}
		}
		else if(MenuLayer!=1)
		{
			hexEdit.all = EditContent;
			hexEditMax.all = PnAttribute[EditIndex][2];			
			switch(WayExu)
			{
				case 0:
					if(hexEdit.hex.hex00 < hexEditMax.hex.hex00)
					{
						hexEdit.hex.hex00 += 1;
					}
					break;
				case 1:
					if(hexEdit.hex.hex01 < hexEditMax.hex.hex01)
					{
						hexEdit.hex.hex01 += 1;
					}
					break;
				case 2:
					if(hexEdit.hex.hex02 < hexEditMax.hex.hex02)
					{
						hexEdit.hex.hex02 += 1;
					}
					break;
				case 3:
					if(hexEdit.hex.hex03 < hexEditMax.hex.hex03)
					{
						hexEdit.hex.hex03 += 1;
					}
					break;
				default:
					break;
			}
			EditContent = hexEdit.all;			
		}
		if(EditIndex >= P_TCR_inLmtADD && EditIndex <= nBRKStopTCRADD)
		{
			if(EditContent >= abl_load)
			{
				EditContent = abl_load;							// ����������ģ���С����
			}
		}
		else
		{
			if (PnAttribute[EditIndex][0]==0)
			{
				if (EditContent>PnAttribute[EditIndex][2]) // ���ֵ����
				{
					EditContent = PnAttribute[EditIndex][2];
				}
			}
			else if(PnAttribute[EditIndex][0]!=3)
			{
				if (EditContent>(int16)PnAttribute[EditIndex][2]) // ���ֵ����
				{
					EditContent = PnAttribute[EditIndex][2];
				}				
			}
		}
	}
	if (KeyPressFlag==DOWN || (KeyHoldFlag==DOWN && KeyTimer>5 && TimeFlag1==1))
	{	
		setWayExu();
		if(PnAttribute[EditIndex][0]!=3)
		{
			switch(WayExu)
			{
				case 0:
					EditContent -= IncData[0];
					break;
				case 1:
					EditContent -= IncData[1];
					break;
				case 2:
					EditContent -= IncData[2];
					break;
				case 3:
					EditContent -= IncData[3];
					break;
				case 4:
					EditContent -= IncData[4];
					break;
				default:
					break;
			}
		}
		else if(MenuLayer!=1)
		{
			hexEdit.all = EditContent;
			hexEditMin.all = PnAttribute[EditIndex][1];
			switch(WayExu)
			{
				case 0:
					if(hexEdit.hex.hex00>hexEditMin.hex.hex00)
					{
						hexEdit.hex.hex00 -= 1;
					}
					break;
				case 1:
					if(hexEdit.hex.hex01>hexEditMin.hex.hex01)
					{
						hexEdit.hex.hex01 -= 1;
					}
					break;
				case 2:
					if(hexEdit.hex.hex02>hexEditMin.hex.hex02)
					{
						hexEdit.hex.hex02 -= 1;
					}
					break;
				case 3:
					if(hexEdit.hex.hex03>hexEditMin.hex.hex03)
					{
						hexEdit.hex.hex03 -= 1;
					}
					break;
				default:
					break;
			}
			EditContent = hexEdit.all;			
		}
		if (PnAttribute[EditIndex][0]==0)
		{
			if (EditContent<PnAttribute[EditIndex][1]) // ��Сֵ����
			{
				EditContent = PnAttribute[EditIndex][1];
			}
		}
		else if(PnAttribute[EditIndex][0]!=3)
		{
			if ((int16)EditContent<(int16)PnAttribute[EditIndex][1]) // ��Сֵ����
			{
				EditContent = (int16)PnAttribute[EditIndex][1];
			}			
		}

	}

	//----��ʾ����----
	switch(PnAttribute[EditIndex][0])
	{
		case 0:
			HexToBcd(EditContent);
			LedSegment[4] = LedCode[BcdCode[4]];
			LedSegment[3] = LedCode[BcdCode[3]];
			LedSegment[2] = LedCode[BcdCode[2]];
			LedSegment[1] = LedCode[BcdCode[1]];
			LedSegment[0] = LedCode[BcdCode[0]];
			break;
		case 1:
			if( EditContent<0)
			{
				LedSegment[4] = 0x0020; // -
				HexToBcd(abs(EditContent)); // ������ȡ����1
			}
			else
			{
				LedSegment[4] = 0x0000; //
				HexToBcd(EditContent);
			}			
			LedSegment[3] = LedCode[BcdCode[3]];
			LedSegment[2] = LedCode[BcdCode[2]];
			LedSegment[1] = LedCode[BcdCode[1]];
			LedSegment[0] = LedCode[BcdCode[0]];
			break;
		case 2:
			HexToBit(EditContent);
			LedSegment[4] = LedCode[0xb];
			LedSegment[3] = LedCode[BcdCode[3]];
			LedSegment[2] = LedCode[BcdCode[2]];
			LedSegment[1] = LedCode[BcdCode[1]];
			LedSegment[0] = LedCode[BcdCode[0]];
			break;
		case 3:
			LedSegment[4] = 0xAD;
			LedSegment[3] = LedCode[(EditContent & 0xF000)>>12];
			LedSegment[2] = LedCode[(EditContent & 0x0F00)>>8];
			LedSegment[1] = LedCode[(EditContent & 0x00F0)>>4];
			LedSegment[0] = LedCode[EditContent & 0x000F];
			break;
		default:
			break;			
	}
}
//_________________________________________________________
//_________________________________________________________	
void InOutDisplay(Uint16 m)
{
	Uint16 i;
	for (i = 1;i <= 4; i++)
	{
		LedSegment[i] = 0;
	}
	if (m & BIT0)
	{
		LedSegment[1] |= 0x05;
	}

	if (m & BIT1)
	{
		LedSegment[1] |= 0x88;
	}

	if (m & BIT2)
	{
		LedSegment[2] |= 0x05;
	}

	if (m & BIT3)
	{
		LedSegment[2] |= 0x88;
	}

	if (m & BIT4)
	{
		LedSegment[3] |= 0x05;
	}

	if (m & BIT5)
	{
		LedSegment[3] |= 0x88;
	}

	if (m & BIT6)
	{
		LedSegment[4] |= 0x05;
	}

	if (m & BIT7)
	{
		LedSegment[4] |= 0x88;
	}
	LedSegment[0] = 0xEC;
}
void DecDisplay(Uint16 m,Uint16 n)// mҪ��ʾ������n���� 0 ������Ҫ��ӷ��š�-��1������Ҫ��ӷ�š?��?
{
	int16 i,j,x;
	if(n)
	{
		x = abs((int16)m);
	}
	else
	{
		x = m;
	}
	HexToBcd(x);
	i = 4;
	while( i > 0 && BcdCode[i] == 0)
	{
		i--;
	}

	for (j = 0; j <= i; j++)
	{		
		LedSegment[j] = LedCode[BcdCode[j]];
	}
	j = i;
	while ( i<4 )
	{
		LedSegment[i+1] = 0;
		i++;
	}
	if(n && (int16)m < 0)
	{
		LedSegment[j+1] = 0x20;
	}	

}
void HexDisplay(Uint16 m)
{
	int16 i;
	for ( i = 1; i <= 4; i++)
	{
		LedSegment[i] = LedCode[m & 0x000f];
		m = m >> 4;
	}
	LedSegment[0] = 0xAD;
}
void Undisplay(Uint16 m,Uint16 n)// m ��ʾ������n ��ʾ��ʽ
{
	switch(n)
	{
	case 0:
		DecDisplay(m,0);
		break;
	case 1:
		DecDisplay(m,1);
		break;
	case 2:
		InOutDisplay(m);
		break;
	case 3:
		HexDisplay(m);
		break;
	case 4:
		if(labs(UnEk) >= 10000)
		{
			HexDisplay(m);
		}
		else 
		{
			DecDisplay(m,1);
		}
		break;
	default:
		break;
	}
}
//_________________________________________________________
void LoadDefaultPar(void) // �ָ�����
{Uint16 i,temp1,temp2;
HEX_DISP hPnxxx,hPnMax,hPnMin,hPnLoad;

	hPnxxx.all = Pn[hexControl01];
	hPnMax.all = PnAttribute[hexControl01][2];
	hPnMin.all = PnAttribute[hexControl01][1];
	if(hPnxxx.hex.hex03 > hPnMax.hex.hex03)
	{
		temp1 = 1;
	}
	else if(hPnxxx.hex.hex03 < hPnMin.hex.hex03)
	{
		temp1 = 1;
	}
	else
	{
		temp1 = hPnxxx.hex.hex03;
	}
	hPnxxx.all = Pn[MotorMurselADD];
	hPnMax.all = PnAttribute[MotorMurselADD][2];
	hPnMin.all = PnAttribute[MotorMurselADD][1];
	hPnLoad.all = PnAttribute[MotorMurselADD][3];	
	if(hPnxxx.hex.hex02 > hPnMax.hex.hex02)
	{
		temp2 = DEFAULT_DRV_TYPE;
	}
	else if(hPnxxx.hex.hex02 < hPnMin.hex.hex02)
	{
		temp2 = DEFAULT_DRV_TYPE;
	}
	else
	{
		temp2 = hPnxxx.hex.hex02;
	}

	if(ResumeIndex < Fn000Addr)
	{
		if(ResumeIndex == P_TCR_inLmtADD || ResumeIndex == N_TCR_inLmtADD || ResumeIndex == nBRKStopTCRADD)
		{
			factory_Iqn = MotorPar4[temp2][temp1][4];
			factory_Iqn *= 5792;    // 1.414*4096=5792

			#if DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW
			factory_Iqn >>= 10;		// 2.5mA
			#elif DRV_TYPE == DRV_5KW
			factory_Iqn >>= 11;		// 5mA
			#elif DRV_TYPE == DRV_15KW
			factory_Iqn >>= 12;		// 10mA
			#endif

			cacuload(factory_Iqn,MotorPar4[temp2][temp1][1]);
			Pn[ResumeIndex] = cacuload_temp;
		}
		else if(ResumeIndex >= UqFFCoFF && ResumeIndex <= ExcelNum)
		{
			Pn[ResumeIndex] = PnAttribute[ResumeIndex][3];//MotorPar1[temp2][temp1][ResumeIndex - UqFFCoFF];
		}
		else if(ResumeIndex >= x_base && ResumeIndex <= x_base+15 )
		{
			Pn[ResumeIndex] = PnAttribute[ResumeIndex][3];//MotorPar2[temp2][temp1][ResumeIndex - x_base];
		}
		else if(ResumeIndex >= y_base && ResumeIndex <= y_base+15)
		{
			Pn[ResumeIndex] = PnAttribute[ResumeIndex][3];//MotorPar3[temp2][temp1][ResumeIndex - y_base];
		}
		else if(ResumeIndex==hexControl01)
		{
			Pn[ResumeIndex] &= 0xF000;
			Pn[ResumeIndex] |= (PnAttribute[hexControl01][3] & 0x0FFF);
			if((Pn[ResumeIndex] & 0xF000) > (PnAttribute[hexControl01][2] & 0xF000))
			{
			    Pn[ResumeIndex] &= 0x0FFF;
				Pn[ResumeIndex] |= 0x1000;
			}
		}
		else if(ResumeIndex==MotorMurselADD)
		{
			if(hPnxxx.hex.hex00 > hPnMax.hex.hex00 || hPnxxx.hex.hex00 < hPnMin.hex.hex00)
			{
				hPnxxx.hex.hex00 = hPnLoad.hex.hex00;
			}
			if(hPnxxx.hex.hex01 > hPnMax.hex.hex01 || hPnxxx.hex.hex01 < hPnMin.hex.hex01)
			{
				hPnxxx.hex.hex01 = hPnLoad.hex.hex01;
			}
			if(hPnxxx.hex.hex02 > hPnMax.hex.hex02 || hPnxxx.hex.hex02 < hPnMin.hex.hex02)
			{
				hPnxxx.hex.hex02 = DEFAULT_DRV_TYPE;
			}
			if(hPnxxx.hex.hex03 > hPnMax.hex.hex03 || hPnxxx.hex.hex03 < hPnMin.hex.hex03)
			{
				hPnxxx.hex.hex03 = hPnLoad.hex.hex03;
			}
			Pn[ResumeIndex] = hPnxxx.all;
		}
		else if(ResumeIndex >= HzConversADD && ResumeIndex <= AsynRunFiADD)
		{
			Pn[ResumeIndex] = PnAttribute[ResumeIndex][3];//MotorPar4[temp2][temp1][ResumeIndex - HzConversADD];
		}
		else if(ResumeIndex == curSamUnit)
		{
			Pn[ResumeIndex] = PnAttribute[321][3];//DrivePar1[temp2];
		}
		else if(ResumeIndex == curKpADD || ResumeIndex == curKiADD || ResumeIndex == MotorLdqADD
				|| ResumeIndex == InitPhaseADD || ResumeIndex == RatedSpeedADD || ResumeIndex == ImagneticADD)
		{
			switch(ResumeIndex)
			{
				case curKpADD:
					Pn[ResumeIndex] = PnAttribute[322][3];//MotorPar4[temp2][temp1][10];
					break;
				case curKiADD:
					Pn[ResumeIndex] = PnAttribute[323][3];//MotorPar4[temp2][temp1][11];
					break;
				case MotorLdqADD:
					Pn[ResumeIndex] = PnAttribute[311][3];//MotorPar4[temp2][temp1][13];
					break;
				case InitPhaseADD:
					Pn[ResumeIndex] = PnAttribute[325][3];//MotorPar4[temp2][temp1][8];
					break;
				case RatedSpeedADD:
					Pn[ResumeIndex] = PnAttribute[328][3];//MotorPar4[temp2][temp1][9];
					break;
				default:
					Pn[ResumeIndex] = MotorPar4[temp2][temp1][12];
					break;
			}
		}
	    #if AC_VOLT == AC_400V && DRV_TYPE == DRV_15KW
		else if (ResumeIndex == Supply5vMeasure)
		{
	        if (Pn[ResumeIndex] < PnAttribute[ResumeIndex][1] || Pn[ResumeIndex] > PnAttribute[ResumeIndex][2])
			{
	        	Pn[ResumeIndex] = PnAttribute[Supply5vMeasure][3];
			}
		}
		#endif
		else
		{
			Pn[ResumeIndex] = PnAttribute[ResumeIndex][3];
		}
		SPI_WriteFRAM(ResumeIndex,&Pn[ResumeIndex],1);
		ResumeIndex++;
	}
	else 
	{
		Pn[SumAddr] = 0;
		CheckSum = 0;
		for (i=0;i<SumAddr;i++)
		{
			Pn[SumAddr] += Pn[i]; 
			CheckSum += Pn[i];
		}
		SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
		ResumeIndex = 0;
		if((MainMenuPage == 3) && (FnNum == LoadDefault))
		{
			FinishFlag = 1;
			key_flag1.bit.loadDefault = 0;
		}
		else if(sci_oper_flag.bit.SciLoadDefault)
		{
			sci_oper_flag.bit.SciLoadDefault = 0;
		}			
	}
}
//------------------------------------------------------------		

//--end----------------------------

