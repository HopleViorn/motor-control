/*******************************************************************************/
#include "CANopen.h"
/*******************************************************************************/
/*****************************************************************************************
*Copyright:    	xxxxxCorporation                                                            
*
*File name:    	CO_DS402.c                                                                 
*
*Author:    	yizhenhua                                                                  
*
*Version:    	V1.0                                                                        
*
*Date:
******************************************************************************************/
#if RAM_RUN
#pragma CODE_SECTION(CO_PP_mode, "ramfuncs1");
#pragma CODE_SECTION(CO_IP_mode, "ramfuncs1");
#pragma CODE_SECTION(deal_maxmin_pos, "ramfuncs2");
//#pragma CODE_SECTION(deal_maxmin_speed, "ramfuncs2");
#pragma CODE_SECTION(PP_IP_Status, "ramfuncs2");
#endif

//#pragma CODE_SECTION(acc_speed_unit_convers, "ramfuncs2");//2011-08-26

#define Pos_Polarity		(Polarity & 0x80)
#define Spd_Polarity		(Polarity & 0x40)

#define Steady_Speed          0
#define	AccAdd_Increase_Speed 1
#define	AccSteady_Increase_Speed 2
#define AccSub_Increase_Speed 3
#define	DecAdd_Decrease_Speed 4
#define	DecSteady_Decrease_Speed 5
#define	DecSub_Decrease_Speed   6
//the next three status just only used for quikstop
#define	Stop_AccSub_Increase_Speed 7
#define Stop_DecAdd_Decrease_Speed 8
#define Stop_DecSteady_Decrease_Speed  9
#define Stop_DecSub_Decrease_Speed 10

typedef	struct{	
				volatile int32  target_position[2];
				volatile Uint32 profile_velocity;
				volatile Uint32 profile_velocity_new;
				volatile Uint32 end_velocity;
				volatile Uint32 profile_acc;
				volatile Uint32 profile_dec;
				volatile Uint32 quick_dec;
				volatile Uint16 status;			// 0: no new target; 1: new target,but not load; 2:new target
}CO_PP_MODE_PARS;
//typedef CO_PP_MODE_PARS *CO_PP_MODE_PARS_handle;

volatile CO_PP_MODE_PARS CO_PPmode_pars  = {0,0,0,0,0,0,0,0,0};

typedef	struct{	 
				volatile Uint32 start_v;	
				volatile Uint32 target_v; 
				volatile Uint32 end_v;				   		
				volatile Uint32 acc;
				volatile Uint32 dec;
				volatile Uint32 quickdec;
				volatile int32  target_position;
				volatile int32  idea_actual_position;
				volatile int32  idea_start_position;
				volatile int32  increase_pulse;
				volatile int32  dec_position;
				volatile int32	dec_length;

}CO_PP_VELOCITY_PLAN_PARS;
//typedef CO_PP_VELOCITY_PLAN_PARS *CO_PP_VELOCITY_PLAN_PARS_handle;

volatile CO_PP_VELOCITY_PLAN_PARS CO_PP_velocity_plan_pars = {0,0,0,0,0,0,0,0,0,0};

typedef	struct{
				volatile Uint32 Agear;	
				volatile Uint32 Bgear;
				volatile int32 ACCorSPEEDin;				//输入变量
				volatile int32 ACCorSPEEDremain;   			//输出余数
				volatile int32 ACCorSPEEDout;				//输出变量
}ACC_OR_SPEED_UNIT_CONVER;									// 单位换算 /s --> /100us
//typedef ACC_OR_SPEED_UNIT_CONVER *ACC_OR_SPEED_UNIT_CONVER_handle;

//ip
typedef	struct{	
				volatile Uint16 NewSync;
				volatile Uint16 NotFirstSync;
				volatile Uint16 Slippgcounter;
				volatile Uint16 StopValid;
				volatile Uint16 lasttime;
				volatile Uint16 SigmapgStep;
				volatile int16  SigmapgSign;
				volatile int16 	SlippgremainSum;
				volatile int16 	Slippgremain;
				volatile int32  Sigmapg;
				volatile int32 	Lastpgabs;
				volatile int32  Slippg;
				volatile int32 	Slippgconstant; 
}CO_IP_MODE_DATA;
//typedef CO_PP_MODE_PARS *CO_PP_MODE_PARS_handle;

volatile CO_IP_MODE_DATA CO_IPmode_data  = {0,0,0,0,0,0,0,0,0,0,0,0,0};


    
volatile int32 temp_acc_v1 =0;	     //unit p/s	  by niekefu 2012.6.4
volatile int32 temp_acc_v2 =0;
volatile int32 temp_stop_v1 =0;	     //used when quikstop
volatile int32 temp_stop_v2 =0;
     
volatile Uint32 temp_acc_a1 =0;
volatile Uint16 pp_s_speed_status=0; 
volatile int32 half_way_distance;
volatile Uint32 acc_cycle_count = 0;
volatile int32 forecast_acc_distance = 0;
volatile int32 s_acc_distance=0;         //整个加速段所走过的脉冲数
volatile int32 s_AccSub_distance=0;      //
volatile int32 s_AccAdd_distance=0;      //加加速度段所走过的脉冲数
volatile Uint32 s_a1 = 0 ;
volatile Uint32 s_a2 = 0 ;
volatile Uint32 jerk;

volatile ACC_OR_SPEED_UNIT_CONVER acc_aster1 = {0,0,0,0,0};		// Unit: 0.1rpm/s
volatile ACC_OR_SPEED_UNIT_CONVER dec_aster1 = {0,0,0,0,0};		// Unit: 0.1rpm/s
volatile ACC_OR_SPEED_UNIT_CONVER acc_aster2 = {0,0,0,0,0};		// Unit: 0.1rpm/100us
volatile ACC_OR_SPEED_UNIT_CONVER dec_aster2 = {0,0,0,0,0};		// Unit: 0.1rpm/100us
volatile ACC_OR_SPEED_UNIT_CONVER Target_Velocity_aster = {0,0,0,0,0};
volatile ACC_OR_SPEED_UNIT_CONVER Velocity_Actual_Value_aster = {0,0,0,0,0};
volatile ACC_OR_SPEED_UNIT_CONVER Velocity_Demand_Value_aster = {0,0,0,0,0};
volatile ACC_OR_SPEED_UNIT_CONVER step_pos = {0,0,0,0,0};		// Unit: p/100us

volatile Uint32 PosSPEEDINS = 0;		//Unit: p/s 
volatile Uint32 PosSPEEDINSout = 0;		//Unit: p/s 
volatile int32  SigmaPgerr = 0, SigmaPgerr_monitor_Target_v = 0;  //2011-8-10
volatile Uint32 Velocity_Window_Timer = 0;
volatile Uint32 Velocity_Threshold_Timer = 0;
volatile Uint32 Following_Error_Timer = 0;
volatile Uint32 Following_Error_Window_aster = 0;
volatile Uint32 Position_Window_Timer = 0;
volatile Uint32 Position_Window_aster = 0;

volatile int16 pos_sign = 1;
volatile int16 immediately_valid = 0;
volatile Uint16 BufferIsFull = 0;
volatile int16 abs_relative[2] = {0,0};

extern	volatile UNSIGNED16 CO_SYNCperiod;
extern  volatile UNSIGNED16 CO_SYNCperiod_producer;
extern	volatile UNSIGNED16 CO_SYNCperiodTimeout;
extern	volatile UNSIGNED16 CO_SYNCperiodEarly;
extern  volatile  char Interpolation_Time_Index;
extern  volatile UNSIGNED8 Interpolation_Time_Unit;

void 	co_pos_speed_plan(volatile CO_PP_VELOCITY_PLAN_PARS * pars);
int32 	deal_maxmin_pos(int32 data, int sele);  //2011-08-11 by Liuyan
void  	deal_maxmin_speed(void);  //2011-08-11
void	PP_IP_Status(int16 StopValid);
void	Clear_IPdata(void);
extern  void WriteFlash(unsigned int pt,unsigned int val);
//-------------------------------------------------------------------------------------------

Uint32 velocity_limit_function(Uint32 v)//SPEEDlmt
{Uint32 MaxVelocity;
 long long TMP;
	TMP = (long long)SPEEDlmt * Velocity_Divisor / Velocity_Numerator;// Unit: 0.1r/min -> xxx
	if(TMP > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
	}
	else
	{
		MaxVelocity = (Uint32)TMP;	// Unit: xxx -> 0.1r/min 	-> p/s
		if(v > MaxVelocity)
		{
			v = MaxVelocity;
		}
	}
	return v;	
}
void acc_limit_function(void)
{
	if(Max_Acceleration)
	{
		Profile_Acceleration_aster = (Profile_Acceleration < Max_Acceleration) ? Profile_Acceleration : Max_Acceleration;
	}
	else
	{
		Profile_Acceleration_aster = Profile_Acceleration;
	}
	if(Max_Deceleration)
	{
		Profile_Decelaration_aster = (Profile_Decelaration < Max_Deceleration) ? Profile_Decelaration : Max_Deceleration;
		Quick_Stop_Deceleration_aster = (Quick_Stop_Deceleration < Max_Deceleration) ? Quick_Stop_Deceleration : Max_Deceleration;	
	}
	else
	{
		Profile_Decelaration_aster = Profile_Decelaration;
		Quick_Stop_Deceleration_aster = Quick_Stop_Deceleration;
	}
}

// 单位换算函数
// acc/dec:	xxx -> 0.1rpm/s
// speed:	xxx -> 0.1rpm
void acc_speed_unit_convers(volatile ACC_OR_SPEED_UNIT_CONVER * v)
{int32 Tmp;
 long long TMP;
	if((v->Agear == 0) || (v->Bgear == 0))
	{
		pos_flag4.bit.GearErr = 1;
		return;
	}
	TMP = (long long)v->ACCorSPEEDin * v->Agear;
	v->ACCorSPEEDremain += (TMP % v->Bgear);
	TMP = TMP/v->Bgear;	
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
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
	v->ACCorSPEEDout = Tmp;
	/*
	if(labs(Tmp) < 30000)
	{
		v->ACCorSPEEDout = Tmp;
	}
	else
	{
		if(Tmp > 0)
		{
			v->ACCorSPEEDout = 30000;
		}
		else
		{
			v->ACCorSPEEDout = -30000;
		}
	}*/	
}


/*******************************************************************************
   
   PP_MODEs形加/减速控制,acc Unit: p/s/s -> p/s/100us, velocity Unit: p/s
*******************************************************************************/

