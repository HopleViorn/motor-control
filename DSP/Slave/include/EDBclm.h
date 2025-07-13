#include "IQmathLib.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "VerInfo.h"

#define RAM_RUN		1 // 0 // 1

#define RESETDPRAMISR		GpioDataRegs.GPADAT.bit.GPIO13
#define HVD33Rxd()			GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1
#define HVD33Txd()			GpioDataRegs.GPBSET.bit.GPIO61 = 1
#define RST_DPRAM_L()		GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
#define RST_DPRAM_H()		GpioDataRegs.GPASET.bit.GPIO29 = 1;

// PG type
#define Encoder_tABCUVW  0	 // ��Ħ����ʡ��ʽ
#define Encoder_tABC	 6   // ��Ħ��ʡ��ʽ
#define Encoder_sABC	 2   // SAMTAK��ʡ��ʽ
#define Encoder_Abs17bit 3
#define Encoder_Inc17bit 4
#define Encoder_Resolver 5
#define EncDataNum		 7
// SCI BUF
#define SAMPLE_BUF	(volatile unsigned int *)0x105000
#define TX_BUF		(volatile unsigned int *)0x107000
#define SX_BUF		(volatile unsigned int *)0x108000
#define RX_BUF		(volatile unsigned int *)0x109000

// �����ַ�������
#define LED			(volatile unsigned int *)0x4000
#define REG_DIV		(volatile unsigned int *)0x4001
#define DRIVE		(volatile unsigned int *)0x4002
#define OUTP		(volatile unsigned int *)0x4002
#define REG_PLS		(volatile unsigned int *)0x4003
#define REG_SPD		(volatile unsigned int *)0x4004	// ���MACHXO�е�QCTMRʵʱ���λ��־
#define PG_ID		(volatile unsigned int *)0x4005
#define PC_REG		(volatile unsigned int *)0x4006
#define PG_DIV		(volatile unsigned int *)0x4007
#define PG_NOP		(volatile unsigned int *)0x4008
#define PC_Enable	(volatile unsigned int *)0x4009
#define PGC_IO		(volatile unsigned int *)0x400A
// �����ַ�����룩
#define QPCNT1		(volatile unsigned int *)0x4000
#define QCPRD		(volatile unsigned int *)0x4001

#define sPGTIME		(volatile unsigned int *)0x4003	// CPLD:MCLAT
#define GPCNT		(volatile unsigned int *)0x4004
#define PG_IN		(volatile unsigned int *)0x4005	// PGEN,PGUVW,ABCBR,PA,PB,PC,UVWBR,PU,PV,PW
#define TOTAL_IN	(volatile unsigned int *)0x4006	// �˿����루���У�PULS&SIGN&PGA&PGB&PGC&PGU&PGV&PGW&INP��
#define PGSTA_ALM	(volatile unsigned int *)0x4007	// ���б�����״̬��ȫ��Ӳ������������PGERR&ABCBR&UVWBR&ALM��
#define QPCNT		((*QPCNT1)&0xFFFC)

#define ADREG0		(volatile unsigned int *)0x400A	//Iu		8
#define ADREG1		(volatile unsigned int *)0x400B	//Iv		9
#define ADREG2		(volatile unsigned int *)0x4009	//Vref		A
#define ADREG3		(volatile unsigned int *)0x4008	//Tref		B

#define SyncQPCNT	(volatile unsigned int *)0x400C	//SYNC SAMPLE PG
#define SyncTime	(volatile unsigned int *)0x400D	//SYNC SAMPLE PG 
#define SyncHTimeADD	(volatile unsigned int *)0x4002	//SYNC time 

//---------------------------------------------------
//����ֵ OR ���� 00:ABS	01:RESOLVER
//---------------------------------------------------
#define	ABS_RESOLVER	(volatile unsigned int *)0x400E
//---------------------------------------------------
//---------------------------------------------------
//			CPLD�汾
//---------------------------------------------------
#define CPLD_Ver	(volatile unsigned int *)0x400F
// �����ַ�����룩
//---------------------------------------------------
//			CPLD����
//---------------------------------------------------
#define AU5561_0	(volatile unsigned int *)0x4010
#define AU5561_1	(volatile unsigned int *)0x4011
#define AU5561_2	(volatile unsigned int *)0x4012
#define AU5561_3	(volatile unsigned int *)0x4013
#define AU5561_4	(volatile unsigned int *)0x4014
#define AU5561_5	(volatile unsigned int *)0x4015
#define AU5561_6	(volatile unsigned int *)0x4016
#define AU5561_7	(volatile unsigned int *)0x4017
//---------------------------------------------------
//			RESOLVER
//---------------------------------------------------
#define AD2S1210_RES	(volatile unsigned int *)0x2000 
#define WrAD2S1210		(volatile unsigned int *)0x2001
#define AD2S1210_SMP	(volatile unsigned int *)0x2002
#define AD2S1210_InOut	(volatile unsigned int *)0x2003

//			SERVO-PUMP
//---------------------------------------------------
#define EXAna0		(volatile unsigned int *)0x200000
#define EXAna1		(volatile unsigned int *)0x200001
#define EXAna2		(volatile unsigned int *)0x200002
#define EXAna3		(volatile unsigned int *)0x200003
#define EXIO		(volatile unsigned int *)0x200004
 
#define EX_55AA		(volatile unsigned int *)0x20000D			// 0x55AA;
#define EX_AA55		(volatile unsigned int *)0x20000E			// 0xAA55;
#define EX_Ver		(volatile unsigned int *)0x20000F			// EXAna-ver;

// CANopen status
#define NotReadyToSwitchOnState 	0
#define SwitchOnDisabledState 		1
#define ReadyToSwitchOnState 		2
#define SwitchedOnState 			3
#define OperationEnableState 		4
#define QuickStopActiveState 		5
#define FaultActiveState 			0xFFFE
#define FaultState 					0xFFFF
//CO_DC_command
#define DC_Shutdown			1
#define DC_SwitchOn			2
#define DC_DisableVol		3
#define DC_QuickStop		4
#define DC_DisableOper		5
#define DC_EnableOper		6
#define DC_Fault			7
//Home mode status
#define NOT_ARRIVAL_PN		0
#define ARRIVAL_PN			1
#define LEAVE_PN			2
#define ARRIVAL_REF			3
#define HOME_END			4
//PV mode status
#define NOT_ARRIVAL_TARGET_VEL	0
#define ARRIVAL_TARGET_VEL		1
// Operation_Mode
#define PP_MODE				1
#define PV_MODE				3
#define HM_MODE				6
#define IP_MODE				7

// ����״̬����
#define NOT_ARRIVAL_ORG		0     //ײ��ԭ���ź�״̬
#define ARRIVAL_ORG			1     //����ԭ���ź�״̬
#define ARRIVAL_ZERO		2     //�뿪ԭ���źŵ���C����״̬
#define ORG_HOME_END		3     //�������״̬

