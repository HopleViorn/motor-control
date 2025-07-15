// 子程序单元
#include "VerInfo.h"
#include "EDBclm.h"			// define and function declare
#include "globalvar.h"		//
#include "SPI4.h"

extern int16 CommandSpeed;
extern Uint16 xyz;
extern int16 PositionSpeed;
extern int16 Masterspeed;
int32  MyPf[2];
int32 WatchDelta;
 int32 DeltaPuls;
Uint16 pulscount;
int32 Sum;
int32 speed100[10];

#define   AvrSpeedCount 20

extern int16  standSum;

char i;
        int32  W5=0;
                  int32     Wspi=0;

        int32 Avr5spd=0;
        unsigned char spdIndex=0;



extern unsigned char SendEnconder;
extern volatile int32 encoder;
int32 EncodrPidOut;
 int16 TogetherEncodr;
extern Uint16 Slaveencoder;
int16 PositionCurrentError=0;



int32 syncOut;
Uint16 CountSpeedLoop;  //5次速度环（10K)做一次同步
int32 SyncSpeed=0;
int16 speed3=0;
int32 XiGeMaspeed[5];
unsigned char  speedIndex;


int32 e_speed;


extern Uint16 Pos485;

extern const int sintab[];
extern const int32 costab[];
extern const Uint16 RigidityGain[16];
extern const Uint16 RigidityTi[16];
#pragma CODE_SECTION(pulsEncoder, "ramfuncs");
//#pragma CODE_SECTION(SModule4, "ramfuncs");
#pragma CODE_SECTION(SModule3, "ramfuncs");
#pragma CODE_SECTION(SModule2, "ramfuncs1");
#pragma CODE_SECTION(SModule1, "ramfuncs2");
#pragma CODE_SECTION(limitIqr, "ramfuncs");
//#pragma CODE_SECTION(JudgeVCMP, "ramfuncs");
#pragma CODE_SECTION(runposition, "ramfuncs");
#pragma CODE_SECTION(notclrEk, "ramfuncs");
#pragma CODE_SECTION(PControl, "ramfuncs2");
#pragma CODE_SECTION(pulsefilter, "ramfuncs");
//#pragma CODE_SECTION(NextAnaSC, "ramfuncs");
//#pragma CODE_SECTION(LmtSpd, "ramfuncs");
//#pragma CODE_SECTION(AnalogTcrCol, "ramfuncs");
//#pragma CODE_SECTION(NoSetZero, "ramfuncs1");-----------2010
//#pragma CODE_SECTION(ParSpd, "ramfuncs1");
//#pragma CODE_SECTION(seek_step, "ramfuncs1");-----------2010
#pragma CODE_SECTION(SoftTime2, "ramfuncs1");
#pragma CODE_SECTION(Jdynamic, "ramfuncs1");
#pragma CODE_SECTION(cacuGain, "ramfuncs1");
//#pragma CODE_SECTION(Scacu, "ramfuncs1");
#pragma CODE_SECTION(Notch, "ramfuncs1");
//#pragma CODE_SECTION(dowith_Notch, "ramfuncs2");
//#pragma CODE_SECTION(findRef, "ramfuncs");

// 压力PI调节器
int32 PressControl(int32 PressFeed,int32 PressBack,int32 QSpeed,int32 RspeedLmt) // RspeedLmt 最大反向转速
{
Uint16 Kpr,xKi;
int32 ePress,Result1,Result2,Result;
	if(input_state.bit.POT && input_state.bit.NOT)
	{
		Kpr = Pn[PressGain1];
		xKi = Pn[PressTime1];
	}
	else if(input_state.bit.POT && (input_state.bit.NOT == 0))
	{
		Kpr = Pn[PressGain2];
		xKi = Pn[PressTime2];
	}
	else if((input_state.bit.POT == 0) && input_state.bit.NOT)
	{
		Kpr = Pn[PressGain3];
		xKi = Pn[PressTime3];
	}
	else
	{
		Kpr = Pn[PressGain4];
		xKi = Pn[PressTime4];
	}
	
	ePress = PressFeed - PressBack;
	Result1 = Kpr * ePress >> 8;
	Result2 = ((long long)Kpr * xPress) / xKi >> 8; 
	Result = Result1 + Result2;

	if ( ePress <= (int32)Pn[PressP_PIADD] * 10 )		//压力环积分分离处理 2012-02-17
    {
		if ( Result < QSpeed && Result > RspeedLmt )
	    {
		     xPress += ePress;
	    }
      	Result += (Kpr * ePress) / xKi >> 8; 
    }

    if(Result > QSpeed)
    {
        Result = QSpeed;
    }
	else if(Result < RspeedLmt)
	{
		Result = RspeedLmt;
	}

	return(Result);
}

// 根据压力梯度计算压力的增加或减小量
void GradsCacu(Uint32 GradsIn,Uint16 GradsTime)
{
	if(GradsTime)
	{
		divGrads = 10000 / GradsTime;
		rmGrads += 10000 % GradsTime;
		if(rmGrads >= GradsTime)
		{
			divGrads++;
			rmGrads -= GradsTime;
		}
		if(GradsIn > GradsOut)
		{
			GradsOut += divGrads;
			if(GradsOut > GradsIn)
			{
				GradsOut = GradsIn;
			}
		}
		else
		{
			GradsOut -= divGrads;
			if ((int32)GradsOut < 0)	//GradsOut为负时，后续比较出错
			{
				GradsOut = 0;
			}
			if(GradsOut < GradsIn)
			{
				GradsOut = GradsIn;
			}
		}		
	}
	else
	{
		GradsOut = GradsIn;
	}
}
void ServoPump(void)
{
Uint32 PressFeed;
	if(AnPressFeed_out<0)
	{
		PressFeed = 0;
	}
	else
	{
		PressFeed = AnPressFeed_out;
	}
	GradsCacu(PressFeed,Pn[PressGrads]);
	SPEEDINS = VCMDG;				// 若以后添加参数设定流量或MODBUS设定要考虑CCW,CW
	LmtSpd();
	CMDref = VCMD;
	if(pos_flag4.bit.PNOTzcl)
	{
		NextAnaSC();
	}
	else if(state_flag2.bit.Son)
	{
		SModule4();
		SPEED = PressControl(GradsOut,AnPressBack_out,SPEEDINSout,-(int32)Pn[PressRspeed]*10);
		SModule3();		        // 计算出对应的限幅数值
		SModule2();        		// 速度PI调节
		limitIqr();         	// 具体的限幅数值
		SModule1();         	// 转矩滤波
	}
	else
	{
		x_speed = 0;
		SPEEDINS = 0;
		SPEEDINSout = 0;
		SPEED = 0;
		xPress = 0;
		rmGrads = 0;
		GradsOut = 0;
	}
}
void findRef(Uint32 EncoderData)
{
	if(int_flag3.bit.FindRef_ot)
	{// Ref_Pot
		if(int_flag3.bit.pFindRef)
		{
			if(input_state.bit.POT == 0)
			{
				int_flag3.bit.pFindRef = 0;		// 清除找参考点的标志
				int_flag3.bit.FindRef_ot = 0;	// 清除遇到ot标志
				Timers.STEPTME = 0;
				Timers.Fndcnt = 0;
				Ek = 0;
				RmFg = 0;
				if(membit01.bit.bit00)	
				{										
					Pfroate = (int32)(EncoderData - singlePos)/10000;
					Pfcount = (int32)(EncoderData - singlePos)%10000;
				}
				else
				{				
					Pfroate = (int32)(singlePos)/10000;
					Pfcount = (int32)(singlePos)%10000;
				}
				//Pf[1] = QPCNT;						
			}
		}
		else if(int_flag3.bit.nFindRef)
		{// Otherdir
			if(input_state.bit.NOT == 0)
			{
				int_flag3.bit.nFindRef = 0;		// 清除找参考点的标志
				int_flag3.bit.FindRef_ot = 0;	// 清除遇到ot标志
				Timers.STEPTME = 0;
				Timers.Fndcnt = 0;
				Ek = 0;
				RmFg = 0;
				if(membit01.bit.bit00)	
				{										
					Pfroate = -singlePos/10000;
					Pfcount = -singlePos%10000;
				}
				else
				{				
					Pfroate = -(int32)(EncoderData - singlePos)/10000;
					Pfcount = -(int32)(EncoderData - singlePos)%10000;
				}
				//Pf[1] = QPCNT;						
			}
		}
	}
}

void pulsEncoder(void)
{
Uint16 MptmpA,MptmpC,Mctmp;
long long TMP;
		do
		{
			MptmpA = *QPCNT1;
			Mctmp = *QCPRD;
			MptmpC = *QPCNT1;
		}
		while (MptmpA != MptmpC);


		MpInA = MptmpA;
		MpInC = MptmpC;
		McOEF = MpInC & 0x0003;	
		MpBuffA = (int16)((MpInA & 0xFFFC) - (MpInB & 0xFFFC));
		MpBuffA >>= 2;
		McInA = Mctmp;
		McBuff =  (int32)(McInA - McInB);
		if ((McOEF & 0x0001) == 0x0001)
		{
			out_state.bit.CLEAR = 1;
			*REG_SPD = out_state.all; // 清除MACHXO中的QCTMR实时溢出位曛荆CLEAR=1）
			if (McHigh < 100)			// 2008-2-25随着cpld时钟而改变
			{
				McHigh++;
			} 
			else
			{
				speed = 0; 
			}
			out_state.bit.CLEAR = 0;
			*REG_SPD = out_state.all; // 指矗–LEAR=0）
		}
		McBuff += (((int32)McHigh)<<16);
		if (MpBuffA!=0) // Mp不为零时计算速度，否则保持当前速度
		{
			if ((McOEF & 0x0003) == 0x0001)
			{
				McBuff -= 0x00010000; // 锁存时无进位，所以减去前面加的进位
				McHigh = 1; // SETB	Posflag2,bit0  pos_flag4.bit.GearErr = 1;
			}	
			else 
			{
				McHigh = 0;
			}
			if (((int32)MpBuffA*(int32)MpBuffB)<0)
			{
				speed = 0;
			}
			else
			{
				MPg[0] = MpBuffA;
				if(((int32)MPg[0] * (int32)MPg[1]) < 0)
				{			
					speed = 0;
				}
				else
				{
					Mc = McBuff;
					Mp = ((int32)MpBuffA)<<5;					
					TMP = Mp*SpeedUnit;
					#if CPU_FRQ == CPU_100MHz
					speed = TMP*18750/Mc;// CPLD 100MHz
					speed = speed*2500/memEncNum;
					#else
					speed = TMP*28125/Mc;// CPLD 75MHz
					speed = speed*1250/memEncNum;
					#endif
				}
				MPg[1] = MPg[0];
			}
		}

		////////////////////////////speed100avr计算/////////////////////////////
#if 0

		                if(spdIndex<(AvrSpeedCount-1))
		                {
		                  spdIndex++;
		                }
		                else spdIndex=0;
		                 speed100[spdIndex]=speed;
		                 Sum=0;
		                 for(i=0;i<AvrSpeedCount;i++)
		                 {
		                     Sum=Sum+speed100[i];
		                 }
		                Avr5spd=(Sum/AvrSpeedCount)%65536;     //发送放encoder处
		             //   Avr5spd=speed%65536;

		                // Avr100spd=speed%65536;
		                 McbspbRegs.DXR1.all=Avr5spd;

#endif


		                 //////////////////////////////////////////////////////////////////////////////




MpInB = MpInA;
		McInB = McInA;
		MpBuffB = MpBuffA;

		Pf[0] = QPCNT; //读了去除后2位   //PC位置时脉冲反馈来源
		Pferr = Pf[0] - Pf[1];     //时间间隔，一个PWM周期

		if(Pferr==0 && !Pn[measureSPDADD])
		{
			speed = 0;
		}
		Pferr >>= 2;

		ZeroPf = Pferr;
		Pf[1] = Pf[0];

		PgDelt = (long)Pferr * memPGDIV / memEncNum;
		PGrm +=  (long)Pferr * memPGDIV % memEncNum;
		if(labs(PGrm)>=memEncNum)
		{
			if(PGrm>0)
			{
				PGrm -= memEncNum;
				PgDelt++;
			}
			else
			{
				PGrm += memEncNum;
				PgDelt--;
			}
		}
}

//===================================================================
//				修改后的单位圆陷波滤波器计算公式
//===================================================================
//
//			Y(Z)	  (Z^2 - 2*cos(alpha)*Z + 1) * (1-u)
//	H(z) = ------ = --------------------------------------
//			X(Z)	 Z^2 - 2*(1-u)*cos(alpha)*Z + (1-2*u)
//
//	Where:	alpha = 2*pi*f0/fs(rad)
//		   	fs:采样频率，即速度环的中断周期
//			余弦表格为1800点对应[0,180],16位精度
//			Teta: rad->angular, Teta = (f0/fs)*3600
//			e.g. fs = 10,000Hz, Teta = (f0*3600*4096/10,000)>>12
//									 = (f0*1475)>>12
//			B1 = B3 = (1-u), B2 = 2*cos(alpha);
//			A1 = 1, A2= 2*(1-u)*cos(alpha), A3 = (1-2*u);
//			u:(0,1), 程序中u为4.12格式，1个单位为20/4096 = 0.0049
//===================================================================

//===================================================================
//				原先单位圆陷波滤波器计算公式（备注）
//===================================================================
//
//			Y(Z)		  Z^2 - 2*cos(alpha)*Z + 1
//	H(z) = ------ = --------------------------------------
//			X(Z)	 Z^2 - 2*(1-u)*cos(alpha)*Z + (1-u)^2
//
//===================================================================
int32 Notch(Uint16 f0,Uint16 u,int32 inNotch)
{
Uint16 Teta;
int16 A2,A3;
long long tmp,tmp3;
	xIqr[0] = inNotch;
	u = (u+1)*20;
	Teta = (Uint32)1475 * f0 >> 12;
	A2 = ((int32)costab[Teta]*(4096-u))>>15;
	A3 = 4096-(u<<1);

	tmp3 = (long long)(xIqr[0] + xIqr[2])*(4096-u);	//A1 = A3 = (4096-u)>>12
	tmp3 -= (long long)xIqr[1]*A2;
	tmp3 += (long long)yIqr[1]*A2;
	tmp3 -= (long long)yIqr[2]*A3;

	tmp = (long long)yIqrm[1]*A2 - (long long)yIqrm[2]*A3;
	if(tmp>=0)
	{
		tmp3 += tmp>>12;
	}
	else
	{
		tmp3 -= (-tmp)>>12;
	}

	if(tmp3>=0)
	{
		yIqr[0] = tmp3>>12;
	}
	else
	{
		yIqr[0] = -((-tmp3)>>12);
	}
	yIqrm[0] = tmp3-(yIqr[0]<<12);

	xIqr[2] = xIqr[1];
	xIqr[1] = xIqr[0];
	yIqr[2] = yIqr[1];
	yIqr[1] = yIqr[0];
	yIqrm[2] = yIqrm[1];
	yIqrm[1] = yIqrm[0];
	return(yIqr[0]);
}

void dowith_Notch(void)
{
int32 Iqr_Notch;
	if(Pn[NotchF1] < 4999)
	{
		xIqr[1] = x1Iqr[1];
		xIqr[2] = x1Iqr[2];
		yIqr[1] = y1Iqr[1];
		yIqr[2] = y1Iqr[2];
		yIqrm[1] = y1Iqrm[0];
		yIqrm[2] = y1Iqrm[1];
		Iqr_Notch = Notch(Pn[NotchF1],Pn[NotchDep1],Iqrbuf);
		x1Iqr[1] = xIqr[1];
		x1Iqr[2] = xIqr[2];
		y1Iqr[1] = yIqr[1];
		y1Iqr[2] = yIqr[2];
		y1Iqrm[0] = yIqrm[1];
		y1Iqrm[1] = yIqrm[2];
	}
	else
	{
		Iqr_Notch = Iqrbuf;
	}
	if(Pn[NotchF2] < 4999)
	{
		xIqr[1] = x2Iqr[1];
		xIqr[2] = x2Iqr[2];
		yIqr[1] = y2Iqr[1];
		yIqr[2] = y2Iqr[2];
		yIqrm[1] = y2Iqrm[0];
		yIqrm[2] = y2Iqrm[1];
		Iqr_Notch = Notch(Pn[NotchF2],Pn[NotchDep2],Iqr_Notch);
		x2Iqr[1] = xIqr[1];
		x2Iqr[2] = xIqr[2];
		y2Iqr[1] = yIqr[1];
		y2Iqr[2] = yIqr[2];
		y2Iqrm[0] = yIqrm[1];
		y2Iqrm[1] = yIqrm[2];
	}
	Iqrbuf = Iqr_Notch;
} 

