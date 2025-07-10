#include "function.h"
#include "Masterrtu.h"
#include "TM1650.h"
#include <string.h>
#include "24cxx.h" 
#include "tm1650.h"
#include "cmsis_armcc.h"
#include <stdlib.h>
uint16_t Vp,Vi,DecTime,ADDTime;
uint32_t OverPowerLiJuat12000,CanFastLowLiJu;
uint16_t BadSyncto600Count;
extern uint32_t PowerLimit;
uint8_t OnFist=0;
extern uint16_t  PreNowCommandSpeed;
extern uint16_t ReadDSPBusy;
uint16_t RiseCheckCount;
uint16_t PreADDTimeData;
uint16_t ADDTimeData;
int32_t FftPowerNow;
uint8_t RisePowerOver;
uint16_t HuiFuCount;	
uint8_t Ok6k,HighPowerOverFlag;
extern	int32_t BadSpeed;
										int32_t BadLiJu;
										int32_t BadPowerNow;
extern uint16_t MaxPowerCount;
uint16_t DeltaSpd;
uint8_t MaxLiJuSlowSpd=60;
extern uint32_t MyFactSpeed;
extern uint32_t PowerLimit;
extern int16_t ToCheckError;
extern uint8_t HighSpeedOk;
extern uint16_t WriteDSPBusy;
uint8_t BadLiJuL,BadLiJuR,BadError,BadLiJuCheck;

uint16_t LiJuSpeed;
uint8_t PingWenFlag;
uint16_t PingWenSpeedCount;
uint32_t PingWenSpeed;
extern uint32_t UsedTime;
extern uint8_t WriteUseTimeTOFlash;
uint8_t NoSaftyCount;
extern int32_t PowerNow;
extern uint32_t PowerMaX;
extern uint16_t LiJuLimit;
uint8_t HisSyncFlag;
extern uint16_t count600;
extern uint16_t R600Time;
extern uint8_t SlowTimeFlag;
extern uint8_t DengerCount;
extern uint16_t SyncWarningVal,OffVal;
extern int16_t  NowError;
extern uint8_t OffAdjFinishedFlag;
extern uint32_t SyncBadCount;  
extern uint8_t adjAddtimeFlag;
extern uint16_t SyncCount;
extern uint8_t FastSpeedFlag;    
int16_t  Tn2st;
uint16_t Tcount;
uint16_t CheckCount;
extern uint8_t  iic_WRtemp[200];
extern uint8_t WorkBegin;
extern int16_t SaveSyncConst;
extern uint16_t syspassword;
extern int16_t syncConst;
uint8_t GetDataEn;
extern int16_t TestPidData;
extern int32_t L,R;
extern int16_t iBaiFenShu[10];
extern uint8_t  BaiFenShuIndex;

extern uint16_t LowstSpeed;
	int16_t AsSpeed;
extern uint16_t Count,CCH,CCL,CCH2,CCL2;
uint8_t over;
uint8_t DoFingInitAngValFlag;
extern uint8_t Safty;
extern uint16_t Saftycount;
extern uint32_t FactSpeed;//Rpm
extern uint8_t SyncCOMMAND;
extern uint8_t RePress;
extern uint16_t NowCommandSPEED; //RPM
extern uint8_t RunFlag;
extern UART_HandleTypeDef huart2;
extern int16_t Pc485RtuReg[100];   
extern uint8_t Dsp485RX2Buff[7];
extern uint8_t Dsp485TX2Buff[8];
extern uint8_t CODE7_180[17];
uint8_t HaveError=0;
uint8_t outtemp;
#define DspSpeedAdd 0x0130
#define DspNowInitAngAdd 0x0902
#define DspPoweOnInitAngAdd 0x0907
#if 0
uint8_t MotoSysBeginRun(void) 
 {
		FindInitAng(1);
		HAL_Delay(100);
		FindInitAng(2);
		HAL_Delay(100);

		AdjSpeed(1,600);
		AdjSpeed(2,600);
        
 }
