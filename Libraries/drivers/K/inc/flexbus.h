/**
  ******************************************************************************
  * @file    flexbus.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __FLEXBUS_H__
#define __FLEXBUS_H__

#include <stdint.h>

//!< Flexbus ���ݶ��䷽ʽѡ��
#define kFLEXBUS_DataLeftAligned   (0x00)
#define kFLEXBUS_DataRightAligned  (0x01)

//!< Flexbus �Զ�Ӧ���ź�ʹ��
#define kFLEXBUS_AutoAckEnable     (0x00)
#define kFLEXBUS_AutoAckDisable    (0x01)

//!< Flexbus �˿�λ��ѡ��
#define kFLEXBUS_PortSize_8Bit     (0x01)
#define kFLEXBUS_PortSize_16Bit    (0x02)
#define kFLEXBUS_PortSize_32Bit    (0x00)

//!< Flexbus Ƭѡ�ź�ѡ�� 
#define kFLEXBUS_CS0               (0x00)
#define kFLEXBUS_CS1               (0x01)
#define kFLEXBUS_CS2               (0x02)
#define kFLEXBUS_CS3               (0x03)
#define kFLEXBUS_CS4               (0x04)
#define kFLEXBUS_CS5               (0x05)

//!< Flexbus Ƭѡ��Χ �ο�ѡ��
#define kFLEXBUS_ADSpace_64KByte        (0x00)
#define kFLEXBUS_ADSpace_128KByte       (0x01)
#define kFLEXBUS_ADSpace_512KByte       (0x07)
#define kFLEXBUS_ADSpace_1MByte         (0x0F)

//!< Flexbus BE�źſ���ģʽѡ��
#define kFLEXBUS_BE_AssertedWrite       (0x00)
#define kFLEXBUS_BE_AssertedReadWrite   (0x01)

//!< Flexus �����źŸ���ѡ��
#define kFLEXBUS_CSPMCR_GROUP1_ALE      (0x00)
#define kFLEXBUS_CSPMCR_GROUP1_CS1      (0x01)
#define kFLEXBUS_CSPMCR_GROUP1_TS       (0x02)
#define kFLEXBUS_CSPMCR_GROUP2_CS4      (0x00)
#define kFLEXBUS_CSPMCR_GROUP2_TSIZ0    (0x01)
#define kFLEXBUS_CSPMCR_GROUP2_BE_31_24 (0x02)
#define kFLEXBUS_CSPMCR_GROUP3_CS5      (0x00)
#define kFLEXBUS_CSPMCR_GROUP3_TSIZ1    (0x01)
#define kFLEXBUS_CSPMCR_GROUP3_BE_23_16 (0x02)
#define kFLEXBUS_CSPMCR_GROUP4_TBST     (0x00)
#define kFLEXBUS_CSPMCR_GROUP4_CS2      (0x01)
#define kFLEXBUS_CSPMCR_GROUP4_BE_15_8  (0x02)
#define kFLEXBUS_CSPMCR_GROUP5_TA       (0x00)
#define kFLEXBUS_CSPMCR_GROUP5_CS3      (0x01)
#define kFLEXBUS_CSPMCR_GROUP5_BE_7_0   (0x02)

//!< FLEXBUS��ʼ���ṹ��
typedef struct
{
    uint32_t dataWidth;
    uint32_t baseAddress;
    uint32_t ADSpaceMask;
    uint32_t dataAlignMode;
    uint32_t autoAckMode;
    uint32_t ByteEnableMode;
    uint32_t CSn;
    uint32_t CSPortMultiplexingCotrol;
}FLEXBUS_InitTypeDef;

//!< API functions
void FLEXBUS_Init(FLEXBUS_InitTypeDef* FLEXBUS_InitStruct);



#endif

