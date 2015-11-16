#include "gpio.h"
#include "uart.h"
#include "can.h"



#define CAN_TX_ID  0x10
#define CAN_RX_ID  0x56

/*
     ʵ�����ƣ�CANͨ�Ų���
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч����ʹ��CAN0ģ���3����������жϵķ�ʽ��������0x56������
         ʹ��2��������0x10��ַ���豸�������ݣ�ʱ������500���� 
*/

/* CANͨ�� �жϻص����������ж��д�����յ������� */
void CAN_ISR(void)
{
    uint8_t buf[8];
    uint8_t len;
    uint32_t id;
    if(CAN_ReadData(HW_CAN0, 3, &id, buf, &len) == 0)
    {
        printf("DataReceived:id 0x%x ", (id>>CAN_ID_STD_SHIFT));
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
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("CAN test\r\n");
    /* ��ʼ�� CAN ʹ��CAN1ģ���PTE24/25���ţ�ͨ���ٶ�Ϊ125k*/
    CAN_QuickInit(CAN0_TX_PB18_RX_PB19, 125*1000);
    
    /* ���ý����ж� ��װ�ص����� */
    CAN_CallbackInstall(HW_CAN0, CAN_ISR);
    /* ����CANͨ���жϽ��չ��ܣ�3������ */
    CAN_ITDMAConfig(HW_CAN0,3, kCAN_IT_RX);
    
    /* ���� 3������ΪCAN�������� */
    CAN_SetRxMB(HW_CAN0, 3, CAN_RX_ID);
    while(1)
    {
        /* ʹ������2 ����ID:0x10 ���� "CAN TEST" */
        CAN_WriteData(HW_CAN0, 2, CAN_TX_ID, (uint8_t *)"CAN TEST", 8); /* ʹ������2 ����ID:0x10 ���� "CAN TEST" */
        DelayMs(500);
        GPIO_ToggleBit(HW_GPIOA, 9);
    }
}


