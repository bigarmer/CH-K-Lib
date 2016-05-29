#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "imu_rev.h"


/* ������̬ģ��������� */
/* ��ʵ����ϳ��˴�����̬ģ�� �뽫��̬ģ�����ӵ� �������Ӧ���š� ����ͨ����ͨ���Դ�����ʾ��� */

imu_data imu;

/* UART4 �����ж� */
static void UART4_ISR(uint16_t byteReceived)
{
    /* �ڴ����ж��е��� imu_rev_process ����, byteReceived�Ǵ��ڽ��յ������� */
    imu_rev_process(byteReceived);
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    /* ��ӡ������� */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("IMU data receie, please connect IMU Tx/RX to PC16, PC17...\r\n");
    
    /*��̬ģ�鴮�� */
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 115200); 
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_OPP);//����ѡ��S0
    GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_OPP);//����ѡ��S1
    GPIO_WriteBit(HW_GPIOE, 26, 1);//����ѡ��S0=0
    GPIO_WriteBit(HW_GPIOE, 27, 0);//����ѡ��S1=1
    
    /* open interrupt */
    UART_CallbackRxInstall(HW_UART4, UART4_ISR);
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);

    /* ��ʼ��IMUģ�� */
    imu_rev_init();
    
    while(1)
    {
        /* ��ȡ���ݲ���ӡ */
        imu_get_data(&imu);
        printf("P/R/Y:%2.2f %2.2f %2.2f\r\n", ((float)imu.pitch)/100, ((float)imu.roll)/100, ((float)imu.yaw)/10);
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(50);
    }
}


