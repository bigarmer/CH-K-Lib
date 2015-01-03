#include "unit_test.h"
#include "mb85rc64.h"
#include "chlib_k.h"
#include "fifo.h"
#include "mb85rc64.h"
#include "boarddef.h"
#include "chlib_k.h"



void TestClock(void)
{
    uint32_t i;
    printf("%s...\r\n", __func__);
    CLOCK_GetClockFrequency(kCoreClock, &i);
    printf("CoreClock:%d\r\n", i);
    CLOCK_GetClockFrequency(kBusClock, &i);
    printf("kBusClock:%d\r\n", i);
    
    printf("%s done.\r\n", __func__);
}

void TestADCTime(void)
{
    uint32_t i, clock;
    printf("%s...\r\n", __func__);
    
    ADC_QuickInit(ADC1_SE0_DP0, kADC_SingleDiff12or13);
    
    i = SYSTICK_GetVal();
    ADC_QuickReadValue(ADC1_SE0_DP0);
    i =  i - SYSTICK_GetVal();
    
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    
    printf("ADC_QuickReadValue cost: %dticks , %dus\r\n", i, i/(clock/1000/1000));
    
    printf("%s done.\r\n", __func__);
}

void TestLED(void)
{
    int i;
    
    printf("%s...\r\n", __func__);
    
    for(i=0;i<5;i++)
    {
        LED0 = !LED0;
        LED1 = !LED1;
        LED2 = !LED2;
        LED3 = !LED3;
        DelayMs(50);
    }
    
    printf("%s done.\r\n", __func__);
}

void TestMB85RC64(void)
{
    int r;
    
    printf("%s...\r\n", __func__);
    r = MB85RC64_Test(BOARD_I2C_MAP);
    printf("Test %s\r\n", (r)?("OK"):("Error"));
    printf("%s done.\r\n", __func__);
}



static void UART_485_ISR1(uint16_t data)
{
    printf("%s:%d %c\r\n", __func__, data, data);
}

void TestRS485Receive(void)
{
    uint32_t instance;
    printf("%s...\r\n", __func__);
    
    instance = UART_QuickInit(RS485_CH1_MAP, 115200);
    UART_CallbackRxInstall(instance, UART_485_ISR1);
    UART_ITDMAConfig(instance, kUART_IT_Rx, true);
    
    printf("%s done.\r\n", __func__);
}

//RS485 �жϷ���
void UART_485_TXISR1(uint16_t dataToSend)
{
    static char data[] = "HelloWorld";
    static char *p = data;
    dataToSend = *p++; //�����ݶ���dataToSend �жϷ����������󣬾ͻ��Զ���������
    
    // ��������ȫ������ �رշ����ж� ��λָ��
    if(p == (data + sizeof(data)))
    {
        UART_ITDMAConfig(HW_UART5, kUART_IT_Tx, false);
        p = data;
    }
}

//485 �жϷ��Ͳ���
void TestRS485IntSend(void)
{
    uint32_t instance;
    printf("%s...\r\n", __func__);
    instance = UART_QuickInit(RS485_CH1_MAP, 115200);
    UART_CallbackRxInstall(instance, UART_485_TXISR1);
    UART_ITDMAConfig(instance, kUART_IT_Tx, true);
    printf("%s done.\r\n", __func__);
}

void TestRTC(void)
{
    RTC_DateTime_Type td = {0};
    
    printf("%s...\r\n", __func__);
    
    td.day = 20;
    td.hour = 23;
    td.minute = 59;
    td.second = 50;
    td.year = 2014;
    td.month = 11;
    RTC_QuickInit();

    if(RTC_IsTimeValid())
    {
        printf("Time error, inject time now!\r\n ");
        RTC_SetDateTime(&td);
    }
    
    RTC_GetDateTime(&td);
    printf("%d-%d-%d %d:%d:%d\r\n", td.year, td.month, td.day, td.hour, td.minute, td.second);
    
    printf("%s done.\r\n", __func__);
    
}


float32_t test_signal[32] = {
0		,0.3090	,0.5878	,0.8090	,0.9511	,1.0000	,0.9511	,0.8090,  
0.5878	,0.3090	,0.0000	,-0.3090,-0.5878,-0.8090,-0.9511,-1.0000,
-0.9511	,-0.8090,-0.5878,-0.3090,-0.0000,0.3090	,0.5878	,0.8090,
0.9511 	,1.0000	,0.9511	,0.8090	,0.5878	,0.3090	,0.0000	,-0.3090   
};

float32_t test_input[64] = {0};
float32_t test_output[32] = {0};

void TestFFTTime(void)
{
    uint32_t i, clock;
    printf("%s...\r\n", __func__);
    
    uint32_t fftSize = 32; 
    uint32_t ifftFlag = 0; 
    uint32_t doBitReverse = 1;
    
    i = SYSTICK_GetVal();

	arm_cfft_f32(&arm_cfft_sR_f32_len32, test_input, ifftFlag, doBitReverse);
	arm_cmplx_mag_f32(test_input, test_output, fftSize);
    
    i =  i - SYSTICK_GetVal();
    
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    
    printf("arm_cfft_f32 cost: %dticks , %dus\r\n", i, i/(clock/1000/1000));
    
    printf("%s done.\r\n", __func__);
}



