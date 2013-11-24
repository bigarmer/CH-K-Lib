/**
  ******************************************************************************
  * @file    uart.c
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.25
  * @brief   ����K60�̼��� UART ���� ������
  ******************************************************************************
  */
#include "uart.h"
#include "common.h"
#include "string.h"
#include "clock.h"
#include "stdarg.h"
#include "stdio.h"
//���ͽṹ
/*
UART_TxSendTypeDef UART_TxIntStruct1;
*/
UART_Type* UART_DebugPort = NULL;


//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup UART
//! @brief UART driver modules
//! @{


//! @defgroup UART_Exported_Functions
//! @{


void UART_DebugPortInit(UART_MapSelect_TypeDef UARTxMAP,uint32_t UART_BaudRate)
{
    UART_InitTypeDef UART_DebugInitStruct1;
    PeripheralMap_TypeDef *pUART_Map = (PeripheralMap_TypeDef*)&(UARTxMAP);
    //����Ĭ�ϵĵ���UART����
    UART_DebugInitStruct1.UART_BaudRate = UART_BaudRate;
    UART_DebugInitStruct1.UARTxMAP = UARTxMAP;
    //�ҳ���Ӧ��UART�˿�
    switch(pUART_Map->m_ModuleIndex)
    {
        case 0:
            UART_DebugPort = UART0;
            break;
        case 1:
            UART_DebugPort = UART1;
            break;
        case 2:
            UART_DebugPort = UART2;
            break;
        case 3:
            UART_DebugPort = UART3;
            break;
        case 4:
            UART_DebugPort = UART4;
            break;
        default:
            UART_DebugPort = NULL;
            break;
    }
    UART_Init(&UART_DebugInitStruct1);
}
/***********************************************************************************************
 ���ܣ���ʼ������
 �βΣ�UART_InitStruct UART��ʼ���ṹ
 ���أ�0
 ��⣺0
************************************************************************************************/
void UART_Init(UART_InitTypeDef* UART_InitStruct)
{
    UART_Type* UARTx = NULL;
    uint16_t sbr;
    uint8_t brfa; 
    uint32_t clock;
    uint8_t i;
    PeripheralMap_TypeDef* pUART_Map = (PeripheralMap_TypeDef*)&(UART_InitStruct->UARTxMAP);
    //������
    
    //�ҳ���Ӧ��UART�˿�
    switch(pUART_Map->m_ModuleIndex)
    {
        case 0:
            SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
            UARTx = UART0;
            break;
        case 1:
            SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
            UARTx = UART1;
            break;
        case 2:
            SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
            UARTx = UART2;
            break;
        case 3:
            SIM->SCGC4 |= SIM_SCGC4_UART3_MASK;
            UARTx = UART3;
            break;
        case 4:
            SIM->SCGC1 |= SIM_SCGC1_UART4_MASK;
            UARTx = UART4;
            break;
        default:
            UARTx = NULL;
            break;
    }
    //���ô���Ƶ��
		CLOCK_GetClockFrequency(kBusClock, &clock);
    if((uint32_t)UARTx == UART0_BASE||(uint32_t)UARTx == UART1_BASE) 
    {
        CLOCK_GetClockFrequency(kCoreClock, &clock); //UART0 UART1ʹ��CoreClock
    }
    sbr = (uint16_t)((clock)/((UART_InitStruct->UART_BaudRate)*16));
    brfa = ((clock*2)/(UART_InitStruct->UART_BaudRate)-(sbr*32));
    UARTx->BDH |= ((sbr>>8)&UART_BDH_SBR_MASK);//���ø�5λ������
    UARTx->BDL = (sbr&UART_BDL_SBR_MASK);//���õ�8λ����
    UARTx->C4 |= brfa&(UART_BDL_SBR_MASK>>3);//����С��λ
    //����uart���ƼĴ�����ʵ�ֻ����İ�λ���书��
    UARTx->C2 &= ~(UART_C2_RE_MASK|UART_C2_TE_MASK);	 //��ֹ���ͽ���
    UARTx->C1 &= ~UART_C1_M_MASK;                      //��������λ��Ϊ8λ
    UARTx->C1 &= ~(UART_C1_PE_MASK);                   //����Ϊ����żУ��λ
    UARTx->S2 &= ~UART_S2_MSBF_MASK;                   //����Ϊ���λ���ȴ���
    //ʹ�ܽ������뷢����
    UARTx->C2|=(UART_C2_RE_MASK|UART_C2_TE_MASK);	 //�������ݷ��ͽ���,�μ��ֲ�1221ҳ
    //PinMux Config
    for(i = 0; i < pUART_Map->m_PinCnt; i++)
    {
        PinMuxConfig(pUART_Map->m_PortIndex, pUART_Map->m_PinBase+i, pUART_Map->m_Mux);
    }
}