#endif
uint8_t CheckBeforeRun(void)
{
	uint16_t SP1,SP2;
	
	//--------------------------
	//ClearALMcom();//用自检代替，有错误不能往下运行
	
	
	//0------------------------检查两电机是否停止--------------------
		while(((SP1!=0)|(SP2!=0))&&(Tcount<10))
		{
					SP1=ReadDsp1Reg(1,0x0806);//读速度
					SP2=ReadDsp1Reg(2,0x0806);//读速度
					Tcount++;
					HAL_Delay(1000);
		}
	
	if(Tcount>=10)
	{
								Pc485RtuReg[12]=308;
								TM1650_Set(0x6E,CODE7_180[10]); 
								TM1650_Set(0x6C,CODE7_180[3]); 
								TM1650_Set(0x6A,CODE7_180[0]);   
								TM1650_Set(0x68,CODE7_180[8]);  
								HaveError=1;
								StopMoto();
								while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
								armReset();
	}
	
	
	
	//---------------------1-检查两电机有无报警----------------------
	if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9))
	{
			
	    Pc485RtuReg[13]=ReadDsp1Reg(1,0x0817);   //0906读故障代码 原自带 0817
			 TM1650_Set(0x6E,CODE7_180[1]); // 1
   		 TM1650_Set(0x6C,CODE7_180[16]); //-
		
    	 TM1650_Set(0x6A,CODE7_180[Pc485RtuReg[12]/10]);  
   		 TM1650_Set(0x68,CODE7_180[Pc485RtuReg[12]%10]);  
			//HaveError=1;
			while(1);   //可以在清报警命令后（HaveError=0）继续运行
		armReset();
	}
	if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10))
	{
	    Pc485RtuReg[14]=ReadDsp1Reg(2,0x0817);
		TM1650_Set(0x6E,CODE7_180[2]);
   		 TM1650_Set(0x6C,CODE7_180[16]);//-
    	 TM1650_Set(0x6A,CODE7_180[Pc485RtuReg[12]/10]);  
   		 TM1650_Set(0x68,CODE7_180[Pc485RtuReg[12]%10]); 
//HaveError=1;
		 while(1);
		armReset();
	}
	//----------------------2-检查两电机初始角度是否能对上-------要转一圈-同步时存储------------
	InitAngIfRight(1);
	InitAngIfRight(1);
	InitAngIfRight(2);
InitAngIfRight(2);
//---------------------3-检查两电机速度是否一致----------------------
	SendDSPCommand(0,0x0130,LowstSpeed);   //做中点校正固定500
	SP1=ReadDsp1Reg(1,0x0130);
	SP2=ReadDsp1Reg(2,0x0130);
	if(SP1!=SP2)
	{
		Pc485RtuReg[12]=203;
		 TM1650_Set(0x6E,CODE7_180[10]); //A
   		 TM1650_Set(0x6C,CODE7_180[2]); //  2
    	 TM1650_Set(0x6A,CODE7_180[0]);  // 0  
   		 TM1650_Set(0x68,CODE7_180[3]);  //3 
		 HaveError=1;
		 while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
		armReset();
	}
//	CCH2=1000;
	FactSpeed=0;
} 

uint8_t InitAngIfRight(uint8_t MotorNum)
{

    uint16_t PowerOnInitAng,NowInitAng;
	  int DltAng;
		
		uint16_t Timeout;
	
	SendDSPCommand(MotorNum,0x102b,0x01);	//执行查找电角度
//-------------检查是否开始-----------------
	
//	while((!CheckFindOver(MotorNum))|(Timeout<50))
//	{
//			HAL_Delay(400);
//			Timeout++;
//	}
//	Timeout=0;
//	if(Timeout>=50)
//	{
//		Pc485RtuReg[12]=204;// 查找角度不能开始
//	}
	//-------------检查是否完成--------------
	Timeout=0;
	outtemp =0;
	
		
	while((outtemp!=1)&&(Timeout<50))
	{
			outtemp=CheckFindOver(MotorNum);
			HAL_Delay(400);
			Timeout++;
	}
	if(Timeout>=50)
	{
		Pc485RtuReg[12]=205;// 查找角度没有成功
		while(1);     //停止运行
		armReset();
	}
	AsSpeed=ReadDsp1Reg(MotorNum,0x0908);  //读拖动时方向 
	if(AsSpeed<0)
	{
		Pc485RtuReg[12]=300+MotorNum;// UVW线接错
		TM1650_Set(0x6E,CODE7_180[10]); //A
   		 TM1650_Set(0x6C,CODE7_180[3]); //  3
    	 TM1650_Set(0x6A,CODE7_180[0]);  // 0  
   		 TM1650_Set(0x68,CODE7_180[MotorNum]);  //1 
				while(1);
		armReset();
	}
	NowInitAng=ReadDsp1Reg(MotorNum,0x0902);  //读当前同步后的角度
	if(NowInitAng>2048)
		{
				Pc485RtuReg[12]=400+MotorNum;// 得出的角度数值错
				TM1650_Set(0x6E,CODE7_180[10]); //A
   	 	 TM1650_Set(0x6C,CODE7_180[4]); // 4
    	 TM1650_Set(0x6A,CODE7_180[0]);  // 0  
   		 TM1650_Set(0x68,CODE7_180[6+MotorNum-1]);  // A406 A407
				while(1);
			armReset();
		}
	if(Pc485RtuReg[4]==1)  //如果是同步，则保存两电机初始角度数据
	{
				Pc485RtuReg[5+MotorNum-1]=NowInitAng;
				//保存至同步板
				memcpy(iic_WRtemp,&NowInitAng,2);
				eeprom_WriteBytes(iic_WRtemp,10+(MotorNum-1)*2,2);   //Reg5为mtot1,Reg6为moto2
	}
	else   //非同步时，进行对比
	{
				DltAng=NowInitAng-Pc485RtuReg[5+MotorNum-1];
				if(DltAng>400||DltAng<-400)
				{
								Pc485RtuReg[12]=200+MotorNum;   //查找的新角度和存储的差异过大
								TM1650_Set(0x6E,CODE7_180[10]); //A
					   		 TM1650_Set(0x6C,CODE7_180[2]); //  2
					    	 TM1650_Set(0x6A,CODE7_180[0]);  // 0  
					   		 TM1650_Set(0x68,CODE7_180[MotorNum]);  //1 
									HaveError=1;
								while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
					armReset();
				}
	}
}