// ����
#define TIMERBASE1	10			// 100us
#define TIMERBASE2	2			// 500us
#define TIME_1MS	(1*TIMERBASE1)
#define TIME_2MS	(2*TIMERBASE1)
#define TIME_10MS	(10*TIMERBASE1)
#define TIME_50MS	(10*TIMERBASE1)
#define TIME_100MS	(100*TIMERBASE1)
#define TIME_200MS	(200*TIMERBASE1)
#define TIME_300MS	(300*TIMERBASE1)
#define TIME_400MS	(400*TIMERBASE1)
#define TIME_500MS	(500*TIMERBASE1)
#define TIME_1000MS	(1000*TIMERBASE1)
#define TIME_2000MS	(2000*TIMERBASE1)

// ����
#define PnMax	345
#define PnSee	260
#define UnMax	21

#if DRV_TYPE == DRV_15KW
	#define UnSee	17
#else
	#define	UnSee	16
#endif

#define FnMax	13
#if ENC_TYPE == ENC17BIT
    #define FnSee	11
#else
    #define FnSee	9
#endif

#define Fn004Addr	PnMax+1
#define Fn006Addr	PnMax+3
#define Fn000Addr	PnMax+5
#define SpdAddr     PnMax+15
#define TcrAddr     PnMax+20
#define SpdflagADD  PnMax+25
#define TcrflagADD  PnMax+26
#define SumAddr		PnMax+27 	
// ����
#define Phase		1250 	
#define DividerNum	2500	
#define SpeedUnit 	10	// 0.1r/min
#define Tspeed 		1	// �ٶȻ�����0.1ms
#define MotorNum	4	// �������
#define sEncoderALMdelay 	1000	// ���б�����������ʱ
#define	resolverALMdelay	1000
#define	resolverNoALMdelay	100
// ����
#define	MODE	0x0001
#define UP		0x0002
#define DOWN	0x0004
#define DATA	0x0008
#define MODAT	0x0009

#define PROFIBUSPASSWORD	2008

#define PWMPERIOD 			5000
#define Time500us			3
#define Reycle_delay		100		// 50ms �̵�����Ȧͨ����ŷ�֮ͨ�����ʱ,50ms����
#define Time_PUMP			500		// �����ŵ粻250ms
//bit display const
#define	StateBit0_TGON		0x02//0x01	// TGON
#define	StateBit0_InTcr		0x40//0x08	// ת��ָ��������
#define	StateBit0_InCLR		0x40//0x08	// ����ź�������
#define	StateBit0_InVref	0x20//0x40	// ָ���ٶ�������
#define	StateBit0_InPuls	0x20//0x40	// ����������
#define	StateBit0_MPower	0x10//0x80	// ����
#define	StateBit1_VCMP_COIN	0x02//0x01	// �ٶ�һ��/��λ���
#define	StateBit1_SVREADY	0x20//0x40	// ����
#define	StateBit1_CPower	0x10//0x80	// ���Ƶ�Դ
#define BIT0	0x0001
#define BIT1	0x0002
#define BIT2	0x0004
#define BIT3	0x0008
#define BIT4	0x0010
#define BIT5	0x0020
#define BIT6	0x0040
#define BIT7	0x0080
#define BIT8	0x0100
#define BIT9	0x0200
#define BIT10	0x0400
#define BIT11	0x0800
#define BIT12	0x1000
#define BIT13	0x2000
#define BIT14	0x4000
#define BIT15	0x8000
#define SetBit(a,b)			(a |= b)
#define ClrBit(a,b)			(a &= (0xFFFF^b))
//#define	StateBit1
//#define	StateBit1
#define AlmLog				0		// ��ʷ������¼
#define LoadDefault			1		// �ָ�����
#define JOGope				2		// JOG����
#define SpdErrAutoAdjust	3		// �ٶ�ָ��ƫ���Զ�����
#define SpdErrManuAdjust	4		// �ٶ�ָ��ƫ���ֶ�����
#define MotorIerrAutoAdjust	5		// ����������ƫ���Զ�����
#define MotorIerrManuAdjust	6		// ����������ƫ���ֶ�����
#define SoftVar				7		// ����汾
#define PosTeach			8		// λ��ʾ��
#define Inertia				9		// ����ʶ��
#define ClrEncMuti			10		// �����Ȧ����
#define ClrEncErr			11		// �������
#define AutoPhase			12		// �Զ�����
#define Adjustview          13      //ģ����У��ֵ��ʾ

#define Un_Unspd			0		// ƽ���ķ����ٶ�
#define Un_UnVCMD			1		// ƽ����ģ���������ٶ�
#define Un_TCMD				2		// ��������
#define Un_Tn				3		// �ڲ�ת�ظ���
#define Un_Encoder			4		// ����������
#define Un_Input			5		// �����
#define Un_PGUVWABC			6		// ����ڸ�4λ
#define Un_OutST			7		// ��������ź�
#define Un_Pgspd			8		// ��������ٶ�
#define Un_CurPosLSB		9		// ��ǰλ��L
#define Un_CurPosMSB		10		// ��ǰλ��H
#define Un_UnEkLSB			11		// ƫ�������L
#define Un_UnEkMSB			12		// ƫ�������H
#define Un_PgLSB			13		// λ�ø���L
#define Un_PgMSB			14		// λ�ø���H
//#define Un_Adc_A			17		//
//#define Un_Adc_B			18		//
//-----------------------------------
#define Pn1FactBeg			0
#define Pn1GropBeg			0

#if ENC_TYPE == ENC2500
    #define	Pn1GropLen		8
#else 
    #define	Pn1GropLen		7
#endif

#define Pn2FactBeg			16
#define Pn2GropBeg			100		// Pn016
#define Pn2GropLen			46

#define Pn3FactBeg			66
#define Pn3GropBeg			200		// Pn066
#define Pn3GropLen			6

#define Pn4FactBeg			80
#define Pn4GropBeg			300		// Pn080
#define Pn4GropLen			23

#define Pn5FactBeg			103
#define Pn5GropBeg			400		// Pn103 
#define	Pn5GropLen			16

#define Pn6FactBeg			120
#define Pn6GropBeg			500		// Pn120
#define	Pn6GropLen			29

#define Pn7FactBeg			152
#define Pn7GropBeg			600		// Pn152
#define	Pn7GropLen			92

#define Pn8FactBeg			252
#define Pn8GropBeg			700		// Pn252
#define	Pn8GropLen			5

#define Pn9FactBeg			272
#define Pn9GropBeg			800		// Pn252
#define	Pn9GropLen			59

#define Pn9ModelBeg			840		// Pn40��������
#define Pn9ModelLen			1		// ������Ҳ����ĳ���
//-----------------------------------
#define JDPOS_OneOrLoopSel	0x0001
#define JDPOS_StartSel		0x0002	
#define JDPOS_StepOrNot		0x0010
#define JDPOS_StartOrNot	0x0020
#define JDPOS_StepValidSel	0x0100
// pn address
#define bitControl00		0		// λ�ؿ���0
#define bitControl01		1		// λ�ؿ���1
#define bitControl02		2		// λ�ؿ���2
#define bitControl03		3		// λ�ؿ���3
#define hexControl00		4		// 16���Ʋ���
#define hexControl01		5		// 16���Ʋ���
#define hexControl02		6		// 16���Ʋ���
#define bitControl04		7		// λ�ؿ���4

