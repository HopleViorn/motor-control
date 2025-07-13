
//--------------------------------------------------------------------------------------
// File:	 globalvar.h
// ����������ȫ�ֱ�������
// 
// �����ˣ�yzhua
// ���ڣ�2007-2-14
//--------------------------------------------------------------------------------------
// History:
// 	1.�޸��ˣ�yzhua	
// 	  ��  �ڣ�
//    ��  ������
//--------------------------------------------------------------------------------------
#ifdef GLOBALS
#define EXT
#else
#define EXT extern
#endif
typedef	struct	_TIMER{				//�������ṹ
				Uint16				cnt20ms;			//20ms������
				Uint16				BrakeTime;			//�ƶ���ʱ����
				Uint16				asyn_counter;		//�첽�϶���ʱ��
				Uint16				SCI_RECEIVE_cnt;	//���ռ�ʱ��
				Uint16				SCI_TRANSMIT_cnt;	//���ռ�ʱ��
				Uint16				SCI_commprd;
				Uint16				SON_Count;			// SON�ȴ�ʱ�������
				Uint16				SONOFF_Count;		// SON�ȴ�ʱ�������
				Uint16				INT_Count;			// �жϼ�ʱ
				Uint16				LV_Count;			// ��Դ�쳣��ʱ
				Uint16				RT_Count;			// �����Դ��ʱ��
				Uint16				InFilterCount;		// input filter timer
				Uint16				AlminFilterCount;	// judge ALM input filter timer
				Uint16 				Rescnt;				// й�ŵ�������ʱ
				Uint16 				cntsum;				// 
				Uint16 				Pumpcnt;			// й����ʱ
				Uint16 				SumPump;
				Uint16				judgeRScnt;			// 20ms
				Uint16				LostPhasecnt;		// 
				Uint16				PowerONcnt;			// c22ms
				Uint16				PowerONZQDDcnt;		// �������ϵ��ź�ʱ˲ͣ������־ά�ֵ�ʱ��(c24ms)
				Uint16				PowerONFinishcnt;	// c2s
				Uint16				LOSTRSALMcnt;		// c23ms
				Uint16				PowerOffcnt;		// offtime
				Uint16				cputon;				// �̵�����Ȧͨ����ŷ�֮ͨ�����ʱ
				Uint16				cntRELAY;			// �ƶ���̵�����ʱһ��ʱ���ͷ�
				Uint16				BRKcnt;				// BRKTIM
				Uint16				BRKWAIcnt;			// BRKWAI
				Uint16				SONWAIcnt;			// SONWAI
				Uint16				cntdone;
				Uint32				STEPTME;			// ����ʱ��
				Uint16				Fndcnt; 			// �Ҳο��������г̿����Ժ����ʱ
				Uint16				Un100ms;
				//Uint16				CommDelayTime;		// ͨѶ���͹ر���ʱ��ʱ��
				Uint16				DPRAM_HS_Delay;		// DPRAM shake init delay timer
				Uint16				DPRAM_HS_ScanCnt;	// DPRAM ����ɨ��timer���������û�ɹ�����ÿ1������1�����ֳ�ʼ����ֱ�����ֳɹ�
				//Uint16				DPRAM_HS_StartCnt;	// DPRAM����������������������������10���򱨾����ִ�ֹͣ�������֣�����ñ�����ͬʱ����ü�����
				Uint16				DPRAMISR_ScanCnt;	// DPRAM ISRɨ��timer
				Uint16				lessEncoderTime;
				Uint16				SCIRxfreecnt;		// SCI���ܿ��м�����
				Uint16				PosCOINtimer;		// ��λʱ�������
				Uint16				CAN_Count;
				Uint16				wEncTimer;
				Uint16				RESTimer;
				Uint16				RESTimer1;
				Uint16				ChargeTimer;
		}TIMER;
typedef struct {
				int32		pbus_SPEEDorIqr;
				int16		pbus_Pgerr;
				//int16		pbus_Iqr;
				int16		pbus_input;
				int32		pbus_PGout;
				int16		pbus_STW;
				int16		pbus_STWnew;
				int16		pbus_STWold;
				int16		pbus_ZSW;
        }PROFIBUS_BUF;

//test data
typedef	struct {		
			unsigned int	Begin;
			unsigned int	TestFinish;
			unsigned int	ScanFinish;
			unsigned int	Period;	
			unsigned long	Encoder;			
		}TEST_DATA;
//test:----------------------------------------------
//parameter for position and speed
typedef	struct {		
			unsigned int	Test_Wave;		//0x1100h
			unsigned int	Distance_H;		//0x1101h
			unsigned int	Distance_L;		//0x1102h	
			unsigned int	Velocity;		//0x1103h
			unsigned int	Acceleration;	//0x1104h
			unsigned int	Frequency;		//0x1105h	
			unsigned int	RepeatTimes;	//0x1106h
			unsigned int	RunTime;		//0x1107h
			unsigned int	Sonoff;			//0x1108h
			unsigned int	CWCCW;			//0x1109h
			unsigned int	StopTime;		//0x110Ah
			unsigned int	Start_Test;		//0x110Bh
			unsigned int	Test_Mode;		//0x110Ch
			unsigned int	Res1;		//0x110Dh
			unsigned int	Res2;			//0x110Eh
			unsigned int	Res3;			//0x110Fh
			unsigned int	Scan_Time;		//0x1110h
			unsigned int	Scan_Number;	//0x1111h
							
		}TEST_PARA;
//Position----------------------------------------------
//data
typedef	struct {
			unsigned int	Stop_Step;	//				
			unsigned int	Repeat_Time;
			unsigned int	Count_Time;
			unsigned int	Integral_Time;//
			int	Velocity;
			int	PulseInc;	//
			signed long	Remain;
			signed long	Sin_Distance;
			signed long	PulseSum;		//
			signed long	Distance_All;	//				
		}POSITION_DATA;
typedef struct {
				Uint16 Home_Timeout;
				int32 Home_offset_In;		// ���ƫ���ڲ���λ
				Uint16 HomeStatus;
					// NOT_ARRIVAL_PN	0
					// ARRIVAL_PN		1
					// LEAVE_PN			2	
					// HOME_END			3
				Uint16 PPStatus;
				Uint16 PVStatus;
		}CANopen402_REGS;
// ����
#ifdef GLOBALS
EXT volatile Uint16 Profitime		= 0;
EXT volatile Uint16 StateBit[2]		= {0,0};
												//BitStatus[1]  
												//	7:���Ƶ�Դ
												//	6:����(soff)
												//	5:
												//	4:
												//	3:
												//	2:
												//	1:����
												//	0:�ٶ�һ��/��λ���
												//BitStatus[0] 
												//	7:����
												//	6:ָ���ٶ�/����������
												//	5:
												//	4:
												//	3:ת��ָ��������/����ź�������
												//	2:
												//	1:
												//	0:TGON
EXT volatile Uint16 INTtimer		= 0; 
EXT volatile Uint16 CheckSum		= 0; 
EXT volatile Uint16 LedSegment[5]	= {0,0,0,0,0};
EXT volatile Uint16 LedSelect		= 0;
EXT volatile Uint16 LedIndex		= 0;
EXT volatile Uint16 alarmno			= 0;
EXT volatile Uint16 EncStatus		= 0; 
EXT volatile Uint16 inUVW			= 0;

EXT volatile Uint16 MpInA			= 0; 
EXT volatile Uint16 MpInB			= 0;
EXT volatile Uint16 MpInC			= 0; 
EXT volatile int16  MpBuffA			= 0; 
EXT volatile int16  MpBuffB			= 0; 

EXT volatile Uint32 McInA			= 0; 
EXT volatile Uint32 McInB			= 0; 
EXT volatile int32  McBuff			= 0; 
EXT volatile Uint16 McHigh			= 0; 
EXT volatile Uint16 McOEF			= 0;
EXT volatile int32 Mp				= 0;
EXT volatile int32 Mc				= 0;
EXT volatile int32 speed			= 0;		// �����ٶ�
EXT volatile int32 sec_speed		= 0;

EXT volatile int16 AVcount			= 0;
EXT volatile int32 AVref			= 0;
EXT volatile int32 Ana1				= 0;
EXT volatile int16 ATcount			= 0;
EXT volatile int32 ATref			= 0;
EXT volatile int32 Ana2				= 0;
EXT volatile int16 spdoffset		= 0;
EXT volatile int16 TCRoffset		= 0;
EXT volatile int32 Ana3				= 0;
EXT volatile int32 Sspd				= 0;
EXT volatile int32 Stcr				= 0;
EXT volatile int32 Scur1			= 0;
EXT volatile int32 Scur2			= 0;

EXT volatile Uint16 Au5561[8]		= {0,0,0,0,0,0,0,0};
EXT volatile Uint32 Position		= 0;
EXT volatile int32 absPos[2]		= {0,0};
EXT volatile int32 incPos			= 0;	 
EXT volatile Uint16 Rotate			= 0;	 

EXT volatile Uint32 theta			= 0;

