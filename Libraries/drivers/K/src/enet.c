/**
  ******************************************************************************
  * @file    enet.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
  
#include "enet.h"
#include "common.h"
#include "clock.h"


//������̫��DMA������
static  uint8_t xENETTxDescriptors_unaligned[ ( 1 * sizeof( NBUF ) ) + 16 ];
static  uint8_t pxENETRxDescriptors_unaligned[ ( CFG_NUM_ENET_RX_BUFFERS * sizeof( NBUF ) ) + 16 ];
static NBUF *pxENETTxDescriptor;
static NBUF *pxENETRxDescriptors;


//��̫�����ջ�����
static uint8_t ucENETRxBuffers[ ( CFG_NUM_ENET_RX_BUFFERS * CFG_ENET_BUFFER_SIZE ) + 16 ];

/***********************************************************************************************
 ���ܣ���̫����������ʼ��
 �βΣ�0
 ���أ�0
 ��⣺��̫��ģ��ͨ�� ������ ������(����USB) ��������̫��
************************************************************************************************/
static void ENET_BDInit(void)
{
    unsigned long ux;
    unsigned char *pcBufPointer;
	// ΪTx ������Ѱ��16�ֽڶ���ռ�
	pcBufPointer = &( xENETTxDescriptors_unaligned[ 0 ] );
	while( ( ( uint32_t ) pcBufPointer & 0x0fUL ) != 0 )
	{
		pcBufPointer++;
	}
	pxENETTxDescriptor = ( NBUF * ) pcBufPointer;	
	//ΪRx ������Ѱ��16�ֽڶ���ռ�
	pcBufPointer = &( pxENETRxDescriptors_unaligned[ 0 ] );
	while( ( ( uint32_t ) pcBufPointer & 0x0fUL ) != 0 )
	{
		pcBufPointer++;
	}
	pxENETRxDescriptors = ( NBUF * ) pcBufPointer;
    // Tx��������ʼ��
	for( ux = 0; ux < CFG_NUM_ENET_TX_BUFFERS; ux++ )
	{
        pxENETTxDescriptor[ux].length = 0;
        pxENETTxDescriptor[ux].status = 0;
        pxENETTxDescriptor[ux].ebd_status = TX_BD_IINS | TX_BD_PINS;   
    }

	//ΪRx������Ѱ��16�ֽڶ���ռ�
	pcBufPointer = &( ucENETRxBuffers[ 0 ] );
	while((( uint32_t ) pcBufPointer & 0x0fUL ) != 0 )
	{
		pcBufPointer++;
	}
	//��ʼ������������
	for( ux = 0; ux < CFG_NUM_ENET_RX_BUFFERS; ux++ )
	{
	    pxENETRxDescriptors[ ux ].status = RX_BD_E;
	    pxENETRxDescriptors[ ux ].length = 0;
        pxENETRxDescriptors[ ux ].data = (uint8_t *)__REV((uint32_t)pcBufPointer);
	    pcBufPointer += CFG_ENET_BUFFER_SIZE;
	    pxENETRxDescriptors[ ux ].bdu = 0x00000000;
	    pxENETRxDescriptors[ ux ].ebd_status = RX_BD_INT;
	}
	//���һ������������ΪWarp
    pxENETTxDescriptor[CFG_NUM_ENET_TX_BUFFERS - 1].status |= TX_BD_W;
	pxENETRxDescriptors[CFG_NUM_ENET_RX_BUFFERS - 1].status |= RX_BD_W;
}

/***********************************************************************************************
 ���ܣ�����MAC��ַ
 �βΣ�0
 ���أ�0
 ��⣺0
************************************************************************************************/
uint8_t ENET_HashAddress(const uint8_t* addr)
{
    uint32_t crc;
    uint8_t byte;
    int i, j;
    crc = 0xFFFFFFFF;
    for(i=0; i < 6; ++i)
    {
        byte = addr[i];
        for(j = 0; j < 8; ++j)
        {
            if((byte & 0x01)^(crc & 0x01))
            {
                crc >>= 1;
                crc = crc ^ 0xEDB88320;
            }
        else
            crc >>= 1;
        byte >>= 1;
    }
    }
    return (uint8_t)(crc >> 26);
}
/***********************************************************************************************
 ���ܣ�����MAC��ַ
 �βΣ�0
 ���أ�0
 ��⣺0
************************************************************************************************/
void ENET_SetAddress(const uint8_t *pa)
{
    uint8_t crc;
    //���������ַ
    ENET->PALR = (uint32_t)((pa[0]<<24) | (pa[1]<<16) | (pa[2]<<8) | pa[3]);
    ENET->PAUR = (uint32_t)((pa[4]<<24) | (pa[5]<<16));
  
    //���������ַ���㲢���ö�����ַ��ϣ�Ĵ�����ֵ
    crc = ENET_HashAddress(pa);
    if(crc >= 32)
    ENET->IAUR |= (uint32_t)(1 << (crc - 32));
    else
    ENET->IALR |= (uint32_t)(1 << crc);
}

