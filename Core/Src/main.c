/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "SPI.h"
#include "Masterrtu.h"
#include "stm32f1xx_hal_uart.h"
#include "tm1650.h"
#include "Function.h"
#include <stdlib.h>
#include "24cxx.h"
#include <string.h>
#include "pid.h"
#include "pc485slave.h"

//#define WaringConst 40//55  多次验证55能正常顺利上12000. 4.21 40在冷泵状态下顺利到12K
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
extern uint8_t BengSetup;
extern uint8_t BigError;
extern uint8_t HaveError;
float  BoardT;
extern uint16_t PreADDTimeData;
extern uint16_t ADDTimeData;
extern uint16_t R600Time;
extern uint8_t ReadLiJuSpdFlag;

extern uint32_t UsedTime;
extern uint8_t WriteUseTimeTOFlash;
uint8_t HighSpeedOk=0;
//uint32_t HighSpeedOkCount;
int32_t PowerNow,PowerNow2;
uint32_t PowerMaX=700000;
uint16_t LiJuLimit;


uint16_t ReadDSPBusy;
extern uint16_t WriteDSPBusy;
extern uint16_t ReadDSPBusy;
extern uint8_t WriteDspDelayCount1,WriteDspDelayCount2;
extern int32_t PidSpi;
extern int32_t CCHSpeed;
uint8_t OffAdjFinishedFlag=0;
uint16_t AvrFactSpeed;
uint16_t  PreNowCommandSpeed;
IWDG_HandleTypeDef hiwdg;

uint16_t count600;
uint16_t ReadCount;
extern uint16_t Count,CCH,CCL,CCH2,CCL2;
int32_t BadSpeed;
uint32_t BadCCH2;

uint8_t MaxIn;
int16_t MaxError;
uint8_t DengerCount;
uint8_t SlowTimeFlag;
uint16_t OffspeeddecCount;
uint8_t adjAddtimeFlag;
uint16_t Off;
extern int16_t SaveSyncConst;
extern uint16_t syspassword;
uint8_t DoZDJzFlag=0;
uint8_t WorkBegin=0;
extern int16_t syncConst;
int32_t L,R;

extern uint16_t AddSpeedCount,DecSpeedCount;
extern uint16_t LowstSpeed; 

uint16_t SyncWarningVal;//=10;//40;
uint16_t OffVal;
uint8_t checkover;
uint32_t SyncBadCount;
uint8_t SlowSpeededFlag;
extern uint16_t  P;//10;//16;//4;//200; 小电机4 大电机200

extern uint32_t SumFactSpeed;
extern uint32_t FactSpeedSub[10];

int32_t PIDout;
uint32_t Saftycount;
uint16_t SyncCount;
extern uint8_t DoFingInitAngValFlag;
uint8_t FastSpeedFlag;
int16_t NowCommandSPEED; //RPM
uint16_t final_speed = 0;
uint8_t Safty;
extern uint8_t  iic_WRtemp[200];

uint8_t Dsp485RX2Buff[7];
uint8_t Dsp485TX2Buff[8];
uint8_t Pc485RX2Buff[8];
uint8_t Pc485TX2Buff[8];
extern uint8_t SyncCOMMAND;
uint16_t Count485;
int ArmErrorCode;
int32_t FactSpeed;//R/pm

extern u8 CODE7_180[17];
extern uint8_t HaveError;
extern int16_t Pc485RtuReg[100]; 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM8_Init(void);
static void MX_SPI1_Init(void);
static void MX_UART4_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM6_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
static void MX_IWDG_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

TIM_HandleTypeDef htim4;
#define KW0_ON  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); 
#define KW0_OFF HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

#define KW1_ON  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET); 
#define KW1_OFF HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET); 
//--------泵型号选择参数--------------------
uint16_t Vp[4]={500,600,100,100};
uint16_t Vi[4]={700,700,100,100};
uint16_t ADDTime[4]={8000,8000,10000,10000};
uint16_t DecTime[4]={1500,5000,10000,10000};
uint16_t OverPowerLiJuat12000[4]={57,200,57,57};   //12000降速力矩
uint16_t CanFastLowLiJu[4]={84,84,84,84};   //5800恢复升速力矩
//--------电机型号选择参数END--------------------
int16_t  SPITxBuffer[2]={0xaabb,0xCCdd};
int16_t 	SPIRxBuffer[2]={3,4};

uint8_t Safty2;
extern uint8_t RunFlag;
int16_t NowSpeed1,NowSpeed2;
int8_t CommandReturnCode;
uint8_t RePress;
uint8_t temp;

extern int16_t TestPidData;
int16_t TempPositonOut1,TempPositonOut2;
extern int16_t  NowError;
uint8_t AveEN;
int16_t PositionErrorAve;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */


