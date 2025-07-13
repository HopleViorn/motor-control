#include "VerInfo.h"
#include "EDBclm.h"			// define and function declare
#include "globalvar.h"		// 


extern Uint16 ResoverBit;
extern Uint16 ABLine;

extern Uint16 WriteData[4];
Uint16 PowerOnInitAng;
Uint16 yyy;
extern const Uint16 MotorPar1[][4][7];
extern const Uint16 MotorPar2[][4][16];
extern const Uint16 MotorPar3[][4][16];
extern const Uint16 MotorPar4[][4][14];
extern  volatile long Home_Offset;
extern  volatile unsigned long Position_Numerator, Position_Divisor;

extern  void WriteFlash(unsigned int pt,unsigned int val);
void init_sci_data(void);
void var_zero(void);
Uint16 tmp1;
extern int16  standSum;

void InitVar(void)
{

    #if	ENC_TYPE == ENC2500
	Uint16 temp;
	#endif
	Uint16 i;
	Uint16 EX55AA,EXAA55;
	long long TMP;




	var_zero();
	SPI_ReadFRAM(0x0000,&Pn[0],SumAddr+1);

	PowerOnInitAng=Pn[325];

    ////////////////////////
    ResoverBit=Pn[332];
    ABLine=Pn[333];

    ///////////////////////


//	SPI_ReadFRAM(56,&standSum,1);
//	if(standSum==0)standSum=13000;
//	standSum=(int16)WriteData[0];

// ����

	#if DRV_TYPE == DRV_400W
    Edition.all = 0x0320;
	#elif DRV_TYPE == DRV_1KW
	Edition.all = 0xd320;
	#elif DRV_TYPE == DRV_5KW
	Edition.all = 0xe320;
	#elif DRV_TYPE == DRV_15KW
	Edition.all = 0xf320;
	#endif

	CpldEdition = * CPLD_Ver;
	EX55AA = *EX_55AA;
	EXAA55 = *EX_AA55;
	if(EX55AA == 0x55AA && EXAA55 == 0xAA55)
	{
		state_flag2.bit.EXrignt = 1;
		EXAnaEdition = *EX_Ver;
	}


	LedSelect=0x0001;
// ����
	home_flag4.bit.Home_State = NOT_ARRIVAL_ORG;
	home_flag4.bit.HM_flag = 0;
	home_flag4.bit.C_Zero = 0;
	Zero_Cnt = 0;
	home_flag4.bit.Ek_flag = 0;
	home_flag4.bit.Homing_flag = 0;
	alarmno = 0;
	sEncROM = 0xAA;
	Pos_forward_filter.InValue = 0;
	Pos_forward_filter.OutData = 0;
	Pos_forward_filter.OutValue = 0;
	Pos_forward_filter.remain = 0;
	Pos_forward_filter.Tfilt = 0;
	Pos_forward_filter.Tperiod = 1;

	One_pulse_filter.InValue = 0;
	One_pulse_filter.OutData = 0;
	One_pulse_filter.OutValue = 0;
	One_pulse_filter.remain = 0;
	One_pulse_filter.Tfilt = 0;
	One_pulse_filter.Tperiod = 1;

	Two_pulse_filter.InValue = 0;
	Two_pulse_filter.OutData = 0;
	Two_pulse_filter.OutValue = 0;
	Two_pulse_filter.remain = 0;
	Two_pulse_filter.Tfilt = 0;
	Two_pulse_filter.Tperiod = 1;

	Tcr_filter.InValue = 0;
	Tcr_filter.OutData = 0;
	Tcr_filter.OutValue = 0;
	Tcr_filter.remain = 0;
	Tcr_filter.Tfilt = 0;
	Tcr_filter.Tperiod = 1;

	One_Jerk_filter.InValue = 0;
	One_Jerk_filter.OutData = 0;
	One_Jerk_filter.OutValue = 0;
	One_Jerk_filter.remain = 0;
	One_Jerk_filter.Tfilt = 0;
	One_Jerk_filter.Tperiod = 1;

	Two_Jerk_filter.InValue = 0;
	Two_Jerk_filter.OutData = 0;
	Two_Jerk_filter.OutValue = 0;
	Two_Jerk_filter.remain = 0;
	Two_Jerk_filter.Tfilt = 0;
	Two_Jerk_filter.Tperiod = 1;

	in_filter_out[0] = 0xFFFF;
	in_filter_out[1] = 0xFFFF;
	almin_filter_out[0] = 0x00FF;
	almin_filter_out[1] = 0x00FF;
	input_buf[0] = 0xFFFF;
	input_buf[1] = 0xFFFF;
	RxbufferIndex = & Rxbuffer[0];
	input_state.all = 0xFFFF;		    // ��ʼ���˿�

	//---------------------------------------------------
	membit00.all = Pn[bitControl00];
	membit01.all = Pn[bitControl01];
	membit02.all = Pn[bitControl02];
	membit03.all = Pn[bitControl03];
	membit04.all = Pn[bitControl04];

	memhex00.all = Pn[hexControl00];
	memhex01.all = Pn[hexControl01];
	memhex02.all = Pn[hexControl02];

	membit16.all = Pn[HomeSet];
	VoltAlmDisBit.all = Pn[VoltAlmDisable];
	SoftAlmEnaBit.all = Pn[SoftAlmEnable];
    OTEnableBit.all = Pn[OT_Enable];

	Bgear				= Pn[BgearADD];
	memAgear1 			= Pn[Agear1ADD];
	memAgear2 			= Pn[Agear2ADD];
	memAccDecSel		= Pn[AccDecSelADD];
	memPosFiltSel		= Pn[PosFiltSelADD];

	memGainSW 			= Pn[SelGainConADD];		// �����л�����
	memSelPPI			= Pn[SelP_PIContiADD];
	memGainSet			= Pn[JMODEset];
	memMotorSel			= memhex01.hex.hex03;

	memCN1DEF14			= Pn[CN1DEF14];
	memCN1DEF58			= Pn[CN1DEF58];
	memCN1OUTDEF.all 	= Pn[CN1OUTDEF];
	if(memCN1OUTDEF.hex.hex00==4 || memCN1OUTDEF.hex.hex01==4 || memCN1OUTDEF.hex.hex02==4)
	{
		state_flag2.bit.HaveBRKout = 1;
	}
	else
	{
		state_flag2.bit.HaveBRKout = 0;
	}
	memCurLoop00.all = Pn[MotorMurselADD];
	memDriveSel = memCurLoop00.hex.hex02;
	memPgSel 	= memCurLoop00.hex.hex00;
	memBusSel	= memhex02.hex.hex00;

	IPMImax	= Pn[IPMImaxADD];		//���������ͺž���   //Pn842 mokuai����
	memcurSamUnit = Pn[curSamUnit];	//���������ͺž���     //560*0.1mV/A  ����ϵ�� 56mV 7*8
	memABS_RESOLVER = * ABS_RESOLVER;	//��CPLD��ȡ����������

	if((memCN1DEF58 & 0xF000)==0x1000)
	{
		SwGearin = 0x80;
	}
	else if((memCN1DEF58 & 0x0F00)==0x0100)
	{
		SwGearin = 0x40;
	}
	else if((memCN1DEF58 & 0x00F0)==0x0010)
	{
		SwGearin = 0x20;
	}
	else if((memCN1DEF58 & 0x000F)==0x0001)
	{
		SwGearin = 0x10;
	}
	else if((memCN1DEF14 & 0xF000)==0x1000)
	{
		SwGearin = 0x08;
	}
	else if((memCN1DEF14 & 0x0F00)==0x0100)
	{
		SwGearin = 0x04;
	}
	else if((memCN1DEF14 & 0x00F0)==0x0010)
	{
		SwGearin = 0x02;
	}
	else if((memCN1DEF14 & 0x000F)==0x0001)
	{
		SwGearin = 0x01;
	}

	if(Pn[MotorTab])
	{
		varUqFFCoFF 		= Pn[UqFFCoFF];
		varEmCoFF	 		= Pn[EmCoFF];
		varAsroateUm		= Pn[AsroateUm];
		varFactualTn		= Pn[FactualTn];
		varKu				= Pn[Ku];
		varMaxSPEED			= Pn[MaxSPEED];
		varExcelNum			= Pn[ExcelNum];

		memHzConvs			= Pn[HzConversADD];
		SPEEDlmt 			= Pn[MaxSpeedADD];//*SpeedUnit;
		SPEEDlmt        =SPEEDlmt *SpeedUnit;
		JMotor				= Pn[JMotorADD];
		Iqn					= (Uint32)Pn[InormalADD];
		Iqm					= (Uint32)Pn[ImagneticADD];
		memPoleNum 			= Pn[PoleNumADD];
		memEncNum			= Pn[EncNumADD];
		AsynRunFi			= Pn[AsynRunFiADD];
		RatedSpeed			= Pn[RatedSpeedADD];
		memLdq				= Pn[MotorLdqADD];
	}
	else
	{
		varUqFFCoFF 		= MotorPar1[memDriveSel][memMotorSel][0];
		varEmCoFF	 		= MotorPar1[memDriveSel][memMotorSel][1];
		varAsroateUm		= MotorPar1[memDriveSel][memMotorSel][2];
		varFactualTn		= MotorPar1[memDriveSel][memMotorSel][3];
		varKu				= MotorPar1[memDriveSel][memMotorSel][4];
		varMaxSPEED			= MotorPar1[memDriveSel][memMotorSel][5];
		varExcelNum			= MotorPar1[memDriveSel][memMotorSel][6];

		memHzConvs			= MotorPar4[memDriveSel][memMotorSel][0];
		SPEEDlmt			= MotorPar4[memDriveSel][memMotorSel][2]*SpeedUnit;
		JMotor				= MotorPar4[memDriveSel][memMotorSel][3];
		Iqn					= MotorPar4[memDriveSel][memMotorSel][4];
		memPoleNum			= MotorPar4[memDriveSel][memMotorSel][5];
		memEncNum			= MotorPar4[memDriveSel][memMotorSel][6];
		AsynRunFi			= MotorPar4[memDriveSel][memMotorSel][7];
		RatedSpeed			= MotorPar4[memDriveSel][memMotorSel][9];
		Iqm					= MotorPar4[memDriveSel][memMotorSel][12];
		memLdq				= MotorPar4[memDriveSel][memMotorSel][13];
	}

	Jx = (Uint32)JMotor * (Uint32)(100 + Pn[perJ]) / 100;
	CommInertia = Jx;

	if((memCurLoop00.hex.hex00 != Encoder_Abs17bit) && (memCurLoop00.hex.hex00 != Encoder_Inc17bit))
	{
		if(Pn[MotorTab])
			Tetacnt = Pn[InitPhaseADD];
		else
			Tetacnt = MotorPar4[memDriveSel][memMotorSel][8];
	}

	for(i = 0; i <= 15; i++)
	{
		if(Pn[MotorTab])
		{
			xMotor[i] = Pn[x_base+i];
			yMotor[i] = Pn[y_base+i];
		}
		else
		{
			xMotor[i] = MotorPar2[memDriveSel][memMotorSel][i];
			yMotor[i] = MotorPar3[memDriveSel][memMotorSel][i];
		}
	}

	backcode = 9;//15;									// ȡ����������
	JPOSn = ((int16)Pn[ProgStartPointADD]-1);		// S-ON�Ժ�λ�����г���ȴ������źŻ���ʱ����
	InitJPOSnEND = 0;

	#if DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW
	 Iqn *= 5792;
	 Iqn >>= 10;				// �ֱ���2.5mA

	 Iqm *= 5792;
	 Iqm >>= 10;				// �ֱ���2.5mA
	#elif DRV_TYPE == DRV_5KW
	 Iqn *= 5792;
	 Iqn >>= 11;				// �ֱ���5mA

	 Iqm *= 5792;
	 Iqm >>= 11;				// �ֱ���5mA
	#elif DRV_TYPE == DRV_15KW
	 Iqn *= 5792;
	 Iqn >>= 12;				// �ֱ���10mA

	 Iqm *= 5792;
 	 Iqm >>= 12;				// �ֱ���10mA
	#endif

	memPGDIV = Pn[PGDIVADD];
	#if ENC_TYPE == ENC17BIT
	 *PG_DIV = (memPGDIV<<2) - 1;
	#elif ENC_TYPE == ENC2500
	 temp = memPGDIV;
	 if(!membit01.bit.bit00)
	 {
	     temp &= 0xEFFF;
	 }
	 else
	 {
	 	 temp |= 0x1000;
	 }

	 if(!membit04.bit.bit00)
	 {
		 temp &= 0xdFFF;
	 }
	 else
	 {
		 temp |= 0x2000;
	 }
	 *PG_DIV = temp;
	#endif

	EncElect = ((Uint32)memEncNum<<2)/memPoleNum;
	if (memCurLoop00.hex.hex00 == Encoder_Abs17bit || memCurLoop00.hex.hex00 == Encoder_Inc17bit)
	{
		memEncNum = 32768;
	}
	else if(memCurLoop00.hex.hex00 == Encoder_Resolver)
	{
		AD2S1210_init();
	}
	Pg[1] = *GPCNT;
	Pf[1] = QPCNT;

	spdoffset = Pn[Fn004Addr];
	TCRoffset = Pn[Fn004Addr+1];
	Iuoffset = Pn[Fn006Addr];
	Ivoffset = Pn[Fn006Addr+1];

	lost_flag.all = 3;

	CANerr_REG.all = 0;
	CO_Controlword.all = 0;
	CO_Statusword.all = 0;
	CO_lastControlword.all = 0;
	CO_oldControlword.all = 0;
	CO_DC_command = 0;
	CO_DrvState = NotReadyToSwitchOnState;

	if(memBusSel == 3)    //NIEKEFU
	{
		if((memCurLoop00.hex.hex00 == Encoder_Abs17bit)
		   &&(!membit02.bit.bit02))
		{
			CO_Statusword.bit.OperSpecific_b15 = Pn[HOMED_FLAG];
			if(CO_Statusword.bit.OperSpecific_b15)
			{
				co_abs_position = ((long)Pn[SinglePosHatZERO])<<16;
				co_abs_position += Pn[SinglePosLatZERO];
				Home_Offset = Pn[HomeOffsetL];
				Home_Offset += (long)Pn[HomeOffsetH] << 16;
				Position_Numerator = ((long)Pn[PosNumH])<<16;
				Position_Numerator += Pn[PosNumL];
				Position_Divisor = ((long)Pn[PosDivH])<<16;
				Position_Divisor += Pn[PosDivL];
				if(membit01.bit.bit00)
				{
					co_abs_position = 0x20000 - co_abs_position; 	
				}

				TMP = co_abs_position - (long long)Home_Offset * Position_Numerator/Position_Divisor;
				co_abs_position = (int32)TMP;	 //
			}
		}
		else
		{
			CO_Statusword.all = 0;
			WriteFlash(HOMED_FLAG,0);
		} 
	}
	
	SetBit(StateBit[1],StateBit1_CPower);	//StateBit[1] |= StateBit1_CPower;	// ���Ƶ�Դ
	HVD33Rxd();								// HVD33����״̬

	init_sci_data();

	if(Pn[SpdflagADD] == 1)
	{
		sci_oper_flag.bit.SciAdSpdEnd = 1;   
		SpdPos10 = Pn[SpdAddr];
		SpdPos5 = Pn[SpdAddr+1];
		SpdZero = Pn[SpdAddr+2];
		SpdNeg5 = Pn[SpdAddr+3];
		SpdNeg10 = Pn[SpdAddr+4];
	  	
	}
	else
	{
       sci_oper_flag.bit.SciAdSpdEnd = 0;
	}
	if(Pn[TcrflagADD] == 1)
	{
        sci_oper_flag.bit.SciAdTcrEnd = 1;        
		TcrPos10 = Pn[TcrAddr];
		TcrPos5 = Pn[TcrAddr+1];
		TcrZero = Pn[TcrAddr+2];
    	TcrNeg5 = Pn[TcrAddr+3];
		TcrNeg10 = Pn[TcrAddr+4];
	}
	else
	{
        sci_oper_flag.bit.SciAdTcrEnd = 0;
	}
}

