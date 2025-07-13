#include "VerInfo.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "EDBclm.h"			// define and function declare
#include "globalvar.h"
#include "SPI4.h"
extern int32 Pd,Pq,Pz;
extern int32 Spq,Spd,Pcount;

int32 temp_x;
int16 AsSpeed;
Uint32 MyMaxI=980;//30A
Uint32 y=0;
int16 kkk1=0;
int16 kkk2=0;
int16 Fkkk1=0;
int16 Fkkk2=0;
int16 watchUnspd;
extern char SlaveDataerror;
extern int SlaveIdr,SlaveIqr;

int32 Avr100spd;

extern unsigned char SendEnconder;
extern volatile int32 encoder;
extern int32 EncodrPidOut;
 extern int16 TogetherEncodr;
extern Uint16 Slaveencoder;
extern int16 PositionCurrentError;



int16 Idr3=0;
int16 Iqr3=0;

extern const Uint16 MotorPar1[][4][7];
extern const Uint16 MotorPar2[][4][16];
extern const Uint16 MotorPar3[][4][16];
extern const Uint16 MotorPar4[][4][14];
extern const int sintab[];

extern char Homing_Method;
extern char Operation_Mode_Display;
extern Uint32 Physical_Outputs;
extern volatile Uint32 PosSPEEDINS;		//Unit: p/s
extern volatile Uint32 PosSPEEDINSout;		//Unit: p/s

void abs_Encoder(void);
void overload(void);
extern	void Test_Program(void);
extern	void Control_Use(void);
extern  void findRef(Uint32 EncoderData);

#if RAM_RUN
#pragma CODE_SECTION(Control_PROG, "ramfuncs1");
#pragma CODE_SECTION(speed_loop_process, "ramfuncs1");
#pragma CODE_SECTION(runcur_process, "ramfuncs");
#pragma CODE_SECTION(abs_Encoder, "ramfuncs");
#pragma CODE_SECTION(SVPWM, "ramfuncs"); 
//#pragma CODE_SECTION(OrientFlux, "ramfuncs1");  
//#pragma CODE_SECTION(Asroate, "ramfuncs1");
#pragma CODE_SECTION(overload, "ramfuncs1");
#pragma CODE_SECTION(ENCODER_PGC_ISR, "ramfuncs2");
#endif

#define lmtVOLT		(((int32)PWM_PERIOD*3)/2)
#define lmtCurInt	((Uint32)PWM_PERIOD*12288)
#define	Charge_Times	50