// ���������˵���
EXT volatile Uint16 MenuLayer		= 0;
EXT volatile Uint16 MainMenuPage	= 0;
// ����������ҳ�棩
EXT volatile Uint16 Pn[SumAddr+1];  
EXT volatile Uint16 Un[UnMax+1]		= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
EXT volatile int16 PnNum			= 0;
EXT volatile int16 PnNumRange		= 0;
EXT volatile int16 UnNumRange		= 0;
EXT volatile int16 FnNumRange		= 0;
// ����������ҳ�棩
EXT volatile int16 UnNum			= 0;
// ��������������ҳ�棩
EXT volatile int16 FnNum			= 0;
EXT volatile Uint16 Fn000Index		= 0;
EXT volatile Uint16 Fn004Index		= 0;
EXT volatile Uint16 Fn006Index		= 0;
EXT volatile Uint16 Fn013Index		= 0;
EXT volatile Uint16 ResumeIndex		= 0;
EXT volatile Uint16 StartFlag		= 0;
EXT volatile Uint16 FinishFlag		= 0;
EXT volatile Uint16 JogSonFlag		= 0;
// �������༭ҳ�棩
EXT volatile Uint16 EditFlag		= 0;
EXT volatile Uint16 EditIndex		= 0;
EXT volatile Uint16 EditBit			= 0;
EXT volatile int32  EditContent		= 0;

// ������LED��ʾ�� 
EXT volatile Uint16 BcdCode[5]		= {0,0,0,0,0};
EXT volatile Uint16 backcode		= 0;
EXT volatile Uint16 back_cnt		= 0;
EXT volatile Uint16 vendor_cnt		= 0;
// ����������ɨ�輰��ʱ��
EXT volatile Uint16 Timer0			= 0;
EXT volatile Uint16 TimeComp0		= 0;
EXT volatile Uint16 TimeComp1		= 0;
EXT volatile Uint16 TimeComp2		= 0;
EXT volatile Uint16 TimeComp3		= 0;
EXT volatile Uint16 TimeBuff0		= 0;
EXT volatile Uint16 TimeBuff1		= 0;
EXT volatile Uint16 TimeFlag1		= 0;
EXT volatile Uint16 KeyIn			= 0;
EXT volatile Uint16 KeyInBuff		= 0;
EXT volatile Uint16 KeyDelay		= 0;
EXT volatile Uint16 KeyAssignments	= 0;
EXT volatile Uint16 KeyBuff			= 0;
EXT volatile Uint16 KeyTimer		= 0;
EXT volatile Uint16 KeyPressFlag	= 0;
EXT volatile Uint16 KeyHoldFlag		= 0;
EXT volatile Uint16 KeyUndoFlag		= 0;
EXT volatile Uint16 KeyIntoFlag		= 0;
EXT volatile Uint16 HoldCmpLostFlag	= 0; 
EXT volatile Uint16 KeepEbitFlag	= 0;
EXT volatile Uint16 SHIFT_PnNum		= 0;	//��������λ��־
EXT volatile Uint16 lSHIFT_PnNum	= 0;	//��������λ��־
EXT volatile Uint16 SHIFTPNbits		= 0;	//�����ŵ���λλ

EXT volatile Uint16 DoneTimer		= 0;
EXT volatile Uint16 input_buf[2]	= {0,0};
EXT volatile Uint16 almin_buf[2]	= {0,0};
EXT volatile Uint16 in_filter_out[2]= {0,0};
EXT volatile Uint16 almin_filter_out[2]	= {0,0};
EXT volatile Uint16 RES_FAULT		= 0;
EXT volatile Uint16 RES_FAULT_LATCH = 0;
EXT volatile int32  Pgplus			= 0;	// λ���������
EXT volatile int32  UnPg			= 0;	// λ��������ʾ
EXT volatile int16  SPgerr			= 0;	// 12���� Pgerr��
EXT volatile int16  PgSpdcnt		= 0; 	// 12���ڼ�����
EXT volatile int16  PgSpd			= 0;	// ��������ٶ�
EXT volatile Uint16 pumpflag		= 0;
EXT volatile int32  Pspeed			= 0;	// ��λ�����������ǲ�����ٶ�
EXT volatile int16  PulsOut			= 0;
EXT volatile int16  RmPuls			= 0;
EXT volatile Uint16 memDisaSONinput	= 0;	// [0] ���ⲿ/S-ON [1] �ڲ�S/ON
EXT volatile Uint16 memDisaPOTinput	= 0;	// P-OT 
EXT volatile Uint16 memDisaNOTinput	= 0;	// N-OT 
EXT volatile Uint16 memenZQDD		= 0;	// ˲ͣ���� 
EXT volatile Uint16 memStopSel		= 0;	// ֹͣ��ʽ 
EXT volatile Uint16 memDisaClrBiascnt= 0;	// ���ƫ�������
EXT volatile Uint16 memPGDir		= 0;	// CCW,CW 
EXT volatile Uint16 memAnalimit		= 0;	// ģ���ٶ�����
EXT volatile Uint16 memPulseSel		= 0;	// ����������ʽ
EXT volatile Uint16 memPulseLogic	= 0;	// ָ�������߼� 
EXT volatile Uint16 memlmtAna		= 0;	// ģ��������ƹ��� 
EXT volatile Uint16 memTCRFFQ		= 0;	// ת��ǰ������
EXT volatile Uint16 memColSel		= 0;
EXT volatile Uint16 memEnGear		= 0;
EXT volatile Uint16 memAgear1		= 0;
EXT volatile Uint16 memAgear2		= 0;
EXT volatile Uint32 rmAgear			= 0;
EXT volatile Uint16 memGearSwitch	= 0;
EXT volatile Uint16 memPosErrOValm	= 0;
EXT volatile Uint16 memAccDecSel	= 0;	//; �Ӽ��ٷ�ʽ
											//; = 0 б���Ӽ���
											//; = 1 S�Ӽ���
											//; = 2 һ��ָ���Ӽ���
											//; = 3 ����ָ���Ӽ���
EXT volatile Uint16 memPosFiltSel	= 0;
EXT volatile Uint16 memGainSW		= 0;	// �����л�����
EXT volatile Uint16 memSelPPI		= 0;	// P/PI
EXT volatile Uint16 memGainSet		= 0;	// ʵʱ�Զ���������
EXT volatile Uint16 memMotorSel		= 0;
EXT volatile Uint16 memDriveSel		= 0;
EXT volatile Uint16 memPgSel		= 0;
EXT volatile Uint16 memBusSel		= 0;
EXT volatile Uint16 memCN1DEF14		= 0;
EXT volatile Uint16 memCN1DEF58		= 0;
EXT volatile Uint16 memPGDIV		= 0;
EXT volatile Uint16 memLdq			= 0;
EXT volatile Uint16	memcurSamUnit	= 0;	// ��������ϵ��
EXT volatile Uint16 memHzConvs		= 0;	// ����ת��
EXT volatile Uint16 IPMImax			= 0;	// 
EXT volatile Uint16 JMotor			= 0;	// ����������
EXT volatile Uint16 memPoleNum		= 0;
EXT volatile Uint16 memEncNum		= 0;
EXT volatile Uint16 AsynRunFi		= 0;	// �첽�϶�����Ƶ��

EXT volatile Uint16 RatedSpeed		= 0;	// �ת��
EXT volatile Uint16 xMotor[16]		= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
EXT volatile Uint16 yMotor[16]		= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
EXT volatile Uint16 varUqFFCoFF		= 0;	// ����ǰ����ϵ��4.12
EXT volatile Uint16 varEmCoFF		= 0;	// �����Ʋ���ϵ��4.12
EXT volatile Uint16 varAsroateUm	= 0;	// �첽�϶�����
EXT volatile Uint16 varFactualTn	= 0;	// ʵ�ʶŤ��
EXT volatile Uint16 varKu			= 0;	// Ku
EXT volatile Uint16 varMaxSPEED		= 0;	// ������ļ���ת��
EXT volatile Uint16 varExcelNum		= 0;	// ������

EXT volatile Uint16 ColBlock		= 0; 
										// 0: λ���������
										// 1: ģ���ٶȿ���
										// 2: ���ؿ���
										// 3: �ٶȽӵ����
										// 4: �ڲ�λ�ø�������
										// 5: �ڲ����������ٶȿ��� 
EXT volatile Uint16 EncElect		= 0;
EXT volatile Uint16 Agear			= 0;
EXT volatile Uint16 Bgear			= 0;
EXT volatile Uint16 GearFlag		= 0;
EXT volatile Uint16 LastGearFlag	= 0;
EXT volatile Uint16 SwGearin		= 0;

EXT volatile PULSEFILTER Pos_forward_filter	= {0,0,0,0,0,0};
EXT volatile PULSEFILTER One_pulse_filter	= {0,0,0,0,0,0};
EXT volatile PULSEFILTER Two_pulse_filter	= {0,0,0,0,0,0};
EXT volatile PULSEFILTER Tcr_filter			= {0,0,0,0,0,0};
EXT volatile PULSEFILTER One_Jerk_filter	= {0,0,0,0,0,0};
EXT volatile PULSEFILTER Two_Jerk_filter	= {0,0,0,0,0,0};
EXT volatile PULSEFILTER PressBack_filter	= {0,0,0,0,0,0};
EXT volatile PULSEFILTER PressFeed_filter	= {0,0,0,0,0,0};

