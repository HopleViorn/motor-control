#ifdef new
#ifndef __24CXX_H
#define __24CXX_H
#include "myiic24c64.h"   


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//Mini STM32������ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02
#define EE_TYPE AT24C64

u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//ָ����ַ��ȡһ���ֽ�
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//ָ����ַд��һ���ֽ�
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					//ָ����ַ��ʼ��ȡָ����������
//void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ�����
//void AT24CXX_Read(char ReadAddr,u8 *pBuffer,u16 NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�����
void AT24CXX_Write(char *pBuffer,u16 WriteAddr,u16 NumToWrite);

void AT24CXX_Read(char *pBuffer,u16 ReadAddr,u16 NumToRead);

u8 AT24CXX_Check(void);  //�������
void AT24CXX_Init(void); //��ʼ��IIC
#endif
#endif



#ifndef __24CXX_H
#define __24CXX_H

#include "myiic.h"

#include "sys.h"   

//#include "myiic24c64.h"   
#include "main.h"  

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t


#if 1
#define I2C_SCL_PIN GPIO_Pin_14/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN GPIO_Pin_13/* ���ӵ�SDA�����ߵ�GPIO */
/* �����дSCL��SDA�ĺ� */

 
#define IIC_SCL    PDout(14) //SCL
#define IIC_SDA    PDout(13) //SDA	 
//#define READ_SDA   PDin(13)  //����SDA 

#define I2C_SCL_1() PDout(14)=1;			// GPIO_PORT_I2C->BSRRL = I2C_SCL_PIN/* SCL = 1 */
#define I2C_SCL_0()  PDout(14)=0;		//GPIO_PORT_I2C->BSRRH = I2C_SCL_PIN/* SCL = 0 */


#define I2C_SDA_1()  PDout(13)=1;//GPIO_PORT_I2C->BSRRL = I2C_SDA_PIN/* SDA = 1 */
#define I2C_SDA_0()  PDout(13)=0;//GPIO_PORT_I2C->BSRRH = I2C_SDA_PIN/* SDA = 0 */


#define I2C_SDA_READ()   PDin(13)//HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_13)//	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)/* ��SDA����״̬ */
//#define I2C_SCL_READ()   HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_14)// GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)/* ��SCL����״̬ */


#define EE_MODEL_NAME "24C64"//"AT24C128"
#define EE_DEV_ADDR 0xA0/* �豸��ַ */
#define EE_PAGE_SIZE 32/* ҳ���С(�ֽ�) */
#define EE_SIZE (256*32)/* ������(�ֽ�) */
#define EE_ADDR_BYTES 2/* ��ַ�ֽڸ��� */


#define I2C_WR 0/* д����bit */
#define I2C_RD 1/* ������bit */
#endif

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//Mini STM32������ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02
#define EE_TYPE AT24C02
//					  
//u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//ָ����ַ��ȡһ���ֽ�
//void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//ָ����ַд��һ���ֽ�
//void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
//u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					//ָ����ַ��ʼ��ȡָ����������
////void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ�����
////void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�����

//void AT24CXX_Write(char *pBuffer,u16 WriteAddr,u16 NumToWrite);

//void AT24CXX_Read(char *pBuffer,u16 ReadAddr,u16 NumToRead);
//u8 AT24CXX_Check(void);  //�������
//void AT24CXX_Init(void); //��ʼ��IIC
//void sEE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
//uint32_t sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t* NumToRead);
//uint32_t sEE_WaitEepromStandbyState(void);



//#endif

//void InitI2c(void);
//void eeprom_ReadTest(void);
//void eeprom_WriteTest(void);

void bsp_InitI2c(void);
uint8_t eeprom_WriteBytes(uint8_t *_pWriteBuf,uint16_t _usAddress,uint16_t _usSize);
uint8_t eeprom_ReadBytes(uint8_t *_pReadBuf,uint16_t _usAddress,uint16_t _usSize);
u8 AT24C64_Check(void);


#endif
































