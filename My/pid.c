#include "main.h"
#include "pid.h"
int32_t EncodrIError;
int32_t EncodrPidIElimit=850;
int32_t EncodrPidMax=15000;
uint16_t  P=200;//100; 平时100




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

int32_t  PowerLmt=700000;
int32_t PowerIError;
uint32_t PowerIErrorIElimit=200000;
float PowerP=0.0005;//0.0005;
float PowerI=0.0001;
float  ZL_PIDPower(int32_t  SetPower,int32_t FactPower)
{
    
    
    int32_t Current_Error;
    float out;
		
	
		
		
    Current_Error=SetPower-FactPower;  //大于输出正值，加速  最大值700000   输出12000最大
		if((Current_Error<30000)&&(Current_Error>-30000))PowerP=0.0001;
		else PowerP=0.0003;
    PowerIError=PowerIError+Current_Error;
    if(PowerIError>PowerIErrorIElimit)PowerIError=PowerIErrorIElimit;
    if(PowerIError<-(int)PowerIErrorIElimit)PowerIError=-(int)PowerIErrorIElimit;
    out= PowerP* Current_Error      //比例P
          + PowerI * PowerIError;//积分I 
		out=out;
		if(out>12000)out=12000;
		else if(out<-11400)out=-11400;
	
    return out;    

}


int32_t WZ_PowerIError;
float  WZ_PowerIErrorIElimit=100000;
float WZ_PowerP=0.001;
float WZ_PowerI=0.0001;
float WZ_PowerD=0.0001;
float WZ_Last_Error,WZ_Previous_Error;

float PID_WZ(int32_t  SetPower,int32_t FactPower)
{
 	 
	int32_t iError;	//当前误差

	float Increase;	//最后得出的实际增量
	float  IE;  //
	iError = SetPower - FactPower;	// 计算当前误差
	IE=WZ_PowerI * iError;

		IE=IE > WZ_PowerIErrorIElimit?WZ_PowerIErrorIElimit:IE;
		IE=IE < -(int)WZ_PowerIErrorIElimit?-(int)WZ_PowerIErrorIElimit:IE;

	Increase =  1.0*WZ_PowerP * (iError - WZ_Last_Error) + IE;//
			    
			   //+1.0*WZ_PowerD * (iError - 2 *  WZ_Last_Error + WZ_Previous_Error);  //微分D
	
	WZ_Previous_Error = WZ_Last_Error;	// 更新前次误差
	WZ_Last_Error = iError;		  	// 更新上次误差
	return Increase;	// 返回增量
}