void StopMoto(void)
{
  if(RunFlag)  //处于运行状态，需要判断速度停止条件，需要同步降至720以下才能完全停止
	{
		//if(NowCommandSPEED==LowstSpeed) //如果命令速度是600，则以实际速度为依据是否停止
		if(NowCommandSPEED==600) //如果命令速度0，则以实际速度为依据是否停止
		{
			//if(FactSpeed<LowstSpeed+20)
			//FactSpeed=ReadDsp1Reg(1,0x0806);
			if(FactSpeed<610)
			{
					
				RunFlag=0;
				RePress=0;
				SyncCOMMAND=0; //同步停止
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);//    LED0=0;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);  //关 电机1
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);  //关 电机2
				if(WriteUseTimeTOFlash)   //关机记录时间
				{
					memcpy(iic_WRtemp,&Pc485RtuReg[25],4);
					eeprom_WriteBytes(iic_WRtemp,50,4);
					WriteUseTimeTOFlash=0;
				}
				 FastSpeedFlag=0; //升速标志reset
					SyncCount=0;
					SyncBadCount=0;
					WorkBegin=0;
					adjAddtimeFlag=0;
					OffAdjFinishedFlag=0;
					Pc485RtuReg[2]=600;
					init_PA();
				
				//	AdjAddTime(0,30000);  //默认加速时间
			}
			else{}//继续等待速度下降	
		}
		else //如果命令速度不是LowstSpeed，则先降至LowstSpeed
		{
				NowCommandSPEED=600;//LowstSpeed;  //记录当前命令速度	 调整
				Pc485RtuReg[2]=600;
//				if(!OffAdjFinishedFlag)
//				{				AdjSpeed(0,600); //双保险 已经在主程序中调整
//					
//								HAL_Delay(100);
//				}
		}
	}
	else   //处于停止状态,或处于起动的中途RunFlag==0
	{
									
					        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);//    LED0=0;
					        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
					        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
									init_PA();

			
	} 
}

int16_t DoFingInitAngVal(uint8_t MotorNum)
{
	uint16_t TimeOut=0;
	uint16_t AngOut;
	over=0;
	FindInitAng(MotorNum);  //发送查找初始角度命令及判断回复是否超时
	HAL_Delay(500);
	
	over=0;
	while((over!=1)&&(TimeOut<30)  )//等待查找完成  15S 
	{
		over=CheckFindOver(MotorNum);
		HAL_Delay(300);
		TimeOut++;
	}
	if(TimeOut>=30)
	{
		Pc485RtuReg[12]=400+MotorNum;//电机找角度超时
		TM1650_Set(0x6E,CODE7_180[10]); //A
   		 TM1650_Set(0x6C,CODE7_180[4]); //  4
    	 TM1650_Set(0x6A,CODE7_180[0]);  // 0  
   		 TM1650_Set(0x68,CODE7_180[MotorNum]);  //0 
			HaveError=1;
									HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);//    LED0=0;
					        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
					        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
								
			while(HaveError);   
		armReset();
	}
	AsSpeed=0;
	AsSpeed=ReadDsp1Reg(MotorNum,0x0908);  //读拖动时方向 
	if(AsSpeed<0)
	{
		Pc485RtuReg[12]=300+MotorNum;// UVW线接错
		TM1650_Set(0x6E,CODE7_180[10]); //A
   		 TM1650_Set(0x6C,CODE7_180[3]); //  3
    	 TM1650_Set(0x6A,CODE7_180[0]);  // 0  
   		 TM1650_Set(0x68,CODE7_180[MotorNum]);  //2 
		while(1);  //不准软reset
	}
	
	AngOut=ReadDsp1Reg(MotorNum,0x0902);  //读当前角度;
	HAL_Delay(300);

	return AngOut;
}

void ClearALMcom(void)
{
	//清除报警  1022  //0904运行时间 
	SendDSPCommand(0,0x1022,0x01);  //两电机同时清除报警 
	Pc485RtuReg[27]=0;
}

