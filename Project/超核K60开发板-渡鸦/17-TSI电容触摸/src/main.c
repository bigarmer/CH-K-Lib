#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "tsi.h"

/* TSI �жϻص����� */
void TSI_ISR(uint32_t outOfRangeArray)
{
    static uint32_t last_chl_array;
    uint32_t value;
    value = outOfRangeArray^last_chl_array;
    /* ������Ӧ��С�� */
    if(value & outOfRangeArray & (1<<HW_TSI_CH1))
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
    if(value & outOfRangeArray & (1<<HW_TSI_CH2))
    {
        GPIO_ToggleBit(HW_GPIOE, 12);
    }
    if(value & outOfRangeArray & (1<<HW_TSI_CH3))
    {
        GPIO_ToggleBit(HW_GPIOE, 11);
    }
    if(value & outOfRangeArray & (1<<HW_TSI_CH4))
    {
        GPIO_ToggleBit(HW_GPIOE, 7);
    }
    last_chl_array = outOfRangeArray;
}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 7, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 11, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /* Ϊ�˴���� ����ֱ�ӵ��ÿ��ٳ�ʼ���� */
    
    printf("TSI test\r\n");
    
    /* ʹ��4��TSI �������� ע�� TSI�ϵ���ҪУ׼ ���غ� ����ε� JLINK Ȼ�� ��λ */
    TSI_QuickInit(TSI0_CH1_PA00);
    TSI_QuickInit(TSI0_CH2_PA01);
    TSI_QuickInit(TSI0_CH3_PA02);
    TSI_QuickInit(TSI0_CH4_PA03);
    /* ��װ TSI�жϻص����� */
    TSI_CallbackInstall(TSI_ISR);
    /* ���� TSI ɨ������ж� */
    TSI_ITDMAConfig(kTSI_IT_EndOfScan);
    
    while(1)
    {
        /* �ȴ��ж� */
    }
}


