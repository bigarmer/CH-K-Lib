#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "spi.h"
#include "dma.h"


/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */


/*K22��DMA����Դ����K64��Щ��ͬ �����¶���һ��*/
#define SPI0_RECEIVE_DMAREQ         14																		
#define SPI0_SEND_DMAREQ        15
#define SPI1_SEND_RECEIVE_DMAREQ    16
/* ����ʹ�õ�DMAͨ���� */
#define DMA_SEND_CH             HW_DMA_CH1
#define DMA_REV_CH              HW_DMA_CH2

static uint32_t SPI_sendBuffer[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
static uint32_t SPI_receiveBuffer[16];

/* SPI ���ʹ���Դ��� */
static const uint32_t SPI_SendDMATriggerSourceTable[] = 
{
		SPI0_SEND_DMAREQ,
		SPI1_SEND_RECEIVE_DMAREQ,
}; 
/* SPI ���մ���Դ��� */
static const uint32_t SPI_RevDMATriggerSourceTable[] = 
{
		SPI0_RECEIVE_DMAREQ,
		SPI1_SEND_RECEIVE_DMAREQ,
}; 
/* SPIģ�鷢�ͼĴ���*/
static const void* SPI_PUSHPortAddrTable[] = 
{
    (void*)&SPI0->PUSHR,
    (void*)&SPI1->PUSHR,  
};
/* SPIģ����ռĴ���*/
static const void* SPI_DataPortAddrTable[] = 
{
    (void*)&SPI0->POPR,
    (void*)&SPI1->POPR,  
}; 

/**
 * @brief  ���� DMA Դ��ַ
 * @param[in]  chl  DMA ͨ����
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * \param[in]  * txBuf  Դ��ַ
 * @retval None
 */
void DMA_SetSAddr(uint8_t chl, uint32_t * txBuf)
{

		DMA0->TCD[chl].SADDR = (uint32_t)txBuf;
}
/**
 * @brief   ���� DMA Ŀ���ַ
 * @param[in]  chl  DMA ͨ����
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * \param[in]  * rxBuf Ŀ���ַ
 * @retval None
 */
void DMA_SetDAddr(uint8_t chl, uint32_t * rxBuf)
{

		DMA0->TCD[chl].DADDR = (uint32_t)rxBuf;
}


void DMA_ISR(void);

/* SPI���� DMA���� */
static void SPI_DMASendInit(uint32_t SPIInstnace, uint8_t dmaChl, uint32_t * txBuf)
{
		DMA_InitTypeDef DMA_InitStruct1 = {0};																							/*��ճ�ʼ���ṹ��*/
    DMA_InitStruct1.chl = dmaChl;																												/*ѡ��DMAͨ��*/
    DMA_InitStruct1.chlTriggerSource = SPI_SendDMATriggerSourceTable[SPIInstnace];			/*ѡ�񴥷�ԴSPI0����*/
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;											/*ѡ�񴥷���ʽ*/
    DMA_InitStruct1.minorLoopByteCnt = 4;																								/*��ѭ�� ÿ��DMA��������ֽ���*/
    DMA_InitStruct1.majorLoopCnt = 16;																									/*��ѭ�� ������DMA�������*/
																																												/*����Դ���� uint32_t SPI_sendBuffer[16] ��ÿ�δ���4���ֽ� �ܹ�����16��*/
    DMA_InitStruct1.sAddr = (uint32_t)txBuf;																						/*Դ��ַ*/
    DMA_InitStruct1.sLastAddrAdj = 0; 																									/*����major loop��ɺ�Դ��ַƫ����*/
    DMA_InitStruct1.sAddrOffset = 4;																										/*ÿ��minor loop��ɺ�Դ��ַƫ����*/
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_32;																	/*Դ���ݵ�ַ���*/
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;																					/*ģֵ����*/
    
    DMA_InitStruct1.dAddr = (uint32_t)SPI_PUSHPortAddrTable[SPIInstnace]; 							/*Ŀ���ַ*/
    DMA_InitStruct1.dLastAddrAdj = 0;																										/*����major loop��ɺ�Ŀ���ַƫ����*/
    DMA_InitStruct1.dAddrOffset = 0;																										/*ÿ��minor loop��ɺ�Ŀ���ַƫ����*/
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_32;																	/*Ŀ���ַ���ݿ��*/
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;																					/*ģֵ����*/
    DMA_Init(&DMA_InitStruct1);																													/*��ʼ��DMAģ��*/
		DMA_EnableAutoDisableRequest(dmaChl, true);																					/*����major loop��ɺ��Զ��ر�DMA����*/
		DMA_CallbackInstall(dmaChl, DMA_ISR);																								/*ע��DMA1�жϻص�����*/
		DMA_ITConfig(dmaChl, kDMA_IT_Major, true);																					/*ʹ��DMA�ж�*/
    DMA_EnableRequest(dmaChl);																													/*ʹ��DMA*/
}

/*SPI ���� DMA����*/
static void SPI_DMAReceiveInit(uint32_t SPIInstnace, uint8_t dmaChl, uint32_t * rxBuf)
{
		DMA_InitTypeDef DMA_InitStruct1 = {0};																							/*��ճ�ʼ���ṹ��*/
    DMA_InitStruct1.chl = dmaChl;																												/*ѡ��DMAͨ��*/
    DMA_InitStruct1.chlTriggerSource = SPI_RevDMATriggerSourceTable[SPIInstnace];				/*ѡ�񴥷�ԴSPI0����*/
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;											/*ѡ�񴥷���ʽ*/
    DMA_InitStruct1.minorLoopByteCnt = 4;																								/*��ѭ�� ÿ��DMA��������ֽ���*/
    DMA_InitStruct1.majorLoopCnt = 1;																										/*��ѭ�� ������DMA�������*/
    
    DMA_InitStruct1.sAddr = (uint32_t)SPI_DataPortAddrTable[SPIInstnace];								/*Դ��ַ*/
    DMA_InitStruct1.sLastAddrAdj = 0; 																									/*����major loop��ɺ�Դ��ַƫ����*/
    DMA_InitStruct1.sAddrOffset = 0;																										/*ÿ��minor loop��ɺ�Դ��ַƫ����*/
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_32;																	/*Դ���ݵ�ַ���*/
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;																					/*ģֵ����*/
    
    DMA_InitStruct1.dAddr = (uint32_t)rxBuf; 																						/*Ŀ���ַ*/
    DMA_InitStruct1.dLastAddrAdj = 0;																										/*����major loop��ɺ�Ŀ���ַƫ����*/
    DMA_InitStruct1.dAddrOffset = 4;																										/*ÿ��minor loop��ɺ�Ŀ���ַƫ����*/
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_32;																	/*Ŀ���ַ���ݿ��*/
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;																					/*ģֵ����*/
    DMA_Init(&DMA_InitStruct1);																													/*��ʼ��DMAģ��*/
		DMA_EnableAutoDisableRequest(dmaChl, false);																				/*����major loop��ɺ󲻹ر�DMA���� �����ȴ�����*/
    DMA_EnableRequest(dmaChl);																													/*ʹ��DMA*/
}


uint8_t DMA_SendFinish = 0;


int main(void)
{	
    DelayInit();

		GPIO_QuickInit(HW_GPIOD, 4, kGPIO_Mode_OPP);																				/*��ʼ��PTD4 SPIƬѡ�ź�*/
/*--------------------------SPI0 ��ʼ��------------------------------*/	
		SPI_InitTypeDef SPI_InitStruct1;
		SPI_InitStruct1.baudrate = 1000000;																									/*������1000k*/
		SPI_InitStruct1.frameFormat = kSPI_CPOL0_CPHA0;																			/*�����ݴ���ʱSCKΪ�� ����λ��*/
		SPI_InitStruct1.dataSize = 8;																												/*���ݿ��*/
		SPI_InitStruct1.instance = HW_SPI0;																									/*ѡ��SPI0*/
		SPI_InitStruct1.mode = kSPI_Master;																									/*��ģʽ*/
		SPI_InitStruct1.bitOrder = kSPI_MSB;																								/*�ȷ����λ*/
		SPI_InitStruct1.ctar = HW_CTAR0;																										/*ʹ��0�ſ��ƼĴ���*/
		
		PORT_PinMuxConfig(HW_GPIOD, 1, kPinAlt2);																						/*�������� PTD1:SCK PTD2:MOSI PTD3:MISO*/
		PORT_PinMuxConfig(HW_GPIOD, 2, kPinAlt2);
		PORT_PinMuxConfig(HW_GPIOD, 3, kPinAlt2);		
		SPI_Init(&SPI_InitStruct1);																													/*��ʼ��SPIģ��*/
		SPI_ITDMAConfig(HW_SPI0,kSPI_DMA_TFFF, true);																				/*ʹ�ܷ���DMA����*/
		GPIO_ResetBit(HW_GPIOD,4);																													/*Ƭѡ����*/
		SPI_DMASendInit(HW_SPI0, DMA_SEND_CH, SPI_sendBuffer);															/* SPI���� DMA���� */
	
	

		SPI_ITDMAConfig(HW_SPI0,kSPI_DMA_RFDF, true);																				/*ʹ�ܽ���DMA����*/
		SPI_DMAReceiveInit(HW_SPI0, DMA_REV_CH, SPI_receiveBuffer);													/*SPI ���� DMA����*/

/*----------------------------------------------------------------------*/
		
    UART_QuickInit(UART1_RX_PE01_TX_PE00, 115200);
    printf("CoreClock:%dHz\r\n", GetClock(kCoreClock));
    printf("BusClock:%dHz\r\n", GetClock(kBusClock));

		
    while(1)
    {					
				if(DMA_SendFinish == 1)
				{
						DMA_SendFinish = 0;
						DelayMs(200);

						uint8_t i;
						for(i=0; i<16; i++)																													/*������� �ٴν���*/
						{
								SPI_receiveBuffer[i] = 0;
						}
						DMA_SetMajorLoopCounter(DMA_SEND_CH,16);																		/*����major loopֵ*/
						DMA_SetSAddr(DMA_SEND_CH, SPI_sendBuffer);																	/*�������÷���Դ��ַ*/
						DMA_SetDAddr(DMA_REV_CH, SPI_receiveBuffer);																/*������յ�ַ*/
						DMA_EnableRequest(DMA_SEND_CH);																							/*ʹ�ܷ���ͨ��DMA����*/					
				}
    }
}
//DMA1�жϻص�����
void DMA_ISR(void)
{
		DMA_SendFinish = 1;																																	/*DMA������ɱ�־λ*/
		printf("DMA INT \r\n");
}


