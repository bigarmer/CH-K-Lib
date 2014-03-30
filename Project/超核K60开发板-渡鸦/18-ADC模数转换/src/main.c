#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"



int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /* Ϊ�˴���� ����ֱ�ӵ��ÿ��ٳ�ʼ���� */
    
    printf("ADC test\r\n");
    
    /* ��ʼ��ADCģ�� */
    ADC_InitTypeDef AD_InitStruct1;
    
    AD_InitStruct1.instance = HW_ADC0;
    AD_InitStruct1.chl = 19;
    AD_InitStruct1.clockDiv = kADC_ClockDiv2;
    AD_InitStruct1.resolutionMode = kADC_SingleDiff10or11; /*���� 10λ���� ��� 11λ���� */
    AD_InitStruct1.triggerMode = kADC_TriggerSoftware;
    AD_InitStruct1.singleOrDiffMode = kADC_Single; /*����ģʽ */
    AD_InitStruct1.continueMode = kADC_ContinueConversionDisable; /*��ֹ���� ת�� */
    AD_InitStruct1.hardwareAveMode = kADC_HardwareAverageDisable; /*��ֹ Ӳ��ƽ�� ���� */
    
    ADC0_SE19_DM0
    while(1)
    {
        /* �ȴ��ж� */
    }
}


