#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"
#include "lptmr.h"
#include "pit.h"

/* LPTMR ��Ϊ�������ʱ ֻ�ܲ���һ·���� */

static void PIT_ISR(void)
{
    uint32_t value;
    value = LPTMR_PC_ReadCounter();
    LPTMR_ClearCounter();
    printf("LPTMR:%dHz\r\n", value);
}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /* Ϊ�˴���� ����ֱ�ӵ��ÿ��ٳ�ʼ���� */
    
    printf("lptrm pulse counter test connect PC05&PC01\r\n");
    
    /* ��PC01 �ϲ���һ��Ƶ�ʵķ��� ��������������� */
    FTM_PWM_QuickInit(FTM0_CH0_PC01, 1000);
    /* ����FTM ��ռ�ձ� */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 5000); // 50%ռ�ձ� 0-10000 ��Ӧ 0-100%
    
    /* ���ٳ�ʼ�� LPTMR �������岶׽ PC = pulse counter ������� */
    LPTMR_PC_QuickInit(LPTMR_ALT2_PC05); /* PC = pulse counter ������� */
    
    /* ����һ��PIT��ʱ�� ����1S�ж� ���ж��ж�ȡLPTMRֵ */
    PIT_QuickInit(HW_PIT_CH0, 1000 * 1000);
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


