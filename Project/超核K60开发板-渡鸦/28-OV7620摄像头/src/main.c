#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pit.h"

static void PIT_ISR(void)
{
    static uint32_t i;
    printf("enter PIT interrupt! %d\r\n", i++);
    /* ��˸С�� */
    GPIO_ToggleBit(HW_GPIOE, 6);
}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    
    /* ��ʼ��һ��ģ���һ��ģʽ: ��ʼ��ģ�鱾��->����оƬ�ֲ� ��ʼ����Ӧ�ĸ�������->ʹ��ģ�� */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.instance = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_Init(&UART_InitStruct1);
    
    /* ��ʼ������0��Ӧ������ D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    /* ��ʼ��PITģ�� */
    PIT_InitTypeDef PIT_InitStruct1;
    PIT_InitStruct1.chl = HW_PIT_CH0; /* ʹ��0�Ŷ�ʱ�� */
    PIT_InitStruct1.timeInUs = 1000*1000; /* ��ʱ����1S */
    PIT_Init(&PIT_InitStruct1);
    /* ע��PIT �жϻص����� */
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
    /* ����PIT��ʱ���ж� */
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF);
    
    while(1)
    {
        /* �ȴ�ϵͳ�ж� */
    }
}