#define JMODEset			16		// ʵʱ�Զ���������
#define Rigidity			17		// ���ظ���
#define SpdloopGainADD		18		// �ٶȻ�����
#define SpdloopItimeADD		19		// �ٶȻ�����
#define PosloopGainADD		20		// λ�û�����
#define TCRFiltTimeADD		21		// ת��ָ���˲�ʱ�䳣��
#define perJ				22		// �������ٷֱ�
#define secSpdloopGainADD	23		// �ڶ��ٶȻ�����
#define secSpdloopItimeADD	24		// �ڶ��ٶȻ�����
#define secPosloopGainADD	25		// �ڶ�λ�û�����
#define secTCRFiltTimeADD	26		// �ڶ�ת���˲�ʱ�䳣��
#define VbiaslvADD			27		// �ٶ�ƫ��
#define PosFFFiltADD		28		// λ��ǰ��
#define FFFiltADD			29		// ǰ���˲�
#define SPEEDACCFF			30		// ת��ǰ��
#define TorFFTime			31		// ת��ǰ���˲�ʱ�䳣��

#define SelP_PIContiADD		32		// P/PI�����л�����ѡ��
#define IqrP_PIADD			33		// ת�ذٷֱ��л�
#define EkP_PIADD			34		// ƫ���������ֵ�л�
#define SPDACCP_PIADD		35		// ���ٶ���ֵ�л�
#define SPEEDP_PIADD		36		// �ٶ���ֵ�л�

#define	SelGainConADD		37		// �����л�����
#define	SelGainDelayADD		38		// �л��ӳ�ʱ��
#define	SelGainLVADD		39		// �л��ż�ˮƽ
//#define	SelGainSpdTIMADD	40		// �ٶ���������л�ʱ��
#define	SelGainPosTIMADD	41		// λ�������л�ʱ��
#define	SelGainZhihuanADD	42		// 
#define measureSPDADD		43		// ����ֵ�������ڲ���
#define KV_KP				44		// Kv,Kp�Ĺ�ϵ
#define PosRepairSGainADD	45		// ����У��ϵ��
#define FrictionPerADD		46		// Ħ�����ذٷֱ�
#define FrictionSpd			47		// Ħ�������ٶ���Ч����rpm��
#define SpdDamp				48		// ճ��Ħ��
#define PressGain1			49		// ѹ������������1
#define PressTime1			50		// ѹ��������ʱ��1
#define PressRspeed			51		// �����ת��
#define An3offset			52		// An3ƫ����
#define Prfilt_time			53		// ѹ�������˲�ʱ��
#define Prfeed_time			54		// ѹ�������˲�ʱ��
#define PressGain2			55		// ѹ������������2
#define PressTime2			56		// ѹ��������ʱ��2
#define PressGain3			57		// ѹ������������3
#define PressTime3			58		// ѹ��������ʱ��3
#define PressGain4			59		// ѹ������������4
#define PressTime4			60		// ѹ��������ʱ��4
#define PressP_PIADD		61		// ѹ������PPI�л���ֵ
	
#define PGDIVADD			66		// PG��Ƶ��
#define Agear1ADD			67		// First Gear B1
#define BgearADD			68		// Gear A
#define Agear2ADD			69		// ��2���ӳ���B2
#define POSFiltTimeADD		70		// λ��ָ���˲�
#define PosFiltSelADD		71		// λ��ָ���˲�һ�λ����ѡ��

#define SpdInsGainADD		80		// �ٶ�ָ������
#define AnaSpdOffset		81		// ģ���ٶȸ������ƫ��
//#define SPEED2ADD			82		// SPEED2
//#define SPEED3ADD			83		// SPEED3
#define ParSpdADD			84		// �����ٶ�
#define JOGSPDADD			85		// JOG�ٶ�
#define SoftAccTimeADD		86		// �����������ʱ��
#define SoftDecTimeADD		87		// �����������ʱ��
#define FiltTimeADD			88		// �ٶ�ָ��һ�κͶ����˲�ʱ��
#define SRiseTimeADD		89		// S��������ʱ��
#define AccDecSelADD		90		// �ٶ�ָ��������ʽ[0:б�� 1:S���� 2:һ���˲� 3:�����˲�]
#define SfigureSelADD		91		// S��״ѡ��
#define PBUSJOGSPD			92		// ���ߵ㶯�ٶ�
#define PressGrads			93		// ѹ���ݶ�
#define PressBackDiv		94		// ѹ����������
#define PressFeedDiv		95		// ѹ����������
#define SPEED1ADD			96		// SPEED1
#define SPEED2ADD			97		// SPEED2
#define SPEED3ADD			98		// SPEED3
#define SPEED4ADD			99		// SPEED4
#define SPEED5ADD			100		// SPEED5
#define SPEED6ADD			101		// SPEED6
#define SPEED7ADD			102		// SPEED7

#define TCRInsGainADD		103		// ת��ָ������
#define P_TCR_inLmtADD		104		// ��תת���ڲ����� |
#define N_TCR_inLmtADD		105		// ��תת���ڲ�����	|
#define P_TCR_outLmtADD		106		// ��תת���ⲿ����	|����λ��������
#define N_TCR_outLmtADD		107		// ��תת���ⲿ����	|
#define nBRKStopTCRADD		108		// �����ƶ�ֹͣת�� |
#define TCRColSpdLmtADD		109		// ת�ؿ����ٶ�����
#define NotchF1				110		// �޲�Ƶ��1
#define NotchDep1			111		// �޲����1
#define NotchF2				112		// �޲�Ƶ��2
#define NotchDep2			113		// �޲����2
#define	Vib_Fr				114		// ��Ƶ�ʣ�0.1Hz)
#define	Damping				115		// ģ������ϵ��
#define Tcrdelay_time		116		// ת�ؿ����ӳ�ʱ��
#define spddelay_width		117		// �ٶ��ͻ����
#define	AnaTcrOffset		118		// ģ��ת�ظ������ƫ��

#define PosErrADD			120		// ��λ���
#define VCMPSPDADD			121		// �ٶ�һ�����
#define ZCLSPDADD			122		// ��ǯλ��ƽ
#define TGONSPDADD			123		// ��ת����ٶ�
#define PosErrCntOVADD		124		// ƫ����������
#define SONWAITIMADD		125		// 01ms �ŷ�ON�ȴ�ʱ��[SONWAI]
#define BRKTIMADD			126		// 10ms �����ĵȴ�����[BRKTIM]
#define BRKSPDADD			127		// �ƶ��ȴ��ٶ�[BRKSPD]
#define BRKWAITIMADD		128		// 10ms �ƶ��ȴ�ʱ��[BRKWAI]
#define CN1DEF14			129		// ������˿ڷ��䵽�ź�16����4λһ���˿�
#define CN1DEF58			130		// ������˿ڷ��䵽�ź�16����4λһ���˿�
#define CN1OUTDEF			131		// 1CN78,12,56[0:COIN,1:TGON,2:S-RDY,3:CLT,4:BRK]
#define Bus_io_LEn			132		// 
#define Bus_io_HEn			133		// 
#define InputFilterTimeADD	134		// �����ź��˲�
#define ALMinFilterTimeADD	135		// �����ź��˲�
#define InputLSBADD			136		// ����˿ڵ���λȡ��
#define InputMSBADD			137		// ����˿ڸ���λȡ��
#define DBTimeADD			138		// ��̬�ƶ�ʱ��
#define EncErrTime			139		// ��������ɴ���ʱ��
#define PosCoinTimeADD		140		// ��������ɴ���ʱ��
#define	VoltAlmDisable		141		// ���籨�����
#define	SoftAlmEnable		142		// ������ⷽʽ
#define	OT_Enable  		    143		// IGBT & Motor ���ȱ������
//
#define OverloadLevelADD	145		// ���ر�����ֵ
#define MotorTemLevelADD	146		// ����¶ȹ��ȱ�����ֵ
#define Supply5vMeasure	    147		// 5V��Դʵ�ʲ���ֵ
#define	OutputNegADD		148		// ����ź�ȡ��

