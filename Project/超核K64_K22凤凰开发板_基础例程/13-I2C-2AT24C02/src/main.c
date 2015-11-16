#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "at24cxx.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�I2C�豸AT24C02
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64FN1MVLQ12
 ʵ��Ч����ʹ��I2Cͨ���뿪�����ϵ�AT24C02�洢оƬͨ�ţ����в��ԣ��ɹ��󴮿ڷ�������
*/
int main(void)
{
    uint32_t ret;
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("at24c02 test\r\n");
    
    I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    

    ret = eep_init(0);
    if(ret)
    {
        printf("at24cxx not found\r\n");
        while(1);
    }

    if(eep_self_test(0, 256))
    {
        printf("at24cxx self test failed\r\n");
        return 1;  
    }
    else
    {
        printf("at24cxx test ok!\r\n");
    }
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