//EXT volatile Uint16 ;
EXT volatile int32 fun0				= 0;	// S���߳�ֵ
EXT volatile int32 SFiltIn[2]		= {0,0};// �˲�����
EXT volatile int32 SFiltOut			= 0;	// �˲����
EXT volatile Uint16 STIME			= 0;	// S���߼Ӽ���ʱ�䳣��
EXT volatile Uint16 xtime			= 0;	// S���߼Ӽ���ʱ�����
EXT volatile int16 SwGainPg[2]		= {0,0};// �����л�
EXT volatile Uint16 SwGainTimer		= 0;	// �����л�
EXT volatile Uint16 SwGainPgerr		= 0;	// �������л�Ƶ��
EXT volatile int16 Pg[2]			= {0,0};
EXT volatile int16 Pf[2]			= {0,0};// λ�÷��� 
EXT volatile int16 MPg[2]			= {0,0};// ���ٷ���
EXT volatile int16 Pgerr			= 0; 	// {P}λ�ø������   
EXT volatile int16 Pgerro			= 0; 	// {P}ǰ���˲����յ����
EXT volatile int16 JPOSn			= 0;	// ��ŵڼ���λ�ýӵ�
EXT volatile int16 InitJPOSnEND		= 0;	// 
EXT volatile Uint32 remadd			= 0;	// λ�ýӵ��е������ۼ���
EXT volatile int16 Kpos				= 0;	// Pn15
EXT volatile int16 Kpspeed			= 0;
EXT volatile int16 Kispeed			= 0;
EXT volatile int32 inPgcnt			= 0;	// λ�ýӵ�����һ��������������
EXT volatile int16 inPgstep			= 0;	// �ڲ�λ�ø����ڵ�λ�����ڵĲ���
EXT volatile int16 Pferr			= 0; 	// {P}λ�÷������                                       
EXT volatile int16 Pfract			= 0; 	// {P}λ�û������С������
EXT volatile int16 Pfcount			= 0;	// �Է����ļ���
EXT volatile int16 Pfroate			= 0;	// ����߹���Ȧ��
EXT volatile int32 UnCurPos			= 0;	// UnCurPos,UnCurPos+1
EXT volatile int32 UnEk				= 0;	// UnEk+3 UnEk+4
EXT volatile int32 SUnEk			= 0;	// 
EXT volatile int16 UnEkcnt			= 0;
EXT volatile int32 Ek				= 0;	// ��������
EXT volatile int32 RmEk				= 0;
EXT volatile int32 ZeroEk			= 0;	// ��н���λ��ƫ��
EXT volatile int16 ZeroPf			= 0;	// ��н���������
EXT volatile int16 UVWencoder		= 0;
EXT volatile int16 UVWcomm			= 0;
EXT volatile Uint16 UVWalmflag		= 0;	// bit0 ת����UVW��ʼλ���Ƿ����C���� bit1 C���岻����
EXT volatile int16 UVWsector		= 0;
EXT volatile int16 UVWcounter		= 0;
EXT volatile int16 Encoder4time		= 0;
EXT volatile int16 Tn				= 0;
EXT volatile int32 Unspd			= 0;
EXT volatile int16 Unspdcnt			= 0;
EXT volatile int32 SUnspd			= 0;
EXT volatile int32 CMDref			= 0;	// ��������ֵ(������趨����һ��,����λ���ٶ�����)
EXT volatile int32 VCMDBUF			= 0;	// ����PN-OT���ж�
EXT volatile int32 Vfact			= 0;	// �û��涨��ת�ٷ�����ʵ��ת��
EXT volatile int32 Vfactcomm		= 0;	// 
EXT volatile int32 VCMD				= 0;	// �ⲿ���ٶȸ���
EXT volatile int16 VCMDcomm			= 0;	// 
EXT volatile int32 UnVCMD			= 0;	// ����VCMD��ƽ��
EXT volatile int32 SUnVCMD			= 0;
EXT volatile int16 UnVCMDcnt		= 0;
EXT volatile int32 VCMDG			= 0;	// �ڲ����ٶȸ���
EXT volatile int32 TCMD				= 0;	// �ⲿ��ת�ظ���
EXT volatile int32 TCMDG			= 0;	// �ڲ���ת�ظ���
EXT volatile int32 AnIqr			= 0;	// ģ����ת�ظ���
EXT volatile int32 AnIqrg			= 0;	// ģ����ת�ظ���
EXT volatile Uint16 Switchtime		= 0;	// ת�ؿ����ӳ�ʱ��

EXT volatile IN_STATE1 		input_state		= {0};
EXT volatile IN_STATE1 		input_laststate	= {0};
EXT volatile OUT_STATE1 	out_state		= {0};
EXT volatile SERVO_STATE1 	servo_state		= {0};
EXT volatile ALM_STATE1 	alm_state		= {0};
EXT volatile KEY_FLAG1 		key_flag1		= {0};
EXT volatile STATE_FLAG2 	state_flag2		= {0};
EXT volatile STATE_FLAG3 	int_flag3		= {0};
EXT volatile POS_FLAG4 		pos_flag4		= {0};
EXT volatile LOST_STATE1 	lost_flag		= {0};
EXT volatile X_FLAG1 		oldsci_flag		= {0};
EXT volatile INT_FLAGX 		int_flagx		= {0};
EXT volatile ALM_ABSENC		alm_absenc		= {0};
EXT volatile Uint16 Rxbuffer[200]			= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
											   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
											   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
											   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
EXT volatile Uint16 * RxbufferIndex;
EXT volatile Uint32 commvar1		= 0;
EXT volatile Uint32 commvar2		= 0;
EXT volatile Uint16 commprd			= 0;
EXT volatile Uint16 savecnt			= 0;
EXT volatile Uint16 drawcount		= 0;
EXT volatile Uint16 Xbuffer[4]		= {0,0,0,0};
EXT volatile Uint16 loadcnt			= 0;
EXT volatile int32 x_speed			= 0;		// �ٶ�ƫ���
EXT volatile int32 SPEED			= 0;		// �����ٶ�
EXT volatile int32 SPEEDINS			= 0;		// �ٶ�ָ��
EXT volatile int32 SPEEDINSout		= 0;		// �Ӽ������
EXT volatile int32 SPEEDlmt			= 0;
EXT volatile int32 lastacctime		= 0;
EXT volatile int32 resaccinc		= 0;
EXT volatile int32 accinc			= 0;
EXT volatile int32 lastdectime		= 0;
EXT volatile int32 resdecinc		= 0;
EXT volatile int32 decinc			= 0;

EXT volatile int16 PosFiltime		= 0;
EXT volatile int16 Filtime_pos		= 0;
EXT volatile int32	Iqrmax			= 0;	//�������ֵ
EXT volatile int32	Iqrmin			= 0;	//������Сֵ
EXT volatile int32	Iqn				= 0;	//��������Ѿ��˹�1.414
EXT volatile int32	Iqm				= 0;
EXT volatile int32  Iqrbuf			= 0;
EXT volatile int32  Iqx				= 0;
EXT volatile Uint16 Iqrcount		= 0;
EXT volatile int32  SIqr			= 0;
EXT volatile int32  Iqrref			= 0;
EXT volatile int32  Iqr				= 0;
EXT volatile int32 	yIqr[3]			= {0,0,0};
EXT volatile int32 	xIqr[3]			= {0,0,0};
EXT volatile int32 	y1Iqr[3]		= {0,0,0};
EXT volatile int32 	x1Iqr[3]		= {0,0,0};
EXT volatile int32 	y2Iqr[3]		= {0,0,0};
EXT volatile int32 	x2Iqr[3]		= {0,0,0};

EXT volatile int32 	yIqrm[3]		= {0,0,0};
EXT volatile int32 	y1Iqrm[2]		= {0,0};
EXT volatile int32 	y2Iqrm[2]		= {0,0};

EXT volatile int32  TCRfiltinput	= 0;	// ת���˲�����
EXT volatile int32  TCRfiltout		= 0;	// ת���˲����
EXT volatile int32  TCR_x_tfilt		= 0;	// ת���˲�С��
EXT volatile int32  TCRo			= 0;
EXT volatile int16  PulsErr			= 0;	// �����ٶȿ���ʹ�õ�λ����������
EXT volatile int16  RmSPEED			= 0;	// ��������

