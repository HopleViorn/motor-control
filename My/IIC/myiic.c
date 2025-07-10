#include "myiic.h"
#define u8 uint8_t

void delay_us(uint16_t numb)
{
	uint16_t i;
	for(i=0;i<numb;i++)
	{
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}
}

//数据信号设为输入模式
void SDA_IN(void)   {
												GPIO_InitTypeDef  GPIO_InitStructure;
												GPIO_InitStructure.Pin = TM_DAT_Pin; 
												GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;//GPIO_Speed_2MHz; 
												GPIO_InitStructure.Mode =GPIO_MODE_INPUT; 
												HAL_GPIO_Init(TM_DAT_GPIO_Port, &GPIO_InitStructure);
										}

void SDA_OUT(void) {	GPIO_InitTypeDef  GPIO_InitStructure;
												GPIO_InitStructure.Pin = TM_DAT_Pin; 
												GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;//GPIO_Speed_2MHz; 
												GPIO_InitStructure.Mode =GPIO_MODE_OUTPUT_PP; 
												HAL_GPIO_Init(TM_DAT_GPIO_Port, &GPIO_InitStructure);
}




//初始化IIC
void IIC_Init(void)
{					     
											GPIO_InitTypeDef GPIO_InitStructure = {0};

											__HAL_RCC_GPIOE_CLK_ENABLE();
												GPIO_InitStructure.Pin = TM_CLK_Pin; 
												GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;//GPIO_Speed_2MHz; 
												GPIO_InitStructure.Mode =GPIO_MODE_OUTPUT_PP; 
												HAL_GPIO_Init(TM_CLK_GPIO_Port, &GPIO_InitStructure);
											
												GPIO_InitStructure.Pin = TM_DAT_Pin; 
												GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;//GPIO_Speed_2MHz; 
												GPIO_InitStructure.Mode =GPIO_MODE_OUTPUT_OD;
												HAL_GPIO_Init(TM_DAT_GPIO_Port, &GPIO_InitStructure);
											
											
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_us(4);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_L;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_H; 
	IIC_SDA_H;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA_H;delay_us(3);	   
	IIC_SCL_H;delay_us(3);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_L;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_L;
	delay_us(3);
	IIC_SCL_H;
	delay_us(3);
	IIC_SCL_L;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H;
	delay_us(3);
	IIC_SCL_H;
	delay_us(3);
	IIC_SCL_L;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t  txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_L;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7){IIC_SDA_H;}
		else	{IIC_SDA_L;}
		txd<<=1; 	  
		delay_us(4);   //对TEA5767这三个延时都是必须的
		IIC_SCL_H;
		delay_us(4); 
		IIC_SCL_L;	
		delay_us(4);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL_L; 
        delay_us(4);
		IIC_SCL_H;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(4); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



void IIC_Write_one_Byte(uint8_t daddr,uint8_t addr,uint8_t data)   // 写24C02 daddr=0x0a0，写显示：daddr=0x
	{
		char i;
		IIC_Start();//产生启动信号
		IIC_Send_Byte(daddr);//I2C写指令和IC地址
		IIC_Wait_Ack();
		IIC_Send_Byte(addr);//写EEPROM地址
		IIC_Wait_Ack();
		IIC_Send_Byte(data);
		IIC_Wait_Ack();
		IIC_Stop();//产生停止信号
	}
			
void IIC_Write_len_Byte(uint8_t daddr,uint8_t addr, u8 *data,u8 len) 	
{
		char i;
	    char adr;
	/*
		IIC_Start();//产生启动信号
		IIC_Send_Byte(daddr);//I2C写指令和IC地址
		IIC_Wait_Ack();
		IIC_Send_Byte(addr);//写EEPROM地址
		IIC_Wait_Ack();
	for(i=0;i<len;i++)//写8个数据
		{
		IIC_Send_Byte(*data);
		data++;
		IIC_Wait_Ack();
		}
  IIC_Stop();//产生停止信号
	*/
		adr=addr;
		for(i=0;i<len;i++)//写8个数据
		{
			IIC_Write_one_Byte(0xa0,adr,*data);
			IIC_Wait_Ack();
			data++;
			adr++;
			HAL_Delay(4);
		}
		HAL_Delay(20);
 }
	
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr)   //读24C02 daddr=0x0a1;
	 {
		 char error;
		 uint8_t i;
		 uint8_t Rdata;
		IIC_Start();//产生启动信号

		IIC_Send_Byte(daddr);//I2C写指令和IC地址
		error=IIC_Wait_Ack();
		 while(error)break;
		IIC_Send_Byte(addr);//写EEPROM地址
		error=IIC_Wait_Ack();
		 while(error)break;
		 IIC_Start();//产生启动信号
		IIC_Send_Byte(daddr+1);//发送读
		error= IIC_Wait_Ack();
		 while(error)break;
		Rdata=IIC_Read_Byte(0);
		IIC_Stop();//产生停止信号
		return Rdata;	 
		}	

uint8_t IIC_Read_len_Byte(uint8_t daddr,uint8_t addr,u8 len,u8  *data)   //读24C02 daddr=0x0a1;
	 {
		 uint8_t i;
		 uint8_t adr;
	/*
	 IIC_Start();//产生启动信号

		IIC_Send_Byte(daddr);//I2C写指令和IC地址
		IIC_Wait_Ack();
		IIC_Send_Byte(addr);//写EEPROM地址
		IIC_Wait_Ack();
		 IIC_Start();//产生启动信号
		 IIC_Send_Byte(daddr+1);//发送读
		 IIC_Wait_Ack();
		 for(i=0;i<len;i++)//写8个数据
		{
		*data=IIC_Read_Byte(0);
		 data++;
		}
		 IIC_Stop();//产生停止信号
		 */
		 adr=addr;
		 for(i=0;i<len;i++)//写8个数据
		{
		
		*data=IIC_Read_One_Byte(0xa0,adr);
		 adr++;
		 data++;
			HAL_Delay(3);
		}
		 
		 
}	
		
		
		
u8 AT24C01_Check(void)
{
	u8 temp;
	temp=IIC_Read_One_Byte(0xa0,200);//避免每次开机都写AT24CXX			   
	if(temp==0x78)return 0;		   
	else//排除第一次初始化的情况
	{
		IIC_Write_one_Byte(0xa0,200,0x78);//AT24CXX_WriteOneByte(255,0x55);
	    temp=IIC_Read_One_Byte(0xa0,200);	  
		if(temp==0x78)return 0;
	}
	return 1;											  
}


















