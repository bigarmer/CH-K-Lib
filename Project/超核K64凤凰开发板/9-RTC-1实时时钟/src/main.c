#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "rtc.h"

/*
     ʵ������: RTCʵʱʱ��
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч����ͨ�����ڷ���ʱ��ʱ��   
*/
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    /* ����һ��ʱ��ṹ */
    RTC_DateTime_Type td = {0};
    td.day = 6;
    td.hour = 23;
    td.minute = 59;
    td.second = 50;
    td.year = 2015;
    td.month = 11;
    printf("RTC test\r\n");
    RTC_QuickInit();
    /* ��ʱ����Ч(����δִ�й�RTCʱ) ��ʼ��ʱ�� */
    if(RTC_IsTimeValid() == false)
    {
        printf("time invalid, reset time!\r\n");
        RTC_SetTime(&td);
    }
    while(1)
    {
        /* ���ʱ�� */
        RTC_GetTime(&td);//���ʱ��
        printf("%d-%d-%d %d:%d:%d\r\n", td.year, td.month, td.day, td.hour, td.minute, td.second);
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(1000);
    }
}