uint8_t testGPIO;
float Vt_Kty81;

		float adcGetBatteryVoltage(void)
{
    HAL_ADC_Start(&hadc1);//启动ADC转化
    if(HAL_OK == HAL_ADC_PollForConversion(&hadc1,50))//等待转化完成、超时时间50ms
        return (float)HAL_ADC_GetValue(&hadc1)/4096*3.3;//计算电池电压
    return -1;
}
float KTY81(float Vt)
{		float T;
		float Rt;
		Rt=2000*Vt/(3.3-Vt);
		T=(Rt+37500)/1580;
		return T;
}
uint16_t x;
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM8_Init();
  MX_SPI1_Init();

  MX_USART1_UART_Init();      //PC485
  MX_USART2_UART_Init();     //DSP485
	
	
	
//	MX_IWDG_Init();
	MX_TIM2_Init();   //TIM2要于看门狗后初始化

  MX_TIM4_Init();
  
 

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

MX_ADC1_Init();

HAL_Delay(2000);//Wait DSP?

	HAL_TIM_Base_Start(&htim4 );
	
HAL_TIM_Base_Start_IT(&htim8 );
HAL_TIM_IC_Start_IT(&htim8 ,TIM_CHANNEL_1);
HAL_TIM_IC_Start_IT(&htim8 ,TIM_CHANNEL_2);
//----------------主master485接收开启----


//---------------------------DSP485------------------
 __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
HAL_UARTEx_ReceiveToIdle_IT(&huart2, Dsp485RX2Buff, 16);  //
C485L;
//---------------------PC4852-----------------------------
__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);//UART_IT_RXNE
HAL_UARTEx_ReceiveToIdle_IT(&huart1, Pc485RX2Buff, 11);  //
Pc485L;//PB5;


		HAL_Delay(100);
		TM1650_Init();
		HAL_Delay(100);
									
		TM1650_Set(0x6E,CODE7_180[0]);
    TM1650_Set(0x6C,CODE7_180[0]);
    TM1650_Set(0x6A,CODE7_180[0]);
    TM1650_Set(0x68,CODE7_180[0]);
		
	bsp_InitI2c();

	if(AT24C64_Check())
	{
					Pc485RtuReg[12]=100;   //
			    TM1650_Set(0x6E,CODE7_180[10]); //A 
   		    TM1650_Set(0x6C,CODE7_180[1]); //1
		
    	    TM1650_Set(0x6A,CODE7_180[0]); // 0
   		    TM1650_Set(0x68,CODE7_180[0]); // 0
					while(1);
	}


		eeprom_ReadBytes(iic_WRtemp,0,200); 
		memcpy(Pc485RtuReg,iic_WRtemp ,200);
	SaveSyncConst=Pc485RtuReg[33];
	if((Pc485RtuReg[25]==0xffff)&&(Pc485RtuReg[26]==0xffff))
	{
		Pc485RtuReg[25]=0;
		Pc485RtuReg[26]=0;
	}
	UsedTime=(((uint32_t)(Pc485RtuReg[25]))<<16)|((uint16_t)Pc485RtuReg[26]);
	NowCommandSPEED=600; 
	PreNowCommandSpeed=600;
	Pc485RtuReg[2]=600;
	//PreNowCommandSpeed=Pc485RtuReg[2];
	
	Pc485RtuReg[4]=0;  //角度查找停止
	Pc485RtuReg[3]=0;  //运行停止
		


		HAL_TIM_Base_Start_IT(&htim2 );
	



	Pc485RtuReg[33]=SaveSyncConst;   //同步数据
	Pc485RtuReg[16]=38;   //驱动器温度
	//AdjDecTime(0,3000);	//默认减速时间   1200慢点直通大气是可以的  1000停机不行   400泵1200可以，但180烧模块后，此处改为1500
//			HAL_Delay(300);
 init_PA();
 
 
// while(1)
//	 {                        //DSP发送McbspbRegs.DXR1由arm的Rx[1] 接收。 也就是DSP的1发送接收都是高位  2为低位
//									SPITxBuffer[0]=0xaabb;   //由DSP McbspbRegs.DRR2.all
//									SPITxBuffer[1]=0xccdd;	//由DSP McbspbRegs.DRR1.all 接收
//									HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
//									HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t * )&SPITxBuffer,(uint8_t * )&SPIRxBuffer,2); // 传输2个16位
//									HAL_Delay(100);
// }

