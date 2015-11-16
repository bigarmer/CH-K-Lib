#include "gpio.h"
#include "common.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�GPIOλ������
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч����ͨ��ʹ��λ���������ƿ������ϵĲ�ɫС�� 
*/
//���ȶ���λ�����ĺ궨��
#define LED_R  PAout(9)  //����PTA�˿ڵ�9�����������
#define LED_G  PAout(10) //����PTA�˿ڵ�10�����������
#define LED_B  PAout(11) //����PTA�˿ڵ�11�����������


int main(void)
{
    /* ��ʼ�� Delay */
    DelayInit();

    /* ����3��С�ƵĿ�������Ϊ��©��� */
    GPIO_QuickInit(HW_GPIOA,  9, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOA, 10, kGPIO_Mode_OPP); 
    GPIO_QuickInit(HW_GPIOA, 11, kGPIO_Mode_OPP); 
    while(1)
    {
        /* ��ˮ�� */
        DelayMs(100);
        LED_R = !LED_R; //�ı����״̬��ԭ��Ϊ�ߵ�ƽ��������͵�ƽ�������෴��
        DelayMs(100);
        LED_G = !LED_G;
        DelayMs(100);
        LED_B = !LED_B;
    }
}


