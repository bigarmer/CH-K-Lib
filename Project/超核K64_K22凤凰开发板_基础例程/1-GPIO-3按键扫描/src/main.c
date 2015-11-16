#include "gpio.h"
#include "common.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�GPIO����ɨ��
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч����ʹ�ð���KEY3����LED_RС�Ƶ�״̬���������»�ı�С�Ƶ�����
*/
//���ȶ���λ�����ĺ궨��
#define KEY3  PAin(7)  //����PTA�˿ڵ�7����Ϊ����
#define LED_R  PAout(9)  //����PTA�˿ڵ�9�����������

/* ��������״̬ 0δ�� 1���� */
#define NO_KEY          (0x00)
#define KEY_SINGLE      (0x01)
/* ��������ֵ */
static uint8_t gRetValue;

/* ״̬���� */
typedef enum
{
    kKEY_Idle,          /*����̬ */
    kKEY_Debounce,      /*ȷ��������̬ */
    kKEY_Confirm,       /*ȷ�ϰ���״̬*/
}KEY_Status;

/* ����ɨ����� */
static void KEY_Scan(void)
{
    static KEY_Status status = kKEY_Idle;
    switch(status)
    {
        case kKEY_Idle:
            gRetValue = NO_KEY;
            if(KEY3 == 0) /* ������������� ����ȷ��������̬ */
            {
                status = kKEY_Debounce;
            }
            break;
        case kKEY_Debounce: /* ȷ��������̬ */
            if(KEY3 == 0)
            {
                status = kKEY_Confirm;
            }
            else
            {
                status = kKEY_Idle;
                gRetValue = NO_KEY;
            }
            break;
        case kKEY_Confirm: /* ȷ��̬ */
            if(KEY3 == 1) /* �����ɿ� */
            {
                gRetValue = KEY_SINGLE;
                status = kKEY_Idle;
            }
            break;
        default:
            break;
    }
}

int main(void)
{
    /* ��ʼ�� Delay */
    DelayInit();
    /* ��GPIO����Ϊ����ģʽ оƬ�ڲ��Զ������������� */
    GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU);
    /* ����Ϊ��� */
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    while(1)
    {
        KEY_Scan(); //���ð���ɨ�����
        DelayMs(10);
        if(gRetValue == KEY_SINGLE) //�������£�С�Ʒ���
        {
            LED_R = !LED_R;
        }
    }
}


