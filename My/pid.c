#include "main.h"
#include "pid.h"
int32_t EncodrIError;
int32_t EncodrPidIElimit=10000;
int32_t EncodrPidMax=15000;
uint16_t  P=70;//100;




int32_t PIDencodr(int16_t  error)
{
    
    int32_t  i=0;
    int32_t Current_Error;
    int32_t out;
    Current_Error=error;
    //if(Current_Error>500)Current_Error=00;
   // if(Current_Error<-100)Current_Error=-100;
    EncodrIError=EncodrIError+Current_Error;
    if(EncodrIError>EncodrPidIElimit)EncodrIError=EncodrPidIElimit;
    if(EncodrIError<-EncodrPidIElimit)EncodrIError=-EncodrPidIElimit;
    out= P* Current_Error      //比例P
          + i * EncodrIError;//积分I

	//out=out/800;   //成品4000转， 800
	//out=out/500;   //成品 500= 5000转成功
	
	//out=out/200;   //成品200 = 90000转成功
	//out=out/100;   //成品 100= 9200转成功
	out=out/400;   //成品 = 9500?转成功
    if(out >EncodrPidMax)out =EncodrPidMax;
    if(out <-EncodrPidMax)out =-EncodrPidMax;

    
    return out;      ///3;

}


int32_t PowerSpdIError;
uint32_t PowerSpdIErrorIElimit=1000;
int32_t PIDPowerSpd(int32_t  UserSpd,int32_t NowSpd)
{
    
    
    int32_t Current_Error;
    int32_t out;
		
		uint16_t P=1;
		float  i=0;//0.2;
		
    Current_Error=UserSpd-NowSpd;
   
    PowerSpdIError=PowerSpdIError+Current_Error;
    if(PowerSpdIError>PowerSpdIErrorIElimit)PowerSpdIError=PowerSpdIErrorIElimit;
    if(PowerSpdIError<-PowerSpdIErrorIElimit)PowerSpdIError=-PowerSpdIErrorIElimit;
    out= P* Current_Error      //比例P
          + i * PowerSpdIError;//积分I
		out=out/2000;
		if(out>12000)out=12000;
		else if(out<-12000)out=-12000;
	
    return out;    

}