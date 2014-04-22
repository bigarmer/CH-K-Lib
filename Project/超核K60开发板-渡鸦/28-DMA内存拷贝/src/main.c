#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"

#include <string.h>
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�DMA�ڴ濽��
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч�����ϵ��оƬִ��dma���ݴ��䣬���ݴ�����8λ��������64�ֽ�
        ���DMA���������Ļ����������ϵ�С�ƽ���˸
*/
static uint8_t SourceBuffer[64]; //Դ��ַ
static uint8_t DestBuffer[64];   //Ŀ���ַ

int main(void)
{
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DMA memcpy test\r\n");
    
    /* һ�δ����� ִ�� minorByteTransferCount ��ɺ� ����һ��Minor Loop */
    /* majorTransferCount �� Minor Loop ѭ��ִ�к� DMA�������� */
    
    /* д��������� ���ҽ��������������*/
    memset(SourceBuffer, 'T', ARRAY_SIZE(SourceBuffer)); //��������д��64�ֽڵ�T
    memset(DestBuffer, 0, ARRAY_SIZE(DestBuffer));       //��������е�����
    
    DMA_InitTypeDef DMA_InitStruct1;
    DMA_InitStruct1.chl = HW_DMA_CH0;  /* ʹ��DMA0ͨ�� */
    DMA_InitStruct1.chlTriggerSource = DMA_MUX1; /*�����Ŭ������ */
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal; /* ��ͨģʽ �������ڴ���ģʽ */
    DMA_InitStruct1.minorByteTransferCount = ARRAY_SIZE(SourceBuffer); /*ÿ�δ�������*/
    DMA_InitStruct1.majorTransferCount = 1; /* �����Ա����� 1�� */
    
    DMA_InitStruct1.sourceAddress = (uint32_t)SourceBuffer; /*Դ��ַ */
    DMA_InitStruct1.sourceAddressMajorAdj = 0; /* ÿ�δ�������Minor Loop��Դ��ַ ƫ���� */
    DMA_InitStruct1.sourceAddressMinorAdj = 1;  /* ÿ�δ�������Major Loop��Դ��ַ ƫ���� */
    DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8; /* 8λ����λ�� */
    
    DMA_InitStruct1.destAddress = (uint32_t)DestBuffer; /* Ŀ�ĵ�ַ */
    DMA_InitStruct1.destAddressMajorAdj = 0;
    DMA_InitStruct1.destAddressMinorAdj = 1; /*ÿ�δ���� Ŀ�ĵ�ַ+1 */
    DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8;  /* 8λ����λ�� */
    DMA_Init(&DMA_InitStruct1);
    /* ����DMA���� */
    DMA_StartTransfer(HW_DMA_CH0);
    /* �ȴ�DMA������� */
    while(DMA_IsTransferComplete(HW_DMA_CH0));
    for(i=0;i<ARRAY_SIZE(DestBuffer);i++)
    {
        /* ����������ɹ� */
        if(DestBuffer[i] != SourceBuffer[i])
        {
            printf("DMA test error:%d\r\n", i);
            while(1);
        }
    }
    printf("DMA memcpy test OK!");
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


