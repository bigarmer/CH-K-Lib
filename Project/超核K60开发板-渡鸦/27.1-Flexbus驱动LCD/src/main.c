#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "flexbus.h"

#include "sram.h"
#include "ili9320.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�Flexbus����LCD
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч������ʼ��Һ���������ϵ�ˢ����Ļ���任��ɫ
*/
static uint16_t ColorTable[] = 
{
    RED,      //��ɫ
    BLUE,     //��ɫ
    YELLOW,   //��ɫ
    GREEN,    //��ɫ
    CYAN,
    LGRAY,
    GRED,
};

int main(void)
{
    uint8_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("flexbus lcd test\r\n");
    ili9320_init();   //Һ������ʼ��
    printf("controller id:0x%X\r\n", ili9320_get_id()); //��ȡҺ����������IC��Ϣ
    
    while(1)
    {
        i++; i%= ARRAY_SIZE(ColorTable);
        ili9320_clear(ColorTable[i]);  //��Ļȫɫ��ʾ
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