EXT volatile int32  RmFg			= 0; 	// ����������ָ������ת��ʱ�������ۼ���
EXT volatile Uint16 cacuload_temp	= 0;
EXT volatile Uint16 cacuload_temp1	= 0;
EXT volatile Uint16 factory_load	= 0;
EXT volatile Uint16 abl_load		= 0;
EXT volatile int32 factory_Iqn		= 0;
//�������ж���ر���
EXT volatile int16 AdcLostCount		= 0;	// F2812 ADC ��ʧ������
EXT volatile int16 ImeasAGND		= 0;
EXT volatile int16 ImeasA15			= 0;
EXT volatile int16 ImeasBGND		= 0;
EXT volatile int16 ImeasB15			= 0;
EXT volatile int16 Iu				= 0;                         
EXT volatile int16 Iv				= 0;
EXT volatile int16 Iuoffset			= 0;                           
EXT volatile int16 Ivoffset			= 0;
EXT volatile int16 Iuu				= 0;                           
EXT volatile int16 Ivv				= 0;
EXT volatile int16 PGCNT[2]			= {0,0};// ����������
EXT volatile int16 encincr			= 0;	// ������������
EXT volatile int32 encoder			= 0;	// ���������[0..2500*2-1]
EXT volatile int16 Ia				= 0;
EXT volatile int16 Ib				= 0;
EXT volatile int16 Iaa				= 0;
EXT volatile int16 Ibb				= 0;
EXT volatile int16 Ic				= 0;
EXT volatile int16 Teta				= 0;	// ת�ӵ�����
EXT volatile int16 Sin_Teta			= 0;	// ����ֵ
EXT volatile int16 Cos_Teta			= 0;	// ����ֵ
EXT volatile int16 Ialfa			= 0;	// ��������
EXT volatile int16 Ibeta			= 0;	// ��������
EXT volatile int16 Idr				= 0;	// �����ο�
EXT volatile int16 Id				= 0;	// ��������
EXT volatile int16 Iq				= 0;	// ��������
EXT volatile int16 e_id				= 0;	// ����ƫ��
EXT volatile int16 e_iq				= 0;	// ����ƫ��
EXT volatile int32 x_id				= 0;	// ���ַ���
EXT volatile int32 x_iq				= 0;	// ���ַ���
EXT volatile int16 Kpcurrent		= 0;	// ��������
EXT volatile int16 Kicurrent		= 0;	// ����ʱ�䳣��
EXT volatile int32 Udbuf			= 0;	// 
EXT volatile int32 Uqbuf			= 0;	// 
EXT volatile int16 Ud				= 0;	// ��ѹ
EXT volatile int16 Uq				= 0;	// ��ѹ
EXT volatile int16 Ualfa			= 0;	// ��ѹ
EXT volatile int16 Ubeta			= 0;	// ��ѹ
EXT volatile int16 Ua				= 0;	// ���ѹ
EXT volatile int16 Ub				= 0;	// ���ѹ
EXT volatile int16 Uc				= 0;	// ���ѹ   
EXT volatile int16 Va				= 0;	// 
EXT volatile int16 Vb				= 0;	// 
EXT volatile int16 Vc				= 0;	//
EXT volatile int16 sector			= 0;	// ���� 
EXT volatile int16 t1				= 0;	// ʸ������ʱ��1
EXT volatile int16 t2				= 0;	// ʸ������ʱ��2
EXT volatile int16 taon				= 0;	// CMPR1����
EXT volatile int16 tbon				= 0;	// CMPR2����
EXT volatile int16 tcon				= 0;	// CMPR3����
EXT volatile int32 x_SQRTiq			= 0;	// [(Iqƽ��) - (Iqnƽ��)]�Ļ���
EXT volatile int16 x_SQcount		= 0;	// ������
EXT volatile Uint16 ccount			= 0;	// ����ʸ����ʱ��
EXT volatile Uint16 Tetacnt			= 0;	// ��ʼ��λ 
EXT volatile int16 sina				= 0;	// 
EXT volatile int16 sinb				= 0;	// 
EXT volatile int16 sinc				= 0;	// 

EXT volatile int16 EncNum			= 0;	// ��������
EXT volatile int16 PoleNum			= 0;	// ������
EXT volatile int16 guai_A			= 0;	// ���ؼ���ϵ��
EXT volatile int32 x_ia				= 0;	// �ų������?
EXT volatile int16 e_ia				= 0;	// �ų�������?

EXT volatile int16 delaycounter		= 0;	// input ��ʱ������
EXT volatile int16 delaycounter1	= 0;	// alm ��ʱ������
EXT volatile Uint16 xiaodou_in		= 0;	// input ����������봦���?
EXT volatile Uint16 xiaodou_alm		= 0;	// alm �������ε����봦����

EXT volatile Uint16 alm_buf[2]		= {0,0};
EXT volatile Uint16 in_buf[2]		= {0,0};
EXT volatile Uint16 xiaodou_rusult[2]= {0,0};// �����ĺ����ս��
EXT volatile Uint16 alm_rusult[2]	= {0,0};// ����������ս��
EXT volatile int16 UqFF				= 0;	// ��ѹǰ��
EXT volatile int16 nUq				= 0;	// �����Ʋ���
EXT volatile int16 AsTeta			= 0;	// �첽�϶��Ƕ�
EXT volatile int16 WayExu			= 0;
EXT volatile Uint32 singlePos = 0;			// ��Ȧ����λ��
EXT volatile Uint16 zeroEncoder = 0;	 
EXT volatile Uint16 EncData[EncDataNum] 	= {0,0,0,0,0,0,0};
EXT volatile Uint16 rEncData[EncDataNum]  	= {0,0,0,0,0,0,0};
EXT volatile Uint16 AddrEnc 		= 0;
EXT volatile Uint16 EncState 		= 0;
EXT volatile Uint16 T3INTtimers 	= 0;
EXT volatile Uint16	sEncROM 		= 0;
EXT volatile Uint16	checkROM 		= 0;	// ������������ݺ�У��
EXT volatile Uint16	rst5v 			= 0;
EXT volatile Uint16	PC_POS 			= 0;
EXT volatile Uint16	ioPGC 			= 0;
EXT volatile Uint16	rstEncTimer		= 0;

EXT volatile int32 adjEncoder		= 0;
EXT volatile int16 SPEED_cnt		= 0;
EXT	volatile int16 ErrPos			= 0;
EXT	volatile Uint16 sPGALM			= 0;
EXT	volatile Uint16 sCOMALM			= 0;
EXT	volatile Uint32 ErrCnt1			= 0;
EXT	volatile Uint32 ErrCnt2			= 0;
EXT	volatile Uint32 ErrCnt3			= 0;
EXT	volatile Uint32 Position2		= 0;
EXT	volatile int16 readMax			= 0;

EXT	volatile int32 spgTime[2]		= {0,0};
EXT	volatile int32 spgTimeDelt[2]	= {0,0};
EXT	volatile Uint16 spgINTcnt		= 0;
EXT	volatile int32 StimeDelt		= 0;
EXT	volatile Uint16 sPGTMEread[2]	= {0,0};

EXT	volatile int32 DeltTime[10]		= {0,0,0,0,0,0,0,0,0,0};
EXT	volatile int32 DeltPos[10]		= {0,0,0,0,0,0,0,0,0,0};
EXT	volatile int32 tspeed[10]		= {0,0,0,0,0,0,0,0,0,0};
EXT	volatile int32 fact_speed		= 0;
EXT volatile int16 slid_speed_index = 0;
EXT	volatile int32 slid_speed[64]	= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
									   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
									   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
									   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
EXT	volatile int32 total_slid		= 0;
EXT	volatile int32 Delt_speed[2]	= {0,0};
EXT volatile Uint16 readPGcnt		= 0;
EXT volatile Uint16 bkFlag			= 0;
EXT volatile int32 avslid_speed		= 0;
EXT volatile int32 Scur[2]			= {0,0};
EXT volatile int16 adjCurCnt		= 0;
EXT volatile int32 SGMAIqr			= 0;
EXT volatile int16 avIqrCnt			= 0;
EXT volatile int32 Pos1				= 0;
EXT volatile int32 Pos2				= 0;
EXT volatile int16 RmPosiout		= 0;
EXT volatile int16 PosCnt			= 0;

EXT volatile Uint16 Sm4cnt			= 0;

EXT volatile Uint16 JSoftcnt		= 0;
EXT volatile Uint16 Jcount			= 0;
EXT volatile Uint32 J_inertia		= 0;
EXT volatile Uint16 InertiaPos		= 0;
EXT volatile int32 SUMIqr			= 0;
EXT volatile Uint32 Jiner[4]		= {0,0,0,0};
EXT volatile Uint16 Jcnt			= 0;
EXT volatile int32 aveTor			= 0;
EXT volatile int16 TORn				= 0;
EXT volatile Uint16 CpldEdition		= 0;
EXT volatile Uint16 DP100Edition	= 0;
EXT volatile Uint16 EXAnaEdition	= 0;

EXT volatile int32 DeltSpd			= 0;
EXT volatile int32 lSpeed			= 0;
EXT volatile int32 SgmaSpd[3]		= {0,0,0};
EXT volatile int32 SgmaIq[3]		= {0,0,0};
EXT volatile int32 SgmaIq0			= 0;
EXT volatile int32 SgmaSpd0			= 0;
EXT volatile Uint16 jpcnt			= 0;
EXT volatile Uint16 jncnt			= 0;
EXT volatile Uint16 jpncnt			= 0;
EXT volatile Uint16 jxcnt			= 0;
EXT volatile Uint16 jsgmacnt[3]		= {0,0,0};
EXT volatile int16 Jauto[64]		= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
									   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
									   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
									   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
EXT volatile int32 Jtotal			= 0;
EXT volatile int16 aveJ				= 0;
EXT volatile int32 SgmaveJ			= 0;
EXT volatile int16 useJ				= 0;
EXT volatile int32 aveSgmaSpd[2]	= {0,0};
EXT volatile int32 Sgma5Delt		= 0;

EXT volatile Uint16 Jx				= 0;					// �����������Ĺ���
EXT volatile Uint16 cacuJcnt		= 0;			// �����������
EXT volatile int16 perJload			= 0;			// ���ع����ٷֱ�
EXT volatile Uint16 SWdelay			= 0;
EXT volatile Uint16 SWtime			= 0;
EXT volatile Uint16 SWKposRes		= 0;
EXT volatile Uint16 SWKspdRes		= 0;
EXT volatile Uint16 SWKispdRes		= 0;
EXT volatile Uint16 Tiqr			= 0;
EXT volatile int32 SPEEDACC			= 0;
EXT volatile int32 last10msSPD		= 0;
EXT volatile Uint16 AccCnt			= 0;
EXT volatile int16 Torque			= 0;
EXT volatile int32 SPEEDACC1ms		= 0;
EXT volatile int32 xSPEEDACC1ms		= 0;
EXT volatile int32 last100usSPD		= 0;
EXT volatile Uint16 AccCnt1			= 0;
EXT volatile int32 outputACCFF		= 0;
EXT volatile Uint16 absALMdelay		= 0;
EXT volatile int32 PGrm				= 0;
EXT volatile int16 PgDelt			= 0;
EXT volatile Uint16 sEncErrCnt		= 0;
EXT volatile int32 xPress			= 0;			// ѹ�����ƻ�����
EXT volatile Uint32 GradsOut		= 0;
EXT volatile Uint16 divGrads		= 0;
EXT volatile Uint16 rmGrads			= 0;
EXT volatile int32 AnPressFeed		= 0;
EXT volatile int32 AnPressFeed_out	= 0;
EXT volatile int32 AnPressBack		= 0;
EXT volatile int32 AnPressBack_out	= 0;
EXT volatile int16 wAnPressFeed_out	= 0;
EXT volatile int16 wAnPressBack_out	= 0;
EXT volatile int16 An2display		= 0;
EXT volatile int16 An3display		= 0;

