#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "wdog.h"

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
    
    /* ��ʼ�����Ź� */
    WDOG_InitTypeDef WDOG_InitStruct1 = {0};
    WDOG_InitStruct1.mode = kWDOG_Mode_Window;
    WDOG_InitStruct1.windowInMs = 600;   /* ����ʱ�� ����Ϊ����ģʽ�� ι�������� ���Ź���ʼ��ʱ�� 600-1000MS����� �������˶���λ ����ͨ���Ź��ϸ�*/
    WDOG_InitStruct1.timeOutInMs = 1000; /* ʱ�� 1000MS : 1000MS ��û��ι����λ */
    WDOG_Init(&WDOG_InitStruct1);
    
    printf("system reset! WDOG test start! \r\n");
    printf("press any character to disable dog feed\r\n");
    
    static uint32_t i;
    uint8_t ch;
    while(1)
    {
        if(UART_ReadByte(HW_UART0, &ch) == 0)
        {
            printf("stop feed wdog, will cause reset!\r\n");
            while(1);
        }
        printf("cnt:i:%d\r\n", i++);
        DelayMs(700); /* ι��ʱ������� 600-1000MS�� */
        GPIO_ToggleBit(HW_GPIOE, 6);
        WDOG_Refresh();
    }
}