void cacuGain(int32 Tmp)
{
int16 temp1,temp2;
	if(memGainSW == 1)
	{
		if(input_state.bit.G_SEL && int_flag3.bit.HowGain)
		{
			int_flag3.bit.SWgain = 1;
		}
		else if(!input_state.bit.G_SEL && !int_flag3.bit.HowGain)
		{
			int_flag3.bit.SWgain = 1;
		}
	}
	else
	{
		if(Tmp >= Pn[SelGainLVADD] + Pn[SelGainZhihuanADD] && !int_flag3.bit.HowGain)
		{
			int_flag3.bit.StartSW = 1;				// 启动切换延时
		}
		else if(Tmp <= Pn[SelGainLVADD] - Pn[SelGainZhihuanADD] && int_flag3.bit.HowGain)
		{
			int_flag3.bit.StartSW = 1;				// 启动切换延时
		}
		else
		{
			int_flag3.bit.StartSW = 0;				// 停止切换延时
		}
		if(int_flag3.bit.StartSW)
		{
			if(SWdelay >= Pn[SelGainDelayADD])
			{
				int_flag3.bit.SWgain = 1;			// 转换开始
			}
			else
			{
				SWdelay++;							// 单位0.1ms
			}
		}
		else
		{
			SWdelay = 0;							// 延时清零
		}
	}
	if(int_flag3.bit.SWgain)
	{
//___________________________________________________________
//
//					求Kpos
//___________________________________________________________
			temp1 = (int32)Pn[secPosloopGainADD] - (int32)Pn[PosloopGainADD];
			temp1 *= 10;
			if(int_flag3.bit.HowGain)
			{
				temp1 = -temp1;
			}
			if(Pn[SelGainPosTIMADD])
			{
				Kpos = Kpos + temp1 / (int16)Pn[SelGainPosTIMADD];
				temp2 = abs(temp1) % Pn[SelGainPosTIMADD];
				SWKposRes += temp2;
				if(SWKposRes >= Pn[SelGainPosTIMADD])
				{
					SWKposRes -= Pn[SelGainPosTIMADD];
					if(temp1 > 0)
					{
						Kpos++;
					}
					else
					{
						Kpos--;
					}
				}
			}
			else
			{
				Kpos += temp1;
			}
			if(!int_flag3.bit.HowGain && 
			   (temp1 >= 0 && Kpos >= (long)Pn[secPosloopGainADD]*10 || temp1 < 0 && Kpos <= (long)Pn[secPosloopGainADD]*10))
			{
				Kpos = Pn[secPosloopGainADD]*10;
				Kpspeed = Pn[secSpdloopGainADD];
				Kispeed = Pn[secSpdloopItimeADD];
				Tiqr = Pn[secTCRFiltTimeADD];
				SWKposRes = 0;
				SWKspdRes = 0;
				SWKispdRes = 0;
				int_flag3.bit.SWgain = 0;
				int_flag3.bit.StartSW = 0;
				int_flag3.bit.HowGain = 1;
			}
			else if(int_flag3.bit.HowGain && 
			   (temp1 >= 0 && Kpos >= (long)Pn[PosloopGainADD]*10 || temp1 < 0 && Kpos <= (long)Pn[PosloopGainADD]*10))
			{
				Kpos = Pn[PosloopGainADD]*10;
				Kpspeed = Pn[SpdloopGainADD];
				Kispeed = Pn[SpdloopItimeADD];
				Tiqr = Pn[TCRFiltTimeADD];
				SWKposRes = 0;
				SWKspdRes = 0;
				SWKispdRes = 0;
				int_flag3.bit.SWgain = 0;
				int_flag3.bit.StartSW = 0;
				int_flag3.bit.HowGain = 0;
			}
	}
	else
	{
		if(int_flag3.bit.HowGain)
		{
			Kpos = Pn[secPosloopGainADD]*10;
			Kpspeed = Pn[secSpdloopGainADD];
			Kispeed = Pn[secSpdloopItimeADD];
			Tiqr = Pn[secTCRFiltTimeADD];
		}
		else
		{
			Kpos = Pn[PosloopGainADD]*10;
			Kpspeed = Pn[SpdloopGainADD];
			Kispeed = Pn[SpdloopItimeADD];
			Tiqr = Pn[TCRFiltTimeADD];
		}
	}
}
void GainSwitch(void)
{
int32 Tmp;
	switch(memGainSW)
	{
		case 0:
			Kpos = Pn[PosloopGainADD]*10;
			Kpspeed = Pn[SpdloopGainADD];
			Kispeed = Pn[SpdloopItimeADD];
			Tiqr = Pn[TCRFiltTimeADD];
			break;
		case 1:
			/*
			if(input_state.bit.G_SEL)
			{
				Kpos = Pn[PosloopGainADD]*10;
				Kpspeed = Pn[SpdloopGainADD];
				Kispeed = Pn[SpdloopItimeADD];
				Tiqr = Pn[TCRFiltTimeADD];
			}
			else
			{
				Kpos = Pn[secPosloopGainADD]*10;
				Kpspeed = Pn[secSpdloopGainADD];
				Kispeed = Pn[secSpdloopItimeADD];
				Tiqr = Pn[secTCRFiltTimeADD];
			}
			*/
			cacuGain(1);
			break;
		case 2:
			Tmp = labs(Iqr * 100 / Iqn);
			cacuGain(Tmp);
			break;
		case 3:
			Tmp = labs(Ek);
			cacuGain(Tmp);
			break;
		case 4:
			Tmp = labs(SPEEDACC);
			cacuGain(Tmp);
			break;
		case 5:
			Tmp = labs(SPEED/SpeedUnit);
			cacuGain(Tmp);
			break;
		case 6:			
			Tmp = labs(SwGainPgerr);
			cacuGain(Tmp);
			break;
		default:
			Kpos = Pn[PosloopGainADD]*10;
			Kpspeed = Pn[SpdloopGainADD];
			Kispeed = Pn[SpdloopItimeADD];
			Tiqr = Pn[TCRFiltTimeADD];
			break;				
	}
}
void Jdynamic(void)
{
int32 temp;
int16 Jtemp,a[4],SignFlag;
	if(state_flag2.bit.Son)
	{
		jxcnt++;
		if(jxcnt >= 1)			// 0.1ms
		{
			jxcnt = 0;
			DeltSpd = speed - lSpeed;
			jpcnt++;
			if(jpcnt <= 100)		
			{
				SgmaSpd0 += DeltSpd;
				SgmaIq0 += Iq;
				SgmaSpd[2] += speed;				
			}
			else
			{
				SgmaSpd0 = 0;
				SgmaIq0 = 0;
				jpcnt = 0;
				SgmaSpd0 += DeltSpd;
				SgmaIq0 += Iq;								
			}
//______________________________________________________
//
//			积分过程中速度方向是否变化
//______________________________________________________

			if(speed>0)
			{
				if(lSpeed<=0)
				{
					SgmaSpd0 = 0;
					SgmaIq0 = 0;
					jpcnt = 0;
					SgmaSpd[2] = 0;
					if(!int_flag3.bit.secondSgma)
					{
						SgmaSpd[0] = 0;
						SgmaIq[0] = 0;
						jsgmacnt[0] = 0;
						aveSgmaSpd[0] = 0;
						Sgma5Delt = 0;
						jsgmacnt[2] = 0;						
					}
					else
					{
						SgmaSpd[1] = 0;
						SgmaIq[1] = 0;
						jsgmacnt[1] = jsgmacnt[0];
						aveSgmaSpd[1] = 0;
					}
				}
			}
			else
			{
				if(lSpeed>=0)
				{
					SgmaSpd0 = 0;
					SgmaIq0 = 0;
					jpcnt = 0;
					SgmaSpd[2] = 0;
					if(!int_flag3.bit.secondSgma)
					{
						SgmaSpd[0] = 0;
						SgmaIq[0] = 0;
						jsgmacnt[0] = 0;
						aveSgmaSpd[0] = 0;
						Sgma5Delt = 0;
						jsgmacnt[2] = 0;						
					}
					else
					{
						SgmaSpd[1] = 0;
						SgmaIq[1] = 0;
						jsgmacnt[1] = jsgmacnt[0];
						aveSgmaSpd[1] = 0;
					}
				}
			}
			lSpeed = speed;
//____________________________________________________________
			if(jpcnt == 100)				// 10ms 完成一段积分
			{
				temp = SgmaSpd[2] / 100;
				SgmaSpd[2] = 0;
				if(labs(temp) > 100)
				{

					if(!int_flag3.bit.secondSgma)
					{
						aveSgmaSpd[0] += temp;				// 用于速度方向判断
						Sgma5Delt += SgmaSpd0;
						jsgmacnt[2]++;
						SgmaSpd[0] += SgmaSpd0;
						SgmaIq[0] += SgmaIq0;
						jsgmacnt[0]++;
						if(jsgmacnt[2] >= 50)
						{
							jsgmacnt[2] = 0;
							if(labs(Sgma5Delt) < 100 * SpeedUnit) 
							{
								SgmaSpd[0] = 0;
								SgmaIq[0] = 0;
								jsgmacnt[0] = 0;
								aveSgmaSpd[0] = 0;								
							}
							Sgma5Delt = 0;
						}
						else if(labs(SgmaSpd[0]) > 100 * SpeedUnit)
						{
							int_flag3.bit.secondSgma = 1;
							jsgmacnt[1] = jsgmacnt[0];
						}
						else if(jsgmacnt[0] >= 100)
						{
							SgmaSpd[0] = 0;
							SgmaIq[0] = 0;
							jsgmacnt[0] = 0;
							aveSgmaSpd[0] = 0;
							Sgma5Delt = 0;
							jsgmacnt[2] = 0;
						}
					}
					else
					{
						aveSgmaSpd[1] += temp;
						SgmaSpd[1] += SgmaSpd0;
						SgmaIq[1] += SgmaIq0;
						jsgmacnt[1]--;
						if(aveSgmaSpd[0] >= 0)
						{
							a[0] = 1;
						}
						else
						{
							a[0] = -1;
						}
						if(aveSgmaSpd[1] >= 0)
						{
							a[1] = 1;
						}
						else
						{
							a[1] = -1;
						}
						if(SgmaSpd[0] >= 0)
						{
							a[2] = 1;
						}
						else
						{
							a[2] = -1;
						}
						if(SgmaSpd[1] >= 0)
						{
							a[3] = 1;
						}
						else
						{
							a[3] = -1;
						}
						if(a[0] * a[1] > 0 || a[2] * a[3] > 0)
						{
							SignFlag = 1;
						}
						else
						{
							SignFlag = 0;
						}
						if(jsgmacnt[1]==0)
						{
							if((labs(SgmaSpd[0]-SgmaSpd[1]) >  100 * SpeedUnit)
								&& (labs(SgmaSpd[1]) >= (jsgmacnt[0] * SpeedUnit<<1))		// 5s-->1000r
								&& SignFlag)
							{    							
								int_flag3.bit.secondSgma = 0;
								int_flag3.bit.endSgma = 1;
							}
							else
							{
								SgmaSpd[1] = 0;
								SgmaIq[1] = 0;
								jsgmacnt[1] = jsgmacnt[0];
								aveSgmaSpd[1] = 0;
							}
						}
					}
					if(int_flag3.bit.endSgma)
					{
						if(aveSgmaSpd[0] > 0)
						{
							if(aveSgmaSpd[1] < 0)					// 负载阻尼方向不同
							{
								if(memGainSet <= 3)
								{
									Jtemp = (SgmaIq[0] + SgmaIq[1])/(SgmaSpd[0] + SgmaSpd[1]);
									Jtemp = (int32)Jtemp * 96 * varFactualTn / Iqm;		// 10的负5次方
								}
								else
								{
									Jtemp = Jx;
								}
							}
							else
							{
								Jtemp =  (SgmaIq[0] - SgmaIq[1])/(SgmaSpd[0] - SgmaSpd[1]);
								Jtemp = (int32)Jtemp * 96 * varFactualTn / Iqm;		// 10的负5次方
							}
						}
						else
						{
							if(aveSgmaSpd[1] > 0)
							{
								if(memGainSet <= 3)
								{
									Jtemp = (SgmaIq[0] + SgmaIq[1])/(SgmaSpd[0] + SgmaSpd[1]);
									Jtemp = (int32)Jtemp * 96 * varFactualTn / Iqm;		// 10的负5次方
								}
								else
								{
									Jtemp = Jx;
								}
							}
							else
							{
								Jtemp = (SgmaIq[0] - SgmaIq[1])/(SgmaSpd[0] - SgmaSpd[1]);
								Jtemp = (int32)Jtemp * 96 * varFactualTn / Iqm;		// 10的负5次方
							}
						}

						if(Jtemp > 0)
						{
							Jtotal = Jtotal + Jtemp - Jauto[0];
							Jauto[0] = Jauto[1];
							Jauto[1] = Jauto[2];
							Jauto[2] = Jauto[3];
							Jauto[3] = Jauto[4];
							Jauto[4] = Jauto[5];
							Jauto[5] = Jauto[6];
							Jauto[6] = Jauto[7];
							Jauto[7] = Jauto[8];
							if(memGainSet == 2 || memGainSet == 5)
							{
								Jauto[7] = Jtemp;
							}
							Jauto[8] = Jauto[9];
							Jauto[9] = Jauto[10];
							Jauto[10] = Jauto[11];
							Jauto[11] = Jauto[12];
							Jauto[12] = Jauto[13];
							Jauto[13] = Jauto[14];
							Jauto[14] = Jauto[15];
							Jauto[15] = Jauto[16];
							Jauto[16] = Jauto[17];
							Jauto[17] = Jauto[18];
							Jauto[18] = Jauto[19];
							Jauto[19] = Jauto[20];
							Jauto[20] = Jauto[21];
							Jauto[21] = Jauto[22];
							Jauto[22] = Jauto[23];
							Jauto[23] = Jauto[24];
							Jauto[24] = Jauto[25];
							Jauto[25] = Jauto[26];
							Jauto[26] = Jauto[27];
							Jauto[27] = Jauto[28];
							Jauto[28] = Jauto[29];
							Jauto[29] = Jauto[30];
							Jauto[30] = Jauto[31];
							Jauto[31] = Jauto[32];
							Jauto[32] = Jauto[33];
							Jauto[33] = Jauto[34];
							Jauto[34] = Jauto[35];
							Jauto[35] = Jauto[36];
							Jauto[36] = Jauto[37];
							Jauto[37] = Jauto[38];
							Jauto[38] = Jauto[39];
							Jauto[39] = Jauto[40];
							Jauto[40] = Jauto[41];
							Jauto[41] = Jauto[42];
							Jauto[42] = Jauto[43];
							Jauto[43] = Jauto[44];
							Jauto[44] = Jauto[45];
							Jauto[45] = Jauto[46];
							Jauto[46] = Jauto[47];
							Jauto[47] = Jauto[48];
							Jauto[48] = Jauto[49];
							Jauto[49] = Jauto[50];
							Jauto[50] = Jauto[51];
							Jauto[51] = Jauto[52];
							Jauto[52] = Jauto[53];
							Jauto[53] = Jauto[54];
							Jauto[54] = Jauto[55];
							Jauto[55] = Jauto[56];
							Jauto[56] = Jauto[57];
							Jauto[57] = Jauto[58];
							Jauto[58] = Jauto[59];
							Jauto[59] = Jauto[60];
							Jauto[60] = Jauto[61];
							Jauto[61] = Jauto[62];
							Jauto[62] = Jauto[63];
							Jauto[63] = Jtemp;
						}
						if(memGainSet == 2 || memGainSet == 5)
						{
							aveJ = Jtotal >> 3;	
						}
						else if(memGainSet == 1 || memGainSet == 4)
						{
							aveJ = Jtotal >> 6;
						}
						else if(Jtemp > 0)
						{
							aveJ = Jtemp;
							Jauto[0] = Jtemp;
						}
						else
						{
							aveJ = Jx;
						}
						if(Jauto[0] || cacuJcnt >= 63)
						{
							Jx = aveJ;
							perJload = (int32)((int16)(Jx - JMotor)) * 100 / JMotor;
							if(perJload<0)
							{
								perJload = 0;
							}
						}
						else
						{							
							cacuJcnt++;
							Jx = Jtotal / cacuJcnt;
							perJload = (int32)((int16)(Jx - JMotor)) * 100 / JMotor;
							if(perJload<0)
							{
								perJload = 0;
							}								 
						}
						jncnt++;
						if(jncnt >= 100)
						{
							SgmaveJ += aveJ;
							useJ = SgmaveJ / 100;
							jncnt = 0;
							SgmaveJ = 0;
						}
						SgmaSpd[0] = 0;
						SgmaSpd[1] = 0;
						SgmaIq[0] = 0;
						SgmaIq[1] = 0;
						jsgmacnt[0] = 0;
						jsgmacnt[1] = 0;
						aveSgmaSpd[0] = 0;
						aveSgmaSpd[1] = 0;
						Sgma5Delt = 0;
						jsgmacnt[2] = 0;
						int_flag3.bit.endSgma = 0;
					}
				}
				else
				{
					if(!int_flag3.bit.secondSgma)
					{
						SgmaSpd[0] = 0;
						SgmaIq[0] = 0;
						jsgmacnt[0] = 0;
						aveSgmaSpd[0] = 0;
						Sgma5Delt = 0;
						jsgmacnt[2] = 0;
					}
					else
					{
						SgmaSpd[1] = 0;
						SgmaIq[1] = 0;
						jsgmacnt[1] = jsgmacnt[0];
						aveSgmaSpd[1] = 0;						
					}

				}
			}
		}
	}
	else
	{
		int_flag3.bit.pSgma = 0;
		jxcnt = 0;
		lSpeed = 0;
		SgmaSpd[0] = 0;
		SgmaSpd[1] = 0;
		SgmaSpd[2] = 0;
		SgmaIq[0] = 0;
		SgmaIq[1] = 0;
		SgmaIq[2] = 0;
		SgmaveJ = 0;
		aveSgmaSpd[0] = 0;
		aveSgmaSpd[1] = 0;
		int_flag3.bit.secondSgma = 0;
		int_flag3.bit.endSgma = 0;
		jsgmacnt[0] = 0;
		jsgmacnt[1] = 0;
		Sgma5Delt = 0;
		jsgmacnt[2] = 0;
		jncnt = 0;
		jpcnt = 0;
		DeltSpd = 0;
	}
}

