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
int32_t FactPower;
float FUTn22;
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
extern int32_t PowerNow,PowerNow2;
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
extern int16_t NowCommandSPEED; //RPM
extern int16_t Pc485RtuReg[100];

uint16_t LowstSpeed=600;
extern int32_t FactSpeed;//R/pm
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
//		HAL_IWDG_Refresh(&hiwdg); 
		//--------------------------------------------
				//	PowerNow=Pc485RtuReg[23]*FactSpeed;
					PowerNow2=Pc485RtuReg[22]*FactSpeed;//FUTn22*FactSpeed;
					FactPower=(PowerNow+PowerNow2)>>1;
				//	FftPowerNow=FftPowerNow+0.05*(PowerNow-FftPowerNow);		
				//	FftPowerNow=__fabs(FftPowerNow);
		
				//--------------检测---------------
				if(SyncCount>=R600Time)	SaftyCheck();  //不加限定一开始就升速了
					//---------------执行--------------
				//--------------------------------执行---2.5ms一次？-----------------------------------------
				
//							if((PreADDTimeData!=ADDTimeData)&&(SyncCount>=R600Time)&&(!ReadDSPBusy))  //调整加速时间
//							{
//									WriteDSPBusy=1;
//								//	AdjAddTime(0,ADDTimeData);   //减慢加速时间  Ver2.0不降加速时间
//							//		HAL_Delay(2);
//									WriteDSPBusy=0;
//									
//									PreADDTimeData=ADDTimeData;
//							}
				
				#if 0//-----------------------执行调速----------------------------------
						if((PreNowCommandSpeed!=NowCommandSPEED)&&(SyncCount>=R600Time)&&(!ReadDSPBusy))
						{				
						
							//------------------执行速度调整（此处可能为加可能为减）----------------
				
							if((RunFlag))//说明main中正在读被 中断打断，需要下一次中断再写最多延时2.5ms
							{

								AdjSpeed(0,NowCommandSPEED);				//现在已改为SPI高16位传递速度
							
								PreNowCommandSpeed=NowCommandSPEED;
						 }
					 }
				
				#endif
				//--------------读--------------------
			if(SaftyCheckCount<50)SaftyCheckCount++;  //100ms读一次
			else
			{
					SaftyCheckCount=0;
				//		///////////////////读Tn或Speed//115200下0.5ms////////////////////
		     if(SyncCount>=R600Time)
				 {
								
											ReadDSPBusy=1;
											ReadLiJuSpdFlag=1;
											ReadDsp1RegITReiver(2,0x0809);   //现在只读Tn2  Tn1由SPI传送
					}
			
				//--------------速度计算--------------------
				MyFactSpeed=MySpeedCount*9.6; // (MySpeedCount/50)*60/0.125 只用于力矩保护
				MySpeedCount=0;
					
					
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
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)   //����ص�
{
	if((htim->Instance == TIM8)){
		
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			{
						if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)&HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7))
						{
							FactError=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);//11ʱ�ɼ�10����
							if(Fhao==1)
							{
									//syncConst=FactError/2;   
							}
							else if(Fhao==-1)
							{
								//	syncConst=FactError/2;  //��ȡ���е����ֵ
									FactError=-FactError;
									
								
							}
									//---------------------------------------------
							//if(FactError >ZerrorMax)FactError=ZerrorMax;//ZerrorMax=FactError;
							//else if(FactError <FerrorMax)FactError=FerrorMax;//FerrorMax =FactError; 
							//syncConst=ZerrorMax -(FactError+ -1*FerrorMax)/2;
							//--------------------------------------------------------------							
								//	NowError=FactError;//+1500;//-syncConst; 1200
									BaiFenShu=FactError*1000/CCH2;
									if(GetDataEn)
									{		iBaiFenShu[BaiFenShuIndex]=BaiFenShu;
											if(BaiFenShuIndex<9)BaiFenShuIndex++;
											else BaiFenShuIndex=0;
									}

									NowError=BaiFenShu-syncConst;//SaveSyncConst;//FactError;//+100;
							if((SyncCOMMAND)&&(FactSpeed>1000))   //同步时会影响 所以只在正常运转用
							{							
								//if((NowError>200)|(NowError<-200))NowError=0;  //相当于滤波  正负200会出现 从12000到0时右机-01
								if((NowError>100)|(NowError<-100))NowError=0;  //相当于滤波 这里小了有的机子也不同步
							}
							ToCheckError=		NowError;
							 //----------------PID----------------

									PIDout= PIDencodr(ToCheckError);
									//if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1))PidSpi=0;
							//-----------------传输------------------
                  if(!SyncCOMMAND)
									{
										PidSpi=TestPidData;
									}
									else PidSpi=PIDout;
									if(PidSpi>200)PidSpi =200;    //10有的机子不能同步。现在改为50
									else if( PidSpi <-200)PidSpi=-200;
