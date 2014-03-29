#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"


/* ���õ�FTMͨ����: */
/*
 FTM0_CH4_PB12   // ftm0 ģ�� 4ͨ�� PB12����
 FTM0_CH5_PB13   
 FTM0_CH5_PA00   
 FTM0_CH6_PA01   
 FTM0_CH7_PA02   
 FTM0_CH0_PA03
 FTM0_CH1_PA04   
 FTM0_CH2_PA05   
 FTM0_CH3_PA06   
 FTM0_CH4_PA07   
 FTM0_CH0_PC01   
 FTM0_CH1_PC02   
 FTM0_CH2_PC03   
 FTM0_CH3_PC04   
 FTM0_CH4_PD04   
 FTM0_CH5_PD05   
 FTM0_CH6_PD06   
 FTM0_CH7_PD07   
 FTM1_CH0_PB12   
 FTM1_CH1_PB13   
 FTM1_CH0_PA08   
 FTM1_CH1_PA09   
 FTM1_CH0_PA12   
 FTM1_CH1_PA13   
 FTM1_CH0_PB00   
 FTM1_CH1_PB01   
 FTM2_CH0_PA10   
 FTM2_CH1_PA11   
 FTM2_CH0_PB18   
 FTM2_CH1_PB19   
*/

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    
    /* ��ʼ��һ��ģ���һ��ģʽ: ��ʼ��ģ�鱾��->����оƬ�ֲ� ��ʼ����Ӧ�ĸ�������->ʹ��ģ�� */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.instance = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_Init(&UART_InitStruct1);
    
    /* ��ʼ������0��Ӧ������ D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    printf("ftm test, pwm will be generated on PC01\r\n");
    
    /* ʹ�ÿ��ٳ�ʼ�� ������ѧ����ɱ�Ҫ���� */
    FTM_PWM_QuickInit(FTM0_CH0_PC01, 3000);
    
//    /* ����һ�ַ��� ��ͨģʽ ��ʼ��FTMģ�� �Ƽ� */
//    FTM_PWM_InitTypeDef FTM_PWM_InitStruct1 = {0};
//    FTM_PWM_InitStruct1.chl = HW_FTM_CH0; /* ͨ��0 */
//    FTM_PWM_InitStruct1.frequencyInHZ = 3000; /* 3Khz */
//    FTM_PWM_InitStruct1.instance = HW_FTM0;
//    FTM_PWM_InitStruct1.mode = kPWM_EdgeAligned; /* ���ض���ģʽ ������ͨ PWM�� */
//    FTM_PWM_Init(&FTM_PWM_InitStruct1);
//    
//    /* ��ʼ����Ӧ������ */
//    PORT_PinMuxConfig(HW_GPIOC, 1, kPinAlt4);
    
    /* ����FTM ��ռ�ձ� */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 5000); // 50%ռ�ձ� 0-10000 ��Ӧ 0-100%
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


