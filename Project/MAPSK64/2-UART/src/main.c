#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/* UART ���ٳ�ʼ���ṹ��֧�ֵ�����* ʹ��ʱ�����Ƽ���׼��ʼ�� */
/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/
 
 /*
     ʵ�����ƣ�UART��ӡ��Ϣ
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ʹ�ô���UART��оƬ�ĳ�����Ϣ��оƬ�ϵ���ͳ�ȥ
        ������Ϻ󣬽���while�У�ִ��С����˸Ч��
*/
 
int main(void)
{
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    /* ��ӡоƬ��Ϣ */
    printf("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());
    /* ��ӡʱ��Ƶ�� */
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("core clock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("bus clock:%dHz\r\n", clock);
    
    
uint8_t SrcBuf[] = "UART send with DMA";

/* enable UART DMA trigger */
UART0->C2 |= UART_C2_TIE_MASK;
UART0->C5 |= UART_C5_TDMAS_MASK;

/* enable DMA and DMAMUX clock */
SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

/* configure DMAMUX */
DMAMUX->CHCFG[0]= DMAMUX_CHCFG_ENBL_MASK| DMAMUX_CHCFG_SOURCE(3);

/* source configuration */
DMA0->TCD[0].SADDR = (uint32_t)SrcBuf; /* ???*/
DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(0); /* ????????1??? */
DMA0->TCD[0].SOFF= 1; /* ??????????1?? */
DMA0->TCD[0].SLAST = 0; /* ????????????? */
/* destination configuration */
DMA0->TCD[0].DADDR = (uint32_t)&UART0->D;
DMA0->TCD[0].ATTR = DMA_ATTR_DSIZE(0);
DMA0->TCD[0].DOFF= 0;
DMA0->TCD[0].DLAST_SGA= 0;
/* set CITER and BITER to maximum value */
DMA0->TCD[0].CITER_ELINKNO = sizeof(SrcBuf); /* ????? */
DMA0->TCD[0].BITER_ELINKNO = sizeof(SrcBuf);
DMA0->TCD[0].NBYTES_MLNO = 1; /* ????? */
/* enable auto close request */
DMA0->TCD[0].CSR |= DMA_CSR_DREQ_MASK; 
/* start transfer */
DMA0->SERQ = DMA_SERQ_SERQ(0); /* ??REQ? ???? */
    
    while(1)
    {
        /* ��˸С�� */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