//接点速度控制的速度选择增加至7段，qdd by 20110527
void SJDControl(void)
{
	if(input_state.bit.PCON)
	{
		if(input_state.bit.PCL && input_state.bit.NCL)
		{
			SPEEDINS = 0;
		}
		else if(input_state.bit.PCL)
		{
			SPEEDINS = (int16)Pn[SPEED1ADD];
		}
		else if(input_state.bit.NCL)
		{
			SPEEDINS = (int16)Pn[SPEED2ADD];
		} 
		else
		{
			SPEEDINS = (int16)Pn[SPEED3ADD];
		}
	}
	else if(input_state.bit.PCON == 0)
	{
		if(input_state.bit.PCL && input_state.bit.NCL)
		{
			SPEEDINS = (int16)Pn[SPEED4ADD];
		}
		else if(input_state.bit.PCL)
		{
			SPEEDINS = (int16)Pn[SPEED5ADD];
		}
		else if(input_state.bit.NCL)
		{
			SPEEDINS = (int16)Pn[SPEED6ADD];
		} 
		else
		{
			SPEEDINS = (int16)Pn[SPEED7ADD];
		}
	} 
	
	SPEEDINS *= SpeedUnit;
	LmtSpd();

	CMDref = SPEEDINS;
	if(pos_flag4.bit.PNOTzcl)
	{
		NextAnaSC();
	}
	else
	{
		if(membit01.bit.bit00)
		{
			SPEEDINS = - SPEEDINS;
		}
		if(state_flag2.bit.Son)
		{
			SModule4();		//        ; 软件加减速
			SModule3();		//        ; 计算出对应的限幅数值
			SPEED = SPEEDINSout;
			SModule2();		//        ; 速度PI调节
			limitIqr();		//        ; 具体的限幅数值
			SModule1();		//        ; 转矩滤波
		}
		else
		{
			x_speed = 0;
			SPEEDINS = 0;
			SPEEDINSout = 0;
			SPEED = 0;
		}
		JudgeVCMP();
	}
	input_laststate.bit.SHOM = input_state.bit.SHOM;	
}

void ParSpd(void)
{
	SPEEDINS = CommandSpeed;//(int16)Pn[ParSpdADD];   //250712 改为由MODBUS改为由SPI传送
	SPEEDINS *= SpeedUnit;
//___________________________________
/*	test2++;
	if(test2>10)
	{
		test1 += 1;
		test2 = 0;
	}
	if (test1 >= 360)
	{
		test1 -= 360;
	}
	SPEEDINS = (int32)3000 * sintab[test1];
	SPEEDINS >>= 12;*/
//____________________________________
	LmtSpd();
	CMDref = SPEEDINS;
	if(pos_flag4.bit.PNOTzcl)
	{
		NextAnaSC();
	}
	else 
	{
		if(membit01.bit.bit00)
		{
			SPEEDINS = - SPEEDINS;
		}
		if(state_flag2.bit.Son)
		{
			SModule4();		//        ; 软件加减速
			SModule3();		//        ; 计算出对应的限幅数值
			SPEED = SPEEDINSout;
			SModule2();		//        ; 速度PI调节
			limitIqr();		//        ; 具体的限幅数值
			SModule1();		//        ; 转矩滤波子程序
		}
		else
		{
			x_speed = 0;
			SPEEDINS = 0;
			SPEEDINSout = 0;
			SPEED = 0;
		}
		JudgeVCMP();
	}	
}

void JudgeVCMP(void)
{Uint32 Tmp1,Tmp2;
	if(membit01.bit.bit00)
	{
		Tmp1 = labs(-SPEEDINS - Unspd);
	}
	else
	{
		Tmp1 = labs(SPEEDINS - Unspd);
	}
	Tmp2 = Pn[VCMPSPDADD];
	Tmp2 *= SpeedUnit;
	if(Tmp1 > Tmp2)
	{
		pos_flag4.bit.VCMP_COIN = 0;
	}
	else
	{
		pos_flag4.bit.VCMP_COIN = 1;
	}
}

void AnalogSpdCol(void)
{
	CMDref = VCMD;
	if(pos_flag4.bit.PNOTzcl)
	{
		NextAnaSC();
	}
	else
	{
		if(labs(UnVCMD - Unspd) > Pn[VCMPSPDADD]*SpeedUnit)
		{
			pos_flag4.bit.VCMP_COIN = 0;
		}
		else
		{
			pos_flag4.bit.VCMP_COIN = 1;
		}
		if(state_flag2.bit.Son)
		{	
			if(Timers.cntdone)
			{
				SPEED = 0;
				SModule3();	//
				SModule2();	//        ; 速度PI调节
				limitIqr();	//
				SModule1();	//        ; 转矩滤波
			}
			else
			{
				if(memhex01.hex.hex00 & 0x0001)
				{
					pos_flag4.bit.TCRFFQvalid = 1;
				}
				else
				{
					pos_flag4.bit.TCRFFQvalid = 0;
				}
				SPEEDINS = VCMDG;
				SModule4();
				NextAnaSC();
			}
		}
		else
		{
			x_speed = 0; 		// \
			SPEEDINS = 0;  		// | 清零
			SPEEDINSout = 0;  	// |
			SPEED = 0; 			// /
			ZeroEk = 0;
			int_flag3.bit.ZCL = 0;//零夹紧标志
		}
	}
	input_laststate.bit.SHOM = input_state.bit.SHOM;
}

void AnalogTcrCol(void)
{int32 Tmp1;
	CMDref = (int16)TCMD;
	if(pos_flag4.bit.PNOTzcl)
	{
		NextAnaSC();
	}
	else if(state_flag2.bit.Son)
	{	
		if(Timers.cntdone)
		{
			Iqr = 0;
			// exit
		}
		else
		{
			Iqrbuf = AnIqrg;//(TCMDG * Iqn/100);
			TCRo = (int16)Iqrbuf;
			SModule3();
			Tmp1 = Pn[TCRColSpdLmtADD];
			Tmp1 *= SpeedUnit;
			if(membit01.bit.bit01)
			{
				if(labs(VCMDG) >= Tmp1)
				{
					SPEEDINS = Tmp1;
				}
				else
				{
					SPEEDINS = labs(VCMDG);
				}
			}
			else
			{// noAnalimit
				SPEEDINS = Tmp1;
			}
			LmtSpd();

			Tmp1 = labs(speed)- SPEEDINS;
			if(Tmp1 >= 0)                        //增加切换延时2011-01-18
			{
				int_flag3.bit.TCR_lmtspd = 1;
				Switchtime = 0;
			}
			else if(labs(Tmp1) > Pn[spddelay_width]* SpeedUnit)
			{
				Switchtime++;
				if(Switchtime < Pn[Tcrdelay_time]) 
				{
					int_flag3.bit.TCR_lmtspd = 1;
				}
				else
				{
					Switchtime = Pn[Tcrdelay_time];
					int_flag3.bit.TCR_lmtspd = 0;
				}				
			}
			else
			{
				Switchtime = 0;
			}

			if(int_flag3.bit.TCR_lmtspd)
			{// LmtSpd_done
				SPEED = SPEEDINS;
				if(AnIqrg < 0)
				{
					SPEED = - SPEED;
				}
				SModule2();
				if(((Iqrbuf >= 0) && (TCRo >= 0)) || ((Iqrbuf < 0) && (TCRo < 0)))
				{
					if(labs(Iqrbuf) >= labs(TCRo))
					{
						Iqrbuf = TCRo;
						x_speed = 0;
					}
				}				
			}
			limitIqr();
			SModule1();
		}
	}
	else
	{
		x_speed = 0;
		SPEEDINS = 0;
		SPEEDINSout = 0;
		SPEED = 0;
	}
	// exit3
	input_laststate.bit.SHOM = input_state.bit.SHOM;	
}

/***********************************************************
  Function: // 函数名称
		void HOme_State_Position(void)
  Description:  // 回零状态判断和更新函数
  Calls:     
  Called By:  void PControl(void)
  Input:  无
  Output: 无
  2012-01 修改
***********************************************************/

void Home_State_Position(void)
{
	if((home_flag4.bit.Home_State == NOT_ARRIVAL_ORG) && input_state.bit.ORG)  //回零状态为未遇到零位信号状态且零位信号有效
	{
		home_flag4.bit.Home_State = ARRIVAL_ORG;  //回零状态为遇到零位信号状态
	}
	if((home_flag4.bit.Home_State == ARRIVAL_ORG) && (home_flag4.bit.C_Zero == 1)) //回零状态为遇到零位信号状态且C脉冲信号有效
	{
		home_flag4.bit.Home_State = ARRIVAL_ZERO;  //回零状态为遇到C脉冲信号状态
	}
	if(home_flag4.bit.Home_State == ARRIVAL_ZERO)
	{
		PD_Zero();  //回零结束的判断
		if(home_flag4.bit.Home_State == ORG_HOME_END)
		{
			Zero_Ek = 0;
			Home_Ek = 0;
			Ek = 0;
			RmEk = 0;
			RmFg = 0;
			Pgerr = 0;
			Pgerro = 0;
			Pg[1] = *GPCNT;
		}
	}
	if(home_flag4.bit.Home_State == ORG_HOME_END) //回零结束后相关量清零并置位回零结束标志位
	{
		Zero_Ek = 0;
		Ek = 0;
		RmEk = 0;
		RmFg = 0;
		Pgerr = 0;
		Pg[1] = *GPCNT;
		Pgerro = 0;
		Zero_Cnt = 0;
		Home_Ek = 0;
		home_flag4.bit.Ek_flag = 0;
		home_flag4.bit.HM_flag = 0;
		home_flag4.bit.Homing_flag = 1;
	}
	else
	{
		home_flag4.bit.Homing_flag = 0;
	}
}

/***********************************************************
  Function: // 函数名称
		void Ref_Zero(void)
  Description:  // 回零偏移脉冲数的计算
  Calls:     
  Called By:  void PControl(void)
  Input:  无
  Output: 无
  2012-01 修改
***********************************************************/

void Ref_Zero(void)
{
int32 temp;
	temp = (int32)Pn[PulseOffset-1] * 10000 + Pn[PulseOffset];
	if(membit16.bit.bit00 == 1)
	{
		if(membit16.bit.bit01 == 1)
		{
			if(membit01.bit.bit00 == 1)
			{
				Zero_Ek = temp;
			}
			else
			{
				Zero_Ek = -temp;
			}
		}
		else
		{
			if(membit01.bit.bit00 == 1)
			{
				Zero_Ek = -temp;
			}
			else
			{
				Zero_Ek = temp;
			}
		}
	}
	else
	{
		if(membit16.bit.bit01 == 1)
		{
			if(membit01.bit.bit00 == 1)
			{
				Zero_Ek = -temp;
			}
			else
			{
				Zero_Ek = temp;
			}
		}
		else
		{
			if(membit01.bit.bit00 == 1)
			{
				Zero_Ek = temp;
			}
			else
			{
				Zero_Ek = -temp;
			}
		}
	}
}

/***********************************************************
  Function: // 函数名称
		void Pos_Zero(void)
  Description:  // 回零过程中位置环调节器
  Calls:     
  Called By:  void Home_Operation(void)
  Input:  无
  Output: 无
  2012-01 修改
***********************************************************/

void Pos_Zero(void)
{
int32 Tmp,Tmp1;
int32 temp1;
	if((memCurLoop00.hex.hex00 == Encoder_Abs17bit) || (memCurLoop00.hex.hex00 == Encoder_Inc17bit))
	{
		temp1 = 13;   //每次给定的脉冲数（绝对值编码器）
	}
	else
	{
		temp1 = 1;   //每次给定的脉冲数（2500线或旋变）
	}
	if(home_flag4.bit.Ek_flag == 0)   //每次给定脉冲数的选择和判断
	{
		if(labs(Zero_Ek) <= temp1)
		{
			Home_Ek += Zero_Ek;
			home_flag4.bit.Ek_flag = 1;
		}
		else
		{
			if(Zero_Ek > 0)
			{
				Home_Ek += temp1;
				Zero_Ek -= temp1;
			}
			else
			{
				Home_Ek -= temp1;
				Zero_Ek += temp1;
			}
		}
	}
	Home_Ek -= Pferr;  //每次给定脉冲数的调节
	Tmp1 = labs(Home_Ek);
	if(state_flag2.bit.Son)
	{
		if(memGainSet)
		{
			Kpos = RigidityGain[Pn[Rigidity]]*10;
		}

		Tmp1 = Tmp1>>16;
		Tmp1 = Tmp1 * Kpos;
		if(Tmp1  > 0x4000)
		{
			if(Home_Ek < 0)
			{
				SPEED = - 5000;
			}
			else
			{
				SPEED = 5000;
			}
		}
		else
		{
			Tmp1 = memEncNum / 15;
			Tmp = Home_Ek * Kpos / Tmp1;
			RmPosgain = RmPosgain + (Home_Ek * Kpos) % Tmp1;
			if (RmPosgain/Tmp1)
			{
				if (RmPosgain > 0)
				{
					Tmp++;
					RmPosgain -= Tmp1;
				}
				else
				{
					Tmp--;
					RmPosgain += Tmp1;
				}
			}
			if(labs(Tmp) >= 5000)
			{
				if(Tmp < 0)
				{
					SPEED = - 5000;
				}
				else
				{
					SPEED = 5000;
				}
			}
			else
			{
				SPEED = Tmp;
			}
		}
		if(pos_flag4.bit.PNOTzcl)
		{
			NextAnaSC();
		}
		else
		{
			SModule3();
			SModule2();
			limitIqr();
			SModule1();
		}
	}
}

/***********************************************************
  Function: // 函数名称
		void PD_Zero(void)
  Description:  // 回零结束的判断
  Calls:     
  Called By:  void Home_State_Position(void)
  Input:  无
  Output: 无
  2012-01 修改
***********************************************************/

void PD_Zero(void)
{
	if (home_flag4.bit.Ek_flag == 1)
	{
		if ((Zero_Cnt >= 10000) && (labs(Home_Ek) <= 2))
		{
			home_flag4.bit.Home_State = ORG_HOME_END;
			home_flag4.bit.C_Zero = 0;
		}
		else
		{
			if (Zero_Cnt <= 10000)
			{
				Zero_Cnt ++;
			}
		}
	}
}

/***********************************************************
  Function: // 函数名称
		void Home_Operation(void)
  Description:  //回零过程中电机运行状态的控制
  Calls:     
  Called By:  void PControl(void)
  Input:  无
  Output: 无
  2012-01 修改
***********************************************************/

void Home_Operation(void)
{
int16 temp;
	temp = 0;
	switch(home_flag4.bit.Home_State)
	{
		case NOT_ARRIVAL_ORG:
			temp = (int16)Pn[FindRefGoOTspeedADD] * SpeedUnit;
			if(membit16.bit.bit00 == 1)
			{
				temp = -temp;
			}
			break;
		case ARRIVAL_ORG:
			temp = (int16)Pn[LeaveOTspeedADD] * SpeedUnit;
			if(membit16.bit.bit00 == 1)
			{
				if(membit16.bit.bit01 == 1)
				{
					temp = -temp;
				}
				else
				{
					temp = temp;
				}
			}
			else
			{
				if(membit16.bit.bit01 == 1)
				{
					temp = temp;
				}
				else
				{
					temp = -temp;
				}
			}
			break;
		case ARRIVAL_ZERO:
			break;
		default:
			temp = 0;
			Zero_Ek = 0;
			RmEk = 0;
			break;
	}
	if(home_flag4.bit.Home_State == NOT_ARRIVAL_ORG)
	{   //以Pn685设定的速度撞向原点信号ORG
		if(pos_flag4.bit.PNOTzcl)
		{
			NextAnaSC();
		}
		else if(state_flag2.bit.Son)
		{										
			if(membit01.bit.bit00)
			{
				SPEEDINS = -temp;						
			}
			else
			{
				SPEEDINS = temp;
			}
			SModule4();
			SModule3();
			SPEED = SPEEDINSout;
			SModule2();
			limitIqr();
			SModule1();
		}
		else
		{
			x_speed = 0;
			SPEEDINS = 0;
			SPEEDINSout = 0;
			SPEED = 0;
			home_flag4.bit.HM_flag = 0;
			Zero_Cnt = 0;
			home_flag4.bit.Ek_flag = 0;
		}
	}
	else if (home_flag4.bit.Home_State == ARRIVAL_ORG)
	{   //以Pn686设定的速度离开原点信号ORG
		if(Timers.Fndcnt < 2000) //找到原点信号ORG后的延时
		{
			Timers.Fndcnt++;
			x_speed = 0;
			SPEEDINS = 0;
			SPEEDINSout = 0;
			SPEED = 0;
            SModule3();
			SModule2();
			limitIqr();
			SModule1();
		}	
		else
		{
			if(pos_flag4.bit.PNOTzcl)
			{
				NextAnaSC();
			}
			else if(state_flag2.bit.Son)
			{										
				if(membit01.bit.bit00)
				{
					SPEEDINS = -temp;						
				}
				else
				{
					SPEEDINS = temp;
				}
				SModule4();
				SModule3();
				SPEED = SPEEDINSout;
				SModule2();
				limitIqr();
				SModule1();
			}
			else
			{
				x_speed = 0;
				SPEEDINS = 0;
				SPEEDINSout = 0;
				SPEED = 0;
				home_flag4.bit.HM_flag = 0;
				Zero_Cnt = 0;
				home_flag4.bit.Ek_flag = 0;
			}
		}
	}
	else if (home_flag4.bit.Home_State == ARRIVAL_ZERO)
	{   //离开原点信号ORG到达C脉冲状态
		if(pos_flag4.bit.PNOTzcl)
		{
			NextAnaSC();
		}
		else if (state_flag2.bit.Son)
		{
			Pos_Zero(); //回零偏移脉冲数控制
		}
		else
		{
			x_speed = 0;
			SPEEDINS = 0;
			SPEEDINSout = 0;
			SPEED = 0;
			home_flag4.bit.HM_flag = 0;
			Zero_Cnt = 0;
			home_flag4.bit.Ek_flag = 0;
		}
	}
	if(home_flag4.bit.Home_State == ORG_HOME_END)
	{
		Zero_Ek = 0;
		Home_Ek = 0;
		Ek = 0;
		RmEk = 0;
		RmFg = 0;
		Pgerr = 0;
		Pgerro = 0;
		Pg[1] = *GPCNT;
		Zero_Cnt = 0;
		home_flag4.bit.Ek_flag = 0;
		home_flag4.bit.HM_flag = 0;
		home_flag4.bit.Homing_flag = 1;
	}
	else
	{
		home_flag4.bit.Homing_flag = 0;
	}
}