//									SPITxBuffer[1]=0;//PidSpi;  //注意：到对方是XR1 
//									SPITxBuffer[0]=0;	
//									HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
//									HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t * )&SPITxBuffer,(uint8_t * )&SPIRxBuffer,2); // 传输1个16位
									
									SPITxBuffer[0]=PidSpi;	   //DSP的2接收
									SPITxBuffer[1]=(uint16_t)NowCommandSPEED;    //也就是DSP的1发送接收都是高位  2为低位  此为DSP1接收 
									HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
									HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t * )&SPITxBuffer,(uint8_t * )&SPIRxBuffer,2); // 传输2个16位
									
////									HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
							////////////////////////////////////////////
//							CC1[0]=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
//							ccsum=CC1[0]+CC1[1]+CC1[2]+CC1[3]; 
						
						}
		//				else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)& !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7))CC1[1]=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		//				else if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)& HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7))CC1[2]=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		//				else if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)& !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7))CC1[3]=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
						
						
						
						
			//-----------------------------------------------
			
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{//7��������
				
				sh=~sh;
				//if(sh)HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_RESET);
				//else HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);
			
			//	CCH2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
			CCH2=__HAL_TIM_GET_COUNTER(&htim4); 
			__HAL_TIM_SET_COUNTER(&htim4,0);
      //if(CCH2<800)CCH2=800;
				 
				if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6))
				{	
					Fhao=1;  //7�����ظ�ʱ��6�Ѿ��Ǹߣ���   ����ǰ
					//FactError=CC1;//ȡ��һ�֡�
				}
				else
				{
					Fhao=-1; //7�����ظ�ʱ��6���ǵͣ�7��ǰ�ˡ�
					
				}
				MySpeedCount++;
		///		FactSpeed=21600000/CCH2;
			//	TempFactSpeed=21600000/CCH2;    //13333336/CCH2 为小齿轮 // 9600000/CCH2;//36为小齿轮 50为大齿轮1666667*36/50=1200000/CCH2;  *8是因为分频变为9了。
//	CCHSpeed=21600000/CCH2;
//				if(FactSpeedIndex<7)FactSpeedIndex++;
//				else FactSpeedIndex=0;
//				FactSpeedSub[FactSpeedIndex]=CCH2; 
				
				//600=10R/s   每格 1/500    周期。  ( 1/500   /  1s/18000000    )*600
				
				
			
			
				
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
		
  		FactSpeed=(int16_t)SPIRxBuffer[1];//if(SyncCOMMAND)	
		//if(SyncCOMMAND)
		//{
				Pc485RtuReg[22]=(int16_t)SPIRxBuffer[0];
				if(Pc485RtuReg[22]==0x7fff)Pc485RtuReg[22]=0;
			//	FUTn22=(int16_t)SPIRxBuffer[0];
			//	Pc485RtuReg[22]=FUTn22; // 同步时有正负 同步完成后再开这个传输 现在由L机的SPI传SPEED和Tn
				
		//}
	}

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxRxCpltCallback should be implemented in the user file
   */
}

//void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
