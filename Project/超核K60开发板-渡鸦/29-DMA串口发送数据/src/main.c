#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"


/* DMA ���ͺ��� */
/* 0 �˴η��ͳɹ� else �˴η���ʧ�� */
static uint32_t UART0_SendWithDMA(uint32_t dmaChl, uint8_t *buf, uint32_t size)
{
    DMA_InitTypeDef DMA_InitStruct1;
    if(DMA_IsTransferComplete(dmaChl))
    {
        /* DMA ͨ����æ */
        return 1;
    }
    DMA_InitStruct1.chl = dmaChl;  /* ʹ��0ͨ�� */
    DMA_InitStruct1.chlTriggerSource = UART0_TRAN_DMAREQ; /* ������ɴ���һ֡�󴥷� */
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal; /* ��ͨģʽ �������ڴ���ģʽ */
    DMA_InitStruct1.minorByteTransferCount = 1;
    DMA_InitStruct1.majorTransferCount = size;
    
    DMA_InitStruct1.sourceAddress = (uint32_t)buf; /*Դ��ַ */
    DMA_InitStruct1.sourceAddressMajorAdj = 0; 
    DMA_InitStruct1.sourceAddressMinorAdj = 1;
    DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8; /* 8λ����λ�� */
    
    DMA_InitStruct1.destAddress = (uint32_t)&UART0->D; 
    DMA_InitStruct1.destAddressMajorAdj = 0;
    DMA_InitStruct1.destAddressMinorAdj = 0; 
    DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8;
    DMA_Init(&DMA_InitStruct1);
    /* �������� */
    DMA_StartTransfer(dmaChl);
    return 0;
}



int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DMA UART transmit test\r\n");
    
    /* ��UART0 DMA����ʹ�� */
    UART_ITDMAConfig(HW_UART0, kUART_DMA_Tx);
    
    while(1)
    {
        UART0_SendWithDMA(HW_DMA_CH0, "This string is send via DMA\r\n", 29);
        /* �ȴ�DMA������� */
        while(DMA_IsTransferComplete(HW_DMA_CH0) == 1);
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(50);
    }
}