void UART_Cmd(UART_Type* UARTx, FunctionalState NewState)
{
	(ENABLE == NewState)?(UARTx->C2 |= (UART_C2_RE_MASK|UART_C2_TE_MASK)):(UARTx->C2 &= ~(UART_C2_RE_MASK|UART_C2_TE_MASK));
}

/***********************************************************************************************
 ���ܣ������жϿ���
 �βΣ�UART_Type ����ѡ��
			 @arg  UART0: ����0
			 @arg  UART1: ����1
			 @arg  UART2: ����2
			 @arg  UART3: ����3
			 @arg  UART4: ����4

			 UART_IT : ֧�ֵ��ж�
 ���أ�0
 ��⣺0
************************************************************************************************/
void UART_ITConfig(UART_Type* UARTx, UART_ITSelect_TypeDef UART_IT, FunctionalState NewState)
{
    IRQn_Type  IRQn;
    //�������
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    //�ҳ���Ӧ��UART�˿�
    switch((uint32_t)UARTx)
    {
        case (uint32_t)UART0:
            IRQn = UART0_RX_TX_IRQn;
            break;
        case (uint32_t)UART1:
            IRQn = UART1_RX_TX_IRQn;
            break;
        case (uint32_t)UART2:
            IRQn = UART2_RX_TX_IRQn;
            break;
        case (uint32_t)UART3:
            IRQn = UART3_RX_TX_IRQn;
            break;
        case (uint32_t)UART4:
            IRQn = UART4_RX_TX_IRQn;
            break;
        default:
            break;
    }
    switch(UART_IT)
    {
        case kUART_IT_TDRE:
            (ENABLE == NewState)?(UARTx->C2 |= UART_C2_TIE_MASK):(UARTx->C2 &= ~UART_C2_TIE_MASK);
            break;
        case kUART_IT_TC:
            (ENABLE == NewState)?(UARTx->C2 |= UART_C2_TCIE_MASK):(UARTx->C2 &= ~UART_C2_TCIE_MASK);
            break;
        case kUART_IT_RDRF:
            (ENABLE == NewState)?(UARTx->C2 |= UART_C2_RIE_MASK):(UARTx->C2 &= ~UART_C2_RIE_MASK);
            break;
        case kUART_IT_IDLE:
            (ENABLE == NewState)?(UARTx->C2 |= UART_C2_ILIE_MASK):(UARTx->C2 &= ~UART_C2_ILIE_MASK);
            break;
        default:break;
    }
		(ENABLE == NewState)?(NVIC_EnableIRQ(IRQn)):(NVIC_DisableIRQ(IRQn));
}
/***********************************************************************************************
 ���ܣ�����жϱ�־
 �βΣ�UART_Type ����ѡ��
			 @arg  UART0: ����0
			 @arg  UART1: ����1
			 @arg  UART2: ����2
			 @arg  UART3: ����3
			 @arg  UART4: ����4

			 UART_IT : ֧�ֵ��ж�
 ���أ�0
 ��⣺0
************************************************************************************************/
ITStatus UART_GetITStatus(UART_Type* UARTx, UART_ITSelect_TypeDef UART_IT)
{
    ITStatus retval;
    //�������
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    
    switch(UART_IT)
    {
        case kUART_IT_TDRE:
            (UARTx->S1 & UART_S1_TDRE_MASK)?(retval = SET):(retval = RESET);
            break;
        case kUART_IT_TC:
            (UARTx->S1 & UART_S1_TC_MASK)?(retval = SET):(retval = RESET);
            break;
        case kUART_IT_RDRF:
            (UARTx->S1 & UART_S1_RDRF_MASK)?(retval = SET):(retval = RESET);	
            break;
        case kUART_IT_IDLE:
            (UARTx->S1 & UART_S1_IDLE_MASK)?(retval = SET):(retval = RESET);			
            break;
        default:break;
    }
    return retval;
}


