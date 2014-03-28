#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "wdog.h"

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU); /* KEY */
    
    /* ��ʼ��һ��ģ���һ��ģʽ: ��ʼ��ģ�鱾��->����оƬ�ֲ� ��ʼ����Ӧ�ĸ�������->ʹ��ģ�� */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.baudrate = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_Init(&UART_InitStruct1);
    
    /* ��ʼ������0��Ӧ������ D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    /* ��ʼ�����Ź� */
    WDOG_InitTypeDef WDOG_InitStruct1 = {0};
    WDOG_InitStruct1.mode = kWDOG_Mode_Normal;
    WDOG_InitStruct1.timeOutInMs = 1000; /* ʱ�� 2000MS : 2000MS ��û��ι����λ */
    WDOG_Init(&WDOG_InitStruct1);
    
    printf("WDOG test start! press KEY1 to feed dog\r\n");
    
    /* ����LED Ȼ��Ϩ��  ָʾϵͳ���д����ϵ����� */
    GPIO_WriteBit(HW_GPIOE, 6, 0);
    DelayMs(200);
    GPIO_WriteBit(HW_GPIOE, 6, 1);
    while(1)
    {
        if(GPIO_ReadBit(HW_GPIOE, 26) == 0) /* ���������� */
        {
            /* ι�� ��ֹ��λ */
            WDOG_Refresh();
        }
        DelayMs(10);
    }
}


