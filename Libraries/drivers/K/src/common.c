/**
  ******************************************************************************
  * @file    common.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "common.h"
#include <string.h>
#include "systick.h"

 /**
 * @brief  ������ٳ�ʼ���ṹ �û�һ�㲻�����
 *
 * @param  type: I2C���ٳ�ʼ���ṹ
 * @retval 32λ���ٳ�ʼ������
 */
uint32_t QuickInitEncode(QuickInit_Type type)
{
    uint32_t value = 0;
    value = type.ip_instance<<0;
    value|= type.io_instance<<3;
    value|= type.mux<<6;
    value|= type.io_base<<9;
    value|= type.io_offset<<14;
    value|= type.channel<<19;
    return value;
}

 /**
 * @brief  ������ٳ�ʼ���ṹ �û�һ�㲻�����
 *
 * @param  map: 32λ���ٳ�ʼ������
 * @param  type: I2C���ٳ�ʼ���ṹָ��
 * @retval None
 */
void QuickInitDecode(uint32_t map, QuickInit_Type* type)
{
    QuickInit_Type * pMap = (QuickInit_Type*)&(map);
    memcpy(type, pMap, sizeof(QuickInit_Type));  
}

#pragma weak DelayMs
void DelayMs(uint32_t ms)
{
    SYSTICK_DelayMs(ms);
}

#pragma weak DelayUs
void DelayUs(uint32_t ms)
{
    SYSTICK_DelayUs(ms);
}

#pragma weak DelayInit
void DelayInit(void)
{
    SYSTICK_DelayInit();
}





