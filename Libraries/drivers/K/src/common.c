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
 * @param  type: ���ٳ�ʼ���ṹ��ָ��
 * @retval 32λ���ٳ�ʼ������
 */
uint32_t QuickInitEncode(QuickInit_Type * type)
{
    return *(uint32_t*)type;
}

 /**
 * @brief  ������ٳ�ʼ���ṹ �û�һ�㲻�����
 *
 * @param  map: 32λ���ٳ�ʼ������
 * @param  type: I2C���ٳ�ʼ���ṹָ��
 * @retval None
 */
void QuickInitDecode(uint32_t map, QuickInit_Type * type)
{
    QuickInit_Type * pMap = (QuickInit_Type*)&(map);
    memcpy(type, pMap, sizeof(QuickInit_Type));  
}

#if (defined(__CC_ARM))
__weak void DelayMs(uint32_t ms)
#elif (defined(__ICCARM__))
#pragma weak DelayMs
void DelayMs(uint32_t ms)
#endif
{
    SYSTICK_DelayMs(ms);
}

#if (defined(__CC_ARM))
__weak void DelayUs(uint32_t us)
#elif (defined(__ICCARM__))
#pragma weak DelayUs
void DelayUs(uint32_t us)
#endif
{
    SYSTICK_DelayUs(us);
}


#if (defined(__CC_ARM))
__weak void DelayInit(void)
#elif (defined(__ICCARM__))
#pragma weak DelayInit
void DelayInit(void)
#endif
{
    SYSTICK_DelayInit();
}