void ResoverBit_Line(uint8_t BitNum)
{
	if(!BitNum)   //12bit，1024线
	{
		SendDSPCommand(0,0x090D,2008);  //输入密码
		HAL_Delay(100);
		SendDSPCommand(0,860,0x02);  //旋变分辨率 0 =10bit,1=14bit  2=12bit  3=16bit
		HAL_Delay(100);
		SendDSPCommand(0,861,0x02);  //改变旋变正交编码线数  
		// 0=256线(4倍后10位）。1=4096线(4倍后14位）。2=1024线（4倍后12位）。
		HAL_Delay(100);
	
		SendDSPCommand(0,847,1024);  //改变DSP接收编码器线数 1024=0x 0400
 	}
	else if(BitNum==1)  //14bit，4096线
	{
		SendDSPCommand(0,0x090D,2008);  //输入密码
		HAL_Delay(100);
		SendDSPCommand(0,860,0x01);  //旋变分辨率 0 =10bit,1=14bit  2=12bit  3=16bit
		HAL_Delay(100);
		SendDSPCommand(0,861,0x01);  //改变旋变正交编码线数 正交输出分辨率多少线  0=256线(4倍后10位）。1=4096线(4倍后14位）。2=1024线（4倍后12位）。  3= 16384 （4倍后16位65536）
		HAL_Delay(100);
		SendDSPCommand(0,847,4096);  //改变DSP接收编码器线数 4096=0x 1000
	}
   
}

void ResetDSP(void)
{
	SendDSPCommand(0,0x090D,2008);  //输入密码
	HAL_Delay(500);
	SendDSPCommand(0,0x1056,0x01);  //复位DSP
	HAL_Delay(500);
}

void AllZdFind(void)
{

		int16_t ZD;
		
		int8_t KK1,KK2;
	//----------中点查找------------------
							
							
							SyncCOMMAND=0; //进入测试模式
							TestPidData=0;
							Pc485RtuReg[22]=0;
							Pc485RtuReg[23]=0;
							CheckCount=0;
							Pc485RtuReg[22]=ReadDsp1Reg(1,0x0809);   //read Tn
							if(Pc485RtuReg[22]>-10)		TestPidData=10; //加压  -10电机2-TN   加正10，Tn1<0.-10Tn2<0
							else TestPidData=0; //已经小于-10，就不再加压，
							while(((Pc485RtuReg[22]>-10))&&(CheckCount<30 ))  //还没被拖动 等待时间5秒
							{ 
									Pc485RtuReg[22]=ReadDsp1Reg(1,0x0809);   //read Tn
								
									HAL_Delay(300);  //300ms
									CheckCount++;
								
							}
							if(CheckCount>=30)
							{
									Pc485RtuReg[12]=305;//查找中点起动失败
								
									TM1650_Set(0x6E,CODE7_180[10]); //A
									TM1650_Set(0x6C,CODE7_180[3]); //  3
									TM1650_Set(0x6A,CODE7_180[0]);  // 0  
									TM1650_Set(0x68,CODE7_180[5]);  //5 
									HaveError=1;
									StopMoto();
									while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
								armReset();
							}
									//--------------------------------------
								Pc485RtuReg[12]=0;//
								TM1650_Set(0x6E,CODE7_180[12]); //C
								TM1650_Set(0x6C,CODE7_180[2]); // 2
								TM1650_Set(0x6A,CODE7_180[2]);  // 2  
								TM1650_Set(0x68,CODE7_180[2]);  //2 
							//-----------------------------------------------
									GetDataEn=1;  //开始采集数据
									HAL_Delay(100);
									L=(iBaiFenShu[0]+iBaiFenShu[1]+iBaiFenShu[2]+iBaiFenShu[3]+iBaiFenShu[4]
									+iBaiFenShu[5]+iBaiFenShu[6]+iBaiFenShu[7]+iBaiFenShu[8]+iBaiFenShu[9])/10;
										GetDataEn=0;
								////////////////反方向////////////////////////////////
									Pc485RtuReg[22]=0;
									Pc485RtuReg[23]=0;
									CheckCount=0;
									if(Pc485RtuReg[22]<(-50))TestPidData=-20;
									else TestPidData=-10;
									while(((Pc485RtuReg[23]>-10))&&(CheckCount<100 ))  //还没被拖动 等待时间6秒
							  { 
										 
										Pc485RtuReg[23]=ReadDsp1Reg(2,0x0809); 
										HAL_Delay(300);  //300ms
										CheckCount++;
								
								}
								if(CheckCount>=100)
								{
									Pc485RtuReg[12]=305;//查找中点起动失败
								
									TM1650_Set(0x6E,CODE7_180[10]); //A
									TM1650_Set(0x6C,CODE7_180[3]); //  4
									TM1650_Set(0x6A,CODE7_180[0]);  // 0  
									TM1650_Set(0x68,CODE7_180[7]);  //7 
									HaveError=1;
									StopMoto();
									while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
									armReset();
								}
								//--------------------------------------
								Pc485RtuReg[12]=0;//
								TM1650_Set(0x6E,CODE7_180[12]); //C
								TM1650_Set(0x6C,CODE7_180[3]); // 2
								TM1650_Set(0x6A,CODE7_180[3]);  // 2  
								TM1650_Set(0x68,CODE7_180[3]);  //2 
							//-----------------------------------------------
									GetDataEn=1;  //开始采集数据
									HAL_Delay(500);
									R=(iBaiFenShu[0]+iBaiFenShu[1]+iBaiFenShu[2]+iBaiFenShu[3]+iBaiFenShu[4]
									+iBaiFenShu[5]+iBaiFenShu[6]+iBaiFenShu[7]+iBaiFenShu[8]+iBaiFenShu[9])/10;
									GetDataEn=0;
								//----------------  如果两个数的 符号位相同，则 (a ^ b) >= 0；

												//如果符号位不同，则 (a ^ b) < 0。
								if((L^R)>0) //符号位相同
								{ 
											if(L>R)
											{
												  syncConst=(L-R)/2+R;
												
											}
											else
											{
													syncConst=(R-L)/2+L;
												
											}
								}
								else      // 符号位不相同
								{
											syncConst=(L+R)/2;  //LR有正负 或同号
								}
								
								Pc485RtuReg[36]=(uint16_t)abs(L-R)*0.72;   //齿轮间隙360/50  =7.2度 *(L-R)/10  千分比
								
								
								
	
							//if((Pc485RtuReg[34]==1)&&(Pc485RtuReg[32]==syspassword))     //先发送存储命令和密码再按运行便能存储。
								if(Pc485RtuReg[4]==1)  //如果是同步过程，存储
								{
										memcpy(iic_WRtemp,&syncConst,2);
										eeprom_WriteBytes(iic_WRtemp,33*2,2);
										SaveSyncConst =syncConst;
										
								}
								else
								{
											if(((syncConst-SaveSyncConst)>50)|((syncConst-SaveSyncConst)<-50))   //如果不是同步
											{
													Pc485RtuReg[12]=304;
													TM1650_Set(0x6E,CODE7_180[10]); 
													TM1650_Set(0x6C,CODE7_180[3]); 
													TM1650_Set(0x6A,CODE7_180[0]);   
													TM1650_Set(0x68,CODE7_180[4]);  
													HaveError=1;
													StopMoto();
													while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
													armReset();
											}
								}
								//--------------------------------------
								Pc485RtuReg[12]=0;//
								TM1650_Set(0x6E,CODE7_180[12]); //C
								TM1650_Set(0x6C,CODE7_180[4]); // 2
								TM1650_Set(0x6A,CODE7_180[4]);  // 2  
								TM1650_Set(0x68,CODE7_180[4]);  //2 
							//-----------------------------------------------
							TestPidData=0;
}

