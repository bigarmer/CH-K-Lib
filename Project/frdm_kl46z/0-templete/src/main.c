#include "gpio.h"
#include "common.h"


 
 void UART_ISR(uint16_t byteReceived)
 {
   //  printf("%c", byteReceived);
 }
 
int main(void)
{
    uint32_t instance;
    uint16_t ch;
    DelayInit();
    /* ��ʼ��һ������ ����Ϊ������� */
    GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 29, kGPIO_Mode_OPP);
    /* ��ʼ��һ������ ʹ��UART0�˿ڵ�PTD6���ź�PTD7������Ϊ���պͷ��ͣ�Ĭ������ baud 115200 */
//    instance = UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
   // UART_CallbackRxInstall(instance, UART_ISR);
   // UART_ITDMAConfig(instance, kUART_IT_Rx, true);
   // printf("HelloWorld!\r\n", __func__);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOD, 5);
        GPIO_ToggleBit(HW_GPIOE, 29);
        DelayMs(500);
    }
}


