#define UserPassword		2008		// �û�����
#define PnMaxNum			873			// ��������
// wr
#define ZeroSpdADD			0x07FB		// �ٶ�ָ�����ƫ����
#define ZeroTcrADD			0x07FC		// ת��ָ�����ƫ����
// Only read
#define ZeroIuADD			0x07FD		// Iuָ�����ƫ����
#define ZeroIvADD			0x07FE		// Ivָ�����ƫ����

#define UnspdADD			0x0806		// �����ٶȷ���Un000
#define UnPgADD				0x0814		// ���Ӹ��������Un014
#define UnperJloadADD		0x0815		// ����Un015
#define Unx_SQRTiqADD		0x0816		// ����Un016
#define AlarmnoADD			0x0817  	// ��ǰ����
#define MotorTemADD         0x0818      // ����¶�

#define MemgearB			0x0850  	// ���ӳ��ַ�ĸ
#define MemgearA1			0x0851  	// ���ӳ��ַ���1
#define MemgearA2			0x0852  	// ���ӳ��ַ���2

#define MODBUSinputADD		0x0900	// MODBUS����ӵ�
#define DRVstatusADD		0x0901	// ������״̬
#define PhaseADD			0x0902	// ������ֵ
#define InertiaADD			0x0903	// ���������ֵ
#define DRVruntimeADD		0x0904	// ����������ʱ��
#define UserPINADD			0x090D	// �û�����
#define SoftDSPVarADD		0x090E	// ���DSP�汾
#define SoftPLDVarADD		0x090F	// ���PLD�汾
#define SoftMODVarADD		0x0910	// ���MODULE�汾

#define speedrminADD		0x0910	// �ٶȷ�����r/min��
#define VCMDrminADD			0x0911	// �ٶȸ�����r/min��
#define TCMDADD				0x0912	// ����ת��ָ��ٷֱ�
#define TnADD				0x0913	// �ڲ�ת��ָ��ٷֱ�
#define inputADD			0x0914	// �����ź�״̬
#define outMemADD			0x0915	// ����ź�״̬
#define CurPos1ADD			0x0916	// ��ǰλ�õ�λ
#define CurPosADD			0x0917	// ��ǰλ�ø�λ
#define Ek4ADD				0x0918	// ƫ�����������λ
#define Ek3ADD				0x0919	// ƫ�����������λ
#define Pg1ADD				0x091A	// ���������λ
#define PgADD				0x091B	// ���������λ
#define IuADD				0x091C	// Iu
#define IvADD				0x091D	// Iv
#define IdrADD				0x091E	// Idr
#define IdADD				0x091F	// Id
#define IqrADD				0x0920	// Iqr
#define IqADD				0x0921	// Iq
#define UdADD				0x0922	// Ud
#define UqADD				0x0923	// Uq
#define speedADD			0x0924	// �ٶȷ�����0.2r/min��
#define VCMDADD				0x0925	// �ٶȸ�����0.2r/min��

#define SPEEDREF			0x0928	// �ٶȸ���

#define wAnPressFeed_outADD	0x0930	// press feed
#define wAnPressBack_outADD	0x0931	// press back

#define DrvTypeADD			0x1000	// �������ͺ�
#define KjPnnumADD			0x1001	// �ɼ�������Χ
#define DrvFnStatusADD		0x1002	// �ŷ���������������״̬
#define MAXroADD			0x1002	//
// �ɶ���д
#define HandOpPnnumADD		0x1003	// 06	�ֲֳ�������������
// ֻ��
#define HandPnMaxADD		0x1004	// 03	�ֳֶ��������ֵ
#define HandPnMinADD		0x1005	// 03	�ֳֶ�������Сֵ
#define HandPnDataADD		0x1006	// 03	�ֳֶ�����ֵ
#define HandPnattributeADD	0x1007	// 03	�ֳֶ���������
#define RotateADD			0x1010	// 03	Rotate
#define SinglePos_ADDL		0x1011	// 03	singlePos_L
#define SinglePos_ADDH		0x1012	// 03	singlePos_H
// ֻ��
#define SampleFirstADD		0x2000	// ���������׵�ַ
#define SampleLastADD		0x2800	// ��������ĩ��ַ

