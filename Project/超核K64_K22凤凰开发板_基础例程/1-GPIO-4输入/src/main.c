#include "gpio.h"
#include "common.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/* GPIO ����ģʽ */
/*
 *         @arg kGPIO_Mode_IFT :��������
 *         @arg kGPIO_Mode_IPD :��������
 *         @arg kGPIO_Mode_IPU :��������
 *         @arg kGPIO_Mode_OOD :��©��� ����������2������������� �ٶ�..
 *         @arg kGPIO_Mode_OPP :�������
 */
 
 /*
     ʵ�����ƣ�GPIO��������
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч�������ƿ������ϵ�3����ͨ����������������ʱ
        �����ź�Ϊ�͵�ƽ��������ѯ�ķ�ʽ��ȡ������״̬
        ��ʹ��С�Ƶ�����չʾ������Ч��
*/
int main(void)
{
    bool val[3];
    /* ��ʼ��Delay */
    DelayInit();
    /* ��GPIO����Ϊ����ģʽ оƬ�ڲ��Զ������������� */
    GPIO_QuickInit(HW_GPIOA, 4, kGPIO_Mode_IPU); //key1����
    GPIO_QuickInit(HW_GPIOA, 6, kGPIO_Mode_IPU); //key2����
    GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU); //key3����
   /* ����3��С�ƵĿ�������Ϊ��©��� */
    GPIO_QuickInit(HW_GPIOA,  9, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOA, 10, kGPIO_Mode_OPP); 
    GPIO_QuickInit(HW_GPIOA, 11, kGPIO_Mode_OPP); 
    while(1)
    {
        /* ���϶�ȡ ���ŵ�ƽ״̬ ����0 �� 1*/
        val[0] = GPIO_ReadBit(HW_GPIOA, 4);
        val[1] = GPIO_ReadBit(HW_GPIOA, 6);
        val[2] = GPIO_ReadBit(HW_GPIOA, 7);
        /* �����صĵ�ƽ���õ�����һ�������� */
        GPIO_WriteBit(HW_GPIOA, 9,  val[0]);
        GPIO_WriteBit(HW_GPIOA, 10, val[1]);
        GPIO_WriteBit(HW_GPIOA, 11, val[2]);
    }
}


