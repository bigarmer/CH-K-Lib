/**
  ******************************************************************************
  * @file    common.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
	* @note    ���ļ�Ϊ�ڲ��ļ����û�������ú��޸�  
  ******************************************************************************
  */
#include "common.h"
#include <string.h>
#include "systick.h"

 /**
 * @brief  ������ٳ�ʼ���ṹ �û��������
 *
 * @param  type: ���ٳ�ʼ���ṹ��ָ��
 * @retval       32λ���ٳ�ʼ������
 */
uint32_t QuickInitEncode(QuickInit_Type * type)
{
    return *(uint32_t*)type;
}

 /**
 * @brief  ������ٳ�ʼ���ṹ �û��������
 *
 * @param  map: 32λ���ٳ�ʼ������
 * @param  type: ���ٳ�ʼ���ṹָ��
 * @retval None
 */
void QuickInitDecode(uint32_t map, QuickInit_Type * type)
{
    QuickInit_Type * pMap = (QuickInit_Type*)&(map);
    memcpy(type, pMap, sizeof(QuickInit_Type));  
}

/**
 * @brief  ��ʱ��ʼ������
 * @code
 *      // �����ʱ��ʼ�����ã�
 *      //ʹ���ں˵�SYSticģ��ʵ����ʱ����
 *        DelayInit();
 * @endcode
 * @param  None
 * @retval None
 */
 #if (defined(__CC_ARM))
__weak void DelayInit(void)
#elif (defined(__ICCARM__))
#pragma weak DelayInit
void DelayInit(void)
#endif
{
    SYSTICK_DelayInit();
}

/**
 * @brief ���뼶����ʱ���ú���
 * @code
 *      // ʵ��500ms����ʱ����
 *        DelayMs(500);
 * @endcode
 * @param  ms :��Ҫ��ʱ��ʱ�䣬��λ����
 * @retval None
 * @note  ������Ҫ�����ʱ��ʼ������
 */
#if (defined(__CC_ARM))
__weak void DelayMs(uint32_t ms)
#elif (defined(__ICCARM__))
#pragma weak DelayMs
void DelayMs(uint32_t ms)
#endif
{
    SYSTICK_DelayMs(ms);
}

/**
 * @brief ΢�뼶����ʱ���ú���
 * @code
 *      // ʵ��500us����ʱ����
 *        DelayUs(500);
 * @endcode
 * @param  us :��Ҫ��ʱ��ʱ�䣬��λ΢��
 * @retval None
 * @note  ������Ҫ�����ʱ��ʼ������
 */
#if (defined(__CC_ARM))
__weak void DelayUs(uint32_t us)
#elif (defined(__ICCARM__))
#pragma weak DelayUs
void DelayUs(uint32_t us)
#endif
{
    SYSTICK_DelayUs(us);
}



#if defined(DEBUG)

void assert_failed(char * file, uint32_t line)
{
	//����ʧ�ܼ��
	while(1);
}
#endif

#if DEBUG

void NMI_Handler(void)
{
    printf("NMI INTERRUPT\r\n");
}

void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");
    while(1);
}

void BusFault_Handler(void)
{
    printf("BusFault_Handler\r\n");
    while(1);
}

#endif




