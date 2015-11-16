#include "gpio.h"
#include "common.h"
/*
     ʵ�����ƣ�DWT��ʱʵ��
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч����ʱ������500���� 
*/
 
int main(void)
{
    /* ��ʼ��Delay */
    DelayInit();
    DWT_DelayInit();

    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);

    GPIO_WriteBit(HW_GPIOA, 9, 0);
    
    while(1)
    {
        PAout(9) = !PAout(9);
        DWT_DelayMs(500);
    }
}