/***********************************************************
  Function: // 函数名称
		   void PControl(void)
  Description:  //位置控制主体函数
  Calls:   void Ref_Zero(void)
           void Home_State_Position(void)
	       void Home_Operation(void)
	       void runposition(void) 
  Called By:  void speed_loop_process(void)
  Input:  无
  Output: 无
  2012-01 修改
***********************************************************/

void PControl(void)
{


	if((pos_flag4.bit.InhibitPulseIn == 0) && membit16.bit.bit02 && pos_flag4.bit.VCMP_COIN && ((input_laststate.bit.SHOM == 0) && input_state.bit.SHOM))
	{  //外部信号SHOM触发回零标志
		if(home_flag4.bit.HM_flag == 0)
		{
			Ref_Zero();
			home_flag4.bit.HM_flag = 1;
			home_flag4.bit.Homing_flag = 0;
			Home_Ek = 0;
			RmPosgain = 0;
		}
	}
	if((home_flag4.bit.HM_flag == 1) && (home_flag4.bit.Home_State != ORG_HOME_END))
	{  //回零操作
		Pgerr = 0;
		Pgerro = 0;
		Home_State_Position();
		Home_Operation();
	}
	else
	{  //上位机脉冲控制方式

		Pg[0] = *GPCNT;//Pos485;//
        //#if P485
		Pgerr=Pos485;//+EncodrPidOut/100;
        //#endif
		//Pgerr =Pg[0] - Pg[1];// 10 333转  Pos485;

		CMDref = Pgerr;
		Pg[1] = Pg[0];
		if(((memBusSel == 1) || (memBusSel == 2)))    //串口模式 参数6中bus种类
		{
			Pgerr = profibus_buf.pbus_Pgerr;
			Pgerr=Pos485;
		}
		if(pos_flag4.bit.InhibitPulseIn)  //回0的相关操作
		{
			Pgerr = 0;
		}
		Filtime_pos = Pn[POSFiltTimeADD];
		runposition();

		home_flag4.bit.Home_State = NOT_ARRIVAL_ORG; //可多次回零
	}
	input_laststate.bit.SHOM = input_state.bit.SHOM;

}

void JDPosCol(void)
{int16 temp;
	if(Pn[JDPOS_PARSADD] & 0x0020)
	{
		pos_flag4.bit.JDPosNeedStart = 1;			// 点位控制需要启动
	}
	else
	{
		pos_flag4.bit.JDPosNeedStart = 0;
	}
/*	if(memCurLoop00.hex.hex00 == Encoder_Abs17bit)
	{
		findRef(131072);
	}
	else if(memCurLoop00.hex.hex00 == Encoder_Inc17bit && !alm_absenc.bit.FS)
	{
		findRef(131072);
	}
*/
	if(int_flag3.bit.pFindRef)
	{// PFindZero
		if((int_flag3.bit.FindRef_ot == 0) && (input_state.bit.POT == 0))
		{ // ZDrun
			CMDref = 1;
			if(pos_flag4.bit.PNOTzcl)
			{
				NextAnaSC();
			}
			else if(state_flag2.bit.Son)
			{					
				temp = (int16)Pn[FindRefGoOTspeedADD]*SpeedUnit;					
				if(membit01.bit.bit00)
				{
					SPEEDINS = -temp;						
				}
				else
				{
					SPEEDINS = temp;
				}
				SModule4();
				SModule3();
				SPEED = SPEEDINSout;
				SModule2();
				limitIqr();
				SModule1();
			}
			else
			{
				x_speed = 0;
				SPEEDINS = 0;
				SPEEDINSout = 0;
				SPEED = 0;
			}
		}
		else
		{
			if(input_state.bit.POT)
			{
				int_flag3.bit.FindRef_ot = 1;
			}
			// FDrun
			if(Timers.Fndcnt < 2000)
			{
				Timers.Fndcnt++;
				x_speed = 0;
				SPEEDINS = 0;
				SPEEDINSout = 0;
				SPEED = 0;
				SModule3();
				SModule2();
				limitIqr();
				SModule1();
			}	
			else
			{// BegFDrun
				CMDref = -1;
				if(pos_flag4.bit.PNOTzcl)
				{
					NextAnaSC();
				}
				else if(state_flag2.bit.Son)
				{	
					temp = (int16)Pn[LeaveOTspeedADD]*SpeedUnit;					
					if(membit01.bit.bit00)
					{
						SPEEDINS = temp;						
					}
					else
					{
						SPEEDINS = -temp;
					}
					SModule4();
					SModule3();
					SPEED = SPEEDINSout;
					SModule2();
					limitIqr();
					SModule1();
				}
				else
				{
					x_speed = 0;
					SPEEDINS = 0;
					SPEEDINSout = 0;
					SPEED = 0;										
				}
			}		
		}
	}
	else if(int_flag3.bit.nFindRef)
	{// NFindZero
		if((int_flag3.bit.FindRef_ot == 0) && (input_state.bit.NOT == 0))
		{ // ZDrun1
			CMDref = -1;
			if(pos_flag4.bit.PNOTzcl)
			{
				NextAnaSC();
			}
			else if(state_flag2.bit.Son)
			{					
				temp = (int16)Pn[FindRefGoOTspeedADD]*SpeedUnit;					
				if(membit01.bit.bit00)
				{
					SPEEDINS = temp;						
				}
				else
				{
					SPEEDINS = -temp;
				}
				SModule4();
				SModule3();
				SPEED = SPEEDINSout;
				SModule2();
				limitIqr();
				SModule1();
			}
			else
			{
				x_speed = 0;
				SPEEDINS = 0;
				SPEEDINSout = 0;
				SPEED = 0;
			}
		}
		else
		{
			if(input_state.bit.NOT)
			{
				int_flag3.bit.FindRef_ot = 1;
			}
			// FDrun1
			if(Timers.Fndcnt < 2000)
			{
				Timers.Fndcnt++;
				x_speed = 0;
				SPEEDINS = 0;
				SPEEDINSout = 0;
				SPEED = 0;
				SModule3();
				SModule2();
				limitIqr();
				SModule1();
			}	
			else
			{// BegFDrun1
				CMDref = 1;
				if(pos_flag4.bit.PNOTzcl)
				{
					NextAnaSC();
				}
				else if(state_flag2.bit.Son)
				{	
					temp = (int16)Pn[LeaveOTspeedADD]*SpeedUnit;					
					if(membit01.bit.bit00)
					{
						SPEEDINS = -temp;						
					}
					else
					{
						SPEEDINS = temp;
					}
					SModule4();
					SModule3();
					SPEED = SPEEDINSout;
					SModule2();
					limitIqr();
					SModule1();
				}
				else
				{
					x_speed = 0;
					SPEEDINS = 0;
					SPEEDINSout = 0;
					SPEED = 0;										
				}
			}		
		}
	}
	// NoFindZero
	else if(state_flag2.bit.Son)
	{
		if(pos_flag4.bit.JDPosNeedStart)
		{
			if(Pn[JDPOS_PARSADD] & JDPOS_StartSel)
			{// NCLdeal
				if(input_state.bit.NCL)
				{
					int_flag3.bit.JDPosStart = 0;
				}
				else
				{// NCLstart
					int_flag3.bit.JDPosStart = 1;
				}
				// nextNCLstart
				if(input_state.bit.PCL)
				{// NoSetZero
					NoSetZero();
				}
				else
				{// SetNdZero1
					int_flag3.bit.pFindRef = 1;
					JPOSn = ((int16)Pn[ProgStartPointADD]-1);
					InitJPOSnEND = 0;
					inPgcnt = 0;
				}
			}
			else
			{
				if(input_state.bit.PCL)
				{
					int_flag3.bit.JDPosStart = 0;
				}
				else
				{// PCLstart
					int_flag3.bit.JDPosStart = 1;
				}
				// nextPCLstart
				if(input_state.bit.NCL)
				{// NoSetZero
					NoSetZero();
				}
				else
				{// SetNdZero1
					int_flag3.bit.nFindRef = 1;
					JPOSn = ((int16)Pn[ProgStartPointADD]-1);
					InitJPOSnEND = 0;
					inPgcnt = 0;
				}
			}
		}
		else
		{// noPNCLstart
			if(input_state.bit.PCL == 0)
			{// SetPdZero
				int_flag3.bit.pFindRef = 1;
				JPOSn = ((int16)Pn[ProgStartPointADD]-1);
				InitJPOSnEND = 0;
				inPgcnt = 0;
			}
			else if(input_state.bit.NCL == 0)
			{// SetNdZero
				int_flag3.bit.nFindRef = 1;
				JPOSn = ((int16)Pn[ProgStartPointADD]-1);
				InitJPOSnEND = 0;
				inPgcnt = 0;
			}
			else
			{// NoSetZero
				NoSetZero();
			}
		}				
	}
	else
	{// NoSetZero
		NoSetZero();
	}
}

void NoSetZero(void) // 内部位置运行
{int32 Tmp; 
	if(pos_flag4.bit.VCMP_COIN)
	{
		if(Pn[JDPOS_PARSADD] & JDPOS_StepOrNot)
		{// P-CON换步
			//if(input_state.bit.PCON == 0)
			if((!(Pn[JDPOS_PARSADD] & JDPOS_StepValidSel) && (input_state.bit.PCON == 0)) ||	// 电平换步
			   (Pn[JDPOS_PARSADD] & JDPOS_StepValidSel) && ((input_laststate.bit.PCON) && (input_state.bit.PCON == 0)))// 脉冲换步
			{// NextStep
				if(inPgcnt == 0)
				{
					pos_flag4.bit.JPOSn = 0;		// 清除上一次进入某个点位控制的标志
					++JPOSn;
					if(JPOSn >= ((int16)Pn[ProgEndPointADD] + 1))
					{
						if(Pn[JDPOS_PARSADD] & JDPOS_OneOrLoopSel)
						{// JnStop
							JPOSn = (int16)Pn[ProgEndPointADD];		// 防止溢出
							pos_flag4.bit.JPOSn = 1; //	为避免多次执行最后一步，置进入该点位控制的标志
						}
						else
						{
							JPOSn = (int16)Pn[ProgStartPointADD];
						}
					}
				}
			}
		}
		else
		{// TMESTOP				
			// cacuStop				
			if(state_flag2.bit.Son)
			{// cacuSTEPTME
				if(JPOSn < 0)
				{
					Tmp = Pn[JOG15StopTimeADD];
				}
				else
				{
					Tmp = Pn[JOG0StopTimeADD + JPOSn];
				}
				//Tmp *= 100;
				//;_____________________________________________
				//;                     延时
				//;          当上一次的脉冲发完蟛拍苎邮奔剖?
				//;_____________________________________________
				if(inPgcnt == 0)
				{
					//pos_flag4.bit.JPOSn = 0;		// 清除上一次进入某个点位控制的标志
					if(++ Timers.STEPTME >= (Tmp*500))		// 500对应中断周期为100us
					{
						Timers.STEPTME = 0;
						// NextStep
						//;_____________________________________________
						//;                     换步
						//;          当上一次的脉冲发完后才能换步
						//;_____________________________________________
						pos_flag4.bit.JPOSn = 0;		// 清除上一次进入某个点位控制的标志
						if(++JPOSn >= ((int16)Pn[ProgEndPointADD] + 1))
						{
							if(Pn[JDPOS_PARSADD] & JDPOS_OneOrLoopSel)
							{// JnStop
								JPOSn = (int16)Pn[ProgEndPointADD];		// 防止溢出
								pos_flag4.bit.JPOSn = 1; //	为避免多次执行最后一步，置进入该点位控制的标志
							}
							else
							{
								JPOSn = (int16)Pn[ProgStartPointADD];
							}
						}
					}
				}
			}
			else
			{
				Timers.STEPTME = 0;
			}
		}		
	}
	// decisionJn
//;_______________________________________________
//;               2005/03/02修改（添加）
//;	当延时使用换步时等待到了首步程序时再启动
//;_______________________________________________
	if(state_flag2.bit.Son)
	{// SONrun
		if(pos_flag4.bit.JDPosNeedStart)
		{
			if(int_flag3.bit.JDPosStart == 0)
			{
				int_flagx.bit.pos_jogen = 1;
				inPgcnt = 0;
				JPOSn = ((int16)Pn[ProgStartPointADD]-1);
				// WaitingChgStp
				InitJPOSnEND = 1;
				pos_flag4.bit.JPOSn = 0;		// 清除上一次进入某个点位控制的标志
				Pgerr  = 0;
				Pgerro = 0;
				Pos_forward_filter.remain = 0;
				Pos_forward_filter.OutData = 0;
				Pos_forward_filter.OutValue = 0;
			}
			else
			{
				int_flagx.bit.pos_jogen = 0;
			}
		}
		if((JPOSn < (int16)Pn[ProgStartPointADD]) && (InitJPOSnEND == 0))
		{// WaitingChgStp
			InitJPOSnEND = 1;
			pos_flag4.bit.JPOSn = 0;		// 清除上一次进入某个点位控制的标志
			Pgerr  = 0;
			Pgerro = 0;
			Pos_forward_filter.remain = 0;
			Pos_forward_filter.OutData = 0;
			Pos_forward_filter.OutValue = 0;
			//runposition();
		}
		else if(int_flagx.bit.pos_jogen && ((input_state.bit.POS_JOGp == 0) || (input_state.bit.POS_JOGn == 0) || sci_oper_flag.bit.sci_pos_jogp || sci_oper_flag.bit.sci_pos_jogn))
		{
			if(((input_state.bit.POS_JOGp == 0) || sci_oper_flag.bit.sci_pos_jogp) && ((input_state.bit.POS_JOGn == 0) || sci_oper_flag.bit.sci_pos_jogn))
			{
				seek_step(0);
			}
			else
			{
				seek_step(Pn[JOGSPDADD]);
				if((input_state.bit.POS_JOGp == 0) || (sci_oper_flag.bit.sci_pos_jogp))
				{
					inPgstep = inPgstep;
				}
				else
				{
					inPgstep = -inPgstep;
				}				
			}
			Pgerr = inPgstep;
			Filtime_pos = Pn[JPOG0AccIncTimeADD];// 该位置行程的一次滤波时间常数
		}
		else if(JPOSn >= (int16)Pn[ProgStartPointADD])
		{
//;________________________________________________
//;
//;		计算点位控制的位置、速度、滤波
//;________________________________________________ 
			if(pos_flag4.bit.JPOSn == 0)
			{
				Tmp = (int32)((int16)Pn[JPOG0froateADD + (JPOSn<<1)]);
				Tmp = Tmp*10000 + (int32)((int16)Pn[JPOG0froateADD + (JPOSn<<1) + 1]);
				inPgcnt = Tmp;
				pos_flag4.bit.JPOSn = 1;
				pos_flag4.bit.HaveAbsoluPos = 0;	// 清除已进行绝对值换算标志
			}
			// noloadPOS
			if(pos_flag4.bit.JDPosNeedStart && (int_flag3.bit.JDPosStart == 0))
			{// stop
				pos_flag4.bit.JPOSn = 0;		// 清除上一次进入某个点位控制的标志
				inPgcnt = 0;
				seek_step(0);
			}
			else if((input_state.bit.POS_HALT == 0) || (sci_oper_flag.bit.sci_pos_halt))
			{// halt
				seek_step(0);
			}
			else
			{
				seek_step(Pn[JPOG0SpeedADD + JPOSn]);
			}			
			Filtime_pos = Pn[JPOG0AccIncTimeADD + JPOSn];// 该位置行程的一次滤波时间常数
			// dowithJPOS 计算具体脉冲个数
			if((pos_flag4.bit.HaveAbsoluPos == 0) && (Pn[ProgSelADD] & 0x0001))
			{// AbsoluPos
				inPgcnt -= (Pfcount + (int32)Pfroate*10000); 
				pos_flag4.bit.HaveAbsoluPos = 1;
			}
			// RelativePosi
			if(inPgcnt < 0)
			{
				inPgstep = - inPgstep;
			}
			if((memhex01.hex.hex02 == 2) 
			&& (input_state.bit.PCON) 
			&& (input_state.bit.CLR == 0) 
			&& (int_flag3.bit.haveCLRZero == 0))
			{
				inPgcnt = 0;
			}
			// noCloseinPuls
			if(labs(inPgcnt) < labs(inPgstep))
			{// noenough
				Pgerr = inPgcnt;
				inPgcnt = 0;
				remadd = 0;
			}
			else
			{
				inPgcnt -= inPgstep;
				Pgerr = inPgstep;
			}
			//runposition();
		}
	}
	else
	{
		inPgcnt = 0;
		if(JPOSn >= (int16)Pn[ProgStartPointADD])
		{
			JPOSn = ((int16)Pn[ProgStartPointADD]-1);
			InitJPOSnEND = 0;
		}
		// WaitingChgStp
		pos_flag4.bit.JPOSn = 0;		// 清除上一次进入某个点位控制的标志
		Pgerr  = 0;
		Pgerro = 0;
		Pos_forward_filter.remain = 0;
		Pos_forward_filter.OutData = 0;
		Pos_forward_filter.OutValue = 0;
		//runposition();
	}
	input_laststate.all = input_state.all;
	runposition();
}

