//=============================================================================
// 
// 
//=============================================================================

//=============================================================================
#define POSITION_RAMP		1
#define POSITION_SINUSOIDAL	2
#define POSITION_STEP		10
#define VELOCITY_TRAPE		11

#define MAX_ACC				65535
#define MAX_Distance		1000000
//step
#define FIRST_STEP			1
#define SECOND_STEP			2
#define THIERD_STEP			3
//=============================================================================

void	Caculate_Time(void);
void	Save_Data_Flag(void);
void	Test_Position_Sin(void);
void	Init_TestData(void);
void	Main_Test(void);
void	Test_Position_Step(void);
void	Test_Velocity_Trape(void);
void	Pulse_give(void);
void	clear_remain_data(void);
void	Test_Position_Ramp(void);
int		SpeedToPulse(int speed);
void	Clear_Test_Data(void);


typedef	struct{	 	
				volatile Uint32 target_v; 			   		
				volatile Uint32 acc;
				volatile Uint32 dec;
				volatile int32  target_position;
				volatile int32  dec_position;
}TEST_PP_VELOCITY_PLAN_PARS1;
typedef TEST_PP_VELOCITY_PLAN_PARS1 *TEST_PP_VELOCITY_PLAN_PARS_handle1;

typedef	struct{
				volatile Uint32 Agear;	
				volatile Uint32 Bgear;
				volatile int32 ACCorSPEEDin;				//输入变量
				volatile int32 ACCorSPEEDremain;   			//输出余数
				volatile int32 ACCorSPEEDout;				//输出变量
}ACC_OR_SPEED_UNIT_CONVER1;									// 单位换算 /s --> /100us
typedef ACC_OR_SPEED_UNIT_CONVER1 *ACC_OR_SPEED_UNIT_CONVER_handle;

volatile ACC_OR_SPEED_UNIT_CONVER1 acc_aster1more = {0,0,0,0,0};		// Unit: 0.1rpm/s
volatile ACC_OR_SPEED_UNIT_CONVER1 dec_aster1more = {0,0,0,0,0};		// Unit: 0.1rpm/s
volatile ACC_OR_SPEED_UNIT_CONVER1 acc_aster2more = {0,0,0,0,0};		// Unit: 0.1rpm/100us
volatile ACC_OR_SPEED_UNIT_CONVER1 dec_aster2more = {0,0,0,0,0};		// Unit: 0.1rpm/100us
volatile ACC_OR_SPEED_UNIT_CONVER1 step_posmore = {0,0,0,0,0};		// Unit: p/100us

volatile Uint32 PosSPEEDINS1 = 0;		//Unit: p/s 
volatile Uint32 PosSPEEDINSout1 = 0;		//Unit: p/s 
volatile int32  SigmaPgerr1 = 0;
volatile int32  inPgcnt1 = 0;
volatile int32  outPgerr = 0;
volatile int16  direction = 0;


volatile TEST_PP_VELOCITY_PLAN_PARS1 TEST_PP_velocity_plan_pars1 = {0,0,0,0,0};
Uint32 test_pos_speed_plan1(volatile TEST_PP_VELOCITY_PLAN_PARS1 * pars);

Uint32 trap_speed_control1(Uint32 acc_a,Uint32 dec_a,int32 SPPEEDINS, int32 SPPEEDINSout);
void acc_speed_unit_convers1(volatile ACC_OR_SPEED_UNIT_CONVER1 * v);

//=============================================================================
// End of file.
//=============================================================================
