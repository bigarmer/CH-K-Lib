#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"
#include "ftm.h"
#include "dma.h"
#include "pit.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�DMA�������
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ʹ��DMAģ��ʵ������������ܣ����ڼ���Ҫʹ��DMA��PITģ��Эͬ
       ʹ��DMA��0ͨ�����ӵ�PTA5�����������
       ʹ��DMA��1ͨ�����ӵ�PTB23�����������
       ��PIT�ж��д�������������
*/
static const uint32_t DMA_PORT_TriggerSourceTable[] = 
{
    PORTA_DMAREQ,
    PORTB_DMAREQ,
    PORTC_DMAREQ,
    PORTD_DMAREQ,
    PORTE_DMAREQ,
};

/**
 * @brief  DMA �������������ʼ��     
 * @param  dmaChl :DMAͨ����
 * @param  instance :�˿ں� ����HW_GPIOA
 * @param  pinIndex :���ź�
 * @retval None
 */
static void DMA_PulseCountInit(uint32_t dmaChl, uint32_t instance, uint32_t pinIndex)
{
    /* ����2·���� ����ΪDMA���� */
    GPIO_QuickInit(instance, pinIndex, kGPIO_Mode_IPU);
    /* ����ΪDMA�����ش��� */
    GPIO_ITDMAConfig(instance, pinIndex, kGPIO_DMA_RisingEdge);
    /* ����DMA */
    uint8_t dummy1, dummy2;
    DMA_InitTypeDef DMA_InitStruct1;  
    DMA_InitStruct1.chl = dmaChl;  
    DMA_InitStruct1.chlTriggerSource = DMA_PORT_TriggerSourceTable[instance];
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal; 
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = DMA_CITER_ELINKNO_CITER_MASK; /* ���ֵ */
    
    DMA_InitStruct1.sAddr = (uint32_t)&dummy1;
    DMA_InitStruct1.sLastAddrAdj = 0; 
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    
    DMA_InitStruct1.dAddr = (uint32_t)&dummy2; 
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 0; 
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_Init(&DMA_InitStruct1);
    /* �������� */
    DMA_StartTransfer(dmaChl);
}

//�жϺ�������
static void PIT_ISR(void)
{
    /* ����DMA �ǵ������� ������Ҫ�����ֵ��һ�� */
    uint32_t ch_value[2];
    /* CH0 */
    ch_value[0] = DMA_CITER_ELINKNO_CITER_MASK - DMA_GetMajorLoopCount(HW_DMA_CH0);
    /* CH1 */
    ch_value[1] = DMA_CITER_ELINKNO_CITER_MASK - DMA_GetMajorLoopCount(HW_DMA_CH1);
    /* ������� */
    DMA_CancelTransfer();
    DMA_SetMajorLoopCount(HW_DMA_CH0, DMA_CITER_ELINKNO_CITER_MASK);
    DMA_SetMajorLoopCount(HW_DMA_CH1, DMA_CITER_ELINKNO_CITER_MASK);
    /* ��ʼ��һ�δ��� */
    DMA_StartTransfer(HW_DMA_CH0);
    DMA_StartTransfer(HW_DMA_CH1);
    printf("[CH%d]:%4dHz [CH%d]:%4dHz\r\n", 0, ch_value[0], 1, ch_value[1]);
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DMA pulse count test\r\n");
    printf("connect A06&C01, B00&B23 \r\n");
    
    /* ����2·PWMͨ�� ������ͬƵ�ʵ�PWM�� */
    FTM_PWM_QuickInit(FTM0_CH0_PC01, 10000);
    FTM_PWM_QuickInit(FTM1_CH0_PB00, 20000);

    
    /* ����DMA��׽���������ź� (ÿ���˿�ֻ�ܲ���һ·DMA Ҳ����˵DMA�������ֻ�ܲ���5·(PTA,PTB,PTC,PTD,PTE))*/
    DMA_PulseCountInit(HW_DMA_CH0, HW_GPIOA, 6);
    DMA_PulseCountInit(HW_DMA_CH1, HW_GPIOB, 23);
    
    /* ����һ��PIT�ж�������ʾ�յ��ļ��� */
    PIT_QuickInit(HW_PIT_CH0, 1000 * 1000);
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF);

    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


