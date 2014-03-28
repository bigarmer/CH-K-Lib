#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

 
int main(void)
{
    uint8_t ch;
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
    
    /* ��ӡоƬ��Ϣ */
    printf(" type any character whitch will echo...\r\n");
    
    while(1)
    {
        /* ��ͣ�Ĳ�ѯ ���ڽ��յ�״̬ һ�����ճɹ� ����0 ���ͻؽ��յ������� ʵ�ֻػ�����*/
        if(UART_ReadByte(HW_UART0, &ch) == 0)
        {
            UART_WriteByte(HW_UART0, ch);
        }
    }
}


