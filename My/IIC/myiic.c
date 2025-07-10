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

//�����ź���Ϊ����ģʽ
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




//��ʼ��IIC
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
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_us(4);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_H; 
	IIC_SDA_H;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
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
	IIC_SCL_L;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t  txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7){IIC_SDA_H;}
		else	{IIC_SDA_L;}
		txd<<=1; 	  
		delay_us(4);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_H;
		delay_us(4); 
		IIC_SCL_L;	
		delay_us(4);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



void IIC_Write_one_Byte(uint8_t daddr,uint8_t addr,uint8_t data)   // д24C02 daddr=0x0a0��д��ʾ��daddr=0x
	{
		char i;
		IIC_Start();//���������ź�
		IIC_Send_Byte(daddr);//I2Cдָ���IC��ַ
		IIC_Wait_Ack();
		IIC_Send_Byte(addr);//дEEPROM��ַ
		IIC_Wait_Ack();
		IIC_Send_Byte(data);
		IIC_Wait_Ack();
		IIC_Stop();//����ֹͣ�ź�
	}
			
void IIC_Write_len_Byte(uint8_t daddr,uint8_t addr, u8 *data,u8 len) 	
{
		char i;
	    char adr;
	/*
		IIC_Start();//���������ź�
		IIC_Send_Byte(daddr);//I2Cдָ���IC��ַ
		IIC_Wait_Ack();
		IIC_Send_Byte(addr);//дEEPROM��ַ
		IIC_Wait_Ack();
	for(i=0;i<len;i++)//д8������
		{
		IIC_Send_Byte(*data);
		data++;
		IIC_Wait_Ack();
		}
  IIC_Stop();//����ֹͣ�ź�
	*/
		adr=addr;
		for(i=0;i<len;i++)//д8������
		{
			IIC_Write_one_Byte(0xa0,adr,*data);
			IIC_Wait_Ack();
			data++;
			adr++;
			HAL_Delay(4);
		}
		HAL_Delay(20);
 }
	
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr)   //��24C02 daddr=0x0a1;
	 {
		 char error;
		 uint8_t i;
		 uint8_t Rdata;
		IIC_Start();//���������ź�

		IIC_Send_Byte(daddr);//I2Cдָ���IC��ַ
		error=IIC_Wait_Ack();
		 while(error)break;
		IIC_Send_Byte(addr);//дEEPROM��ַ
		error=IIC_Wait_Ack();
		 while(error)break;
		 IIC_Start();//���������ź�
		IIC_Send_Byte(daddr+1);//���Ͷ�
		error= IIC_Wait_Ack();
		 while(error)break;
		Rdata=IIC_Read_Byte(0);
		IIC_Stop();//����ֹͣ�ź�
		return Rdata;	 
		}	

uint8_t IIC_Read_len_Byte(uint8_t daddr,uint8_t addr,u8 len,u8  *data)   //��24C02 daddr=0x0a1;
	 {
		 uint8_t i;
		 uint8_t adr;
	/*
	 IIC_Start();//���������ź�

		IIC_Send_Byte(daddr);//I2Cдָ���IC��ַ
		IIC_Wait_Ack();
		IIC_Send_Byte(addr);//дEEPROM��ַ
		IIC_Wait_Ack();
		 IIC_Start();//���������ź�
		 IIC_Send_Byte(daddr+1);//���Ͷ�
		 IIC_Wait_Ack();
		 for(i=0;i<len;i++)//д8������
		{
		*data=IIC_Read_Byte(0);
		 data++;
		}
		 IIC_Stop();//����ֹͣ�ź�
		 */
		 adr=addr;
		 for(i=0;i<len;i++)//д8������
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
	temp=IIC_Read_One_Byte(0xa0,200);//����ÿ�ο�����дAT24CXX			   
	if(temp==0x78)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		IIC_Write_one_Byte(0xa0,200,0x78);//AT24CXX_WriteOneByte(255,0x55);
	    temp=IIC_Read_One_Byte(0xa0,200);	  
		if(temp==0x78)return 0;
	}
	return 1;											  
}


















