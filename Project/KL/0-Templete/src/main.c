
#include "kl_common.h"


 
int main(void)
{
   // DelayInit();
    /* ��ʼ��һ������ ����Ϊ������� */
  //  GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    /* ��ʼ��һ������ ʹ��UART0�˿ڵ�PTD6���ź�PTD7������Ϊ���պͷ��ͣ�Ĭ������ baud 115200 */
 //   UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("HelloWorld!\r\n");
    
    while(1)
    {
        /* ��˸С�� */
    //    GPIO_ToggleBit(HW_GPIOE, 6);
    //    DelayMs(500);
    }
}


