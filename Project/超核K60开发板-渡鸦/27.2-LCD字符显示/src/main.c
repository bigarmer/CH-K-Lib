#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ili9320.h"

#include "gui.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�LCD�ַ���ʾ
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ʹ��ucgui���棬��ɫ���棬��ʾ����
*/
int main(void)
{
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("flexbus lcd test\r\n");
    GUI_Init(); //Һ�������ʼ��
    GUI_DispString("ucGUI"); //��ʾ�ַ���
    GUI_DispString(GUI_GetVersionString());//��ó���汾
    GUI_DispString("\r\nHello world!");
    while(1) 
    {
        GUI_DispDecAt( i++, 20,20,4);
        if (i>9999)
        {
            i=0;
        }
    }
}


