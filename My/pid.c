#include "main.h"
#include "pid.h"
int32_t EncodrIError;
int32_t EncodrPidIElimit=850;
int32_t EncodrPidMax=15000;
uint16_t  P=200;//100; ƽʱ100




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
    out= P* Current_Error      //����P
          + i * EncodrIError;//����I

	//out=out/800;   //��Ʒ4000ת�� 800
	//out=out/500;   //��Ʒ 500= 5000ת�ɹ�
	
	//out=out/200;   //��Ʒ200 = 90000ת�ɹ�
	//out=out/100;   //��Ʒ 100= 9200ת�ɹ�
	out=out/400;   //��Ʒ = 9500?ת�ɹ�
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
		
	
		
		
    Current_Error=SetPower-FactPower;  //���������ֵ������  ���ֵ700000   ���12000���
		if((Current_Error<30000)&&(Current_Error>-30000))PowerP=0.0001;
		else PowerP=0.0003;
    PowerIError=PowerIError+Current_Error;
    if(PowerIError>PowerIErrorIElimit)PowerIError=PowerIErrorIElimit;
    if(PowerIError<-(int)PowerIErrorIElimit)PowerIError=-(int)PowerIErrorIElimit;
    out= PowerP* Current_Error      //����P
          + PowerI * PowerIError;//����I 
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
 	 
	int32_t iError;	//��ǰ���

	float Increase;	//���ó���ʵ������
	float  IE;  //
	iError = SetPower - FactPower;	// ���㵱ǰ���
	IE=WZ_PowerI * iError;

		IE=IE > WZ_PowerIErrorIElimit?WZ_PowerIErrorIElimit:IE;
		IE=IE < -(int)WZ_PowerIErrorIElimit?-(int)WZ_PowerIErrorIElimit:IE;

	Increase =  1.0*WZ_PowerP * (iError - WZ_Last_Error) + IE;//
			    
			   //+1.0*WZ_PowerD * (iError - 2 *  WZ_Last_Error + WZ_Previous_Error);  //΢��D
	
	WZ_Previous_Error = WZ_Last_Error;	// ����ǰ�����
	WZ_Last_Error = iError;		  	// �����ϴ����
	return Increase;	// ��������
}




