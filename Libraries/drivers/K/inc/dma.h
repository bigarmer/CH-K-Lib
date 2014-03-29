/**
  ******************************************************************************
  * @file    dma.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ���ļ�ΪоƬDMAģ��ĵײ㹦�ܺ���
  ******************************************************************************
  */
#ifndef __CH_LIB_DMA_H__
#define __CH_LIB_DMA_H__
  
#include <stdint.h>

//!< DMA ����Դѡ��
#define UART0_REV_DMAREQ  2
#define UART0_TRAN_DMAREQ 3
#define UART1_REV_DMAREQ  4
#define UART1_TRAN_DMAREQ 5
#define UART2_REV_DMAREQ  6
#define UART2_TRAN_DMAREQ 7
#define UART3_REV_DMAREQ  8
#define UART3_TRAN_DMAREQ 9
#define UART4_REV_DMAREQ  10
#define UART4_TRAN_DMAREQ 11
#define UART5_REV_DMAREQ  12
#define UART5_TRAN_DMAREQ 13
#define I2S0_REV_DMAREQ   14
#define I2S0_TRAN_DMAREQ  15
#define SPI0_REV_DMAREQ   16
#define SPI0_TRAN_DMAREQ  17
#define SPI1_REV_DMAREQ   18
#define SPI1_TRAN_DMAREQ  19
#define I2C0_DMAREQ       22
#define I2C1_DMAREQ       23
#define FTM0_CH0_DMAREQ   24
#define FTM0_CH1_DMAREQ   25
#define FTM0_CH2_DMAREQ   26
#define FTM0_CH3_DMAREQ   27
#define FTM0_CH4_DMAREQ   28
#define FTM0_CH5_DMAREQ   29
#define FTM0_CH6_DMAREQ   30
#define FTM0_CH7_DMAREQ   31
#define FTM1_CH0_DMAREQ   32
#define FTM1_CH1_DMAREQ   33
#define FTM2_CH0_DMAREQ   34
#define FTM2_CH1_DMAREQ   35
#define FTM3_CH0_DMAREQ   36
#define FTM3_CH1_DMAREQ   37
#define FTM3_CH2_DMAREQ   38
#define FTM1_CH3_DMAREQ   39  
#define ADC0_DMAREQ       40
#define ADC1_DMAREQ       41
#define CMP0_DMAREQ       42
#define CMP1_DMAREQ       43
#define CMP2_DMAREQ       44
#define DAC0_DMAREQ       45
#define DAC1_DMAREQ       46
#define CMT_DMAREQ        47
#define PDB_DMAREQ        48
#define PORTA_DMAREQ      49
#define PORTB_DMAREQ      50
#define PORTC_DMAREQ      51
#define PORTD_DMAREQ      52
#define PORTE_DMAREQ      53
#define FTM3_CH4_DMAREQ   54
#define FTM3_CH5_DMAREQ   55
#define FTM3_CH6_DMAREQ   56
#define FTM3_CH7_DMAREQ   57
#define DMA_MUX1          58
#define DMA_MUX2          59
#define DMA_MUX3          60


//!< DMAͨ��
#define HW_DMA_CH0    (0x00)
#define HW_DMA_CH1    (0x01)
#define HW_DMA_CH2    (0x02)
#define HW_DMA_CH3    (0x03)

//!< ����ģʽѡ��
typedef enum
{
    kDMA_TriggerSource_Normal,    //����ģʽ
    kDMA_TriggerSource_Periodic,  //���ڴ���ģʽ
}DMA_TriggerSource_Type;

//!< DMA�������ݶ˿�λ������
typedef enum
{
    kDMA_DataWidthBit_8,
    kDMA_DataWidthBit_16,
    kDMA_DataWidthBit_32,
}DMA_DataWidthBit_Type;

//!< ��ʼ���ṹ��
typedef struct 
{
    uint8_t         chl;                                //DMAͨ����0~15         
    uint8_t         chlTriggerSource;                   //DMA����Դѡ��
    uint16_t        minorByteTransferCount;             //MINOR LOOP ��һ�δ�����ֽ���
    uint16_t        majorTransferCount;                 //MAJOR LOOP ѭ������
    DMA_TriggerSource_Type triggerSourceMode;           //����ģʽѡ��
    /* ����Դ���� */
    uint32_t        sourceAddressMinorAdj;              //����Դ��ַ��MINOR LOOP ÿ�δ���ʱ��ƫ�����������ɸ�
    uint32_t        sourceAddress;                      //����Դ��ַ
    DMA_DataWidthBit_Type        sourceDataWidth;       //����Դ��ַ���ݿ�� 8 16 32
    uint32_t        sourceAddressMajorAdj;              //����MAJOR LOOPѭ����ɺ� Դ��ַƫ����
    /* Ŀ���������� */
    int32_t         destAddressMinorAdj;                //Ŀ���ַ��MINOR LOOP ÿ�δ���ʱ��ƫ�����������ɸ�
    uint32_t        destAddress;                        //Ŀ���ַ
    DMA_DataWidthBit_Type        destDataWidth;         //Ŀ���ַ���ݿ�� 8 16 32
    int32_t         destAddressMajorAdj;                //����MAJOR LOOPѭ����ɺ� Ŀ���ַƫ����
}DMA_InitTypeDef;
  

//!< interrupt select
typedef enum
{
    kDMA_IT_Half_Disable,
    kDMA_IT_Major_Disable,
    kDMA_IT_Half,
    kDMA_IT_Major,
}DMA_ITConfig_Type;  

//!< Callback Type
typedef void (*DMA_CallBackType)(void);

 //!< API functions
void DMA_StartTransfer(uint8_t chl);
void DMA_Init(DMA_InitTypeDef *DMA_InitStruct);
uint8_t DMA_IsTransferComplete(uint8_t chl);
void DMA_ITConfig(uint8_t chl, DMA_ITConfig_Type config);
void DMA_CallbackInstall(uint8_t chl, DMA_CallBackType AppCBFun);
void DMA_CancelTransfer(uint8_t chl);
void DMA_SetDestAddress(uint8_t chl, uint32_t address);
void DMA_SetSourceAddress(uint8_t chl, uint32_t address);


#endif
  