/***********************************************************************************************
 ���ܣ���ʼ����̫��ģ��
 �βΣ�0
 ���أ�0�ɹ� 1ʧ��
 ��⣺0
************************************************************************************************/
void ENET_Init(ENET_InitTypeDef* ENET_InitStrut)
{
	uint16_t usData;
	uint16_t timeout = 0;
    //ʹ��ENETʱ��
    SIM->SCGC2 |= SIM_SCGC2_ENET_MASK;
    //����������MPU������
    MPU->CESR = 0;         
    //��������������ʼ��
    ENET_BDInit();
	//��PORTʱ��
	SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;
	//����Ҫ����
	MCG->C2 &= ~MCG_C2_EREFS_MASK;
	//��λ��̫��
	ENET->ECR = ENET_ECR_RESET_MASK;
	for( usData = 0; usData < 100; usData++ )
	{
		__NOP();
	}
    //��ʼ��MII�ӿ�
    ENET_MII_Init();  

	//�����ж�
	NVIC_EnableIRQ(ENET_Transmit_IRQn);
	NVIC_EnableIRQ(ENET_Receive_IRQn);
	NVIC_EnableIRQ(ENET_Error_IRQn);
    //ʹ��GPIO���Ÿ��ù���
    PORTB->PCR[0]  = PORT_PCR_MUX(4); 
    PORTB->PCR[1]  = PORT_PCR_MUX(4); 
    PORTA->PCR[5]  = PORT_PCR_MUX(4); 
	PORTA->PCR[12] =  PORT_PCR_MUX(4);  
	PORTA->PCR[13] =  PORT_PCR_MUX(4);  
	PORTA->PCR[14] =  PORT_PCR_MUX(4);  
	PORTA->PCR[15] =  PORT_PCR_MUX(4);  
	PORTA->PCR[16] =  PORT_PCR_MUX(4);  
	PORTA->PCR[17] =  PORT_PCR_MUX(4);   
    //�ȴ�PHY�շ�����λ���
    do
    {
        DelayMs(10);
        timeout++;
        if(timeout > 500) return ;
        usData = 0xffff;
        ENET_MII_Read(CFG_PHY_ADDRESS, PHY_PHYIDR1, &usData );
    } while( usData == 0xffff );

#ifdef DEBUG
    printf("PHY_PHYIDR1=0x%X\r\n",usData);
    ENET_MII_Read(CFG_PHY_ADDRESS, PHY_PHYIDR2, &usData );
    printf("PHY_PHYIDR2=0x%X\r\n",usData); 
    ENET_MII_Read(CFG_PHY_ADDRESS, PHY_ANLPAR, &usData );
    printf("PHY_ANLPAR=0x%X\r\n",usData);
    ENET_MII_Read(CFG_PHY_ADDRESS, PHY_ANLPARNP, &usData );
    printf("PHY_ANLPARNP=0x%X\r\n",usData);
    ENET_MII_Read(CFG_PHY_ADDRESS, PHY_PHYSTS, &usData );
    printf("PHY_PHYSTS=0x%X\r\n",usData);
    ENET_MII_Read(CFG_PHY_ADDRESS, PHY_MICR, &usData );
    printf("PHY_MICR=0x%X\r\n",usData);
    ENET_MII_Read(CFG_PHY_ADDRESS, PHY_MISR, &usData );
    printf("PHY_MISR=0x%X\r\n",usData);
#endif 
    //��ʼ�Զ�Э��
    ENET_MII_Write(CFG_PHY_ADDRESS, PHY_BMCR, ( PHY_BMCR_AN_RESTART | PHY_BMCR_AN_ENABLE ) );

#ifdef DEBUG
    ENET_MII_Read(CFG_PHY_ADDRESS, PHY_BMCR, &usData );
    printf("PHY_BMCR=0x%X\r\n",usData);
#endif 
  //�ȴ��Զ�Э�����
    do
    {
        DelayMs(100);
		timeout++;
		if(timeout > 30)
        {
            #if DEBUG
            printf("enet phy reset failed\r\n");
            #endif
            return ;
        }
        ENET_MII_Read(CFG_PHY_ADDRESS, PHY_BMSR, &usData );
    } while( !( usData & PHY_BMSR_AN_COMPLETE ) );
    //����Э�̽������ENETģ��
    usData = 0;
    ENET_MII_Read(CFG_PHY_ADDRESS, PHY_STATUS, &usData );	
  
    //������������ַ��ϣ�Ĵ���
    ENET->IALR = 0;
    ENET->IAUR = 0;
    ENET->GALR = 0;
    ENET->GAUR = 0;
    //����ENETģ��MAC��ַ
    ENET_SetAddress(ENET_InitStrut->pMacAddress);
    //���ý��տ��ƼĴ�������󳤶ȡ�RMIIģʽ������CRCУ���
    ENET->RCR = ENET_RCR_MAX_FL(CFG_ENET_MAX_PACKET_SIZE) | ENET_RCR_MII_MODE_MASK | ENET_RCR_CRCFWD_MASK | ENET_RCR_RMII_MODE_MASK;

    //������ͽ��տ���
    ENET->TCR = 0;
    //ͨѶ��ʽ����
    if( usData & PHY_DUPLEX_STATUS )
    {
        //ȫ˫��
        ENET->RCR &= (unsigned long)~ENET_RCR_DRT_MASK;
        ENET->TCR |= ENET_TCR_FDEN_MASK;
		#ifdef DEBUG
        printf("full-duplex\r\n");
		#endif 
    }
    else
    {
        //��˫��
        ENET->RCR |= ENET_RCR_DRT_MASK;
        ENET->TCR &= (unsigned long)~ENET_TCR_FDEN_MASK;
		#ifdef DEBUG
		printf("half-duplex\r\n");
		#endif 
    }
    //ͨ����������
    if( usData & PHY_SPEED_STATUS )
    {
        //10Mbps
        ENET->RCR |= ENET_RCR_RMII_10T_MASK;
    }

    //ʹ����ǿ�ͻ�����������
    ENET->ECR = ENET_ECR_EN1588_MASK;

    //���ý��ջ�������С
    ENET->MRBR = ENET_MRBR_R_BUF_SIZE(CFG_ENET_MAX_PACKET_SIZE);

	//ָ���λ��������������׵�ַ(RX)
	ENET->RDSR = (uint32_t)  pxENETRxDescriptors;

	//ָ���λ��������������׵�ַ(TX)
	ENET->TDSR = (uint32_t) pxENETTxDescriptor;

	//��������жϱ�־
	ENET->EIR = ( uint32_t ) 0xFFFFFFFF;

	//ʹ���ж�
//	ENET->EIMR = ENET_EIR_TXF_MASK | ENET_EIMR_RXF_MASK ;
//    ENET->EIMR |= ENET_EIMR_RXF_MASK;
	//ʹ��MACģ��
	ENET->ECR |= ENET_ECR_ETHEREN_MASK;
    //�������ջ�����Ϊ��
	ENET->RDAR = ENET_RDAR_RDAR_MASK;
}

