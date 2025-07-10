


#include "24cxx.h" 
//#include "delay.h"


uint8_t  iic_WRtemp[200];

uint32_t sEE_WaitEepromStandbyState(void)      
{
	HAL_Delay(100);
}



void bsp_InitI2c(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}


/****************************************************************************************
*函 数 名:i2c_Delay
*函数功能:延时
*形    参:无
*返 回 值:无
*****************************************************************************************/
static void i2c_Delay(void)
{
//uint8_t i=0;
//for(i = 0; i < 30; i++);
	delay_us(10);
}


/****************************************************************************************
*函 数 名:i2c_start
*函数功能:开始信号  
*形    参:无
*返 回 值:无
*****************************************************************************************/
void i2c_start(void)
{
I2C_SCL_0();/*SCL为高电平，SDA由高到低的跳变 就是开始信号*/
i2c_Delay();
I2C_SDA_1();
i2c_Delay();
I2C_SCL_1();
i2c_Delay();
I2C_SDA_0();
i2c_Delay();
I2C_SCL_0();
i2c_Delay();
}

/****************************************************************************************
*函 数 名:i2c_stop
*函数功能:结束
*形    参:无
*返 回 值:无
*****************************************************************************************/
void i2c_stop(void)
{
I2C_SCL_0();/*SCL在高电平期间，SDA由低到高的跳变 就是停止信号*/
i2c_Delay();
I2C_SDA_0();
i2c_Delay();
I2C_SCL_1();
i2c_Delay();
I2C_SDA_1();
i2c_Delay();
I2C_SCL_0();
i2c_Delay();
}

/****************************************************************************************
*函 数 名:i2c_WaitAck
*函数功能:从器件应答
*形    参:无
*返 回 值:1 从器件应答
*****************************************************************************************/
uint8_t i2c_WaitAck(void)
{
uint8_t re;
uint16_t i=0;/*SCL为高电平 从器件把ASD拉低 称为从器件应答*/
I2C_SCL_0();
i2c_Delay();
I2C_SDA_1();
i2c_Delay();
I2C_SCL_1();
i2c_Delay();
while(I2C_SDA_READ() && (i < 65530))/*防止从器件不应答 在这死等*/
{
i++;
}
if(i < 65530)/*小于65530 表示已经应答   具体i的值是多少我也不知道 这里只是大概*/
re = 1;
else
re = 0;
i2c_Delay();
I2C_SCL_0();
i2c_Delay();
return re;
}
/****************************************************************************************
*函 数 名:i2c_Ack
*函数功能:主器件应答
*形    参:无
*返 回 值:无
*****************************************************************************************/
void i2c_Ack(void)
{
I2C_SCL_0();/*SCL为高电平 SDA为低电平表示主器件应答*/
i2c_Delay();
I2C_SDA_0();
i2c_Delay();
I2C_SCL_1();
i2c_Delay();
I2C_SCL_0();
i2c_Delay();
}
/****************************************************************************************
*函 数 名:i2c_NAck
*函数功能：主器件不应答
*形    参:无
*返 回 值:无
*****************************************************************************************/
void i2c_NAck(void)
{
I2C_SCL_0();/*SCL为高电平 SDA为低电平表示主器件非应答*/
i2c_Delay();
I2C_SDA_1();
i2c_Delay();
I2C_SCL_1();
i2c_Delay();
I2C_SCL_0();
i2c_Delay();
}
/****************************************************************************************
*函 数 名:i2c_SendByte
*函数功能:发送一个字节
*形    参:_ucByte 发送的字节
*返 回 值:无
*****************************************************************************************/
void i2c_SendByte(uint8_t _ucByte)
{
uint8_t i;
I2C_SCL_0();
for( i = 0; i < 8; i++)
{
	if(_ucByte & 0x80){I2C_SDA_1();}
	else I2C_SDA_0();/*SCL上升沿将数据写入器件*/
i2c_Delay();
I2C_SCL_1();
i2c_Delay();
I2C_SCL_0();
i2c_Delay();
_ucByte = _ucByte << 1;
}
I2C_SDA_1();/*释放数据线*/
i2c_Delay();

}