/***********************************************************************************************
 ���ܣ����ڷ���һ���ֽ�
 �βΣ�UART_Type ����ѡ��
			 @arg  UART0: ����0
			 @arg  UART1: ����1
			 @arg  UART2: ����2
			 @arg  UART3: ����3
			 @arg  UART4: ����4

			 Data : 0-0xFF ���͵�����
 ���أ�0
 ��⣺0
************************************************************************************************/
void UART_SendByte(UART_Type* UARTx, uint8_t Data)
{
    while(!(UARTx->S1 & UART_S1_TDRE_MASK));
    UARTx->D = (uint8_t)Data;
}

void UART_SendByte_Async(UART_Type* UARTx, uint8_t Data)
{
    UARTx->D = (uint8_t)Data;
}

/***********************************************************************************************
 ���ܣ�����UART DMA֧��s
 �βΣ�UART_Type ����ѡ��
			 @arg  UART0: ����0
			 @arg  UART1: ����1
			 @arg  UART2: ����2
			 @arg  UART3: ����3
			 @arg  UART4: ����4

			 UART_DMAReq : DMA�ж�Դ

			 NewState    : ʹ�ܻ��߹ر�
			 @arg  ENABLE : ʹ��
			 @arg  DISABLE: ��ֹ
 ���أ�0
 ��⣺��ҪDMA������֧�� ��Ҫʹ��DMA�����е� Iscomplete�����ж��Ƿ������
************************************************************************************************/
void UART_DMACmd(UART_Type* UARTx, UART_DMAReq_Select_TypeDef UART_DMAReq, FunctionalState NewState)
{
    //�������
    assert_param(IS_UART_ALL_PERIPH(UARTx));
	
    switch(UART_DMAReq)
    {
        case kUART_DMAReq_TX:
            (ENABLE == NewState)?(UARTx->C5 |= UART_C5_TDMAS_MASK):(UARTx->C5 &= ~UART_C5_TDMAS_MASK);
            (ENABLE == NewState)?(UARTx->C2 |= UART_C2_TIE_MASK):(UARTx->C2 &= ~UART_C2_TIE_MASK);	
            break;
        case kUART_DMAReq_RX:
            (ENABLE == NewState)?(UARTx->C5 |= UART_C5_RDMAS_MASK):(UARTx->C5 &= ~UART_C5_RDMAS_MASK);
				    (ENABLE == NewState)?(UARTx->C2 |= UART_C2_RIE_MASK):(UARTx->C2 &= ~UART_C2_RIE_MASK);	
            break;
        default:break;
    }
}
/***********************************************************************************************
 ���ܣ�ʹ���жϷ��ʹ�������
 �βΣ�UART_Type ����ѡ��
			 @arg  UART0: ����0
			 @arg  UART1: ����1
			 @arg  UART2: ����2
			 @arg  UART3: ����3
			 @arg  UART4: ����4

			 *DataBuf : ���͵����� ������ָ��
			  Len     : ���͵����ݳ���
 ���أ�0
 ��⣺0
************************************************************************************************/
/*
void UART_SendDataInt(UART_Type* UARTx,uint8_t* pBuffer,uint8_t NumberOfBytes)
{
    //�������
    assert_param(IS_UART_ALL_PERIPH(UARTx));
	
    //�ڴ濽��
    memcpy(UART_TxIntStruct1.TxBuf,pBuffer,NumberOfBytes);
    UART_TxIntStruct1.Length = NumberOfBytes;
    UART_TxIntStruct1.Offset = 0;
    UART_TxIntStruct1.IsComplete = FALSE;
    //ʹ���жϷ�ʽ���� ��ʹ��DMA
    UARTx->C5 &= ~UART_C5_TDMAS_MASK; 
    //ʹ�ܴ����ж�
    UARTx->C2 |= UART_C2_TIE_MASK;
}
*/
/***********************************************************************************************
 ���ܣ�ʹ���жϷ�ʽ ���ʹ������� �жϹ���
 �βΣ�UART_Type ����ѡ��
			 @arg  UART0: ����0
			 @arg  UART1: ����1
			 @arg  UART2: ����2
			 @arg  UART3: ����3
			 @arg  UART4: ����4
 ���أ�0
 ��⣺�����������жϷ���ʱ �ڶ�Ӧ�Ĵ����ж��е��ô˹���
************************************************************************************************/
/*
void UART_SendDataIntProcess(UART_Type* UARTx)
{
    if((UARTx->S1 & UART_S1_TDRE_MASK) && (UARTx->C2 & UART_C2_TIE_MASK))
    {
        if(UART_TxIntStruct1.IsComplete == FALSE)
        {
            UARTx->D = UART_TxIntStruct1.TxBuf[UART_TxIntStruct1.Offset++];
            if(UART_TxIntStruct1.Offset >= UART_TxIntStruct1.Length)
            {
                UART_TxIntStruct1.IsComplete = TRUE;
                //�رշ����ж�
                UARTx->C2 &= ~UART_C2_TIE_MASK;
            }
        } 
    }
}
*/
/***********************************************************************************************
 ���ܣ����ڽ���һ���ֽ�
 �βΣ�UART_Type ����ѡ��
			 @arg  UART0: ����0
			 @arg  UART1: ����1
			 @arg  UART2: ����2
			 @arg  UART3: ����3
			 @arg  UART4: ����4

			 *ch : ���յ����ֽ� ����ָ��
 ���أ�0 ����ʧ��
       1 ���ճɹ�
 ��⣺0
************************************************************************************************/


