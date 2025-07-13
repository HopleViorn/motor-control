/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pid.h"
#include "SPI.h"
#include "Function.h"
#include "Masterrtu.h"
#include "Pc485slave.h"
#include <string.h>
#include "tm1650.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
uint16_t WritedDelayCount;
extern uint16_t PreADDTimeData;
uint8_t BigError=0;
extern uint8_t HaveError;
extern u8 CODE7_180[17];
extern uint16_t ADDTimeData;
extern int32_t FftPowerNow;
int32_t DltaSpd;
uint16_t AdjspdDelayCount;
uint8_t ReadLiJuSpdFlag;
int32_t MyFactSpeed;
uint32_t MySpeedCount;
int16_t ToCheckError;
uint16_t UsedTimeCount;
uint32_t UsedTime;
extern uint8_t HighSpeedOk;
//uint32_t HighSpeedOkCount;
extern int32_t PowerNow;
extern uint32_t PowerMaX;
uint32_t PowerLimit=540000;//500000;//500000;
uint16_t MaxPowerCount;
extern uint16_t ReadDSPBusy;
uint16_t R600Time=4000;//0;//后面要改成40000.

uint16_t WriteDSPBusy;
uint16_t SaftyCheckCount;
uint8_t WriteDspDelayCount1,WriteDspDelayCount2;
extern uint16_t  PreNowCommandSpeed;
extern uint8_t OffAdjFinishedFlag;
int32_t CCHSpeed;
extern uint16_t AvrFactSpeed;
extern IWDG_HandleTypeDef hiwdg;
extern uint8_t DengerCount; 
extern uint8_t SlowTimeFlag;
extern uint8_t adjAddtimeFlag;
extern uint32_t Saftycount; 
extern uint16_t SyncWarningVal,OffVal;
extern uint16_t OffspeeddecCount;
extern uint16_t Off;
int16_t SaveSyncConst;
extern uint8_t WorkBegin;
extern uint8_t GetDataEn;
int16_t iBaiFenShu[10];
uint8_t  BaiFenShuIndex,BeginStep;
extern uint8_t  BaiFenShuIndex;
uint16_t AddSpeedCount,DecSpeedCount;
extern uint16_t NowCommandSPEED; //RPM
extern int16_t Pc485RtuReg[100];

uint16_t LowstSpeed=600;
extern uint32_t FactSpeed;//R/pm
extern uint32_t SyncBadCount;
uint8_t  FactSpeedIndex;
 uint16_t TempFactSpeed;//R/pm
uint32_t SumFactSpeed;
uint32_t FactSpeedSub[10];
extern int32_t PIDout;
extern uint16_t SyncCount;
extern SPI_HandleTypeDef hspi1;

 extern uint16_t  P;//=16;//4;//200; 小电机4 大电机200

extern int16_t Pc485RtuReg[100]; 
extern uint8_t Safty;

uint16_t DangerousCount;

uint8_t PositionErrorIndex;
int16_t PositionError[50];
int32_t PositionErrorSum; 
uint8_t SyncCOMMAND=0;
int16_t TestPidData=0;
extern uint8_t AveEN;
extern int16_t PositionErrorAve;

extern TIM_HandleTypeDef htim4;
extern uint16_t  SPITxBuffer[2];
extern uint16_t 	SPIRxBuffer[2];
extern SPI_HandleTypeDef hspi1;
extern int32_t EncodrIError;
int16_t syncConst=0;
int16_t  NowError;

int32_t PidSpi;
	uint16_t Count,CCH,CCL,CCH2,CCL2;
	
	uint16_t CC1[4],ccsum;
	int16_t BaiFenShu;
	
	int16_t FactError;
	
	
	int8_t  Fhao;
uint32_t hhh;
uint8_t sh;
uint8_t RunFlag;
extern uint8_t RePress;

