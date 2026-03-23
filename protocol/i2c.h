#ifndef _iic_h_
#define _iic_h_

#include "stm32f10x.h"

#define  IIC_RCC_PORT   RCC_APB2Periph_GPIOB

#define  SCL_PORT   GPIOB
#define  SDA_PORT   GPIOB

#define  SCL_PIN    GPIO_Pin_7
#define  SDA_PIN    GPIO_Pin_6

#define SCL_HIGH          SCL_PORT->BSRR = SCL_PIN
#define SCL_LOW           SCL_PORT->BRR  = SCL_PIN

#define SDA_HIGH          SDA_PORT->BSRR = SDA_PIN 
#define SDA_LOW           SDA_PORT->BRR  = SDA_PIN

#define SDA_READ        ((SDA_PORT->IDR &  SDA_PIN)!=0) ? 1 : 0

void I2cInit(void); 

void SDA_OUT(void);
void SDA_IN(void);

void IIC_Start(void);
void IIC_Stop(void);
unsigned char IIC_Slave_Ack(void);
void IIC_Send_Byte(unsigned char byte);
unsigned char IIC_Read_Byte(void);

void IIC_Write_One_Byte(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);

unsigned char IIC_Read_One_Byte(unsigned char SlaveAddress,unsigned char REG_Address);
unsigned short int IIC_Read_Two_Bytes(unsigned char SlaveAddress,unsigned char REG_Address);


#endif


