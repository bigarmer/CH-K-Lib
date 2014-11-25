#include "gpio.h"
#include "common.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/* GPIO ����ģʽ */
/*
 *         @arg kGPIO_Mode_IFT :��������
 *         @arg kGPIO_Mode_IPD :��������
 *         @arg kGPIO_Mode_IPU :��������
 *         @arg kGPIO_Mode_OOD :��©��� ����������2������������� �ٶ�..
 *         @arg kGPIO_Mode_OPP :�������
 */

 
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOD, 1, kGPIO_Mode_OPP);
    GPIO_WriteBit(HW_GPIOD, 1, 0);
    
    while(1)
    {
        PDout(1) = !PDout(1);
        DelayMs(500);
    }
}