uint8_t UART_ReceiveByte(UART_Type* UARTx, uint8_t *pData)
{
    while((UARTx->S1 & UART_S1_RDRF_MASK) == 0) {};
    *pData = UARTx->D;
    return TRUE;
}

uint8_t UART_ReceiveByte_Async(UART_Type* UARTx, uint8_t *pData)
{
    if((UARTx->S1 & UART_S1_RDRF_MASK)!= 0) //is buffer full
		{
        *pData = (UARTx->D);	//get data
        return TRUE;
		}
		return FALSE;
}

int UART_printf(const char *format,...)
{
    int chars;
    int i;
    va_list ap;
    char printbuffer[UART_PRINTF_CMD_LENGTH];
    va_start(ap, format);
    chars = vsprintf(printbuffer, format, ap);
    va_end(ap);
    for(i=0;i<chars;i++)
    {
        UART_SendByte(UART_DebugPort,printbuffer[i]);
    }
    return chars ;
}

/***********************************************************************************************
 ���ܣ���ӡ��������Ϣ
 �βΣ�0          
 ���أ�0
 ��⣺�����Freescale Kinetisϵ��
************************************************************************************************/
void DisplayCPUInfo(void)
{
    //FwLib Version
    UART_printf("CH_K60 FW_Version:%d\r\n",GetFWVersion());
    //Reset infomation
	/*
    switch(CPUInfo.m_ResetStateCode)
    {
        case kSoftwareReset:
            UART_printf("Software Reset\r\n");
            break;
        case kCoreLockupEventReset: 
            UART_printf("Core Lockup Event Reset\r\n"); 
            break;
        case kJTAGReset: 
            UART_printf("JTAG Reset\r\n");         
            break;
        case kPOReset:
            UART_printf("Power-on Reset\r\n");    
            break;
        case kExternalPinReset: 
            UART_printf("External Pin Reset\r\n");     
            break;
        case kWdogReset:
            UART_printf("Watchdog(COP) Reset\r\n");    
            break;
        case kLossOfClockReset: 
            UART_printf("Loss of Clock Reset\r\n");    
            break;
        case kLowVoltageDetectReset: 
            UART_printf("Low-voltage Detect Reset\r\n");
            break;
        case kLLWUReset: 
            UART_printf("LLWU Reset\r\n");     
            break;
    }
		*/
    //PFlash size
    //UART_printf("PFlash Size: %dKB\r\n",CPUInfo.PFlashSizeInKB);
    //Flex RAM size
    //UART_printf("FlexNVM Size: %dKB\r\n",CPUInfo.FlexNVMSizeInKB);
    //RAM size
    //UART_printf("RAM Size :%dKB\r\n",CPUInfo.RAMSizeInKB);
    //core clock
  //  UART_printf("CoreClock: %dHz\r\n",CPUInfo.CoreClockInHz);
    //bus clock
   // UART_printf("BusClock: %dHz\r\n",CPUInfo.BusClockInHz);
    //flexbus clock
   // UART_printf("FlexBusClock: %dHz\r\n",CPUInfo.FlexBusClockHz);
    //flash clock
  //  UART_printf("FlashClock: %dHz\r\n",CPUInfo.FlashClockHz);
}

