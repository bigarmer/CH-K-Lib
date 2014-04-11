#include "gpio.h"
#include "common.h"
#include "uart.h"

/*
     ʵ�����ƣ�GPIO�ⲿ�����ж�
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����һ��LED�ƹ������˸��������KEY2ÿ��һ����һ��С�ƽ���˸һ��
          ͨ����������С�Ƶ����𡣶԰��������Ų��������ش����жϵķ�ʽ��
*/

/* �ص������е� array ��32λÿһλ����һ�����ţ�1Ϊ �ý��жϱ����� 0����ý��ж�δ���� */
static void GPIO_ISR(uint32_t array)
{
    printf("KEY:0x%X\r\n", array);
    if(array & (1<<26)) /*��Ӧ�İ����ж� ��ת��Ӧ��LED��ƽ */
    {
        GPIO_ToggleBit(HW_GPIOE, 12);
    }
}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP); /* LED */
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU); /* KEY */
    
    /* ��ʼ��һ��ģ���һ��ģʽ: ��ʼ��ģ�鱾��->����оƬ�ֲ� ��ʼ����Ӧ�ĸ�������->ʹ��ģ�� */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.instance = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_InitStruct1.parityMode = kUART_ParityDisabled;
    UART_InitStruct1.bitPerChar = kUART_8BitsPerChar;
    UART_Init(&UART_InitStruct1);
    
    /* ��ʼ������0��Ӧ������ D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    /* ����GPIO�ⲿ�����жϻص����� */
    GPIO_CallbackInstall(HW_GPIOE, GPIO_ISR);
    /* ��GPIO�����ж� �����ش��� */
    GPIO_ITDMAConfig(HW_GPIOE, 26, kGPIO_IT_RisingEdge);
    
    printf("press any key1 to let LED toggle\r\n");
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


