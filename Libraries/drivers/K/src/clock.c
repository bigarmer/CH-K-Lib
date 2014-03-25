/**
  ******************************************************************************
  * @file    clock.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
	* @note    ���ļ�Ϊ�ڲ��ļ����������úͻ�ȡоƬʱ��Ƶ�ʣ��û�������ú��޸�  
  ******************************************************************************
  */
#include "clock.h"
#include "common.h"

#define MCGOUT_TO_CORE_DIVIDER           (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT) + 1)
#define MCGOUT_TO_SYSTEM_DIVIDER         (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT) + 1)
#define MCGOUT_TO_BUS_DIVIDER            (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT) + 1)
#define MCGOUT_TO_PERIPHERAL_DIVIDER     (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT) + 1)
#define MCGOUT_TO_FLEXBUS_DIVIDER        (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV3_MASK)>>SIM_CLKDIV1_OUTDIV3_SHIFT) + 1)
#define MCGOUT_TO_FLASH_DIVIDER          (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK)>>SIM_CLKDIV1_OUTDIV4_SHIFT) + 1)

#define MCGOUT_TO_CORE_DIV_SET(x)        SIM_CLKDIV1_OUTDIV1(x)
#define MCGOUT_TO_SYSTEM_DIV_SET(x)      SIM_CLKDIV1_OUTDIV1(x)
#define MCGOUT_TO_BUS_DIV_SET(x)         SIM_CLKDIV1_OUTDIV2(x)
#define MCGOUT_TO_PERIPHERAL_DIV_SET(x)  SIM_CLKDIV1_OUTDIV2(x)
//#define MCGOUT_TO_FLEXBUS_DIV_SET(x)     SIM_CLKDIV1_OUTDIV3(x)
#define MCGOUT_TO_FLASH_DIV_SET(x)       SIM_CLKDIV1_OUTDIV4(x)


 /**
 * @brief  ���ø���ʱ�ӵķ�Ƶϵ��
 * @code
 *      // �����ں�ʱ�ӵ����Ƶ��Ϊ1/3��Ƶ
 *      CLOCK_SetClockDivider(kMcgOut2CoreDivider, kClockSimDiv3);
 * @endcode
 * @param  clockDivName: ʱ������
 *         @arg kMcgOut2CoreDivider   :�ں�ʱ�ӷ�Ƶ
 *         @arg kMcgOut2SystemDivider :ϵͳʱ�ӷ�Ƶ
 *         @arg kMcgOut2BusDivider    :����ʱ�ӷ�Ƶ
 *         @arg kMcgOut2FlashDivider  :flashʱ�ӷ�Ƶ
 * @param  dividerValue: ��Ƶϵ��
 *         @arg kClockSimDiv2  :2��Ƶ
 *         @arg kClockSimDiv3  :3��Ƶ
 *         @arg             .  : .      
 *         @arg             .  : .  
 *         @arg             .  : .   
 *         @arg kClockSimDiv16 :16��Ƶ
 * @retval None
 */
void CLOCK_SetClockDivider(CLOCK_DividerSource_Type clockDivName, CLOCK_DivideValue_Type dividerValue)
{
    switch (clockDivName)
		{
			case kMcgOut2CoreDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_CORE_DIV_SET(dividerValue);
				break;
			case kMcgOut2SystemDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_SYSTEM_DIV_SET(dividerValue);
				break;
			case kMcgOut2BusDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_BUS_DIV_SET(dividerValue);
				break;
			case kMcgOut2FlashDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_FLASH_DIV_SET(dividerValue);
				break;
			default :
				break;
		}
}

 /**
 * @brief  ���ϵͳ��������ʱ�ӵ�Ƶ��
 * @code
 *         //�������ʱ��Ƶ��
 *         uint32_t BusClock;
 *         CLOCK_GetClockFrequency(kBusClock, &BusClock);
 *         //������ʱ��Ƶ����ʾ����
 *         printf("BusClock:%dHz\r\n", BusClock);
 * @endcode
 * @param  clockName:ʱ������
 *         @arg kCoreClock    :�ں�ʱ��
 *         @arg kSystemClock  :ϵͳʱ�� = �ں�ʱ��
 *         @arg kBusClock     :����ʱ��
 *         @arg kFlexBusClock :Flexbus����ʱ��
 *         @arg kFlashClock   :Flash����ʱ��
 * @param  FrequenctInHz: ���Ƶ�����ݵ�ָ�� ��λHz
 * @retval 0: �ɹ� ��0: ����
 */
int32_t CLOCK_GetClockFrequency(CLOCK_Source_Type clockName, uint32_t* FrequenctInHz)
{
    uint32_t MCGOutClock = 0;
    /* calualte MCGOutClock system_MKxxx.c must not modified*/
    MCGOutClock = SystemCoreClock * MCGOUT_TO_CORE_DIVIDER;
    switch (clockName)
    {
        case kCoreClock:
            *FrequenctInHz = MCGOutClock / MCGOUT_TO_CORE_DIVIDER;
            break;
        case kSystemClock:
            *FrequenctInHz = MCGOutClock / MCGOUT_TO_CORE_DIVIDER;
            break;	
        case kBusClock:
            *FrequenctInHz = MCGOutClock / MCGOUT_TO_BUS_DIVIDER;
            break;
        case kFlashClock:
            *FrequenctInHz = MCGOutClock / MCGOUT_TO_FLASH_DIVIDER;	
            break;
        default:
            return 1;
    }
    return 0;
}