int32_t ZerrorMax=2000;//500;
int32_t FerrorMax=-2000;//-500;

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern SPI_HandleTypeDef hspi1;
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim8;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */
void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi1);
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  //2.5ms
{
	
	//-------------------------------------------
		if (htim->Instance == TIM2)    //2.5ms
		{
			//-----------开机时间统计----------------
				if(UsedTimeCount<400)UsedTimeCount++;
				else
				{
					UsedTimeCount=0;
					if(RunFlag)UsedTime++;
					Pc485RtuReg[25]=(UsedTime&0xffff0000)>>16; 
					Pc485RtuReg[26]=(UsedTime&0x0000ffff);
				}
		//-----------------weigou------------------
		HAL_IWDG_Refresh(&hiwdg); 
		//--------------------------------------------
					PowerNow=Pc485RtuReg[23]*FactSpeed;
					FftPowerNow=FftPowerNow+0.05*(PowerNow-FftPowerNow);		
					FftPowerNow=__fabs(FftPowerNow);
		
				//--------------检测---------------
					SaftyCheck();
					//---------------执行--------------
				//--------------------------------执行---2.5ms一次？-----------------------------------------
				
							if((PreADDTimeData!=ADDTimeData)&&(SyncCount>=R600Time)&&(!ReadDSPBusy))  //调整加速时间
							{
									WriteDSPBusy=1;
								//	AdjAddTime(0,ADDTimeData);   //减慢加速时间  Ver2.0不降加速时间
							//		HAL_Delay(2);
									WriteDSPBusy=0;
									
									PreADDTimeData=ADDTimeData;
							}
				
				#if 1//-----------------------执行调速----------------------------------
						if((PreNowCommandSpeed!=NowCommandSPEED)&&(SyncCount>=R600Time)&&(!ReadDSPBusy))
						{				
						
							//------------------执行速度调整（此处可能为加可能为减）----------------
				
							if((RunFlag))//说明main中正在读被 中断打断，需要下一次中断再写最多延时2.5ms
							{
								WriteDSPBusy=1;
								AdjSpeed(0,NowCommandSPEED);				//关机时先在stop子程序里将NowCommandSPEED=600,然后在此处调速
								//HAL_Delay(3);
								  //写双DSP无回复，故无法稳步中断里 
								PreNowCommandSpeed=NowCommandSPEED;
						 }
					 }
					if(WriteDSPBusy)
					{
						 if(WritedDelayCount<40) WritedDelayCount++;// 如果写后100ms后再读
						{
								WritedDelayCount=0;
								WriteDSPBusy=0;
						}
					}
				#endif
//				//--------------读--------------------
			if(SaftyCheckCount<100)SaftyCheckCount++;  //250ms读一次
			else
			{
					SaftyCheckCount=0;
				//		///////////////////读Tn或Speed//115200下0.5ms////////////////////
		     if(SyncCount>=R600Time)
				 {
						if(!WriteDSPBusy)   //总线空闲读 Tn 是否空闲从写开始，到收到结束
						{
									if(!ReadLiJuSpdFlag)
									{
											ReadDSPBusy=1;
											ReadLiJuSpdFlag=1;
											ReadDsp1RegITReiver(1,0x0809);
										//	Pc485RtuReg[22]=ReadDsp1Reg(1,0x0809);
									}	
									else
									{
											ReadDSPBusy=1;
											ReadLiJuSpdFlag=0;
											ReadDsp1RegITReiver(1,0x0806);
										//	FactSpeed=ReadDsp1Reg(1,0x0806);   //总线空闲读 speed
																			}
											//ReadDSPBusy=0; //移到收到中断里去结束
						}
					}
			
				
			}
		
		
		
			
				
				
				

//				//-------------------------------------------
//				//------------------PID功率速度------------------
//		DltaSpd=PIDPowerSpd(PowerLimit,PowerNow);
//	  if(!Safty)DltaSpd=0;
////		if(DltaSpd<(-NowCommandSPEED+600))DltaSpd=(-NowCommandSPEED+600);
////		else
////		{
////				NowCommandSPEED=NowCommandSPEED+DltaSpd;
////		}
//		 if(DltaSpd<(FactSpeed-600))DltaSpd=(FactSpeed-600);
//			else
//			{
//				NowCommandSPEED=FactSpeed+DltaSpd;
//			}
//				
//				
//		if(NowCommandSPEED>Pc485RtuReg[2])NowCommandSPEED=Pc485RtuReg[2];
//		else if(NowCommandSPEED<600)NowCommandSPEED=600;
		
					
				
				
					
			
		
			
					
		//		Pc485RtuReg[22]=ReadDsp1Reg(1,0x0809); 
		//		Pc485RtuReg[23]=ReadDsp1Reg(2,0x0809); 	
		//		FactSpeed=ReadDsp1Reg(1,0x0806); 					
	
			
			//	if(WriteDspDelayCount1<40)WriteDspDelayCount1++; // 2.5*40=100ms 第一个加时间的延时还没到
			//	if(WriteDspDelayCount2<40)WriteDspDelayCount2++; 
			//	if((WriteDspDelayCount1>20)&&(WriteDspDelayCount2>20))WriteDSPBusy=0;
	//--------------------------------------------------------------------

		
	
			////////////////////开机时600转等待//////////////////////////////////////
				//if((RunFlag ==1)&&(FactSpeed> (LowstSpeed-50)))
				if(	WorkBegin)
				{	
				//	if(SyncCount<500)SyncCount++;  //等中点检测完毕自动开始同步
				   //允许同步
				  if(SyncCount<R600Time)SyncCount++;    //开机加速等待。 目前4000，后面要改成40000.在define里改
				   //允许升速
				} 
			//	if(OffspeeddecCount<10)OffspeeddecCount++;//减速计数/
				
				////////////////////////////////////////////
		//		if((DecSpeedCount<500)&&(SyncCount>=4000))	DecSpeedCount++;   // 1.25s内不能再次减速//
				//if(AddSpeedCount<500)	AddSpeedCount++;   // 1.25s内不能再次加速//
				
				
		
		}
}
/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)          //2.5ms
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
	

			
  /* USER CODE BEGIN TIM2_IRQn 1 */
	
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)   
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles TIM8 update interrupt.
  */
