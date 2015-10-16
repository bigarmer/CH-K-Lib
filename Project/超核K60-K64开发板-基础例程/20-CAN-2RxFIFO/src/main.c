#include "gpio.h"
#include "uart.h"
#include "can.h"


#ifdef MK64F12
#error "yandld: MK64 only has CAN0, no CAN1"
#endif

#define CAN_TX_MB  12
#define CAN_TX_ID  0x10

/*
     ʵ�����ƣ�CANͨ�Ų���
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ʹ��CAN1ģ���3����������жϵķ�ʽ��������0x56������
         ʹ��2��������0x10��ַ���豸�������ݣ�ʱ������500���� 
*/

/* CANͨ�� �жϻص����������ж��д�����յ������� */
void CAN_ISR(void)
{
    static uint32_t cnt;
    uint8_t buf[8];
    uint8_t len;
    uint32_t id;
    
    /* FIFO */
    if(CAN_IsRxFIFOEnable(HW_CAN1))
    {
        if(CAN1->IFLAG1 & (1 << CAN_RX_FIFO_MB))
        {
            if(CAN_ReadFIFO(HW_CAN1, &id, buf, &len) == 0)
            {
                printf("DataReceived:%d ", cnt++);
                while(len--)
                {
                    printf("[%d]:0x%X ", len, buf[len]);
                }
                printf("\r\n");
            }
        }
    }
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("CAN test\r\n");
    CAN_QuickInit(CAN1_TX_PE24_RX_PE25, 125*1000);
    
    /* FIFO deep is 0+6 = 6 
    Once FIFO is enabled, MB0-5 cannot used as normal MB, MB0-5 used as RxFIFO and they automaically configed as Rx MB 
    */
    CAN_SetRxFIFO(HW_CAN1);
    CAN_CallbackInstall(HW_CAN1, CAN_ISR);
    CAN_ITDMAConfig(HW_CAN1, CAN_RX_FIFO_MB, kCAN_IT_RX);
    
    while(1)
    {
        CAN_WriteData(HW_CAN1, CAN_TX_MB, CAN_TX_ID, (uint8_t *)"CAN TEST", 8); /* ʹ������2 ����ID:0x10 ���� "CAN TEST" */
        DelayMs(500);
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
}


