#include "shell.h"
#include "clock.h"
#include "adc.h"
#include "board.h"

static uint8_t instance;
static void ADC_CallBack(uint32_t readValue)
{
   // cnt =  ADC_ReadValue(instance, kADC_MuxA);
    shell_printf("Enter ADC ISR:%d\r", readValue);
}

int CMD_ADC(int argc, char * const * argv)
{

    shell_printf("ADC Test CMD\r\n");
    int32_t value;
    ADC_InitTypeDef ADC_InitStruct1;
    ADC_InitStruct1.chl = 1;
    ADC_InitStruct1.clockDiv = kADC_ClockDiv8; //ADCת��ʱ��Ϊ����ʱ��(Ĭ��BusClock) ��8��Ƶ
    ADC_InitStruct1.instance = HW_ADC0;
    ADC_InitStruct1.resolutionMode = kADC_SingleDiff8or9; //����ģʽ��8λ���� ���ģʽ��9λ����
    ADC_InitStruct1.singleOrDiffMode = kADC_Single;
    ADC_InitStruct1.triggerMode = kADC_TriggerSoftware;
    //��ʼ��ADCģ��
  //  ADC_Init(&ADC_InitStruct1);
    
    instance = ADC_QuickInit(BOARD_ADC_MAP, kADC_SingleDiff10or11);
    ADC_CallbackInstall(instance, ADC_CallBack);
    ADC_ITDMAConfig(instance, kADC_MuxA, kADC_IT_EOF);
    ADC_StartConversion(instance, 19, kADC_MuxA);

    return 0;
}




const cmd_tbl_t CommandFun_ADC = 
{
    .name = "ADC",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_ADC,
    .usage = "ADC TEST",
    .complete = NULL,
    .help = "\r\n"
};
