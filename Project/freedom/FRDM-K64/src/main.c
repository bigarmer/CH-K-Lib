#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "math.h"
 /*
     ʵ�����ƣ�UART��ӡ��Ϣ
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ʹ�ô���UART��оƬ�ĳ�����Ϣ��оƬ�ϵ���ͳ�ȥ
        ������Ϻ󣬽���while�У�ִ��С����˸Ч��
*/
 
int main(void)
{
    int cnt;
    int i;
    float num;
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    /* ��ӡоƬ��Ϣ */
    printf("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());

    while(1)
    { 
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(1);
    }
}