Uint32 trap_s_speed_control(Uint32 acc_a,Uint32 dec_a, int32 SPPEEDINSout,int16 stop_code,Uint32 jerk)
{	
	Uint16 acc_inc,dec_inc;			
	static int16 old_stop_code = 0;
	static Uint16 stop_type = 0;	  			
	
		if(!CO_PPmode_pars.profile_velocity&&!stop_code)
		{
			return 0;
		}
		if(!old_stop_code && stop_code)
		{
			temp_stop_v1 = SPPEEDINSout;	
			if(dec_a)
			{					
				acc_aster2.ACCorSPEEDin = dec_a;					// Unit: p/s -> p/s/100us
				
			}
			else
			{						
				acc_aster2.ACCorSPEEDin = Max_Deceleration;                		// Unit: p/s -> p/s/100us
			}
			acc_aster2.Agear = 1;
			acc_aster2.Bgear = 10000;
			acc_speed_unit_convers( & acc_aster2);					// Unit: xxx/s -> xxx/100us
			dec_inc = acc_aster2.ACCorSPEEDout;
			if(pp_s_speed_status==AccAdd_Increase_Speed
			  ||pp_s_speed_status==AccSteady_Increase_Speed
			  ||pp_s_speed_status==AccSub_Increase_Speed)
			{
				if(pp_s_speed_status==AccAdd_Increase_Speed)
				{
					temp_acc_v1 = SPPEEDINSout;
				}
				stop_type = 1;
				s_a2=0;
				pp_s_speed_status = Stop_AccSub_Increase_Speed;			
			}
			else
			{
				stop_type=0;
				if(pp_s_speed_status!=DecSub_Decrease_Speed)
			 	{	//
					if(pp_s_speed_status == Steady_Speed && SPPEEDINSout>0)
					{
						stop_type = 1;	
					}
					temp_stop_v1 = SPPEEDINSout;
					pp_s_speed_status = Stop_DecAdd_Decrease_Speed ;	
			 	}
			}
		   
		}
		else
		{
			if(acc_a)
			{					
				acc_aster2.ACCorSPEEDin = acc_a;					// Unit: p/s -> p/s/100us
				
			}
			else
			{						
				acc_aster2.ACCorSPEEDin = Max_Acceleration;                		// Unit: p/s -> p/s/100us
			}	
			
			acc_aster2.Agear = 1;
			acc_aster2.Bgear = 10000;
			acc_speed_unit_convers( & acc_aster2);					// Unit: xxx/s -> xxx/100us
			acc_inc = acc_aster2.ACCorSPEEDout;
			dec_inc = acc_inc;
			   			   			   		   	
		}
		old_stop_code = stop_code;
		
		if(pp_s_speed_status == AccAdd_Increase_Speed
		  || pp_s_speed_status == AccSteady_Increase_Speed)
		{
		  	forecast_acc_distance = (acc_cycle_count + 1) * labs(inPgstep) + s_AccSub_distance;
		  		 
		}
				 
		switch(pp_s_speed_status)
		{	case AccAdd_Increase_Speed:
			  	
			  	if(SigmaPgerr+forecast_acc_distance>half_way_distance
				||SPPEEDINSout * 2>CO_PP_velocity_plan_pars.target_v)//cannot reach the max acceleration
				
				{
					temp_acc_v1 = SPPEEDINSout;	
					temp_acc_a1 = s_a1;
					temp_acc_v2 = temp_acc_v1;
					
					pp_s_speed_status = AccSub_Increase_Speed;					
				}		   												
				else
				{											
					if((s_a1 + jerk) < acc_inc)
					{	
						s_a1 += jerk;
						SPPEEDINSout += s_a1;
						acc_cycle_count++;

						s_AccAdd_distance = SigmaPgerr;
						s_AccSub_distance = labs(acc_cycle_count * labs(inPgstep) - s_AccAdd_distance);
										
					}
					else
					{						
						s_a1 = acc_inc;
						temp_acc_v1 = SPPEEDINSout;     //save the point for the accSub_increase_speed 
						temp_acc_a1 = s_a1;
						pp_s_speed_status = AccSteady_Increase_Speed;
					}
					
				}
					
				break;
					
			case AccSteady_Increase_Speed:
																 					
				if((SigmaPgerr + forecast_acc_distance) > half_way_distance
				  || SPPEEDINSout + temp_acc_v1 > CO_PP_velocity_plan_pars.target_v)
							
				{
					temp_acc_v2 = SPPEEDINSout;
					pp_s_speed_status = AccSub_Increase_Speed;
				} 
				
				else
				{
					SPPEEDINSout += s_a1;
				}
				
			  	break;

			case AccSub_Increase_Speed: 
			
				if(s_a1 > jerk)
				{			
					s_a1 -= jerk;
					SPPEEDINSout += s_a1;
				}
				else
				{
				 	s_a1 = 0;
					s_acc_distance = SigmaPgerr;
					acc_aster1.ACCorSPEEDremain = 0;
					acc_aster2.ACCorSPEEDremain = 0;
					dec_aster1.ACCorSPEEDremain = 0;
					dec_aster2.ACCorSPEEDremain = 0;
					pp_s_speed_status = Steady_Speed;
				}
				break;
			case DecAdd_Decrease_Speed:
				if(SPPEEDINSout <= s_a2)       //when the distance is too bit,it will work
				{
					pp_s_speed_status = DecSub_Decrease_Speed;
					break;
				}
					
				if(SPPEEDINSout-s_a2 < temp_acc_v2)
				{
				    SPPEEDINSout = temp_acc_v2;
				    if(temp_acc_v2 == temp_acc_v1)                
					{                                               
						SPPEEDINSout = temp_acc_v1;
						s_a2 = temp_acc_a1;	
						pp_s_speed_status = DecSub_Decrease_Speed;    
					}                                               
 					else                                            
 					{                                               
						s_a2 = dec_inc;	
						pp_s_speed_status = DecSteady_Decrease_Speed;
					}	 
				}		                                        
				
				else
				{	
					if(s_a2 + jerk < dec_inc)
					{	
						s_a2 += jerk;
						SPPEEDINSout -= s_a2;
					}
					else
					{
						s_a2 = dec_inc;
						pp_s_speed_status = DecSteady_Decrease_Speed;
					}
				}
				break;
			case DecSteady_Decrease_Speed:
							
				if(labs(inPgcnt)-labs(inPgstep) <= s_AccAdd_distance)
				{
					SPPEEDINSout = temp_acc_v1;
					s_a2 = temp_acc_a1;
					pp_s_speed_status = DecSub_Decrease_Speed;
				}
				else
			    {
					if(SPPEEDINSout>s_a2)       //when the max acc is too little,it will work
					{
						SPPEEDINSout -= s_a2;
					}
					else
					{
						pp_s_speed_status = DecSub_Decrease_Speed;
					}
			    }		
				break;
			case DecSub_Decrease_Speed: 
							
				if(s_a2 >jerk)
				{			
					s_a2-=jerk;
					if(SPPEEDINSout > s_a2)
					{
						SPPEEDINSout -= s_a2;
					}
					else
					{
						                         //s曲线最后一段完成，清零全局变量
						SPPEEDINSout = 0;

						acc_cycle_count = 0;
						s_AccSub_distance = 0;
						forecast_acc_distance = 0;
						temp_acc_v1 = 0;
						temp_acc_v2 = 0;
						s_a1 = 0;
						s_a2 = 0;

						acc_aster1.ACCorSPEEDremain = 0;
						acc_aster2.ACCorSPEEDremain = 0;
						dec_aster1.ACCorSPEEDremain = 0;
						dec_aster2.ACCorSPEEDremain = 0;					
						pp_s_speed_status = Steady_Speed;
					}
				}
				else
				{
				 	
					SPPEEDINSout = 0;

					acc_cycle_count = 0;
					s_AccSub_distance = 0;
					forecast_acc_distance = 0;
					temp_acc_v1 = 0;
					temp_acc_v2 = 0;
					s_a1 = 0;
					s_a2 = 0;

					acc_aster1.ACCorSPEEDremain = 0;
					acc_aster2.ACCorSPEEDremain = 0;
					dec_aster1.ACCorSPEEDremain = 0;
					dec_aster2.ACCorSPEEDremain = 0;					
					pp_s_speed_status = Steady_Speed;
				}
				break;
			case Stop_AccSub_Increase_Speed://reduce the current s_a1 which is more than 0 to zero
				if(s_a1 > jerk)
				{			
					s_a1 -= jerk;
					SPPEEDINSout += s_a1;
				}
				else
				{
				 	s_a1 = 0;
					s_a2 = 0;
					temp_stop_v1 = SPPEEDINSout; 
					pp_s_speed_status = Stop_DecAdd_Decrease_Speed;
				}
							
			    break; 
			case Stop_DecAdd_Decrease_Speed	:  
			    if(SPPEEDINSout <= s_a2)        //when the distance is to bit it will work   
				{
					pp_s_speed_status = DecSub_Decrease_Speed;
					break;
				}
				if(stop_type==0)
				{
					if(temp_stop_v1<=temp_acc_v1)//to protect the following sentence ,can be put  before
					{
						pp_s_speed_status = Stop_DecSub_Decrease_Speed;
						break;
					}	
					if(SPPEEDINSout <= (temp_acc_v1+((temp_stop_v1-temp_acc_v1)>>1))) //can not reach the max acceleration
					{
						pp_s_speed_status = Stop_DecSub_Decrease_Speed;
						break;	
					}
				}
				else
				{
					if(SPPEEDINSout <= (temp_stop_v1>>1)) //can not reach the max acceleration
					{
						pp_s_speed_status = DecSub_Decrease_Speed;
						break;	
					}
				}	                                        				
				if(s_a2 + jerk < dec_inc)
				{	
					s_a2 += jerk;
					SPPEEDINSout -= s_a2;
				}
				else  //reach the quik stop acceleration
				{
					temp_stop_v2 = SPPEEDINSout;
					s_a2 = dec_inc;
					pp_s_speed_status = Stop_DecSteady_Decrease_Speed;
				}
								
			    break; 
			case Stop_DecSteady_Decrease_Speed :
				if(stop_type==0)
				{
					if(SPPEEDINSout <= labs(temp_acc_v1 + temp_stop_v1 - temp_stop_v2)) 
					{					
						pp_s_speed_status = Stop_DecSub_Decrease_Speed;
					}
	
					else
				    {
						if(SPPEEDINSout>s_a2)       //when the max acc is too little,it will work
						{
							SPPEEDINSout -= s_a2;
						}
						else
						{
							pp_s_speed_status = DecSub_Decrease_Speed;
						}
				    }
				}
				else
				{
					if(SPPEEDINSout <=  labs(labs(temp_stop_v1) - labs(temp_stop_v2))) 
					{					
						pp_s_speed_status = DecSub_Decrease_Speed;
					}
	
					else
				    {
						if(SPPEEDINSout>s_a2)       //when the max acc is too little,it will work
						{
							SPPEEDINSout -= s_a2;
						}
						else
						{
							pp_s_speed_status = DecSub_Decrease_Speed;
						}
				    }

				}    		
				break;
			case Stop_DecSub_Decrease_Speed:
			 	if(SPPEEDINSout>temp_acc_v1)
				{			
					if(s_a2>jerk)
					{
						s_a2-=jerk;
						if(SPPEEDINSout > s_a2)
						{
							SPPEEDINSout -= s_a2;
						}
						else
						{
							pp_s_speed_status = DecSub_Decrease_Speed;
						}
					}
					else
					{
						pp_s_speed_status = DecSub_Decrease_Speed;
					}
				}
				else
				{
				 						
					pp_s_speed_status = DecSub_Decrease_Speed;
				}
				break;
			case Steady_Speed:
				if(SPPEEDINSout > 0)   // moving	
				{
					if(labs(inPgcnt) < s_acc_distance + labs(inPgstep))
					{
						s_a2 = 0;
						pp_s_speed_status = DecAdd_Decrease_Speed;
					}
				
				}
				break;

			default :;
		}		
	  
	return SPPEEDINSout;
}


// PP_MODE梯形加/减速控制,acc Unit: p/s/s -> p/s/100us, velocity Unit: p/s
// PV_MODE梯形加/减速控制,acc Unit: xxx -> 0.1rpm/s -> 0.1rpm/100us, velocity Unit: 0.1rpm
// Mode = 0: PP
// Mode = 1: PV
Uint32 trap_speed_control(Uint32 acc_a,Uint32 dec_a,int32 SPPEEDINS, int32 SPPEEDINSout,Uint16 Mode)
{Uint16 acc_inc,dec_inc;	
	if(SPPEEDINS > SPPEEDINSout)		// acc
	{
		if(acc_a)
		{
			if(Mode)
			{
				acc_aster1.ACCorSPEEDin = acc_a;
				acc_aster1.Agear = Acceleration_Numerator;
				acc_aster1.Bgear = Acceleration_Divisor;
				acc_speed_unit_convers( & acc_aster1);				// Unit: xxx -> 0.1rpm/s
				acc_aster2.ACCorSPEEDin = acc_aster1.ACCorSPEEDout;	// Unit: 0.1rpm/s -> 0.1rpm/100us
			}
			else
			{
				acc_aster2.ACCorSPEEDin = acc_a;					// Unit: p/s -> p/s/100us
			}
			acc_aster2.Agear = 1;
			acc_aster2.Bgear = 10000;
			acc_speed_unit_convers( & acc_aster2);					// Unit: xxx/s -> xxx/100us
			acc_inc = acc_aster2.ACCorSPEEDout;
		}
		else
		{
			if(Mode)
			{
				acc_aster1.ACCorSPEEDin = Max_Acceleration;
				acc_aster1.Agear = Acceleration_Numerator;
				acc_aster1.Bgear = Acceleration_Divisor;
				acc_speed_unit_convers( & acc_aster1);				// Unit: xxx -> 0.1rpm/s
				acc_aster2.ACCorSPEEDin = acc_aster1.ACCorSPEEDout;	// Unit: 0.1rpm/s -> 0.1rpm/100us
			}
			else
			{
				acc_aster2.ACCorSPEEDin = Max_Acceleration;			// Unit: p/s -> p/s/100us
			}
			acc_aster2.Agear = 1;
			acc_aster2.Bgear = 10000;
			acc_speed_unit_convers( & acc_aster2);					// Unit: xxx/s -> xxx/100us
			acc_inc = acc_aster2.ACCorSPEEDout;
		}
		if((SPPEEDINSout + acc_inc) < SPPEEDINS)
		{
			SPPEEDINSout += acc_inc;
		}
		else
		{
			SPPEEDINSout = SPPEEDINS;
			acc_aster1.ACCorSPEEDremain = 0;
			acc_aster2.ACCorSPEEDremain = 0;
			dec_aster1.ACCorSPEEDremain = 0;
			dec_aster2.ACCorSPEEDremain = 0;
		}
	}
	else if(SPPEEDINS < SPPEEDINSout)	// dec
	{
		if(dec_a)
		{
			if(Mode)
			{
				dec_aster1.ACCorSPEEDin = dec_a;
				dec_aster1.Agear = Acceleration_Numerator;
				dec_aster1.Bgear = Acceleration_Divisor;
				acc_speed_unit_convers( & dec_aster1);				// Unit: xxx -> 0.1rpm/s
				dec_aster2.ACCorSPEEDin = dec_aster1.ACCorSPEEDout;	// Unit: 0.1rpm/s -> 0.1rpm/100us
			}
			else
			{
				dec_aster2.ACCorSPEEDin = dec_a;					// Unit: p/s -> p/s/100us
			}
			dec_aster2.Agear = 1;
			dec_aster2.Bgear = 10000;
			acc_speed_unit_convers( & dec_aster2);					// Unit: xxx/s -> xxx/100us
			dec_inc = dec_aster2.ACCorSPEEDout;
		}
		else
		{
			if(Mode)
			{
				dec_aster1.ACCorSPEEDin = Max_Deceleration;
				dec_aster1.Agear = Acceleration_Numerator;
				dec_aster1.Bgear = Acceleration_Divisor;
				acc_speed_unit_convers( & dec_aster1);				// Unit: xxx -> 0.1rpm/s
				dec_aster2.ACCorSPEEDin = dec_aster1.ACCorSPEEDout;	// Unit: 0.1rpm/s -> 0.1rpm/100us
			}
			else
			{
				dec_aster2.ACCorSPEEDin = Max_Deceleration;			// Unit: p/s -> p/s/100us
			}
			dec_aster2.Agear = 1;
			dec_aster2.Bgear = 10000;
			acc_speed_unit_convers( & dec_aster2);					// Unit: xxx/s -> xxx/100us
			dec_inc = dec_aster2.ACCorSPEEDout;
		}
		if(SPPEEDINSout > (SPPEEDINS + dec_inc))
		{
			SPPEEDINSout -= dec_inc;
		}
		else
		{
			SPPEEDINSout = SPPEEDINS;
			acc_aster1.ACCorSPEEDremain = 0;
			acc_aster2.ACCorSPEEDremain = 0;
			dec_aster1.ACCorSPEEDremain = 0;
			dec_aster2.ACCorSPEEDremain = 0;
		}
	}
	else
	{
		SPPEEDINSout = SPPEEDINS;		
		acc_aster1.ACCorSPEEDremain = 0;
		acc_aster2.ACCorSPEEDremain = 0;
		dec_aster1.ACCorSPEEDremain = 0;
		dec_aster2.ACCorSPEEDremain = 0;
	}
	return SPPEEDINSout;
}

