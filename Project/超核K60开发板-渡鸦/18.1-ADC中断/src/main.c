#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"

static int32_t ADC_Value;

void ADC_ISR(int32_t val)
{
    ADC_Value = val;
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("ADC test\r\n");
    
    /* ��ʼ��ADCģ�� ADC0_SE19_BM0 */
    ADC_InitTypeDef ADC_InitStruct1;
    ADC_InitStruct1.instance = HW_ADC0;
    ADC_InitStruct1.clockDiv = kADC_ClockDiv2; /* ADC����ʱ��2��Ƶ */
    ADC_InitStruct1.resolutionMode = kADC_SingleDiff10or11;
    ADC_InitStruct1.triggerMode = kADC_TriggerSoftware; /* �������ת�� */
    ADC_InitStruct1.singleOrDiffMode = kADC_Single; /*����ģʽ */
    ADC_InitStruct1.continueMode = kADC_ContinueConversionEnable; /* ��������ת�� ת��һ�κ� �Զ���ʼ��һ��ת��*/
    ADC_InitStruct1.hardwareAveMode = kADC_HardwareAverageDisable; /*��ֹ Ӳ��ƽ�� ���� */
    ADC_InitStruct1.vref = kADC_VoltageVREF;                       /* ʹ���ⲿVERFH VREFL ��Ϊģ���ѹ�ο� */
    ADC_Init(&ADC_InitStruct1);
    
    /* �ж����� */
    ADC_CallbackInstall(HW_ADC0, ADC_ISR);
    ADC_ITDMAConfig(HW_ADC0, kADC_MuxA, kADC_IT_EOF);
    
    /* ��ʼ����Ӧ���� */
    /* DM0����Ϊר�ŵ�ģ������ ADCʱ �������ø���  DM0Ҳ�޷�������ͨ���������� */
    
    /* ����һ��ADCת�� */
    ADC_StartConversion(HW_ADC0, 19, kADC_MuxA);
    
    while(1)
    {
        printf("ADC:%d\r", ADC_Value);
        DelayMs(50);
    }
}