EXT volatile BIT_DISP membit00		= {0};
EXT volatile BIT_DISP membit01		= {0};
EXT volatile BIT_DISP membit02		= {0};
EXT volatile BIT_DISP membit03		= {0};
EXT volatile BIT_DISP membit04		= {0};
EXT volatile BIT_DISP VoltAlmDisBit = {0};
EXT volatile BIT_DISP SoftAlmEnaBit = {0};
EXT volatile BIT_DISP OTEnableBit	= {0};
EXT volatile HEX_DISP memhex00		= {0};
EXT volatile HEX_DISP memhex01		= {0};
EXT volatile HEX_DISP memhex02		= {0};
EXT volatile HEX_DISP memCN1OUTDEF	= {0};
EXT volatile HEX_DISP memCurLoop00	= {0};
EXT volatile HEX_DISP Edition		= {0};

EXT volatile TIMER Timers;
EXT volatile PROFIBUS_BUF profibus_buf= {0,0,0,0,0,0,0,0};
EXT volatile PBUS_STW pbus_stw		= {0};
EXT volatile PBUS_ZSW pbus_zsw		= {0};
EXT volatile int16 AppIndCopy		= 0;			// DPRAM flag
EXT volatile int16 AbIndCopy		= 0;			// DPRAM flag
EXT volatile FIELDBUSSTATUS FieldbusStatus 	= {0};	// �ֳ�����״̬
EXT volatile MODULESTATUS ModuleStatus		= {0};	// ͨ�Ű�״̬
EXT volatile Uint16 DPRAM_HS_StartCnt		= 0;	// DPRAM����������������������������10���򱨾����ִ�ֹͣ�������֣�����ñ�����ͬʱ����ü�����
EXT volatile Uint16 InitialShakehandLength	= 0;
EXT volatile Uint16 password				= 0;	// �����޸Ĳ���Ȩ�� ֻ��= PROFIBUSPASSWORD �ſ����޸�700�Ժ����
EXT volatile int32 syncPGout				= 0;
EXT volatile Uint16 syncprofiTime			= 0;
EXT volatile Uint16 DeltSyncTime			= 0;
EXT volatile int16 DeltSyncPos				= 0;
EXT volatile int16 SyncInStatus				= 0;

//------------------------------------------------------------
EXT volatile SCI_FLAG 		sci_flag		= {0};
EXT volatile SCI_INT_FLAG 	sci_int_flag	= {0};
EXT volatile SCI_DATA		sci_data		= {0};
EXT volatile SCI_OPE_FLAG	sci_oper_flag	= {0};
EXT volatile SCI_OPE_STATUS sci_oper_status	= {0};
EXT volatile DRV_STATUS 	DrvStatus		= {0};

EXT	TXBUFEER TXbuffer;			//���ͻ���ṹ��
EXT	RXBUFEER RXbuffer;			//���ջ���ṹ��
EXT	SXBUFEER SXbuffer;			//�洢���ݻ�����

EXT volatile Uint16 sci_scan_finished		= 0;
EXT volatile Uint16 Hand_Start_Flag			= 0;

EXT volatile Uint16 * wComm;	
EXT volatile Uint16 * rComm;
EXT volatile Uint16 memModbusAdd	= 0;
EXT volatile Uint16 memSCIBRT		= 0;
EXT volatile Uint16 memSCIPTL		= 0;
EXT volatile Uint16 memSCIsel		= 0;
EXT volatile Uint16 ComminitPhase	= 0;
EXT volatile Uint16 CommInertia		= 0;
EXT volatile Uint16 Commpassword	= 0;
EXT volatile Uint16 CommTrigMode	= 0;
EXT volatile int16 CommTrigLevel	= 0;
EXT volatile int16 XRAMerr			= 0;			// !=0 : �ⲿ�����߻��ַ�߻�RAM�쳣
//add
EXT volatile int32 test_abs_position= 0;
EXT	volatile Uint16 Save32bit[4]	= {0,0,0,0};
EXT volatile Uint32 commvar3		= 0;
EXT volatile Uint32 commvar4		= 0;
EXT volatile int32  SpeedRef		= 0;
EXT volatile int32  test_abs_position_more	= 0;
EXT volatile int16  Position_Falg	= 0;
//test
//=============================================================================
EXT volatile	TEST_DATA		TestData = {0,0,0,0,0};
EXT volatile	TEST_PARA		TestPara  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
EXT volatile	POSITION_DATA	TDPosition = {0,0,0,0,0,0,0,0,0,0};

//==================================================================
EXT	volatile int32	Speed[3] = {0,0,0};
EXT volatile int32	SpeedL[4] = {0,0,0,0};
EXT volatile int32	SpeedLRm[4] = {0,0,0,0};
//==================================================================
EXT volatile int16	index_Ohm	= 0;
EXT volatile int32	Sigma_Ohm	= 0;
EXT volatile int16	Motor_Tem	= 0;

EXT volatile Uint16	memABS_RESOLVER = 0;
EXT volatile int16	UnPg_roate = 0;
EXT volatile int16	UnPg_count = 0;
EXT volatile int16	co_abs_roate = 0;
EXT volatile int16	co_abs_count = 0;

//CANopen
EXT volatile CAN_ERROR_REG CANerr_REG		= {0};
EXT volatile Uint16 CANParasSET				= 0;
EXT volatile CANOPEN_CTL CO_Controlword		= {0};
EXT volatile CANOPEN_STATUS CO_Statusword	= {0};
EXT volatile CANOPEN_CTL CO_lastControlword	= {0};
EXT volatile CANOPEN_CTL CO_oldControlword	= {0};
EXT volatile Uint16 CO_DC_command			= 0;
EXT volatile int16 CO_DrvState				= 0;
			// 0 		NotReadyToSwitchOn
			// 1		SwitchOnDisabled
			// 2		ReadyToSwitchOn
			// 3		SwitchedOn
			// 4		OperationEnable
			// 5		QuickStopActive
			// FFFE		FaultActive
			// FFFF		Fault
EXT volatile Uint16 homing_timeout			= 0;	// ms 0:disable
EXT volatile Uint16 already_homed_flag		= 0;
EXT volatile Uint16 rehome_start_flag       = 0;
EXT volatile Uint16 PosH = 0;
EXT volatile Uint16 PosL = 0;
EXT volatile CANopen402_REGS CANopen402_regs= {0,0,0,0,0};
EXT volatile int32 co_abs_position			= 0;	// �ҵ��ο���������λ��
EXT volatile int16 CANinit = 0; 
EXT volatile Uint16 CO_HBcons_error_bit = 0;

EXT volatile Uint16 Syncflag = 0;
EXT volatile Uint16 SyncError_TimeOut = 0;
EXT volatile Uint16 SyncError_TimeEarly = 0;
//EXT volatile Uint16 Syncfirst = 0;