void seek_step(int16 JPOSnSpeed)
{int32 Tmp,Tmp1;
Uint16 xRM;
//long long TMP;
/*
	将速度单位r/min转换成p/0.1ms   ---------->0.1ms位置环周期
	增量编码器
	=10000p/60 s
	=10000p/(60*1000) ms
	=10000p/(60*1000*10) 0.1ms
	=1p/60 0.1ms
	=1p/60*1024 0.1ms(Q10)
	=1p/61440 0.1ms(Q10)
	绝对值编码器
	=131072p/60 s
	=131072p/(60*1000) ms
	=131072p/(60*1000*10) 0.1ms
	=1p/4.57763671875 0.1ms
	=1p/4.57763671875*4096 0.1ms(Q12)
	=1p/18750 0.1ms(Q12)
	旋转编码器
	=65536p/60 s
	=65536p/(60*1000) ms
	=65536p/(60*1000*10) 0.1ms
	=1p/9.1552734375 0.1ms
	=1p/9.1552734375*4096 0.1ms(Q12)
	=1p/37500 0.1ms(Q12)
*/	
	Tmp = (int32)Bgear;
	Tmp *= JPOSnSpeed;
	Tmp1 = Tmp / Agear;		// 电子齿轮后速度
	xRM = Tmp % Agear;
	rmAgear += xRM;
	if(rmAgear >= Agear)
	{
		Tmp1++;
		rmAgear -= Agear;
	}
	Tmp1 = Tmp1 * memEncNum;
	Tmp = Tmp1 / 150000;
	if(Tmp < 30000)
	{
		inPgstep = (int16)Tmp;
		remadd += (Uint32)(Tmp1 % 150000);
		if(remadd >= 150000)
		{
			remadd -= 150000;
			inPgstep ++;
		} 
	}
	else
	{
		inPgstep = 30000;
	}
}
void runposition(void)
{int32 Tmp,quo,res;
//step1: 反馈脉冲
	//  反馈脉冲换算
	Tmp = labs(Pferr);
	Tmp *= Bgear;    //Bgear 分母
	quo = Tmp/Agear;	// 商   Agear电子齿轮分子
	res = Tmp%Agear;	// 余数         反馈的脉冲要除以电子齿轮
	if(quo > 32767)
	{
		pos_flag4.bit.GearErr = 1;
	}
	else
	{		
		// 反馈脉冲累加 Pfcount计算
		if(membit01.bit.bit00)
		{
			if(Pferr < 0)
			{
				Pfcount += quo;
				RmFg += res;
			}
			else
			{
				Pfcount -= quo;
				RmFg -= res;
			}
		}
		else
		{
			if(Pferr > 0)
			{
				Pfcount += quo;
				RmFg += res;
			}
			else
			{
				Pfcount -= quo;
				RmFg -= res;
			}
		}
		// 余数处理
		if(labs(RmFg) >= Agear)
		{
			if(RmFg > 0)
			{
				Pfcount ++;
				RmFg -= (int32)Agear;
			}
			else
			{
				Pfcount --;
				RmFg += (int32)Agear;
			}
		}
		// Pfroate计算
		if(abs(Pfcount) >= 10000)
		{
			if(Pfcount < 0)
			{
				if(abs(Pfroate) < 9999)
				{
					Pfroate--;
				}
				Pfcount += 10000;
			}
			else
			{// Pdir
				if(Pfroate < 9999)
				{
					Pfroate++;
				}
				Pfcount -= 10000;
			}
		}
		if(memBusSel == 3)		//2012-03-15
		{
			UnCurPos = co_abs_position;
		}
		else
		{
			UnCurPos = (int32)Pfroate*10000 + Pfcount;
		}

// step2: 给定脉冲位置/脉冲给定速度显示
		// EndPfroate
		Pgplus += Pgerr;
		if(Pgplus > 99999999)
		{
			UnPg = 99999999;
		}
		else if(Pgplus < -99999999)
		{
			UnPg = -99999999;
		}
		else
		{
			UnPg = Pgplus;
		}
		UnPg_roate = (int16)(UnPg/10000);
		UnPg_count = (int16)(UnPg%10000);

		// EndPgPulsDo 给定脉冲速度计算
		SPgerr += Pgerr;
		if((++PgSpdcnt) >= 10)
		{
			PgSpdcnt = 0;
			PgSpd = SPgerr;	// 脉冲给定速度	显示
			SPgerr = 0;
		}
// step3: SON/SOFF处理(偏差计数器清零)
		if(((!state_flag2.bit.Son) && (memhex00.hex.hex01 == 2))
		 ||((!state_flag2.bit.Son) && (memhex00.hex.hex01 == 0) && (Timers.cputon < Reycle_delay))	//RYCTL继电器吸合延时
		 ||((!state_flag2.bit.Son) && (memhex00.hex.hex01 == 0) && (int_flag3.bit.adjCurZero == 0))	//上主电电流调零
		 ||((!state_flag2.bit.Son) && (memhex00.hex.hex01 == 0) && (state_flag2.bit.SvReady == 0))// ALM
		 ||((!state_flag2.bit.Son) && (memhex00.hex.hex01 == 0) && state_flag2.bit.SvReady && (membit00.bit.bit00 == 0) && input_state.bit.SON)// IO SOFF
		  )
		{// ClrBiascnt
			Ek = 0;
			RmEk = 0;
			Pgerro = 0;
			RmPuls = 0;
			One_pulse_filter.OutData = 0;
			One_pulse_filter.OutValue = 0;
			One_pulse_filter.remain = 0;
			Two_pulse_filter.OutData = 0;
			Two_pulse_filter.OutValue = 0;
			Two_pulse_filter.remain = 0;
			notclrEk();    //是否到位等判断
		}
		else
		{
			if(state_flag2.bit.Son == 0)
			{
				x_speed = 0;
				SPEED = 0;
			}
// step4: Filter 位置平滑
			// 输入脉冲当量换算
			if(membit01.bit.bit00)
			{
				Pgerr = - Pgerr;
			}
			Tmp = Pgerr;
			Tmp *= Agear;
			quo = Tmp/Bgear;	// 商
			res = Tmp%Bgear;	// 余数
			if(labs(quo) > 32767)
			{
				pos_flag4.bit.GearErr = 1;
				// exit3
			}
			else
			{
				Pgerr = quo;
				// nextzheng 位置瞬?
				if((One_pulse_filter.remain == 0) && (Two_pulse_filter.remain == 0))
				{
					PosFiltime = Filtime_pos;
				}				
				One_pulse_filter.InValue = Pgerr;
				One_pulse_filter.Tfilt = PosFiltime;
				One_pulse_filter.Tperiod = Tspeed;
				if(memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit)
				{
					One_pulse_filter.InValue *= 12*SpeedUnit;
				}
				pulsefilter( & One_pulse_filter);
				Two_pulse_filter.InValue = One_pulse_filter.OutData;
				Two_pulse_filter.Tfilt = PosFiltime;
				Two_pulse_filter.Tperiod = Tspeed;
				pulsefilter( & Two_pulse_filter);
				
				if(memPosFiltSel & 0x0001)
				{
					Pspeed = Two_pulse_filter.OutData;
				}
				else
				{
					Pspeed = One_pulse_filter.OutData;
				}

				if(memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit)
				{
					// Spd_Pos();
					PulsOut = Pspeed/(12*SpeedUnit);
					RmPuls += Pspeed%(12*SpeedUnit);
					if(RmPuls >= 12*SpeedUnit)
					{
						PulsOut ++;
						RmPuls -= 12*SpeedUnit;
					}
					else if(RmPuls <= -12*SpeedUnit)
					{
						PulsOut --;
						RmPuls += 12*SpeedUnit;
					}

					Pgerro = PulsOut;
				}
				else
				{
					Pgerro = Pspeed;
				}
				// cacuEk
				Ek += Pgerro;	// 偏差计数加上给定脉冲当量
				RmEk += res;				
				if(RmEk >= (int32)Bgear)
				{
					RmEk -= (int32)Bgear;
					Ek++;
				}
				else if(RmEk <= - (int32)Bgear)
				{
					RmEk += (int32)Bgear;
					Ek--;
				}
				Ek -= Pferr;
// step4: Filter 位置前馈
				Tmp = Pn[PosFFFiltADD];
				Tmp *= Pgerro;
				/*if(memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit)
				{
					Tmp = 60 * SpeedUnit * Tmp;
					Pos_forward_filter.InValue = Tmp/100;
				}
				else
				{
					Pos_forward_filter.InValue = (long long)Tmp * 375 * SpeedUnit >> 13;			
				}*/
				Pos_forward_filter.InValue = (long long)Tmp * 1500 * SpeedUnit / memEncNum;
				Pos_forward_filter.Tfilt = Pn[FFFiltADD];
				Pos_forward_filter.Tperiod = Tspeed;
				pulsefilter( & Pos_forward_filter);
				if(memhex01.hex.hex02 == 2)
				{
					if(input_state.bit.PCON == 0)
					{
						int_flag3.bit.haveCLRZero = 1;	
					}
					// decsintflag
					if((int_flag3.bit.haveCLRZero == 0)	&& (input_state.bit.CLR == 0))					
					{// DecsiClr
						Ek = 0; 		// 偏差计数器清零
						RmEk = 0;		// 当量计算的余数累加器
						Pgerro = 0;
						RmPuls = 0;
						One_pulse_filter.OutData = 0;
						One_pulse_filter.OutValue = 0;
						One_pulse_filter.remain = 0;
						Two_pulse_filter.OutData = 0;
						Two_pulse_filter.OutValue = 0;
						Two_pulse_filter.remain = 0;
					}
				}
				else if(input_state.bit.CLR == 0)
				{// DecsiClr
					Ek = 0; 		// 偏差计数器清零
					RmEk = 0;		// 当量计算的余数累加器
					Pgerro = 0;
					RmPuls = 0;
					One_pulse_filter.OutData = 0;
					One_pulse_filter.OutValue = 0;
					One_pulse_filter.remain = 0;
					Two_pulse_filter.OutData = 0;
					Two_pulse_filter.OutValue = 0;
					Two_pulse_filter.remain = 0;
				}				
			}
		}
		notclrEk();
	}
}

void notclrEk(void)
{int32 Tmp,Tmp1;
 long long TMP;
	if(memhex01.hex.hex02 == 1)
	{
		Tmp = Pn[PosErrCntOVADD];
		Tmp <<= 8;			// Pn36 256 Uint
		TMP = Tmp;
		TMP *= Agear;		
		Tmp = TMP/Bgear;
		if(labs(Ek) < Tmp)
		{
			pos_flag4.bit.PosErrOValm = 0;
		}
		else
		{
			pos_flag4.bit.PosErrOValm = 1;
		}		
	}
	else
	{
		pos_flag4.bit.PosErrOValm = 0;
	}
// 到位判断
	if(Pgerr)
	{
		Timers.PosCOINtimer = 0;
	}
	else
	{
		if(Timers.PosCOINtimer < 60000)
		{
			Timers.PosCOINtimer++;
		}
	}
	Tmp = Pn[PosErrADD];
	Tmp *= Agear;
	Tmp /= Bgear;
	if(labs(Ek) > Tmp)
	{// notcoin
		pos_flag4.bit.VCMP_COIN = 0;
	}
	else
	{
		if(Timers.PosCOINtimer >= Pn[PosCoinTimeADD])
		{
			pos_flag4.bit.VCMP_COIN = 1;
		}
		else
		{
			pos_flag4.bit.VCMP_COIN = 0;
		}
	}
// 位置环调节
	Tmp1 = labs(Ek);
	if(state_flag2.bit.Son)
	{
		if(memGainSet)
		{
			Kpos = RigidityGain[Pn[Rigidity]]*10;
		}

		Tmp1 = Tmp1>>16;
		Tmp1 = Tmp1 * Kpos;
		// 判断偏差计数器是否超过16位
		if(Tmp1  > 0x4000)
		{
			// limit_speed
			if(Ek < 0)
			{
				SPEED = - ((int32)SPEEDlmt);
			}
			else
			{
				SPEED = (int32)SPEEDlmt;
			}
		}
		else
		{
			/*if(memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit)
			{
				Tmp1 = 160;
				Tmp = (Ek*Kpos) / Tmp1;		// 转化为0.1Hz
			}
			else
			{
				Tmp1 = 2180;
				Tmp = (Ek*Kpos) / Tmp1;		// 转化为0.1Hz
			}*/
			Tmp1 = memEncNum / 15;
			Tmp = Ek * Kpos / Tmp1;
			// 小增益无误差处理
			RmPosiout = RmPosiout + (Ek * Kpos) % Tmp1;
			if (RmPosiout/Tmp1)
			{
				if (RmPosiout > 0)
				{
					Tmp++;
					RmPosiout -= Tmp1;
				}
				else
				{
					Tmp--;
					RmPosiout += Tmp1;
				}
			}
			Tmp += Pos_forward_filter.OutData;					// Pq + Ek(0,1) * Kpos
			if(pos_flag4.bit.VCMP_COIN == 0)
			{
				if(Ek < 0)
				{
					Tmp -= ((int16)Pn[VbiaslvADD]*SpeedUnit);
				}
				else
				{
					Tmp += ((int16)Pn[VbiaslvADD]*SpeedUnit);
				}
			}
			// NotBias
/*
			if (memhex02.hex.hex02 == 2) //消抖控制位置环补偿
			{
				TMP = (long long)SpeedL[0]*Kpos + SpeedLRm[3];
				SpeedL[3] += TMP/100000;
				SpeedLRm[3] = TMP%100000;
				Tmp -= SpeedL[3];
			}
*/
			if(labs(Tmp) >= SPEEDlmt)
			{
				if(Tmp < 0)
				{
					SPEED = - ((int32)SPEEDlmt);
				}
				else
				{
					SPEED = ((int32)SPEEDlmt);
				}
			}
			else
			{
				SPEED = Tmp;
			}
		}
		// Posiout
		if(memBusSel == 3)
		{
			if(SPEED)
			{
				if(membit01.bit.bit00)
				{
					CMDref = - SPEED;
				}
				else
				{
					CMDref = SPEED;
				}
			}
		}
		else
		{
			if(membit01.bit.bit00)
			{
				CMDref = - SPEED;
			}
			else
			{
				CMDref = SPEED;
			}
		}
		if(pos_flag4.bit.PNOTzcl)
		{
			NextAnaSC();
		}
		else
		{
			SModule3();
			SModule2();
			limitIqr();
			SModule1();
		}
	}
}
void NextAnaSC(void)		// 零夹紧控制
{Uint32 Tmp,Tmp1;
	if(pos_flag4.bit.PCONzcl || pos_flag4.bit.PNOTzcl)
	{//RdyZeroClose
		if(labs(SPEEDINSout) >= (5+Pn[ZCLSPDADD])*SpeedUnit)	// 五转的滞环
		{//noZeroClose
			int_flag3.bit.ZCL = 0;
			ZeroEk = 0;
			SPEED = SPEEDINSout;
		}
		else if((int_flag3.bit.ZCL == 0) && (labs(SPEEDINSout) > (Pn[ZCLSPDADD])*SpeedUnit))
		{//noZeroClose
			int_flag3.bit.ZCL = 0;
			ZeroEk = 0;
			SPEED = SPEEDINSout;
		}
		else
		{
			if((int_flag3.bit.ZCL == 0) && (labs(SPEEDINSout) <= (Pn[ZCLSPDADD])*SpeedUnit))
			{
				int_flag3.bit.ZCL = 1;
			}
			// ZeroClose 
			ZeroEk -= ZeroPf;
			/*if(memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit)
			{
				Tmp1 = 160;
			}
			else
			{
				Tmp1 = 2180;
			}*/
			Tmp1 = memEncNum / 15;
			Tmp = labs(ZeroEk)*Kpos/Tmp1;
			if((Tmp & 0xFFFF0000))
			{
				SPEED = SPEEDlmt;
				if(ZeroEk < 0)
				{
					SPEED = - SPEED;
				}
			}
			else
			{//In16bits
				if(Tmp > SPEEDlmt)
				{
					SPEED = SPEEDlmt;
					if(ZeroEk < 0)
					{
						SPEED = - SPEED;
					}
				}
				else
				{
					SPEED = Tmp;
					if(ZeroEk < 0)
					{
						SPEED = - SPEED;
					}
				}
			}
		}		
	}
	else
	{//noZeroClose
		int_flag3.bit.ZCL = 0;
		ZeroEk = 0;
		SPEED = SPEEDINSout;
	}
	SModule3();
	SModule2();
	limitIqr();
	SModule1();
}
//;=========================================
//;  		入口:Iqrbuf[0,1]
//;         出口:Iqrref
//;		    限幅处理(电流给定)
//;=========================================
void limitIqr(void)
{
	if(((membit01.bit.bit00 == 0) && (Iqrbuf > 0)) || ((membit01.bit.bit00 == 1)&&(Iqrbuf<0)))
	{// magzeroiqr
		if(labs(Iqrbuf) > Iqrmax)
		{//limit_max
			int_flag3.bit.limitIqr = 1;
			if(Iqrbuf > 0)
			{
				Iqrref = Iqrmax;
			}
			else
			{
				Iqrref = -Iqrmax;
			}
		}
		else
		{
			int_flag3.bit.limitIqr = 0;
			Iqrref = Iqrbuf;
		}
	}
	else
	{//
		if(labs(Iqrbuf) > Iqrmin)
		{//limit_min
			int_flag3.bit.limitIqr = 1;
			if(Iqrbuf > 0)
			{
				Iqrref = Iqrmin;
			}
			else
			{
				Iqrref = -Iqrmin;
			}
		}
		else
		{
			int_flag3.bit.limitIqr = 0;
			Iqrref = Iqrbuf;
		}
	}

	#if DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW
	if(labs(Iqrref)>IPMImax*400)		// 最大电流限幅
	{
		int_flag3.bit.limitIqr = 1;
		if(Iqrbuf > 0)
		{
			Iqrref = IPMImax*400;
		}
		else
		{
			Iqrref = -(int16)IPMImax*400;		// 100-->10mA,400-->2.5mA
		}
	}
	#elif DRV_TYPE == DRV_5KW
	if(labs(Iqrref)>IPMImax*200)		// 最大电流限幅
	{
		int_flag3.bit.limitIqr = 1;
		if(Iqrbuf > 0)
		{
			Iqrref = IPMImax*200;
		}
		else
		{
			Iqrref = -(int16)IPMImax*200;		// 200-->5mA,400-->2.5mA
		}
	}
	#elif DRV_TYPE == DRV_15KW
	if(labs(Iqrref)>IPMImax*100)		// 最大电流限幅
	{
		int_flag3.bit.limitIqr = 1;
		if(Iqrbuf > 0)
		{
			Iqrref = IPMImax*100;
		}
		else
		{
			Iqrref = -(int16)IPMImax*100;		// 100-->10mA,400-->2.5mA
		}
	}
	#endif
}

