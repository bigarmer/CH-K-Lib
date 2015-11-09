#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "flexbus.h"

#include "IS61WV25616.h" 
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�Flexbus����SRAM
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64FN1MVLQ12
 ʵ��Ч����������ҵ�SRAM�������  
       �����SRAMӦ����μ�sram.c�ļ�
*/
int main(void)
{
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("Flexbus SRAM test\r\n");
    
    /* ��ʼ���ⲿSRAM */
    SRAM_Init();
    /* SRAM �Բ� */
    if(!SRAM_SelfTest())
    {
        printf("sram test ok!\r\n");
    }
    else
    {
        printf("sram test failed!\r\n");
    }
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