void TIM8_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_UP_IRQn 0 */
//Count++;
  /* USER CODE END TIM8_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_UP_IRQn 1 */
//	hhh=	__HAL_TIM_GetCounter(&htim8 );
	
  /* USER CODE END TIM8_UP_IRQn 1 */
}

/**
  * @brief This function handles TIM8 trigger and commutation interrupts.
  */
void TIM8_TRG_COM_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_TRG_COM_IRQn 0 */

  /* USER CODE END TIM8_TRG_COM_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_TRG_COM_IRQn 1 */

  /* USER CODE END TIM8_TRG_COM_IRQn 1 */
}

/**
  * @brief This function handles TIM8 capture compare interrupt.
  */
void TIM8_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_CC_IRQn 0 */

  /* USER CODE END TIM8_CC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_CC_IRQn 1 */

  /* USER CODE END TIM8_CC_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */

  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN UART4_IRQn 1 */

  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)   // 输入捕获中断回调函数
{
	// 首先，检查中断是否由TIM8触发。TIM8用于处理编码器信号。
	if((htim->Instance == TIM8)){
		
		// 接下来，使用 htim->Channel 来区分是哪个具体的通道触发了中断。
		// 这允许在同一个回调函数中为不同的事件编写不同的处理逻辑。
		
		// --- 分支1: TIM8 通道1中断 ---
		// 功能: 测量两个电机之间的角度/相位偏差，并执行PID同步校正。
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			{
						if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)&HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7))
						{
							// 1. 读取由TIM8自身捕获的计数值。这个值代表了两个电机编码器信号的相位差（时间差）。
							FactError=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
							
							if(Fhao==1)
							{
									//syncConst=FactError/2;   
							}
							else if(Fhao==-1)
							{
								//	syncConst=FactError/2;  //获取反向的中点值
									FactError=-FactError;
							}
							
							// 2. 使用由TIM4测得的周期CCH2来归一化误差，消除速度变化对偏差测量的影响。
							BaiFenShu=FactError*1000/CCH2;
							
							if(GetDataEn)
							{		iBaiFenShu[BaiFenShuIndex]=BaiFenShu;
									if(BaiFenShuIndex<9)BaiFenShuIndex++;
									else BaiFenShuIndex=0;
							}

							// 3. 计算出最终用于PID控制器的误差。
							NowError=BaiFenShu-syncConst;
							
							if((SyncCOMMAND)&&(FactSpeed>1000))   //同步时会影响 所以只在正常运转用
							{							
								if((NowError>100)|(NowError<-100))NowError=0;  //相当于滤波
							}
							ToCheckError = NowError;
							
							// 4. 执行PID计算，得到修正量。
							PIDout= PIDencodr(ToCheckError);
							
							// 5. 通过高速SPI接口发送PID修正指令，对从电机进行实时同步校正。
							if(!SyncCOMMAND)
							{
								PidSpi=TestPidData;
							}
							else PidSpi=PIDout;
							if(PidSpi>10)PidSpi =10;
							else if( PidSpi <-10)PidSpi=-10;
							
							SPITxBuffer[0]=PidSpi;	
							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
							HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t * )&SPITxBuffer,(uint8_t * )&SPIRxBuffer,1); // 传输1个16位
						}
			}
		// --- 分支2: TIM8 通道2中断 ---
		// 功能: 测量单个电机转动一个齿轮齿所需的时间周期，用于计算电机转速。
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			sh=~sh;
			if(sh)HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_RESET);
			else HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);
			
			// 1. 读取自由运行的TIM4的计数值，得到上一个齿到这个齿之间的时间周期。
			CCH2=__HAL_TIM_GET_COUNTER(&htim4); 
			
			// 2. 立刻将TIM4计数器清零，为下一次周期测量做准备。
			__HAL_TIM_SET_COUNTER(&htim4,0);
      
			// 3. 根据编码器信号判断电机转动方向。
			if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6))
			{	
				Fhao=1;  // 7脚边沿来时，6脚已是高，说明...
			}
			else
			{
				Fhao=-1; // 7脚边沿来时，6脚还是低c，说明...
			}
			MySpeedCount++;
		}
	}
}