/****************************************************************************************
*函 数 名:i2c_ReadByte
*函数功能:读到的值
*形    参:无
*返 回 值:无
*****************************************************************************************/
uint8_t i2c_ReadByte(void)
{
uint8_t i;
uint8_t value = 0;
I2C_SDA_1();/*释放数据线*/
for(i = 0; i < 8; i++)
{
value <<=1;
I2C_SCL_1();
i2c_Delay();
if(I2C_SDA_READ())/*读SDA*/
{
value = value + 1;
}
else
{
value = value;
}
i2c_Delay();/*SCL下降沿将数据取出*/
I2C_SCL_0();
i2c_Delay();
}
return value;

}

/*********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************                                                                      上面主要是模拟I2C时序                                                                                                                                                                    ************                                                                      下面就是对24C128的读写操作

*********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/



uint8_t buf[EE_SIZE];
/****************************************************************************************
*函 数 名:eeprom_ReadBytes
*函数功能:eeprom读字节
*形    参:_pReadBuf 存放读取的数据  _usAddress 开始读取的地址  _usSize读取的大小
*返 回 值:1 表示读取成功  0 表示读取失败
*****************************************************************************************/


uint8_t eeprom_ReadBytes(uint8_t *_pReadBuf,uint16_t _usAddress,uint16_t _usSize)
{
uint16_t i;
/*第一步：发起I2C启动信号*/
i2c_start();
/*第二步：发送控制字节，高7位表示地址，最低位表示读写控制 0表示写 1表示读*/
i2c_SendByte(EE_DEV_ADDR | I2C_WR);
/*第三步：等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
/*第四步：发送读取的地址 24C128 要发送2次地址*/
i2c_SendByte(_usAddress >> 8);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
/*发送读取的地址 */
i2c_SendByte(_usAddress);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
/*第五步：重启I2C总线  之后开始读取数据*/
i2c_start();
/*第六步：发送控制字节 */
i2c_SendByte(EE_DEV_ADDR | I2C_RD);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
/*第七步：循环读取数据*/
for(i = 0; i < _usSize; i++)
{
_pReadBuf[i] = i2c_ReadByte();/*读一个字节*/
if( i != _usSize -1)/*不是最后一个主器件就要应带，是最后一个就主器件非应答*/
{
i2c_Ack();
}
else
{
i2c_NAck();
}
}
/*第八步：发送I2C停止信号*/
i2c_stop();

return 1;

}

uint8_t eeprom_WriteBytes(uint8_t *_pWriteBuf,uint16_t _usAddress,uint16_t _usSize)
{
uint16_t i,m;
uint16_t usAddr;
uint16_t num = 0;
usAddr = _usAddress;
	

for( i = 0; i< _usSize; i++)
{
/*写eeprom不可以像读那样连续读取多个字节，写每次只能在一个页page
24C64每也32个字节*/
if(usAddr%32==0)
{
	num=0;
}
	
if (num % 32 == 0)       /*写入64个就要重新开始 因为每page为64*/
{
/*第一步：发送停止信号，启动内部写操作*/
i2c_stop();
/*通过循环判断eeprom内部写操作是否完成*/
for(m = 0; m < 100; m++)
{
/*第二步：发起I2C总线启动信号*/
i2c_start();
/*第三步：发起控制字节，高7位是地址，最低位 0表示写 1表示读*/
i2c_SendByte(EE_DEV_ADDR | I2C_WR);
/*第四步：等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() == 1)
{
break;                                                                                                                                                                                                                                                                                                                                                                                                                                      
}
if(m >= 100)/*大于100表示从器件没有应答*/
{
return 0;
}
}
/*第五步：发送写入的地址 24C128 要发送2次地址*/
i2c_SendByte(usAddr /256);//(usAddr >> 8);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
/*发送写入的地址*/
i2c_SendByte(usAddr %256);//(usAddr&0x00ff);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
}
/*第六步：写入数据*/
i2c_SendByte(_pWriteBuf[i]);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
num++;
usAddr++;/*地址增1*/
}

/*发送停止信号*/
i2c_stop();
HAL_Delay(2);
return 1;
}