//home mode
EXT volatile int32 Zero_Ek = 0;  //ƫ��������������
EXT volatile Uint16 Zero_Cnt = 0;
EXT volatile BIT_DISP membit16 = {0};
EXT volatile int32 Home_Ek = 0;
EXT volatile int32 RmPosgain = 0;
EXT volatile HOME_FLAG4 home_flag4 = {0};
EXT volatile int32 Vref_Sample = 0; // �ٶ�ģ��������ֵ
EXT volatile int32 Tref_Sample = 0; // ����ģ��������ֵ
EXT volatile int32 Iu_Sample = 0;   // U���������ֵ
EXT volatile int32 Iv_Sample = 0;   // V���������ֵ
EXT volatile int16 SpdPos10 = 0;    // �ٶ�ģ����10V��Ӧ��У��ֵ
EXT volatile int16 SpdPos5 = 0;     // �ٶ�ģ����5V��Ӧ��У��ֵ
EXT volatile int16 SpdZero = 0;     // �ٶ�ģ����0V��Ӧ��У��ֵ
EXT volatile int16 SpdNeg5 = 0;     // �ٶ�ģ����-5V��Ӧ��У��?
EXT volatile int16 SpdNeg10 = 0;    // �ٶ�ģ����-10V��Ӧ��У��ֵ
EXT volatile int16 TcrPos10 = 0;    // ����ģ����10V��Ӧ��У��ֵ
EXT volatile int16 TcrPos5 = 0;     // ����ģ����5V��Ӧ��У��ֵ
EXT volatile int16 TcrZero = 0;     // ����ģ����0V��Ӧ��У��ֵ
EXT volatile int16 TcrNeg5 = 0;     // ����ģ����-5V��Ӧ��У��ֵ
EXT volatile int16 TcrNeg10 = 0;    // ����ģ����-10V��Ӧ��У��ֵ
#else
EXT volatile Uint16 Profitime;
EXT volatile Uint16 StateBit[2];
EXT volatile Uint16 INTtimer;
EXT volatile Uint16 CheckSum; 
EXT volatile Uint16 LedSegment[5];
EXT volatile Uint16 LedSelect;
EXT volatile Uint16 LedIndex;
EXT volatile Uint16 alarmno;
EXT volatile Uint16 EncStatus;
EXT volatile Uint16 inUVW;
EXT volatile Uint16 MpInA; 
EXT volatile Uint16 MpInB;
EXT volatile Uint16 MpInC; 
EXT volatile int16  MpBuffA; 
EXT volatile int16  MpBuffB;
EXT volatile Uint32 McInA; 
EXT volatile Uint32 McInB;
EXT volatile int32  McBuff; 
EXT volatile Uint16 McHigh; 
EXT volatile Uint16 McOEF;
EXT volatile int32 Mp;
EXT volatile int32 Mc;
EXT volatile int32 speed;
EXT volatile int32 sec_speed;
EXT volatile int16 AVcount;
EXT volatile int32 AVref;
EXT volatile int32 Ana1;
EXT volatile int16 ATcount;
EXT volatile int32 ATref;
EXT volatile int32 Ana2;
EXT volatile int16 spdoffset;
EXT volatile int16 TCRoffset;
EXT volatile int32 Ana3;
EXT volatile int32 Sspd;
EXT volatile int32 Stcr;
EXT volatile int32 Scur1;
EXT volatile int32 Scur2;
EXT volatile Uint16 Au5561[8];
EXT volatile Uint32 Position;
EXT volatile int32 absPos[2];
EXT volatile int32 incPos;	 
EXT volatile Uint16 Rotate;	 
EXT volatile Uint32 singlePos;			// ��Ȧ����λ��
EXT volatile Uint16 zeroEncoder;	 
EXT volatile Uint16 EncData[EncDataNum];
EXT volatile Uint16 rEncData[EncDataNum];
EXT volatile Uint16 AddrEnc;
EXT volatile Uint16 EncState;
EXT volatile Uint16 T3INTtimers;
EXT volatile Uint16	sEncROM;
EXT volatile Uint16	checkROM;			// ������������ݺ�У��
EXT volatile Uint16	rst5v;
EXT volatile Uint16	PC_POS;
EXT volatile Uint16	ioPGC;
EXT volatile Uint16	rstEncTimer;
EXT volatile Uint32 theta;
// ���������˵���
EXT volatile Uint16 MenuLayer;
EXT volatile Uint16 MainMenuPage;
// ����������ҳ�棩
EXT volatile Uint16 Pn[SumAddr+1];
EXT volatile Uint16 Un[UnMax+1];
EXT volatile int16 PnNum;
EXT volatile int16 PnNumRange;
EXT volatile int16 UnNumRange;
EXT volatile int16 FnNumRange;
// ����������ҳ�棩
EXT volatile int16 UnNum;
// ��������������ҳ�棩
EXT volatile int16 FnNum;
EXT volatile Uint16 Fn000Index;
EXT volatile Uint16 Fn004Index;
EXT volatile Uint16 Fn006Index;
EXT volatile Uint16 Fn013Index;
EXT volatile Uint16 ResumeIndex;
EXT volatile Uint16 StartFlag;
EXT volatile Uint16 FinishFlag;
EXT volatile Uint16 JogSonFlag;
// �������༭ҳ�棩
EXT volatile Uint16 EditFlag;
EXT volatile Uint16 EditIndex;
EXT volatile Uint16 EditBit;
EXT volatile int32  EditContent;
// ������LED��ʾ�� 
EXT volatile Uint16 BcdCode[5];
EXT volatile Uint16 LedSegment[5];
EXT volatile Uint16 backcode;
EXT volatile Uint16 back_cnt;
EXT volatile Uint16 vendor_cnt;
// ����������ɨ�輰��ʱ��
EXT volatile Uint16 Timer0;
EXT volatile Uint16 TimeComp0;
EXT volatile Uint16 TimeComp1;
EXT volatile Uint16 TimeComp2;
EXT volatile Uint16 TimeComp3;
EXT volatile Uint16 TimeBuff0;
EXT volatile Uint16 TimeBuff1;
EXT volatile Uint16 TimeFlag1;
EXT volatile Uint16 KeyIn;
EXT volatile Uint16 KeyInBuff;
EXT volatile Uint16 KeyDelay;
EXT volatile Uint16 KeyAssignments;
EXT volatile Uint16 KeyBuff;
EXT volatile Uint16 KeyTimer;
EXT volatile Uint16 KeyPressFlag;
EXT volatile Uint16 KeyHoldFlag;
EXT volatile Uint16 KeyUndoFlag;
EXT volatile Uint16 KeyIntoFlag;
EXT volatile Uint16 HoldCmpLostFlag; 
EXT volatile Uint16 KeepEbitFlag;
EXT volatile Uint16 SHIFT_PnNum;
EXT volatile Uint16 lSHIFT_PnNum;
EXT volatile Uint16 SHIFTPNbits;	
EXT volatile Uint16 DoneTimer;
EXT volatile Uint16 input_buf[2];
EXT volatile Uint16 almin_buf[2];
EXT volatile Uint16 in_filter_out[2];
EXT volatile Uint16 almin_filter_out[2];
EXT volatile Uint16 RES_FAULT;
EXT volatile Uint16 RES_FAULT_LATCH;
EXT volatile int32  Pgplus;			
EXT volatile int32  UnPg;			
EXT volatile int16  SPgerr;				
EXT volatile int16  PgSpdcnt; 		
EXT volatile int16  PgSpd;			
EXT volatile Uint16 pumpflag;
EXT volatile int32  Pspeed;				
EXT volatile int16  PulsOut;
EXT volatile int16  RmPuls;
EXT volatile Uint16 memDisaSONinput;	
EXT volatile Uint16 memDisaPOTinput;	
EXT volatile Uint16 memDisaNOTinput;	
EXT volatile Uint16 memenZQDD;			
EXT volatile Uint16 memStopSel;		
EXT volatile Uint16 memDisaClrBiascnt;	
EXT volatile Uint16 memPGDir;		
EXT volatile Uint16 memAnalimit;		
EXT volatile Uint16 memPulseSel;	
EXT volatile Uint16 memPulseLogic;		
EXT volatile Uint16 memlmtAna;			
EXT volatile Uint16 memTCRFFQ;		
EXT volatile Uint16 memColSel;
EXT volatile Uint16 memEnGear;
EXT volatile Uint16 memAgear1;
EXT volatile Uint16 memAgear2;
EXT volatile Uint32 rmAgear;
EXT volatile Uint16 memGearSwitch;
EXT volatile Uint16 memPosErrOValm;
EXT volatile Uint16 memAccDecSel;
EXT volatile Uint16 memPosFiltSel;
EXT volatile Uint16 memGainSW;		
EXT volatile Uint16 memSelPPI;		
EXT volatile Uint16 memGainSet;		
EXT volatile Uint16 memMotorSel;
EXT volatile Uint16 memDriveSel;
EXT volatile Uint16 memPgSel;
EXT volatile Uint16 memBusSel;
EXT volatile Uint16 memCN1DEF14;
EXT volatile Uint16 memCN1DEF58;
EXT volatile Uint16 memPGDIV;
EXT volatile Uint16 memLdq;
EXT volatile Uint16	memcurSamUnit;
EXT volatile Uint16 memHzConvs;	
EXT volatile Uint16 IPMImax;			
EXT volatile Uint16 JMotor;			
EXT volatile Uint16 memPoleNum;	
EXT volatile Uint16 memEncNum; 		
EXT volatile Uint16 AsynRunFi;			
EXT volatile Uint16 RatedSpeed;		
EXT volatile Uint16 xMotor[16];
EXT volatile Uint16 yMotor[16];
EXT volatile Uint16 varUqFFCoFF;		
EXT volatile Uint16 varEmCoFF;		
EXT volatile Uint16 varAsroateUm;	
EXT volatile Uint16 varFactualTn;	
EXT volatile Uint16 varKu;			
EXT volatile Uint16 varMaxSPEED;		
EXT volatile Uint16 varExcelNum;		
EXT volatile Uint16 ColBlock; 
EXT volatile Uint16 EncElect;
EXT volatile Uint16 Agear;
EXT volatile Uint16 Bgear;
EXT volatile Uint16 GearFlag;
EXT volatile Uint16 LastGearFlag;
EXT volatile Uint16 SwGearin;
EXT volatile PULSEFILTER Pos_forward_filter;
EXT volatile PULSEFILTER One_pulse_filter;
EXT volatile PULSEFILTER Two_pulse_filter;
EXT volatile PULSEFILTER Tcr_filter;
EXT volatile PULSEFILTER One_Jerk_filter;
EXT volatile PULSEFILTER Two_Jerk_filter;
EXT volatile PULSEFILTER PressBack_filter;
EXT volatile PULSEFILTER PressFeed_filter;
EXT volatile int32 fun0;	
EXT volatile int32 SFiltIn[2];
EXT volatile int32 SFiltOut;
EXT volatile Uint16 STIME;		
EXT volatile Uint16 xtime;	
EXT volatile int16 SwGainPg[2];	
EXT volatile Uint16 SwGainTimer;
EXT volatile Uint16 SwGainPgerr;
EXT volatile int16 Pg[2];
EXT volatile int16 Pf[2];		 
EXT volatile int16 MPg[2];		
EXT volatile int16 Pgerr; 		  
EXT volatile int16 Pgerro; 		
EXT volatile int16 JPOSn;		
EXT volatile int16 InitJPOSnEND; 
EXT volatile Uint32 remadd;		
EXT volatile int16 Kpos;		
EXT volatile int16 Kpspeed;
EXT volatile int16 Kispeed;
EXT volatile int32 inPgcnt;	
EXT volatile int16 inPgstep;
EXT volatile int16 Pferr; 	                                       
EXT volatile int16 Pfract; 		
EXT volatile int16 Pfcount;		
EXT volatile int16 Pfroate;		
EXT volatile int32 UnCurPos;	
EXT volatile int32 UnEk;		
EXT volatile int32 SUnEk;		
EXT volatile int16 UnEkcnt;
EXT volatile int32 Ek;		
EXT volatile int32 RmEk;
EXT volatile int32 ZeroEk;		
EXT volatile int16 ZeroPf;		
EXT volatile int16 UVWencoder;
EXT volatile int16 UVWcomm;
EXT volatile Uint16 UVWalmflag;			
EXT volatile int16 UVWsector;
EXT volatile int16 UVWcounter;
EXT volatile int16 Encoder4time;
EXT volatile int16 Tn;
EXT volatile int32 Unspd;
EXT volatile int16 Unspdcnt;
EXT volatile int32 SUnspd;
EXT volatile int32 CMDref;			
EXT volatile int32 VCMDBUF;				
EXT volatile int32 Vfact;			
EXT volatile int32 Vfactcomm;		
EXT volatile int32 VCMD;		
EXT volatile int16 VCMDcomm;		
EXT volatile int32 UnVCMD;	
EXT volatile int32 SUnVCMD;
EXT volatile int16 UnVCMDcnt;
EXT volatile int32 VCMDG;		
EXT volatile int32 TCMD;
EXT volatile int32 TCMDG;
EXT volatile int32 AnIqr;	
EXT volatile int32 AnIqrg;
EXT volatile Uint16 Switchtime;
EXT volatile IN_STATE1 		input_state;
EXT volatile IN_STATE1 		input_laststate;
EXT volatile OUT_STATE1 	out_state;
EXT volatile SERVO_STATE1 	servo_state;
EXT volatile ALM_STATE1 	alm_state;
EXT volatile KEY_FLAG1 		key_flag1;
EXT volatile STATE_FLAG2 	state_flag2;
EXT volatile STATE_FLAG3 	int_flag3;
EXT volatile POS_FLAG4 		pos_flag4;
EXT volatile LOST_STATE1 	lost_flag;
EXT volatile X_FLAG1 		oldsci_flag;
EXT volatile INT_FLAGX 		int_flagx;
EXT volatile ALM_ABSENC		alm_absenc;
EXT volatile Uint16 Rxbuffer[200];
EXT volatile Uint16 * RxbufferIndex;
EXT volatile Uint32 commvar1;
EXT volatile Uint32 commvar2;
EXT volatile Uint16 commprd;
EXT volatile Uint16 savecnt;
EXT volatile Uint16 drawcount;
EXT volatile Uint16 Xbuffer[4];
EXT volatile Uint16 loadcnt;
EXT volatile int32 x_speed;	
EXT volatile int32 SPEED;	
EXT volatile int32 SPEEDINS;
EXT volatile int32 SPEEDINSout;
EXT volatile int32 SPEEDlmt;
EXT volatile int32 lastacctime;
EXT volatile int32 resaccinc;
EXT volatile int32 accinc;
EXT volatile int32 lastdectime;
EXT volatile int32 resdecinc;
EXT volatile int32 decinc;
EXT volatile int16 PosFiltime;
EXT volatile int16 Filtime_pos;
EXT volatile int32	Iqrmax;	
EXT volatile int32	Iqrmin;	
EXT volatile int32	Iqn;
EXT volatile int32	Iqm;
EXT volatile int32  Iqrbuf;
EXT volatile int32  Iqx;
EXT volatile Uint16 Iqrcount;
EXT volatile int32  SIqr;
EXT volatile int32  Iqrref;
EXT volatile int32  Iqr;
EXT volatile int32 	yIqr[3];
EXT volatile int32 	xIqr[3];
EXT volatile int32 	y1Iqr[3];
EXT volatile int32 	x1Iqr[3];
EXT volatile int32 	y2Iqr[3];
EXT volatile int32 	x2Iqr[3];
EXT volatile int32 	yIqrm[3];
EXT volatile int32 	y1Iqrm[2];
EXT volatile int32 	y2Iqrm[2];

