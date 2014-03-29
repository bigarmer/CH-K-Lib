#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"
#include "lptmr.h"

/* LPTMR ��Ϊ�������ʱ ֻ�ܲ���һ·���� */

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /* Ϊ�˴���� ����ֱ�ӵ��ÿ��ٳ�ʼ���� */
    
    printf("lptrm pulse counter test\r\n");
    
    /* ��PC01 �ϲ���һ��Ƶ�ʵķ��� ��������������� */
    FTM_PWM_QuickInit(FTM0_CH0_PC01, 1000);
    /* ����FTM ��ռ�ձ� */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 5000); // 50%ռ�ձ� 0-10000 ��Ӧ 0-100%
    
    /* ���ٳ�ʼ�� LPTMR �������岶׽ */
    LPTMR_PC_QuickInit(LPTMR_ALT2_PC05); /* PC = pulse counter ������� */

    uint32_t value;
    while(1)
    {
        value = LPTMR_PC_ReadCounter();
        LPTMR_ClearCounter();
        printf("LPTMR:%dHz\r\n", value);
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(1000);
    }
}


