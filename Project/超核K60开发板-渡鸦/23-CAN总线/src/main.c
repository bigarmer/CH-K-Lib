#include "gpio.h"
#include "uart.h"
#include "can.h"


void CAN_ISR(void)
{
    static uint32_t cnt;
    uint8_t buf[8];
    uint8_t len;
    if(CAN_ReadData(HW_CAN1, 3, buf, &len) == 0)
    {
        printf("DataReceived:%d ", cnt++);
        while(len--)
        {
            printf("[%d]:0x%X ", len, buf[len]);
        }
        printf("\r\n");
    }
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("CAN test\r\n");
    /* ��ʼ�� CAN */
    CAN_QuickInit(CAN1_TX_PE24_RX_PE25, kCAN_Baudrate_125K);
    
    /* ���ý����ж� ��װ�ص����� */
    CAN_CallbackInstall(HW_CAN1, CAN_ISR);
    CAN_ITDMAConfig(HW_CAN1, 3, kCAN_IT_RX);
    
    /* ���� 3������ΪCAN�������� */
    CAN_SetReceiveMB(HW_CAN1, 3, 0x56);
    while(1)
    {
        CAN_WriteData(HW_CAN1, 2, 0x10, (uint8_t *)"CAN TEST", 8); /* ʹ������2 ����ID:0x10 ���� "CAN TEST" */
        DelayMs(500);
    }
}


