#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dac.h"

int main(void)
{
    uint32_t i;
    uint16_t value[16];
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DAC test please measure DAC0_OUT\r\n");
    
    DAC_InitTypeDef DAC_InitStruct = {0};
    DAC_InitStruct.bufferMode = kDAC_Buffer_Swing; /*Bufferҡ��ģʽ �������ǲ� */
    DAC_InitStruct.instance = HW_DAC0;
    DAC_InitStruct.referenceMode = kDAC_Reference_2; /* ʹ��VDDA��Ϊ�ο�Դ */
    DAC_InitStruct.triggerMode = kDAC_TriggerSoftware; /*������� */
    DAC_Init(&DAC_InitStruct);
    for(i = 0; i < 16; i++)
    {
        value[i] = i*4096/16;
    }
    DAC_SetBufferValue(HW_DAC0, value, 16); /*д��DAC buffer ���д��16�� uint16_t ��ת��ֵ */
    while(1)
    {
        /* ��ͣ�Ĵ��� DAC ���������� */
        DAC_StartConversion(HW_DAC0);
    }
}


