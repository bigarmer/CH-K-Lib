/**
  ******************************************************************************
  * @file    24l01.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "24l01.h"
#include "spi.h"
#include "board.h"
#include "gpio.h"
/*
#define BOARD_SPI_CS_PORT           HW_GPIOC
#define BOARD_SPI_CS_PIN            (2)
#define BOARD_NRF2401_CE_PORT       HW_GPIOB
#define BOARD_NRF2401_CE_PIN        (0)
#define BOARD_NRF2401_IRQ_PORT      HW_GPIOB
#define BOARD_NRF2401_IRQ_PIN       (1)
*/
uint8_t NRF2401_Init(void)
{
    SPI_QuickInit(BOARD_SPI_MAP, kSPI_CPOL0_CPHA0);
	GPIO_QuickInit(BOARD_SPI_CS_PORT, BOARD_SPI_CS_PIN , kGPIO_Mode_OPP);   //Init CS
    /*
	SPI_InitTypeDef SPI_InitStruct1;
	//CE���ų�ʼ��
	GPIO_QuickInit(HW_GPIOB, 0 , kGPIO_Mode_OPP);   //Init CE
    CE=0;
	 //IRQ���ų�ʼ��
	GPIO_QuickInit(HW_GPIOB, 1 , kGPIO_Mode_IPU);   //Init IRQ
	//��ʼ��SPI���нӿ�����
	SPI_InitStruct1.SPIxDataMap = SPI0_SCK_PC5_SOUT_PC6_SIN_PC7;
	SPI_InitStruct1.SPIxPCSMap =  SPI0_PCS2_PC2;
	SPI_InitStruct1.SPI_DataSize = 8;
	SPI_InitStruct1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStruct1.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct1.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct1.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct1.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(&SPI_InitStruct1);
	//��ʼ��Ӳ��GPIO����
	//������Ӳ�� CS ʹ��GPIO��CS
	GPIO_QuickInit(HW_GPIOC, 2 , kGPIO_Mode_OPP);   //Init CS
	//IO��ƽ��ʼ��
	CE =0;// chip enable
	CS =1;// cs disable
	//���NRF2401 �Ƿ����
	return 0;
    */
}