void init_sci_data(void)
{
// ----------------------------------------------------------------------------
//	ͨѶ��ʼ��
// ----------------------------------------------------------------------------
	memModbusAdd = Pn[ModbusAdderADD];
	memSCIBRT = Pn[SCIParaSADD] & 0x000F;
	memSCIPTL = (Pn[SCIParaSADD] & 0x00F0)>>4;
	memSCIsel = (Pn[SCIParaSADD] & 0x0F00)>>8;
	
	TXbuffer.paddress=(Uint16 * ) TX_BUF;		//���ͻ�������ʼ��ͷָ��
	TXbuffer.pcurrent=TXbuffer.paddress;		//���ͻ�������ǰָ���ʼ��
	TXbuffer.data_length=0;						//��Ч����Ϊ0 ��������

	RXbuffer.paddress=(Uint16 * ) RX_BUF;		//���ջ�������ǰָ���ʼ��
	RXbuffer.pcurrent=RXbuffer.paddress;
	RXbuffer.data_length=0;

	SXbuffer.paddress= (Uint16 * ) SX_BUF;		//���ݴ洢�ռ�
	SXbuffer.pcurrent=SXbuffer.paddress;
	SXbuffer.data_length=0;	
//-------------------Modbus-----------------------------------
	sci_data.sci_state=0;						//��ʹ��ʱ����һ��״̬
	sci_flag.all=0;								//���SCIͨѶFLAG�ṹ
	sci_int_flag.all=0;	
	sci_oper_flag.all = 0;		
	
	sci_data.SCIFourbyteTIME=0;
	if(memSCIsel)
	{
		SCIBRST_init();	
	}
	else
	{
		InitSci();
	}
}

void var_zero(void)
{
 Uint16 *tmp_pt;

	//Rxbuffer[200];
	tmp1 = sizeof(Rxbuffer);
	tmp_pt = (void *)&Rxbuffer;
	while(tmp1)
	{
	    tmp1--;
		*tmp_pt++ = 0;
	}
	RxbufferIndex = &Rxbuffer[0];

	// Timers = 0;
	tmp1 = sizeof(Timers);
	tmp_pt = (void *)&Timers;
	while(tmp1--)
	{
		*tmp_pt++ = 0;
	}
	
	// profibus_buf = 0;
	tmp1 = sizeof(profibus_buf);
	tmp_pt = (void *)&profibus_buf;
	while(tmp1--)
	{
		*tmp_pt++ = 0;
	}

	tmp1 = sizeof(sci_data);
	tmp_pt = (void *)&sci_data;
	while(tmp1--)
	{
		*tmp_pt++ = 0;
	}

	//CANopen
	tmp1 = sizeof(CANopen402_regs);
	tmp_pt = (void *)&CANopen402_regs;
	while(tmp1--)
	{
		*tmp_pt++ = 0;
	}
}