#define JPOG0froateADD		152		// JPOG0 Ȧ��
#define JPOG0pulseADD		153		// JPOG0 ������
#define JPOG0SpeedADD		184		// λ��0���ٶ�
#define JPOG0AccIncTimeADD	200		// JPOG0��+/-��ʱ�䳣��
#define JOG0StopTimeADD		216		// JPOS0��λ��
#define JOG15StopTimeADD	231		// JPOS15��λ��ֹͣʱ��

//#define LoopSelADD			232		
#define JDPOS_PARSADD		233		// hex0:ѡ�񵥴�/ѭ��������/�ο���
									// hex1:������������ʽ
#define ProgSelADD			234		// ��̷�ʽ
#define ProgStartPointADD	235		// ������ʼ��
#define ProgEndPointADD		236		// ������ֹ��
#define FindRefGoOTspeedADD	237		// �Ҳο���ײ�г̿����ٶ�
#define LeaveOTspeedADD		238		// �뿪�г̿����ٶ�
#define PositionTeach		239		// ʾ��
#define HomeSet             241     // ���㷽ʽ�趨
#define PulseOffset         243     // ����ƫ���������趨

//Pn700-->252
#define SCIParaSADD			252		// SCIͨѶ��ز���
#define ModbusAdderADD		253		// MODBUS���ַ
#define ModbusTimeOutADD	254		// MODBUS time out
#define CANParasADD			255		// CANͨѶ����
#define CANAdderADD			256		// CANͨѶ�ڵ�

#define UqFFCoFF			272		// ����ǰ����ϵ��4.12	 |
#define EmCoFF				273		// �����Ʋ�4.12	 |
#define AsroateUm			274		// �첽�϶�����			 |
#define FactualTn			275		// ʵ�ʶŤ��			 |	�������屣֤��������ָ�����ֵ����
#define Ku					276		// Ku					 |
#define MaxSPEED			277		// ������ļ���ת��	 |
#define ExcelNum			278		// ������				 |
#define x_base				279		// x0 ǰ��x��������ַ
#define y_base				295		// y0 ǰ��y��������ַ

#define MotorLdqADD			311		// ������
#define MotorMurselADD		312		// �����������ѡ��
#define	HzConversADD		313		// ���浥λת��
#define IPMImaxADD			314		// IPMģ�����
#define MaxSpeedADD			315		// ���ת��
#define JMotorADD			316		// ����������
#define InormalADD			317		// �����
#define PoleNumADD			318		// ������
#define EncNumADD			319		// ����������
#define AsynRunFiADD		320		// �첽�϶�����Ƶ��
#define curSamUnit			321		// ��������ϵ��
#define curKpADD			322		// ����������
#define curKiADD			323		// ����������ϵ��
#define simPowerADD			324		// ģ���ϵ�
#define InitPhaseADD		325		// ��ʼ��λ
#define Kisp1ADD			326		// Kisp1 
#define Kisp2ADD			327		// Kisp2
#define RatedSpeedADD		328		// �ת��
#define ImagneticADD		329		// �Ŷ����
#define MotorTab			330		// �Ƿ�ȡ����������

#define HOMED_FLAG  		337     //��������ֵ������ʱ�Ƿ��Ѿ������־
#define PosNumL				338
#define PosNumH				339
#define PosDivL				340
#define PosDivH				341
#define HomeOffsetL			342
#define HomeOffsetH		 	343
#define SinglePosLatZERO    344    
#define SinglePosHatZERO    345

struct hexdisp{
					Uint16 hex00:4;
					Uint16 hex01:4;
					Uint16 hex02:4;
					Uint16 hex03:4;
				};
typedef union{
				Uint16 					all;
				struct hexdisp		  	hex;
			 }HEX_DISP;
typedef	struct{	
					int32	InValue;		//�������
					Uint16 	Tfilt;     		//�˲�ʱ��
					Uint16 	Tperiod;     	//����ʱ��
					int32	OutValue;		//�������
					int16	remain;			//�������
					int32	OutData;
                }PULSEFILTER;
typedef PULSEFILTER *PULSEFILTER_handle;

struct inputstate11{
					Uint16 SON:1;
					Uint16 PCON:1;
					Uint16 POT:1;
					Uint16 NOT:1;

					Uint16 AlMRST:1;
					Uint16 CLR:1;
					Uint16 PCL:1;
					Uint16 NCL:1;

					Uint16 G_SEL:1;
					Uint16 POS_JOGp:1;
					Uint16 POS_JOGn:1;
					Uint16 POS_HALT:1;

					Uint16 HmRef:1;
					Uint16 SHOM:1;
					Uint16 ORG:1;
					Uint16 resver15:1;

				};
typedef union{
					Uint16 					all;
					struct inputstate11  	bit;
			 }IN_STATE1;

struct ouputstate11{
					Uint16 TGON:1;
					Uint16 ALM:1;
					Uint16 SRDY:1;
					Uint16 COIN:1;

					Uint16 PWM7:1;
					Uint16 RYCTL:1;
					Uint16 DBCTL:1;
					Uint16 resver07:1;

					Uint16 CLEAR:1;
					Uint16 resver09:1;
					Uint16 resver10:1;
					Uint16 resver11:1;

					Uint16 resver12:1;
					Uint16 resver13:1;
					Uint16 resver14:1;
					Uint16 RstDCM:1;

				};
typedef union{
					Uint16 					all;
					struct ouputstate11  	bit;
			 }OUT_STATE1;

struct servostate11{
					Uint16 svstate:1;		// [ ] bb [X] run
					Uint16 alm:1;			// [X] Alarm
					Uint16 mainpower:1;		// [X] ����·���ϵ�
					Uint16 p_ot:1;			// [X] P-OT

					Uint16 n_ot:1;			// [X] N-OT
					Uint16 coin:1;			// [X] ��λ���ٶȵ�
					Uint16 resver06:1;
					Uint16 resver07:1;

					Uint16 resver08:1;
					Uint16 resver09:1;
					Uint16 resver10:1;
					Uint16 resver11:1;

					Uint16 resver12:1;
					Uint16 resver13:1;
					Uint16 resver14:1;
					Uint16 resver15:1;

				};
