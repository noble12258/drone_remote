#include "usart.h"
#include "log_lib.h"
#include <string.h>
#include "systick.h"

#define UART1_SEND_BUFF_LENGTH	512

static uint8_t uart1SendBuff[UART1_SEND_BUFF_LENGTH] = {0};

//上外上位机串口初始化
void Uart1GpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1, ENABLE);

    //  TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //  RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Uart1Config(uint32_t baudRate)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = baudRate;		//设置波特率，这里由参数决定
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //停止位，这里设置1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                             //无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                 //模式选择；TX和RX
  USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);		//RX中断使能
    
	USART_Cmd(USART1, ENABLE);		//使能串口2
	
}

/* 串口DMA通道配置                */
/* 存储器到外设传输方向              */
/* DMA_CHx:         DMA传输通道x     */
/* peripheral_addr: 外设地址         */
/* memory_addr:     内存地址         */
/* data_length:     传输的数据长度   */  
static void UsartDmaConfig(DMA_Channel_TypeDef* DMA_CHx, uint32_t peripheral_addr, uint32_t memory_addr, u16 data_length)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		//时钟使能                     
                                                                                
    DMA_DeInit(DMA_CHx);		//复位
                                                                                
    DMA_InitStructure.DMA_PeripheralBaseAddr = peripheral_addr;                 //外设地址     
    DMA_InitStructure.DMA_MemoryBaseAddr = memory_addr;                          //内存地址  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          //外设作为传输的目的地
    DMA_InitStructure.DMA_BufferSize = data_length;                             //数据缓存大小                       
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址不自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据宽度8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存数据宽度8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                //无内存到内存传输                    
    DMA_Init(DMA_CHx, &DMA_InitStructure);                                 
}

void UartInit(void)
{
	Uart1GpioInit();
	Uart1Config(115200);
	UsartDmaConfig(DMA1_Channel4, (uint32_t)&USART1->DR, (uint32_t)uart1SendBuff, UART1_SEND_BUFF_LENGTH);
}

/* 串口DMA数据发送 */
void Usart1DMASendData(uint8_t *sendBuff, u16 len)
{	
	static uint8_t firstSendFlag = 0;

	while(1){
		if(DMA_GetFlagStatus(DMA1_FLAG_TC4)==SET){//判断通道4传输完成
			DMA_ClearFlag(DMA1_FLAG_TC4);
			break;
		}
		else if(firstSendFlag == 0){
			firstSendFlag = 1;
			break;
		}
		delay_ms(2);
	}
	
	DMA_Cmd(DMA1_Channel4, DISABLE); //关闭
	memcpy((void *)uart1SendBuff, (void *)sendBuff, len);
	DMA_SetCurrDataCounter(DMA1_Channel4, len);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);		//使能串口DMA发送
  DMA_Cmd(DMA1_Channel4, ENABLE);		//使能DMA传输
	

}