void armReset(void)
{
		__HAL_RCC_CLEAR_RESET_FLAGS();
	  HAL_Delay(10);
		HAL_NVIC_SystemReset();
		//__set_FAULTMASK(1); 
	//SCB->AIRCR =(0x5FA<<SCB_AIRCR_VECTKEY_Pos)|SCB_AIRCR_SYSRESETREQ_Msk;
	
}

void SaftyCheck(void) 
{
	#if 0
	        if(SyncCount>=R600Time)
	        {
						//------------------Safty  产生   危险检测----------------------------
							if((ToCheckError<SyncWarningVal)&&(ToCheckError>-SyncWarningVal)&&(Pc485RtuReg[22]<90)&&(Pc485RtuReg[23]<95))  //偏差和力矩要不要不降只停。。。
							{ 
											NoSaftyCount=0; //恢复
											if(Saftycount<30){	Safty=0;Saftycount++;}  //2.5ms检测一次- 125ms内不能再次升速
											else 
											{
													
													Safty=1;
										      DengerCount=0;
													if(PingWenSpeedCount<3)PingWenSpeedCount++;
													else
													{
														PingWenFlag=1;    //当Safty=1后，PinWenFlag=0时，速度不变，平稳期。Falg=1后，开始加速
													}
											}												
              }
							else 
             {
										PingWenSpeedCount=0;
										PingWenFlag=0;
                    Saftycount=0;
										Safty=0;     
               }
	        }   //正式工作以前不检测
	        else
	        {
		          Safty =1;
	        }
	      #endif
				
				//----------------------根据危险算出调整速度----------------------------------------------		
			
				#if 0
			
				if((Safty)&&(RunFlag))  //安全时
				{
					if((SyncCount>=R600Time) &&(NowCommandSPEED !=Pc485RtuReg[2])&&(Pc485RtuReg[3]==1)) //要在开机状态,关机不跟 力短
					{
						if(SlowTimeFlag==1)//降过速
						{
										//WenFlag时间到后再加速，否则平稳过渡
									if(PingWenFlag)    //平稳期结束，可以加速了
									{
											NowCommandSPEED=Pc485RtuReg[2];
											//if(NowCommandSPEED<(Pc485RtuReg[2]-10))NowCommandSPEED +=10;//Pc485RtuReg[2];     //后面看要不要+1，逐渐到Reg[2]
											//else NowCommandSPEED=Pc485RtuReg[2];
									}
									else   //平稳期 速度稳定
									{
											NowCommandSPEED=FactSpeed;//NowCommandSPEED;
									}
							}//降速后
							else //刚开机 
							{
										NowCommandSPEED=Pc485RtuReg[2];
							}
					 }
				}
				else   if((!Safty)&&(RunFlag)&&(FactSpeed>1000))//不安全处理   //此处已经将指令速度减少2
				{
						
								if(Pc485RtuReg[3]==1)NowCommandSPEED=FactSpeed -5;//2.5ms减10 1秒减2000转
								SlowTimeFlag=1; //降速标志
				}
				#endif
				
				
					
				
				
	 // --------------------------功率限速-----------------------------------------------
			
			if((HighSpeedOk)&&(FactSpeed<6010))
				{
							HighPowerOverFlag=0;
							Ok6k=1;
					   HighSpeedOk=0;    //下到6000转不恢复则监控后面所有功率
				}//到达降速目的,准备新到达12000监控 与开机到6000转不一样
				if(FactSpeed>11950)
				{
						if(MaxPowerCount<1000)MaxPowerCount++;
						else
						{
							//	ADDTimeData=16000;
								MaxPowerCount=0;
								Ok6k=0;
								HighSpeedOk=1;    //开始监控功率
								OnFist=1;  //第一次开机完成，用于初次升速不监控功率
						}
						
				}
				//----------------------------------------------------------
				if(HighSpeedOk)//下降过程 只有下降到6000以下了才为0
				{
//					if(__fabs(PowerNow)>630000)  //直接掉到6K  力矩大约47  2.5ms一次 功率监控各个阶段
//					//if(__fabs(Pc485RtuReg[23])>50)    //3.58对应12000转力矩  冷机基本上43.热好机35 力矩监控12000
//					{
//								NowCommandSPEED=5000;	
//								HighPowerOverFlag=1;
//								
//					 }
					if(__fabs(Pc485RtuReg[23])>59)    //用左电机右电机易过载，只好移动右电机 。故加5
					//if(__fabs(Pc485RtuReg[22])>52) //  12K转时，4.6孔左52以上下降 。而4.2，3.58不降
					{
							//	NowCommandSPEED=6000;	
								NowCommandSPEED=5800;	 //6000最大孔容易力矩超100
								HighPowerOverFlag=1;
								
					 }
					 else
					 {
								if(!HighPowerOverFlag)NowCommandSPEED=Pc485RtuReg[2];//没过载才让用户调整。否则降到6K后才能调整。 不过载时 防止下降过程来回震荡
					 }
				}	
				else  //上升过程
				{
								//------------------力矩和偏差限制---------可能要独立出来，任何时候都需要检测--------------------------------
							if(((__fabs(Pc485RtuReg[23])>102)||(__fabs(Pc485RtuReg[22])>95)||(ToCheckError>SyncWarningVal)||(ToCheckError<-SyncWarningVal))&&(SyncCount>=R600Time)) //只监控第一次上升 停机，报错
							//if(((__fabs(Pc485RtuReg[23])>102)||(__fabs(Pc485RtuReg[22])>95))&&(SyncCount>=R600Time)) 	
							{
												if(BadSyncto600Count<800)BadSyncto600Count++;
												else
												{
														BadSyncto600Count=0;
													//	NowCommandSPEED=600;	
														Pc485RtuReg[12]=501;//
																										
												
													Pc485RtuReg[3]=0;
													TM1650_Set(0x6E,CODE7_180[10]); //A
													TM1650_Set(0x6C,CODE7_180[5]); //  
													TM1650_Set(0x6A,CODE7_180[0]);  //   
													TM1650_Set(0x68,CODE7_180[1]);  // 
													HaveError=1;
													while(HaveError);
													
													
												}

								}
								else if((OnFist)&&(Ok6k))//表示第二次上升 
								{
									if((FactSpeed<6010)&&(__fabs(Pc485RtuReg[23])<84))  //80小了，关了不升。太小    84 10.2的孔能关了马上升 4.6的孔6K转85.《84刚好能升
										//if((FactSpeed<6010)&&(__fabs(Pc485RtuReg[22])<72))  //6K转时，4.6孔左73
										{
													if(HuiFuCount<400)HuiFuCount++;    //1秒连续小于78才能升  第一次除外
													else
													{
															HuiFuCount=0;
															NowCommandSPEED=Pc485RtuReg[2];			//力矩小于78，可以升速
													}
										}
										else
										{
												if(Pc485RtuReg[2]<6000)NowCommandSPEED=Pc485RtuReg[2];//力矩过大停在6000	
										 }
								}
								else
								{
										NowCommandSPEED=Pc485RtuReg[2];
								}

				}										
					
				
}			
				
				
				
				
	