typedef union{
					Uint16 					all;
					struct servostate11  	bit;
			 }SERVO_STATE1;

struct abs_encoderalm{
					Uint16 OS:1;		// 
					Uint16 FS:1;		//
					Uint16 CE:1;		// 
					Uint16 OF:1;		// 

					Uint16 resver4:1;	// 
					Uint16 ME:1;		// 
					Uint16 BE:1;
					Uint16 BA:1;

					Uint16 CONTE:1;
					Uint16 CRCE:1;
					Uint16 FOME:1;
					Uint16 SFOME:1;

					Uint16 TIMEOT:1;
					Uint16 resver13:1;
					Uint16 resver14:1;
					Uint16 resver15:1;

				};
typedef union{
				Uint16 					all;
				struct abs_encoderalm  	bit;
			 }ALM_ABSENC;
//ALM_FLAG1 AlarmStatus: (10)PGERR,(9)ABCBR,(8)UVWBR,(7)OHEAT,(6)RESNO,(5)PUMP,(4)LOSRS,(3)OCURR7,(2)OVOLT,(1)OCURR,(0)UVOLT
struct almstate11{
					Uint16 UVOLT:1;
					Uint16 OCURR:1;
					Uint16 OVOLT:1;
					Uint16 OCURR7:1;

					Uint16 LOSRS:1;
					Uint16 PUMP:1;
					Uint16 RESNO:1;
					Uint16 OHEAT:1;

					Uint16 UVWBR:1;
					Uint16 ABCBR:1;
					Uint16 PGERR:1;
					Uint16 ADCErr:1;

					Uint16 LOT:1;			// =0 OK
					Uint16 DOS:1;			// =0 OK
					Uint16 resver14:1;
					Uint16 resver15:1;

				};
typedef union{
					Uint16 				all;
					struct almstate11  	bit;
			 }ALM_STATE1;	


struct flag11{	// flag at key 	

					Uint16 AlmSaved:1;
					Uint16 AnAuto:1;			// ģ�����Զ������־
					Uint16 CurAuto:1;			// �����Զ������־
					Uint16 wEncState:1;			//
					Uint16 resver04:1;
					Uint16 JOGrun:1;
					Uint16 JOGpos:1;			// 
					Uint16 JOGneg:1;
					Uint16 JInertia:1;			// ����ʶ��
					Uint16 DSP_PLD_Var:1;		// DSP/PLD�汾��ʾ
					Uint16 rstEncErr:1;			// �������ֵ��������ش���
					Uint16 rstEncMuti:1;		// �����������Ȧ���ݼ���ش���
					Uint16 resver12:1;
					Uint16 loadDefault:1;			// 
					Uint16 resver14:1;
					Uint16 FRAM_W:1;			// 
		 };

typedef union{
				Uint16 			all;
				struct flag11  	bit;
			 }KEY_FLAG1;

struct flag22{	// main flag
				 	Uint16 Son:1;			// ��־
					Uint16 Pot:1;			// ��־
					Uint16 Not:1;			// ��־
					Uint16 HaveDB:1;		// ��־ flag2.bit3
					Uint16 PotNotvalid:1;	// ��־
					Uint16 DB:1;			// ��־ flag2.bit5
					Uint16 JOGsv_on:1;		// ��־
					Uint16 SvReady:1;		// ��־
					Uint16 HaveAlm:1;		// ��־
					Uint16 DispAlm:1;		// ��־
					Uint16 ClrAlm:1;		// ��־
					Uint16 ClrMenu:1;		// ��־
					Uint16 haveReadUVW:1;	// ��־
					Uint16 enPWM:1;			// ��־
					Uint16 FindIinitPhase:1;// 
					Uint16 PWM_HIZ:1;		// PWM����̬����

					Uint16 CommDelay:1;		// ͨѶ�رշ���ʹ����ʱ��־
					Uint16 RES_read:1;		// [0]�������ʼֵδ����[1]�������ʼֵ�Ѷ�
					Uint16 alminfiltercal:1;// 
					Uint16 nBRKfinish:1;	// ��־flag5.bit3
					Uint16 nBRKsoff:1;
					Uint16 nBRKzcl:1;
					Uint16 POTvalid:1;		// ˲ͣ flag5.bit6
					Uint16 NOTvalid:1;
					Uint16 HandPhase:1;		// flag5.bit8
					Uint16 INorJOG_SOFF:1;
					Uint16 BRKon:1;
					Uint16 HaveBRKout:1;
					Uint16 enSON:1;			// flag5.bit12
					Uint16 TGONvalid:1;		// flag5.bit13
					Uint16 IGBTOFF:1;		// �����ɺ�IGBT�ض�
					Uint16 EXrignt:1;		// �˿ڲ������μ���һ��
		 };

typedef union{
					Uint32 			all;
					struct flag22  	bit;
			 }STATE_FLAG2;

struct flag33{	// flag at int 	

					Uint16 J_initia:1;			// ����ʶ�������ʼ��
					Uint16 J_End:1;				// ����ʶ�����
					Uint16 PowerOn:1;			// 
					Uint16 LOSTRS:1;			// ȱ���־flag3.bit3
					Uint16 TCRlimitDir:1;
					Uint16 ZCL:1;
					Uint16 ZQDD:1;				// ˲ͣ flag3.bit6
					Uint16 SpdLoopPmode:1;
					Uint16 HavePower:1;			// flag3.bit8
					Uint16 pSgma:1;				// �������
					Uint16 pFindRef:1;
					Uint16 nFindRef:1;
					Uint16 endSgma:1;			// ��һ�λ���
					Uint16 PowerFinsih:1;		// flag3.bit13
					Uint16 secondSgma:1;		// �ڶ��λ���
					Uint16 HowGain:1;			// ��״̬[0]:��һ������[1]:�ڶ�������

					Uint16 AsynFixedRun:1;		// intflag.bit0
					Uint16 HaveFindInitPos:1;	// intflag.bit1
					Uint16 StartSW:1;			// �����л���ʱ
					Uint16 SWgain:1;			// �����л���ʼ

					Uint16 FindRef_ot:1;		// intflag.bit4
					Uint16 limitIqr:1;			// intflag.bit5
					Uint16 JDPosStart:1;		// intflag.bit6
					Uint16 haveCLRZero:1;		// intflag.bit7

					Uint16 pass360:1;			// ��ת����360
					Uint16 adjCurZero:1;		// ����������
					Uint16 infiltercal:1;		// �˿ڲ������μ���һ��
					Uint16 PC_inti:1;			// c����λ�ó�ʼ��

					Uint16 PWM_Charge:1;		// ��ʼ���
					Uint16 Volt_Reached:1;		// 15V�ѳ���
					Uint16 AdcINT:1;
					Uint16 TCR_lmtspd:1;
		 };

typedef union{
					Uint32 			all;
					struct flag33  	bit;
			 }STATE_FLAG3;

struct intflag2{
					Uint16 int_time1:1;			// ����ʱ�ж�1
					Uint16 int_time2:1;			// ����ʱ�ж�2
					Uint16 rstEncErr:1;			// �������ֵ��������ش���
					Uint16 rstEncMuti:1;		// �����������Ȧ���ݼ���ش���

