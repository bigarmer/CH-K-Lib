#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "DataScope_DP.h"
#include "math.h"
 /*
     ʵ�����ƣ�UART��ӡ��Ϣ
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ʹ�ô���UART��оƬ�ĳ�����Ϣ��оƬ�ϵ���ͳ�ȥ
        ������Ϻ󣬽���while�У�ִ��С����˸Ч��
*/
 
int main(void)
{
    int cnt;
    int i;
    float num;
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    /* ��ӡоƬ��Ϣ */
    printf("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());
    /* ��ӡʱ��Ƶ�� */
//  CLOCK_GetClockFrequency(kCoreClock, &clock);
//    printf("core clock:%dHz\r\n", clock);
//    CLOCK_GetClockFrequency(kBusClock, &clock);
 //   printf("bus clock:%dHz\r\n", clock);

    while(1)
    { 
        DataScope_Get_Channel_Data( sin(num+=0.5) , 1 ); //������ 1.0  д��ͨ�� 1
        DataScope_Get_Channel_Data( 2.0 , 2 ); //������ 2.0  д��ͨ�� 2
        DataScope_Get_Channel_Data( 3.0 , 3 ); //������ 3.0  д��ͨ�� 3
        DataScope_Get_Channel_Data( 4.0 , 4 ); //������ 4.0  д��ͨ�� 4
        DataScope_Get_Channel_Data( 5.0 , 5 ); //������ 5.0  д��ͨ�� 5
        DataScope_Get_Channel_Data( 6.0 , 6 ); //������ 6.0  д��ͨ�� 6
        DataScope_Get_Channel_Data( 7.0 , 7 ); //������ 7.0  д��ͨ�� 7
        DataScope_Get_Channel_Data( 8.0 , 8 ); //������ 8.0  д��ͨ�� 8
        DataScope_Get_Channel_Data( 9.0 , 9 ); //������ 9.0  д��ͨ�� 9
        DataScope_Get_Channel_Data( 10.0 , 10); //������ 10.0 д��ͨ�� 10
      
        cnt = DataScope_Data_Generate(10); //����10��ͨ���� ��ʽ��֡���ݣ�����֡���ݳ���
		
        for( i = 0 ; i < cnt; i++)  //ѭ������,ֱ���������   
        {
            UART_WriteByte(HW_UART0, DataScope_OutPut_Buffer[i]);  
        }
    //    GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(1);
        }
}


