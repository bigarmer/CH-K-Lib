#include "gpio.h"
#include "common.h"
#include "uart.h"
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
 
/*
     ʵ�����ƣ�GPIOС��ʵ��
     ʵ��ƽ̨������K60���İ�
     ����оƬ��MK60DN512ZVQ10
     ˵    ������ʵ�����򼰹̼����ɳ��˵����ṩ
     ������£�www.beyondcore.net
     ʵ��Ч��������PTA�˿ڵ�17���������ģʽ
               �ڵ͵�ƽʱС�������ڸߵ�ƽʱС��Ϩ��
               С����������˸����˸ʱ����500ms     
*/
 
int main(void)
{
    /* ��ʼ��Delay */
    DelayInit();
    /* ʹ�ü��׳�ʼ����ʼ��PTA�˿ڵ�17������ΪIOʹ�� ����Ϊ�������ģʽ */
    GPIO_QuickInit(HW_GPIOA, 17, kGPIO_Mode_OPP);
    /* ����һ�ֳ�ʼ����ʽ: �ṹ��ģʽ�ĳ�ʼ�� �е�����STM32�̼���*/
//    GPIO_InitTypeDef GPIO_InitStruct1;
//    GPIO_InitStruct1.instance = HW_GPIOA;
//    GPIO_InitStruct1.mode = kGPIO_Mode_OPP;
//    GPIO_InitStruct1.pinx = 17;
//    GPIO_Init(&GPIO_InitStruct1);
    
    /* ����PTA�˿ڵ�17��������͵�ƽ */
    GPIO_WriteBit(HW_GPIOA, 17, 0);
    
    while(1)
    {
//        /* ��ת���ŵ�ƽ ԭ���ǵͱ�ɸ� ԭ���Ǹ߱�ɵ� */
//        GPIO_ToggleBit(HW_GPIOA, 17);
        /* ����һ�ַ��� ʹ��λ������ */
        PAout(17) = !PAout(17);
        DelayMs(500);
    }
}