void Control_PROG(void)
{
int16 ctemp;//virtualPuls;
int16 Teta2;
int32 TMP;
BIT_DISP led_disp0,led_disp1;

	int_flagx.bit.int_time1 = 1;		// ���жϱ�־
//---------------------------------------
	if(memBusSel == 3)
	{				
		CO_Timer01msIsr();		//100us�� PDO for test
		Can_Transmit_Data();	//Txdata	
	}

//---------------------------------------
	if (Timer0 < 40000)
	{	
		Timer0++; 
	}
	else 
	{
		Timer0 = 0;
	}

	if (Timers.SCIRxfreecnt < 60000)		// 6s
	{
		Timers.SCIRxfreecnt++;
	}

//________________________________________________________A,B PG��Ƶд����
	rst5v = (* PG_IN) & 0x8000;
	if(rst5v)
	{
		PgDelt = 0;
	}
	if(membit01.bit.bit00)
	{
		if(PgDelt<=0)
		{
			PgDelt = abs(PgDelt);
		}
		else
		{
			PgDelt |= 0x1000;
		}
		*REG_DIV = PgDelt;
	}
	else
	{
		if(PgDelt<0)
		{
			PgDelt = abs(PgDelt);
			PgDelt |= 0x1000;
		}
		*REG_DIV = PgDelt;
	}
	if(int_flagx.bit.wPC_POS)
	{
		*PC_REG = PC_POS;
		*PC_Enable = 1;
		*PG_NOP = 0;
		int_flagx.bit.wPC_POS = 0;
	}
//________________________________________________________

	Read_AD();
	speed_loop_process();
    DPRAM_Timers_process();
        
	TMP = Iu_Sample;	//TMP = Iu_Sample;    //Iu_Sample   �����ڴ˴�ȡ��
	if(TMP==0)//��������� -2048---+2048
	{
		Iu = -2048;
	}
	else
	{
		Iu = TMP - 2048;
	}

	TMP = Iv_Sample;
	if(TMP==0)
	{
		Iv = -2048;
	}
	else
	{
		Iv = TMP - 2048;
	}

	#if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
	if(!int_flag3.bit.adjCurZero)	//����
	{
		if (Timers.cputon < 100)
		{
			int_flag3.bit.PWM_Charge = 0;		// �������Ծٳ�磬DB�ƶ�״̬
			int_flag3.bit.Volt_Reached = 0;		// ���15V���ѳ�����־
			Timers.ChargeTimer = 0;
		}
		else
		{
			int_flag3.bit.PWM_Charge = 1;		// ����δ��ɣ���ҪPWM���
			if(int_flag3.bit.Volt_Reached)		// 15V������󣬿��Կ�ʼ����
			{
				if(adjCurCnt<=4095)
				{
					adjCurCnt++;
					if (adjCurCnt >= 2048)
					{
						Scur[0] += Iu;
						Scur[1] += Iv;
					}
				}
				else
				{
					int_flag3.bit.adjCurZero = 1;
					int_flag3.bit.PWM_Charge = 0;		// ��������󣬳���PWM���
					int_flag3.bit.Volt_Reached = 0;		// ���15V���ѳ�����־
					Timers.ChargeTimer = 0;
				}
			}
		}
	}
	#else
	if(!int_flag3.bit.adjCurZero && Timers.cputon >= 100)		//采集偏置?
	{
		if(adjCurCnt<=4095)
		{
			adjCurCnt++;
			if (adjCurCnt >= 2048)
			{
				Scur[0] += Iu;
				Scur[1] += Iv;
			}
		}
		else
		{
			int_flag3.bit.adjCurZero = 1;// ��0���
		}
	}
	#endif
	else if(int_flag3.bit.adjCurZero)  //计算采集的电流偏移值
	{
		TMP = Iu;                       //�˴�IU�Ѿ��˲����޷����Ѿ���ȥ2048 �� Ϊ��ֵΪ����������
		TMP = TMP - (Scur[0]>>11) - Iuoffset;   //(Scur[0]>>11Ϊ�Զ���0��ƫ�ã�IuoffsetΪ����ƫ��
		if(TMP > 2048)			//���������
		{
			Iu = 2048;
		}
		else if(TMP < -2048)
		{
			Iu = -2048;
		}
		else
		{
			Iu = TMP;
		}
//////////////////////////////////////////////////////////////////////////////
	      //if(Iu>0)
	      //    {
	      //        if(kkk1<Iu)kkk1=Iu;
	      //    }
	       //   else
	       //   {
	       //       if(Iu<Fkkk1)Fkkk1=Iu;
	       //   }
		     //       if((Iu>1310)||(Iu<-1310))   //过流 30A //过流 10A       40*24*2048/1500=650
		      if((Iu>1146)||(Iu<-1146))   //目前980(30A) 12K时一会后报A01
		            {
		              state_flag2.bit.HaveAlm = 1;
		               state_flag2.bit.enPWM=0;//关PWM
		               Disable_PWM();
		                alarmno = 01;

		            }
	///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////




		TMP = Iv;
		TMP = TMP - (Scur[1]>>11) - Ivoffset;
		if(TMP > 2048)			//���������
		{
			Iv = 2048;
		}
		else if(TMP < -2048)
		{
			Iv = -2048;
		}
		else
		{
			Iv = TMP;
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	 //if(Iv>0)
	                     //{
	                    //     if(kkk2<Iv)kkk2=Iv;
	                    // }
	                     //else
	                     //{
	                     //    if(Iv<Fkkk2)Fkkk2=Iv;
	                     //}
	        if((Iv>1146)||(Iv<-1146))   //过流 30A
	  //      if((Iv>1146)||(Iv<-1146))   //过流 35A
	        {
	            state_flag2.bit.HaveAlm = 1;
	            state_flag2.bit.enPWM=0;//关PWM
	            Disable_PWM();
	           alarmno = 01;
	        }


	///////////////////////////////////////////////////////////////////////////////



	Iuoffset = Pn[Fn006Addr];    //ƫ��
	Ivoffset = Pn[Fn006Addr+1];
	




	#if DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW
	Ia = -((int32)Iu * 2930)/(int16)memcurSamUnit;	// 2.5mA     56mV/A    24  12.5mv/A
	Ib = -((int32)Iv * 2930)/(int16)memcurSamUnit;
	#elif DRV_TYPE == DRV_5KW
	Ia = -((int32)Iu * 1469)/(int16)memcurSamUnit;	// 5mA
	Ib = -((int32)Iv * 1469)/(int16)memcurSamUnit;
	#elif DRV_TYPE == DRV_15KW
	Ia = -((int32)Iu * 730)/(int16)memcurSamUnit;	// 10mA
	Ib = -((int32)Iv * 730)/(int16)memcurSamUnit;
	#endif

//---------------------------------------------------------------------------------
// ����λ��
//---------------------------------------------------------------------------------
	if (memCurLoop00.hex.hex00 == Encoder_Abs17bit || memCurLoop00.hex.hex00 == Encoder_Inc17bit)
	{int32 vIncEnc;	
		vIncEnc = 0;
		abs_Encoder();
		if(memCurLoop00.hex.hex00 == Encoder_Abs17bit)
		{
			profibus_buf.pbus_PGout = (int32)Rotate << 15;
			profibus_buf.pbus_PGout = profibus_buf.pbus_PGout << 2;
			profibus_buf.pbus_PGout |= Position;
		}
		else
		{
			profibus_buf.pbus_PGout = Position;
		}				
		EncElect = 131072 / memPoleNum;
		vIncEnc = singlePos + ((speed * 134)>>12);
		if (vIncEnc >= 131072)
		{
			vIncEnc -= 131072;
		}
		else if(vIncEnc < 0)
		{
			vIncEnc = 0;
		}
		encoder = vIncEnc % EncElect;
		//      virtualPuls = (int16)(speed / 1200);
		adjEncoder = encoder - Tetacnt;
		if (adjEncoder < 0)
		{
			adjEncoder = adjEncoder + EncElect;
		}
		Teta = (adjEncoder * 360) / EncElect;
		Teta = Teta + 180;
		if (Teta > 360)
		{
			Teta = Teta - 360;
		}
	}
//_________________________________________________________________________________
	// 增量编码器
	else
	{		
	    PGCNT[0] = (int16)(QPCNT);
		encincr = (PGCNT[0] - PGCNT[1])>>2; //脉冲*4
		PGCNT[1] = PGCNT[0];
		TMP = memEncNum<<2; //编码器总线数 4096*4 16384
		profibus_buf.pbus_PGout += encincr;
		if (profibus_buf.pbus_PGout >= TMP)
		{
			profibus_buf.pbus_PGout -= TMP;
		}
		else if (profibus_buf.pbus_PGout < 0)
		{
			profibus_buf.pbus_PGout += TMP;
		}
		if (memCurLoop00.hex.hex00 == Encoder_Resolver)
		{
			switch(memhex02.hex.hex01)
			{
				case 0:
					encincr <<= 6;
					break;
				case 1:
					encincr <<= 4;
					break;
				case 2:
					encincr <<= 2;					
					break;
				default:
					break;
			}
			if(!RES_FAULT)
			{
				if(Timers.RESTimer1 < resolverNoALMdelay)
				{
					Timers.RESTimer1++;
				}
				else
				{
					Timers.RESTimer = 0;
				}
			}
			else
			{
				if(Timers.RESTimer < resolverALMdelay)
				{
					Timers.RESTimer++;
				}
				Timers.RESTimer1 = 0;
			}
		}
//		virtualPuls = (int16)(speed / 1200);
		encoder = encoder + encincr;
		if (encoder < 0)
		{
		  encoder = encoder + EncElect;   // EncElect// 每一电角度脉冲数量8192
		}
		else
		{
		  if (encoder >= EncElect)
		    encoder = encoder - EncElect;			// 保证计数范围
		}
		ctemp = encoder - Tetacnt;					//// 编码器零点校准Tetacnt初始相位
//		ctemp = ctemp + virtualPuls;
		if (ctemp < 0) 
		{
		  ctemp = ctemp + EncElect;
		}  
		ctemp = ctemp + (EncElect>>1);		// 电角度加180度        8192/2 =180度电角度
		if (ctemp > EncElect)
		{
		  ctemp = ctemp - EncElect;
		}
		Teta = (int16) (( ((int32)ctemp)*360 / EncElect));				// �Ƕ�ת��换成真正角度
	}



///////////////////////////////encoder////////////////////////////////////////////////////////////////////
  //  y++;
  //  McbspbRegs.DXR2.all= encoder;                    //发送赋值  发一次32位


     //-------------------------计算---------------------------------------------
#if 0
        speed3=(Avr100spd+Slavespeed)>>1;

                   if((speed3>500)|(speed3<-500)){
                       W5=Avr5spd;
                       Wspi=Slavespeed;
                   }
        syncOut= PIDsync(speed3);
#endif
   //     WatchLine=0;
   //           WatchLine=1;
////////////////////////////////////////////////////////////////////////////////////////////









/////////////////////////////////////////////////////////////////////////////////////////////////////
	Sin_Teta = sintab[Teta];
	Teta2 = Teta + 90;
	if (Teta2 > 360)
    {
	  Teta2 = Teta2 - 360;
	}
	Cos_Teta = sintab[Teta2];

	// judge son flag
	#if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
	if(state_flag2.bit.enPWM)
	{
		Enable_PWM();
	}
    else if(int_flag3.bit.PWM_Charge)		//��ҪPWM���
	{
        PWM_Charge();

        if(Timers.ChargeTimer++ < Charge_Times)
		{
		    int_flag3.bit.Volt_Reached = 0;
		}
		else
		{
		    int_flag3.bit.Volt_Reached = 1;
		}
	}
	else
	{
		Disable_PWM();
		int_flag3.bit.PWM_Charge = 0;		//SOFFʱ�����PWM����־
		int_flag3.bit.Volt_Reached = 0;		//SOFFʱ�����15V���ѳ�����־
		state_flag2.bit.IGBTOFF = 0;		//SOFFʱ����������ɺ�IGBT�ѹضϹ���־
		Timers.ChargeTimer = 0;
	}
	#else
	if(state_flag2.bit.enPWM)
	{
		Enable_PWM();
	}
	else
	{
		Disable_PWM();
	}
	#endif

	if(state_flag2.bit.Son)
	{
		if(!state_flag2.bit.HandPhase && !sci_oper_flag.bit.sciPhaseSON)
		{
			overload();
		}
		runcur_process();
	}
	else
	{
		if(abs(Encoder4time) < ((EncElect<<2)+1000))
		{
			Encoder4time += encincr;
		}
		x_id = 0;
		x_iq = 0;
		x_ia = 0;
		e_ia = 0;
		Iqr = 0;
		SIqr = 0;
		x_SQRTiq = 0;
		SGMAIqr = 0;
		avIqrCnt = 0;
		EPwm1Regs.CMPA.half.CMPA = 0;
		EPwm2Regs.CMPA.half.CMPA = 0;
		EPwm3Regs.CMPA.half.CMPA = 0;
	}		

	InputFilter();
	SaveCommData();

	if((memBusSel == 1) || (memBusSel == 2))
	{	 
		++Profitime;
		DPRAM_DirectDataExchange();
	}

//---------------------------------------------------------------------------------
// Display seven segment LED	
//---------------------------------------------------------------------------------
	if (LedIndex<5) LedIndex++;
	else LedIndex = 0;
	if(TestPara.Test_Mode)led_disp0.all = 0;
	else
	led_disp0.all = LedSegment[LedIndex];
	led_disp1.bit.bit15 = led_disp0.bit.bit00; //B
	led_disp1.bit.bit14 = led_disp0.bit.bit01; //A
	led_disp1.bit.bit13 = led_disp0.bit.bit02; //C
	led_disp1.bit.bit12 = led_disp0.bit.bit03; //F
	led_disp1.bit.bit11 = led_disp0.bit.bit04; //DP
	led_disp1.bit.bit10 = led_disp0.bit.bit05; //G
	led_disp1.bit.bit09 = led_disp0.bit.bit06; //D
	led_disp1.bit.bit08 = led_disp0.bit.bit07; //E
    led_disp1.all = (~led_disp1.all)&0xFF00;

	if (LedIndex == 0)	//LCS1
		led_disp1.bit.bit07 = 1;
	else if (LedIndex == 1)	//LCS2
		led_disp1.bit.bit06 = 1;
	else if (LedIndex == 2)	//LCS3
		led_disp1.bit.bit05 = 1;
	else if (LedIndex == 3)	//LCS4
		led_disp1.bit.bit04 = 1;
	else if (LedIndex == 4)	//LCS5
		led_disp1.bit.bit03 = 1;
	
	*LED = led_disp1.all;
	
	//*LED = (~led_disp1.all<<8)|(LedSelect<<(LedIndex+3));


//  	EvbRegs.EVBIFRA.all = BIT9;
//      Acknowledge interrupt to receive more interrupts from PIE group 4
//  	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

void Read_AD(void)
{
//	28234/28335Ƭ��AD��������
	while(AdcRegs.ADCST.bit.SEQ1_BSY)
	{};

    MedianFilter();

    Tref_Sample = ((Uint32)AdcRegs.ADCRESULT12 + (Uint32)AdcRegs.ADCRESULT14) >> 5;//ת��ָ��
	Vref_Sample = ((Uint32)AdcRegs.ADCRESULT13 + (Uint32)AdcRegs.ADCRESULT15) >> 5;//�ٶȵ�ѹָ��
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;	//Reset the sequence
	AdcRegs.ADCTRL2.bit.RST_SEQ2 = 1;

}

void MedianFilter(void)
{
int16 Ix[6], maxIx, minIx, i;

// Iu filter
	Ix[0] = AdcRegs.ADCRESULT0>>4;
	Ix[1] = AdcRegs.ADCRESULT2>>4;
	Ix[2] = AdcRegs.ADCRESULT4>>4;
	Ix[3] = AdcRegs.ADCRESULT6>>4;
	Ix[4] = AdcRegs.ADCRESULT8>>4;
	Ix[5] = AdcRegs.ADCRESULT10>>4;

	maxIx = minIx = Ix[0];
	for (i=1; i < 6; i++)
	{
		if (Ix[i] > maxIx)
	  	{
	  		maxIx = Ix[i];
	  	}

	  	if (Ix[i] < minIx)
	  	{
	  		minIx = Ix[i];
	  	}
	}

	Iu_Sample = ( (Uint32)Ix[0] + (Uint32)Ix[1] + (Uint32)Ix[2]
				 +(Uint32)Ix[3] + (Uint32)Ix[4] + (Uint32)Ix[5]
				 - maxIx - minIx) >> 2;

// Iv filter
	Ix[0] = AdcRegs.ADCRESULT1>>4;
	Ix[1] = AdcRegs.ADCRESULT3>>4;
	Ix[2] = AdcRegs.ADCRESULT5>>4;
	Ix[3] = AdcRegs.ADCRESULT7>>4;
	Ix[4] = AdcRegs.ADCRESULT9>>4;
	Ix[5] = AdcRegs.ADCRESULT11>>4;

	maxIx = minIx = Ix[0];
	for (i=1; i < 6; i++)
	{
	  	if (Ix[i] > maxIx)
	  	{
	  		maxIx = Ix[i];
	  	}

	  	if (Ix[i] < minIx)
	  	{
	  		minIx = Ix[i];
	  	}
	}

	Iv_Sample = ( (Uint32)Ix[0] + (Uint32)Ix[1] + (Uint32)Ix[2]
				 +(Uint32)Ix[3] + (Uint32)Ix[4] + (Uint32)Ix[5]
				 - maxIx - minIx) >> 2;

}

void Disable_PWM(void)
{
	EALLOW;
	EPwm1Regs.TZCTL.bit.TZB = TZ_HIZ;	//Force EPWM1B HIZ
	EPwm2Regs.TZCTL.bit.TZB = TZ_HIZ;	//Force EPWM2B HIZ
	EPwm3Regs.TZCTL.bit.TZB = TZ_HIZ;	//Force EPWM3B HIZ
	EPwm1Regs.TZFRC.bit.OST = 1;//Forces a one-shot trip event and sets the TZFLG[OST] bit.
	EPwm2Regs.TZFRC.bit.OST = 1;//Forces a one-shot trip event and sets the TZFLG[OST] bit.
	EPwm3Regs.TZFRC.bit.OST = 1;//Forces a one-shot trip event and sets the TZFLG[OST] bit.
	EDIS;
	state_flag2.bit.PWM_HIZ = 1;//PWM����̬����
}

void Enable_PWM(void)
{
	EALLOW;
	EPwm1Regs.TZCLR.bit.OST = 1;//Clear Flag for One-Shot Trip (OST) Latch
	EPwm2Regs.TZCLR.bit.OST = 1;
	EPwm3Regs.TZCLR.bit.OST = 1;
	EDIS;
	state_flag2.bit.PWM_HIZ = 0;//����PWM����̬����
}

void PWM_Charge(void)
{
	EALLOW;
	EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO;	//Force EPWM1B Low State
	EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO;	//Force EPWM2B Low State
	EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO;	//Force EPWM3B Low State
	EPwm1Regs.TZFRC.bit.OST = 1;		//SW Force
	EPwm2Regs.TZFRC.bit.OST = 1;
	EPwm3Regs.TZFRC.bit.OST = 1;
	EDIS;
}

interrupt void SPEED_Loop_ISR(void)
{

	Control_PROG();
	INTtimer++;
	if(INTtimer >= 5)
	{
		INTtimer = 0;
		CNT_TIMER();

		#if AC_VOLT == AC_400V && DRV_TYPE == DRV_15KW
     	ADC_HANDLE();		//ĸ�ߵ�ѹ������¶ȼ��
		#endif
	}

	EPwm1Regs.ETCLR.bit.INT = 1;//ʹ��ȫ���ж�
  	// To receive more interrupts from this PIE group, acknowledge this interrupt 
  	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;//�����ж�Ӧ���־(PIEACK)��Ӧλ���������´��ж�����
	EINT;//��ȫ���ж�

	//EvbRegs.EVBIFRB.bit.T4PINT = 1;
  	// Acknowledge interrupt to receive more interrupts from PIE group 4
  	//PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
}

void CNT_TIMER(void)
{int16 temp1,temp2;
		//Mstime_clr();
		//speed_loop_process();
		int_flagx.bit.int_time2 = 1;			//���жϱ�־
//////////////////////////////////////////////////////////////////////////////////////		
		if(Timers.lessEncoderTime < 1600)
		{
			Timers.lessEncoderTime++;
		}		

		if(Timers.Un100ms < 200)
		{
			Timers.Un100ms++;
		}
		if(Timers.cntsum)
		{
			Timers.cntsum ++;
		}
		// nostart
		if(alm_state.bit.PUMP)// 
		{
			if(out_state.bit.PWM7 && (Timers.cntsum==0))
			{
				Timers.cntsum = 1;
			}
			// nextstartcntsum
			if(Timers.Pumpcnt < 500)
			{
				Timers.Pumpcnt ++;
				Timers.SumPump ++;
			}		
		}
		else
		{// CLRPumP
			Timers.Pumpcnt = 0;
			pumpflag &= 0xFFFE;
		}
		// NextPump
		if(Timers.cntsum == 2000)
		{// pump1s
			if(Timers.SumPump >= 500)
			{
				pumpflag |= 0x0001;				
			}
			// endPump
			if((pumpflag & 0x0001))
			{
				Timers.SumPump = 0;
				Timers.cntsum = 0;
			}
		}
		else if(Timers.cntsum == 4000)
		{// pump2s
			if(Timers.SumPump >= 700)
			{
				pumpflag |= 0x0001;
			}
			// endPump	
			if((pumpflag & 0x0001))
			{
				Timers.SumPump = 0;
				Timers.cntsum = 0;
			}		
		}
		else if(Timers.cntsum == 6000)
		{// pump3s
			if(Timers.SumPump >= 900)
			{
				pumpflag |= 0x0001;				
			}
			// endPump
			if((pumpflag & 0x0001))
			{
				Timers.SumPump = 0;
				Timers.cntsum = 0;
			}
		}
		else if(Timers.cntsum >= 20000)
		{// pump10s
			if(Timers.SumPump >= 1000)
			{
				pumpflag |= 0x0001;
				// endPump
				Timers.SumPump = 0;
				Timers.cntsum = 0;
			}
			else
			{// clrcntpump
				Timers.SumPump = 0;
				Timers.cntsum = 0;
			}
		}
		else
		{// endPump
			if((pumpflag & 0x0001))
			{
				Timers.SumPump = 0;
				Timers.cntsum = 0;
			}
		}
//_______________________________________________________________

		if (key_flag1.bit.AnAuto && Timers.cntdone < 2000)
		{
			Timers.cntdone++;
			if (!sci_oper_flag.bit.SciAdSpdEnd)
			{
				Sspd += Ana1;
			}
			else
			{
				Sspd += (Ana1 - SpdZero);
			}

			if (!sci_oper_flag.bit.SciAdTcrEnd)
			{
				Stcr += Ana2;
			}
			else
			{
				Stcr += (Ana2 - TcrZero);
			}		
		}
		else if(sci_oper_flag.bit.sciStartZeroSpd || sci_oper_flag.bit.sciStartZeroTcr)
		{
			if(Timers.cntdone >= 2000)
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
				Timers.cntdone = 0;
				Sspd = 0;
				Stcr = 0;
				int_flagx.bit.SciAnAutoWrFRAM = 1;
				sci_oper_flag.bit.sciStartZeroSpd = 0;
				sci_oper_flag.bit.sciStartZeroTcr = 0;
				sci_oper_status.bit.sciZeroSpdStatus = 2;
				sci_oper_status.bit.sciZeroTcrStatus = 2;
				//SPI_WriteFRAM(Fn004Addr,&Pn[Fn004Addr],1);
				//SPI_WriteFRAM(Fn004Addr+1,&Pn[Fn004Addr+1],1);
				//SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
			}
			else
			{				
				Timers.cntdone++;
				if (!sci_oper_flag.bit.SciAdSpdEnd)
				{
					Sspd += Ana1;
				}
				else
				{
					Sspd += (Ana1 - SpdZero);
				}
				
				if (!sci_oper_flag.bit.SciAdTcrEnd)
				{
					Stcr += Ana2;
				}
				else
				{
					Stcr += (Ana2 - TcrZero);
				}
			}
		}
		if (key_flag1.bit.CurAuto)
		{
			#if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
		 	if(int_flag3.bit.Volt_Reached && Timers.cntdone < 2000)		//15V������󣬿��Կ�ʼ����
			#else
			if(Timers.cntdone < 2000)
			#endif
			{
				Timers.cntdone++;
				Scur1 += Iu;
				Scur2 += Iv;
			}
		}

//_______________________________________________________________
		#if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
		if(Timers.judgeRScnt++ <= 40)	//20ms�������
		{
			 if(alm_state.bit.LOSRS)
		     {
		     	 Timers.LostPhasecnt++;
		     }
		}
		else
		{
			 if(Timers.LostPhasecnt < 8)
			 {
				 lost_flag.all = 3;	//ʧ��
			 }
			 else
			 {
				 lost_flag.all = 0;  //����
			 }

			    Timers.judgeRScnt = 0;
			    Timers.LostPhasecnt = 0;
		}
		#else
		if(Timers.judgeRScnt++ <= 40)	//20ms�������
		{
			if(alm_state.bit.LOSRS)
			{
				 Timers.LostPhasecnt++;
			}
		}
		else
		{
			if(Timers.LostPhasecnt < 8)
			{
				lost_flag.all = 3;	//ʧ��
			}
			else if(Timers.LostPhasecnt < 32)
			{
				lost_flag.all = 1;	//ȱ��
			}
			else
			{
				lost_flag.all = 0;  //����
			}

			Timers.judgeRScnt = 0;
			Timers.LostPhasecnt = 0;
		}
		#endif

		if((lost_flag.all == 0) || (lost_flag.all == 3))
		{
			int_flag3.bit.LOSTRS = 0;
			Timers.LOSTRSALMcnt = 0;
		}
		else if(Timers.LOSTRSALMcnt > 60)
		{
			int_flag3.bit.LOSTRS = 1;	//	ȱ���־
		}
		else
		{
			Timers.LOSTRSALMcnt ++;
		}

		// POWER on
		lost_flag.all =1;
		if(lost_flag.all == 3 )
		{
			int_flag3.bit.PowerOn = 0;
		}
		else
		{
			int_flag3.bit.PowerOn = 1;
		}
		if(int_flag3.bit.PowerOn == 0)
		{
			if(int_flag3.bit.PowerFinsih)
			{
				if(Timers.PowerONcnt++ >= 66)
				{
					Timers.PowerONcnt = 0;
					int_flag3.bit.PowerFinsih = 0;
				}
			}
		}
		else
		{// ���ݳ��1�붨ʱ
			Timers.PowerONcnt = 0;
			if(int_flag3.bit.ZQDD)			// ˲ͣ��������һ����Ƶ����
			{
				if(Timers.PowerONZQDDcnt++ >= 40)
				{
					Timers.PowerONZQDDcnt = 0;
					int_flag3.bit.ZQDD = 0;
				}
			}
			if(int_flag3.bit.PowerFinsih == 0)
			{
				if(Timers.PowerONFinishcnt++ >= 2000)
				{
					Timers.PowerONFinishcnt = 0;
					int_flag3.bit.PowerFinsih = 1;
				}
			}
		}

		// NextRST
		if(Timers.PowerONcnt >= 24)
		{
			int_flag3.bit.ZQDD = 1;
		}
		// Next_exittime
		if(int_flag3.bit.PowerOn)
		{
			int_flag3.bit.HavePower = 1;
		}
		else if(int_flag3.bit.HavePower && (Timers.PowerOffcnt < 1000))
		{
			Timers.PowerOffcnt ++;
		}
		else// if((int_flag3.bit.HavePower==0) || (PowerOffcnt >= 1000))
		{
			int_flag3.bit.HavePower = 0;	// ����������ñ�־
			Timers.PowerOffcnt = 0;
		}
		// Endofftime
/*		if(out_state.bit.PWM7)// ==0 or ==1 ? debug:yzhua*********************************
		{
			Timers.Rescnt = 0;
		}
		else if(Timers.Rescnt <= 1100)
		{
			Timers.Rescnt ++;
		}*/

		if (alm_state.bit.RESNO)	//��RESNO�źţ�����++
		{
			Timers.Rescnt++;
		}
		else	//��RESNO�źţ�����
		{
			Timers.Rescnt = 0;
		}
		// NextRescnt
		if(out_state.bit.RYCTL == 0)
		{
			Timers.cputon = 0;
		}
		else if(Timers.cputon < Reycle_delay)
		{
			Timers.cputon ++;
		}
		// nexttimedo DBTimeADD
		if(state_flag2.bit.HaveDB == 0)
		{
			Timers.cntRELAY = 0;
		}
		else if(Timers.cntRELAY < (Pn[DBTimeADD]<<1))
		{
			Timers.cntRELAY ++;
		}
		// nexttimedo1
		// �ƶ���ʱ����
		if(state_flag2.bit.HaveBRKout)
		{
			if(state_flag2.bit.BRKon && state_flag2.bit.INorJOG_SOFF)	// �ⲿԭ��S-OFF
			{
				Timers.BRKcnt ++;
			}
			else
			{
				Timers.BRKcnt = 0;
			}
		}
		// NextBRKTIM
		if(state_flag2.bit.Son || state_flag2.bit.BRKon)
		{
			Timers.BRKWAIcnt = 0;
		}
		else
		{
			Timers.BRKWAIcnt ++;
		}
		// NextBRKWAI
		if(state_flag2.bit.enSON)
		{
			if(Timers.SONWAIcnt < (abs((int16)Pn[SONWAITIMADD])<<1))
			{
				Timers.SONWAIcnt ++;
			}			
		}
		else
		{
			Timers.SONWAIcnt = 0;
		}
		if(!alm_state.bit.PGERR && absALMdelay < sEncoderALMdelay)	// ��ʱ100ms
		{
			absALMdelay++;	
		}
		else
		{
			absALMdelay = 0;
		}
	//Mstime_set();
}

interrupt void ENCODER_PGC_ISR(void)  //Z信号出现
{
	if((memCurLoop00.hex.hex00 == Encoder_Abs17bit) || (memCurLoop00.hex.hex00 == Encoder_Inc17bit))
	{
		if((memBusSel == 3) && (Operation_Mode_Display == HM_MODE))
		{
			if((memCurLoop00.hex.hex00 == Encoder_Abs17bit) || ((memCurLoop00.hex.hex00 == Encoder_Inc17bit) && (!alm_absenc.bit.FS)))
			{
				if(!int_flagx.bit.findCpuls && ((CANopen402_regs.HomeStatus == ARRIVAL_PN) || CANopen402_regs.HomeStatus == LEAVE_PN))
				{
					if(((Homing_Method == 1) && (input_state.bit.NOT == 0))
					 ||((Homing_Method == 2) && (input_state.bit.POT == 0))
					 ||((Homing_Method == 3) && (input_state.bit.HmRef == 0))
					 ||((Homing_Method == 4) && (input_state.bit.HmRef == 0)))
					{	
						Timers.Fndcnt = 0;
						Ek = 0;
						RmEk = 0;
						Pfroate = 0;
						Pfcount = 0;
						RmFg = 0;
						Pf[1] = QPCNT;
						int_flagx.bit.findCpuls = 1;
						co_abs_position = 0;
					}
				}
			}
		}
		else if((memBusSel == 0) && (ColBlock == 4))
		{
			if(memCurLoop00.hex.hex00 == Encoder_Abs17bit)
			{
				findRef(131072);
			}
			else if(memCurLoop00.hex.hex00 == Encoder_Inc17bit && !alm_absenc.bit.FS)
			{
				findRef(131072);
			}
		}
		if((home_flag4.bit.Home_State == ARRIVAL_ORG) && (input_state.bit.ORG == 0) && (memBusSel == 0) && (ColBlock == 0))
		{
			home_flag4.bit.C_Zero = 1;
			Timers.Fndcnt = 0;
			RmEk = 0;
			RmFg = 0;
		}
	}
	else if(membit03.bit.bit01)
	{
		if(((UVWalmflag & 0x0001) == 0) 
		&& (encoder >= UVWsector + 85) 
		&& (EncElect >= encoder + UVWsector + 85))
		{// C ������
		}
		else if((UVWalmflag & 0x0001) && (encoder>=110) && (EncElect >= encoder+110))
		{// C ������
			UVWcounter = encoder;
			if(UVWcounter>=110)
			{
				UVWcounter = EncElect - encoder;
			}
		}
		else
		{// normCpuls
			UVWcounter = encoder;
			if(UVWcounter>=110)
			{
				UVWcounter = EncElect - encoder;
			}
			UVWcounter = abs(UVWcounter);
			if((labs(speed)<300*SpeedUnit) && (UVWcounter >= 25) && (state_flag2.bit.Son == 0))
			{
				UVWalmflag |= 0x0002;	// C ������
			}
			// notCheck
			UVWalmflag |= 0x0001;		// ����ִ�й�C��������
			encoder = 0;
			Encoder4time = 0;
			if(int_flag3.bit.FindRef_ot)
			{// Ref_Pot
				if(int_flag3.bit.pFindRef)
				{
					if(input_state.bit.POT == 0)
					{
						int_flag3.bit.pFindRef = 0;// ����Ҳο���ı�־
						int_flag3.bit.FindRef_ot = 0;// �������ot��־
						Timers.STEPTME = 0;
						Timers.Fndcnt = 0;
						Ek = 0;
						RmEk = 0;
						Pfroate = 0;
						Pfcount = 0;
						RmFg = 0;
						Pf[1] = QPCNT;	
						co_abs_position = 0;					
					}
				}
				else if(int_flag3.bit.nFindRef)
				{// Otherdir
					if(input_state.bit.NOT == 0)
					{
						int_flag3.bit.nFindRef = 0;// ����Ҳο���ı�־
						int_flag3.bit.FindRef_ot = 0;// �������ot��־
						Timers.STEPTME = 0;
						Timers.Fndcnt = 0;
						Ek = 0;
						RmEk = 0;
						Pfroate = 0;
						Pfcount = 0;
						RmFg = 0;
						Pf[1] = QPCNT;	
						co_abs_position = 0;					
					}
				}
			}
			else if((home_flag4.bit.Home_State == ARRIVAL_ORG) && (input_state.bit.ORG == 0) && (memBusSel == 0) && (ColBlock == 0))
			{
				home_flag4.bit.C_Zero = 1;
				Timers.Fndcnt = 0;
				RmEk = 0;
				RmFg = 0;
			}
			else if((memBusSel == 3) && !int_flagx.bit.findCpuls && ((CANopen402_regs.HomeStatus == ARRIVAL_PN) || CANopen402_regs.HomeStatus == LEAVE_PN))
			{
				if(((Homing_Method == 1) && (input_state.bit.NOT == 0))
				 ||((Homing_Method == 2) && (input_state.bit.POT == 0))
				 ||((Homing_Method == 3) && (input_state.bit.HmRef == 0))
				 ||((Homing_Method == 4) && (input_state.bit.HmRef == 0)))
				{	
					Timers.Fndcnt = 0;
					Ek = 0;
					RmEk = 0;
					Pfroate = 0;
					Pfcount = 0;
					RmFg = 0;
					Pf[1] = QPCNT;
					int_flagx.bit.findCpuls = 1;
					co_abs_position = 0;
				}
			}
			else if(state_flag2.bit.Son)
			{
				if((state_flag2.bit.HandPhase || sci_oper_flag.bit.sciPhaseSON) && (memCurLoop00.hex.hex00 != Encoder_Abs17bit) && (memCurLoop00.hex.hex00 != Encoder_Inc17bit) )
				{// TouchPc
					int_flag3.bit.HaveFindInitPos = 1; // ���ó�ʼ��λ���ҵ�
				}
			}

		}
	}
	else
	{
		// notCheck
			UVWalmflag |= 0x0001;		// ����ִ�й�C��������
			encoder = 0;
			Encoder4time = 0;
			if(int_flag3.bit.FindRef_ot)
			{// Ref_Pot
				if(int_flag3.bit.pFindRef)
				{
					if(input_state.bit.POT == 0)
					{
						int_flag3.bit.pFindRef = 0;// ����Ҳο���ı�־
						int_flag3.bit.FindRef_ot = 0;// �������ot��־
						Timers.STEPTME = 0;
						Timers.STEPTME = 0;
						Ek = 0;
						Timers.Fndcnt = 0;
						Pfroate = 0;
						Pfcount = 0;
						RmFg = 0;
						Pf[1] = QPCNT;	
						co_abs_position = 0;					
					}
				}
				else if(int_flag3.bit.nFindRef)
				{// Otherdir
					if(input_state.bit.NOT == 0)
					{
						int_flag3.bit.nFindRef = 0;// ����ҵ�ı�־
						int_flag3.bit.FindRef_ot = 0;// �������ot��־
						Timers.STEPTME = 0;
						Ek = 0;
						Timers.Fndcnt = 0;
						Pfroate = 0;
						Pfcount = 0;
						RmFg = 0;
						Pf[1] = QPCNT;	
						co_abs_position = 0;			
					}
				}
			}
			else if((home_flag4.bit.Home_State == ARRIVAL_ORG) && (input_state.bit.ORG == 0) && (memBusSel == 0) && (ColBlock == 0))
			{
				home_flag4.bit.C_Zero = 1;
				Timers.Fndcnt = 0;
				RmEk = 0;
				RmFg = 0;
			}
			else if((memBusSel == 3) && !int_flagx.bit.findCpuls && ((CANopen402_regs.HomeStatus == ARRIVAL_PN) || CANopen402_regs.HomeStatus == LEAVE_PN))
			{
				if(((Homing_Method == 1) && (input_state.bit.NOT == 0))
				 ||((Homing_Method == 2) && (input_state.bit.POT == 0))
				 ||((Homing_Method == 3) && (input_state.bit.HmRef == 0))
				 ||((Homing_Method == 4) && (input_state.bit.HmRef == 0)))
				{	
					Timers.Fndcnt = 0;
					Ek = 0;
					RmEk = 0;
					Pfroate = 0;
					Pfcount = 0;
					RmFg = 0;
					Pf[1] = QPCNT;
					int_flagx.bit.findCpuls = 1;
					co_abs_position = 0;
				}
			}
			else if(state_flag2.bit.Son)
			{
				if((state_flag2.bit.HandPhase || sci_oper_flag.bit.sciPhaseSON) && (memCurLoop00.hex.hex00 != Encoder_Abs17bit) && (memCurLoop00.hex.hex00 != Encoder_Inc17bit))
				{// TouchPc
					int_flag3.bit.HaveFindInitPos = 1; // ���ó�ʼ��λ���ҵ�
				}
			}
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;//�����ж�Ӧ���־(PIEACK)��Ӧλ���������´��ж�����
	EINT;//��ȫ���ж�
}
//void IncEncoder(void){}
void abs_Encoder(void)
{
//int16 PgDelt;
long absPgtmp;
Uint32 pcPos;
int16 Busy,lData,i;
	Au5561[4] = *AU5561_5;
	Au5561[5] = *AU5561_6;
	PgDelt = 0;
	alm_absenc.all = (Au5561[5] & 0xFF00) + (Au5561[4] & 0x00FF);
	if(int_flagx.bit.rstEncErr)
	{
		*AU5561_0 = (AddrEnc<<8) | 0x07;
		EncState = 0x07;
		rstEncTimer++;
		if(rstEncTimer >= 10)
		{
			int_flagx.bit.rstEncErr = 0;
			rstEncTimer = 0;
		}
	}
	else if(int_flagx.bit.rstEncMuti)
	{
		*AU5561_0 = (AddrEnc<<8) | 0x0C;
		EncState = 0x0C;
		rstEncTimer++;
		if(rstEncTimer >= 10)
		{
			int_flagx.bit.rstEncMuti = 0;
			rstEncTimer = 0;
		}
	}
	else if(int_flagx.bit.rEEPROM)				// ������������
	{
		if(EncState != 0x0D)
		{
			*AU5561_0 = (AddrEnc<<8) | 0x0D;
			EncState = 0x0D;
		}
		else
		{
			rEncData[AddrEnc] = *AU5561_2;
			AddrEnc++;
			*AU5561_0 = (AddrEnc<<8) | 0x0D;
			EncState = 0x0D;
			
			if(AddrEnc == EncDataNum )	// �������������˳���״̬
			{
				AddrEnc = 0;
				int_flagx.bit.rEEPROM = 0;
				int_flagx.bit.rFshROM = 1;
				sEncROM = 0;
				if(!Pn[MotorTab])
				{
					Tetacnt = (rEncData[3]<<8) + rEncData[2];
				}
				for(i = 0; i < EncDataNum; i++)
				{
					sEncROM += rEncData[i];
					if (i == EncDataNum - 2)
					{
						checkROM = sEncROM & 0x00FF;
					}
				}
				return;
			}
		}
	}
	else if(int_flagx.bit.wEEPROM)			// д����������
	{
		if(Timers.wEncTimer>=15000)
		{
			AddrEnc = 0;
			int_flagx.bit.wEEPROM = 0;
			return;			
		}
		else
		{
			Timers.wEncTimer++;
		}
		if(EncState != 0x0D)
		{
			*AU5561_0 = (AddrEnc<<8) | 0x0D;
			EncState = 0x0D;
		}
		else
		{
			Busy =	*AU5561_1 & 0x8000;						
			if(!Busy)							// ��æ�����д
			{
				lData = *AU5561_2;				// ���ϴ�д������
				if(lData == EncData[AddrEnc] )
				{
					AddrEnc++;					// ��ͬ���ַ�ۼӷ�����д
					if(AddrEnc == EncDataNum )	// д�����������˳�д״̬
					{
						AddrEnc = 0;
						int_flagx.bit.wEEPROM = 0;
						Timers.wEncTimer = 0;
						return;
					}
				}
				*AU5561_1 = EncData[AddrEnc];
				*AU5561_0 = (AddrEnc<<8) | 0x06;
				EncState = 0x06;
			}
			else								// æ�������
			{
				*AU5561_0 = (AddrEnc<<8) | 0x0D;
				EncState = 0x0D;				
			}
		}
	}
	else
	{
		Au5561[0] = *AU5561_1;
		Au5561[1] = *AU5561_2;
		Au5561[2] = *AU5561_3;
		Au5561[3] = *AU5561_4;

		spgTime[0] = *sPGTIME;
		*AU5561_0 = 0x03;							   // Request ID = 3
		*PG_ID = 0;
//_____________________________________________________�����ʱ����
		spgTimeDelt[0] = spgTime[0] - spgTime[1];
		if(spgTimeDelt[0] <= 0)
		{
			spgTimeDelt[0] = spgTimeDelt[0] + 0x10000;
		}
		StimeDelt = StimeDelt + spgTimeDelt[0];	
		spgTime[1] = spgTime[0];
//_____________________________________________________λ�ü�Deltλ�ô���
		if(T3INTtimers<1000)
		{
			T3INTtimers++;
		}

		if((alm_absenc.all & 0x1F00)==0)
		{
			sEncErrCnt = 0;
		}
		else
		{
			//T3INTtimers = 0;			
			if(sEncErrCnt < Pn[EncErrTime])
			{
				alm_absenc.all &= 0xE0FF;
				sEncErrCnt++;
			}
		}

		if(T3INTtimers >= 1000 && !rst5v && (sEncErrCnt == 0)) //û�б�������1000�Σ�û�и�λ��
		{
			if(!int_flagx.bit.PgIniti)
			{
				int_flagx.bit.rEEPROM = 1;				
				int_flagx.bit.PgIniti = 1;
				int_flagx.bit.rFshROM = 0;
			}
			if(EncState == 3)
			{
				if(memCurLoop00.hex.hex00 == Encoder_Abs17bit)
				{
					Rotate = ((Uint32)Au5561[3]*0x10000+(Uint32)Au5561[2])>>8;
					Position = ((Uint32)Au5561[1]*0x10000+(Uint32)Au5561[0])>>8;
					singlePos = Position;
				}
				else
				{
					Position = ((Uint32)Au5561[3]*0x10000+(Uint32)Au5561[2])>>8;
					singlePos = ((Uint32)Au5561[1]*0x10000+(Uint32)Au5561[0])>>8;				
				}

				if(!int_flagx.bit.PgPosIniti)
				{
					if (Position & 0x00010000)
					{
						absPos[1] = Position | 0xFFFE0000;
					}
					else
					{
						absPos[1] = Position;
					}
					Pos2 = absPos[1];
					if(!membit02.bit.bit02)
					{
						if((membit01.bit.bit00) && CO_Statusword.bit.OperSpecific_b15)
						{
							Rotate = 65536 - Rotate;
							singlePos = 0x20000 - singlePos;
							co_abs_position = singlePos - co_abs_position + Rotate * 0x20000;
						}
						else
						{
							co_abs_position = singlePos - co_abs_position + Rotate * 0x20000;
						}
					}    //added by niekfu 2012-7-27
					int_flagx.bit.PgPosIniti = 1;				// ��ֹ���β��ٲ���
				}

				if(!int_flag3.bit.PC_inti)
				{
					if(memCurLoop00.hex.hex00 == Encoder_Abs17bit)
					{
						pcPos = (long long)Position * memPGDIV>>15;
						if(membit01.bit.bit00)
						{
							pcPos = (memPGDIV<<2) - pcPos + 3;
						}
						PC_POS = pcPos;
						int_flagx.bit.wPC_POS = 1;
						int_flag3.bit.PC_inti = 1;
					}
					else if(alm_absenc.bit.FS==0)
					{
						pcPos = (long long)singlePos * memPGDIV>>15;
						if(membit01.bit.bit00)
						{
							pcPos = (memPGDIV<<2) - pcPos + 3;
						}
						PC_POS = pcPos;
						int_flagx.bit.wPC_POS = 1;
						int_flag3.bit.PC_inti = 1;				
					}
				}
			}
		}
		else
		{
			int_flag3.bit.PC_inti = 0;
			int_flagx.bit.PgIniti = 0;
			int_flagx.bit.PgPosIniti = 0;
			PGrm = 0;
			*PC_Enable = 0;
			*PG_NOP = 0;
		}

		if (Position & 0x00010000)
		{
			absPos[0] = Position | 0xFFFE0000;
		}
		else
		{
			absPos[0] = Position;
		}

		ErrPos = (int16)(absPos[0] - absPos[1]);

		incPos = ErrPos + incPos;
		absPos[1] = absPos[0];
//________________________________________________________A,B PG��Ƶ
		absPgtmp = (long)ErrPos * (long)memPGDIV; 
		PgDelt = (int)(absPgtmp / 32768);
		PGrm += absPgtmp % 32768;
		if(labs(PGrm)>=32768)
		{
			if(PGrm>0)
			{
				PGrm -= 32768;
				PgDelt++;
			}
			else
			{
				PGrm += 32768;
				PgDelt--;
			}
		}
		EncState = 0x03;								// ��¼ID״̬
//_____________________________________________________�ٶȼ���
		spgINTcnt++;
		if(spgINTcnt >= 1 && labs(incPos) >= 10 || spgINTcnt >= Pn[measureSPDADD] )
		{
			#if CPU_FRQ == CPU_100MHz
			fact_speed = incPos * 457764 / StimeDelt;
			#else
			fact_speed = incPos * 343323 / StimeDelt;
			#endif
			Delt_speed[0] = fact_speed - avslid_speed;
			if (labs(Delt_speed[0]) > 40 && !membit02.bit.bit01)	
			{
				if (labs(Delt_speed[1]) > 40 &&
				    (Delt_speed[0] * Delt_speed[1]) > 0)
				{
					speed = fact_speed;	
				}			
			}
			else
			{
				speed = fact_speed;
			}
	//--------------------------------------------�����˲�
			total_slid = total_slid + speed - slid_speed[slid_speed_index];
			if(slid_speed_index == 0)
			{
				i = 63;
			}
			else
			{
				i = slid_speed_index - 1;
			}

			if(++slid_speed_index > 63)
			{
				slid_speed_index = 0;
			}
			slid_speed[i] = speed;
			avslid_speed = total_slid >> 6;
	//--------------------------------------------
			Delt_speed[1] = Delt_speed[0];
			StimeDelt = 0;
			incPos = 0;
			spgINTcnt = 0;
		}
	}
}
void overload(void)
{
int32 avIqr,temp,loadLevelTemp;
int16 guai_A,guai_B,flag;

	loadLevelTemp = (((int32)Pn[OverloadLevelADD]*Pn[OverloadLevelADD])<<12)/625;	//Pn[OverloadLevelADD]过载百分比
	temp = labs(speed/SpeedUnit);                                                   // <<16/10000 ��Ϊ<<12)/625����ֹ�������
	if(temp > RatedSpeed )
	{
		temp = temp * Iqr / RatedSpeed;
		SGMAIqr += labs(temp);
		if(Iqr < 2 * Iqn)
		{
			flag = 1;	
		}
		else
		{
			flag = 0;
		}
	}
	else
	{
		SGMAIqr += labs(Iqr);
		flag = 0;
	}
	avIqrCnt++;
	if(avIqrCnt >= 200)
	{
		avIqr = SGMAIqr / avIqrCnt;
		SGMAIqr = 0;
		avIqrCnt = 0;
		if(speed && membit03.bit.bit03 || flag)
		{
			guai_A = 1;
			guai_B = 512;
		}
		else
		{
			switch(factory_load)
			{
				case 400:
					guai_A = 3;
					guai_B = 512;
					break;				
				case 300:
					guai_A = 6;
					guai_B = 512;
					break;
				case 250:
					guai_A = 13;
					guai_B = 512;
					break;
				case 200:
			//	    guai_A = 14;
					guai_A = 12;	//�޸����ʱ��ϵ����ʹ����ע�ܻ�Ӧ�?2012-02-17
					guai_B = 384;
					break;
				case 150:
					guai_A = 29;
					guai_B = 320;
					break;
				default:
					guai_A = 29;
					guai_B = 320;
					break;
			}
		}
		temp = Iqn * guai_B >> 8;		// �յ���ֵ
		if (avIqr > temp)				// x_SQRTiq = sqr(2 + a * (Iq-2Iqn) / Iqn)
		{
			temp = guai_A * (avIqr - temp) + temp;
		}
		else
		{
			temp = avIqr;				// Iq/Iqn < 2, x_SQRTiq = sqr( Iq/Iqn )
		}
		temp = (temp<<8) / Iqn; 		// 8.8f	
		temp = temp * temp;				// 0.16f

        if (temp <= loadLevelTemp)
		    x_SQRTiq = x_SQRTiq + (temp - loadLevelTemp);
		else 
		    x_SQRTiq = x_SQRTiq + (temp - 0x10000);

	    if(x_SQRTiq < 0)
	    {
		    x_SQRTiq = 0;
	    }
	}

}
void runcur_process(void)
{int32 cur_temp2;
 int16 i,cur_temp,cur_temp1;
 int32 Vdq,Vqd;
//------------------------------------------------- 
// Ialfa = Ia
// Ibeta = (2*Ib + Ia) / sqrt(3)
//-------------------------------------------------
	Ialfa = Ia;
	cur_temp2 = (  ( (int32)((int32)Ia + ((int32)Ib << 1)) ) * 0x093d   )>>12;
	if(cur_temp2 > 32767)
	{
		Ibeta = 32767;
	}
	else if(cur_temp2 < -32767)
	{
		Ibeta = -32767;
	}
	else
	{
		Ibeta = cur_temp2;
	}
      
//------------------------------------------------- 
// Id =  Ialfa * cos(Teta) + Ibeta * sin(Teta)
// Iq = -Ialfa * sin(Teta) + Ibeta * cos(Teta) 
//-------------------------------------------------
	cur_temp2 = (((int32)Ialfa) * Cos_Teta + ((int32)Ibeta) * Sin_Teta ) >> 12;
	if(cur_temp2 > 32767)
	{
		Id = 32767;
	}
	else if(cur_temp2 < -32767)
	{
		Id = -32767;
	}
	else
	{
		Id = cur_temp2;
	}
	cur_temp2 = (((int32)Ibeta) * Cos_Teta - ((int32)Ialfa) * Sin_Teta ) >> 12;
	if(cur_temp2 > 32767)
	{
		Iq = 32767;
	}
	else if(cur_temp2 < -32767)
	{
		Iq = -32767;
	}
	else
	{
		Iq = cur_temp2;
	}
	//---------------------------------------------
#if 1





#endif
	            //-----------------------------------------------
	#if	DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW
	Vqd = (long long)speed * Id * memLdq * memPoleNum >> 24;
	Vdq = (long long)speed * Iq * memLdq * memPoleNum >> 24;
	#elif	DRV_TYPE == DRV_5KW
	Vqd = (long long)speed * Id * memLdq * memPoleNum >> 23;
	Vdq = (long long)speed * Iq * memLdq * memPoleNum >> 23;
	#elif DRV_TYPE == DRV_15KW
	Vqd = (long long)speed * Id * memLdq * memPoleNum >> 22;
	Vdq = (long long)speed * Iq * memLdq * memPoleNum >> 22;
	#endif
	Vqd = PWM_PERIOD * Vqd / 5000;              //Ҫ����
	Vdq = PWM_PERIOD * Vdq / 5000;
//------------------------------------------------- 
// ����������1
//-------------------------------------------------
	if(Pn[MotorTab])
	{
		Kpcurrent = Pn[curKpADD];
		Kicurrent = Pn[curKiADD];
	}
	else
	{
		Kpcurrent = MotorPar4[memDriveSel][memMotorSel][10];
		Kicurrent = MotorPar4[memDriveSel][memMotorSel][11];
	}


	e_id = Idr - Id;
	x_id = x_id + ((int32)e_id) * Kicurrent;
	if (labs(x_id) > lmtCurInt)				// -->Ud <= 2500
	{
	  if (x_id>0)
	  {
	    x_id = lmtCurInt;
	  }
	  else
	  {
	    x_id = -lmtCurInt;
	  }
	}
	Udbuf = (x_id + (((int32)e_id) * Kpcurrent))>>12;
	Udbuf -= Vdq;
	if (labs(Udbuf)>lmtVOLT)
	{
	  if (Udbuf>0)
	    Udbuf = lmtVOLT;
	  else
	    Udbuf = -lmtVOLT;
	}
	Ud = (int16)Udbuf;
//------------------------------------------------- 
// ����������2
//-------------------------------------------------
	e_iq = Iqr - Iq;	
	x_iq = x_iq + ((int32)e_iq) * Kicurrent;
	if (labs(x_iq) > lmtCurInt)
	{
	  if (x_iq>0)
	  {
	    x_iq = lmtCurInt;
	  }
	  else
	  {
	    x_iq = -lmtCurInt;
	  }
	}
	Uqbuf = (x_iq + (((int32)e_iq) * Kpcurrent))>>12;
//-------------------------------------------------
// ����ǰ��
//-------------------------------------------------
	nUq = (int16)((speed * PWM_PERIOD)/(varMaxSPEED*SpeedUnit));
//	nUq = (int32)nUq * PWM_PERIOD / 5000;
	nUq = (((int32)nUq) * varEmCoFF)>>12;
	cur_temp = labs(Iqr);
	i = 0;
	if (varExcelNum > 1)
	{	  
	  while( ( xMotor[i] <= cur_temp) &&  ( i <= (varExcelNum - 1 ) ))  			// ������Iqr�ڱ���е�λ��
	  {
	    i++;
	  }

	  if (i > (varExcelNum - 1 ) )														// Iqr�������
	  {
	    cur_temp1 = yMotor[varExcelNum - 1] + 
	    			( ( ( (int32)varKu ) * 
	    			    ( cur_temp - xMotor[varExcelNum - 1]) )>>12);
	  }
	  else																				// �ڱ��֮��
	    if ( i > 0)																		// ��x0����
		  {
		    cur_temp1 = xMotor[i] - xMotor[i - 1];
			cur_temp2 = ((int32) (yMotor[i] - yMotor[i - 1]))<<12;
			cur_temp = cur_temp - xMotor[i - 1];
			cur_temp1 = yMotor[i - 1] + 
			            ((cur_temp * cur_temp2 / cur_temp1)>>12);
		  }
		else																			// x0--x15֮��
		  {
		    cur_temp2 =   ( ( ((int32)cur_temp) * yMotor[0] )<<12 ) / xMotor[0];
			cur_temp1 = (int16) ( cur_temp2 >> 12 );
		  }
     if (Iqr<0)	
	    cur_temp1 = - cur_temp1;
      UqFF = cur_temp1;
	}
	else
	{
	  UqFF = 0;
	}
//	UqFF = (int32)UqFF * PWM_PERIOD / 5000;
	UqFF = ((int32)UqFF * varUqFFCoFF)>>12;
	Uqbuf = Uqbuf + nUq + UqFF;				// ����ǰ���ͷ����Ʋ���
	Uqbuf += Vqd;
	if (labs(Uqbuf)>lmtVOLT)
	{					// Uq
	  if (Uqbuf > 0)
	    Uqbuf = lmtVOLT;
      else
	    Uqbuf = -lmtVOLT;
	}	
	Uq = (int16) ( Uqbuf );
	////////////////////////
#if 1

//	Pq=abs(Tn)*speed;//Iq*Vdq;
	//Pd=Id*Vqd;
//    if(Pcount<1500)
//        {
 //           Pcount++;
          //  Spd=Spd+Pd;
  //          Spq=Spq+abs(Pq);

   //     }
  //  else
  //  {
   //     Pz=(Spq)/1500;
   //     Pcount=0;
   //     Spq=0;
   // }
#endif
////////////////////////////
//-------------------------------------------------
// ���ദ��
//-------------------------------------------------
	if(state_flag2.bit.HandPhase)			// �ֶ�����
	{
	  	if ((memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit && int_flag3.bit.HaveFindInitPos) || 
	  	    (memCurLoop00.hex.hex00 == Encoder_Abs17bit||memCurLoop00.hex.hex00 == Encoder_Inc17bit) && int_flag3.bit.pass360)	// �ֶ��������
	  	{
	    	if (AsTeta == 90)
			{
		  		OrientFlux();					// �����ջ��ų���λ
			}
			else
			{
		  		Asroate();						// �첽�϶�
			}
	  	}
	  	else
	  	{
	    	Asroate();							// �첽�϶�
	  	}	  
	}
	else if(sci_oper_flag.bit.sciPhaseSON && (!int_flagx.bit.SciFindInitPhase))
	{
	  	if (((memCurLoop00.hex.hex00 != Encoder_Abs17bit) && (memCurLoop00.hex.hex00 != Encoder_Inc17bit) && int_flag3.bit.HaveFindInitPos) || 
	  	    (((memCurLoop00.hex.hex00 == Encoder_Abs17bit) || (memCurLoop00.hex.hex00 == Encoder_Inc17bit)) && int_flag3.bit.pass360))	// �ֶ��������
	  	{
	    	if (AsTeta == 90)
			{
		  		OrientFlux();					// �����ջ��ų���λ
				if(ccount++ > 20000)			// ʸ�����������ʱ2��
				{
					ccount = 0;
					int_flagx.bit.SciFindInitPhase = 1;
					Tetacnt = (Uint16)encoder;   // 原来是冷开机时赋值，现在每次赋值   //Tetacnt = Pn[InitPhaseADD];
					ComminitPhase = Tetacnt;    //�õ���λ
				}
			}
			else
			{
		  		Asroate();						// �첽�϶�
		  		 /////////
		  	   if(AsTeta==80)
		  	                                                       {
		  	                                                                        AsSpeed=speed;
		  	                                                       }
		  		                     //////////
			}
	  	}
	  	else
	  	{
	    	Asroate();							// �첽�϶�
	  	}
	}
	else
	{
		AsTeta = 0;
		int_flag3.bit.pass360 = 0;
	}
	SVPWM();

}
//----------------------------拖动1度检查旋变-----------------------------
//----------------------------------------------------------------------

//_________________________________________________
//
//	异步拖动一圈
//_________________________________________________
void Asroate(void)
{int16 Astmp;
    Uq = varAsroateUm;
	Ud = 0;
	ccount++;
	if (ccount >= AsynRunFi )   //异步拖动频率 （30个pwm加一度，然后SVPWM()）
	{ ccount = 0;
	  AsTeta ++;

	  if (AsTeta >= 360)
	  {
	    AsTeta = 0;
		int_flag3.bit.pass360 = 1;
	  }
	}
    Sin_Teta = sintab[AsTeta];
    Astmp = AsTeta + 90;
    if (Astmp > 360)
    {
    Astmp = Astmp - 360;
    }
    Cos_Teta = sintab[Astmp];
}
//_________________________________________________
//
//	�����ջ��ų���?
//_________________________________________________
void OrientFlux(void)
{
int16 Astmp1;
int32 xIa;
	if(Pn[MotorTab])   //  330=1ʱ������������ȡֵ
	{
		Kpcurrent = Pn[curKpADD];
		Kicurrent = Pn[curKiADD];
		xIa = (int16)Pn[InormalADD];    //845 额定电流     !>模块电流*200
	}
	else
	{
		Kpcurrent = MotorPar4[memDriveSel][memMotorSel][10];
		Kicurrent = MotorPar4[memDriveSel][memMotorSel][11];
		xIa = (int16)Iqn;
	}

	#if DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW
	if(xIa>IPMImax*400)    //������޷���С��ģ�����*400
	{
		xIa = IPMImax*400;
	}
	#elif DRV_TYPE == DRV_5KW
	if(xIa>IPMImax*200)
	{
		xIa = IPMImax*200;
	}
	#elif  DRV_TYPE == DRV_15KW
	if(xIa>IPMImax*100)
	{
		xIa = IPMImax*100;
	}
	#endif

	e_ia = - Ia - xIa;
	x_ia = x_ia + ((int32) (Kicurrent)) * e_ia;
	if (labs(x_ia) > 0x01D4C000 )
	{
	  if (x_ia > 0)
	    x_ia = 0x01D4C000;
      else
	    x_ia = -0x01D4C000;
	}
	Uqbuf = x_ia + ((int32)(Kpcurrent)) * e_ia;
	Uqbuf = -Uqbuf;	
	Sin_Teta = sintab[AsTeta];
    Astmp1 = AsTeta + 90;
    if (Astmp1 > 360)
    {
    Astmp1 = Astmp1 - 360;
    }
    Cos_Teta = sintab[Astmp1];
	Uq = Uqbuf >> 12;
	if (labs(Uq) > lmtVOLT)
	{
	  if (Uq > 0)
	    Uq = lmtVOLT;
	  else
	    Uq = -lmtVOLT;
	}
	Ud = 0;
}
void SVPWM(void)
{
    int16 A,B,C;
	int16 cTime1,cTime2;
//	Ud = 0;
//	Uq = 800;
//------------------------------------------------- 
// Ualfa = Ud * cos(Teta) - Uq * sin(Teta)
// Ubeta = Ud * sin(Teta) + Uq * cos(Teta)
//-------------------------------------------------

	Ualfa = (((int32)Ud) * Cos_Teta - ((int32)Uq) * Sin_Teta) >> 12;
	Ubeta = (((int32)Ud) * Sin_Teta + ((int32)Uq) * Cos_Teta) >> 12;

//------------------------------------------------- 
// Ua = Ualfa
// Ub = -1/2 * Ualfa + sqrt(3)/2 * Ubeta     
// Uc = -1/2 * Ualfa - sqrt(3)/2 * Ubeta
//-------------------------------------------------

	Ua = Ualfa;
	Ub = -(Ualfa >> 1) + ((((int32)Ubeta) * 0x0ddb)>>12);
	Uc = -(Ualfa >> 1) - ((((int32)Ubeta) * 0x0ddb)>>12);

//------------------------------------------
// Va = Ubeta
// Vb = -1/2 * Ubeta + sqrt(3)/2 * Ualfa
// Vc = -1/2 * Ubeta - sqrt(3)/2 * Ualfa
//------------------------------------------

	Va = Ubeta;
	Vb = -(Ubeta >> 1) + ((((int32)Ualfa) * 0x0ddb)>>12);
	Vc = -(Ubeta >> 1) - ((((int32)Ualfa) * 0x0ddb)>>12);

//------------------------------------------
// if Va>0 then A=1 else A=0	
// if Vb>0 then B=1 else B=0
// if Vc>0 then C=1 else C=0
// sector = 1*A + 2*B + 4*C
//------------------------------------------

	if ( Va > 0 )  A = 1;	
	else A = 0;

	if ( Vb > 0 )  B = 1;
	else B = 0;

	if ( Vc > 0 )  C = 1;
	else C = 0;

	sector = A + ( B << 1) + ( C << 2 );

//================================
//|  sector	 	t1		 t2      |	
//| ---------------------------- |
//|    1		-Vb 	-Vc		 |
//|    2		-Vc		-Va		 |	
//|    3		 Vb	 	 Va		 |
//|    4		-Va		-Vb 	 |
//|    5		 Va	 	 Vc		 |
//|    6		 Vc	 	 Vb		 |
//================================
	switch(sector)
	{
	  case 1:
	    t1 = -Vb;
		t2 = -Vc;
		break;
	  case 2:
	    t1 = -Vc;
		t2 = -Va;
		break;
      case 3:
	    t1 = Vb;
		t2 = Va;
		break;
	  case 4:
	    t1 = -Va;
		t2 = -Vb;
		break;
	  case 5:
	    t1 = Va;
		t2 = Vc;
		break;
	  case 6:
	    t1 = Vc;
		t2 = Vb;
		break;
	  default:
	    break;
	}
//------------------------------------------
// saturation,do with
//------------------------------------------
	if ((t1 + t2) > PWM_PERIOD)
	{
	   cTime1 = (int16)((PWM_PERIOD * ((int32)t1)) / ( t1 + t2 ));
	   cTime2 = (int16)((PWM_PERIOD * ((int32)t2)) / ( t1 + t2 ));
	   t1 = cTime1;
	   t2 = cTime2;
	}
//------------------------------------------
// taon=(T1PR-t1-t2)/2
// tbon=taon+t1
// tcon=tbon+t2
//------------------------------------------
	taon = (PWM_PERIOD - t1 - t2 ) >> 1;
	tbon = taon + t1;
	tcon = tbon + t2;
/*	if(taon == 0)
	{
		taon = 1;
	}
	else if(taon == 5000)
	{
		taon = 4999;
	}
	if(tbon == 0)
	{
		tbon = 1;
	}
	else if(tbon == 5000)
	{
		tbon = 4999;
	}
	if(tcon == 0)
	{
		tcon = 1;
	}
	else if(tcon == 5000)
	{
		tcon = 4999;
	}*/

//================================
//| sector	CMPR4  CMPR5  CMPR6  |
//|------------------------------|
//|   1		tbon   taon   tcon   |
//|   2		taon   tcon   tbon   |	
//|   3 	taon   tbon   tcon   |
//|   4 	tcon   tbon   taon   |				
//|   5 	tcon   taon   tbon   |
//|   6 	tbon   tcon   taon   |
//================================
	switch(sector)
	{
	  case 1:
		EPwm1Regs.CMPA.half.CMPA = tbon;
		EPwm2Regs.CMPA.half.CMPA = taon;
		EPwm3Regs.CMPA.half.CMPA = tcon;
		break;
	  case 2:
		EPwm1Regs.CMPA.half.CMPA = taon;
		EPwm2Regs.CMPA.half.CMPA = tcon;
		EPwm3Regs.CMPA.half.CMPA = tbon;
		break;
	  case 3:
		EPwm1Regs.CMPA.half.CMPA = taon;
		EPwm2Regs.CMPA.half.CMPA = tbon;
		EPwm3Regs.CMPA.half.CMPA = tcon;
		break;
	  case 4:
		EPwm1Regs.CMPA.half.CMPA = tcon;
		EPwm2Regs.CMPA.half.CMPA = tbon;
		EPwm3Regs.CMPA.half.CMPA = taon;
		break;
	  case 5:
		EPwm1Regs.CMPA.half.CMPA = tcon;
		EPwm2Regs.CMPA.half.CMPA = taon;
		EPwm3Regs.CMPA.half.CMPA = tbon;
		break;
	  case 6:
		EPwm1Regs.CMPA.half.CMPA = tbon;
		EPwm2Regs.CMPA.half.CMPA = tcon;
		EPwm3Regs.CMPA.half.CMPA = taon;
		break;
	  default: break;
	}
//_________________________________________________

}

// Y(n) = [X(n) ^ X(n-1) & Y(n-1)] | [X(n) & X(n-1)]
// AlarmStatus: (9)PGBR,(8)PGerr,(7)OHEAT,(6)RESNO,(5)PUMP,(4)LOSRS,(3)LOSST,(2)OVOLT,(1)OCURR,(0)UVOLT
void InputFilter(void)
{
// input
int16 temp,profibus_io,sci_io;
	temp = 0;
	if(Timers.InFilterCount++ > Pn[InputFilterTimeADD])
	{
		input_buf[1] = *TOTAL_IN;
			temp = (Pn[Bus_io_HEn])<<4;
			temp |= Pn[Bus_io_LEn];
			if(((memBusSel == 1) || (memBusSel == 2)))
			{
				profibus_io = profibus_buf.pbus_input & temp;
				input_buf[1] |= temp;			// Par set "1",profibus io valid
				input_buf[1] &= (~profibus_io);
			}
			else if(memBusSel == 3)
			{
				input_buf[1] |= temp;			
				input_buf[1] &= (~((Physical_Outputs>>24)&temp));
			}
			else
			{				
				sci_io = sci_data.SCIInput & temp;
				input_buf[1] |= temp;			// Par set "1",profibus io valid
				input_buf[1] &= (~sci_io);
			}			

		if(int_flag3.bit.infiltercal)
		{
			int_flag3.bit.infiltercal = 0;
			in_filter_out[1] = (((input_buf[1] ^ input_buf[0]) & in_filter_out[0]) | (input_buf[1] & input_buf[0]));
			Timers.InFilterCount = 0;
		}
		else
		{
			int_flag3.bit.infiltercal = 1;
		}
		input_buf[0] = input_buf[1];
		in_filter_out[0] = in_filter_out[1];
	}
// alm & PGerr : PGERR ABCBR UVWBR 
	if(Timers.AlminFilterCount++ > Pn[ALMinFilterTimeADD])
	{
		almin_buf[1] = *PGSTA_ALM;
		if(state_flag2.bit.alminfiltercal)
		{
			state_flag2.bit.alminfiltercal = 0;
			almin_filter_out[1] = (((almin_buf[1] ^ almin_buf[0]) & almin_filter_out[0]) | (almin_buf[1] & almin_buf[0]));
			Timers.AlminFilterCount = 0;
		}
		else
		{
			state_flag2.bit.alminfiltercal = 1;
		}
		almin_buf[0] = almin_buf[1];
		almin_filter_out[0] = almin_filter_out[1];
	}
}


void speed_loop_process(void)
{

int32 tmpAna1,tmpAna2;//,tmpAna3;
//---------------------------------------------------------------------------------
// �ٶȼ���
//---------------------------------------------------------------------------------
	if (memCurLoop00.hex.hex00 != Encoder_Abs17bit && memCurLoop00.hex.hex00 != Encoder_Inc17bit)
	{

		pulsEncoder();    //speed速度计算结果由此输出

//--------------------------------------------------------
	}
	else
	{
		if (Position & 0x00010000)
		{
			Pos1 = Position | 0xFFFE0000;
		}
		else
		{
			Pos1 = Position;
		}
		if(PosCnt > 2)
		{
			Pferr = Pos1 - Pos2;
			ZeroPf = Pferr;
		}
		else
		{
			PosCnt++;
		}
		Pos2 = Pos1;
	}
	test_abs_position += Pferr;
	if(TestPara.Test_Mode)
	{
		test_abs_position_more = (long long)test_abs_position * Bgear/Agear;
	}
	co_abs_position += Pferr;
	co_abs_roate = (int16)(co_abs_position/10000);
	co_abs_count = (int16)(co_abs_position%10000);

//------------------------------------------
	if(++SwGainTimer >= 10)
	{
		SwGainTimer = 0;
		SwGainPg[0] = *GPCNT;
		SwGainPgerr = SwGainPg[0] - SwGainPg[1];
		SwGainPg[1] = SwGainPg[0];
	}
	if(!state_flag2.bit.Son)				// 2007-8-17 modify;   δ������Ϊ0
	{
		One_Jerk_filter.remain = 0;
		One_Jerk_filter.OutData = 0;
		One_Jerk_filter.OutValue = 0;
		Two_Jerk_filter.remain = 0;
		Two_Jerk_filter.OutData = 0;
		Two_Jerk_filter.OutValue = 0;
		Tcr_filter.remain = 0;
		Tcr_filter.OutData = 0;
		Tcr_filter.OutValue = 0;
		SFiltOut = 0;
		fun0 = 0;
		xtime = 0;
		xIqr[0] = 0;		//S_offʱ �����ݲ��˲�����ر���
		xIqr[1] = 0;
		xIqr[2] = 0;
		x1Iqr[0] = 0;
		x1Iqr[1] = 0;
		x1Iqr[2] = 0;
		x2Iqr[0] = 0;
		x2Iqr[1] = 0;
		x2Iqr[2] = 0;

		yIqr[0] = 0;
		yIqr[1] = 0;
		yIqr[2] = 0;
		y1Iqr[0] = 0;
		y1Iqr[1] = 0;
		y1Iqr[2] = 0;
		y2Iqr[0] = 0;
		y2Iqr[1] = 0;
		y2Iqr[2] = 0;

		yIqrm[0] = 0;
		yIqrm[1] = 0;
		yIqrm[2] = 0;
		y1Iqrm[0] = 0;
		y1Iqrm[1] = 0;
		y2Iqrm[0] = 0;
		y2Iqrm[1] = 0;		//S_offʱ �����ݲ��˲�����ر���

//=====================================================================
		Speed[0] = Speed[1] = Speed[2] = 0;
		SpeedL[0] = SpeedL[1] = SpeedL[2] = SpeedL[3] = 0;
		SpeedLRm[0] = SpeedLRm[1] = SpeedLRm[2] = SpeedLRm[3] = 0;
//=====================================================================

//_______________________________________________
//
//		�����л���ʼ��
//_______________________________________________
		Kpos = Pn[PosloopGainADD]*10;
		Kpspeed = Pn[SpdloopGainADD];
		Kispeed = Pn[SpdloopItimeADD];
		Tiqr = Pn[TCRFiltTimeADD];
		SWKposRes = 0;
		SWKspdRes = 0;
		SWKispdRes = 0;
		SWdelay = 0;
		int_flag3.bit.SWgain = 0;
		int_flag3.bit.HowGain = 0;
		int_flag3.bit.StartSW = 0;
		AccCnt = 0;
		last10msSPD = 0;
		AccCnt1 = 0;
		xSPEEDACC1ms = 0;
		SPEEDACC1ms = 0;
		last100usSPD = 0;
	}
	else									// �����л�
	{
		if(AccCnt<100)       //100���ٶ��ж� 10KƵ��  100us*100 =10ms
		{
			AccCnt++;
		}
		else
		{
			SPEEDACC = SPEED - last10msSPD;   //�ٶȲ���ٶ�  SPEED �����ٶ�
			AccCnt = 0;
			last10msSPD = SPEED;
		}
		if(memhex01.hex.hex00<2)   //10KƵ��   ������ת��ǰ��
		{
			if(AccCnt1<10)          //1ms
			{
				AccCnt1++;
				xSPEEDACC1ms = xSPEEDACC1ms + SPEED - last100usSPD;
			}
			else
			{
				SPEEDACC1ms = xSPEEDACC1ms;
				xSPEEDACC1ms = 0;
				AccCnt1 = 0;			
			}
		}
		else
		{
			SPEEDACC1ms = SPEED - last100usSPD;
		}
		last100usSPD = SPEED;
		GainSwitch();
	}
	// notclrspeedpara
	Agear = memAgear1;
	if(input_state.bit.CLR)
	{
		int_flag3.bit.haveCLRZero = 0;			
	}
	
	ControlSel();
//________________________________________________
//
// ����ģ��ͨ��
//________________________________________________
	if(memBusSel != 3) //�������ͣ�0 �� 1 profibus9=1, 2 profibus 2 3 can
	{
		if(sci_oper_flag.bit.SciAdSpdEnd == 0) // �ٶ�ģ����δУ��,�������۹�ʽ���м���  SCIģ����У����־
	  	{
        	Ana1 = Vref_Sample;
			Ana1 -= 2048;			// 1.5v = 2048
			tmpAna1 = Ana1 - spdoffset;
			tmpAna1 += (int16)Pn[AnaSpdOffset] * 139264 / 70500;	// (2^11*6.8)/[(47*1.5)*100]
			if(tmpAna1 > 2048)
			{
				tmpAna1 = 2048;
			}
			else if (tmpAna1 < -2048)
			{
				tmpAna1 = -2048;
			}
		    VCMD = (long long)tmpAna1 * Pn[SpdInsGainADD] * SpeedUnit * 705 / 139264;//47*1.5/(2^11*6.8)
		}
        else // �ٶ�ģ����У��
	   	{	     	
	        Ana1 = Vref_Sample;
			tmpAna1 = Ana1 - spdoffset;
	     	if(Vref_Sample > SpdPos5)
		 	{
				tmpAna1 = (tmpAna1 - 2 * SpdPos5 + SpdPos10) * 500 / (SpdPos10 - SpdPos5);//����10V��5VУ��ֵ���¼��㹫ʽ
			}
		 	else if((Vref_Sample > SpdZero) && (Vref_Sample <= SpdPos5))
		 	{
				tmpAna1 = (tmpAna1 - SpdZero) * 500 / (SpdPos5 - SpdZero);//����5V��0VУ��ֵ���¼��㹫ʽ
		 	}
		 	else if((Vref_Sample > SpdNeg5) && (Vref_Sample <= SpdZero))
		 	{
                tmpAna1 = (tmpAna1 - SpdZero) * 500 / (SpdZero - SpdNeg5);//����0V��-5VУ��ֵ���¼��㹫ʽ
		 	}
		 	else if(Vref_Sample <= SpdNeg5)
         	{
				tmpAna1 = (tmpAna1 - 2 * SpdNeg5 + SpdNeg10) * 500 / (SpdNeg5 - SpdNeg10);//����-5V��-10VУ��ֵ���¼��㹫ʽ
		 	}
			tmpAna1 = tmpAna1 + (int16)Pn[AnaSpdOffset];
			if(tmpAna1 > 1035) // �޷�,�Ͳ�У��������޷�ֵ��ͬ
			{
               tmpAna1 = 1035;
			}
			else if(tmpAna1 < -1035)
			{
			   tmpAna1 = -1035;
			}
	        VCMD = tmpAna1 * Pn[SpdInsGainADD] * SpeedUnit / 100;
        }	
		if(labs(VCMD) > SPEEDlmt)
		{
			if(VCMD > 0)
			{
				VCMD = SPEEDlmt;
			}
			else
			{
				VCMD = -SPEEDlmt;
			}
		}
		AVref = 0;
		AVcount = 0;
		if(membit01.bit.bit00)
		{
			VCMDG = - VCMD;
		}
		else
		{
			VCMDG = VCMD;
		}

//---------------------------------------
		if(sci_oper_flag.bit.SciAdTcrEnd == 0) // ����ģ����δУ��,�������۹�ʽ���м���
	  	{
			Ana2 = Tref_Sample;
			Ana2 -= 2048;		// 1.5v = 2048	
			tmpAna2 = Ana2 - TCRoffset;
			tmpAna2 += (int16)Pn[AnaTcrOffset] * 139264 / 70500;	// (2^11*6.8)/[(47*1.5)*100]

			if(tmpAna2 > 2048)
			{
				tmpAna2 = 2048;
			}
			else if (tmpAna2 < -2048)
			{
				tmpAna2 = -2048;
			}
			AnIqr = (long long)tmpAna2 * Iqn * 7050 / (139264 * (int32)Pn[TCRInsGainADD]);	
		}
		else // ����ģ����У��
		{
			Ana2 = Tref_Sample;
			tmpAna2 = Ana2 - TCRoffset;
			if(Tref_Sample > TcrPos5)
		 	{
				tmpAna2 = (tmpAna2 - 2 * TcrPos5 + TcrPos10) * 500/ (TcrPos10-TcrPos5);//����10V��5VУ��ֵ���¼��㹫ʽ
			}
		 	else if((Tref_Sample > TcrZero) && (Tref_Sample <= TcrPos5))
		 	{
				tmpAna2 = (tmpAna2 - TcrZero) * 500 / (TcrPos5 - TcrZero);//����5V��0VУ��ֵ���¼��㹫ʽ
		 	}
		 	else if((Tref_Sample > TcrNeg5) && (Tref_Sample <= TcrZero))
		 	{
				tmpAna2=(tmpAna2 - TcrZero)*500 / (TcrZero - TcrNeg5);//����0V��-5VУ��ֵ���¼��㹫ʽ
		 	}
		 	else if(Tref_Sample <= TcrNeg5)
         	{
				tmpAna2 = (tmpAna2 - 2 * TcrNeg5 + TcrNeg10) * 500 / (TcrNeg5 - TcrNeg10);//����-5V��-10VУ��ֵ���¼��㹫ʽ
		 	}
			tmpAna2 = tmpAna2 + (int16)Pn[AnaTcrOffset];
			if(tmpAna1 > 1035)// �޷�,�Ͳ�У��������޷�ֵ��ͬ
			{
               tmpAna1 = 1035;
			}
			else if(tmpAna1 < -1035)
			{
			   tmpAna1 = -1035;
			}
	        AnIqr = (long long)tmpAna2 * Iqn  / (10 * (int32)Pn[TCRInsGainADD]);

		}

	/*	if(ColBlock==6)
		{// ��λ0.01bar
			AnPressFeed = (long long)tmpAna2 * Pn[PressFeedDiv] * 705 * 10 / 139264;

			PressFeed_filter.InValue = AnPressFeed;
			PressFeed_filter.Tfilt = Pn[Prfeed_time];
			PressFeed_filter.Tperiod = 1;
			pulsefilter( & PressFeed_filter);
			AnPressFeed_out = PressFeed_filter.OutData;
			temp_x = AnPressFeed_out/10;
			if(temp_x > 2048)
			{
				wAnPressFeed_out = 2048;
			}
			else
			{
				wAnPressFeed_out = temp_x;
			}		

			Ana3 = *EXAna0;
			Ana3 -= 2048;			// 1.5v = 19660,2.5v = 32768
			tmpAna3 = Ana3 - (int16)Pn[An3offset];
			if(tmpAna3 > 2048)
			{
				tmpAna3 = 2048;
			}
			else if (tmpAna3 < -2048)
			{
				tmpAna3 = -2048;
			}

			An3display = (int16)tmpAna3;
			AnPressBack = (long long)tmpAna3 * Pn[PressBackDiv] * 705 * 10 / 139264;
			VCMD = labs(VCMD);

			PressBack_filter.InValue = AnPressBack;
			PressBack_filter.Tfilt = Pn[Prfilt_time];
			PressBack_filter.Tperiod = 1;
			pulsefilter( & PressBack_filter);
			AnPressBack_out = PressBack_filter.OutData;
			temp_x = AnPressBack_out/10;
			if(temp_x > 2048)
			{
				wAnPressBack_out = 2048;
			}
			else
			{
				wAnPressBack_out = temp_x;
			}

			if(membit01.bit.bit00)
			{
				VCMDG = - VCMD;
			}
			else
			{
				VCMDG = VCMD;
			}
		}
		else
		{
			AnIqr = (long long)tmpAna2 * Iqn * 7050 / (139264 * (int32)Pn[TCRInsGainADD]);
		}*/

		TCMD = AnIqr * 100 / Iqn;

		// ģ��Ť���޷�
		if(TCMD > 300)
		{
			TCMD = 300;
		}
		else if(TCMD <- 300)
		{
			TCMD = -300;
		}
		if(membit01.bit.bit00)
		{
			AnIqrg = - AnIqr;
		}
		else
		{
			AnIqrg = AnIqr;
		}
	}
//_______________________________________________________
//________________________________________________________
//
//				����ʵʱ���
//________________________________________________________	

 	if(memGainSet)
	{
		Jdynamic();
	}
//________________________________________________________
	// JOGCOL
	SpeedRef=SPEED/10;
	if(TestPara.Test_Mode || key_flag1.bit.JOGrun || (pbus_stw.bit.JOG && ((memBusSel == 1) || (memBusSel == 2))) || (sci_oper_flag.bit.sciJogson))
	{
		if(TestPara.Test_Mode && !key_flag1.bit.JOGrun){//test mode		
			Test_Program();
			Control_Use();
		}else{
			if(key_flag1.bit.JOGrun || sci_oper_flag.bit.sciJogson)
			{// JOGrunspd
				if(membit01.bit.bit00)
				{
					if(labs(- SPEEDINS - Unspd) > (Pn[VCMPSPDADD]*SpeedUnit))
					{
						pos_flag4.bit.VCMP_COIN = 0;
					}
					else
					{
						pos_flag4.bit.VCMP_COIN = 1;
					}
				}
				else
				{
					if(labs(SPEEDINS - Unspd) > (Pn[VCMPSPDADD]*SpeedUnit))
					{
						pos_flag4.bit.VCMP_COIN = 0;
					}
					else
					{
						pos_flag4.bit.VCMP_COIN = 1;
					}
				}
				if(state_flag2.bit.Son)
				{
					if(key_flag1.bit.JOGpos)
					{
						if(membit01.bit.bit00)
						{
							SPEEDINS = - (int16)Pn[JOGSPDADD];
						}
						else
						{
							SPEEDINS = (int16)Pn[JOGSPDADD];
						}
					}
					else if(key_flag1.bit.JOGneg)
					{
						if(membit01.bit.bit00)
						{
							SPEEDINS = (int16)Pn[JOGSPDADD];
						}
						else
						{
							SPEEDINS = - (int16)Pn[JOGSPDADD];
						}
					}
					else
					{
						SPEEDINS = 0;
					}
					SPEEDINS *= SpeedUnit;
					LmtSpd();
					SModule4();
					//b	NextAnaSC	; 	
					NextAnaSC();	// ����н��ж�
				}
				else
				{
					x_speed = 0;
					SPEEDINSout = 0;
					SPEEDINS = 0;
					SPEED = 0;
					//b	exit3
				}					
			}
			else
			{
				if(state_flag2.bit.Son)
				{
					SPEEDINS =(int16)Pn[PBUSJOGSPD];
					if(membit01.bit.bit00)
					{
						SPEEDINS = -SPEEDINS;
					}
					SPEEDINS *= SpeedUnit;
					LmtSpd();
					SModule4();
					NextAnaSC();
				}
				else
				{
					x_speed = 0;
					SPEEDINSout = 0;
					SPEEDINS = 0;
					SPEED = 0;
					//b	exit3
				}
			}
		}
	}
	else if( key_flag1.bit.JInertia || sci_oper_flag.bit.sciStartInertia)
	{// ��ʶ��
		if(!int_flag3.bit.J_End && state_flag2.bit.Son)
		{
			if(!int_flag3.bit.J_initia)
			{
				Jcount = 0;
				SUMIqr = 0;
				Jiner[0] = 0;
				Jiner[1] = 0;
				Jiner[2] = 0;
				Jiner[3] = 0;
				Jcnt = 0;
				int_flag3.bit.J_initia = 1;				
			}
			else
			{
				switch(Jcount)	
				{
					case 0:
						SPEEDINS = 1000 *SpeedUnit;
						break;
					case 1:
						SPEEDINS = -1000 *SpeedUnit;
						break;
					case 2:
						SPEEDINS = 1000 *SpeedUnit;
						break;
					case 3:
						SPEEDINS = -1000 *SpeedUnit;
						break;
					default:
						break;
				}
				if (Jcnt >= 5000)
				{
					SPEEDINS = 0;
				}

				SoftTime2(5000,5000);
				SPEED = SPEEDINSout;
				SModule3();		        // �������Ӧ���޷���ֵ
				SModule2();        		// �ٶ�PI����
				limitIqr();         	// ������޷���ֵ
				SModule1();         	// ת���˲�
				if (Jcnt >= 10000)
				{
					temp_x = labs(SUMIqr) / Jcnt;
					temp_x = temp_x << 12;
					temp_x = temp_x / Iqm >> 4;
					aveTor = temp_x * varFactualTn;
					temp_x = (aveTor * 60 * 500 / 628) >> 8;	// 500ms����ʱ��
					Jiner[Jcount] = temp_x;
					Jcount++;
					if(Jcount >= 4)
					{
						J_inertia = (Jiner[0] + Jiner[1] + Jiner[2] + Jiner[3])>>2;
						int_flag3.bit.J_End = 1;
						Jx = J_inertia;				
						CommInertia = J_inertia;
						perJload = (int32)((int16)(Jx - JMotor)) * 100 / JMotor;
						if(perJload<0)
						{
							perJload = 0;
						}
					}
					Jcnt = 0;
					SUMIqr = 0;
				}
				else
				{
					if (Jcnt < 5000)
					{
						SUMIqr += Iqr;
					}
					else
					{
						SUMIqr -= Iqr;
					}
					Jcnt++;
				}
			}
		}
		else
		{
			x_speed = 0;
			SPEEDINSout = 0;
			SPEEDINS = 0;
			SPEED = 0;			
		}
	}
	else
	{
	// ConTrol_Sel
		if(state_flag2.bit.nBRKsoff)
		{// OnlyFan
			SPEEDINSout = 0;			
			//b	NextAnaSC	; ����н��ж�
			NextAnaSC();
		}
		else if(state_flag2.bit.nBRKzcl && (labs(speed) > 15*SpeedUnit))
		{
			SPEEDINSout = 0;
			NextAnaSC();
		}
		else
		{
			if(state_flag2.bit.nBRKzcl)
			{
				SPEEDINSout = 0;
				pos_flag4.bit.PNOTzcl = 1;
			}
			else
			{// ClearZP
				pos_flag4.bit.PNOTzcl = 0;			
			}
			//b	OCustmerCol
			if((memBusSel == 3)) //CANopen
			{		
				if(Operation_Mode_Display > 0)
				{
					if(Operation_Mode_Display == HM_MODE)
					{
						CO_HM_mode();			
					}		
					else if(Operation_Mode_Display == PP_MODE)
					{
						CO_PP_mode();
					}
					else if(Operation_Mode_Display == IP_MODE)
					{
						CO_IP_mode();
					}
					else if(Operation_Mode_Display == PV_MODE)
					{
						CO_PV_mode();
					}
					else// if(Operation_Mode_Display == PV_MODE)
					{
						inPgcnt = 0;
						PosSPEEDINS = 0;
						PosSPEEDINSout = 0;
						SPEED = 0;
						SModule2();		//        ; �ٶ�PI����
						limitIqr();		//        ; ������޷���ֵ
						SModule1();		//        ; ת���˲��ӳ���
					}
				}
				else
				{
					inPgcnt = 0;
					PosSPEEDINS = 0;
					PosSPEEDINSout = 0;
					SPEED = 0;
					SModule2();		//        ; �ٶ�PI����
					limitIqr();		//        ; ������޷���ֵ
					SModule1();		//        ; ת���˲��ӳ���
				}
			}
			else if(ColBlock == 0)
			{
				PControl();
			}
			else if(ColBlock == 1)
			{
				AnalogSpdCol();
			}
			else if(ColBlock == 2)
			{
				AnalogTcrCol();
			}
			else if(ColBlock == 3)
			{
				SJDControl();
			}
			else if(ColBlock == 4)
			{
				JDPosCol();
			}
			else if(ColBlock == 5)
			{
				ParSpd();
			}
			else if(ColBlock == 6)
			{
				ServoPump();
			}
			else
			{
				PControl();
			}
		}
	}
	// exit3
	// normbitcode
	SIqr += Iqr;
	if(Iqrcount++ >= 1500)
	{
		temp_x = SIqr / Iqrcount;
		Tn = temp_x *100 / Iqn;  //Iqn额定电流

		if (abs(temp_x *100%Iqn) >= Iqn/2)
        {
            if (Tn >= 0) Tn++;
			else Tn--;
        }

		SIqr = 0;
		if(membit01.bit.bit00)
		{
			Tn = -Tn;
		}
		Iqrcount = 0;
	}
	if(Pn[MotorTab])
	{
		Torque = ((int32)Iqr * varFactualTn)/Iqn;        //扭矩
	}
	else
	{
		Torque = ((int32)Iqr * MotorPar1[memDriveSel][memMotorSel][3])/Iqn;
	}
	// ���ٶ���ƽ��	normTn1
#if 1
	SUnspd += speed;
	if(Unspdcnt++ >= 1500)
	//if(Unspdcnt++ >= 700)
	{
		Unspd = SUnspd/Unspdcnt;
		if(labs(SUnspd%1500) >= 750)  //4舍5入
		{
			if(SUnspd > 0)
			{
				Unspd ++;
			}
			else
			{
				Unspd --;
			}
		}
		SUnspd = 0;
		Unspdcnt = 0;

		//Unspd =speed;

		if(membit01.bit.bit00)
		{
			Unspd = -Unspd;
		}

		//watchUnspd=Unspd;
	}
#endif
	if(memBusSel == 3)
	{
		if(membit01.bit.bit00)
		{
			Vfact = -SPEED;
		}
		else
		{
			Vfact = SPEED;
		}
	}
	else if(ColBlock == 2)
	{
		if(membit01.bit.bit00)
		{
			Vfact = -Iqr;
		}
		else
		{
			Vfact = Iqr;
		}
	}
	else
	{
		if(membit01.bit.bit00)
		{
			Vfact = -SPEED;
		}
		else
		{
			Vfact = SPEED;
		}
	}

	Vfactcomm = speed/SpeedUnit;
	if(membit01.bit.bit00)
	{
		Vfactcomm = - Vfactcomm;
	}
	if(labs(Vfactcomm) >= 15)
	{
		Vfact = Vfactcomm;
	}
	VCMDcomm = VCMD/SpeedUnit;
	// �Ը����ٶ���ƽ��
	SUnVCMD += VCMD;
	if(UnVCMDcnt ++ >= 1500)
	{
		UnVCMD = SUnVCMD / UnVCMDcnt;
		UnVCMDcnt = 0;
		SUnVCMD = 0;
	}

	// ��ƫ���������ֵ��ƽ��
	if(llabs((long long)SUnEk + Ek) > 0x7FFFFFFF)
	{
		UnEk = Ek;
		SUnEk = 0;
		UnEkcnt = 0;
	}
	else
	{
		SUnEk += Ek;
		if( ++UnEkcnt >= 1500)
		{
			UnEk = SUnEk/UnEkcnt;
			SUnEk = 0;
			UnEkcnt = 0;
		}
	}
}

void ControlSel(void)
{
	switch(memhex01.hex.hex01)
	{
		case 0:		// AnSpdCol
			ColBlock = 1;
			if(input_state.bit.PCON)
			{
				int_flag3.bit.SpdLoopPmode = 0;	// P
			}
			else
			{// IntMode
				int_flag3.bit.SpdLoopPmode = 1;	// PI
			}
			break;
		case 1:		// PosCol
			ColBlock = 0;
			GearFlag = input_state.all & SwGearin;
			if(membit01.bit.bit03 && SwGearin)
			{
				if(membit02.bit.bit00)
				{// Change1
					if(GearFlag)
					{// ChangeGear1
						Agear = memAgear1;
					}
					else
					{
						Agear = memAgear2;
					}
					LastGearFlag = GearFlag;
				}
				else
				{
					if(LastGearFlag)
					{// ChangeGear1
						Agear = memAgear1;
					}
					else
					{// ChangeGear2
						Agear = memAgear2;
					}
					LastGearFlag = GearFlag;
				}
				// 
				int_flag3.bit.SpdLoopPmode = 0;
			}
			else
			{
				if(input_state.bit.PCON)
				{
					int_flag3.bit.SpdLoopPmode = 0;
				}
				else
				{
					int_flag3.bit.SpdLoopPmode = 1;
				}
			}
			break;
		case 2:		// TcrCol
			ColBlock = 2;
			break;
		case 3:		// SJDControl0
			ColBlock = 3;
			break;
		case 4:		// SJDAnSpd
			if(input_state.bit.PCON && input_state.bit.NCL && input_state.bit.PCL)
			{
				ColBlock = 1;
			}
			else
			{// JDCol1
				ColBlock = 3;
			}
			break;
		case 5:		// SJDPos
			if(input_state.bit.PCON && input_state.bit.NCL && input_state.bit.PCL)
			{
				ColBlock = 0;
			}
			else if (home_flag4.bit.HM_flag == 1)
			{
				ColBlock = 0;
			}
			else
			{// JDCol1
				if (home_flag4.bit.HM_flag == 0)
				{
					ColBlock = 3;
				}
			}
			break;
		case 6:		// SJDTcr
			if(input_state.bit.PCON && input_state.bit.NCL && input_state.bit.PCL)
			{
				ColBlock = 2;
			}
			else
			{// JDCol1
				ColBlock = 3;
			}
			break;
		case 7:		// PosAnSpd
			if(input_state.bit.PCON)
			{
				ColBlock = 0;
			}
			else if (home_flag4.bit.HM_flag == 1)
			{
				ColBlock = 0;
			}
			else
			{// SwitchAn2
				if (home_flag4.bit.HM_flag == 0)
				{
					ColBlock = 1;
				}
			}
			break;
		case 8:		// PosTcr
			if(input_state.bit.PCON)
			{
				ColBlock = 0;
			}
			else if (home_flag4.bit.HM_flag == 1)
			{
				ColBlock = 0;
			}
			else
			{// SwitchAn2
				if (home_flag4.bit.HM_flag == 0)
				{
					ColBlock = 2;
				}
			}
			break;
		case 9:		// TcrAnSpd
			if(input_state.bit.PCON)
			{
				ColBlock = 2;
			}
			else
			{// SwitchAn2
				ColBlock = 1;
			}
			break;
		case 10:	// AnSpdZero
			if(input_state.bit.PCON)
			{
				ColBlock = 1;
				pos_flag4.bit.PCONzcl = 0;
			}
			else
			{// SwitchAn2
				ColBlock = 1;
				pos_flag4.bit.PCONzcl = 1;
			}
			break;
		case 11:	// PosinPhobit
			if(input_state.bit.PCON)
			{
				ColBlock = 0;
				pos_flag4.bit.InhibitPulseIn = 0;
			}
			else if (home_flag4.bit.HM_flag == 1)

			{
				ColBlock = 0;
			}
			else
			{// SwitchAn2
				if (home_flag4.bit.HM_flag == 0)
				{
					ColBlock = 0;
					pos_flag4.bit.InhibitPulseIn = 1;
				}
			}
			break;
		case 12:	// JPos
			ColBlock = 4;
			break;
		case 13:
			ColBlock = 5;	// ParSpd
			break;
		default: 
			ColBlock = 6;	// servo pump
			break;
	}
}
