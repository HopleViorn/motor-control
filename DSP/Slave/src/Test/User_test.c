#include "EDBclm.h"			// define and function declare
#include "globalvar.h"		// 
#include "testmode.h"
//=============================================================================
//	main program
//=============================================================================
void	Main_Test(void){
	if(TestPara.Test_Wave && TestPara.Test_Mode){
		TestData.Begin=TestPara.Start_Test;
		if(!TestData.Begin){
			TestData.TestFinish=1;
		}
		if(TestData.Begin && TestData.TestFinish){//start test
			//clear position
			Clear_Test_Data();
			//caculate scan time
			Caculate_Time();
			//save data
			Save_Data_Flag();
		}
	}
}
//=============================================================================//
void	Test_Program(void){
	if(!TestData.TestFinish){
		switch(TestPara.Test_Wave){
			case POSITION_RAMP:
				//give pulse
				Test_Position_Ramp();

				break;
			case POSITION_SINUSOIDAL:
				//
				Test_Position_Sin();
				break;
			case POSITION_STEP:
				//
				Test_Position_Step();
				break;
			case VELOCITY_TRAPE:
				//
				Test_Velocity_Trape();
				break;
			default:
				TDPosition.PulseInc=0;//in stop state
				break;
		}
	}else{
		TDPosition.PulseInc=0;//in stop state
	}
}
//=============================================================================
//	input
//		Distance:The largest distance in motor moving; (dct:pulse)
//		Velocity:The Velocity is constance;(dct:rpm)
//		Repeats:The test times;
//		Revers:The Revers move;
//		Stoptime:The stop time;
//	output:
//		Pulse:The pulse in control
//		FinishFlag:The Finish flag
//=============================================================================
void	Test_Position_Ramp(void){
//Test.Position.Distance
	long	pulsesum;
	if(!TestData.TestFinish){
		switch(TDPosition.Stop_Step){
			case FIRST_STEP:
				TDPosition.PulseInc=SpeedToPulse(TestPara.Velocity);//caculate the
				pulsesum=TDPosition.PulseSum + TDPosition.PulseInc;
				if(pulsesum>=TDPosition.Distance_All){
					TDPosition.PulseInc=TDPosition.Distance_All-TDPosition.PulseSum;//The last time pulse
		
					TDPosition.Stop_Step=SECOND_STEP;
					TDPosition.PulseSum=0;
				}else
					TDPosition.PulseSum +=TDPosition.PulseInc;
				break;
			case SECOND_STEP:
				TDPosition.PulseInc=SpeedToPulse(TestPara.Velocity);//caculate the
				pulsesum=TDPosition.PulseSum + TDPosition.PulseInc;
				if(pulsesum>=TDPosition.Distance_All){
					TDPosition.PulseInc=TDPosition.Distance_All-TDPosition.PulseSum;//The last time pulse
					TDPosition.PulseSum=0;
					if(++TDPosition.Repeat_Time>=TestPara.RepeatTimes){
						TDPosition.Stop_Step=THIERD_STEP;
					}else
					TDPosition.Stop_Step=FIRST_STEP;//Next step
				}else
					TDPosition.PulseSum +=TDPosition.PulseInc;
				if(!TestPara.CWCCW){//0:two direction
					TDPosition.PulseInc=-TDPosition.PulseInc;
				}
				break;
			case THIERD_STEP:
				TDPosition.PulseInc=0;
				if((++TDPosition.Count_Time/10)>=TestPara.StopTime * 10){
					TestData.TestFinish=1;
					TestData.ScanFinish=1;
					TestPara.Start_Test=0;
				}
				break;
			default:
				break;
		}
		if(Position_Falg==-1)TDPosition.PulseInc=-TDPosition.PulseInc;
	}
}
void	Test_Position_Step(void){
	if(!TestData.TestFinish){
		switch(TDPosition.Stop_Step){
			case FIRST_STEP:
				if(++TDPosition.Integral_Time==1){
					TDPosition.PulseInc=TDPosition.Distance_All;
				}else TDPosition.PulseInc=0;
				if((TDPosition.Integral_Time/10)>TestPara.RunTime){
					TDPosition.Stop_Step=SECOND_STEP;
					TDPosition.Integral_Time=0;
				}
				break;
			case SECOND_STEP:
				if(++TDPosition.Integral_Time==1){
					if(!TestPara.CWCCW){//0:two directio
					TDPosition.PulseInc=-TDPosition.Distance_All;
					}else TDPosition.PulseInc=TDPosition.Distance_All;
				}else TDPosition.PulseInc=0;
				if((TDPosition.Integral_Time/10)>TestPara.RunTime){
					if(++TDPosition.Repeat_Time>=TestPara.RepeatTimes){
						TDPosition.Stop_Step=THIERD_STEP;
					}else
					TDPosition.Stop_Step=FIRST_STEP;//Next step
					TDPosition.Integral_Time=0;
				}
				break;
			case THIERD_STEP:
				TDPosition.PulseInc=0;
				if((++TDPosition.Count_Time/10)>=TestPara.StopTime){
					TestData.TestFinish=1;
					TestData.ScanFinish=1;
					TestPara.Start_Test=0;
				}
				break;
			default:
				break;
		}
		if(Position_Falg==-1)TDPosition.PulseInc=-TDPosition.PulseInc;
	}
}

