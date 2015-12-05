#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "imu_rev.h"

/* ������̬ģ��������� */
/* ��ʵ����ϳ��˴�����̬ģ�� �뽫��̬ģ�����ӵ� �������Ӧ���š� ����ͨ����ͨ���Դ�����ʾ��� */

void IMU_REV_Handler(void)
{
    struct imu_data data;
    
    /* get data */
    imu_rev_get_data(&data);
    
    /* print results */
    printf("P/R/Y/P:%05d %05d %05d %05d\r", data.pitch/100, data.roll/100, data.yaw/10, data.presure);
}


static void UART4_ISR(uint16_t byteReceived)
{
    /* hander imu receive */
    imu_rev_process((char)byteReceived, IMU_REV_Interrupt);
}

 struct imu_rev_init installer = 
{
    NULL, /* ���ʹ��Interruptģʽ ������NULL���� */
    IMU_REV_Handler, /* �ɹ�����һ֡��Ĵ����� */
};

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
	  GPIO_WriteBit(HW_GPIOE, 26, 0);//����ѡ��S0=0
	  GPIO_WriteBit(HW_GPIOE, 27, 1);//����ѡ��S1=1
    /* open interrupt */
    UART_CallbackRxInstall(HW_UART4, UART4_ISR);
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);

    /* install functions */
    imu_rev_init(&installer);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


