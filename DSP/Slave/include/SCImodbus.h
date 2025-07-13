#define UserPassword		2008		// 用户密码
#define PnMaxNum			873			// 最大参数号
// wr
#define ZeroSpdADD			0x07FB		// 速度指令零点偏移量
#define ZeroTcrADD			0x07FC		// 转矩指令零点偏移量
// Only read
#define ZeroIuADD			0x07FD		// Iu指令零点偏移量
#define ZeroIvADD			0x07FE		// Iv指令零点偏移量

#define UnspdADD			0x0806		// 监视速度反馈Un000
#define UnPgADD				0x0814		// 监视给定脉冲高Un014
#define UnperJloadADD		0x0815		// 监视Un015
#define Unx_SQRTiqADD		0x0816		// 监视Un016
#define AlarmnoADD			0x0817  	// 当前报警
#define MotorTemADD         0x0818      // 电机温度

#define MemgearB			0x0850  	// 电子齿轮分母
#define MemgearA1			0x0851  	// 电子齿轮分子1
#define MemgearA2			0x0852  	// 电子齿轮分子2

#define MODBUSinputADD		0x0900	// MODBUS输入接点
#define DRVstatusADD		0x0901	// 驱动器状态
#define PhaseADD			0x0902	// 对相数值
#define InertiaADD			0x0903	// 惯量检测数值
#define DRVruntimeADD		0x0904	// 驱动器运行时间
#define UserPINADD			0x090D	// 用户密码
#define SoftDSPVarADD		0x090E	// 软件DSP版本
#define SoftPLDVarADD		0x090F	// 软件PLD版本
#define SoftMODVarADD		0x0910	// 软件MODULE版本

#define speedrminADD		0x0910	// 速度反馈（r/min）
#define VCMDrminADD			0x0911	// 速度给定（r/min）
#define TCMDADD				0x0912	// 输入转矩指令百分比
#define TnADD				0x0913	// 内部转矩指令百分比
#define inputADD			0x0914	// 输入信号状态
#define outMemADD			0x0915	// 输出信号状态
#define CurPos1ADD			0x0916	// 当前位置低位
#define CurPosADD			0x0917	// 当前位置高位
#define Ek4ADD				0x0918	// 偏差脉冲计数低位
#define Ek3ADD				0x0919	// 偏差脉冲计数高位
#define Pg1ADD				0x091A	// 给定脉冲低位
#define PgADD				0x091B	// 给定脉冲高位
#define IuADD				0x091C	// Iu
#define IvADD				0x091D	// Iv
#define IdrADD				0x091E	// Idr
#define IdADD				0x091F	// Id
#define IqrADD				0x0920	// Iqr
#define IqADD				0x0921	// Iq
#define UdADD				0x0922	// Ud
#define UqADD				0x0923	// Uq
#define speedADD			0x0924	// 速度反馈（0.2r/min）
#define VCMDADD				0x0925	// 速度给定（0.2r/min）

#define SPEEDREF			0x0928	// 速度给定

#define wAnPressFeed_outADD	0x0930	// press feed
#define wAnPressBack_outADD	0x0931	// press back

#define DrvTypeADD			0x1000	// 驱动器型号
#define KjPnnumADD			0x1001	// 可见参数范围
#define DrvFnStatusADD		0x1002	// 伺服驱动器辅助功能状态
#define MAXroADD			0x1002	//
// 可读可写
#define HandOpPnnumADD		0x1003	// 06	手持操作参数索引号
// 只读
#define HandPnMaxADD		0x1004	// 03	手持读参数最大值
#define HandPnMinADD		0x1005	// 03	手持读参数最小值
#define HandPnDataADD		0x1006	// 03	手持读参数值
#define HandPnattributeADD	0x1007	// 03	手持读参数属性
#define RotateADD			0x1010	// 03	Rotate
#define SinglePos_ADDL		0x1011	// 03	singlePos_L
#define SinglePos_ADDH		0x1012	// 03	singlePos_H
// 只读
#define SampleFirstADD		0x2000	// 采样数据首地址
#define SampleLastADD		0x2800	// 采样数据末地址

// 只能写，而且不能连续写
#define CLRALMLOGcom		0x1021	// 清除历史报警
#define CLRALMcom			0x1022	// 清除当前报警
#define JOGSoncom			0x1023	// JOG son
#define JOGPruncom			0x1024	// JOG正转
#define JOGNruncom			0x1025	// JOG反转
#define JDPOSJOGpcom		0x1026	// 接点位置下JOG正转
#define JDPOSJOGncom		0x1027	// 接点位置下JOG反转
#define JDPOSHaltcom		0x1028	// 接点位置下暂停
#define PHASEstartcom		0x102B	// 启动对相
#define Inertiastartcom		0x102C	// 启动惯量检测
#define Samplestartcom		0x1030	// 启动绘图数据操作
#define SampleADD1com		0x1031	// 采样数据地址1
#define SampleADD2com		0x1032	// 采样数据地址2
#define SampleTrigcom		0x1033	// 采样数据触发条件
#define SampleTrigV1com		0x1034	// 采样数据触发门限1
#define SampleTrigV2com		0x1035	// 采样数据触发门限2
#define SampleTimecom		0x1036	// 采样时间
#define ClrEncErrcom		0x1040	// clear encoder alarm
#define ClrEncMuticom		0x1041	// clear encoder muti
#define ZeroSpdstartcom		0x1052	// 模拟量校零启动
#define ZeroTcrstartcom		0x1053	// 模拟量校零启动
#define LoadDefaultcom		0x1055	// 恢复默认值
#define RESETDSPcom			0x1056	// reset

//test
#define TESTWave			0x1100	// 测试波形
#define DISTANCEHigh		0x1101	// 移动距离
#define DISTANCELow			0x1102	//
#define TESTSpeed			0x1103	// 测试速度
#define TESTAcc				0x1104	// 加速度
#define TESTFreq			0x1105	// 频率
#define TESTRepeat			0x1106	// 重复次数
#define TESTRuntime			0x1107	// 阶跃时间
#define TESTStopServo		0x1108	// 
#define TESTCWCCW			0x1109	// 方向
#define TESTStoptime		0x110A	// 停止时间
#define TESTStart			0x110B	// 启动测试
#define TESTMODE			0x110C	// 测试模式

#define TESTScantime		0x1110	//
#define TESTScannumber		0x1111	//

#define SampleADD3com		0x1200	// addr3
#define SampleADD4com		0x1201	// addr4

#define AdjustSpdADD		0x1300  // 速度模拟量校正地址
#define AdjustTcrADD		0x1301  // 力矩模拟量校正地址
#define AdjustSpdENDADD     0x1302  // 速度模拟量校正完成标志地址
#define AdjustTcrENDADD     0x1303  // 力矩模拟量校正完成标志地址
#define AdResultADD         0x1304  // 十个校正值读取首地址


#define SampleDataBuf		0xF000  // 采样数据缓冲区
								 

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
//	串行通讯数据结构定义 
// ==============================================================================
#define	RECEIVE_DATA_SAVE		0
#define	RECEIVE_FR_END_FLAG		1
#define	RECEIVE_FR_END			2
#define	RECEIVE_SCI_ERROR		3
#define	RECEIVE_SCI_END			1
//------------------------------------------------------------------
