#include "gpio.h"
#include "common.h"


#define LED1  PEout(6)
#define LED2  PEout(7)
#define LED3  PEout(11)
#define LED4  PEout(12)

int main(void)
{
    /* ��ʼ�� Delay */
    DelayInit();

    /* ����Ϊ��� */
    GPIO_QuickInit(HW_GPIOE,  6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE,  7, kGPIO_Mode_OPP); 
    GPIO_QuickInit(HW_GPIOE, 11, kGPIO_Mode_OPP); 
    GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP); 
    while(1)
    {
        /* ��ˮ�� */
        DelayMs(100);
        LED1 = !LED1;
        DelayMs(100);
        LED2 = !LED2;
        DelayMs(100);
        LED3 = !LED3;
        DelayMs(100);
        LED4 = !LED4;
    }
}