EXT volatile int32  TCRfiltinput;
EXT volatile int32  TCRfiltout;	
EXT volatile int32  TCR_x_tfilt;
EXT volatile int32  TCRo;
EXT volatile int16  PulsErr;
EXT volatile int16  RmSPEED;	

EXT volatile int32  RmFg; 			
EXT volatile Uint16 cacuload_temp;
EXT volatile Uint16 cacuload_temp1;
EXT volatile Uint16 factory_load;
EXT volatile Uint16 abl_load;
EXT volatile int32 factory_Iqn;
//�������ж���ر���
EXT volatile int16 AdcLostCount;
EXT volatile int16 ImeasAGND;
EXT volatile int16 ImeasA15;
EXT volatile int16 ImeasBGND;
EXT volatile int16 ImeasB15;
EXT volatile int16 Iu;			                               
EXT volatile int16 Iv;				
EXT volatile int16 Iuoffset;	                                    
EXT volatile int16 Ivoffset;		
EXT volatile int16 Iuu;			                                   
EXT volatile int16 Ivv;				
EXT volatile int16 PGCNT[2];		
EXT volatile int16 encincr;			
EXT volatile int32 encoder;			
EXT volatile int16 Ia;					
EXT volatile int16 Ib;				
EXT volatile int16 Iaa;				
EXT volatile int16 Ibb;				
EXT volatile int16 Ic;				
EXT volatile int16 Teta;			
EXT volatile int16 Sin_Teta;		
EXT volatile int16 Cos_Teta;		
EXT volatile int16 Ialfa;		
EXT volatile int16 Ibeta;		
EXT volatile int16 Idr;				
EXT volatile int16 Id;					
EXT volatile int16 Iq;				
EXT volatile int16 e_id;			
EXT volatile int16 e_iq;			
EXT volatile int32 x_id;	
EXT volatile int32 x_iq;			
EXT volatile int16 Kpcurrent;	
EXT volatile int16 Kicurrent;		
EXT volatile int32 Udbuf;			
EXT volatile int32 Uqbuf;			
EXT volatile int16 Ud;				
EXT volatile int16 Uq;				
EXT volatile int16 Ualfa;			
EXT volatile int16 Ubeta;			
EXT volatile int16 Ua;		
EXT volatile int16 Ub;			
EXT volatile int16 Uc;					
EXT volatile int16 Va;			
EXT volatile int16 Vb;				
EXT volatile int16 Vc;		
EXT volatile int16 sector;			
EXT volatile int16 t1;					
EXT volatile int16 t2;
EXT volatile int16 taon;
EXT volatile int16 tbon;
EXT volatile int16 tcon;
EXT volatile int32 x_SQRTiq;
EXT volatile int16 x_SQcount;
EXT volatile Uint16 ccount;
EXT volatile Uint16 Tetacnt;
EXT volatile int16 sina;
EXT volatile int16 sinb;
EXT volatile int16 sinc;
EXT volatile int16 EncNum;
EXT volatile int16 PoleNum;
EXT volatile int16 guai_A;
EXT volatile int32 x_ia;
EXT volatile int16 e_ia;
EXT volatile int16 delaycounter;
EXT volatile int16 delaycounter1;
EXT volatile Uint16 xiaodou_in;
EXT volatile Uint16 xiaodou_alm;
EXT volatile Uint16 alm_buf[2];
EXT volatile Uint16 in_buf[2];
EXT volatile Uint16 xiaodou_rusult[2];
EXT volatile Uint16 alm_rusult[2];
EXT volatile int16 UqFF;
EXT volatile int16 nUq;
EXT volatile int16 AsTeta;
EXT volatile int16 WayExu;
EXT volatile int32 adjEncoder;
EXT volatile int16 SPEED_cnt;
EXT	volatile int16 ErrPos;
EXT	volatile Uint16 sPGALM;
EXT	volatile Uint16 sCOMALM;
EXT	volatile Uint32 ErrCnt1;
EXT	volatile Uint32 ErrCnt2;
EXT	volatile Uint32 ErrCnt3;
EXT	volatile Uint32 Position2;
EXT	volatile int16 readMax;
EXT	volatile int32 spgTime[2];
EXT	volatile int32 spgTimeDelt[2];
EXT	volatile Uint16 spgINTcnt;
EXT	volatile int32 StimeDelt;
EXT	volatile Uint16 sPGTMEread[2];
EXT	volatile int32 DeltTime[10];
EXT	volatile int32 DeltPos[10];
EXT	volatile int32 tspeed[10];
EXT	volatile int32 fact_speed;
EXT volatile int16 slid_speed_index;
EXT	volatile int32 slid_speed[64];
EXT	volatile int32 total_slid;
EXT	volatile int32 Delt_speed[2];
EXT volatile Uint16 readPGcnt;
EXT volatile Uint16 bkFlag;
EXT volatile int32 avslid_speed;
EXT volatile int32 Scur[2];
EXT volatile int16 adjCurCnt;
EXT volatile int32 SGMAIqr;
EXT volatile int16 avIqrCnt;
EXT volatile int32 Pos1;
EXT volatile int32 Pos2;
EXT volatile int16 RmPosiout;
EXT volatile int16 PosCnt;
EXT volatile Uint16 Sm4cnt;
EXT volatile Uint16 JSoftcnt;
EXT volatile Uint16 Jcount;
EXT volatile Uint32 J_inertia;
EXT volatile Uint16 InertiaPos;
EXT volatile int32 SUMIqr;
EXT volatile Uint32 Jiner[4];
EXT volatile Uint16 Jcnt;
EXT volatile int32 aveTor;
EXT volatile int16 TORn;
EXT volatile Uint16 CpldEdition;
EXT volatile Uint16 DP100Edition;
EXT volatile Uint16 EXAnaEdition;

