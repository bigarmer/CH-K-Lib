#include "shell.h"
#include "clock.h"
#include "adc.h"
#include "board.h"

static uint8_t instance;
static void ADC_CallBack(void)
{
    static uint32_t cnt;
    cnt =  ADC_ReadValue(instance, kADC_MuxA);
    shell_printf("Enter ADC ISR:%d\r", cnt);
  //  cnt++;
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
    ADC_InitStruct1.SingleOrDifferential = kADC_Single;
    ADC_InitStruct1.triggerMode = kADC_TriggleSoftware;
    //��ʼ��ADCģ��
    ADC_Init(&ADC_InitStruct1);
    
    
    instance = ADC_QuickInit(BOARD_ADC_MAP, kADC_SingleDiff12or13);
    ADC_CallbackInstall(instance, ADC_CallBack);
    
    ADC_ITDMAConfig(instance, kADC_MuxA, kADC_IT_EOF, ENABLE);
    while(1)
    {
        ADC_StartConversion(instance, 19, kADC_MuxA);
       // value = ADC_QuickReadValue(ADC0_SE20_DM1);
       // shell_printf("AD:%d\r\n", value);
      
        DelayMs(50);
    }
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