//;=======================================
//; SModule1 ---- 转矩滤波子程序
//; 变    量: temp3,x_tfilt,Ktfilt,filtbuf[2]
//; 用户参数: 转矩指令滤波时间?Pn018)
//; 入口参数: 电流参考值(Iqrref)
//; 出口参数: 电流缓冲(Iqr)
//;=======================================
void SModule1(void)
{ int16 Ts;
  int32 Tmp;
	if(Pn[TCRFiltTimeADD])
	{
		Ts = Tspeed;
		if(memGainSet)
		{
			Tiqr = Pn[TCRFiltTimeADD];
		}
		TCRfiltinput = Iqrref;
		Tmp = (int32)Tiqr*TCRfiltout + (int32)Ts*TCRfiltinput;
		Tmp <<= 12;			//Q4.12
		Tmp += TCR_x_tfilt;		//小数部分
		TCRfiltout = ((Tmp / (Tiqr + Ts))>>12);
		TCR_x_tfilt = (Tmp % (Tiqr + Ts));
		Iqr = TCRfiltout;
	}
	else
	{
		Iqr = Iqrref;
	}
}

//;=======================================
//; SModule2 ---- 速度PI调节,转矩限制子程序
//; (3535表示2.5A的峰值3535A).
//; 变    量: 	temp3,e_speed,x_speed,Kpspeed
//; 	    	Kispeed,Iqrmax,Iqrmin,Iqrbuf[2]
//; 用户参数: 	速度增益(Pn013),
//;	    		积分时间常数(Pn014)
//;           	正转转矩限制(Pn026)
//;           	反转转矩限制(Pn027)
//; 入口参数: 	速度给定(SPEED), 速(speed)
//; 出口参数: 	电流参考值(Iqrref)
//;=======================================
void SModule2(void)
{Uint16 Kisp1,Kisp2;
 int32 lx_speed;
 int32 spdtmp,Tmp;
 int32 spdout;
 int16 xKpspeed,xKispeed;
 int32 ACCFF;
 Uint16 Jtemp;
 int32 Tf,Damp;
 long long xTmp;

//=====================================
 int32	B1,B2,B3,C1,C2,C3;
 int32	tmp,tmp1;
 long long tmp2,tmp3;
//=====================================

	Tf = Iqn * Pn[FrictionPerADD] / 1000;
	Tmp = (speed << 12) / (1000 * SpeedUnit);
	Damp = Tmp * Iqn >> 12;
	Damp = Damp * Pn[SpdDamp] / 1000;
	if(memGainSet)
	{
		Jtemp = Jx;
	}
	else
	{
		Jtemp = (Uint32)JMotor * (Uint32)(100 + Pn[perJ]) / 100;
	}
	//ACCFF = (long long)Jtemp * Iqn * SPEEDACC1ms / ((Uint32)varFactualTn * 10);
	ACCFF = (long long)Jtemp * Iqm * SPEEDACC1ms / ((Uint32)varFactualTn * 10);
	ACCFF *= 43;
	ACCFF>>= 12;
	ACCFF *= Pn[SPEEDACCFF];
	if(memhex01.hex.hex00 >= 2)
	{
		ACCFF /= 10;
	}
	else
	{
		ACCFF /= 100;
	}
	Tcr_filter.InValue = ACCFF;
	Tcr_filter.Tfilt = Pn[TorFFTime];
	Tcr_filter.Tperiod = 1;
	pulsefilter( & Tcr_filter);
	outputACCFF = Tcr_filter.OutData;
	if(memGainSet)
	{
		Kpspeed = RigidityGain[Pn[Rigidity]] * (Pn[KV_KP] + 1);
		Kispeed = RigidityTi[Pn[Rigidity]];
		if(perJload > 2000)
		{
			Kispeed = Kispeed + ((Uint32)(perJload - 2000) * 4506>>12);		// 惯量 〉20;积分时间延长
		}
	}
	xKpspeed = Kpspeed;
	xKispeed = Kispeed;
	Kisp1 = Pn[Kisp1ADD];
	Kisp2 = Pn[Kisp2ADD];
//-----------------------------------------------------
//	增量17位编耄能够找到精确的位置
//-----------------------------------------------------
	if(memCurLoop00.hex.hex00 == Encoder_Inc17bit && alm_absenc.bit.FS)
	{
		if(labs(SPEED)>100*SpeedUnit)
		{
			SPEED = 100*SpeedUnit;
		}
	}
//-----------------------------------------------------


//=========================================================================================
//	入口参数：速度反馈（speed）、振动频率（Pn[Vib_Fr] 0.1hz）、
//			  模型阻尼系数（Pn[Damping] 0.001）、低频振动抑制开关（Pn[Model_En]）
//	输出：消抖控制的速度环补偿縎peedL[0]⑽恢没凡钩チ縎peedL[3](偃环的积分)
//
//	Y(s)为SpeedL[0]、X(s)为speed, SpeedL[0] 的推导公式为：
//
//  		Y(s)		  2*k*Wa*S^2 + Wa*S
//	H(s) = ------ = ---------------------------
//			X(s)	   S^2 + 2*k*Wa*S + Wa^2
//
//	令S = (2/T)*(Z-1)/(Z+1) 代入上式整理得:
//
//			Y(Z)	  C1*Z^2 + C2*Z +C3
//  H(Z) = ------ = ---------------------
//			X(Z)	 B1*Z^2 + B2*Z + B3
//
// t = 2*fs/Wa = 2*fs/(2*pi*Pn[Vib_Fr]*0.1) = 31831/Pn[Vib_Fr] (注意频率和阻尼的单位)
// t1 = (t^2)*2*k = (t^2)*2*Pn[Damping]/1000 = (t^2)*Pn[Damping]>>9 (阻尼项近似处理)
// t2 = t*2*k = t*2*Pn[Damping]/1000 = t*Pn[Damping]>>9
//
// where: C1 = t1 + t
//		  C2 = -2*t1
//		  C3 = t1 - t
//		  B1 = t^2 + t2 +1
//		  B2 = 2 - 2*(t^2)
//		  B3 = t^2 - t2 +1
//========================================================================================

	if (memhex02.hex.hex02)   //低速抖动补㑽
	{
		Speed[0] = speed;

		tmp = 31831/Pn[Vib_Fr];
		tmp1 = (long long)tmp*tmp*Pn[Damping]>>9; //*2/1000
		C1 = tmp1+tmp;
		C2 = -(tmp1<<1);
		C3 = tmp1-tmp;

		tmp1 = tmp*tmp;
		tmp = tmp*Pn[Damping]>>9; //*2/1000
		B1 = tmp1+tmp+1;
		B2 = 2-(tmp1<<1);
		B3 = tmp1-tmp+1;

		tmp3 = (long long)C1*Speed[0];
		tmp3 += (long long)C2*Speed[1];
		tmp3 += (long long)C3*Speed[2];
		tmp3 -= (long long)B2*SpeedL[1];
		tmp3 -= (long long)B3*SpeedL[2];
		tmp2 = (long long)B2*SpeedLRm[1] + (long long)B3*SpeedLRm[2];
		tmp3 -= tmp2/B1;
		SpeedL[0] = tmp3/B1;
		SpeedLRm[0] = tmp3%B1;

		Speed[2] = Speed[1];
		Speed[1] = Speed[0];
		SpeedL[2] = SpeedL[1];
		SpeedL[1] = SpeedL[0];
		SpeedLRm[2] = SpeedLRm[1];
		SpeedLRm[1] = SpeedLRm[0];
	}

//===============================================================
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	        ///////////////////////////////encoder位置补偿///////////////////////////////////////////////////////////////
//	   EncodrPidOut=PIDencodr(PositionCurrentError);


    //syncOut=AvrspdPid(speed3);

	if (memhex02.hex.hex02) //速度环补偿   //memhex02 低速抖动补偿？
	{
		e_speed = SPEED - speed - SpeedL[0];
	}

	else			 //不启用消抖补偿控制
	{


		//speed3=(Slavespeed+speed)>>1;  //与第三-轴速度速度差 ;相加/2  速度相等时第三轴速度为0  主速为正，从速为负

	           // e_speed = SPEED - speed-syncOut-EncodrPidOut;
	         //   e_speed = SPEED - speed;
	       //     e_speed = SPEED - speed-EncodrPidOut;//-syncOut;

        //if(xyz==0) PositionSpeed=0;   //MODBUS 1062
       // else PositionSpeed=PositionCurrentError;
	    PositionSpeed=PositionCurrentError;
        if(PositionSpeed>3000)PositionSpeed=3000;
        else if(PositionSpeed<-3000)PositionSpeed=-3000;

         //e_speed = SPEED - speed-PositionSpeed;//PositionCurrentError;//-syncOut;  int16 CommandSpeed;
        e_speed = SPEED - speed-PositionSpeed;
	}
//=================================================================

	spdtmp = labs(e_speed);
	if(spdtmp>30000)
	{
		spdtmp = 30000;
	}
	if(Kisp1)
	{
		xKpspeed += spdtmp/Kisp2;
	}
	//Tmp = Kispeed + (spdtmp/Kisp2)*(spdtmp/Kisp2);
	Tmp = spdtmp*spdtmp;
	Tmp /= ((Uint32)Kisp2*Kisp2);
	Tmp += xKispeed;
	if(Tmp & 0xFFFF0000)
	{//limitKispeed
		xKispeed = 30000;
	}
	else
	{
		Tmp *= Kisp1;
		if(Tmp > 30000)
		{
			xKispeed = 30000;
		}
		else
		{
			xKispeed = (int16)Tmp;
		}
	}
	//spdtmp <<= 12;		//Q12
	//Tmp = spdtmp/Kispeed;
	//if(e_speed < 0)
	//{
		//Tmp = - Tmp;
	//}
//-----------------------------------------
//		2009-4-30 积分特殊处理
//-----------------------------------------
	lx_speed = x_speed;
	lx_speed += (e_speed<<12)/xKispeed;
	Tmp = (lx_speed>>12) + e_speed;
	if(Tmp > 0)
	{
		if(e_speed < 0)
		{
			xKispeed = Kispeed;
		}
	}
	else
	{
		if(e_speed > 0)
		{
			xKispeed = Kispeed;
		}
	}

//-----------------------------------------
	Tmp = (e_speed<<12)/xKispeed;
	// 控制模式
	if(int_flag3.bit.SpdLoopPmode)	// 开关切换
	{
		Tmp = 0;
	}
	else
	{
		switch(memSelPPI)
		{
			case 0:
				if(labs(Iqr*100/Iqn) > Pn[IqrP_PIADD] && ColBlock != 2)
				{
					Tmp = 0;
				}
				break;
			case 1:
				if(labs(Ek) > Pn[EkP_PIADD] && (ColBlock == 0 || ColBlock == 4))
				{
					Tmp = 0;
				}
				break;
			case 2:
				if(labs(SPEEDACC) > Pn[SPDACCP_PIADD] && ColBlock != 2)
				{
					Tmp = 0;
				}
				break;
			case 3:
				if(labs(SPEED) > ((Uint32)Pn[SPEEDP_PIADD]*SpeedUnit) && ColBlock != 2)
				{
					Tmp = 0;
				}
				break;
			default:
				break;
		}
	}
	x_speed += Tmp;
	if(Jtemp > JMotor)
	{
		Tmp = (Uint32) xKpspeed * Jtemp / JMotor;
		if(Tmp > 32767)
		{
			xKpspeed = 32767;
		}
		else
		{
			xKpspeed = Tmp;
		}
	}

	// 积分器抗饱和 Iqmax(Iqmin)*3*1024*103
	Tmp = 4096;
	Tmp *= memHzConvs;
	Tmp = 1+ Tmp/xKpspeed;
	if((e_speed + (x_speed>>12))>=0)		// not same f240
	{//Pv_LimitTn
		if(membit01.bit.bit00)
		{
			int_flag3.bit.TCRlimitDir = 1;
		}
		else
		{
			int_flag3.bit.TCRlimitDir = 0;
		}
	}
	else
	{
		if(membit01.bit.bit00)
		{
			int_flag3.bit.TCRlimitDir = 0;
		}
		else
		{
			int_flag3.bit.TCRlimitDir = 1;
		}
	}
	if(int_flag3.bit.TCRlimitDir)
	{
		Tmp *= Iqrmin;
		if(labs(x_speed) > Tmp)
		{
			if(x_speed < 0)
			{
				x_speed = - Tmp;
			}
			else
			{
				x_speed = Tmp;
			}
		}
	}
	else
	{// UseIqrmax
		Tmp *= Iqrmax;
		if(labs(x_speed) > Tmp)
		{
			if(x_speed < 0)
			{
				x_speed = - Tmp;
			}
			else
			{
				x_speed = Tmp;
			}
		}
	}

	Tmp = (x_speed>>12) + e_speed;	
	xTmp = (long long)Tmp * xKpspeed / memHzConvs;
	xTmp += outputACCFF;
	if(labs(SPEED) > Pn[FrictionSpd] * SpeedUnit)
	{
		if(SPEED > 0)
		{
			xTmp += Tf;
		}
		else if(SPEED < 0)
		{
			xTmp -= Tf;
		}
	}
	xTmp += Damp;


	if (llabs(xTmp)>0x7FFFFFFF)
	{
	// 速度环的限幅输出
	// 判断是否超出32位
		if (Tmp>0) Iqrbuf = 0x3FFFFFFF;
		else Iqrbuf = -0x3FFFFFFF;
	}
	else 
	{
		spdout = xTmp;
		if(spdout>0x3FFFFFFF)
		{
			spdout = 0x3FFFFFFF;
		}
		else if(spdout<-0x3FFFFFFF)
		{
			spdout = -0x3FFFFFFF;
		}

	    //-------------同速补偿----------------------------------------------------

	    //第三轴速度两者之和 。同步时速度为0  ;相加/ 2   主速为正，从速为负 均取的后16位。负值需要转换吗

	////////////////////////////////////////////////////	syncOut=AvrspdPid(speed3);
	        ///////////////////////////////////////////
		//if(syncOut>100)syncOut=100;
		  //////////////////////////////////////////////      if(syncOut<-100)syncOut=-100;

	    Iqrbuf = spdout;//-syncOut; //   pos485下，同速误差较小。20左右。但encoder在不断变大
	//    Iqrbuf = spdout;
		/////////////////////////////////////

		// 转矩前馈
		if(pos_flag4.bit.TCRFFQvalid && (state_flag2.bit.nBRKzcl==0) && (state_flag2.bit.nBRKsoff==0))
		{			
			Iqrbuf += AnIqrg;
		}
	}
	if(membit03.bit.bit02)   //低速补偿
	{
		SPEED_PULS();     //位置补偿
		Iqrbuf += Iqx;
	}
	dowith_Notch();
}