EXT volatile int32 DeltSpd;
EXT volatile int32 lSpeed;
EXT volatile int32 SgmaSpd[3];
EXT volatile int32 SgmaIq[3];
EXT volatile int32 SgmaIq0;
EXT volatile int32 SgmaSpd0;
EXT volatile Uint16 jpcnt;
EXT volatile Uint16 jncnt;
EXT volatile Uint16 jpncnt;
EXT volatile Uint16 jxcnt;
EXT volatile Uint16 jsgmacnt[3];
EXT volatile int16 Jauto[64];
EXT volatile int32 Jtotal;
EXT volatile int16 aveJ;
EXT volatile int32 SgmaveJ;
EXT volatile int16 useJ;
EXT volatile int32 aveSgmaSpd[2];
EXT volatile int32 Sgma5Delt;

EXT volatile Uint16 Jx;
EXT volatile Uint16 cacuJcnt;
EXT volatile int16 perJload;
EXT volatile Uint16 SWdelay;
EXT volatile Uint16 SWtime;
EXT volatile Uint16 SWKposRes;
EXT volatile Uint16 SWKspdRes;
EXT volatile Uint16 SWKispdRes;
EXT volatile Uint16 Tiqr;
EXT volatile int32 SPEEDACC;
EXT volatile int32 last10msSPD;
EXT volatile Uint16 AccCnt;
EXT volatile int16 Torque;
EXT volatile int32 SPEEDACC1ms;
EXT volatile int32 xSPEEDACC1ms;
EXT volatile int32 last100usSPD;
EXT volatile Uint16 AccCnt1;
EXT volatile int32 outputACCFF;
EXT volatile Uint16 absALMdelay;
EXT volatile int32 PGrm;
EXT volatile int16 PgDelt;
EXT volatile Uint16 sEncErrCnt;
EXT volatile int32 xPress;			// ѹ�����ƻ�����
EXT volatile Uint32 GradsOut;
EXT volatile Uint16 divGrads;
EXT volatile Uint16 rmGrads;
EXT volatile int32 AnPressFeed;
EXT volatile int32 AnPressFeed_out;
EXT volatile int32 AnPressBack;
EXT volatile int32 AnPressBack_out;
EXT volatile int16 wAnPressFeed_out;
EXT volatile int16 wAnPressBack_out;
EXT volatile int16 An2display;
EXT volatile int16 An3display;
EXT volatile BIT_DISP membit00;
EXT volatile BIT_DISP membit01;
EXT volatile BIT_DISP membit02;
EXT volatile BIT_DISP membit03;
EXT volatile BIT_DISP membit04;
EXT volatile BIT_DISP VoltAlmDisBit;
EXT volatile BIT_DISP SoftAlmEnaBit;
EXT volatile BIT_DISP OTEnableBit;
EXT volatile HEX_DISP memhex00;
EXT volatile HEX_DISP memhex01;
EXT volatile HEX_DISP memhex02;
EXT volatile HEX_DISP memCN1OUTDEF;
EXT volatile HEX_DISP memCurLoop00;
EXT volatile HEX_DISP Edition;
EXT volatile TIMER Timers;
EXT volatile PROFIBUS_BUF profibus_buf;
EXT volatile PBUS_STW pbus_stw;
EXT volatile PBUS_ZSW pbus_zsw;
EXT volatile int16 AppIndCopy;
EXT volatile int16 AbIndCopy;
EXT volatile FIELDBUSSTATUS FieldbusStatus;
EXT volatile MODULESTATUS ModuleStatus;
EXT volatile Uint16 DPRAM_HS_StartCnt;
EXT volatile Uint16 InitialShakehandLength;
EXT volatile Uint16 password;
EXT volatile int32 syncPGout;
EXT volatile Uint16 syncprofiTime;
EXT volatile Uint16 DeltSyncTime;
EXT volatile int16 DeltSyncPos;
EXT volatile int16 SyncInStatus;

EXT volatile SCI_FLAG 		sci_flag;
EXT volatile SCI_INT_FLAG 	sci_int_flag;
EXT volatile SCI_DATA		sci_data;
EXT volatile SCI_OPE_FLAG	sci_oper_flag;
EXT volatile SCI_OPE_STATUS sci_oper_status;
EXT volatile DRV_STATUS 	DrvStatus;

EXT	TXBUFEER TXbuffer;
EXT	RXBUFEER RXbuffer;
EXT	SXBUFEER SXbuffer;

EXT volatile Uint16 sci_scan_finished;
EXT volatile Uint16 Hand_Start_Flag;

EXT volatile Uint16 * wComm;
EXT volatile Uint16 * rComm;
EXT volatile Uint16 memModbusAdd;
EXT volatile Uint16 memSCIBRT;
EXT volatile Uint16 memSCIPTL;
EXT volatile Uint16 memSCIsel;
EXT volatile Uint16 ComminitPhase;
EXT volatile Uint16 CommInertia;
EXT volatile Uint16 Commpassword;
EXT volatile Uint16 CommTrigMode;
EXT volatile int16 CommTrigLevel;
EXT volatile int16 XRAMerr;
//add
EXT volatile int32 test_abs_position;
EXT	volatile Uint16 Save32bit[4];
EXT volatile Uint32 commvar3;
EXT volatile Uint32 commvar4;
EXT volatile int32  SpeedRef;
EXT volatile int32  test_abs_position_more;
EXT volatile int16  Position_Falg;
//test
//=============================================================================
EXT volatile	TEST_DATA		TestData;
EXT volatile	TEST_PARA		TestPara;
EXT volatile	POSITION_DATA	TDPosition;

//==================================================================
EXT	volatile int32	Speed[3];
EXT volatile int32	SpeedL[4];
EXT volatile int32	SpeedLRm[4];
//==================================================================
EXT volatile int16	index_Ohm;
EXT volatile int32	Sigma_Ohm;
EXT volatile int16	Motor_Tem;

EXT volatile Uint16	memABS_RESOLVER;
EXT volatile int16	UnPg_roate;
EXT volatile int16	UnPg_count;
EXT volatile int16	co_abs_roate;
EXT volatile int16	co_abs_count;
//CANopen
EXT volatile CAN_ERROR_REG CANerr_REG;
EXT volatile Uint16 CANParasSET;
EXT volatile CANOPEN_CTL CO_Controlword;
EXT volatile CANOPEN_STATUS CO_Statusword;
EXT volatile CANOPEN_CTL CO_lastControlword;
EXT volatile CANOPEN_CTL CO_oldControlword;
EXT volatile Uint16 CO_DC_command;
EXT volatile int16 CO_DrvState;
EXT volatile Uint16 homing_timeout;
EXT volatile Uint16 already_homed_flag;
EXT volatile Uint16 rehome_start_flag;
EXT volatile Uint16 PosH ;
EXT volatile Uint16 PosL ;
EXT volatile CANopen402_REGS CANopen402_regs;
EXT volatile int32 co_abs_position;
EXT volatile Uint16 Polarity;
EXT volatile int16 CANinit; 
EXT volatile Uint16 CO_HBcons_error_bit;

EXT volatile Uint16 Syncflag;
EXT volatile Uint16 SyncError_TimeOut;
EXT volatile Uint16 SyncError_TimeEarly;
//EXT volatile Uint16 Syncfirst;
//home mode
EXT volatile int32 Zero_Ek;
EXT volatile Uint16 Zero_Cnt;
EXT volatile BIT_DISP membit16;
EXT volatile int32 Home_Ek;
EXT volatile int32 RmPosgain;
EXT volatile HOME_FLAG4 home_flag4;
EXT volatile int32 Vref_Sample; // �ٶ�ģ��������ֵ
EXT volatile int32 Tref_Sample; // ����ģ��������ֵ
EXT volatile int32 Iu_Sample;   // U���������ֵ
EXT volatile int32 Iv_Sample;   // V���������ֵ
EXT volatile int16 SpdPos10;    // �ٶ�ģ����10V��Ӧ��У��ֵ
EXT volatile int16 SpdPos5;     // �ٶ�ģ����5V��Ӧ��У��ֵ
EXT volatile int16 SpdZero;     // �ٶ�ģ����0V��Ӧ��У��ֵ
EXT volatile int16 SpdNeg5;     // �ٶ�ģ����-5V��Ӧ��У��ֵ
EXT volatile int16 SpdNeg10;    // �ٶ�ģ����-10V��Ӧ��У��ֵ
EXT volatile int16 TcrPos10;    // ����ģ����10V��Ӧ��У��ֵ
EXT volatile int16 TcrPos5;     // ����ģ����5V��Ӧ��У��ֵ
EXT volatile int16 TcrZero;     // ����ģ����0V��Ӧ��У��ֵ
EXT volatile int16 TcrNeg5;     // ����ģ����-5V��Ӧ��У��ֵ
EXT volatile int16 TcrNeg10;    // ����ģ����-10V��Ӧ��У��ֵ   
#endif



