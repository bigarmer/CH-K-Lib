/**
  ******************************************************************************
  * @file    systick.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ���ļ�ΪоƬARM�ں��е�SysTickģ��ĵײ㹦�ܺ���
  ******************************************************************************
  */
#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "common.h"

//API functions
void SYSTICK_DelayInit(void);
void SYSTICK_Init(uint32_t timeInUs);
void SYSTICK_Cmd(FunctionalState NewState);
void SYSTICK_ITConfig(FunctionalState NewState);
void SYSTICK_DelayUs(uint32_t us);
void SYSTICK_DelayMs(uint32_t ms);

#endif

