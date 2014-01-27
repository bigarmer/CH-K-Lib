/**
  ******************************************************************************
  * @file    pit.h
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.23
  * @brief   ����K60�̼��� �ڲ���ʱ������ ͷ�ļ�
  ******************************************************************************
  */
#ifndef __PIT_H__
#define __PIT_H__

#include "common.h"

#define PIT_DELAY_CHL   PIT3



#ifdef __cplusplus
 extern "C" {
#endif



	 
//��ʹ�õ�UART��ʼ���ṹ
typedef enum
{
    PIT0,                
    PIT1,                
    PIT2,  
    PIT3,	
}PIT_MapSelect_TypeDef;

//���������
#define IS_PIT_CH(CH)     (((CH) == PIT0) || \
													((CH) == PIT1)  || \
													((CH) == PIT2)  || \
													((CH) == PIT3))


//PIT ��ʼ���ṹ
typedef struct
{
    PIT_MapSelect_TypeDef  PITxMap;             //!< PIT Module and pinmux select 
    uint32_t PIT_PeriodInUs;                    //!< Tick Period Us
}PIT_InitTypeDef;

//��������ʵ�ֵĽӿں���
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct);
void PIT_ITConfig(PIT_MapSelect_TypeDef PITx, FunctionalState NewState);
ITStatus PIT_GetITStates(PIT_MapSelect_TypeDef PITx);
void PIT_ClearITPendingBit(PIT_MapSelect_TypeDef PITx);
void PIT_ClearAllITPendingBit(void);
void PIT_DelayInit(void);
uint32_t PIT_GetReloadValue(PIT_MapSelect_TypeDef PITx);
void PIT_SetReloadValue(PIT_MapSelect_TypeDef PITx, uint32_t Value);
uint32_t PIT_GetCurrentCounter(PIT_MapSelect_TypeDef PITx);
void PIT_Start(PIT_MapSelect_TypeDef PITx);
void PIT_Stop(PIT_MapSelect_TypeDef PITx);
void PIT_Cmd(FunctionalState NewState);
void PIT_DelayUs(uint32_t us);
void PIT_DelayMs(uint32_t ms);


#ifdef __cplusplus
}
#endif

#endif



