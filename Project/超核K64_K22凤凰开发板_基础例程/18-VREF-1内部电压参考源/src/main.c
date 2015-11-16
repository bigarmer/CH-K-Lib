#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "vref.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�VREF�ڲ���ѹ�ο�Դ����
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч����ѡ��оƬ�ڲ��Դ��ĵ�ѹ�ο�Դ��ΪADģ��Ĳο�Դ������ģ��ת��
*/
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("VREF Test please connect ADC1_SE16 & VERF_OUT\r\n");
    /* ��ʼ�� VREF ���1.2V��׼��ѹ */
    VREF_QuickInit();
    ADC_QuickInit(ADC1_SE16, kADC_SingleDiff12or13);
    uint32_t val;
    while(1)
    {
        val = ADC_QuickReadValue(ADC1_SE16);
        printf("ADC:%d | %0.3fV  \r", val, (double)val*3.300/(1<<12));
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