/*
static const PeripheralMap_TypeDef UART_Check_Maps[] = 
{ 
    {0, 0, 2, 1, 2}, //UART0_RX_PA01_TX_PA02
    {0, 0, 3,14, 2}, //UART0_RX_PA14_TX_PA15
    {0, 1, 3,16, 2}, //UART0_RX_PB16_TX_PB17
    {0, 3, 3, 6, 2}, //UART0_RX_PD06_TX_PD07
    {1, 4, 3, 0, 2}, //UART1_RX_PE00_TX_PE01
    {1, 2, 3, 3, 2}, //UART1_RX_PC03_TX_PC04
    {2, 3, 3, 2, 2}, //UART2_RX_PD02_TX_PD03
    {3, 1, 3,10, 2}, //UART3_RX_PB10_TX_PB11
    {3, 2, 3,16, 2}, //UART3_RX_PC16_TX_PC17
    {3, 4, 3, 4, 2}, //UART3_RX_PE04_TX_PE05
    {4, 4, 3,24, 2}, //UART4_RX_PE24_TX_PE25
    {4, 2, 3,14, 2}, //UART4_RX_PC14_TX_PC15
    {0, 0, 0, 0, 0}, //UART0_USER_DEFINE
    {1, 0, 0, 0, 0}, //UART1_USER_DEFINE
    {2, 0, 0, 0, 0}, //UART2_USER_DEFINE
    {3, 0, 0, 0, 0}, //UART3_USER_DEFINE
    {4, 0, 0, 0, 0}, //UART4_USER_DEFINE
};

void CalConstValue(void)
{
	uint8_t i = 0;
	uint32_t value = 0;
	for(i = 0; i < sizeof(UART_Check_Maps)/sizeof(PeripheralMap_TypeDef); i++)
	{
		value = UART_Check_Maps[i].m_ModuleIndex<<0;
		value |= UART_Check_Maps[i].m_PortIndex<<3;
		value |= UART_Check_Maps[i].m_Mux<<6;
		value |= UART_Check_Maps[i].m_PinBase<<9;
		value |= UART_Check_Maps[i].m_PinCnt<<14;
		value |= UART_Check_Maps[i].m_Channel<<17;
		UART_printf("(0x%08XU)\r\n",value);
	}
}
*/


//! @}

//! @}

//! @}