					Uint16 SciFindInitPhase:1;	// ͨѶ�ҵ���ʼ��λ
					Uint16 SciAnAutoWrFRAM:1;	// У�������дFRAM
					Uint16 rFshROM:1;		// �������
					Uint16 PgIniti:1;		// ��������ʼ����־

					Uint16 pos_jogen:1;		// λ��JOGʹ��
					Uint16 wEEPROM:1;
					Uint16 rEEPROM:1;
					Uint16 PgPosIniti:1;

					Uint16 wPC_POS:1;
					Uint16 resver13:1;
					Uint16 resver14:1;
					Uint16 findCpuls:1;

				};
typedef union{
				Uint16 				all;
				struct intflag2  	bit;
			 }INT_FLAGX;

struct flag44{	// flag at pos 
					Uint16 SignedFlag:1;
					Uint16 VCMP_COIN:1;
					Uint16 JPOSn:1;			// 
					Uint16 resver03:1;			//
					Uint16 resver04:1;
					Uint16 resver05:1;
					Uint16 TCRFFQvalid:1;				//
					Uint16 PosErrOValm:1;
					Uint16 HaveAbsoluPos:1;				// �ѻ���ɾ���λ��
					Uint16 JDPosNeedStart:1;			// ��λ������Ҫ����
					Uint16 GearErr:1;
					Uint16 resver11:1;
					Uint16 resver12:1;
					Uint16 PNOTzcl:1;		// flag4.bit13
					Uint16 InhibitPulseIn:1;
					Uint16 PCONzcl:1;			// zero clamp
		 };

typedef union{
				Uint16 			all;
				struct flag44  	bit;
			 }POS_FLAG4;

struct flaghm{	// flag at HOME 
					Uint16 HM_flag:1;
					Uint16 Ek_flag:1;
					Uint16 C_Zero:1;
					Uint16 Homing_flag:1;
					Uint16 Home_State:2;
					Uint16 resver06:1;
					Uint16 resver07:1;
					Uint16 resver08:1;
					Uint16 resver09:1;
					Uint16 resver10:1;
					Uint16 resver11:1;
					Uint16 resver12:1;
					Uint16 resver13:1;
					Uint16 resver14:1;
					Uint16 resver15:1;
		 };

typedef union{
				Uint16 			all;
				struct flaghm  	bit;
			 }HOME_FLAG4;

struct loststate11{
					Uint16 LOSST:1;
					Uint16 LOSRS:1;
					Uint16 resver02:1;
					Uint16 resver03:1;

					Uint16 resver04:1;
					Uint16 resver05:1;
					Uint16 resver06:1;
					Uint16 resver07:1;

					Uint16 resver08:1;
					Uint16 resver09:1;
					Uint16 resver10:1;
					Uint16 resver11:1;

					Uint16 resver12:1;
					Uint16 resver13:1;
					Uint16 resver14:1;
					Uint16 resver15:1;

				};
typedef union{
				Uint16 					all;
				struct loststate11  	bit;
			 }LOST_STATE1;


struct xflag1{	// flag at sci xflag0/1 
					Uint16 SaveSingleComData:1;
					Uint16 resver01:1;
					Uint16 resver02:1;			// 
					Uint16 resver03:1;			// 
					Uint16 resver04:1;
					Uint16 resver05:1;
					Uint16 resver06:1;			// 
					Uint16 resver07:1;
					Uint16 resver08:1;			//
					Uint16 resver09:1;
					Uint16 resver10:1;
					Uint16 resend:1;
					Uint16 TrailErr:1;			//
					Uint16 UPdrawdata:1;
					Uint16 SaveDoubleComData:1;
					Uint16 notTrail:1;
		 };
typedef union{
				Uint16 			all;
				struct xflag1  	bit;
			 }X_FLAG1;

struct FIELDBUS{	// �ֳ�����״̬

					Uint16 RXfail:1;				//
					Uint16 TXfail:1;				//
					Uint16 FB_ASIC_err:1;			// 
					Uint16 Module_SW_err:1;			//
					Uint16 FB_CONFIG_err:1;			//
					Uint16 no_NET_Power:1;			//
					Uint16 PLC_not_ACTIVE:1;		//
					Uint16 FB_RUNNING:1;
					Uint16 Vendor_Mode:1;			//
					Uint16 resver09:1;				// 
					Uint16 resver10:1;
					Uint16 resver11:1;
					Uint16 resver12:1;
					Uint16 resver13:1;				//
					Uint16 resver14:1;
					Uint16 DPRAM_EX_Dataerr:1;				//
		 };

typedef union{
					Uint16 			all;
					struct FIELDBUS	bit;
			 }FIELDBUSSTATUS;


struct MODULEST{	// ͨ�Ű�״̬
					Uint16 DPRAMerr:1;			// 0��DPRAM OK 	1��DPRAM����ʧ��/DPRAM��
					Uint16 VPC3err:1;			// 0��VPC3����
					Uint16 HSerr:1;				// 0��HSû��	1��HS��
					Uint16 DPRAM_EX_Dataerr:1;	// ���ݽ�����
					Uint16 DPcommbreakerr:1;	// profibusͨѶ��
					Uint16 resver05:1;			//
					Uint16 resver06:1;			//
					Uint16 resver07:1;
					Uint16 resver08:1;			//
					Uint16 resver09:1;			// 
					Uint16 resver10:1;
					Uint16 resver11:1;
					Uint16 resver12:1;
					Uint16 resver13:1;
					Uint16 SCANerr:1;			// 0��ɨ��		1��ɨ�費��DPRAM�ж�
					Uint16 HS:1;				// 0��û����	1�����ֳɹ�
		 };

typedef union{
					Uint16 			all;
					struct MODULEST	bit;
			 }MODULESTATUS;

struct	PBUSSTW{
					Uint16 Son:1;
					Uint16 resver01:1;
					Uint16 resver02:1;
					Uint16 resver03:1;
					Uint16 resver04:1;
					Uint16 resver05:1;
					Uint16 resver06:1;
					Uint16 Alm_Rst:1;
					Uint16 JOG:1;
					Uint16 resver09:1;
					Uint16 resver10:1;
					Uint16 resver11:1;
					Uint16 resver12:1;
					Uint16 resver13:1;
					Uint16 resver14:1;
					Uint16 resver15:1;			
			   };
typedef union{
				Uint16			all;
				struct PBUSSTW bit;
			 }PBUS_STW;
struct	PBUSZSW{
					Uint16 SVReady1:1;
					Uint16 SVReady2:1;
					Uint16 Son:1;
					Uint16 Alm:1;
					Uint16 Son1:1;
					Uint16 Son2:1;
					Uint16 resver06:1;
					Uint16 resver07:1;
					Uint16 COIN:1;
					Uint16 CtrlReq:1;
					Uint16 resver10:1;
					Uint16 CLT:1;
					Uint16 resver12:1;
					Uint16 resver13:1;
					Uint16 resver14:1;
					Uint16 resver15:1;			
			   };
typedef union{
				Uint16			all;
				struct PBUSZSW bit;
			 }PBUS_ZSW;