/****************************************************************************************
*函 数 名:eeprom_WriteBytes
*函数功能:写整个eeprom测试
*形    参:_pWriteBuf 写入的值  _usAddress 写入的地址 _usSize写入的大小
*返 回 值:1 表示写入成功 0 表示写入失败
*****************************************************************************************/
uint8_t eeprom_WriteBytesBk(uint8_t *_pWriteBuf,uint16_t _usAddress,uint16_t _usSize)
{
uint16_t i,m;
uint16_t usAddr;
uint16_t num = 0;
usAddr = _usAddress;

for( i = 0; i< _usSize; i++)
{
/*写eeprom不可以像读那样连续读取多个字节，写每次只能在一个页page
24C64每也32个字节*/
if (num % 32 == 0)/*写入64个就要重新开始 因为每page为64*/
{
/*第一步：发送停止信号，启动内部写操作*/
i2c_stop();
/*通过循环判断eeprom内部写操作是否完成*/
for(m = 0; m < 100; m++)
{
/*第二步：发起I2C总线启动信号*/
i2c_start();
/*第三步：发起控制字节，高7位是地址，最低位 0表示写 1表示读*/
i2c_SendByte(EE_DEV_ADDR | I2C_WR);
/*第四步：等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() == 1)
{
break;
}
if(m >= 100)/*大于100表示从器件没有应答*/
{
return 0;
}
}
/*第五步：发送写入的地址 24C128 要发送2次地址*/
i2c_SendByte(usAddr /256);//(usAddr >> 8);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
/*发送写入的地址*/
i2c_SendByte(usAddr %256);//(usAddr&0x00ff);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
}
/*第六步：写入数据*/
i2c_SendByte(_pWriteBuf[i]);
/*等待从器件应答 1表示应答 0 表示没有应带*/
if(i2c_WaitAck() != 1)
{
return 0;
}
num++;
usAddr++;/*地址增1*/
}
/*发送停止信号*/
i2c_stop();
return 1;
}




u8 AT24C64_Check(void)
{
	u8 temp;
	eeprom_ReadBytes(&temp,300,1);//避免每次开机都写AT24CXX			   
	if(temp==0xAA)return 0;		   
	else//排除第一次初始化的情况
	{
		temp =0xaa;
		eeprom_WriteBytes(&temp,300,1);//AT24CXX_WriteOneByte(255,0x55);
	  eeprom_ReadBytes(&temp,300,1);
		if(temp==0xaa)return 0;
	}
	return 1;											  
}






#if 0
/****************************************************************************************
*函 数 名:eeprom_ReadTest
*函数功能:读取整个24C128的数据
*形    参:无
*返 回 值:无
*****************************************************************************************/
void eeprom_ReadTest(void)
{
uint16_t i;
int32_t Time1,Time2;

if(eeprom_ReadBytes(buf,0,EE_SIZE))/*1 读取成功 0读取失败*/
{

}
else
{

}

for(i = 0;i < EE_SIZE ;i++)
{

if(i % 16 == 0)/*输出16个字节换行*/
{

}


}


}




/****************************************************************************************
*函 数 名:eeprom_WriteTest
*函数功能:写入数据
*形    参:无
*返 回 值:无
*****************************************************************************************/


void eeprom_WriteTest(void)
{
uint16_t i;
int32_t Time1,Time2;

for(i = 0; i < EE_SIZE; i++)
{
buf[i] = 0x01;/*填充要写入的数据*/
}


if(eeprom_WriteBytes(buf,0,EE_SIZE))/*1 表示写入成功 0表示失败*/
{

}
else
{

}


}

#endif