/********************************************************************************
*Function name:    	CO_PP_mode                		     						*
*Parameters:    	stw,target_velocity,profile_acc/dec,quick_stop_dec			*
*data:    													           			*
*Returns:    		zsw														    *
*        											                			*
*Description:    	 															*
********************************************************************************/
void CO_PP_mode(void)
{
	long long TMP;
	long temp=0,temp1=0;
	Uint32 v_acc,v_dec;//,tmp;
	int32 end_postion;
	//int32 min_position,max_position;
	static int16 StopValid = 0;	// halt / stop / leave operation state
	static int32 position_remain = 0;
	int flag = 0;
	if(!CO_oldControlword.bit.OperSpecific_b4 && CO_Controlword.bit.OperSpecific_b4 && state_flag2.bit.Son)
	{// rising : new set
//		immediately_valid = 0;

		// judge data_valid.
		// ...
		acc_limit_function();
		CO_PPmode_pars.profile_acc = Profile_Acceleration_aster;
		CO_PPmode_pars.profile_dec = Profile_Decelaration_aster;
		CO_PPmode_pars.quick_dec = Quick_Stop_Deceleration_aster;
		CO_PPmode_pars.profile_velocity = velocity_limit_function(Profile_Velocity);
		CO_PPmode_pars.end_velocity = velocity_limit_function(END_Velocity);
		
		if(Motion_Profile_Type==2)                                          //added by niekefu 2012.6.5
		{
			CO_Controlword.bit.OperSpecific_b5=0;                           //屏蔽到立即有效位
			CO_PPmode_pars.status=0;			
			inPgcnt=0;
			BufferIsFull=0 ;                                                //单点定位模式，不用缓冲
		}

		
		if(BufferIsFull==0 || CO_Controlword.bit.OperSpecific_b5)
		{	
			if((immediately_valid && !pos_sign) && CO_PPmode_pars.status && !CO_Controlword.bit.OperSpecific_b5)
			{
				CO_PPmode_pars.target_position[1] = Target_Position;
				if(Pos_Polarity)
				{
					CO_PPmode_pars.target_position[1] = - CO_PPmode_pars.target_position[1];
				}
				abs_relative[1] = CO_Controlword.bit.OperSpecific_b6;
				BufferIsFull = 2;	
			}
			else
			{
				if(CO_Controlword.bit.OperSpecific_b5)
				{
//					BufferIsFull = 0;
					immediately_valid = 1;
				}

				CO_PPmode_pars.target_position[0] = Target_Position;				
				if(Pos_Polarity)
				{
					CO_PPmode_pars.target_position[0] = - CO_PPmode_pars.target_position[0];
				}
				BufferIsFull = 1;
				abs_relative[0] = CO_Controlword.bit.OperSpecific_b6;
			}
			flag = 1;
		}

		if(!CO_Statusword.bit.TargetReached && inPgcnt && flag)
		{// sequence or immdeiately
			CO_PPmode_pars.status = 1;
			CO_Statusword.bit.OperSpecific_b12 = 1;

			if(CO_Controlword.bit.OperSpecific_b6)
			{// relative
				if((CO_PPmode_pars.target_position[BufferIsFull-1]>0 && inPgcnt<0) || 
					(CO_PPmode_pars.target_position[BufferIsFull-1]<0 && inPgcnt>0))
				{
					if(CO_Controlword.bit.OperSpecific_b5)
					{
						SigmaPgerr_monitor_Target_v = CO_PP_velocity_plan_pars.dec_position;
						CO_PP_velocity_plan_pars.dec_length = (long long)PosSPEEDINSout*PosSPEEDINSout/2/ CO_PP_velocity_plan_pars.dec;
						BufferIsFull = 0;//2012-03-12

						if(inPgcnt<0)
						{
							
							if(labs(inPgcnt)<CO_PP_velocity_plan_pars.dec_length)
							{
								CO_PP_velocity_plan_pars.dec_length = -inPgcnt;
							}
							else
							{
								 inPgcnt = -CO_PP_velocity_plan_pars.dec_length;
							}
							CO_PPmode_pars.target_position[0] = CO_PPmode_pars.target_position[0] - (long long)inPgcnt*Position_Divisor/Position_Numerator;
						}
						else
						{
							if(inPgcnt<CO_PP_velocity_plan_pars.dec_length)
							{
								CO_PP_velocity_plan_pars.dec_length = inPgcnt;	
							}
							else
							{
								 inPgcnt = CO_PP_velocity_plan_pars.dec_length;
							}
							CO_PPmode_pars.target_position[0] = CO_PPmode_pars.target_position[0] + (long long)inPgcnt*Position_Divisor/Position_Numerator;
						}
						pos_sign = 0;
					}
				}
				else if(CO_Controlword.bit.OperSpecific_b5)
				{
					pos_sign = 1;
					inPgcnt = 0;
				}
			}
			else
			{// absolute
				
				if((CO_PPmode_pars.target_position[BufferIsFull-1]>CO_PP_velocity_plan_pars.target_position && inPgcnt<0) 
				|| (CO_PPmode_pars.target_position[BufferIsFull-1]<CO_PP_velocity_plan_pars.target_position && inPgcnt>0))
				{
					if(CO_Controlword.bit.OperSpecific_b5)
					{
						BufferIsFull = 0;//2012-03-12
						temp = (long long)CO_PPmode_pars.target_position[0]*Position_Numerator/Position_Divisor
							 - CO_PP_velocity_plan_pars.idea_actual_position;
						CO_PP_velocity_plan_pars.dec_length = (long long)PosSPEEDINSout*PosSPEEDINSout/2/ CO_PP_velocity_plan_pars.dec;
						if(inPgcnt<0)
						{
							if(temp<0 && labs(temp)>CO_PP_velocity_plan_pars.dec_length)
							{
								pos_sign = 1;
							}
							else
							{
								temp1 = CO_PP_velocity_plan_pars.idea_actual_position
									- (long long)CO_PP_velocity_plan_pars.target_position*Position_Numerator/Position_Divisor;
								if(temp1>0 && temp1>CO_PP_velocity_plan_pars.dec_length)
								{
									inPgcnt = -CO_PP_velocity_plan_pars.dec_length;
									SigmaPgerr_monitor_Target_v = CO_PP_velocity_plan_pars.dec_position;
								}
								else
								{
									CO_PP_velocity_plan_pars.dec_length = -inPgcnt;
								}
								pos_sign = 0;		
							}
						}
						else
						{
							if(temp>CO_PP_velocity_plan_pars.dec_length)
							{
								pos_sign = 1;
							}
							else
							{
								temp1 = (long long)CO_PP_velocity_plan_pars.target_position*Position_Numerator/Position_Divisor 
									- CO_PP_velocity_plan_pars.idea_actual_position;
								if(temp1>0 && temp1>CO_PP_velocity_plan_pars.dec_length)
								{
									inPgcnt = CO_PP_velocity_plan_pars.dec_length;
									SigmaPgerr_monitor_Target_v = CO_PP_velocity_plan_pars.dec_position;
								}
								else
								{
									CO_PP_velocity_plan_pars.dec_length = inPgcnt;
								}
								pos_sign = 0;
							}
						}
					}
				}
				else if(CO_Controlword.bit.OperSpecific_b5)
				{
					pos_sign = 1;
				}
			}
		}	
		else if(flag)
		{
			CO_PPmode_pars.status = 1;
			CO_Statusword.bit.OperSpecific_b12 = 1;
		}
	}
	else if((CO_PPmode_pars.status == 0) || (immediately_valid && pos_sign==0 && BufferIsFull == 0))
	{
		if(!CO_Controlword.bit.OperSpecific_b4)
		{
			CO_Statusword.bit.OperSpecific_b12 = 0;
		}
	}

	//if(CO_PPmode_pars.status && (((inPgcnt == 0) && (CO_PP_velocity_plan_pars.end_v || CO_Statusword.bit.TargetReached))) || immediately_valid)
	if(CO_PPmode_pars.status && ((inPgcnt==0) || (immediately_valid && pos_sign)))
	{
		CO_PPmode_pars.status = 0;
		deal_maxmin_speed(); // 2011-08-10

		if(immediately_valid)
		{// immdeiately
			SigmaPgerr = 0;
			immediately_valid = 0;
		}
		else
		{	//081219
			if(PosSPEEDINSout && !CO_Statusword.bit.TargetReached)// change_on_setpoint = 1
			{// sequence
				SigmaPgerr = SigmaPgerr - labs(CO_PP_velocity_plan_pars.increase_pulse);
			}
			else //if(CO_Statusword.bit.TargetReached)  //change_on_setpoint=0
			{// single
				SigmaPgerr = 0; //081219
//				CO_PP_velocity_plan_pars.idea_actual_position = co_abs_position;
			} 
		}

		CO_PP_velocity_plan_pars.idea_start_position = CO_PP_velocity_plan_pars.idea_actual_position;
		CO_PP_velocity_plan_pars.target_position = CO_PPmode_pars.target_position[0];
		// position limit
		if(abs_relative[0])
		{// relative
			TMP = (long long)(CO_PPmode_pars.target_position[0]) * Position_Numerator / Position_Divisor;	//niekefu 增加余数处理		
			position_remain += (long long)(CO_PPmode_pars.target_position[0]) * Position_Numerator % Position_Divisor;
		
			if(labs(position_remain) >= Position_Divisor)
			{	
				if(position_remain > 0)
				{
					position_remain -= Position_Divisor;
					TMP++;
				}
				else
				{
					position_remain +=  Position_Divisor;
					TMP--;
				}
			}
			
									
			if(CO_PPmode_pars.target_position[0] > 0)
			{
				TMP -= SigmaPgerr;
			}
			else
			{
				TMP += SigmaPgerr;
			}
			if(llabs(TMP) > 0x7FFFFFFF)
			{
				pos_flag4.bit.GearErr = 1;
				return;
			}
			SigmaPgerr = 0;
			SigmaPgerr_monitor_Target_v = 0;//2011-8-10
			//081219
			if(membit01.bit.bit00)      //niekefu
			{			
				end_postion = (int32)TMP - CO_PP_velocity_plan_pars.idea_actual_position;
			}
			else
			{
				end_postion = (int32)TMP + CO_PP_velocity_plan_pars.idea_actual_position;	
			}
			end_postion = deal_maxmin_pos(end_postion,0);
		}
		else
		{// absolute
			inPgcnt = 0;
			SigmaPgerr = 0;//081219
			SigmaPgerr_monitor_Target_v = 0;//2011-8-10
			end_postion = deal_maxmin_pos(CO_PP_velocity_plan_pars.target_position,1);
		}
		
		if(membit01.bit.bit00)//2011-08-30 by LiuYan
		{
			CO_PP_velocity_plan_pars.increase_pulse = end_postion - (-CO_PP_velocity_plan_pars.idea_actual_position);
		}
		else
		{
			CO_PP_velocity_plan_pars.increase_pulse = end_postion - (CO_PP_velocity_plan_pars.idea_actual_position);
		}

		inPgcnt += CO_PP_velocity_plan_pars.increase_pulse;		

		CO_PP_velocity_plan_pars.start_v = PosSPEEDINSout;// Unit: p / s
		co_pos_speed_plan(& CO_PP_velocity_plan_pars);
		StopValid = 0;

		if(BufferIsFull==2)
		{
			CO_PPmode_pars.target_position[0] = CO_PPmode_pars.target_position[1];
			abs_relative[0] = abs_relative[1];
		}

		if(BufferIsFull>0)
		{
			BufferIsFull -= 1;
		}
	}

	if(state_flag2.bit.Son)
	{
		if(CO_Controlword.bit.halt 
		 ||((CO_DrvState == QuickStopActiveState) && (Quickstop_Option_Code != 0))
		 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_DisableVol))
		 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_Shutdown) && (Shutdown_Option == 1))
		 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_DisableOper) && (Disable_Operation_Option == 1)))
		{
			if((CO_Controlword.bit.halt && (Halt_Option_Code == 2))
			 ||((CO_DrvState == QuickStopActiveState) && ((Quickstop_Option_Code == 2) || (Quickstop_Option_Code == 6))))
			{// Quick stop
				StopValid = 2;
			}
			else
			{
				StopValid = 1;
			}
		}

		if(!CO_oldControlword.bit.OperSpecific_b4 && CO_Controlword.bit.OperSpecific_b4
			&&(!SPEEDINSout||CO_Statusword.bit.TargetReached)&& Motion_Profile_Type==2)
		{   //Add by niekefu 2012.5.12 just when there is a new set point that means cannot do the next when moving
			
			
			if(Profile_Jerk1 > 20)
			{
				jerk = 20;
			}
			else
			{
				jerk = Profile_Jerk1;
			}

			CO_Statusword.bit.TargetReached=0;
			PosSPEEDINSout = 0;

			acc_cycle_count = 0;
			s_AccAdd_distance = 0;
			s_AccSub_distance = 0;
			forecast_acc_distance = 0;
			temp_acc_v1 = 0;
			temp_acc_v2 = 0;
			temp_acc_a1 = 0;
			s_a1 = 0;
			s_a2 = 0;

			pp_s_speed_status = AccAdd_Increase_Speed;				                 
			CO_PP_velocity_plan_pars.idea_start_position = CO_PP_velocity_plan_pars.idea_actual_position;
			half_way_distance = labs((end_postion-CO_PP_velocity_plan_pars.idea_actual_position)/2);
																	
		}
		else
		{
			if(Motion_Profile_Type==0)//梯形
			{
				CO_PPmode_pars.profile_velocity_new = velocity_limit_function(Profile_Velocity);

		   		if(CO_PPmode_pars.profile_velocity_new != CO_PPmode_pars.profile_velocity)
				{
					CO_PPmode_pars.profile_velocity = CO_PPmode_pars.profile_velocity_new;
					deal_maxmin_speed();
					CO_PP_velocity_plan_pars.start_v = PosSPEEDINSout;// Unit: p / s
					co_pos_speed_plan(& CO_PP_velocity_plan_pars);
					SigmaPgerr_monitor_Target_v = 0;//2011-8-10
				}	
			}

		}

		if(inPgcnt)
		{
			v_acc = CO_PP_velocity_plan_pars.acc;
			if(StopValid)
			{
				PosSPEEDINS = 0;
				if(StopValid == 2)
				{// Quick stop
					v_dec = CO_PP_velocity_plan_pars.quickdec;
				}
				else
				{
					v_dec = CO_PP_velocity_plan_pars.dec;  
				}
			}
			else
			{
				v_dec = CO_PP_velocity_plan_pars.dec;
				if(SigmaPgerr_monitor_Target_v >= CO_PP_velocity_plan_pars.dec_position)
				{
					PosSPEEDINS = CO_PP_velocity_plan_pars.end_v;										
				}
				else
				{
					PosSPEEDINS = CO_PP_velocity_plan_pars.target_v;
				}
			}
			
			if((Motion_Profile_Type==2)&&jerk)
			{
				PosSPEEDINSout = trap_s_speed_control(v_acc,v_dec,PosSPEEDINSout,StopValid,jerk);
			}
			else
			{
				PosSPEEDINSout = trap_speed_control(v_acc,v_dec,PosSPEEDINS,PosSPEEDINSout,0);
			}
			// Unit: p/s -> p/100us				
			if(!PosSPEEDINSout && !StopValid && CO_PP_velocity_plan_pars.target_v)
			{
				if((memCurLoop00.hex.hex00 == Encoder_Abs17bit) || (memCurLoop00.hex.hex00 == Encoder_Inc17bit))
				{
					step_pos.ACCorSPEEDin = 1092;		// 0.5rmps
				}
				else
				{
					step_pos.ACCorSPEEDin = 83;			// 0.5rmps
				}
			}
			else
			{
				step_pos.ACCorSPEEDin = PosSPEEDINSout;	
			}
			step_pos.Agear = 1;
			step_pos.Bgear = 10000;
			acc_speed_unit_convers( & step_pos);		// Unit: p/s -> p/100us	
			inPgstep = step_pos.ACCorSPEEDout;
			if(CO_PP_velocity_plan_pars.increase_pulse < 0)
			{
				inPgstep = -inPgstep;
			}

			if(labs(inPgcnt) < labs(inPgstep))
			{// noenough
				if(CO_PP_velocity_plan_pars.end_v && CO_PPmode_pars.status && !StopValid)//081219
				{// end_velocity != 0
					Pgerr = inPgstep;
				}
				else
				{
					Pgerr = inPgcnt;
				}
				inPgcnt = 0;
			}
			else
			{
				inPgcnt -= inPgstep;
				Pgerr = inPgstep;
			}
			SigmaPgerr += abs(Pgerr);
			SigmaPgerr_monitor_Target_v += abs(Pgerr);
			if(!PosSPEEDINSout && !Pgerr && StopValid)
			{
				inPgcnt = 0;
			}
		}
		else
		{
			Pgerr = 0;
			PosSPEEDINSout = 0;
		}
		if(Pgerr > 0)
		{
			CMDref = 1;
		}
		else if(Pgerr < 0)
		{
			CMDref = -1;
		}
	}
	else
	{
		inPgcnt = 0;
		immediately_valid = 0;
		BufferIsFull = 0;
		Pgerr  = 0;
		Pgerro = 0;
		Pos_forward_filter.remain = 0;
		Pos_forward_filter.OutData = 0;
		Pos_forward_filter.OutValue = 0;
		acc_aster1.ACCorSPEEDout = 0;
		acc_aster1.ACCorSPEEDremain = 0;
		acc_aster2.ACCorSPEEDout = 0;
		acc_aster2.ACCorSPEEDremain = 0;
		dec_aster1.ACCorSPEEDout = 0;
		dec_aster1.ACCorSPEEDremain = 0;
		dec_aster2.ACCorSPEEDout = 0;
		dec_aster2.ACCorSPEEDremain = 0;
		step_pos.ACCorSPEEDout = 0;
		step_pos.ACCorSPEEDremain = 0;
		PosSPEEDINS = 0;
		PosSPEEDINSout = 0;
		SigmaPgerr = 0;
		SigmaPgerr_monitor_Target_v = 0;//2011-8-10
		SPEED = 0;
		CO_PPmode_pars.status = 0;
		if(CO_Controlword.bit.OperSpecific_b4)
		{
			if(CO_Controlword.bit.OperSpecific_b6)
			{// relative
				if(Target_Position < 0)
				{
					CMDref = -1;
				}
				else
				{
					CMDref = 1;
				}
				if(Pos_Polarity)
				{
					CMDref = - CMDref;
				}
			}
			else
			{// absolute
				if(Target_Position < co_abs_position)
				{
					CMDref = -1;
				}
				else
				{						
					CMDref = 1;
				}
				if(Pos_Polarity)
				{
					CMDref = - CMDref;
				}
			}
		}
	}
	PP_IP_Status(StopValid);
}
//
void	PP_IP_Status(int16 StopValid)
{
	long long TMP;
	int16 CO_oldControlwordtmp;
	
	//
	Velocity_Sensor_Actual_Value = speed;
	Velocity_Actual_Value_aster.Agear = Velocity_Divisor;//Velocity_Numerator;
	Velocity_Actual_Value_aster.Bgear = Velocity_Numerator;//Velocity_Divisor;
	Velocity_Actual_Value_aster.ACCorSPEEDin = Velocity_Sensor_Actual_Value;
	acc_speed_unit_convers(& Velocity_Actual_Value_aster);		// speed unit 0.1rpm -> xxx
	Velocity_Actual_Value = Velocity_Actual_Value_aster.ACCorSPEEDout;

	if(Pos_Polarity)
	{
		Velocity_Sensor_Actual_Value = - Velocity_Sensor_Actual_Value;
		Velocity_Actual_Value = - Velocity_Actual_Value;
	}
	
	if(CO_Statusword.bit.TargetReached)
	{
		CO_PP_velocity_plan_pars.idea_actual_position = co_abs_position;
	}
	if(membit01.bit.bit00)
	{
		CO_PP_velocity_plan_pars.idea_actual_position -= Pgerr;
	}
	else
	{
		CO_PP_velocity_plan_pars.idea_actual_position += Pgerr;
	}
	

	Position_Demand_Value_aster = Pgerr;
	Position_Demand_Value = (long long)CO_PP_velocity_plan_pars.idea_actual_position * Position_Divisor / Position_Numerator;
	Position_Acture_Value_aster = co_abs_position;
	Position_Acture_Value = (long long)co_abs_position * Position_Divisor / Position_Numerator;
	if(Pos_Polarity)
	{
		Position_Demand_Value_aster = - Position_Demand_Value_aster;
		Position_Demand_Value = - Position_Demand_Value;
//		Position_Acture_Value_aster = - Position_Acture_Value_aster;//2011-08-30
//		Position_Acture_Value = - Position_Acture_Value;//2011-08-30
	}

	if(membit01.bit.bit00)//11-08-29
	{
		Position_Demand_Value_aster = - Position_Demand_Value_aster;
		Position_Demand_Value = - Position_Demand_Value;
		Position_Acture_Value = - Position_Acture_Value;  // by niekefu 20121128
	}
	// Filtime_pos = 0;
	runposition();
	if(Operation_Mode_Display == PP_MODE)
	{//PP
		if(!state_flag2.bit.Son && state_flag2.bit.PotNotvalid && !state_flag2.bit.DispAlm)
		{
			if(CO_Controlword.bit.OperSpecific_b4 && !CO_oldControlword.bit.OperSpecific_b4)
			{
				CO_oldControlwordtmp = 0;
			}
			else
			{
				CO_oldControlwordtmp = CO_Controlword.bit.OperSpecific_b4;
			}
			CO_oldControlword.all = CO_Controlword.all;
			CO_oldControlword.bit.OperSpecific_b4 = CO_oldControlwordtmp;
		}
		else
		{
			CO_oldControlword.all = CO_Controlword.all;
		}
		// Following Error
		TMP = (long long)Following_Error_Window * Position_Numerator / Position_Divisor;
		if(llabs(TMP) > 0x7FFFFFFF)
		{
			pos_flag4.bit.GearErr = 1;
			return;
		}
		else
		{
			Following_Error_Window_aster = (int32)TMP;
		}
		if(labs(Ek) > Following_Error_Window_aster)
		{
			if(Following_Error_Timer < (Uint32)Following_Error_Time_Out*10)
			{
				++Following_Error_Timer;
			}
			else
			{
				CO_Statusword.bit.OperSpecific_b13 = 1;
			}
		}
		else
		{
			Following_Error_Timer = 0;
			CO_Statusword.bit.OperSpecific_b13 = 0;
		}
	}
	else
	{
		// ip mode active
		if(state_flag2.bit.Son && !CO_IPmode_data.StopValid && CO_Controlword.bit.OperSpecific_b4)
		{
			CO_Statusword.bit.OperSpecific_b12 = 1;
		}
		CO_Statusword.bit.OperSpecific_b13 = 0;
	}

	// Target reached
	//if(CO_Controlword.bit.halt)
	if(StopValid)
	{
		if(labs(Velocity_Actual_Value) < Velocity_Window)
		{
			if(Velocity_Window_Timer < (Uint32)Velocity_Window_Time*10)
			{
				++Velocity_Window_Timer;
				//CO_Statusword.bit.TargetReached = 0;
			}
			else
			{// target_reached
				CO_Statusword.bit.TargetReached = 1;
			}
		}
		else
		{
			Velocity_Window_Timer = 0;
			CO_Statusword.bit.TargetReached = 0;
		}
	}
	else
	{
		TMP = (long long)Position_Window * Position_Numerator / Position_Divisor;
		if(llabs(TMP) > 0x7FFFFFFF)
		{
			pos_flag4.bit.GearErr = 1;
			return;
		}
		else
		{
			Position_Window_aster = (int32)TMP;
		}
		if(!Pgerr)// && !CO_PPmode_pars.status)
		{
			if(labs(Ek) <= Position_Window_aster)
			{
				if(Position_Window_Timer < (Uint32)Position_Window_Time*10)
				{
					++Position_Window_Timer;
					//CO_Statusword.bit.TargetReached = 0;
				}
				else
				{
					CO_Statusword.bit.TargetReached = 1;
				}
			}
			else
			{
				Position_Window_Timer = 0;
				CO_Statusword.bit.TargetReached = 0;
			}
		}
		else
		{
			Position_Window_Timer = 0;
			CO_Statusword.bit.TargetReached = 0;
		}
	}
	
	Control_Effort = (long long)SPEED*Velocity_Divisor/Velocity_Numerator;
}
//----------------------------------------------------------------------------
void CO_IP_mode(void)
{
	//long long TMP;
	Uint32 v_acc,v_dec;
	int32  temp;
	//long long TMP;
	Uint16 InterpolatPeriod,InterpolatTimeout,InterpolatEarly;
	//
	CO_IPmode_data.StopValid = 0;
	//
	if(Interpolation_Time_Index>128)
	{
		Interpolation_Time_Index = Interpolation_Time_Index - 256;
	}
	switch(Interpolation_Time_Index)
	{
		case -4:
			temp = 1;
			break;
		case -3:
			temp = 10;
			break;
		case -2:
			temp = 100;
			break;
		case -1:
			temp = 1000;
			break;
		default:
			temp = 10000;
			break;
	}
	temp = (Uint32)Interpolation_Time_Unit * temp;//100us
	if(temp > 32767)
	{
		InterpolatPeriod = 32767;
	}
	else
	{
		InterpolatPeriod = temp;
	}
	InterpolatTimeout = InterpolatPeriod * 2;//
	InterpolatEarly = InterpolatPeriod/2;//not use

	CO_IPmode_data.StopValid = 0;

	if(state_flag2.bit.Son)
	{
		if(CO_Controlword.bit.halt 
		 ||((CO_DrvState == QuickStopActiveState) && (Quickstop_Option_Code != 0))
		 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_DisableVol))
		 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_Shutdown) && (Shutdown_Option == 1))
		 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_DisableOper) && (Disable_Operation_Option == 1)))
		{
			if((CO_Controlword.bit.halt && (Halt_Option_Code == 2))
			 ||((CO_DrvState == QuickStopActiveState) && ((Quickstop_Option_Code == 2) || (Quickstop_Option_Code == 6))))
			{// Quick stop
				CO_IPmode_data.StopValid = 2;
			}
			else
			{
				CO_IPmode_data.StopValid = 1;
			}
		}

		if(CO_Controlword.bit.OperSpecific_b4)//ip active	 
		{
			CO_Statusword.bit.OperSpecific_b12 = 1;
			if(CO_IPmode_data.StopValid)
			{
				Syncflag = 0;
				CO_IPmode_data.Slippgcounter = 0;
				CO_IPmode_data.NotFirstSync = 0;
			}
			else
			{
				CO_IPmode_data.Slippgcounter++;
				if(CO_IPmode_data.Slippgcounter > InterpolatTimeout)
				{
					SyncError_TimeOut = 1;//error1
					CO_IPmode_data.Slippgcounter = 0;
				}
				else if(Syncflag)
				{
					if(CO_IPmode_data.Slippgcounter < InterpolatEarly)
					{
						if(CO_IPmode_data.NotFirstSync)
						{
							SyncError_TimeEarly = 1;//error2
						}
					}
					if(!CO_IPmode_data.NotFirstSync)
					{
						CO_IPmode_data.Lastpgabs = co_abs_position;
						CO_IPmode_data.NotFirstSync = 1;
					}
					if(Pos_Polarity)
					{
						temp = -Interpolation_Data_Record1;
					}
					else
					{
						temp = Interpolation_Data_Record1;
					}
					temp = deal_maxmin_pos(temp,1);
					CO_IPmode_data.Slippgcounter = 0;
					Syncflag = 0;
					CO_IPmode_data.lasttime = 0;
					//
					CO_IPmode_data.Sigmapg += (temp - CO_IPmode_data.Lastpgabs);//abs
					CO_IPmode_data.Lastpgabs = temp;
					//
					CO_IPmode_data.Slippgconstant  = CO_IPmode_data.Sigmapg/InterpolatPeriod;
					CO_IPmode_data.Slippgremain  = CO_IPmode_data.Sigmapg%InterpolatPeriod;
					CO_IPmode_data.SlippgremainSum = 0;
					//
					if(CO_IPmode_data.Sigmapg > 0)
					{
						CO_IPmode_data.SigmapgSign = 1;
					}
					else if(CO_IPmode_data.Sigmapg < 0)
					{
						CO_IPmode_data.SigmapgSign = -1;
					}
					else
					{
						CO_IPmode_data.SigmapgSign = 0;
					}
					CO_IPmode_data.SigmapgStep = 0;
				}
				if(CO_IPmode_data.SigmapgStep < 2)////为什么这样处理?
				{
					CO_IPmode_data.Slippg = CO_IPmode_data.Slippgconstant;
					CO_IPmode_data.SlippgremainSum +=CO_IPmode_data.Slippgremain;
				}
				else if(CO_IPmode_data.SigmapgStep == 2)
				{
					CO_IPmode_data.Slippg = -CO_IPmode_data.Slippgconstant;
					CO_IPmode_data.SlippgremainSum -=CO_IPmode_data.Slippgremain;
				}
				else
				{
					CO_IPmode_data.Slippg = 0;
					CO_IPmode_data.Slippgremain = 0;
					CO_IPmode_data.SlippgremainSum = 0;
				}
				if(abs(CO_IPmode_data.SlippgremainSum) >= abs(InterpolatPeriod))
				{
					if(CO_IPmode_data.SlippgremainSum > 0)
					{
						CO_IPmode_data.Slippg ++;
						CO_IPmode_data.SlippgremainSum -=InterpolatPeriod;
					}
					else
					{
						CO_IPmode_data.Slippg --;
						CO_IPmode_data.SlippgremainSum +=InterpolatPeriod;
					}

				}
				//Limit
				temp = ((Uint32)SPEEDlmt<<10)/46875;  //665pulse/100us
				if(labs(CO_IPmode_data.Slippg) > temp)
				{
					if(CO_IPmode_data.Slippg>0)
					{
						CO_IPmode_data.Slippg = temp;//error,servo can not arrive
					}
					else
					{
						CO_IPmode_data.Slippg = -temp;
					}
				}
				//
				switch(CO_IPmode_data.SigmapgStep)
				{
					case 0:
						if(CO_IPmode_data.Sigmapg * CO_IPmode_data.SigmapgSign < 0)
						{
							CO_IPmode_data.SigmapgStep = 1;
						}
						break;
					case 1:
						if(++CO_IPmode_data.lasttime > (InterpolatPeriod>>1))
						{
							CO_IPmode_data.SigmapgStep = 2;
						}
						break;
					case 2:
						if(labs(CO_IPmode_data.Sigmapg) < labs(CO_IPmode_data.Slippg))
						{
							CO_IPmode_data.SigmapgStep = 3;
							CO_IPmode_data.Slippg = -CO_IPmode_data.Sigmapg;
						}
						break;
					case 3:
						break;
					default:
						break;
				}
				inPgcnt = CO_IPmode_data.Slippg;
			}
		}
		else  //not ip active
		{
			Clear_IPdata();
			CO_Statusword.bit.OperSpecific_b12 = 0;
		}

		if(inPgcnt)
		{
			if(CO_IPmode_data.StopValid)
			{
				PosSPEEDINS = 0;
				acc_limit_function();
				CO_PPmode_pars.profile_acc = Profile_Acceleration_aster;
				CO_PPmode_pars.profile_dec = Profile_Decelaration_aster;
				CO_PPmode_pars.quick_dec = Quick_Stop_Deceleration_aster;
				deal_maxmin_speed();
				v_acc = CO_PP_velocity_plan_pars.acc;
				if(CO_IPmode_data.StopValid == 2)
				{// Quick stop
					v_dec = CO_PP_velocity_plan_pars.quickdec;
				}
				else
				{
					v_dec = CO_PP_velocity_plan_pars.dec;  
				}
				PosSPEEDINSout = trap_speed_control(v_acc,v_dec,PosSPEEDINS,PosSPEEDINSout,0);//deal simple
				step_pos.ACCorSPEEDin = PosSPEEDINSout;	
				step_pos.Agear = 1;
				step_pos.Bgear = 10000;
				acc_speed_unit_convers( & step_pos);		// Unit: p/s -> p/100us	
				inPgstep = step_pos.ACCorSPEEDout;
			}
			else
			{
				inPgstep = labs(inPgcnt);
				PosSPEEDINSout = (labs(SPEED)<<14)/75;					
			}
			if(CO_IPmode_data.Sigmapg < 0)
			{
				inPgstep = -inPgstep;
			}
			/*if(labs(inPgcnt)>labs(inPgstep) && !CO_IPmode_data.StopValid)//
			{
				;//error
			}*/
			Pgerr = inPgstep;
			if(!PosSPEEDINSout && CO_IPmode_data.StopValid)
			{
				inPgcnt = 0;
				CO_IPmode_data.Sigmapg = 0;
				CO_IPmode_data.NotFirstSync = 0;
			}
			CO_IPmode_data.Sigmapg -= Pgerr;
		}
		else
		{
			Pgerr = 0;
			PosSPEEDINSout = 0;
		}
		if(!inPgcnt)
		{
			Pgerr = 0;
		}
		if(Pgerr > 0)
		{
			CMDref = 1;
		}
		else if(Pgerr < 0)
		{
			CMDref = -1;
		}
	}
	else
	{
		//IPflag = 0;
		//rstSigmaflag=0;
		Pgerro = 0;
		Pos_forward_filter.remain = 0;
		Pos_forward_filter.OutData = 0;
		Pos_forward_filter.OutValue = 0;
		acc_aster1.ACCorSPEEDout = 0;
		acc_aster1.ACCorSPEEDremain = 0;
		acc_aster2.ACCorSPEEDout = 0;
		acc_aster2.ACCorSPEEDremain = 0;
		dec_aster1.ACCorSPEEDout = 0;
		dec_aster1.ACCorSPEEDremain = 0;
		dec_aster2.ACCorSPEEDout = 0;
		dec_aster2.ACCorSPEEDremain = 0;
		step_pos.ACCorSPEEDout = 0;
		step_pos.ACCorSPEEDremain = 0;
		PosSPEEDINS = 0;
		PosSPEEDINSout = 0;
		SigmaPgerr = 0;
		SPEED = 0;
		Clear_IPdata();
		if(Interpolation_Data_Record1 > 0)
		{
			CMDref = (Interpolation_Data_Record1* Position_Numerator/Position_Divisor) + 10 - co_abs_position;
		}
		else
		{
			CMDref = (Interpolation_Data_Record1* Position_Numerator/Position_Divisor) - 10 - co_abs_position;
		}//防止产生震荡
		
	}
	PP_IP_Status(CO_IPmode_data.StopValid);
}
void	Clear_IPdata(void)
{
	inPgcnt = 0;
	Pgerr  = 0;
	CO_IPmode_data.Slippgcounter = 0;
	Syncflag = 0;
	CO_IPmode_data.lasttime = 0;
	CO_IPmode_data.SigmapgStep = 0;
	CO_IPmode_data.SigmapgSign = 0;
	CO_PPmode_pars.status = 0;
	CO_IPmode_data.lasttime = 0;
	CO_IPmode_data.Sigmapg = 0;
	CO_IPmode_data.Lastpgabs = 0;
	CO_IPmode_data.NotFirstSync = 0;
	CO_IPmode_data.Slippg = 0;
	CO_IPmode_data.Slippgconstant = 0;
	CO_IPmode_data.SlippgremainSum = 0;
	CO_IPmode_data.Slippgcounter = 0;
	CO_IPmode_data.Slippgremain = 0;
	CO_IPmode_data.StopValid = 0;
}
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void deal_maxmin_speed(void)
{
	long long TMP,TMP1,TMP2,TMP3,TMP4;
	Uint32 tmp;

	// velocity Unit switch:  xxx -> 0.1r/min 	-> p/s
	if((memCurLoop00.hex.hex00 == Encoder_Abs17bit) || (memCurLoop00.hex.hex00 == Encoder_Inc17bit))
	{
		tmp = 16384;
	}
	else
	{
		tmp = 1250;
	}
	TMP1 = (long long)Velocity_Numerator*tmp;
	TMP2 = (long long)Velocity_Divisor*75;
	TMP3 = (long long)Acceleration_Numerator*tmp;
	TMP4 = (long long)Acceleration_Divisor*75;
	TMP = CO_PPmode_pars.profile_velocity * TMP1 / TMP2;// Unit: xxx -> 0.1r/min 	-> p/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.target_v = (Uint32)TMP;	// Unit: xxx -> 0.1r/min 	-> p/s
	}
	TMP = CO_PPmode_pars.end_velocity * TMP1 / TMP2;// Unit: xxx -> 0.1r/min 	-> p/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.end_v = (Uint32)TMP;	// Unit: xxx -> 0.1r/min 	-> p/s
	}

	// acc Unit switch: xxx -> 0.1r/min/s	-> p/s/s
	TMP = CO_PPmode_pars.profile_acc * TMP3 / TMP4;// Unit: xxx -> 0.1r/min/s	-> p/s/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.acc = (Uint32)TMP;	// Unit: xxx -> 0.1r/min/s	-> p/s/s
	}
	TMP = CO_PPmode_pars.profile_dec * TMP3 / TMP4;// Unit: xxx -> 0.1r/min/s	-> p/s/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.dec = (Uint32)TMP;	// Unit: xxx -> 0.1r/min/s	-> p/s/s
	}
	TMP = (long long)CO_PPmode_pars.quick_dec * TMP3 / TMP4;// Unit: xxx -> 0.1r/min/s	-> p/s/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.quickdec = (Uint32)TMP;	// Unit: xxx -> 0.1r/min/s	-> p/s/s
	}
}

