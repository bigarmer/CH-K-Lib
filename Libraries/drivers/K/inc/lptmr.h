/**
  ******************************************************************************
  * @file    lptmr.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __LPTMR_H__
#define __LPTMR_H__

#include "common.h"

#define HW_LPTMR0   (0x00)

//!< LPTMR �ص���������
typedef void (*LPTMR_CallBackType)(void);

//!< LPTMR �ж�DMA����
typedef enum
{
    kLPTMR_IT_Disable,        //!< Disable Interrupt
    kLPTMR_IT_TOF,            //!< Enable time overflow flag
}LPTMR_ITDMAConfig_Type;

//!< LPTMR ��ʱ������ ��ʼ��
typedef struct
{
    uint32_t        instance;
    uint16_t        timeInMs;
}LPTMR_TC_InitTypeDef;

//!< LPTMR �������Դѡ��
typedef enum
{
    kLPTMR_PC_InputSource_CMP0, //!< CMP0 ��Ϊ�������ʱ��Դ
    kLPTMR_PC_InputSource_ALT1, //!< �ⲿ����LPTMR_ALT1��Ϊ�ⲿ����ʱ��Դ
    kLPTMR_PC_InputSource_ALT2, //!< �ⲿ����LPTMR_ALT2��Ϊ�ⲿ����ʱ��Դ
}LPTMR_PC_IntputSource_Type;

//!< LPTMR �ⲿ������Ϊ����ʱ ��������ѡ��
typedef enum
{
    kLPTMR_PC_PinPolarity_RigsingEdge,
    kLPTMR_PC_PinPolarity_FallingEdge,
}LPTMR_PC_PinPolarity_Type;

//!< LPTMR �����ⲿ��·���弼�� ��ʼ��
typedef struct
{
    uint32_t                        instance;               //!<ģ���
    uint16_t                        counterOverflowValue;   //!<�����ۼ��������ֵ ���0xFFFF
    LPTMR_PC_IntputSource_Type      inputSource;            //!< ����Դѡ��
    LPTMR_PC_PinPolarity_Type       pinPolarity;            //!< ���벶׽����ѡ��
}LPTMR_PC_InitTypeDef;

//!< ���ٳ�ʼ���ṹ ���ڵ�·�������
#define LPTMR_ALT1_PA19   (0x86780U)
#define LPTMR_ALT2_PC05   (0x104b10U)

//!< API functions
void LPTMR_TC_Init(LPTMR_TC_InitTypeDef* LPTMR_TC_InitStruct);
void LPTMR_PC_Init(LPTMR_PC_InitTypeDef* LPTMR_PC_InitStruct);
void LPTMR_ITDMAConfig(LPTMR_ITDMAConfig_Type config);
void LPTMR_CallbackInstall(LPTMR_CallBackType AppCBFun);
uint32_t LPTMR_PC_ReadCounter(void);
uint32_t LPTMR_PC_QuickInit(uint32_t LPTMRxMAP);
void LPTMR_ClearCount(void);

#endif