void FindSaveInitAngle(void)
{
					
          Pc485RtuReg[5]=DoFingInitAngVal(1); //查找初始角度
          eeprom_WriteBytes((uint8_t *)&Pc485RtuReg[5],10,2);//存储
					SendDSPCommand(1,0x090D,2008);  //输入密码
					HAL_Delay(200);
					SendDSPCommand(1,853,Pc485RtuReg[5]);//往DSP存储初始角度
					HAL_Delay(1000);
          Pc485RtuReg[6]=DoFingInitAngVal(2); //查找2初始角度
          eeprom_WriteBytes((uint8_t *)&Pc485RtuReg[6],12,2);//存储
					SendDSPCommand(2,0x090D,2008);
					HAL_Delay(200);
					SendDSPCommand(2,853,Pc485RtuReg[6]);
          DoFingInitAngValFlag=0; //标志位，查找完成，
          Pc485RtuReg[4]=0;//复位查找完成命令

}


void BeginSystemSyncProcess(void)
{
						
							
							HAL_Delay (100);
							SyncCOMMAND=0;  //测试模式
							TestPidData=0;
							RunFlag=1;
              HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);//    LED0=1;
					    CheckBeforeRun();
              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
							
							
						//已经在开始运转了
							HAL_Delay(500);
							while((FactSpeed<580)&&(count600<50))  //5秒
							{
									FactSpeed=ReadDsp1Reg(1,0x0806);	
									count600++;
									HAL_Delay(500);
							}
							
							if(count600>=50)  //超过5秒转速没达到580R
							{
								Pc485RtuReg[12]=306;//
								
								TM1650_Set(0x6E,CODE7_180[10]); //A
								TM1650_Set(0x6C,CODE7_180[3]); //  3
								TM1650_Set(0x6A,CODE7_180[0]);  // 0  
								TM1650_Set(0x68,CODE7_180[6]);  //6 
								HaveError=1;
								StopMoto();
								while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
								armReset();
							}
							////////////////起动600R成功
							Pc485RtuReg[12]=0;//
								
								TM1650_Set(0x6E,CODE7_180[12]); //
								TM1650_Set(0x6C,CODE7_180[1]); //  
								TM1650_Set(0x6A,CODE7_180[1]);  //   
								TM1650_Set(0x68,CODE7_180[1]);  // 
							///////////////////////////////////
							HAL_Delay(1000);
							AllZdFind();    //中点检测
							
							adjAddtimeFlag=0;
							SlowTimeFlag=0;
							
							SyncCOMMAND=1;    //同步开始 
							WorkBegin=1;  //进入正常工作倒计时
               //开始正常工作 
              //运转开始，同步是否开始在外面检测
						
}