struct bitdisp{
					Uint16 bit00:1;
					Uint16 bit01:1;
					Uint16 bit02:1;
					Uint16 bit03:1;

					Uint16 bit04:1;
					Uint16 bit05:1;
					Uint16 bit06:1;
					Uint16 bit07:1;

					Uint16 bit08:1;
					Uint16 bit09:1;
					Uint16 bit10:1;
					Uint16 bit11:1;

					Uint16 bit12:1;
					Uint16 bit13:1;
					Uint16 bit14:1;
					Uint16 bit15:1;

				};
typedef union{
				Uint16 					all;
				struct bitdisp		  	bit;
			 }BIT_DISP;

struct flag{	// sci flag
					Uint16 RX_end:1;
					Uint16 RX_data_number:1;	// 
					Uint16 RX_valid:1;			// 
					Uint16 RX_have_data:1;		// ���ܵ�����
					Uint16 resver04:1;			// 
					Uint16 resver05:1;			// 
					Uint16 resver06:1;			// 
					Uint16 resver07:1;
					Uint16 resver08:1;			//
					Uint16 resver09:1;
					Uint16 HOST_undone:1;
					Uint16 resver11:1;
					Uint16 TX_end:1;
					Uint16 TX_data_ready:1;		// 
					Uint16 Reply:1;
					Uint16 SCI_Change_Flag:1;	//
		 };
typedef union{
				Uint32 			all;
				struct flag  	bit;
			 }SCI_FLAG;

struct sci_flag_int{	// sci flag_int
					Uint16 RX_type:1;
					Uint16 RXbuffer_add:1;
					Uint16 ASCII:1;			// 
					Uint16 Save_Flag:1;			// 
					Uint16 ASCIIHead:1;
					Uint16 resver05:1;
					Uint16 resver06:1;			// 
					Uint16 resver07:1;
					Uint16 resver08:1;			//

					Uint16 resver09:1;			// 
					Uint16 resver10:1;			// 
					Uint16 resver11:1;			// 
					Uint16 resver12:1;			// 
					Uint16 resver13:1;			// 
					Uint16 resver14:1;			// 
					Uint16 resver15:1;			// 
		 };
typedef union{
				Uint16 			all;
				struct sci_flag_int  	bit;
			 }SCI_INT_FLAG;

typedef struct _SCI_DATA{	// sci data
					Uint16 Address;			//ͨѶ��ŵ�ַ
					Uint16 sci_state;		//sci״̬
					Uint16 RX_data_number;	//
					Uint16 TX_data_number;	//
					Uint16 Error_mesg;		//������������
					Uint16 RX_data_ASCII;	//�ж��д���������
					Uint16 TX_cmd;
					Uint16 RX_cmd;
					Uint16 SCIFourbyteTIME;	//�������ʺ�ֹͣʱ��
					Uint16 SCI_Changed;
					Uint16 SCISampleTrig;
					int16  SCISampleTrigV1;
					int16  SCISampleTrigV2;
					Uint16 SCISampleTime;
					Uint16 SCIInput;
					Uint16 SCIHandCurPnnum;

					Uint16 * RX_data_ADR;	//������ʼ���ϵ�ַ
					//Uint16 * plot;			//�����ݲ���Դ���?
					//Uint16 * Target;		//��ſ�ĵ��?
		 }SCI_DATA;

struct _sci_ope_flag{	// sci 
					Uint16 SciAdSpdEnd:1;
					Uint16 SciAdTcrEnd:1;
					Uint16 sciJogson:1;			// ͨѶSON
					Uint16 sci_pos_jogp:1;		// 
					Uint16 sci_pos_jogn:1;		// 
					Uint16 sciPhaseSON:1;		// ͨѶ�������� xflag1,bit1	 �����ҳ�ʼλ�õ��ŷ�ʹ��
					Uint16 sciStartInertia:1;	// ͨѶ��������
					Uint16 sciStartSample:1;	// ͨѶ�������ݲɼ�
					Uint16 sciStartZeroSpd:1;	// ͨѶ��������ADУ��
					Uint16 sciStartZeroTcr:1;	// ͨѶ��������ADУ��
					Uint16 sci_pos_halt:1;		// 
					Uint16 resver11:1;			// 
					Uint16 SciFindInitPhase:1;	// ͨѶ�ҵ���ʼ��λ
					Uint16 SciSampleing:1;		// 0: ����û��ʼ 1�������Ѿ�����������ʼ��
					Uint16 SciLoadDefault:1;	// �ָ�����ֵ
					Uint16 resver15:1;			// 
		 };
typedef union{
					Uint16 			all;
					struct _sci_ope_flag  	bit;
			 }SCI_OPE_FLAG;

struct _sci_ope_status{	// sci ������־
					Uint16 sciPhaseStatus:2;		// 0:û�ж��ࣻ			1����������У�		2���������
					Uint16 sciInertiaStatus:2;		// 0:û�й����⣻		1�������������У�	2������������
					Uint16 sciZeroSpdStatus:2;	// 0:û��ģ����У0��	1��ģ����У0�����У�2��ģ����У0���
					Uint16 sciZeroTcrStatus:2;				// 
					Uint16 resver04:2;				// 
					Uint16 resver05:2;				// 
					Uint16 resver06:2;				// 
					Uint16 sciSampleStatus:2;		// 0:û�л�ͼ���ݲɼ���	1����ͼ���ݲɼ������У�2����ͼ���ݲɼ���
		 };
typedef union{
					Uint16 			all;
					struct _sci_ope_status  	bit;
			 }SCI_OPE_STATUS;

struct _drv_status{	// ������״̬
					Uint16 p_ot:1;
					Uint16 n_ot:1;
					Uint16 TGON:1;			// 
					Uint16 TCR_CLRerr:1;	// 
					Uint16 ovVref_inPuls:1;	// 
					Uint16 mainpower:1;	// 
					Uint16 coin:1;	// 
					Uint16 svstate:1;	// 
					Uint16 SvReady:1;	// 
					Uint16 resver09:1;	// 
					Uint16 resver10:1;	// 
					Uint16 resver11:1;	// 
					Uint16 resver12:1;	// 
					Uint16 resver13:1;	// 
					Uint16 resver14:1;	// 
					Uint16 ALM:1;	// 
		 };
typedef union{
					Uint16 			all;
					struct _drv_status  	bit;
			 }DRV_STATUS;

// ------------------------------------------------------------------------------
//	���ͻ������ṹ
// ------------------------------------------------------------------------------
typedef	struct	_TXBUFEER	{
					Uint16	* paddress;		//�������׵�ַ
					Uint16	* pcurrent;		//д��������ǰ��ַ
					Uint16	data_length;	//��������Ч����
					Uint16	Hst_data_wrtten;//������д������������BYTE����
		}TXBUFEER,* pTXBUFEER;
// ------------------------------------------------------------------------------
//	���ջ������ṹ Old and MOD
// ------------------------------------------------------------------------------
typedef	struct	_RXBUFEER	{
					Uint16	* paddress;		//�������׵�ַ
					Uint16	* pcurrent;		//д��������ǰ��ַ
					Uint16	data_length;	//��������Ч����
		}RXBUFEER,* pRXBUFEER;
