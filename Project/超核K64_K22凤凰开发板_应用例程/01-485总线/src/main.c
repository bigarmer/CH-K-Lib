#include "gpio.h"
#include "common.h"
#include "uart.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */
 
/*
     ʵ�����ƣ�485����
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64FN1MVLQ12
 ʵ��Ч���������봮�ڣ�ͨ���������ϵ�485ͨ�Žӿ�ͨ��  
        485ͨ�Ų���3.3v��MAX3485��SP3485��ʹ��PTA25���ſ�������״̬��ʹ��PTE24/25�����շ���
				���ڰ������Ÿ��ã���Ҫ����Ƭѡ�ź�����E26/E27�������ŵĵ�ƽΪ0.
*/
/* ����һ���ַ� */
static void UART_SendString(uint32_t instance, char * str)
{
    while(*str != '\0')
    {
        UART_WriteByte(instance, *str++);
    }
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);//LED��
	  GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_OPP);//485����ѡ��S0
	  GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_OPP);//485����ѡ��S1
	  GPIO_QuickInit(HW_GPIOA, 25, kGPIO_Mode_OPP);//485���Ϳ���
	
	  GPIO_WriteBit(HW_GPIOE, 26, 0);//485����ѡ��S0=0
	  GPIO_WriteBit(HW_GPIOE, 27, 0);//485����ѡ��S1=0
	  GPIO_WriteBit(HW_GPIOA, 25, 0);//����485���ڽ���״̬
    /* ���Դ��� */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    /* 485 �� ��ͨ��TTL ���� ֻ�Ǻ�Ӳ����ͬ ����MCU��˵�����Ǵ��� */
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 115200);
    printf("485 test! please connnect other 485 device\r\n");
    
    /* ��485���߷���һ���ַ� */
    UART_SendString(HW_UART4, "485 device test\r\n");
    uint16_t ch;
    while(1)
    {
        /* ��485���ߵ����ݷ���ȥ */
        if(UART_ReadByte(HW_UART4, &ch) == 0)
        {
            GPIO_WriteBit(HW_GPIOE, 26, 1); //����485оƬ���ڷ���״̬
					  UART_WriteByte(HW_UART4, ch);
					  GPIO_WriteBit(HW_GPIOE, 26, 0); //����485оƬ���ڽ���״̬
        }
        GPIO_ToggleBit(HW_GPIOA, 9);
    }
}