BengParaCheck(Pc485RtuReg[38]);
SelfCheck();  //有错误死循环 ,需重启设备

   while (1)
  {	 
		
			if(BigError)  //如果中断中有错误产生，则在此循环并读取错误码，需要重上电
			{
					SelfCheck();
			}
			
			
		Vt_Kty81=adcGetBatteryVoltage();
		BoardT=KTY81(Vt_Kty81);
		Pc485RtuReg[16]=BoardT*10;
		if(Pc485RtuReg[16]>500)
		{
			Pc485RtuReg[3]=0x0;
			Pc485RtuReg[12]=500;
			TM1650_Set(0x6E,CODE7_180[10]); //A
							TM1650_Set(0x6C,CODE7_180[5]); //  3
								TM1650_Set(0x6A,CODE7_180[0]);  // 0  
								TM1650_Set(0x68,CODE7_180[0]);  //6 
		}
		
		//ReadDsp1RegITReiver(2,0x0809);   //现在只读Tn2  Tn1由SPI传送
		// HAL_Delay(10);


		//------------------------------------------------------------					
							

//			Pc485RtuReg[24]=(uint16_t)abs(NowError)*0.72;  //角度偏差 0.01度
			
		
						
						ReadDSPBusy=0;
						if((!HaveError)&&(RunFlag))DisSpeed(FactSpeed ); 
						else
						{
							DisSpeed(Pc485RtuReg[12]);
						}
						

	//					PowerNow=Pc485RtuReg[23]*FactSpeed;  左SPI接收中计算
						Pc485RtuReg[20]=Pc485RtuReg[22]*FactSpeed*0.008;     //(Tn/100)*9*400  =W  功率
						Pc485RtuReg[21]=Pc485RtuReg[23]*FactSpeed*0.008;
						
						Pc485RtuReg[18]=Pc485RtuReg[22];     //电流
						Pc485RtuReg[19]=Pc485RtuReg[23];
	
				
			//----------------------------------------------------------------------
		
	if(FactSpeed<1100)    //1000以内     是否能删掉保留else部分
  {
      SyncWarningVal=1000;     //500ms 250能通过
      OffVal=140; 
  }
  else
  {
    SyncWarningVal=50;//50//WaringConst;//35;// 1万0多转开始减速，停在1。1万转左右   //每边余量70以内  20可以上6K 不出错
    OffVal=45; 
  }


/////////////////////开机检测/////////////////////////
	
	if((Pc485RtuReg[3]==0x01)&&(Pc485RtuReg[4]==0x0)&&(HaveError==0)) //同步过程中禁止开机//运行检测 Reg[3] 1:运行 0：停止
  {
        Pc485RtuReg[4]=0x00; //       开机状态下禁止查找电机角度
         //------------命令开始，则检测启动条件-----------------
        if((!RunFlag)&(!RePress))  //运行处理
			  {				
								SelfCheck();//无错误才往下
								BeginSystemSyncProcess();
				}

		}
		else     //Pc485RtuReg[3]==0 //关机命令处理
		{
      StopMoto(); //关机条件及过程等判断
		
    }
		///////////////////////未运行时是否运行初始角度查找现改为HisSync(包括同步测量）////////////////////////
		if((Pc485RtuReg[3]==0)&&(Pc485RtuReg[4]==1)&&(HaveError==0) ) //双电机查找角度条件 DoFingInitAngValFlag为HisSYnc正在运行标志
		{
   			SelfCheck();//无错误才往下
				HisSyncAction();
				Pc485RtuReg[32]=0;//执行完一次清除密码
					Pc485RtuReg[4]=0;
		}
		
		
if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))   //强制直接停机
{
			HAL_Delay(20);
	if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))  //运行时可由机械开关开启
	{
					RunFlag=0;
				RePress=0;
				SyncCOMMAND=0; //同步停止
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);//    LED0=0;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);  //关 电机1
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);  //关 电机2
				FastSpeedFlag=0; //升速标志reset
					SyncCount=0;
					SyncBadCount=0;
					WorkBegin=0;
					adjAddtimeFlag=0;
					OffAdjFinishedFlag=0;
					Pc485RtuReg[2]=600;
					Pc485RtuReg[3]=0;
					Pc485RtuReg[12]=502;
					TM1650_Set(0x6E,CODE7_180[10]); //A
								TM1650_Set(0x6C,CODE7_180[5]); //  3
								TM1650_Set(0x6A,CODE7_180[0]);  // 0  
								TM1650_Set(0x68,CODE7_180[2]);  //6 
	}
}	
if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1))   //强制逐渐停机
{
		HAL_Delay(20);
	if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1))  //运行时可由机械开关开启
	{
		
		
			StopMoto(); //关机条件及过程等判断
			RunFlag=0;
			Pc485RtuReg[3]=0;
			PowerMaX=700000;  //关机或重开机功率恢复最大
			HighSpeedOk=0;
			Pc485RtuReg[3]=0;
	}
}	