// ------------------------------------------------------------------------------
//	�洢���ݻ������ṹ
// ------------------------------------------------------------------------------
typedef	struct	_SVBUFEER	{
					Uint16	* paddress;		//�������׵�ַ
					Uint16	* pcurrent;		//д�����׵�ַ,��ǰ��ַ
					Uint16	data_length;	//��������Ч����
		}SXBUFEER,* pSXBUFEER;

//CAN control error register
struct _CAN_ERROR_REG{	// CAN����������Ĵ���
			Uint16 EW:1;
			Uint16 EP:1;
			Uint16 BO:1; 
			Uint16 ACKE:1;
			Uint16 SE:1;
			Uint16 CRCE:1;
			Uint16 BE:1;
			Uint16 FE:1;	// 
			Uint16 SvReady:1;	// 
			Uint16 resver09:1;	// 
			Uint16 resver10:1;	// 
			Uint16 resver11:1;	// 
			Uint16 resver12:1;	// 
			Uint16 resver13:1;	// 
			Uint16 resver14:1;	// 
			Uint16 resver15:1;	// 
		 };
typedef union{
				Uint16 			all;
				struct _CAN_ERROR_REG  	bit;
			 }CAN_ERROR_REG;

//CanͨѶ���Ʊ�־
struct _canopen_ctl_bits{	// CANOpen Controlword
			Uint16 SwitchON:1;
			Uint16 EnVoltage:1;
			Uint16 QkStop:1;			// 
			Uint16 EnOper:1;			// 
			Uint16 OperSpecific_b4:1;	// 
			Uint16 OperSpecific_b5:1;	// 
			Uint16 OperSpecific_b6:1;	// 
			Uint16 RstAlm:1;			// 
			Uint16 halt:1;				//
			Uint16 OperSpecific_b9:1; 
			Uint16 resvers:6;			// 

		 };
typedef union{
				Uint16 			all;
				struct _canopen_ctl_bits  	bit;
			 }CANOPEN_CTL;

struct _canopen_status_bits{	// CANOpen Controlword
			Uint16 ReadySwitchOn:1;			// 0
			Uint16 SwitchednOn:1;			// 1
			Uint16 OperEnabled:1;			// 2
			Uint16 Fault:1;					// 3
			Uint16 VoltageEnabled:1;		// 4
			Uint16 QkStop:1;				// 5
			Uint16 SwitchednOnDisabled:1;	// 6
			Uint16 resvers07:1;				// warning
			Uint16 resvers08:1;				// specific
			Uint16 Remote:1;				// 
			Uint16 TargetReached:1;			// 
			Uint16 InLimitAct:1;			// 
			Uint16 OperSpecific_b12:1;		// 
			Uint16 OperSpecific_b13:1;		// 
			Uint16 resvers:1;				// 
            Uint16 OperSpecific_b15:1;       //�Ƿ�ع���ı�־
		 };
typedef union{
				Uint16 			all;
				struct _canopen_status_bits  	bit;
			 }CANOPEN_STATUS;

// Functions
void Menu(void);
void InitVar(void);
void CommTxData(void);
void SPI_ReadFRAM(Uint16 address,volatile Uint16 *data,Uint16 quantity);
void SPI_WriteFRAM(Uint16 address,volatile Uint16 *data,Uint16 quantity);	
void HexToBcd(Uint16 data);
void HexToBit(Uint16 data);
void ModifyContent(void);
void InOutDisplay(Uint16 m);
void DecDisplay(Uint16 m,Uint16 n);
void HexDisplay(Uint16 m);
void Undisplay(Uint16 m,Uint16 n);
void JudgeAlarm(void);
void Read_AD(void);
void MedianFilter(void);
//interrupt void CURRENT_T3UFINT_ISR(void);
interrupt void SPEED_Loop_ISR(void);
interrupt void ENCODER_PGC_ISR(void);
interrupt void SCI_RX_ISR(void);
interrupt void ECAN_ISR(void);
void admeasure_read(void);
void runcur_process(void);
void SVPWM(void);
void Asroate(void);
void OrientFlux(void);
void asyn_fixed_run(void);
void load_calculate(void);
void InputFilter(void);
void speed_loop_process(void);
void ControlSel(void);
void delay_loop(void);
void SaveCommData(void);
void error(void);
void limitIqr(void);
void dowith_Notch(void);
int32 Notch(Uint16 f0,Uint16 u,int32 inNotch);
void SModule1(void);
void SModule2(void);
void SModule3(void);
void SModule4(void);
void SoftTime2(Uint16 AccTime,Uint16 DecTime);
void NextAnaSC(void);
void SPEED_PULS(void);
void AnalogSpdCol(void);
void AnalogTcrCol(void);
void SJDControl(void);
void JDPosCol(void);
void PControl(void);
void NoSetZero(void);
void runposition(void);
void seek_step(int16);
void notclrEk(void);
void ParSpd(void);
void pulsefilter(volatile PULSEFILTER * v);
void LmtSpd(void);
void JudgeVCMP(void);
void cacuload(int32 Iqrate,Uint16 curIPM);
void NoPNOT(void);
void PN_OTrelay(void);
void BRstate(void);
void SET_PROout(int16);
void CLR_PROout(int16);
void def_input(Uint16 x,Uint16 y,Uint16 z);
void SingleOut(int16,int16);
void Jdynamic(void);
void GainSwitch(void);
void cacuGain(int32 Tmp);
void pulsEncoder(void);
void GradsCacu(Uint32 GradsIn,Uint16 GradsTime);
void ServoPump(void);
int32 PressControl(int32 PressFeed,int32 PressBack,int32 QSpeed,int32 RspeedLmt);
int32 Scacu(int userTime,int line_style,int32 sfiltin);
// DPRAM
//void PowerUpInitHandshake(void);
//Uint16 InitialHandshake(void);
//DRAM
void WrParFRAM(void);						// ����д����
void CheckGuard(void);						// ����������Ƿ����仯
void DPRAM_main_process(void);
void DPRAM_Timers_process(void);
void DPRAM_init(void);
void ParaDataExchange(void);
void DPRAM_DirectDataExchange(void);
//SCI
void SCI_COM_Process(void);
void SCIBRST_init(void);
void LoadDefaultPar(void);
//ad2s1210
void AD2S1210_init(void);
void readRES_POS(void);
void readRES_FAULT(void);
void CNT_TIMER(void);
void Control_PROG(void);
//CANopen
void CO_ProcessMain(void);
void CO_ResetComm(void);
void CO_Timer01msIsr(void);
void Can_Transmit_Data(void);
void CO_DrvStateMachine(void);
void CO_HM_mode(void);
void CO_PP_mode(void);
void CO_PV_mode(void);
void CO_IP_mode(void);

void ADC_HANDLE(void);
void Enable_PWM(void);
void Disable_PWM(void);
void PWM_Charge(void);

Uint16 XRAMtest(Uint16 * XRAM_FirstIndex,Uint16 length);
Uint16 DatalineTest(void);

//Home state
void Home_State_Position(void);
void Home_Operation(void);
void Pos_Zero(void);
void Ref_Zero(void);
void PD_Zero(void);