void ENET_MacSendData(uint8_t *ch, uint16_t len)
{
    //��鵱ǰ���ͻ������������Ƿ����
    while( pxENETTxDescriptor->status & TX_BD_R ) {};
    //���÷��ͻ�����������
    pxENETTxDescriptor->data = (uint8_t *)__REV((uint32_t)ch);		
    pxENETTxDescriptor->length = __REVSH(len);
    pxENETTxDescriptor->bdu = 0x00000000;
	pxENETTxDescriptor->ebd_status = TX_BD_INT | TX_BD_TS;// | TX_BD_IINS | TX_BD_PINS;
	pxENETTxDescriptor->status = ( TX_BD_R | TX_BD_L | TX_BD_TC | TX_BD_W );
    //ʹ�ܷ���
    ENET->TDAR = ENET_TDAR_TDAR_MASK;
}

uint16_t ENET_MacRecData(uint8_t *ch)
{
    uint16_t len = 0;
    //Ѱ�ҷǿյĻ�����������
    if((pxENETRxDescriptors[0].status & RX_BD_E ) == 0)
    {
		//��ȡ����
		len =  __REVSH(pxENETRxDescriptors[0].length);
		memcpy(ch,(uint8_t *)__REV((uint32_t)pxENETRxDescriptors[0].data), len);
		//��ʾ�Ѿ���ȡ�Ļ���������
		pxENETRxDescriptors[0].status |= RX_BD_E;
		ENET->RDAR = ENET_RDAR_RDAR_MASK;
        return len;
	}
	return 0;
}