if((BengSetup)&&(!Pc485RtuReg[3])&&(!Pc485RtuReg[4]))
{
	WriteBengType(BengSetup);
	BengSetup=0;  //执行完后恢复
	BengParaCheck(Pc485RtuReg[38]);
}

		
#if 0
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1))  //启动开关
		{
			HAL_Delay(200);
			if((!RunFlag)&(!RePress))  //�ﵽ��������ʱ
			 {
					    CheckBeforeRun();
              RunFlag=1;
							HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);//    LED0=1;
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
							HAL_Delay(2000);
				 
						
				 
              #if 0
              TestPidData=10 ;
              HAL_Delay(2000);
              AveEN=1; //启动平均速度
              while(AveEN);//等待平均速度结束
              TempPositonOut1=PositionErrorAve;
              TestPidData=-10 ;
              HAL_Delay(2000);
              AveEN=1; //启动平均速度
              while(AveEN);//等待平均速度结束
              TempPositonOut2=PositionErrorAve;

							if(abs(TempPositonOut1+TempPositonOut2)<100)
              {
                SyncCOMMAND=1; //同步开始
								}
              else    //同步失败
              {
                  RePress=0;
				          RunFlag=0;
					        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);//    LED0=0;
					        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
					        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
                  Pc485RtuReg[12]=300;   //0906读故障代码
			            TM1650_Set(0x6E,CODE7_180[10]); //A 
   		            TM1650_Set(0x6C,CODE7_180[3]); //3
		
    	            TM1650_Set(0x6A,CODE7_180[0]); // 0
   		            TM1650_Set(0x68,CODE7_180[0]); // 0
			            HaveError=1;
			            while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行 
              }
              #endif
              
						 
					
			}

		}
		else
		{
			 if(((RunFlag)&&(FactSpeed<800))||(!RunFlag))
			 {
			    RePress=0;
				  RunFlag=0;
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);//    LED0=0;
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET); 
			 }
		}
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		
//		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))          //同步开关
//		{
//			HAL_Delay(100);
//			if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))
//			{
//					SyncCOMMAND=1; //    
//          HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET);//    LED1=1;
//			}
//		}
//		else
//		{
//			HAL_Delay(100);
//			if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))
//			{
//				SyncCOMMAND=0; //    
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_RESET);//    LED1=0;
//			}
//		}
		
		
  }
  /* USER CODE END 3 */
}




static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}







/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
	
	//2组：0-4
	 //  3组：0-7
  /* TIM8_UP_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM8_UP_IRQn, 2, 0);   //
  HAL_NVIC_EnableIRQ(TIM8_UP_IRQn);
  /* TIM8_TRG_COM_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM8_TRG_COM_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM8_TRG_COM_IRQn);
  /* TIM8_CC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM8_CC_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);    //危险预警和 处理
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
	
	HAL_NVIC_SetPriority(USART1_IRQn, 2, 2); //PC485
  HAL_NVIC_EnableIRQ(USART1_IRQn);
	

  HAL_NVIC_SetPriority(USART2_IRQn, 2,1);   //DSP485
  HAL_NVIC_EnableIRQ(USART2_IRQn);
	
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 1);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	  HAL_NVIC_SetPriority(SPI1_IRQn, 2, 1);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS =	SPI_NSS_SOFT;//SPI_NSS_HARD_OUTPUT; 
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;    //1us
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2500;//1000;   //2.5ms
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 3;//17;//35;//72;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
//  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
//  if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 72;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 3;//17;//35;//71;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1F_ED;
  sSlaveConfig.TriggerFilter = 16;//3//4//8;
  if (HAL_TIM_SlaveConfigSynchro(&htim8, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_TIM_ConfigTI1Input(&htim8,TIM_TI1SELECTION_XORCOMBINATION);  //单刀开关选择XOR
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;//TIM_INPUTCHANNELPOLARITY_BOTHEDGE;//
  sConfigIC.ICSelection = TIM_ICSELECTION_TRC;//TIM_ICSELECTION_DIRECTTI;//
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter =16;//3;//4//8; 
  if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
		//HAL_TIM_ConfigTI1Input(&htim8,TIM_TI1SELECTION_XORCOMBINATION);  //单刀开关选择XOR 
 // sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;//TIM_INPUTCHANNELPOLARITY_BOTHEDGE;// 
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;//
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter =16; //3;//4//8;  
  if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */
__HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_UPDATE);
  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */
  HAL_NVIC_SetPriority(USART1_IRQn, 3, 1);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;//9600;//19200;//115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate =115200;// 9600;//
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_2;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_3|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PE2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 PC3 PC4 PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_3|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PD9 PD10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;//RISING; 无故障时灯亮，高电平
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	


  /*Configure GPIO pin : PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);




}

/* USER CODE BEGIN 4 */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
  hiwdg.Init.Reload = 10000;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



