//---------------------------------EXTI LINE CALLBACK----------------------------------
void EXTI9_5_IRQHandler(void)
{
  
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
 
}
void EXTI15_10_IRQHandler(void)
{
  
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
 
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	
	if(GPIO_Pin==GPIO_PIN_9)
	{
    HAL_Delay(10);
		if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9))
		{
		      RunFlag=0;
					Pc485RtuReg[3]=0;
			    RePress=1;
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);//    LED0熄灭;
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET); 
					BigError=1;
				
		}
	}
  else if(GPIO_Pin==GPIO_PIN_10)
	{
    HAL_Delay(10);
		if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10))
		{
		      RunFlag=0;
					Pc485RtuReg[3]=0;
			    RePress=1;
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);//    LED0熄灭;
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
					BigError=1;			
	
	  }
  }
	//读0x0817,读报警编号 到主存序去读


}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi==&hspi1)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi==&hspi1)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Prevent unused argument(s) compilation warning */
  //UNUSED(hspi);
	if(hspi==&hspi1)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	//	if(SyncCOMMAND)Pc485RtuReg[23]=(int16_t)SPIRxBuffer[1]; // 同步时有正负 同步完成后再开这个传输
	//	MyFactSpeed=	(int16_t)SPIRxBuffer[0];	
		if(SyncCOMMAND)Pc485RtuReg[23]=(int8_t)SPIRxBuffer[0]; // 同步时有正负 同步完成后再开这个传输
	}

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxRxCpltCallback should be implemented in the user file
   */
}

//void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/