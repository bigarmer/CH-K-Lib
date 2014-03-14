/**
  ******************************************************************************
  * @file    spi.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __SPI_H__
#define __SPI_H__

#include "common.h"
  
#define HW_SPI0     (0x00)
#define HW_SPI1     (0x01)
#define HW_SPI2     (0x02)
  
//!< CPHA
typedef enum
{
    kSPI_CPHA_1Edge,
    kSPI_CPHA_2Edge,
} SPI_CPHA_Type;

//!< CPOL
typedef enum
{
    kSPI_CPOL_InactiveLow,
    kSPI_CPOL_InactiveHigh,
} SPI_CPOL_Type;

typedef enum
{
    kSPI_Master,
    kSPI_Slave,
} SPI_Mode_Type;

//!< SPI ������ѡ��
typedef enum
{
    kSPI_BaudrateDiv_2,
    kSPI_BaudrateDiv_4,
    kSPI_BaudrateDiv_6,
    kSPI_BaudrateDiv_8,
    kSPI_BaudrateDiv_16,
    kSPI_BaudrateDiv_32,
    kSPI_BaudrateDiv_64,
    kSPI_BaudrateDiv_128,
    kSPI_BaudrateDiv_256,
    kSPI_BaudrateDiv_512,
    kSPI_BaudrateDiv_1024,
}SPI_BaudrateDiv_Type;

//!< interrupt and DMA select
typedef enum
{
    kSPI_IT_TCF_Disable,    //!< ��ֹTCF�ж�(TCF:����һ�����)
    kSPI_IT_TCF,            //!< SPI����һ������ж�ʹ��
}SPI_ITDMAConfig_Type;


#define kSPI_PCS_ReturnInactive   (0x00)      //!< ������ɺ�CS�źű���Ƭѡ��״̬
#define kSPI_PCS_KeepAsserted     (0x01)      //!< ������ɺ�CS�źű���δѡ��״̬

//!< ��ʼ���ṹ
typedef struct
{
    uint32_t                instance;               //!< ģ���
    SPI_CPHA_Type           CPHA;                   //!< ʱ����λ
    SPI_CPOL_Type           CPOL;                   //!< ʱ�Ӽ���
	SPI_Mode_Type           mode;                   //!< ����ģʽ
    SPI_BaudrateDiv_Type    baudrateDivSelect;      //!< �����ʷ�Ƶѡ��   
    uint8_t                 dataSizeInBit;          //!< ÿ֡�����ж���λ ͨ��Ϊ8��16
}SPI_InitTypeDef;

//!< ���ٳ�ʼ���ṹ

#define SPI0_SCK_PC05_SOUT_PC06_SIN_PC07   (0xca90U)
#define SPI0_SCK_PD01_SOUT_PD02_SIN_PD03   (0xc298U)
#define SPI1_SCK_PE02_SOUT_PE01_SIN_PE03   (0xc2a1U)
#define SPI0_SCK_PA15_SOUT_PA16_SIN_PA17   (0xde80U)
#define SPI2_SCK_PB21_SOUT_PB22_SIN_PB23   (0xea8aU)
#define SPI2_SCK_PD12_SOUT_PD13_SIN_PD14   (0xd89aU)

//!< Callback Type
typedef void (*SPI_CallBackType)(void);

//!< API functions
void SPI_Init(SPI_InitTypeDef * SPI_InitStruct);
uint16_t SPI_ReadWriteByte(uint32_t instance, uint16_t data, uint16_t CSn, uint16_t csState);
void SPI_ITDMAConfig(uint32_t instance, SPI_ITDMAConfig_Type config);
void SPI_CallbackInstall(uint32_t instance, SPI_CallBackType AppCBFun);
uint32_t SPI_QuickInit(uint32_t SPIxMAP);


#endif