void HisSyncAction(void)
{
						HisSyncFlag=1	;
						Pc485RtuReg[37]=1;
		
             BeginSystemSyncProcess();
							HAL_Delay(5000);
							StopMoto();
							
//							while((FactSpeed>0)&&(count600<30))  //15秒
//							{
//									FactSpeed=ReadDsp1Reg(1,0x0806);	
//									count600++;
//									HAL_Delay(500);
//							}
//							
//							if(count600>=30)  //超过5秒转速没停止
//							{
//								Pc485RtuReg[12]=403;//
//								
//								TM1650_Set(0x6E,CODE7_180[10]); //A
//								TM1650_Set(0x6C,CODE7_180[4]); //  4
//								TM1650_Set(0x6A,CODE7_180[0]);  // 0  
//								TM1650_Set(0x68,CODE7_180[3]);  //3 
//								HaveError=1;
//								StopMoto();
//								while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
//								armReset();
//								SyncCOMMAND=0;    
//								WorkBegin=0;  
//							}
						HisSyncFlag=0	;    //同步完成
						Pc485RtuReg[37]=0;
						
						SyncCOMMAND=0;    //同步开始 
						WorkBegin=0;  //进入正常工作倒计时
						adjAddtimeFlag=0;
						SlowTimeFlag=0;
					
		
}

void readDSPerrorCode(void)
	{
	uint16_t errorCode1,errorCode2;

	errorCode1=ReadDsp1Reg(1,0x0817);
	Pc485RtuReg[12]=errorCode1;
	TM1650_Set(0x6E,CODE7_180[10]); //A
	
   		 TM1650_Set(0x6C,CODE7_180[errorCode1/100]); // 
    	 TM1650_Set(0x6A,CODE7_180[errorCode1%100/10]);  
   		 TM1650_Set(0x68,CODE7_180[errorCode1%10]);  //
				HaveError=1;
		 while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
	armReset();
		
	
	errorCode2=ReadDsp1Reg(2,0x0817);
	TM1650_Set(0x6E,CODE7_180[10]); //A
	   		 TM1650_Set(0x6C,CODE7_180[errorCode1/100]); // 
    	 TM1650_Set(0x6A,CODE7_180[errorCode1%100/10]);    
   		 TM1650_Set(0x68,CODE7_180[errorCode1%10]);  //
				HaveError=1;
		 while(HaveError);   //可以在清报警命令后（HaveError=0）继续运行
	armReset();
}
	

uint16_t CalculatSpeed(uint16_t LiJu)
{
		uint16_t out;
		out=540000/LiJu;
	if(out>Pc485RtuReg[2])out=Pc485RtuReg[2];//安全范围，故实现用户速度
	return out;
}