void	Test_Velocity_Trape(void){
	if(!TestData.TestFinish){
		switch(TDPosition.Stop_Step){
			case FIRST_STEP:
				if(inPgcnt1){
					Pulse_give();
				}else{
					TDPosition.Stop_Step=SECOND_STEP;
					if(!TestPara.CWCCW)direction=1;
					else direction=0;
					inPgcnt1=TDPosition.Distance_All;
					clear_remain_data();
				}
				break;
			case SECOND_STEP:
				if(inPgcnt1){
					Pulse_give();
				}else{
					if(++TDPosition.Repeat_Time>=TestPara.RepeatTimes){
						TDPosition.Stop_Step=THIERD_STEP;
					}else{
						clear_remain_data();
						TDPosition.Stop_Step=FIRST_STEP;//Next step
						inPgcnt1=TDPosition.Distance_All;
						direction=0;
					}
					outPgerr=0;
				}
				//if(Position_Falg==-1)TDPosition.PulseInc=-TDPosition.PulseInc;
				break;
			case THIERD_STEP:
				TDPosition.PulseInc=0;
				if((++TDPosition.Count_Time/10)>=TestPara.StopTime){
					TestData.TestFinish=1;
					TestData.ScanFinish=1;
					TestPara.Start_Test=0;
				}
				break;
			default:
				break;
		}
		if(direction)
		TDPosition.PulseInc=-outPgerr;
		else TDPosition.PulseInc=outPgerr;
		if(Position_Falg==-1)TDPosition.PulseInc=-TDPosition.PulseInc;
				
	}
}
void	Pulse_give(void){
	Uint32 v_acc,v_dec;
	int32 inPgstep;
	if(SigmaPgerr1 >= TEST_PP_velocity_plan_pars1.dec_position)//开始减速
	{
		PosSPEEDINS1 = 0;										
	}else
	{
		PosSPEEDINS1 = TEST_PP_velocity_plan_pars1.target_v;
	}
	v_acc = TEST_PP_velocity_plan_pars1.acc;
	v_dec = v_acc;
	PosSPEEDINSout1 = trap_speed_control1(v_acc,v_dec,PosSPEEDINS1,PosSPEEDINSout1);
	// Unit: p/s -> p/100us
	if(!PosSPEEDINSout1){
		step_posmore.ACCorSPEEDin=memEncNum/15;
		/*if (memCurLoop00.hex.hex00 != Encoder_Abs17bit)
		step_posmore.ACCorSPEEDin = 167;//1rpm
		else step_posmore.ACCorSPEEDin = 2185;//1rpm*/
	}else step_posmore.ACCorSPEEDin = PosSPEEDINSout1;	
	step_posmore.Agear = 1;
	step_posmore.Bgear = 10000;
	acc_speed_unit_convers1( & step_posmore);		// Unit: p/s -> p/100us	
	inPgstep = step_posmore.ACCorSPEEDout;

	if(labs(inPgcnt1) < labs(inPgstep))
	{
		outPgerr = inPgcnt1;
		inPgcnt1 = 0;
	}else{
		inPgcnt1 -= inPgstep;
		outPgerr = inPgstep;
	}
	SigmaPgerr1 += abs(outPgerr);
}
// 单位换算函数
// acc/dec:	xxx -> 0.1rpm/s
// speed:	xxx -> 0.1rpm
void acc_speed_unit_convers1(volatile ACC_OR_SPEED_UNIT_CONVER1 * v)
{int32 Tmp,pulsemax;
 long long TMP;
	pulsemax = (int32)500000;
	TMP = (long long)v->ACCorSPEEDin;
	v->ACCorSPEEDremain += (int32)(TMP % v->Bgear);
	TMP = TMP/v->Bgear;	
	if(llabs(TMP) > 0x7FFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		v->ACCorSPEEDout=0;
		return;
	}
	Tmp = TMP;
	if(labs(v->ACCorSPEEDremain) >= v->Bgear)
	{
		
		if(v->ACCorSPEEDremain > 0)
		{
			v->ACCorSPEEDremain -= v->Bgear;
			Tmp++;
		}
		else
		{
			v->ACCorSPEEDremain += v->Bgear;
			Tmp--;
		}
	}

	if(labs(Tmp) < pulsemax)//position
	{
		v->ACCorSPEEDout = Tmp;
	}else{
		if(Tmp > 0)
		{
			v->ACCorSPEEDout = pulsemax;
		}
		else
		{
			v->ACCorSPEEDout = -pulsemax;
		}
	}
}

