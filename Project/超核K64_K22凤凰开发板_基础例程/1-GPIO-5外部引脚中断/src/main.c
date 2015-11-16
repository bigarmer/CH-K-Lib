#include "gpio.h"
#include "common.h"
#include "uart.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�GPIO�ⲿ�����ж�
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
		 ʵ��Ч��: ������KEY3��PTA7��ÿ��һ����һ��С�ƽ���˸һ��
          ͨ���������ƺ�ɫС�Ƶ����𡣶԰��������Ų����½��ش����жϵķ�ʽ��
*/

/* �ص������е� array ��32λÿһλ����һ�����ţ�1Ϊ �ý��жϱ����� 0����ý��ж�δ���� */
static void GPIO_ISR(uint32_t array)
{
    GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, false);
	
    if(array & (1 << 7)) /*��Ӧ�İ����ж� ��ת��Ӧ��LED��ƽ */
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
    }
    GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP); /* LED */
	
    /*����PTA�˿ڵ�7����Ϊ��������ģʽ */
    GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU); /* KEY */
   
    /* ����GPIO�ⲿ�����жϻص����� */
    GPIO_CallbackInstall(HW_GPIOA, GPIO_ISR);
    /* ��PTA7���ŵ��ж� �����ش��� */
    GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
    
    while(1)
    {
        DelayMs(500);
    }
}


