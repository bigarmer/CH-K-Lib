#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */


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

/*
     ʵ�����ƣ�FTM���PWM
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч��������PTA�˿ڵ�9������PWMģʽ
      �ڳ����ռ�ձ�Ϊ50%��Ƶ��Ϊ3KHz�ķ��� 
			����С�Ƶ�ռ�ձ��𽥱仯��С�Ƶ����Ȼ��𽥱仯
*/
int main(void)
{
    uint16_t cnt;
   	DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("ftm test, pwm will be generated on PA09\r\n");
    
    /* ʹ�ÿ��ٳ�ʼ�� ������ѧ����ɱ�Ҫ���� */
    FTM_PWM_QuickInit(FTM1_CH1_PA09, kPWM_EdgeAligned, 5000);
    
    while(1)
    {
        DelayMs(5);
			  cnt = cnt+10;
			 /* ����FTM1ģ��1ͨ����ռ�ձ� */
        FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, cnt); /* 0-10000 ��Ӧ 0-100% */
			 if(cnt >= 10000) cnt = 0;
    }
}