// 位置补偿
void SPEED_PULS(void)
{int16 Pulstemp;
 int32 Tmp;
 int16 SpdMode;
	/*if(memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit)
	{
		SpdMode = 600;
	}
	else
	{
		SpdMode = 46;	// (45.7763671875)
	}*/
	SpdMode = 1500000 / memEncNum;
	Pulstemp = (int16)(labs(SPEED)/SpdMode);
	RmSPEED += (int16)(labs(SPEED)%SpdMode);
	if(SPEED < 0)
	{
		PulsErr -= Pulstemp;
		if(RmSPEED >= SpdMode)
		{
			PulsErr -= 1;
			RmSPEED -= SpdMode;	// 余数到矫正 12 * 5 : 1r/min = 每12个周期1个脉冲
		}
	}
	else
	{// PulsErrAdd
		PulsErr += Pulstemp;
		if(RmSPEED >= SpdMode)
		{
			PulsErr += 1;
			RmSPEED -= SpdMode;	// 余数到矫正 12 * 5 : 1r/min = 每12个周期1个脉冲
		}
	}
	
	// adjustPulsErr
	PulsErr -= Pferr;
	if(int_flag3.bit.SpdLoopPmode)   //什么
	{
		PulsErr = 0;
	}

	// normPiCol 高速时去除位置校正速度
	//Kerr = Pn[PosRepairSGainADD];
	/*if(labs(speed) > 100*SpeedUnit)
	{
		Tmp = labs(Iqx<<10);
		Tmp /= Pn[SpdloopGainADD];
		if(x_speed>=0)
		{//xspeedAdd
			x_speed += Tmp;
		}
		else
		{
			x_speed -= Tmp;
		}
		Iqrbuf += Iqx;
		PulsErr = 0;
	}*/
	// adjustPulsErr1
	Tmp = (int32)Pn[PosRepairSGainADD] * (int32)PulsErr; //  低速校正系数 Pn129
	Iqx = Tmp>>10;
}

//;=======================================
//; SModule3 ---- 速度PI调节,转矩限制子程序
//; (3535表示2.5A的峰值3535A).
//; 变    量: 	temp3,e_speed,x_speed,Kpspeed
//; 	    	Kispeed,Iqrmax,Iqrmin,Iqrbuf[2]
//; 用户参数: 	速度增益(Pn013),
//;	    		积分时间常数(Pn014)
//;           	正转转矩限制(Pn026)
//;           	反转转矩限制(Pn027)
//; 入口参数:
//; 出口参数: 	Iqrmax Iqrmin
//;=======================================
void SModule3(void)
{
	if((membit01.bit.bit02) && ((memhex01.hex.hex00 & 0x0001) == 0) && (ColBlock != 2))
	{
		Iqrmax = (labs(TCMD)*Iqn/100);
		Iqrmin = Iqrmax;
	}
	else
	{// nolmtAna
		if((memhex01.hex.hex01 == 3) || (memhex01.hex.hex01 == 4) || (memhex01.hex.hex01 == 5) || (memhex01.hex.hex01 == 6) || (memhex01.hex.hex01 == 12))
		{// no_outLmt 有外部接速度控制时外部限制无效
			Iqrmax = ((Uint32)Pn[P_TCR_inLmtADD]*Iqn/100);
			Iqrmin = ((Uint32)Pn[N_TCR_inLmtADD]*Iqn/100);
		}
		else 
		{
			if(input_state.bit.PCL)
			{//PInLmt
				Iqrmax = ((Uint32)Pn[P_TCR_inLmtADD]*Iqn/100);
			}
			else
			{
				Iqrmax = ((Uint32)Pn[P_TCR_outLmtADD]*Iqn/100);
			}
			if(input_state.bit.NCL)
			{//NInLmt
				Iqrmin = ((Uint32)Pn[N_TCR_inLmtADD]*Iqn/100);
			}
			else
			{
				Iqrmin = ((Uint32)Pn[N_TCR_outLmtADD]*Iqn/100);
			}
		}
	}
	// EndSM3
	if(state_flag2.bit.nBRKzcl || state_flag2.bit.nBRKsoff)
	{// FanTCR
		if(pos_flag4.bit.PNOTzcl == 0)
		{
			Iqrmax = ((Uint32)Pn[nBRKStopTCRADD]*Iqn/100);
			Iqrmin = Iqrmax;
		}
	}
}
//-------------------------------------------------------------
//		位置指令平滑虑波器
//	输入N个脉冲，经过时间T后 桓雎龀宀换岵
//  Tfilt:此值越小，响λ俣仍娇?First -(er:+,remain+);-(-,-)
//-------------------------------------------------------------
void pulsefilter(volatile PULSEFILTER * v)
{	int16 temp1,temp2;
	int32 temp;
	//int64 TMP;
	if(v->Tfilt==0)
	{
		v->OutData=v->InValue;//Q0
		v->OutValue=0;
		v->remain = 0;
	}
	else
	{
		//temp1 = v->Tperiod + v->Tfilt;
		temp2 = ((int32)v->Tfilt<<12)/(v->Tperiod + v->Tfilt);
		temp1 = 4096 - temp2;
		temp = temp1 * v->InValue + temp2 * v->OutValue + v->remain;
		if(temp < 0)
		{
			v->OutValue = temp>>12;
		}
		else
		{
			v->OutValue = -((-temp)>>12);
		}
		v->remain = temp - (v->OutValue<<12);
		v->OutData = v->OutValue;
		if(!v->OutData)
		{
			temp2 = v->remain/temp1;
			if(temp2)
			{
				if(temp2 < 0)
				{
					v->OutData = -1;
					v->remain += temp1;
				}
				else
				{
					v->OutData = +1;
					v->remain -= temp1;
				}
			}		
		}
	}
}

//f(t) = f(0) + 3[(t/t0)^2]*[f(t0) - f(0)] - 2[(t/t0)^3][f(t0) - f(0)]其中t0为用户设定的时间
int32 Scacu(int userTime,int line_style,int32 sfiltin)		// userTime用户设定时间，line_style曲线类型，sfiltin输入
{
int32 InOut_Delt;		// 输入输出的差值
int32 SBaseOut;			// 标准3次曲线输出
int32 halfcopy;			// 后半周期对前半周期的复制
Uint16 ytime;
int32 divtime1,divtime2,divtime3;					// divtime1 = t/t0; divtime2 = (t/t0)^2; divtime3 = (t/t0)^3
int32 ydivtime1,ydivtime2,ydivtime3;
	SFiltIn[1] = SFiltIn[0];
	SFiltIn[0] = sfiltin;
	if(labs(SFiltIn[0]-SFiltIn[1]) > 5*SpeedUnit || xtime >= userTime)		// S曲线初始化,SFiltIn[0]为当前输入值,SFiltIn[1]是上个周期输入值
	{
		xtime = 0;
		fun0 = SFiltOut;
	}
	InOut_Delt = SFiltIn[0] - fun0;
	divtime1 = (((int32)xtime<<12) / userTime);
	divtime2 = divtime1 * divtime1 >> 12;
	divtime3 = divtime2 * divtime1 >> 12;
	SBaseOut = InOut_Delt * (3 * divtime2 - 2 * divtime3) >> 12;
	if ((xtime<<1) <= userTime)
	{
		switch(line_style)
		{
			case 0:
				SFiltOut = SBaseOut;
				break;
			case 1:
				SFiltOut = SBaseOut * ((divtime2<<1) - divtime1 + 4096) >> 12;
				break;
			case 2:
				SFiltOut = SBaseOut * ((divtime2<<2) - (divtime1<<1) + 4096) >> 12;
				break;
			case 3:
				SFiltOut = SBaseOut * ((divtime2<<3) - (divtime1<<2) + 4096) >> 12;
				break;
		}
	}
	else
	{
		ytime = userTime - xtime;
		ydivtime1 = (((int32)ytime<<12) / userTime);
		ydivtime2 = ydivtime1 * ydivtime1 >> 12;
		ydivtime3 = ydivtime2 * ydivtime1 >> 12;
		halfcopy = InOut_Delt * (3 * ydivtime2 - 2 * ydivtime3) >> 12;
		switch(line_style)
		{
			case 0:
				SFiltOut = SBaseOut;
				break;
			case 1:
				SFiltOut = SBaseOut + (halfcopy * (ydivtime1 - (ydivtime2<<1))  >> 12);
				break;
			case 2:
				SFiltOut = SBaseOut + (halfcopy * ((ydivtime1<<1) - (ydivtime2<<2)) >> 12);
				break;
			case 3:
				SFiltOut = SBaseOut + (halfcopy * ((ydivtime1<<2) - (ydivtime2<<3)) >> 12);
				break;
		}
	}
	xtime++;
	SFiltOut += fun0;
	return(SFiltOut);
}
//;=======================================
//; SModule4 ---- 软件加减速子程序
//; 变    量: temp3[2],accinc,decinc
//; 用户参数: 软件起动加速时间(Pn019)
//; 	    软件起动减速时间(Pn020)
//; 入口参数: 速度指令(SPEEDINS)
//; 出口参数: 速度给定(SPEEDINSout)
//;=======================================
void SModule4(void)
{
Uint32 AccTime,DecTime;
	Sm4cnt++;
	AccTime = Pn[SoftAccTimeADD]<<1;
	DecTime = Pn[SoftDecTimeADD]<<1;
	if(!AccTime && !DecTime && !Pn[SRiseTimeADD] && !Pn[FiltTimeADD])
	{
		SPEEDINSout = SPEEDINS;
	}
	else if(Sm4cnt >= 5)
	{
		Sm4cnt = 0;
		if(memAccDecSel == 1)
		{// SoftTime1	S曲线
			STIME = (Pn[SRiseTimeADD]<<1);	// 转换为1ms
			if(STIME)
			{
				//SFiltIn = SPEEDINS;
				//Scacu();
				//SPEEDINSout = SFiltOut;
				SPEEDINSout = Scacu(STIME,Pn[SfigureSelADD],SPEEDINS);
			}	
			else
			{
				SPEEDINSout = SPEEDINS;
			}
		}
		else if(memAccDecSel == 0)
		{// SoftTime2	斜坡
			SoftTime2(AccTime,DecTime);
		}
		else
		{
			
			One_Jerk_filter.InValue = SPEEDINS;
			One_Jerk_filter.Tfilt = (Pn[FiltTimeADD]<<1);	// 转换为1ms;
			One_Jerk_filter.Tperiod = 1;
			pulsefilter( & One_Jerk_filter);
			if(memAccDecSel == 3)
			{// SoftTime3 二次滤波
				Two_Jerk_filter.InValue = One_Jerk_filter.OutData;
				Two_Jerk_filter.Tfilt = One_Jerk_filter.Tfilt;
				Two_Jerk_filter.Tperiod = 1;
				pulsefilter( & Two_Jerk_filter);
				SPEEDINSout = Two_Jerk_filter.OutData;
			}
			else
			{
				SPEEDINSout = One_Jerk_filter.OutData;
			}
		}
	}
}

void LmtSpd(void)
{
	if(labs(SPEEDINS) > SPEEDlmt)
	{		
		if(SPEEDINS > 0)
		{
			SPEEDINS = SPEEDlmt;
		}
		else
		{
			SPEEDINS = - (int32)SPEEDlmt;
		}
	}
}

//cacuload_temp = 出厂值,cacuload_temp1 = 最大值
void cacuload(int32 Iqrate,Uint16 curIPM)
{Uint32 Tmp;
	Tmp = (int32)curIPM*10000;
	Tmp <<= 8;						// unit A , Q12

	#if DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW
	Tmp /= (Iqrate/4);	// 2.5mA -> A	//分辨率
	#elif  DRV_TYPE == DRV_5KW
	Tmp /= (Iqrate/2);	// 5mA -> A	//分辨率
	#elif  DRV_TYPE == DRV_15KW
	Tmp /= Iqrate;	// 10mA -> A	//分辨率
	#endif

	Tmp >>= 8;					// 5792 -> 1.414*4096	Q0
	cacuload_temp = (Uint16)Tmp;
	cacuload_temp1 = cacuload_temp;

	if(cacuload_temp >= 300)
	{
		cacuload_temp = 300;
		cacuload_temp1= 300;
	}
	else if(cacuload_temp >= 250)
	{
		cacuload_temp = 250;
	}
	else if(cacuload_temp >= 200)
	{
		cacuload_temp = 200;
	}
	else
	{
		cacuload_temp = 150;
	}
}

void SET_PROout(int16 selCNx)
{
	if(selCNx == 0)
	{// CN1_11,12
		out_state.bit.COIN = 1;
		out_state.bit.COIN ^= ((Pn[OutputNegADD] & 0x0008)>>3);	//1CN_11,12
	}
	else if (selCNx == 1)
	{// CN1_5,6
		out_state.bit.TGON = 1;
		out_state.bit.TGON ^= (Pn[OutputNegADD] & 0x0001);	//1CN_5,6
	}
	else
	{// CN1_9,10
		out_state.bit.SRDY = 1;
		out_state.bit.SRDY ^= ((Pn[OutputNegADD] & 0x0004)>>2);	//1CN_9,10
	}
}

void CLR_PROout(int16 selCNx)
{
	if(selCNx == 0)
	{// CN1_11,12
		out_state.bit.COIN = 0;
		out_state.bit.COIN ^= ((Pn[OutputNegADD] & 0x0008)>>3);	//1CN_11,12
	}
	else if (selCNx == 1)
	{// CN1_5,6
		out_state.bit.TGON = 0;
		out_state.bit.TGON ^= (Pn[OutputNegADD] & 0x0001);	//1CN_5,6
	}
	else
	{// CN1_9,10
		out_state.bit.SRDY = 0;
		out_state.bit.SRDY ^= ((Pn[OutputNegADD] & 0x0004)>>2);	//1CN_9,10
	}
}