// ֻ��д�����Ҳ�������д
#define CLRALMLOGcom		0x1021	// �����ʷ����
#define CLRALMcom			0x1022	// �����ǰ����
#define JOGSoncom			0x1023	// JOG son
#define JOGPruncom			0x1024	// JOG��ת
#define JOGNruncom			0x1025	// JOG��ת
#define JDPOSJOGpcom		0x1026	// �ӵ�λ����JOG��ת
#define JDPOSJOGncom		0x1027	// �ӵ�λ����JOG��ת
#define JDPOSHaltcom		0x1028	// �ӵ�λ������ͣ
#define PHASEstartcom		0x102B	// ��������
#define Inertiastartcom		0x102C	// �����������
#define Samplestartcom		0x1030	// ������ͼ���ݲ���
#define SampleADD1com		0x1031	// �������ݵ�ַ1
#define SampleADD2com		0x1032	// �������ݵ�ַ2
#define SampleTrigcom		0x1033	// �������ݴ�������
#define SampleTrigV1com		0x1034	// �������ݴ�������1
#define SampleTrigV2com		0x1035	// �������ݴ�������2
#define SampleTimecom		0x1036	// ����ʱ��
#define ClrEncErrcom		0x1040	// clear encoder alarm
#define ClrEncMuticom		0x1041	// clear encoder muti
#define ZeroSpdstartcom		0x1052	// ģ����У������
#define ZeroTcrstartcom		0x1053	// ģ����У������
#define LoadDefaultcom		0x1055	// �ָ�Ĭ��ֵ
#define RESETDSPcom			0x1056	// reset

//test
#define TESTWave			0x1100	// ���Բ���
#define DISTANCEHigh		0x1101	// �ƶ�����
#define DISTANCELow			0x1102	//
#define TESTSpeed			0x1103	// �����ٶ�
#define TESTAcc				0x1104	// ���ٶ�
#define TESTFreq			0x1105	// Ƶ��
#define TESTRepeat			0x1106	// �ظ�����
#define TESTRuntime			0x1107	// ��Ծʱ��
#define TESTStopServo		0x1108	// 
#define TESTCWCCW			0x1109	// ����
#define TESTStoptime		0x110A	// ֹͣʱ��
#define TESTStart			0x110B	// ��������
#define TESTMODE			0x110C	// ����ģʽ

#define TESTScantime		0x1110	//
#define TESTScannumber		0x1111	//

#define SampleADD3com		0x1200	// addr3
#define SampleADD4com		0x1201	// addr4

#define AdjustSpdADD		0x1300  // �ٶ�ģ����У����ַ
#define AdjustTcrADD		0x1301  // ����ģ����У����ַ
#define AdjustSpdENDADD     0x1302  // �ٶ�ģ����У����ɱ�־��ַ
#define AdjustTcrENDADD     0x1303  // ����ģ����У����ɱ�־��ַ
#define AdResultADD         0x1304  // ʮ��У��ֵ��ȡ�׵�ַ


#define SampleDataBuf		0xF000  // �������ݻ�����
								 

const Uint16 ESTUN[]={0x45,0x53,0x54,0x55,0x4E};
const Uint16 EDB_Servo[]={0x45,0x44,0x43,0x20,0x53,0x65,0x72,0x76,0x6F};
const Uint16 Speed_Software[]={0x30,0x31,0x30,0x30,0x30,0x30};//010000 V100
//------------------------------------------------------------------
//	
//------------------------------------------------------------------

#define	Motor_Type_MOD		222
#define ASCII_0				48
#define ASCII_9				57
#define ASCII_A				65
#define ASCII_F				70

#define ASCII_3A			0x3A
#define DataLength_Max_MOD	300//255

#define	TRUE	1
#define FALSE	0
// ==============================================================================
//	����ͨѶ���ݽṹ���� 
// ==============================================================================
#define	RECEIVE_DATA_SAVE		0
#define	RECEIVE_FR_END_FLAG		1
#define	RECEIVE_FR_END			2
#define	RECEIVE_SCI_ERROR		3
#define	RECEIVE_SCI_END			1
//------------------------------------------------------------------