// PP_MODE梯形加/减速控制,acc Unit: p/s/s -> p/s/100us, velocity Unit: p/s
// PV_MODE梯形加/减速控制,acc Unit: xxx -> 0.1rpm/s -> 0.1rpm/100us, velocity Unit: 0.1rpm
// Mode = 0: PP
// Mode = 1: PV
Uint32 trap_speed_control1(Uint32 acc_a,Uint32 dec_a,int32 SPPEEDINS, int32 SPPEEDINSout)
{int32 acc_inc,dec_inc;	
	if(SPPEEDINS > SPPEEDINSout)		// acc
	{
		if(acc_a)
		{
			acc_aster2more.ACCorSPEEDin = acc_a;					// Unit: p/s -> p/s/100us
			acc_aster2more.Agear = 1;
			acc_aster2more.Bgear = 10000;
			acc_speed_unit_convers1( & acc_aster2more);					// Unit: xxx/s -> xxx/100us
			acc_inc = acc_aster2more.ACCorSPEEDout;
		}else{
			acc_aster2more.ACCorSPEEDin = MAX_ACC;			// Unit: p/s -> p/s/100us
			acc_aster2more.Agear = 1;
			acc_aster2more.Bgear = 10000;
			acc_speed_unit_convers1( & acc_aster2more);					// Unit: xxx/s -> xxx/100us
			acc_inc = acc_aster2more.ACCorSPEEDout;
		}
		if((SPPEEDINSout + acc_inc) < SPPEEDINS)
		{
			SPPEEDINSout += acc_inc;
		}else
		{
			SPPEEDINSout = SPPEEDINS;
			acc_aster1more.ACCorSPEEDremain = 0;
			acc_aster2more.ACCorSPEEDremain = 0;
			dec_aster1more.ACCorSPEEDremain = 0;
			dec_aster2more.ACCorSPEEDremain = 0;
		}
	}else if(SPPEEDINS < SPPEEDINSout)	// dec
	{
		if(dec_a)
		{
			dec_aster2more.ACCorSPEEDin = dec_a;					// Unit: p/s -> p/s/100us	
			dec_aster2more.Agear = 1;
			dec_aster2more.Bgear = 10000;
			acc_speed_unit_convers1( & dec_aster2more);					// Unit: xxx/s -> xxx/100us
			dec_inc = dec_aster2more.ACCorSPEEDout;
		}
		else
		{
			dec_aster2more.ACCorSPEEDin = MAX_ACC;			// Unit: p/s -> p/s/100us
			dec_aster2more.Agear = 1;
			dec_aster2more.Bgear = 10000;
			acc_speed_unit_convers1( & dec_aster2more);					// Unit: xxx/s -> xxx/100us
			dec_inc = dec_aster2more.ACCorSPEEDout;
		}
		if(SPPEEDINSout > (SPPEEDINS + dec_inc))
		{
			SPPEEDINSout -= dec_inc;
		}else
		{
			SPPEEDINSout = SPPEEDINS;
			acc_aster1more.ACCorSPEEDremain = 0;
			acc_aster2more.ACCorSPEEDremain = 0;
			dec_aster1more.ACCorSPEEDremain = 0;
			dec_aster2more.ACCorSPEEDremain = 0;
		}
	}else
	{
		SPPEEDINSout = SPPEEDINS;		
		acc_aster1more.ACCorSPEEDremain = 0;
		acc_aster2more.ACCorSPEEDremain = 0;
		dec_aster1more.ACCorSPEEDremain = 0;
		dec_aster2more.ACCorSPEEDremain = 0;
	}
	return SPPEEDINSout;
}
//
int	SpeedToPulse(int speed){
	signed long	temp,temp1,Pulse_Inc,Pulse_Incremain;
	temp=(signed long)1500000;//1000
	temp1=(signed long)(TestData.Encoder>>2) * TestData.Period;//
	temp1=temp1* speed;
	Pulse_Inc=temp1/temp;
	Pulse_Incremain=temp1%temp;
	TDPosition.Remain +=Pulse_Incremain;
	if(labs(TDPosition.Remain)>=temp){
		if(TDPosition.Remain>0){
			Pulse_Inc++;
			TDPosition.Remain -=temp;
		}else{
			Pulse_Inc--;
			TDPosition.Remain +=temp;
		}
	}
	return	Pulse_Inc;
}
//=============================================================================
//	input
//		Distance:The largest distance in motor moving; (dct:pulse)
//		Frequency:Hz;
//		Repeats:The test times;
//		Revers:The Revers move;
//		Stoptime:The stop time;
//	output:
//		Pulse:The pulse in control
//		FinishFlag:The Finish flag
//=============================================================================
void	Test_Position_Sin(void){
	int32	Distance_Now,temp,temp1;
	if(!TestData.TestFinish){
		switch(TDPosition.Stop_Step){
			case FIRST_STEP:
				temp=(int32)TestData.Period * TestPara.Frequency;
				temp1=(int32)100000/temp;
				TDPosition.Integral_Time ++;
				if(TDPosition.Integral_Time>=temp1){
					TDPosition.Integral_Time=TDPosition.Integral_Time-temp1;
					if(++TDPosition.Repeat_Time>=TestPara.RepeatTimes){
						TDPosition.Stop_Step=SECOND_STEP;
					}
				}
				temp=((int32)TDPosition.Integral_Time<<16)/temp1;
				//
				temp=_IQ16sinPU(temp);
				temp=temp>>2;
				temp1=TDPosition.Distance_All>>2;
				Distance_Now=(temp * temp1)>>12;
				temp1=Distance_Now-TDPosition.Sin_Distance;
				if(labs(temp1)>32767){
					if(temp1<0)TDPosition.PulseInc=-32767;
					else TDPosition.PulseInc=32767;
				}else TDPosition.PulseInc=temp1;
				if(TestPara.CWCCW){
					if(TDPosition.PulseInc<0)TDPosition.PulseInc=-TDPosition.PulseInc;
				}
				TDPosition.Sin_Distance=Distance_Now;
				if(Position_Falg==-1)TDPosition.PulseInc=-TDPosition.PulseInc;
				break;
			case SECOND_STEP:
				TDPosition.PulseInc=0;
				if((++TDPosition.Count_Time/10)>=TestPara.StopTime){
					TestData.TestFinish=1;
					TestData.ScanFinish=1;
					TestPara.Start_Test=0;
				}
				break;
			default:
				break;
		}
	}
}
//=============================================================================
//	input
//		Distance:The largest distance in motor moving; (dct:pulse)
//		Velocity:The Velocity is constance;(dct:rpm)
//		Repeats:The test times;
//		Revers:The Revers move;
//		Stoptime:The stop time;
//	output:
//		Scan_time:scan time
//		StartFlag:start save data
//=============================================================================
void	Caculate_Time(void){
	int32	temp,temp1,Remain,Quotient;
	long long longdata;
	switch(TestPara.Test_Wave){
			case POSITION_RAMP:
				
				temp=(int32)TestData.Encoder * TestPara.Velocity;
				temp1=(int32)600 * TDPosition.Distance_All;
				longdata=(long long)TestPara.RepeatTimes * temp1;
				Quotient=(int32)(longdata/temp);// repeat times
				Remain=(int32)(longdata%temp);
				if(!Quotient){
					Quotient=1;
					if(Remain>(temp>>1)){
						if(!TestPara.CWCCW)Quotient=Quotient<<1;//one direction
					}
				}else{
					if(Remain>(temp>>1)){
						Quotient++;
						if(!TestPara.CWCCW)Quotient=Quotient<<1;//one direction
					}else{
						if(!TestPara.CWCCW)Quotient=1+Quotient<<1;
						else Quotient=1+Quotient;
					}
				}
				temp1=(int32)TestPara.StopTime /100;//stop time
				temp=Quotient + temp1;// +1;
				TestPara.Scan_Time=temp;
				break;
			case POSITION_SINUSOIDAL:
				temp=(Uint32)10000 * TestPara.RepeatTimes/TestPara.Frequency;//100us
				if(TestPara.CWCCW)temp=temp>>1;//one direction
				temp1=(Uint32)TestPara.StopTime * 10;
				temp=temp + temp1;
				if(temp>1000){
					TestPara.Scan_Time=temp/1000 + 1;//scan time in ever time
				}else TestPara.Scan_Time=1;
				break;
			case POSITION_STEP:
				temp=((Uint32)TestPara.RunTime<<1)* 10;
				temp=temp * TestPara.RepeatTimes;
				temp1=(Uint32)TestPara.StopTime * 10;
				temp=temp + temp1;
				if(temp>1000){
					TestPara.Scan_Time=temp/1000 + 1;//scan time in ever time
				}else TestPara.Scan_Time=1;
				if(TDPosition.Distance_All>32767)TDPosition.Distance_All=32767;
				break;
			case VELOCITY_TRAPE:
				TEST_PP_velocity_plan_pars1.acc=(long long)TestPara.Acceleration * memEncNum/15;//p/s/s
				TEST_PP_velocity_plan_pars1.target_v = (Uint32)TestPara.Velocity * memEncNum/15;//p/s
				/*if(memCurLoop00.hex.hex00 == Encoder_Abs17bit){
					TEST_PP_velocity_plan_pars1.acc=(long long)TestPara.Acceleration * 32768/15;//p/s/s
					TEST_PP_velocity_plan_pars1.target_v = (Uint32)TestPara.Velocity * 32768/15;//p/s
				}else{
					TEST_PP_velocity_plan_pars1.acc=(long long)TestPara.Acceleration * 500/3;//p/s/s
					TEST_PP_velocity_plan_pars1.target_v = (Uint32)TestPara.Velocity * 500/3;//p/s
				}*/
				
				TEST_PP_velocity_plan_pars1.target_position=TDPosition.Distance_All;
				temp=test_pos_speed_plan1(& TEST_PP_velocity_plan_pars1);
				temp=temp * TestPara.RepeatTimes;
				temp1=(Uint32)TestPara.StopTime * 10;
				temp=temp + temp1;
				if(temp>1000){
					TestPara.Scan_Time=temp/1000 + 1;//scan time in ever time
				}else TestPara.Scan_Time=1;
				inPgcnt1=TDPosition.Distance_All;
				break;
			default:
				break;
		}
}