void SingleOut(int16 defCNout,int16 selCNx)
{
	if(defCNout == 0)
	{// COIN_VCMP0
		if(pos_flag4.bit.VCMP_COIN)
		{
			SET_PROout(selCNx);
		}
		else
		{
			CLR_PROout(selCNx);
		}
	}
	else if(defCNout == 1)
	{// TGON_OUT0
		if(state_flag2.bit.TGONvalid)
		{
			SET_PROout(selCNx);
		}
		else
		{
			CLR_PROout(selCNx);
		}
	}
	else if(defCNout == 2)
	{// SRDY_OUT0
		if(state_flag2.bit.SvReady)
		{
			SET_PROout(selCNx);
		}
		else
		{
			CLR_PROout(selCNx);
		}
	}
	else if(defCNout == 3)
	{// CLT_OUT0
		if(int_flag3.bit.limitIqr)
		{
			SET_PROout(selCNx);
		}
		else
		{
			CLR_PROout(selCNx);
		}
	}
	else if(defCNout == 4)
	{// BRK_OUT0
		if(state_flag2.bit.BRKon)
		{
			CLR_PROout(selCNx);
		}
		else
		{
			SET_PROout(selCNx);
		}
	}
	else if (defCNout == 5)
	{
		if(selCNx == 0)
		{
			ioPGC |= 0x0004;
			*PGC_IO = ioPGC;
		}
		else if(selCNx == 1)
		{
			ioPGC |= 0x0001;
			*PGC_IO = ioPGC;
		}
		else
		{
			ioPGC |= 0x0002;
			*PGC_IO = ioPGC;
		}
	}
	else if (defCNout == 6)
	{
		if (state_flag2.bit.Pot || state_flag2.bit.Not)
		{
			CLR_PROout(selCNx);		//常开
		}
		else
		{
			SET_PROout(selCNx);		//常闭
		}
	}
	else if (defCNout == 7)
	{
		if (state_flag2.bit.Son)
		{
			SET_PROout(selCNx);		//常闭
		}
		else
		{
			CLR_PROout(selCNx);		//常开
		}
	}
	else
	{
		if(home_flag4.bit.Homing_flag)
		{
			SET_PROout(selCNx);
		}
		else
		{
			CLR_PROout(selCNx);
		}
	}
}
void SoftTime2(Uint16 AccTime,Uint16 DecTime)
{// SoftTime2	斜坡
	if(AccTime != lastacctime)
	{
		resaccinc = 0;	// 新数值余数和清零
	}
	lastacctime = AccTime;
	if(AccTime)
	{// str_Acc
		accinc = (1000*SpeedUnit)/AccTime;		//计算每个周期的加速量
		resaccinc += ((1000*SpeedUnit)%AccTime);//每个周期余数之和
		if(resaccinc >= AccTime)
		{
			resaccinc -= AccTime;
			accinc ++;
		}
	}
	else
	{
		accinc = SPEEDlmt;
	}
	// Decdowith
	if(DecTime != lastdectime)
	{
		resdecinc = 0;
	}
	lastdectime = DecTime;
	if(DecTime)
	{// str_Dec
		decinc = (1000*SpeedUnit)/DecTime;		//计算每个周期的加速量
		resdecinc += ((1000*SpeedUnit)%DecTime);//每个周期余数之和
		if(resdecinc >= DecTime)
		{
			resdecinc -= DecTime;
			decinc ++;
		}
	}
	else
	{
		decinc = SPEEDlmt;
	}
	// nextstr_Dec
	if((SPEEDINS > 0 && SPEEDINSout < 0) || (SPEEDINS < 0 && SPEEDINSout > 0))
	{
		SPEEDINS = 0;
	}
	/*if((SPEEDINS * SPEEDINSout) < 0)// SPEEDINS与SPEEDINSout同方向吗?
	{
		SPEEDINS = 0;
	}*/
	if(SPEEDINS)
	{
		if(labs(SPEEDINS) > labs(SPEEDINSout))// if |SPEEDINS|>|SPEEDINSout| then 加速
		{
			// accelerate
			if((labs(SPEEDINSout) + accinc) < labs(SPEEDINS))	// if |SPEEDINSout|<|SPEEDINS| then 继续加速
			{// gotoacc
				if(SPEEDINS > 0)
				{
					SPEEDINSout = (labs(SPEEDINSout) + accinc);
				}
				else
				{
					SPEEDINSout = - (labs(SPEEDINSout) + accinc);
				}
			}
			else
			{//
				SPEEDINSout=SPEEDINS;
			}
		}
		else if(labs(SPEEDINS) < labs(SPEEDINSout))// elseif |SPEEDINS|<|SPEEDINSout| then 减速
		{
			//decelerate
			if((labs(SPEEDINSout) - decinc) > labs(SPEEDINS))	// if |SPEEDINSout|>|SPEEDINS| then 继续减速
			{// gotodec
				if(SPEEDINSout > 0)
				{
					SPEEDINSout = (labs(SPEEDINSout) - decinc);
				}
				else
				{
					SPEEDINSout = - (labs(SPEEDINSout) - decinc);
				}
			}
			else	// else SPEEDINSout=SPEEDINS
			{
				SPEEDINSout=SPEEDINS;
			}
		}
	}
	else
	{// gotoStop
		if(SPEEDINSout != 0)
		{
			//decelerate
			if((labs(SPEEDINSout) - decinc) > labs(SPEEDINS))	// if |SPEEDINSout|>|SPEEDINS| then 继续减速
			{// gotodec
				if(SPEEDINSout > 0)
				{
					SPEEDINSout = (labs(SPEEDINSout) - decinc);
				}
				else
				{
					SPEEDINSout = - (labs(SPEEDINSout) - decinc);
				}
			}
			else	// else SPEEDINSout=SPEEDINS
			{
				SPEEDINSout = SPEEDINS;
			}
		}
	}

}
void def_input(Uint16 x,Uint16 y,Uint16 z)
{
HEX_DISP hexPar1,hexPar2;
BIT_DISP incn1;
		incn1.all = x;
		hexPar1.all = y;
		hexPar2.all = z;
		switch(hexPar1.hex.hex00)
		{
			case 0:
				input_state.bit.SON = incn1.bit.bit00;
				break;
			case 1:
				input_state.bit.PCON = incn1.bit.bit00;
				break;
			case 2:
				input_state.bit.POT = incn1.bit.bit00;
				break;
			case 3:
				input_state.bit.NOT = incn1.bit.bit00;
				break;
			case 4:
				input_state.bit.AlMRST = incn1.bit.bit00;
				break;
			case 5:
				input_state.bit.CLR = incn1.bit.bit00;
				break;
			case 6:
				input_state.bit.PCL = incn1.bit.bit00;
				break;
			case 7:
				input_state.bit.NCL = incn1.bit.bit00;
				break;
			case 8:
				input_state.bit.G_SEL = incn1.bit.bit00;
				break;
			case 9:
				input_state.bit.POS_JOGp = incn1.bit.bit00;
				break;
			case 10:
				input_state.bit.POS_JOGn = incn1.bit.bit00;
				break;
			case 11:
				input_state.bit.POS_HALT = incn1.bit.bit00;
				break;
			case 12:
				input_state.bit.HmRef = incn1.bit.bit00;
				break;
			case 13:
				input_state.bit.SHOM = incn1.bit.bit00;
				break;
			case 14:
				input_state.bit.ORG = incn1.bit.bit00;
				break;
		}
		switch(hexPar1.hex.hex01)
		{
			case 0:
				input_state.bit.SON = incn1.bit.bit01;
				break;
			case 1:
				input_state.bit.PCON = incn1.bit.bit01;
				break;
			case 2:
				input_state.bit.POT = incn1.bit.bit01;
				break;
			case 3:
				input_state.bit.NOT = incn1.bit.bit01;
				break;
			case 4:
				input_state.bit.AlMRST = incn1.bit.bit01;
				break;
			case 5:
				input_state.bit.CLR = incn1.bit.bit01;
				break;
			case 6:
				input_state.bit.PCL = incn1.bit.bit01;
				break;
			case 7:
				input_state.bit.NCL = incn1.bit.bit01;
				break;
			case 8:
				input_state.bit.G_SEL = incn1.bit.bit01;
				break;
			case 9:
				input_state.bit.POS_JOGp = incn1.bit.bit01;
				break;
			case 10:
				input_state.bit.POS_JOGn = incn1.bit.bit01;
				break;
			case 11:
				input_state.bit.POS_HALT = incn1.bit.bit01;
				break;
			case 12:
				input_state.bit.HmRef = incn1.bit.bit01;
				break;
			case 13:
				input_state.bit.SHOM = incn1.bit.bit01;
				break;
			case 14:
				input_state.bit.ORG = incn1.bit.bit01;
				break;
		}
		switch(hexPar1.hex.hex02)
		{
			case 0:
				input_state.bit.SON = incn1.bit.bit02;
				break;
			case 1:
				input_state.bit.PCON = incn1.bit.bit02;
				break;
			case 2:
				input_state.bit.POT = incn1.bit.bit02;
				break;
			case 3:
				input_state.bit.NOT = incn1.bit.bit02;
				break;
			case 4:
				input_state.bit.AlMRST = incn1.bit.bit02;
				break;
			case 5:
				input_state.bit.CLR = incn1.bit.bit02;
				break;
			case 6:
				input_state.bit.PCL = incn1.bit.bit02;
				break;
			case 7:
				input_state.bit.NCL = incn1.bit.bit02;
				break;
			case 8:
				input_state.bit.G_SEL = incn1.bit.bit02;
				break;
			case 9:
				input_state.bit.POS_JOGp = incn1.bit.bit02;
				break;
			case 10:
				input_state.bit.POS_JOGn = incn1.bit.bit02;
				break;
			case 11:
				input_state.bit.POS_HALT = incn1.bit.bit02;
				break;
			case 12:
				input_state.bit.HmRef = incn1.bit.bit02;
				break;
			case 13:
				input_state.bit.SHOM = incn1.bit.bit02;
				break;
			case 14:
				input_state.bit.ORG = incn1.bit.bit02;
				break;
		}
		switch(hexPar1.hex.hex03)
		{
			case 0:
				input_state.bit.SON = incn1.bit.bit03;
				break;
			case 1:
				input_state.bit.PCON = incn1.bit.bit03;
				break;
			case 2:
				input_state.bit.POT = incn1.bit.bit03;
				break;
			case 3:
				input_state.bit.NOT = incn1.bit.bit03;
				break;
			case 4:
				input_state.bit.AlMRST = incn1.bit.bit03;
				break;
			case 5:
				input_state.bit.CLR = incn1.bit.bit03;
				break;
			case 6:
				input_state.bit.PCL = incn1.bit.bit03;
				break;
			case 7:
				input_state.bit.NCL = incn1.bit.bit03;
				break;
			case 8:
				input_state.bit.G_SEL = incn1.bit.bit03;
				break;
			case 9:
				input_state.bit.POS_JOGp = incn1.bit.bit03;
				break;
			case 10:
				input_state.bit.POS_JOGn = incn1.bit.bit03;
				break;
			case 11:
				input_state.bit.POS_HALT = incn1.bit.bit03;
				break;
			case 12:
				input_state.bit.HmRef = incn1.bit.bit03;
				break;
			case 13:
				input_state.bit.SHOM = incn1.bit.bit03;
				break;
			case 14:
				input_state.bit.ORG = incn1.bit.bit03;
				break;
		}
		switch(hexPar2.hex.hex00)
		{
			case 0:
				input_state.bit.SON = incn1.bit.bit04;
				break;
			case 1:
				input_state.bit.PCON = incn1.bit.bit04;
				break;
			case 2:
				input_state.bit.POT = incn1.bit.bit04;
				break;
			case 3:
				input_state.bit.NOT = incn1.bit.bit04;
				break;
			case 4:
				input_state.bit.AlMRST = incn1.bit.bit04;
				break;
			case 5:
				input_state.bit.CLR = incn1.bit.bit04;
				break;
			case 6:
				input_state.bit.PCL = incn1.bit.bit04;
				break;
			case 7:
				input_state.bit.NCL = incn1.bit.bit04;
				break;
			case 8:
				input_state.bit.G_SEL = incn1.bit.bit04;
				break;
			case 9:
				input_state.bit.POS_JOGp = incn1.bit.bit04;
				break;
			case 10:
				input_state.bit.POS_JOGn = incn1.bit.bit04;
				break;
			case 11:
				input_state.bit.POS_HALT = incn1.bit.bit04;
				break;
			case 12:
				input_state.bit.HmRef = incn1.bit.bit04;
				break;
			case 13:
				input_state.bit.SHOM = incn1.bit.bit04;
				break;
			case 14:
				input_state.bit.ORG = incn1.bit.bit04;
				break;
		}
		switch(hexPar2.hex.hex01)
		{
			case 0:
				input_state.bit.SON = incn1.bit.bit05;
				break;
			case 1:
				input_state.bit.PCON = incn1.bit.bit05;
				break;
			case 2:
				input_state.bit.POT = incn1.bit.bit05;
				break;
			case 3:
				input_state.bit.NOT = incn1.bit.bit05;
				break;
			case 4:
				input_state.bit.AlMRST = incn1.bit.bit05;
				break;
			case 5:
				input_state.bit.CLR = incn1.bit.bit05;
				break;
			case 6:
				input_state.bit.PCL = incn1.bit.bit05;
				break;
			case 7:
				input_state.bit.NCL = incn1.bit.bit05;
				break;
			case 8:
				input_state.bit.G_SEL = incn1.bit.bit05;
				break;
			case 9:
				input_state.bit.POS_JOGp = incn1.bit.bit05;
				break;
			case 10:
				input_state.bit.POS_JOGn = incn1.bit.bit05;
				break;
			case 11:
				input_state.bit.POS_HALT = incn1.bit.bit05;
				break;
			case 12:
				input_state.bit.HmRef = incn1.bit.bit05;
				break;
			case 13:
				input_state.bit.SHOM = incn1.bit.bit05;
				break;
			case 14:
				input_state.bit.ORG = incn1.bit.bit05;
				break;
		}
		switch(hexPar2.hex.hex02)
		{
			case 0:
				input_state.bit.SON = incn1.bit.bit06;
				break;
			case 1:
				input_state.bit.PCON = incn1.bit.bit06;
				break;
			case 2:
				input_state.bit.POT = incn1.bit.bit06;
				break;
			case 3:
				input_state.bit.NOT = incn1.bit.bit06;
				break;
			case 4:
				input_state.bit.AlMRST = incn1.bit.bit06;
				break;
			case 5:
				input_state.bit.CLR = incn1.bit.bit06;
				break;
			case 6:
				input_state.bit.PCL = incn1.bit.bit06;
				break;
			case 7:
				input_state.bit.NCL = incn1.bit.bit06;
				break;
			case 8:
				input_state.bit.G_SEL = incn1.bit.bit06;
				break;
			case 9:
				input_state.bit.POS_JOGp = incn1.bit.bit06;
				break;
			case 10:
				input_state.bit.POS_JOGn = incn1.bit.bit06;
				break;
			case 11:
				input_state.bit.POS_HALT = incn1.bit.bit06;
				break;
			case 12:
				input_state.bit.HmRef = incn1.bit.bit06;
				break;
			case 13:
				input_state.bit.SHOM = incn1.bit.bit06;
				break;
			case 14:
				input_state.bit.ORG = incn1.bit.bit06;
				break;
		}
		switch(hexPar2.hex.hex03)
		{
			case 0:
				input_state.bit.SON = incn1.bit.bit07;
				break;
			case 1:
				input_state.bit.PCON = incn1.bit.bit07;
				break;
			case 2:
				input_state.bit.POT = incn1.bit.bit07;
				break;
			case 3:
				input_state.bit.NOT = incn1.bit.bit07;
				break;
			case 4:
				input_state.bit.AlMRST = incn1.bit.bit07;
				break;
			case 5:
				input_state.bit.CLR = incn1.bit.bit07;
				break;
			case 6:
				input_state.bit.PCL = incn1.bit.bit07;
				break;
			case 7:
				input_state.bit.NCL = incn1.bit.bit07;
				break;
			case 8:
				input_state.bit.G_SEL = incn1.bit.bit07;
				break;
			case 9:
				input_state.bit.POS_JOGp = incn1.bit.bit07;
				break;
			case 10:
				input_state.bit.POS_JOGn = incn1.bit.bit07;
				break;
			case 11:
				input_state.bit.POS_HALT = incn1.bit.bit07;
				break;
			case 12:
				input_state.bit.HmRef = incn1.bit.bit07;
				break;
			case 13:
				input_state.bit.SHOM = incn1.bit.bit07;
				break;
			case 14:
				input_state.bit.ORG = incn1.bit.bit07;
				break;
		}
}
void AD2S1210_init(void)
{
	Uint16 i;
	for(i=0;i<10;i++){}
 	*WrAD2S1210 = 0x0003;	
//	
	// control REG
	for(i=0;i<10;i++){}		
	*AD2S1210_InOut = 0x92;		
	for(i=0;i<10;i++){}
	switch(memhex02.hex.hex01)
	{
		case 0:
			*AD2S1210_RES = 0x00;		// 10bit Disable Hysteresis
			*AD2S1210_InOut = 0x60;
			memEncNum = 256;
			break;
		case 1:
			*AD2S1210_RES = 0x01;		// 12bit Disable Hysteresis
			*AD2S1210_InOut = 0x6A;
			memEncNum = 1024;
			break;
		case 2:
			*AD2S1210_RES = 0x02;		// 14bit Disable Hysteresis
			*AD2S1210_InOut = 0x65;
			memEncNum = 4096;
			break;
		case 3:
			*AD2S1210_RES = 0x03;		// 16bit Disable Hysteresis
			*AD2S1210_InOut = 0x6F;
			memEncNum = 16384;
			break;
		default:
			break;
	}
	// WR Freq
	for(i=0;i<10;i++){}		
	*AD2S1210_InOut = 0x91;
	for(i=0;i<10;i++){}
	*AD2S1210_InOut = 0x28;		 //0x28=10k,0x14=5k

	if (membit02.bit.bit03)
	{
		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x88;		// LOT High Threshold
		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x10;

		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x89;		// LOT High Threshold
		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x7F;

		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x8A;		// LOT High Threshold
		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x7F;

		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x8B;		// LOT High Threshold
		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x7F;

		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x8C;		// LOT High Threshold
		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x10;

		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x8D;		// LOT High Threshold
		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x7F;

		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x8E;		// LOT High Threshold
		for(i=0;i<10;i++){}
		*AD2S1210_InOut = 0x70;
	}

	EncElect = 65536 / memPoleNum;
	for(i=0;i<10;i++){}		
	*AD2S1210_InOut = 0xFF;
}
void readRES_POS(void)
{
int i;
	for(i=0;i<10;i++){}
	*AD2S1210_SMP = 0;
	for(i=0;i<10;i++){}
	*AD2S1210_SMP = 1;
	for(i=0;i<10;i++){}
	*WrAD2S1210 = 0;
	for(i=0;i<10;i++){}
	asm("	SETC INTM");
	encoder = *AD2S1210_InOut;
	encoder = encoder % EncElect;
	asm("	CLRC INTM");
}
void readRES_FAULT(void)
{
int i;
	for(i=0;i<10;i++){}
	*AD2S1210_SMP = 0;
	for(i=0;i<10;i++){}
	*AD2S1210_SMP = 1;
	for(i=0;i<10;i++){}
	*WrAD2S1210 = 3;
	for(i=0;i<10;i++){}
	RES_FAULT = *AD2S1210_InOut & 0xFF;
}
