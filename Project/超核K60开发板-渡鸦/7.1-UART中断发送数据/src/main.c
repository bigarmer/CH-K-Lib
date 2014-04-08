#include "gpio.h"
#include "common.h"
#include "uart.h"


static const char UART_String1[] = "HelloWorld\r\n";

static void UART_TX_ISR(uint8_t * byteToSend)
{
    static uint32_t cnt = sizeof(UART_String1);
    static const char *p = UART_String1;
    *byteToSend = *p++;
    cnt--;
    if(!cnt)
    {
        p = UART_String1;
        cnt = sizeof(UART_String1);
        UART_ITDMAConfig(HW_UART0, kUART_IT_Tx_Disable);
    }
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    /* ��ʼ��һ��ģ���һ��ģʽ: ��ʼ��ģ�鱾��->����оƬ�ֲ� ��ʼ����Ӧ�ĸ�������->ʹ��ģ�� */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.instance = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_InitStruct1.bitPerChar = kUART_8BitsPerChar;
    UART_InitStruct1.parityMode = kUART_ParityDisabled;
    UART_Init(&UART_InitStruct1);
    
    /* ��ʼ������0��Ӧ������ D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    printf("uart will be send on interrupt mode...\r\n");
    /* ע�ᷢ���жϻص����� */
    UART_CallbackTxInstall(HW_UART0, UART_TX_ISR);
    /* �򿪷�������ж� */
    UART_ITDMAConfig(HW_UART0, kUART_IT_Tx);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