Uint32 test_pos_speed_plan1(volatile TEST_PP_VELOCITY_PLAN_PARS1 * pars)
{// cauclate dec pos
long long square_v0,square_v1,square_v2,temp;
Uint32 v0,v1,v2,T,t1,t2;			// Unit: p/s
Uint32 a1,a2;						// Unit: p/s/s
int32 all_distance,acc_distance,dec_distance,increase_distance;
	v0 = 0;
	v1 = pars->target_v;
	v2 = 0;
	a1 = pars->acc;
	a2 = a1;

	square_v0 = (long long)v0*v0;
	square_v1 = (long long)v1*v1;
	square_v2 = (long long)v2*v2;

	increase_distance = pars->target_position;
	all_distance = labs(increase_distance);
	// cacu_Acc_Dec_distance
	acc_distance = (llabs(square_v1 - square_v0)>>1)/a1;
	dec_distance = acc_distance;
	if((acc_distance + dec_distance) > all_distance)
	{
		pars->dec_position = ((long long)all_distance*a2*2 + llabs(square_v2 - square_v0))/((long long)(a1 + a2)*2);
		temp=(long long)4 * pars->dec_position * 100000000/a1;
		t1=_IQ1sqrt(temp);
		T=t1<<1;		//100us
	}
	else
	{
		pars->dec_position = all_distance - (llabs(square_v1 - square_v2)>>1)/a2;
		t1=labs(v1) * 625/(a1>>4);//s->100us
		temp=(long long)a1 * t1 * t1/((Uint32)100000000);
		t2=(long long)(all_distance-temp) * 10000/labs(v1);
		T=(t1<<1)+t2;	//100us
		T=T<<1;
	}
	return T;
}
//=============================================================================
//	start 
//=============================================================================
void	Save_Data_Flag(void){
	//save data
	savecnt = 0;
	sci_oper_status.bit.sciSampleStatus = 1;
	sci_oper_flag.bit.sciStartSample = 1;
	sci_oper_flag.bit.SciSampleing = 0;
	sci_data.SCISampleTrig = 0;					//no triger
	sci_data.SCISampleTime = TestPara.Scan_Time;
	wComm = SAMPLE_BUF;							// 保存的首地址
	TestData.TestFinish=0;						//start
	TestData.ScanFinish=0;
}
void	Init_TestData(void){
	test_abs_position=0;
	test_abs_position_more=0;
	Position_Falg=0;
	TestData.Begin=0;
	TestData.Encoder=0;
	TestData.Period=10;
	TestData.ScanFinish=0;
	TestData.TestFinish=1;

	TestPara.Acceleration=1000;
	TestPara.CWCCW=0;
	TestPara.Distance_H=0;
	TestPara.Distance_L=10000;
	TestPara.Frequency=1;
	TestPara.RepeatTimes=1;
	TestPara.RunTime=100;
	TestPara.Scan_Number=0;
	TestPara.Scan_Time=0;
	TestPara.Sonoff=0;
	TestPara.Start_Test=0;
	TestPara.StopTime=0;
	TestPara.Test_Mode=0;
	TestPara.Test_Wave=1;
	TestPara.Velocity=100;
	
	TDPosition.Count_Time=0;
	TDPosition.Distance_All=0;
	TDPosition.Integral_Time=0;
	TDPosition.PulseInc=0;
	TDPosition.PulseSum=0;
	TDPosition.Remain=0;
	TDPosition.Repeat_Time=0;
	TDPosition.Sin_Distance=0;
	TDPosition.Stop_Step=0;
	TDPosition.Velocity=0;
}
//===========================================================================
//	
//===========================================================================
void Control_Use(void){
	if(TestPara.Test_Wave<19){//position mode
		Pgerr=TDPosition.PulseInc;
	}else{
		Pgerr=0;
		Pgerro = 0;
	}
	Filtime_pos = Pn[POSFiltTimeADD];
	runposition();
}
//-----------------------------------------------------------------------------
// clear test data
//-----------------------------------------------------------------------------
void Clear_Test_Data(void){
	int32 temp,temp1;
	//clear all data
	TestData.Begin=0;	//
	//
	//position data
	temp=((int32)TestPara.Distance_H<<16)+TestPara.Distance_L;
	temp1=labs(temp);
	if(temp1>MAX_Distance)temp1=MAX_Distance;
	else if(!temp1)temp1=1;
	if(temp<0)Position_Falg=-1;
	else Position_Falg=1;
	TDPosition.Distance_All=temp1;
	if(!TestPara.Velocity || TestPara.Velocity>3000)TestPara.Velocity=10;//for test
	if(!TestPara.RepeatTimes || TestPara.RepeatTimes>10)TestPara.RepeatTimes=1;
	if(!TestPara.RunTime || TestPara.RunTime>32767)TestPara.RunTime=100;
	if(!TestPara.Acceleration)TestPara.Acceleration=100;
	if(!TestPara.Frequency>50)TestPara.Frequency=1;
	if(TestPara.StopTime>32767)TestPara.StopTime=32767;
	TDPosition.Stop_Step=FIRST_STEP;
	TDPosition.Repeat_Time=0;
	TDPosition.Count_Time=0;
	TDPosition.Integral_Time=0;
	TDPosition.Remain=0;
	TDPosition.Sin_Distance=0;
	TDPosition.PulseSum=0;
	//clear data
	Pgerr=0;
	Pgerro = 0;
	Pos_forward_filter.remain = 0;
	Pos_forward_filter.OutData = 0;
	Pos_forward_filter.OutValue = 0;
	Ek = 0;
	RmEk = 0;
	RmFg = 0;	
	test_abs_position = 0;
	Pgplus = 0;
	//load encoder
	//if (memCurLoop00.hex.hex00 == Encoder_Abs17bit)
	//	TestData.Encoder=(Uint32)1<<17;
	//else TestData.Encoder=(Uint32)2500<<2;
	TestData.Encoder=(Uint32)memEncNum<<2;
	clear_remain_data();
	direction = 0;
}
void	clear_remain_data(void){
	Pos_forward_filter.remain = 0;
	Pos_forward_filter.OutData = 0;
	Pos_forward_filter.OutValue = 0;
	acc_aster1more.ACCorSPEEDout = 0;
	acc_aster1more.ACCorSPEEDremain = 0;
	acc_aster2more.ACCorSPEEDout = 0;
	acc_aster2more.ACCorSPEEDremain = 0;
	dec_aster1more.ACCorSPEEDout = 0;
	dec_aster1more.ACCorSPEEDremain = 0;
	dec_aster2more.ACCorSPEEDout = 0;
	dec_aster2more.ACCorSPEEDremain = 0;
	step_posmore.ACCorSPEEDout = 0;
	step_posmore.ACCorSPEEDremain = 0;
	PosSPEEDINS1 = 0;
	PosSPEEDINSout1 = 0;
	SigmaPgerr1 = 0;
	outPgerr = 0;
}
//=============================================================================