/**
 * @brief  ��ʼ����̫�� MII���ò� �ӿ�
 *
 * @param  None
 * @retval None
 */
void ENET_MII_Init(void)
{
    uint32_t i;
    uint32_t clock;
	// Enable the ENET clock.
    SIM->SCGC2 |= SIM_SCGC2_ENET_MASK;
    //FSL: allow concurrent access to MPU controller. Example: ENET uDMA to SRAM, otherwise bus error
    MPU->CESR = 0;   
    CLOCK_GetClockFrequency(kBusClock, &clock);
    i = (clock/1000)/1000;
    ENET->MSCR = 0 | ENET_MSCR_MII_SPEED((2*i/5)+1);
}

/**
 * @brief  д����̫�� MII ���ò�����
 *
 * @param   phy_addr    PHYоƬ��ַ
 * @param   reg_addr    �Ĵ�����PHY�ڲ���ƫ�Ƶ�ַ
 * @param   data        ��Ҫд�������
 * @retval  None
 */
uint8_t ENET_MII_Write(uint16_t phy_addr, uint16_t reg_addr, uint16_t data)
{
    uint32_t timeout;
    // clear MII it pending bit
    ENET->EIR |= ENET_EIR_MII_MASK;
    // initiatate the MII Management write
    ENET->MMFR = 0
            | ENET_MMFR_ST(0x01)
            | ENET_MMFR_OP(0x01)
            | ENET_MMFR_PA(phy_addr)
            | ENET_MMFR_RA(reg_addr)
            | ENET_MMFR_TA(0x02)
            | ENET_MMFR_DATA(data);
    // waitting for transfer complete
    for (timeout = 0; timeout < MII_TIMEOUT; timeout++)
    {
        if (ENET->EIR & ENET_EIR_MII_MASK)
        {
            #if DEBUG
            //printf("MII - timeout:%d\r\n", timeout);
            #endif
            break;  
        }
    }
    if(timeout == MII_TIMEOUT)
    {
        return timeout;
    }
    // software clear it
    ENET->EIR |= ENET_EIR_MII_MASK;
    return 0;
}

/**
 * @brief  ����̫�� MII ���ò�����
 *
 * @param   phy_addr    PHYоƬ��ַ
 * @param   reg_addr    �Ĵ�����PHY�ڲ���ƫ�Ƶ�ַ
 * @param   data        ��Ҫ���������
 * @retval  None
 */
uint8_t ENET_MII_Read(uint16_t phy_addr, uint16_t reg_addr, uint16_t *data)
{
    uint32_t timeout;
    // clear MII IT(interrupt) pending bit
    ENET->EIR |= ENET_EIR_MII_MASK;
    // initiatate the MII Management write
    ENET->MMFR = 0
            | ENET_MMFR_ST(0x01)
            | ENET_MMFR_OP(0x02)
            | ENET_MMFR_PA(phy_addr)
            | ENET_MMFR_RA(reg_addr)
            | ENET_MMFR_TA(0x02);
  
	// waitting for transfer complete
    for (timeout = 0; timeout < MII_TIMEOUT; timeout++)
    {
        if (ENET->EIR & ENET_EIR_MII_MASK)
        {
            #if DEBUG
            //printf("MII - timeout:%d\r\n", timeout);
            #endif
            break; 
        }
    }
    if(timeout == MII_TIMEOUT) 
    {
        return timeout;
    }
    // software clear it
    ENET->EIR |= ENET_EIR_MII_MASK;
    *data = ENET->MMFR & 0x0000FFFF;
    return 0;
}


void ENET_Transmit_IRQHandler(void)
{
	ENET->EIR |= ENET_EIMR_TXF_MASK; 
}


uint8_t gEnetFlag = 0;
void ENET_Receive_IRQHandler(void)
{
	ENET->EIR |= ENET_EIMR_RXF_MASK; 
    gEnetFlag = 1;
}













