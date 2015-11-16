#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "rtc.h"

/*
     ʵ������: RTC����
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч����ͨ�����ڷ���ʱ��ʱ�䣬3�����������жϣ�ͨ�����ڷ��ͳ�ȥ��
           LED��ÿ��1s��˸һ�� 
*/

void RTC_ISR(void)
{
    printf("RTC INT\r\n"); 
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    /* ����һ��ʱ��ṹ */
    RTC_DateTime_Type td = {0};
    td.day = 20;
    td.hour = 23;
    td.minute = 59;
    td.second = 50;
    td.year = 2014;
    td.month = 11;
    printf("RTC alarm test\r\n");
    
    RTC_QuickInit();
    if(RTC_IsTimeValid() == false)
    {
        printf("bad time, reset!\r\n");
        RTC_SetTime(&td);
    }
    /* �����ж� */
    RTC_CallbackInstall(RTC_ISR);
    RTC_ITDMAConfig(kRTC_IT_TimeAlarm, true);
    
    /* ���������ڵ�ǰ3��� */
    RTC_GetTime(&td);
    td.second += 3;
    RTC_SetAlarm(&td);

    while(1)
    {
        /* ���ʱ�� */
        RTC_GetTime(&td);//���ʱ��
        printf("%d-%d-%d %d:%d:%d\r\n", td.year, td.month, td.day, td.hour, td.minute, td.second);
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(1000);
    }
}