int32 deal_maxmin_pos(int32 data,int sele)
{
	long long TMP;
	int32 end_postion;
	int32 min_position,max_position;

// 	end_postion = data;
	if(sele)
	{
		TMP = (long long)data*Position_Numerator/Position_Divisor;
		if(llabs(TMP) > 0x7FFFFFFF)
		{
			pos_flag4.bit.GearErr = 1;
			return 0x7FFFFFFF;
		}
		end_postion = (int32)TMP;
	}
	else
	{
		end_postion = data;
	}

	if(Min_Position_Range_Limit||Max_Position_Range_Limit||sele)
	{
		TMP = (long long)Min_Position_Range_Limit*Position_Numerator/Position_Divisor;
		if(llabs(TMP) > 0x7FFFFFFF)
		{
			min_position = -0x7FFFFFFF;
		}
		else
		{
			min_position = (int32)TMP;
		}
		TMP = (long long)Max_Position_Range_Limit*Position_Numerator/Position_Divisor;
		if(llabs(TMP) > 0x7FFFFFFF)
		{
			max_position = 0x7FFFFFFF;
		}
		else
		{
			max_position = (int32)TMP;
		}
		if(end_postion < min_position)
		{
			end_postion = min_position;
		}
		else if(end_postion > max_position)
		{
			end_postion = max_position;
		}

		TMP = ((long long)Min_Position_Limit - Home_Offset)*Position_Numerator/Position_Divisor;
		if(llabs(TMP) > 0x7FFFFFFF)
		{
			min_position = -0x7FFFFFFF;
		}
		else
		{
			min_position = (int32)TMP;
		}
		TMP = ((long long)Max_Position_Limit - Home_Offset)*Position_Numerator/Position_Divisor;
		if(llabs(TMP) > 0x7FFFFFFF)
		{
			max_position = 0x7FFFFFFF;
		}
		else
		{
			max_position = (int32)TMP;
		}
		if(end_postion < min_position)
		{
			end_postion = min_position;
		}
		else if(end_postion > max_position)
		{
			end_postion = max_position;
		}
	}
	
	// velocity Unit switch:  xxx -> 0.1r/min 	-> p/s
/*	if((memCurLoop00.hex.hex00 == Encoder_Abs17bit) || (memCurLoop00.hex.hex00 == Encoder_Inc17bit))
	{
		tmp = 16384;
	}
	else
	{
		tmp = 1250;
	}
	TMP1 = (long long)Velocity_Numerator*tmp;
	TMP2 = (long long)Velocity_Divisor*75;
	TMP3 = (long long)Acceleration_Numerator*tmp;
	TMP4 = (long long)Acceleration_Divisor*75;
	TMP = CO_PPmode_pars.profile_velocity * TMP1 / TMP2;// Unit: xxx -> 0.1r/min 	-> p/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.target_v = (Uint32)TMP;	// Unit: xxx -> 0.1r/min 	-> p/s
	}
	TMP = CO_PPmode_pars.end_velocity * TMP1 / TMP2;// Unit: xxx -> 0.1r/min 	-> p/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.end_v = (Uint32)TMP;	// Unit: xxx -> 0.1r/min 	-> p/s
	}

	// acc Unit switch: xxx -> 0.1r/min/s	-> p/s/s
	TMP = CO_PPmode_pars.profile_acc * TMP3 / TMP4;// Unit: xxx -> 0.1r/min/s	-> p/s/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.acc = (Uint32)TMP;	// Unit: xxx -> 0.1r/min/s	-> p/s/s
	}
	TMP = CO_PPmode_pars.profile_dec * TMP3 / TMP4;// Unit: xxx -> 0.1r/min/s	-> p/s/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.dec = (Uint32)TMP;	// Unit: xxx -> 0.1r/min/s	-> p/s/s
	}
	TMP = (long long)CO_PPmode_pars.quick_dec * TMP3 / TMP4;// Unit: xxx -> 0.1r/min/s	-> p/s/s
	if(llabs(TMP) > 0xFFFFFFFF)
	{
		pos_flag4.bit.GearErr = 1;
		//return;
	}
	else
	{
		CO_PP_velocity_plan_pars.quickdec = (Uint32)TMP;	// Unit: xxx -> 0.1r/min/s	-> p/s/s
	}
*/
	return end_postion;
}
//-------------------------------------------------------------------------------------------------
void co_pos_speed_plan(volatile CO_PP_VELOCITY_PLAN_PARS * pars)
{// cauclate dec pos
long long square_v0,square_v1,square_v2;
Uint32 v0,v1,v2;			// Unit: p/s
Uint32 a1,a2;				// Unit: p/s/s
int32 all_distance,acc_distance,dec_distance,increase_distance;
	v0 = pars->start_v;
	v1 = pars->target_v;
	v2 = pars->end_v;
	if(v0 <= v1)
	{
		a1 = pars->acc;
	}
	else
	{		
		a1 = pars->dec;
	}

	if(v1 >= v2)
	{
		a2 = pars->dec;
	}
	else
	{		
		a2 = pars->acc;
	}

	square_v0 = (long long)v0*v0;
	square_v1 = (long long)v1*v1;
	square_v2 = (long long)v2*v2;

	increase_distance = inPgcnt;
	all_distance = labs(increase_distance);
	// cacu_Acc_Dec_distance
	acc_distance = (llabs(square_v1 - square_v0)>>1)/a1;
	dec_distance = (llabs(square_v1 - square_v2)>>1)/a2;
	if((acc_distance + dec_distance) > all_distance)
	{
		pars->dec_position = ((long long)all_distance*a2*2 + llabs(square_v2 - square_v0))/((long long)(a1 + a2)*2);
	}
	else
	{
		pars->dec_position = all_distance - (llabs(square_v1 - square_v2)>>1)/a2;
	}
//	pars->dec_length = all_distance - pars->dec_position;
}