void init_PA(void)
{
	Pc485RtuReg[3]=0;
		ADDTimeData=8000;
		PreADDTimeData=8000;
		PowerLimit=540000;  //关机或重开机功率恢复最大
		HighSpeedOk=0;
	Ok6k=0;
										OnFist=0;
									SyncCOMMAND=0;
	TestPidData=0;
								//	PreStop=0;
									RePress=0;
				          RunFlag=0;
									Safty=0;
									Saftycount=0;
	Pc485RtuReg[2]=600;
	
}


 void SelfCheck(void)
{
//	//----------------------电机型号--------------------------------
//		if((Pc485RtuReg[38]!=Pc485RtuReg[39])||(Pc485RtuReg[38]>4)||(Pc485RtuReg[38]<1)||(Pc485RtuReg[39]>4)||(Pc485RtuReg[39]<1))
//		{
//				Pc485RtuReg[12]=550;
//			 TM1650_Set(0x6E,CODE7_180[10]); // A 550 电机型号匹配错误
//   		 TM1650_Set(0x6C,CODE7_180[5]); //-
//		
//    	 TM1650_Set(0x6A,CODE7_180[5]);  
//   		 TM1650_Set(0x68,CODE7_180[0]);  
//			 while(1);
//		}
	
	//-----------------------dsp故障wadc----------------------------
			Pc485RtuReg[13]=ReadDsp1Reg(1,0x0817);   //0906读故障代码
		if(Pc485RtuReg[13]!=0)
		{
				Pc485RtuReg[12]=Pc485RtuReg[13];
			 TM1650_Set(0x6E,CODE7_180[1]); // 1
   		 TM1650_Set(0x6C,CODE7_180[16]); //-
		
    	 TM1650_Set(0x6A,CODE7_180[Pc485RtuReg[13]/10]);  
   		 TM1650_Set(0x68,CODE7_180[Pc485RtuReg[13]%10]);  
			 while(1);
		}
		Pc485RtuReg[14]=ReadDsp1Reg(2,0x0817);   //0906读故障代码
		if(Pc485RtuReg[14]!=0)
		{
				Pc485RtuReg[12]=Pc485RtuReg[14];
			 TM1650_Set(0x6E,CODE7_180[2]); // 1
   		 TM1650_Set(0x6C,CODE7_180[16]); //-
		
    	 TM1650_Set(0x6A,CODE7_180[Pc485RtuReg[14]/10]);  
   		 TM1650_Set(0x68,CODE7_180[Pc485RtuReg[14]%10]);  
			 while(1);
		}
}

uint8_t WriteBengType(uint8_t Index)
{
	
	
			switch (Index)
			{
				case 1:
				{
						Vp=500;
						Vi=700;
						DecTime=1500;
						ADDTime=8000;
						OverPowerLiJuat12000=57;
						CanFastLowLiJu=84;
					
				}break;
				case 2:
				{
				}break;
				case 3:
				{
				}break;
				case 4:
				{
				}break;
				default:
				{
				}
			}
			AdjAddTime(0,ADDTime);  //默认加速时间
			HAL_Delay(200);
			AdjDecTime(0,DecTime);	//默认减速时间   1200慢点直通大气是可以的  1000停机不行   400泵1200可以，但180烧模块后，此处改为1500
			HAL_Delay(200);
			SendDSPCommand(0,102,Vp);
			HAL_Delay(200);
			SendDSPCommand(0,103,Vi);
			HAL_Delay(200);
		
			
}

void DisError(uint8_t A,uint16_t Code)
{
		Pc485RtuReg[12]=Code;
		TM1650_Set(0x6E,CODE7_180[A]); 
		TM1650_Set(0x6C,CODE7_180[Code/100]); 
		TM1650_Set(0x6A,CODE7_180[Code%100/10]);  
		TM1650_Set(0x68,CODE7_180[Code%10]);  
		while(1);
}
void BengParaCheck(uint8_t BengType )
{
			if((Pc485RtuReg[38]==Pc485RtuReg[39])&&(Pc485RtuReg[38]<5)&&(Pc485RtuReg[38]>0))//泵型匹配
			{
				switch(BengType)
				{
					case 1:
					{
						Vp=500;
						Vi=700;
						DecTime=1500;
						ADDTime=8000;
						OverPowerLiJuat12000=57;
						CanFastLowLiJu=84;
					
						}break;
						case 2:
						{
						}break;
						case 3:
						{
						}break;
						case 4:
						{
						}break;
						default:
						{
						}
					}
					if((ReadDsp1Reg(1,306)!=ReadDsp1Reg(2,306))||(ReadDsp1Reg(1,307)!=ReadDsp1Reg(2,307)))DisError(10,560);
					if((ReadDsp1Reg(1,306)!=ADDTime)||(ReadDsp1Reg(1,307)!=DecTime))DisError(10,560);
					
					if((ReadDsp1Reg(1,102)!=ReadDsp1Reg(2,102))||(ReadDsp1Reg(1,103)!=ReadDsp1Reg(2,103)))DisError(10,561);
					if((ReadDsp1Reg(1,102)!=Vp)||(ReadDsp1Reg(1,103)!=Vi))DisError(10,561);
			 }
			else  //泵型不匹配
			{
				DisError(10,550);
			}
}