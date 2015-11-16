#include "gpio.h"
#include "common.h"
#include "uart.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�SMC�͹���STOP
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч�������ƿ������ϵ�3����ͨ����������������ʱ
        �����ź�Ϊ�͵�ƽ��������ѯ�ķ�ʽ��ȡ������״̬
        ��ʹ��С�Ƶ�����չʾ������Ч��
*/

/* �ص������е� array ��32λÿһλ����һ�����ţ�1Ϊ �ý��жϱ����� 0����ý��ж�δ���� */
static void GPIO_ISR(uint32_t array)
{
    printf("enter interrupt, back to RUN now!\r\n");
}

int main(void)
{
    DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    GPIO_QuickInit(HW_GPIOA, 6, kGPIO_Mode_IPU);
    GPIO_CallbackInstall(HW_GPIOA, GPIO_ISR);
    GPIO_ITDMAConfig(HW_GPIOA, 6, kGPIO_IT_FallingEdge, true);
    
    printf("low power STOP mode\r\n");
    
    while(1)
    {
        EnterSTOPMode(false);
        printf("Wake Up!\r\n");
    }
}