/********************************************************************************
*Function name:    	CO_PV_mode                		     						*
*Parameters:    	stw,target_velocity,profile_acc/dec,quick_stop_dec			*
*data:    													           			*
*Returns:    		zsw														    *
*        											                			*
*Description:    	 															*
********************************************************************************/
void CO_PV_mode(void)
{Uint32 v_acc,v_dec;
int32 Target_Velocitytmp;
	CO_PP_velocity_plan_pars.idea_actual_position = co_abs_position;
	Position_Demand_Value_aster = 0;
	Position_Demand_Value = 0;
	Position_Acture_Value_aster = 0;
	Position_Acture_Value = (long long)co_abs_position * Position_Divisor / Position_Numerator;
	// out user_unit_data
	Velocity_Sensor_Actual_Value = speed;
	Velocity_Actual_Value_aster.Agear = Velocity_Divisor;
	Velocity_Actual_Value_aster.Bgear = Velocity_Numerator;
	Velocity_Actual_Value_aster.ACCorSPEEDin = Velocity_Sensor_Actual_Value;
	acc_speed_unit_convers(& Velocity_Actual_Value_aster);		// speed unit 0.1rpm -> xxx
	Velocity_Actual_Value = Velocity_Actual_Value_aster.ACCorSPEEDout;	
	Velocity_Demand_Value_aster.Agear = Velocity_Divisor;
	Velocity_Demand_Value_aster.Bgear = Velocity_Numerator;
	Velocity_Demand_Value_aster.ACCorSPEEDin = SPEEDINSout;
	acc_speed_unit_convers(& Velocity_Demand_Value_aster);		// speed unit 0.1rpm -> xxx
	Velocity_Demand_Value = Velocity_Demand_Value_aster.ACCorSPEEDout;	
	acc_limit_function();
	v_acc = Profile_Acceleration_aster;
	v_dec = Profile_Decelaration_aster;
	CMDref = Target_Velocity;
	if(!state_flag2.bit.Son || CO_Controlword.bit.halt
	||((CO_DrvState == QuickStopActiveState) && (Quickstop_Option_Code != 0))
	||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_DisableVol))
	||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_Shutdown) && (Shutdown_Option == 1))
	||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_DisableOper) && (Disable_Operation_Option == 1)))
	{
		Target_Velocitytmp = 0;
		if((CO_Controlword.bit.halt && (Halt_Option_Code == 2))
		 ||((CO_DrvState == QuickStopActiveState) && ((Quickstop_Option_Code == 2) || (Quickstop_Option_Code == 6))))
		{
			v_dec = Quick_Stop_Deceleration_aster;
		}
	}
	else
	{
		Target_Velocitytmp = Target_Velocity;			
		v_dec = Profile_Decelaration_aster;	
	}
	
	if(Spd_Polarity)
	{
		Velocity_Actual_Value = -Velocity_Actual_Value;
		Velocity_Demand_Value = -Velocity_Demand_Value;
		Target_Velocitytmp = - Target_Velocitytmp;
	}	
	
	Target_Velocity_aster.ACCorSPEEDin = Target_Velocitytmp;	
	Target_Velocity_aster.Agear = Velocity_Numerator;
	Target_Velocity_aster.Bgear = Velocity_Divisor;
	acc_speed_unit_convers(& Target_Velocity_aster);		// speed unit xxx -> 0.1rpm
	SPEEDINS = Target_Velocity_aster.ACCorSPEEDout;
	LmtSpd();
	if(pos_flag4.bit.PNOTzcl)
	{
		NextAnaSC();
	}
	else 
	{
		if(membit01.bit.bit00)
		{
			SPEEDINS = - SPEEDINS;
		}
		if(state_flag2.bit.Son)
		{
			SPEEDINSout = trap_speed_control(v_acc,v_dec,SPEEDINS,SPEEDINSout,1);			
			SModule3();		//        ; 计算出对应的限幅数值
			SPEED = SPEEDINSout;
			SModule2();		//        ; 速度PI调节
			limitIqr();		//        ; 具体的限幅数值
			SModule1();		//        ; 转矩滤波子程序
		}
		else
		{
			x_speed = 0;
			SPEEDINS = 0;
			SPEEDINSout = 0;
			SPEED = 0;
			acc_aster1.ACCorSPEEDremain = 0;
			acc_aster2.ACCorSPEEDremain = 0;
			dec_aster1.ACCorSPEEDremain = 0;
			dec_aster2.ACCorSPEEDremain = 0;
			Target_Velocity_aster.ACCorSPEEDremain = 0;
			Velocity_Actual_Value_aster.ACCorSPEEDremain = 0;
			Velocity_Demand_Value_aster.ACCorSPEEDremain = 0;
		}
		JudgeVCMP();
	}
	// target_windows_comparator
	if((Velocity_Actual_Value < Target_Velocitytmp + (int32)Velocity_Window) &&
	   (Velocity_Actual_Value > Target_Velocitytmp - (int32)Velocity_Window))
	{
		if(Velocity_Window_Timer < Velocity_Window_Time*10)
		{
			++Velocity_Window_Timer;
			CO_Statusword.bit.TargetReached = 0;
		}
		else
		{// target_reached
			CO_Statusword.bit.TargetReached = 1;
		}
	}
	else
	{
		Velocity_Window_Timer = 0;
		CO_Statusword.bit.TargetReached = 0;
	}
	// Velocity_Threshold_windows_comparator  Velocity_Threshold_Timer
	if(labs(Velocity_Actual_Value) <= Velocity_Threshold)
	{
		if(Velocity_Threshold_Timer < Velocity_Threshold_Time*10)
		{
			++Velocity_Threshold_Timer;
			CO_Statusword.bit.OperSpecific_b12 = 0;
		}
		else
		{// target_reached
			CO_Statusword.bit.OperSpecific_b12 = 1;
		}
	}
	else
	{
		Velocity_Threshold_Timer = 0;
		CO_Statusword.bit.OperSpecific_b12 = 0;
	}
	CO_Statusword.bit.OperSpecific_b13 = 0;
	Control_Effort = (long long)SPEED*Velocity_Divisor/Velocity_Numerator;
	CO_oldControlword.all = CO_Controlword.all;
}
/********************************************************************************
*Function name:    	CO_HM_mode                		     						*
*Parameters:    	stw,homing_speed,homing_acc,home_offset						*
*data:    													           			*
*Returns:    		zsw														    *
*        											                			*
*Description:    	 															*
********************************************************************************/
void CO_HM_mode(void)
{	Uint32 tmp;
	int32 Target_Velocitytmp;
	Uint32 v_acc,v_dec;
	static int32 InPulses = 0;
	long long TMP  = 0;
	int16 StopFlag = 0;	// halt / stop / leave operation state
	int16 home_dir = 0;	// 0: Positive
						// 1: Neqative
	CO_PP_velocity_plan_pars.idea_actual_position = co_abs_position;
	Position_Demand_Value_aster = 0;
	Position_Demand_Value = 0;
	Position_Acture_Value_aster = 0;
	Position_Acture_Value = (long long)co_abs_position * Position_Divisor / Position_Numerator;
	Velocity_Sensor_Actual_Value = speed;
	Velocity_Actual_Value_aster.Agear = Velocity_Divisor;
	Velocity_Actual_Value_aster.Bgear = Velocity_Numerator;
	Velocity_Actual_Value_aster.ACCorSPEEDin = Velocity_Sensor_Actual_Value;
	acc_speed_unit_convers(& Velocity_Actual_Value_aster);		// speed unit 0.1rpm -> xxx
	Velocity_Actual_Value = Velocity_Actual_Value_aster.ACCorSPEEDout;

	if(Max_Acceleration)
	{
		Homing_Acceleration_aster = (Homing_Acceleration < Max_Acceleration) ? Homing_Acceleration : Max_Acceleration;
		Quick_Stop_Deceleration_aster = (Quick_Stop_Deceleration < Max_Deceleration) ? Quick_Stop_Deceleration : Max_Deceleration;
	}
	else
	{
		Homing_Acceleration_aster = Homing_Acceleration;
		Quick_Stop_Deceleration_aster = Quick_Stop_Deceleration;
	}
	v_acc = Homing_Acceleration_aster;
	v_dec = Homing_Acceleration_aster;

	if(CO_Controlword.bit.OperSpecific_b4 && (CO_Controlword.bit.halt == 0))
	{
		if((CANopen402_regs.HomeStatus != HOME_END))
		{
			if(CO_Statusword.bit.OperSpecific_b15)
			{
				rehome_start_flag = 1;
				CO_Statusword.bit.OperSpecific_b15 = 0;
			}

			if(CANopen402_regs.Home_Timeout < 655360)
			{
				CANopen402_regs.Home_Timeout++;
			}
			// Home_State_Position();	//回零位状态
		 	if((Homing_Method == 1) || (Homing_Method == 17))
			{// NOT
				home_dir = 0;
				if((CANopen402_regs.HomeStatus == NOT_ARRIVAL_PN) && (input_state.bit.NOT))
				{
					CANopen402_regs.HomeStatus = ARRIVAL_PN;
				}
				else if((CANopen402_regs.HomeStatus == ARRIVAL_PN) && (input_state.bit.NOT == 0))
				{
					CANopen402_regs.HomeStatus = LEAVE_PN;
					Timers.Fndcnt = 0;
				}
				if(CANopen402_regs.HomeStatus == LEAVE_PN)
				{
					if(Homing_Method == 1)
					{// c puls
						if(int_flagx.bit.findCpuls)
						{
							CANopen402_regs.HomeStatus = ARRIVAL_REF;
							//co_abs_position = 0;
						}
					}
					else
					{// NOT
						if(input_state.bit.NOT == 0)
						{
							CANopen402_regs.HomeStatus = ARRIVAL_REF;
							co_abs_position = 0;
						}
					}
				}
			}
			else if((Homing_Method == 2) || (Homing_Method == 18))
			{// POT
				home_dir = 1;
				if((CANopen402_regs.HomeStatus == NOT_ARRIVAL_PN) && (input_state.bit.POT))
				{
					CANopen402_regs.HomeStatus = ARRIVAL_PN;
				}
				else if((CANopen402_regs.HomeStatus == ARRIVAL_PN) && (input_state.bit.POT == 0))
				{
					CANopen402_regs.HomeStatus = LEAVE_PN;
					Timers.Fndcnt = 0;
				}
				if(CANopen402_regs.HomeStatus == LEAVE_PN)
				{
					if(Homing_Method == 2)
					{// c puls
						if(int_flagx.bit.findCpuls)
						{
							CANopen402_regs.HomeStatus = ARRIVAL_REF;
							//co_abs_position = 0;
						}
					}
					else
					{// POT
						if(input_state.bit.POT == 0)
						{
							CANopen402_regs.HomeStatus = ARRIVAL_REF;
							co_abs_position = 0;
						}
					}
				}
			}
			else if((Homing_Method == 3) || (Homing_Method == 19))
			{// P home
				home_dir = 1;
				if((CANopen402_regs.HomeStatus == NOT_ARRIVAL_PN) && (input_state.bit.HmRef))
				{
					CANopen402_regs.HomeStatus = ARRIVAL_PN;
				}
				else if((CANopen402_regs.HomeStatus == ARRIVAL_PN) && (input_state.bit.HmRef == 0))
				{
					CANopen402_regs.HomeStatus = LEAVE_PN;
					Timers.Fndcnt = 0;
				}
				if(CANopen402_regs.HomeStatus == LEAVE_PN)
				{
					if(Homing_Method == 3)
					{// c puls
						if(int_flagx.bit.findCpuls)
						{
							CANopen402_regs.HomeStatus = ARRIVAL_REF;
							//co_abs_position = 0;
						}
					}
					else
					{// Home
						if(input_state.bit.HmRef == 0)
						{
							CANopen402_regs.HomeStatus = ARRIVAL_REF;
							co_abs_position = 0;
						}
					}
				}
			}
			else if((Homing_Method == 4) || (Homing_Method == 20))
			{// N home
				home_dir = 0;
				if((CANopen402_regs.HomeStatus == NOT_ARRIVAL_PN) && (input_state.bit.HmRef))
				{
					CANopen402_regs.HomeStatus = ARRIVAL_PN;
				}
				else if((CANopen402_regs.HomeStatus == ARRIVAL_PN) && (input_state.bit.HmRef == 0))
				{
					CANopen402_regs.HomeStatus = LEAVE_PN;
					Timers.Fndcnt = 0;
				}
				if(CANopen402_regs.HomeStatus == LEAVE_PN)
				{
					if(Homing_Method == 4)
					{// c puls
						if(int_flagx.bit.findCpuls)
						{
							CANopen402_regs.HomeStatus = ARRIVAL_REF;
							//co_abs_position = 0;
						}
					}
					else
					{// Home
						if(input_state.bit.HmRef == 0)
						{
							CANopen402_regs.HomeStatus = ARRIVAL_REF;
							co_abs_position = 0;
						}
					}
				}				
			}
			else if(Homing_Method == 35)
			{
			 	if(CANopen402_regs.HomeStatus != ARRIVAL_REF)
				{
//					co_abs_position = 0;//-(long long)Home_Offset * Position_Numerator / Position_Divisor;
					co_abs_position = (long long)Home_Offset * Position_Numerator / Position_Divisor;
				}
				CANopen402_regs.HomeStatus = ARRIVAL_REF;
			}
			else
			{
				Homing_Method = 0x8000;	// error Homing_Method
			}
			if(CANopen402_regs.HomeStatus == ARRIVAL_REF)
			{				
				if((InPulses == 0) && pos_flag4.bit.VCMP_COIN && Timers.Fndcnt > 0)
				{
					CANopen402_regs.HomeStatus = HOME_END;
					co_abs_position = (long long)Home_Offset * Position_Numerator / Position_Divisor;//2011-09-20 in promax
					Timers.Fndcnt = 0;
				}
				if((CANopen402_regs.HomeStatus != HOME_END) && (!StopFlag))
				{
					int_flagx.bit.rstEncMuti = 1;
				}
			}
			if(CANopen402_regs.HomeStatus == HOME_END)
			{
				Timers.STEPTME = 0;
				Timers.Fndcnt = 0;
				Ek = 0;
				RmEk = 0;
				RmFg = 0;
				CO_Statusword.bit.OperSpecific_b15 = 1;
				if (memCurLoop00.hex.hex00 == Encoder_Abs17bit && (!membit02.bit.bit02))
				{
					already_homed_flag = 1;             //在main函数中保存回零结束后的单圈信息等
					PosL = singlePos & 0xFFFF;
					PosH = singlePos >>16;
				
				}
				TMP = (long long)(Home_Offset * Position_Numerator)/Position_Divisor;
				co_abs_position = (int32)TMP;
			}
		}
	}
	if((CO_DrvState != OperationEnableState) && (CO_DrvState != QuickStopActiveState))
	{
		StopFlag = 1;
		InPulses = 0;
	}
	else if(!CO_Controlword.bit.OperSpecific_b4 || CO_Controlword.bit.halt
	 ||((CO_DrvState == QuickStopActiveState) && (Quickstop_Option_Code != 0))
	 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_DisableVol))
	 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_Shutdown) && (Shutdown_Option == 1))
	 ||((CO_DrvState == OperationEnableState) && (CO_DC_command == DC_DisableOper) && (Disable_Operation_Option == 1)))
	{
		StopFlag = 1;
		Target_Velocitytmp = 0;
		Pgerr = 0;
		inPgstep = 0;
		remadd = 0;	
		Timers.STEPTME = 0;
		Timers.Fndcnt = 0;
		InPulses = 0;

		if((CO_Controlword.bit.halt && (Halt_Option_Code == 2))
		 ||((CO_DrvState == QuickStopActiveState) && ((Quickstop_Option_Code == 2) || (Quickstop_Option_Code == 6))))
		{
			v_dec = Quick_Stop_Deceleration_aster;
		}

		CANopen402_regs.HomeStatus = NOT_ARRIVAL_PN;//可以多次回零
		int_flagx.bit.findCpuls = 0;
		CANopen402_regs.Home_Timeout = 0;
	}
		
	if((CANopen402_regs.HomeStatus == NOT_ARRIVAL_PN) 
	|| (CANopen402_regs.HomeStatus == ARRIVAL_PN)
	|| (CANopen402_regs.HomeStatus == LEAVE_PN))
	{			
		if(StopFlag)
		{			
			Target_Velocitytmp = 0;
			if(home_dir)
			{
				CMDref = 1;
			}
			else
			{
				CMDref = -1;
			}
		}
		else if(CANopen402_regs.HomeStatus == NOT_ARRIVAL_PN)
		{
			Target_Velocitytmp = Switch_Speed;
			if(home_dir)
			{
				CMDref = 1;
			}
			else
			{
				CMDref = -1;
				Target_Velocitytmp = -Target_Velocitytmp;
			}
		}
		else if(CANopen402_regs.HomeStatus == LEAVE_PN)
		{
			Target_Velocitytmp = Zero_Speed;
			if(home_dir)
			{
				CMDref = -1;
				Target_Velocitytmp = -Target_Velocitytmp;
			}
			else
			{
				CMDref = 1;
			}
		}
		else// if(CANopen402_regs.HomeStatus == ARRIVAL_PN)
		{
			if(Timers.Fndcnt < 200)
			{
				Timers.Fndcnt++;				
				x_speed = 0;
				SPEEDINS = 0;
				SPEEDINSout = 0;
				SPEED = 0;
				Target_Velocitytmp = 0;
				acc_aster1.ACCorSPEEDremain = 0;
				acc_aster2.ACCorSPEEDremain = 0;
				dec_aster1.ACCorSPEEDremain = 0;
				dec_aster2.ACCorSPEEDremain = 0;
				Target_Velocity_aster.ACCorSPEEDremain = 0;
				Velocity_Actual_Value_aster.ACCorSPEEDremain = 0;
				Velocity_Demand_Value_aster.ACCorSPEEDremain = 0;
			}
			else
			{
				Target_Velocitytmp = Zero_Speed;
				if(home_dir)
				{
					CMDref = -1;
					Target_Velocitytmp = -Target_Velocitytmp;
				}
				else
				{
					CMDref = 1;
				}
			}
		}			

		Target_Velocity_aster.ACCorSPEEDin = Target_Velocitytmp;	
		Target_Velocity_aster.Agear = Velocity_Numerator;
		Target_Velocity_aster.Bgear = Velocity_Divisor;
		acc_speed_unit_convers(& Target_Velocity_aster);		// speed unit xxx -> 0.1rpm
		SPEEDINS = Target_Velocity_aster.ACCorSPEEDout;
		LmtSpd();
		//CMDref = SPEEDINS;
		if(pos_flag4.bit.PNOTzcl)
		{
			NextAnaSC();
		}
		else 
		{
			if(membit01.bit.bit00)
			{
				SPEEDINS = - SPEEDINS;
			}
			if(state_flag2.bit.Son)
			{
				SPEEDINSout = trap_speed_control(v_acc,v_dec,SPEEDINS,SPEEDINSout,1);			
				SModule3();		//        ; 计算出对应的限幅数值
				SPEED = SPEEDINSout;
				SModule2();		//        ; 速度PI调节
				limitIqr();		//        ; 具体的限幅数值
				SModule1();		//        ; 转矩滤波子程序
			}
			else
			{
				x_speed = 0;
				SPEEDINS = 0;
				SPEEDINSout = 0;
				SPEED = 0;
				acc_aster1.ACCorSPEEDremain = 0;
				acc_aster2.ACCorSPEEDremain = 0;
				dec_aster1.ACCorSPEEDremain = 0;
				dec_aster2.ACCorSPEEDremain = 0;
				Target_Velocity_aster.ACCorSPEEDremain = 0;
				Velocity_Actual_Value_aster.ACCorSPEEDremain = 0;
				Velocity_Demand_Value_aster.ACCorSPEEDremain = 0;
			}
			JudgeVCMP();
		}
	}
	else
	{
		if(StopFlag)
		{
			Target_Velocitytmp = 0;
			Pgerr = 0;
			inPgstep = 0;
			remadd = 0;	
			Timers.STEPTME = 0;
			Timers.Fndcnt = 0;
			x_speed = 0;
			SPEEDINS = 0;
			SPEEDINSout = 0;
			SPEED = 0;

			acc_aster1.ACCorSPEEDremain = 0;
			acc_aster2.ACCorSPEEDremain = 0;
			dec_aster1.ACCorSPEEDremain = 0;
			dec_aster2.ACCorSPEEDremain = 0;
			Target_Velocity_aster.ACCorSPEEDremain = 0;
			Velocity_Actual_Value_aster.ACCorSPEEDremain = 0;
			Velocity_Demand_Value_aster.ACCorSPEEDremain = 0;

			CANopen402_regs.HomeStatus = NOT_ARRIVAL_PN;//可以多次回零
			int_flagx.bit.findCpuls = 0;
			CANopen402_regs.Home_Timeout = 0;
		}
		else if(CANopen402_regs.HomeStatus == HOME_END)
		{//HOME END
			Pgerr = 0;
			inPgstep = 0;
			remadd = 0;
			seek_step(0);
		}
		else
		{// ARRIVAL_REF
			if(Timers.Fndcnt < 1)
			{
				Timers.Fndcnt++;
				Pgerr = 0;
				inPgstep = 0;
				remadd = 0;
//				InPulses = ((long long)Home_Offset * Position_Numerator)/Position_Divisor; //2011-09-20 in promax
				InPulses = 0;//2011-09-20 in promax
			}
			if(Timers.Fndcnt >= 1)
			{
				// velocity Unit switch:  xxx -> 0.1r/min 	-> p/s
				if((memCurLoop00.hex.hex00 == Encoder_Abs17bit) || (memCurLoop00.hex.hex00 == Encoder_Inc17bit))
				{
					tmp = 16384;
				}
				else
				{
					tmp = 1250;
				}
				TMP = (long long)Zero_Speed * Velocity_Numerator * tmp / ((long long)Velocity_Divisor*75);// Unit: xxx -> 0.1r/min 	-> p/s
				if(llabs(TMP) > 0xFFFFFFFF)
				{
					pos_flag4.bit.GearErr = 1;
					return;
				}
				else
				{
					step_pos.ACCorSPEEDin = (Uint32)TMP;
				}
				step_pos.Agear = 1;	
				step_pos.Bgear = 10000;
				acc_speed_unit_convers( & step_pos);		// Unit: p/s -> p/100us	
				inPgstep = step_pos.ACCorSPEEDout;

				if(InPulses < 0)
				{
					inPgstep = - inPgstep;
					CMDref = - 1;
				}
				else
				{
					CMDref = 1;	
				}

				if(labs(InPulses) < labs(inPgstep))
				{// noenough
					Pgerr = InPulses;
					InPulses = 0;
					remadd = 0;
				}
				else
				{
					InPulses -= inPgstep;
					Pgerr = inPgstep;
				}
			}			
		}
		runposition();
	}

	// Target reached
	if(StopFlag)
	{
		if(labs(Velocity_Actual_Value) < Velocity_Window)
		{
			if(Velocity_Window_Timer < Velocity_Window_Time*10)
			{
				++Velocity_Window_Timer;
				CO_Statusword.bit.TargetReached = 0;
			}
			else
			{// target_reached
				CO_Statusword.bit.TargetReached = 1;
			}
		}
		else
		{
			Velocity_Window_Timer = 0;
			CO_Statusword.bit.TargetReached = 0;
		}
	}
	else
	{
		if(CANopen402_regs.HomeStatus == HOME_END)
		{
			CO_Statusword.bit.TargetReached = 1;
		}
		else
		{
			CO_Statusword.bit.TargetReached = 0;
		}
	}
	// Home attained	
	if(CANopen402_regs.HomeStatus == HOME_END)
	{
		CO_Statusword.bit.OperSpecific_b12 = 1;
		int_flagx.bit.findCpuls = 0;
		InPulses = 0;
	}
	else
	{
		CO_Statusword.bit.OperSpecific_b12 = 0;
	}
	// Home error
	if(CO_Controlword.bit.halt
	||(Home_Offset < Min_Position_Range_Limit)
	||(Home_Offset > Max_Position_Range_Limit)
	||(Home_Offset < (long long)Min_Position_Limit - Home_Offset)
	||(Home_Offset > (long long)Max_Position_Limit - Home_Offset))
	{
		CO_Statusword.bit.OperSpecific_b13 = 1;
	}
	else
	{
		CO_Statusword.bit.OperSpecific_b13 = 0;
	}
	Control_Effort = (long long)SPEED*Velocity_Divisor/Velocity_Numerator;
	CO_oldControlword.all = CO_Controlword.all;
}

