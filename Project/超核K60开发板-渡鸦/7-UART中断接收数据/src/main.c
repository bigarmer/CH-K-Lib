#include "gpio.h"
#include "common.h"
#include "uart.h"


/* ���ڽ����жϻص����� */
static void UART_RX_ISR(uint8_t byteReceived)
{
    /* �����յ������ݷ��ͻ�ȥ */
    UART_WriteByte(HW_UART0, byteReceived);
}

 
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    /* ��ʼ��һ��ģ���һ��ģʽ: ��ʼ��ģ�鱾��->����оƬ�ֲ� ��ʼ����Ӧ�ĸ�������->ʹ��ģ�� */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.baudrate = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_Init(&UART_InitStruct1);
    
    /* ��ʼ������0��Ӧ������ D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    printf("type any character which will echo...\r\n");
    
    /*  ����UART �ж����� �򿪽����ж� ��װ�жϻص����� */
    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
    /* �򿪴��ڽ����жϹ��� IT �����жϵ���˼*/
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


