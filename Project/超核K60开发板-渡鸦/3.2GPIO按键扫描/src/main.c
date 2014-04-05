#include "gpio.h"
#include "common.h"


#define KEY1  PEin(26)
#define LED1  PEout(6)

/* ��������״̬ 0 Ϊ���� 1���� */
#define NO_KEY          (0x00)
#define KEY_SINGLE      (0x01)
/* ��������ֵ */
static uint8_t gRetValue;

/* ״̬���� */
typedef enum
{
    kKEY_Idle,          /* ����̬ */
    kKEY_Debounce,      /* ȷ��������̬ */
    kKEY_Confirm,       /* ȷ��̬ */
}KEY_Status;

/* ����ɨ����� */
static void KEY_Scan(void)
{
    static KEY_Status status = kKEY_Idle;
    switch(status)
    {
        case kKEY_Idle:
            gRetValue = NO_KEY;
            if(KEY1 == 0) /* ������������� ����ȷ��������̬ */
            {
                status = kKEY_Debounce;
            }
            break;
        case kKEY_Debounce: /* ȷ��������̬ */
            if(KEY1 == 0)
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
            if(KEY1 == 1) /* �����ɿ� */
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
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    /* ����Ϊ��� */
    GPIO_QuickInit(HW_GPIOE,  6, kGPIO_Mode_OPP);
    while(1)
    {
        KEY_Scan();
        DelayMs(10);
        if(gRetValue == KEY_SINGLE)
        {
            LED1 = !LED1;
        }
    }
}


