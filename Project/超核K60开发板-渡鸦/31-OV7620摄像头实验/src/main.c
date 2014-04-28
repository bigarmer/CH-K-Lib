#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"
#include "ili9320.h"
#include "dma.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

/*
     ʵ�����ƣ�OV7620����ͷʵ��
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч������Һ��������ʾͼ����Ҫ0V7620����ͷ
*/
#define OV7620_W    (320) // ÿ���ж�������
#define OV7620_H    (240) // �߶��ж�����

//uint8_t CCDBufferPool[OV7620_W*OV7620_H];   //ʹ���ڲ�RAM
volatile uint8_t * CCDBufferPool = SRAM_START_ADDRESS; //ʹ���ⲿSRAM

/* CCD�ڴ�� */
uint8_t * CCDBuffer[OV7620_H];
/* ���Ŷ��� */
#define BOARD_OV7620_PCLK_PORT      HW_GPIOA
#define BOARD_OV7620_PCLK_PIN       (7)   //����ͷʱ������
#define BOARD_OV7620_VSYNC_PORT     HW_GPIOA
#define BOARD_OV7620_VSYNC_PIN      (16) //����ͷ�����ж�
#define BOARD_OV7620_HREF_PORT      HW_GPIOA
#define BOARD_OV7620_HREF_PIN       (17) //����ͷ���ж�
#define BOARD_OV7620_DATA_OFFSET    (8) /* ����ͷ��������PTA8-PTA15 ֻ��Ϊ 0 8 16 24 */

/* ״̬������ */
typedef enum
{
    TRANSFER_IN_PROCESS, //�����ڴ���
    NEXT_FRAME,          //��һ֡����
}OV7620_Status;

static void UserApp(void);

/* ����ͷ���жϴ����� */
void OV7620_ISR(uint32_t pinArray)
{

    static uint8_t status = TRANSFER_IN_PROCESS;  
    switch(status)
    {
        case TRANSFER_IN_PROCESS: /* ���ڴ��� */
            /* �鿴DMA�Ƿ���ɴ��� */
            if(DMA_IsTransferComplete(HW_DMA_CH2) == 0)
            {
                GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_Disable);
                /* ��ʼ�����û� ���� */
                UserApp();
                /* �û������������ */
                status = NEXT_FRAME;
                /* �������û������� �����������ж� ׼����һ�����ݵ��� */
                GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);  
            }
            else
            {
                status = TRANSFER_IN_PROCESS;
            }
            break;
        case NEXT_FRAME:
            /* ������� ��λbuffer��ַ ��ʼ��һ������ */
            DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)CCDBuffer[0]);
            DMA_StartTransfer(HW_DMA_CH2); 
            status =  TRANSFER_IN_PROCESS;
            break;
        default:
            break;
    }
}

static uint16_t RGB2COLOR(uint8_t RR,uint8_t GG,uint8_t BB)
{
  return (((RR/8)<<11)+((GG/8)<<6)+BB/8); 
}

/* �������һ���� �û������� */
static void UserApp(void)
{
    uint32_t i,j;
    static uint32_t cnt;
    printf("SYNC cnt:%d\r\n", cnt++); 
    for(i=0;i<OV7620_H;i++)
    {
        for(j=0;j<OV7620_W;j++)
        {
            ili9320_write_pixel(OV7620_H - i, OV7620_W- j, RGB2COLOR(CCDBuffer[i][j], CCDBuffer[i][j], CCDBuffer[i][j]));
        }
    } 
}

//ov7620����ͷ��ʼ������
static void OV7620_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    uint32_t i;
    //�ѿ��ٵ��ڴ�ظ���ָ��
    for(i=0;i< OV7620_H;i++)
    {
        CCDBuffer[i] = (uint8_t *) &CCDBufferPool[i*OV7620_W];
    }
    /* ���ж�  ���ж� �����ж� */
    GPIO_QuickInit(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_Mode_IPD);
    /* ��ʼ�����ݶ˿� */
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOA, BOARD_OV7620_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    /* ��װ�ص����� */
    GPIO_CallbackInstall(BOARD_OV7620_VSYNC_PORT, OV7620_ISR);
    /* ���ж�����ΪDMA���� */
    GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_DMA_RisingEdge);
    /* ���ж�����Ϊ�жϴ��� */
    GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);
    /* �����ж� */
  //  GPIO_ITDMAConfig(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge); //ʵ�ʲ�û���õ�
    /* ��ʼ��DMA */
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = PORTA_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = OV7620_W;
    DMA_InitStruct1.majorLoopCnt = OV7620_H;
    
    DMA_InitStruct1.sAddr = (uint32_t)&PTA->PDIR + BOARD_OV7620_DATA_OFFSET/8;
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    
    DMA_InitStruct1.dAddr = (uint32_t)CCDBuffer[0];
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;

    DMA_Init(&DMA_InitStruct1);
    /* ��ʼ���� */
    DMA_StartTransfer(HW_DMA_CH2); 
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("OV7620 test\r\n");
    
    ili9320_init();
    SRAM_Init();
    /* ����ͷ�ٶȷǳ��� ��FLexbus �����ٶȵ������ */
    SIM->CLKDIV1 &= ~SIM_CLKDIV1_OUTDIV3_MASK;
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(2);
    OV7620_Init();

    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