/********************************************************************************
*Function name:    	CO_DrvStateMachine         		     						*
*Parameters:    	stw															*
*data:    													           			*
*Returns:    		zsw														    *
*        											                			*
*Description:    	canope state machine of servo controller					*
********************************************************************************/
void CO_DrvStateMachine(void)
{	
	// mode switch
	if(Operation_Mode != Operation_Mode_Display)
	{
		if((Operation_Mode_Display != HM_MODE)
		 &&(Operation_Mode_Display != PP_MODE)
		 &&(Operation_Mode_Display != IP_MODE)
		 &&(Operation_Mode_Display != PV_MODE))
		{
			if(labs(speed) < 20)
			{
				CO_Statusword.bit.TargetReached = 1;
			}
			Operation_Mode_Display = Operation_Mode;
		}
		if(CO_Statusword.bit.TargetReached)
		{
			Operation_Mode_Display = Operation_Mode;
		}
		if(Operation_Mode == PP_MODE || Operation_Mode == IP_MODE)
		{
			inPgcnt = 0;
			Clear_IPdata();
		}
	}
	if(int_flag3.bit.limitIqr)
	{
		CO_Statusword.bit.InLimitAct = 1;	
	}
	else
	{
		CO_Statusword.bit.InLimitAct = 0;
	}
//	CO_Statusword.bit.InLimitAct = int_flag3.bit.limitIqr;
	// mode status
	if(Operation_Mode_Display == HM_MODE)
	{
		if(CANopen402_regs.HomeStatus == HOME_END)
		{
			CO_Statusword.bit.OperSpecific_b12 = 1;//home_attain
		}
		else
		{
			if(CO_Controlword.bit.halt == 0)
			{
				CO_Statusword.bit.OperSpecific_b12 = 0;//home_not_attain
			}
			else
			{
				if(Velocity_Actual_Value == 0)
				{
					CO_Statusword.bit.OperSpecific_b12 = 1;//axis stop
				}
				else
				{
					CO_Statusword.bit.OperSpecific_b12 = 0;//axis acc
				}
			}
		}
		if((CANopen402_regs.HomeStatus != HOME_END) && (CO_Controlword.bit.halt)&&(CO_Statusword.bit.OperSpecific_b12))
		{
			CO_Statusword.bit.OperSpecific_b13 = 1;
		}
		else
		{
			if(homing_timeout)
			{
				if(CANopen402_regs.Home_Timeout/10 > homing_timeout)
				{
					CO_Statusword.bit.OperSpecific_b13 = 1;
				}
				else
				{
					CO_Statusword.bit.OperSpecific_b13 = 0;
				}
			}
			else
			{
				CO_Statusword.bit.OperSpecific_b13 = 0;
			}
		}
	}
	else if(Operation_Mode_Display == PV_MODE)
	{
		if(labs(speed) > ((Pn[MaxSpeedADD]+500)*SpeedUnit))
		{
			CO_Statusword.bit.OperSpecific_b13 = 1;
		}
		else
		{
			CO_Statusword.bit.OperSpecific_b13 = 0;
		}
	}
	// machine status
	if(state_flag2.bit.DispAlm && (CO_DrvState != FaultActiveState) && (CO_DrvState != FaultState))
	{		
		CO_Statusword.bit.SwitchednOnDisabled = 0;
		CO_Statusword.bit.QkStop = 0;
		CO_Statusword.bit.Fault = 0;
		CO_Statusword.bit.OperEnabled = 1;
		CO_Statusword.bit.SwitchednOn = 1;
		CO_Statusword.bit.ReadySwitchOn = 1;
		CO_DrvState = FaultActiveState;
	}
	switch(CO_DrvState)
	{
		case NotReadyToSwitchOnState:
			CO_Statusword.bit.SwitchednOnDisabled = 1;
			CO_Statusword.bit.Fault = 0;
			CO_Statusword.bit.OperEnabled = 0;
			CO_Statusword.bit.SwitchednOn = 0;
			CO_Statusword.bit.ReadySwitchOn = 0;
			CO_Statusword.bit.VoltageEnabled = 0;
			CO_DrvState = SwitchOnDisabledState;
			break;
		case SwitchOnDisabledState:
			CO_Statusword.bit.SwitchednOnDisabled = 1;
			CO_Statusword.bit.Fault = 0;
			CO_Statusword.bit.OperEnabled = 0;
			CO_Statusword.bit.SwitchednOn = 0;
			CO_Statusword.bit.ReadySwitchOn = 0;
			CO_Statusword.bit.VoltageEnabled = 0;
			if(CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage && !CO_Controlword.bit.SwitchON)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 0;
				CO_Statusword.bit.QkStop = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 1;
				CO_DrvState = ReadyToSwitchOnState;
				CO_DC_command = DC_Shutdown;
			}
			break;
		case ReadyToSwitchOnState:
			CO_Statusword.bit.SwitchednOnDisabled = 0;
			CO_Statusword.bit.QkStop = 1;
			CO_Statusword.bit.Fault = 0;
			CO_Statusword.bit.OperEnabled = 0;
			CO_Statusword.bit.SwitchednOn = 0;
			CO_Statusword.bit.ReadySwitchOn = 1;
			CO_Statusword.bit.VoltageEnabled = 0;
			if(int_flag3.bit.PowerFinsih && CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage && CO_Controlword.bit.SwitchON)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 0;
				CO_Statusword.bit.QkStop = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 1;
				CO_Statusword.bit.ReadySwitchOn = 1;
				CO_DrvState = SwitchedOnState;
				CO_DC_command = DC_SwitchOn;
			}
			else if(!CO_Controlword.bit.EnVoltage)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 0;
				CO_DrvState = SwitchOnDisabledState;
				CO_DC_command = DC_DisableVol;
			}
			else if(!CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 0;
				CO_DrvState = SwitchOnDisabledState;
				CO_DC_command = DC_QuickStop;
			}
			break;
		case SwitchedOnState:
			CO_Statusword.bit.SwitchednOnDisabled = 0;
			CO_Statusword.bit.QkStop = 1;
			CO_Statusword.bit.Fault = 0;
			CO_Statusword.bit.OperEnabled = 0;
			CO_Statusword.bit.SwitchednOn = 1;
			CO_Statusword.bit.ReadySwitchOn = 1;
			CO_Statusword.bit.VoltageEnabled = 0;
			if(CO_Controlword.bit.EnOper && CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage && CO_Controlword.bit.SwitchON)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 0;
				CO_Statusword.bit.QkStop = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 1;
				CO_Statusword.bit.SwitchednOn = 1;
				CO_Statusword.bit.ReadySwitchOn = 1;
				CO_DrvState = OperationEnableState;
				CO_DC_command = DC_EnableOper;
			}
			else if(CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage && !CO_Controlword.bit.SwitchON)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 0;
				CO_Statusword.bit.QkStop = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 1;
				CO_DrvState = ReadyToSwitchOnState;
				CO_DC_command = DC_Shutdown;
			}
			else if(!CO_Controlword.bit.EnVoltage)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 0;
				CO_DrvState = SwitchOnDisabledState;
				CO_DC_command = DC_DisableVol;
			}
			else if(!CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 0;
				CO_DrvState = SwitchOnDisabledState;
				CO_DC_command = DC_QuickStop;
			}
			break;
		case OperationEnableState:
			CO_Statusword.bit.SwitchednOnDisabled = 0;
			CO_Statusword.bit.QkStop = 1;
			CO_Statusword.bit.Fault = 0;
			CO_Statusword.bit.OperEnabled = 1;
			CO_Statusword.bit.SwitchednOn = 1;
			CO_Statusword.bit.ReadySwitchOn = 1;
			CO_Statusword.bit.VoltageEnabled = 1;
			if(!CO_Controlword.bit.EnOper && CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage && CO_Controlword.bit.SwitchON)
			{
				CO_DC_command = DC_DisableOper;
				if(!Disable_Operation_Option || (labs(speed) < 10*SpeedUnit))
				{
					CO_Statusword.bit.SwitchednOnDisabled = 0;
					CO_Statusword.bit.QkStop = 1;
					CO_Statusword.bit.Fault = 0;
					CO_Statusword.bit.OperEnabled = 0;
					CO_Statusword.bit.SwitchednOn = 1;
					CO_Statusword.bit.ReadySwitchOn = 1;
					CO_DrvState = SwitchedOnState;
				}
			}
			else if(CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage && !CO_Controlword.bit.SwitchON)
			{				
				CO_DC_command = DC_Shutdown;
				if(!Shutdown_Option || (labs(speed) < 10*SpeedUnit))
				{
					CO_Statusword.bit.SwitchednOnDisabled = 0;
					CO_Statusword.bit.QkStop = 1;
					CO_Statusword.bit.Fault = 0;
					CO_Statusword.bit.OperEnabled = 0;
					CO_Statusword.bit.SwitchednOn = 0;
					CO_Statusword.bit.ReadySwitchOn = 1;
					CO_DrvState = ReadyToSwitchOnState;
				}
			}
			else if(!CO_Controlword.bit.EnVoltage)
			{
				CO_DC_command = DC_DisableVol;
				if(!Disable_Operation_Option || (labs(speed) < 10*SpeedUnit))
				{
					CO_Statusword.bit.SwitchednOnDisabled = 1;
					CO_Statusword.bit.Fault = 0;
					CO_Statusword.bit.OperEnabled = 0;
					CO_Statusword.bit.SwitchednOn = 0;
					CO_Statusword.bit.ReadySwitchOn = 0;
					CO_DrvState = SwitchOnDisabledState;
				}				
			}
			else if (!CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage)
			{
				CO_Statusword.bit.SwitchednOnDisabled = 0;
				CO_Statusword.bit.QkStop = 0;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 1;
				CO_Statusword.bit.SwitchednOn = 1;
				CO_Statusword.bit.ReadySwitchOn = 1;
				CO_DrvState = QuickStopActiveState;
				CO_DC_command = DC_QuickStop;
			}
			break;
		case QuickStopActiveState:
			CO_Statusword.bit.SwitchednOnDisabled = 0;
			CO_Statusword.bit.QkStop = 0;
			CO_Statusword.bit.Fault = 0;
			CO_Statusword.bit.OperEnabled = 1;
			CO_Statusword.bit.SwitchednOn = 1;
			CO_Statusword.bit.ReadySwitchOn = 1;
			if(!CO_Controlword.bit.EnVoltage || (Quickstop_Option_Code == 0) || ((Quickstop_Option_Code < 5) && (labs(speed) < 10*SpeedUnit)))//braking has ended or disable voltage
			{
				CO_Statusword.bit.SwitchednOnDisabled = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 0;
				CO_Statusword.bit.VoltageEnabled = 0;
				CO_DrvState = SwitchOnDisabledState;
				if(!CO_Controlword.bit.EnVoltage)
				{
					CO_DC_command = DC_DisableVol;
				}
			}
			else if((Quickstop_Option_Code >= 5) && (Quickstop_Option_Code <= 8)
			 &&(CO_Controlword.bit.EnOper && CO_Controlword.bit.QkStop && CO_Controlword.bit.EnVoltage && CO_Controlword.bit.SwitchON))
			{
				CO_Statusword.bit.SwitchednOnDisabled = 0;
				CO_Statusword.bit.QkStop = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 1;
				CO_Statusword.bit.SwitchednOn = 1;
				CO_Statusword.bit.ReadySwitchOn = 1;
				CO_Statusword.bit.VoltageEnabled = 1;
				CO_DrvState = OperationEnableState;
				CO_DC_command = DC_EnableOper;
			}
			else 
			break;
		case FaultActiveState:
			CO_Statusword.bit.SwitchednOnDisabled = 0;
			CO_Statusword.bit.Fault = 1;
			CO_Statusword.bit.OperEnabled = 1;
			CO_Statusword.bit.SwitchednOn = 1;
			CO_Statusword.bit.ReadySwitchOn = 1;
			if(state_flag2.bit.Son == 0)//故障响应结束
			{	
				CO_Statusword.bit.SwitchednOnDisabled = 0;
				CO_Statusword.bit.Fault = 1;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 0;
				CO_Statusword.bit.VoltageEnabled = 0;
				CO_DrvState = FaultState;
			}
			break;
		case FaultState:
			CO_Statusword.bit.SwitchednOnDisabled = 0;
			CO_Statusword.bit.Fault = 1;
			CO_Statusword.bit.OperEnabled = 0;
			CO_Statusword.bit.SwitchednOn = 0;
			CO_Statusword.bit.ReadySwitchOn = 0;
			SyncError_TimeEarly = 0;//
			SyncError_TimeOut = 0;
			if((CO_Controlword.bit.RstAlm && !CO_lastControlword.bit.RstAlm) || (state_flag2.bit.DispAlm == 0))
			{
				CO_Statusword.bit.SwitchednOnDisabled = 1;
				CO_Statusword.bit.Fault = 0;
				CO_Statusword.bit.OperEnabled = 0;
				CO_Statusword.bit.SwitchednOn = 0;
				CO_Statusword.bit.ReadySwitchOn = 0;
				CO_Statusword.bit.VoltageEnabled = 0;
				CO_DrvState = SwitchOnDisabledState;
				CO_DC_command = DC_DisableVol;
				state_flag2.bit.DispAlm = 0;
				state_flag2.bit.HaveAlm = 0;
				state_flag2.bit.ClrAlm = 1;	
			}
			break;
		default:;
	}
	CO_lastControlword.all = CO_Controlword.all;
	if(Operation_Mode_Display != PP_MODE)
	{
		CO_oldControlword.all = CO_Controlword.all;
	}

}	
