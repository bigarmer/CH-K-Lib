/**
  ******************************************************************************
  * @file    can.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.4.10
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ���ļ�ΪоƬCANģ��ĵײ㹦�ܺ���������Ӧ����鿴ʵ������
             �˵ײ��������������canͨ��ģʽ����֧�������ȳ�����
  ******************************************************************************
  */
  
#include "can.h"
#include "gpio.h"

#if (!defined(CAN_BASES))

    #if (defined(MK60DZ10))
        #define CAN_BASES {CAN0, CAN1}
    #endif
#endif
/* global vars */
CAN_Type * const CAN_InstanceTable[] = CAN_BASES;
static const struct reg_ops SIM_CANClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FLEXCAN0_MASK},
    {(void*)&(SIM->SCGC3), SIM_SCGC3_FLEXCAN1_MASK},
}; 
        
        

/**
 * @brief  ����CANͨѶ����
 * @note   �ڲ����� BucClock������50M  ����ͨ���ٶȲ�׼ȷ
 * @param  can           :����ͨѶ��������
 * @param  baudrate      :CANģ���ͨ���ٶ�
 *         @arg CAN_SPEED_20K  :����ͨ���ٶ�Ϊ20K  �������3300m
 *         @arg CAN_SPEED_50K  :����ͨ���ٶ�Ϊ50K  
 *         @arg CAN_SPEED_100K :����ͨ���ٶ�Ϊ100K  
 *         @arg CAN_SPEED_125K :����ͨ���ٶ�Ϊ125K  
 *         @arg CAN_SPEED_250K :����ͨ���ٶ�Ϊ250K  
 *         @arg CAN_SPEED_500K :����ͨ���ٶ�Ϊ500K  
 *         @arg CAN_SPEED_1M   :����ͨ���ٶ�Ϊ1M   �������40m
 * @retval 0:�ɹ���1:ʧ��
 */
/***********************************************************************************************
 ���ܣ�
 �βΣ�can : CAN0 ����  CAN1
			CAN_BAUDRATE_SELECT baudrate:
					
					,
					,
					,
					,
					,
					,  
 ���أ�0�ɹ�  1ʧ��
 ���
************************************************************************************************/
static uint32_t CAN_SetBaudrate(CAN_Type *can, CAN_Baudrate_Type baudrate)
{
    switch(baudrate)
    {
        case kCAN_Baudrate_25K:
			 // 50M/125 = 400k sclock, 16Tq
			 // PROPSEG = 5, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 5, PSEG2 = 5,PRESDIV = 125
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(4) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(4) 
							 | CAN_CTRL1_PSEG2(4)
							 | CAN_CTRL1_PRESDIV(124));
            break;
		case kCAN_Baudrate_50K:
			 // 50M/100= 500K sclock, 10Tq
			 // PROPSEG = 3, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 3, PSEG2 = 3, PRESDIV = 100
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(99));	
            break;
		case kCAN_Baudrate_100K:
			 // 50M/50= 1M sclock, 10Tq
			 // PROPSEG = 3, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 3, PSEG2 = 3, PRESDIV = 50
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(49));	
            break;
		case kCAN_Baudrate_125K:
			 // 50M/25 = 2000k sclock, 16Tq
			 // PROPSEG = 5, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 5, PSEG2 = 5,PRESDIV = 25
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(4) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(4) 
							 | CAN_CTRL1_PSEG2(4)
							 | CAN_CTRL1_PRESDIV(24));	
            break;
		case kCAN_Baudrate_250K:
			 // 50M/20= 2500K sclock, 10Tq
			 // PROPSEG = 3, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 3, PSEG2 = 3, PRESDIV = 20
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(19));			
            break;
		case kCAN_Baudrate_500K:
			 // 50M/10= 5000K sclock, 10Tq
			 // PROPSEG = 3, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 3, PSEG1 = 3, PSEG2 = 3, PRESDIV = 20
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(9));				
            break;
		case kCAN_Baudrate_1000K:
			 // 50M/5= 10000K sclock, 10Tq
			 // PROPSEG = 2, LOM = 0x0, LBUF = 0x0, TSYNC = 0x0, SAMP = 1
			 // RJW = 2, PSEG1 = 3, PSEG2 = 3, PRESDIV = 20
			can->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(2) 
							 | CAN_CTRL1_RJW(2)
							 | CAN_CTRL1_PSEG1(2) 
							 | CAN_CTRL1_PSEG2(2)
							 | CAN_CTRL1_PRESDIV(4));	
            break;
		default: 
            return 1;
	}
	return 0;
}
/***********************************************************************************************
 ���ܣ����������ж�����
 �βΣ�can : CAN0  CAN1
 ���أ�0
 ��⣺�ڲ�����
************************************************************************************************/
void CAN_Set_Rev_Mask(CAN_Type *can, uint32_t rxid)
{
    can->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
    	// �ȴ�ģ����붳��ģʽ
	while(!(CAN_MCR_FRZACK_MASK & (can->MCR))); 
    if(rxid>0x7FF)
    {	  //ʶ��Ϊ��չid
        can->RXMGMASK = CAN_ID_EXT(rxid); 
    }
    else
    {	//ʶ��Ϊ��׼11λid
        can->RXMGMASK = CAN_ID_STD(rxid); 
    } 
    can->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK); //ȡ����ͣ 
	while((CAN_MCR_FRZACK_MASK & (can->MCR)));   // �ȴ�ģ���Ƴ�����ģʽ
}
/***********************************************************************************************
 ���ܣ�����CAN��������
 �βΣ�can : CAN0 ����  CAN1
       boxindex��0~15
       rxid��׼����������Դ�ĵ�ַ
 ���أ�0
 ��⣺�����жϷ�ʽ����
************************************************************************************************/
void CAN_EnableRev(CAN_Type *can, uint8_t boxindex, uint32_t rxid)
{
	CAN_Set_Rev_Mask(can,rxid);
    can->IMASK1 |= CAN_IMASK1_BUFLM(1<<boxindex);  //���������ж�
    // д�� ID
    if(rxid>0x7FF)
    {	  //ʶ��Ϊ��չid   
        can->MB[boxindex].CS |= CAN_CS_IDE_MASK;
        can->MB[boxindex].CS &= ~CAN_CS_SRR_MASK;
        can->MB[boxindex].ID &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
        can->MB[boxindex].ID |= (rxid & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
    }
    else
    {	//ʶ��Ϊ��׼11λid
        can->MB[boxindex].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);
        can->MB[boxindex].ID &= ~CAN_ID_STD_MASK;
        can->MB[boxindex].ID |= CAN_ID_STD(rxid);       
    } 
    can->MB[boxindex].CS &= ~CAN_CS_CODE_MASK; // 
	can->MB[boxindex].CS |= CAN_CS_CODE(4); // ������� MB ��Ϊ��������
}

void CAN_Init(CAN_InitTypeDef* CAN_InitStruct)
{
    uint32_t i;
    /* enable clock gate */
    *((uint32_t*) SIM_CANClockGateTable[CAN_InitStruct->instance].addr) |= SIM_CANClockGateTable[CAN_InitStruct->instance].mask; 
    /* set clock source is bus clock */
    CAN_InstanceTable[CAN_InitStruct->instance]->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;
    /* enable module */
    CAN_InstanceTable[CAN_InitStruct->instance]->MCR &= ~CAN_MCR_MDIS_MASK;
    /* software reset */
	CAN_InstanceTable[CAN_InitStruct->instance]->MCR |= CAN_MCR_SOFTRST_MASK;	
	while(CAN_MCR_SOFTRST_MASK & (CAN_InstanceTable[CAN_InitStruct->instance]->MCR)); 
    /* init all mb */
    for(i = 0; i < 16; i++)
	{
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].CS = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].ID = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].WORD0 = 0x00000000;
		CAN_InstanceTable[CAN_InitStruct->instance]->MB[i].WORD1 = 0x00000000;
	}
	//������������
	CAN_InstanceTable[CAN_InitStruct->instance]->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
    CAN_InstanceTable[CAN_InitStruct->instance]->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
    CAN_InstanceTable[CAN_InitStruct->instance]->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK); 
    
   //���ô�������
	CAN_SetBaudrate(CAN_InstanceTable[CAN_InitStruct->instance], CAN_InitStruct->baudrate);
	CAN_InstanceTable[CAN_InitStruct->instance]->MCR &= ~(CAN_MCR_FRZ_MASK| CAN_MCR_HALT_MASK); //ȡ����ͣ 
	while((CAN_MCR_FRZACK_MASK & (CAN_InstanceTable[CAN_InitStruct->instance]->MCR)));   // �ȴ�ģ���Ƴ�����ģʽ
	while(((CAN_InstanceTable[CAN_InitStruct->instance]->MCR)&CAN_MCR_NOTRDY_MASK));    // �ȴ�ͬ�������ο��ֲ�k10 1046ҳ 

}

void CAN_QuickInit(uint32_t CANxMAP, CAN_Baudrate_Type baudrate)
{
	uint32_t i;
    QuickInit_Type * pq = (QuickInit_Type*)&(CANxMAP); 
    CAN_InitTypeDef CAN_InitSturct1;
    CAN_InitSturct1.instance = pq->ip_instance;
    CAN_InitSturct1.baudrate = baudrate;
    CAN_Init(&CAN_InitSturct1);
    /* init pinmux */
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux); 
    }
}

uint32_t CAN_SendData2(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len)
{
	if(mb >= 16 || len >8)
	{
		return 1; //�������ݴ���
	} 
}

/*
void CAN_Init2(uint32_t CANxMAP, CAN_Baudrate_Type baudrate)
{
	uint8_t i;
    CAN_Type *can;
	//��¼CANdev������
	//����CANʱ�� ���ö�Ӧ����
     QuickInit_Type * pq = (QuickInit_Type*)&(CANxMAP);
    
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux); 
    }
	if((pq->ip_instance) == HW_CAN0)
	{
		can = CAN0;
        SIM->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;	 //����CAN0��ʱ��
		NVIC_EnableIRQ(CAN0_ORed_Message_buffer_IRQn);//�����ж�
	}
	if((pq->ip_instance) == HW_CAN1)
	{
		can = CAN1;
        SIM->SCGC3 |= SIM_SCGC3_FLEXCAN1_MASK;	//����can1�ĵ�ʱ��
		NVIC_EnableIRQ(CAN1_ORed_Message_buffer_IRQn);//�����ж�
	}
	//ѡ��ΪBusClock ʱ��
	can->CTRL1 |=  CAN_CTRL1_CLKSRC_MASK;	
	//����CANͨ��ģ�� 
	can->MCR &= ~CAN_MCR_MDIS_MASK;	
	//�ȴ�ͨ��ģ�鸴λ����
	while((CAN_MCR_LPMACK_MASK & (can->MCR)));	 
	//�����λģ�� //�ȴ������λ���
	can->MCR |= CAN_MCR_SOFTRST_MASK;	
	while(CAN_MCR_SOFTRST_MASK & (can->MCR));  
    //ʹ�ܶ���ģʽ
	can->MCR |= (CAN_MCR_FRZ_MASK| CAN_MCR_HALT_MASK);  
	// �ȴ�ģ����붳��ģʽ
	while(!(CAN_MCR_FRZACK_MASK & (can->MCR))); 
    //��ʼ����������
	for(i=0;i<16;i++)
	{
		can->MB[i].CS = 0x00000000;
		can->MB[i].ID = 0x00000000;
		can->MB[i].WORD0 = 0x00000000;
		can->MB[i].WORD1 = 0x00000000;
	}
	//������������
	can->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
    can->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
    can->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK); 
   //���ô�������
	CAN_SetBaudrate(can,baudrate);
	can->MCR &= ~(CAN_MCR_FRZ_MASK| CAN_MCR_HALT_MASK); //ȡ����ͣ 
	while((CAN_MCR_FRZACK_MASK & (can->MCR)));   // �ȴ�ģ���Ƴ�����ģʽ
	while(((can->MCR)&CAN_MCR_NOTRDY_MASK));    // �ȴ�ͬ�������ο��ֲ�k10 1046ҳ 
}
*/
/***********************************************************************************************
 ���ܣ�CAN �豸��������
 �βΣ�can : CAN0 ����  CAN1
       Data: ����ָ�� 
 ���أ�0
 ���: ����������󲻳��� 8�ֽ�
************************************************************************************************/
uint8_t CAN_SendData(CAN_Type *can,uint8_t boxindex, uint32_t txid, uint8_t *Data, uint8_t len)
{
	uint8_t  i;
	uint32_t word[2] = {0};
    can->IMASK1 |= CAN_IMASK1_BUFLM(1<<boxindex);  //���������ж�
	if(boxindex >= 16 || len >8)
	{
		return 1; //�������ݴ���
	}
	// ת�����ݸ�ʽ
	for(i=0;i<len;i++)
	{
		if(i<4)
		word[0] |= (*(Data+i)<<((3-i)*8));
	   else
		word[1] |= (*(Data+i)<<((7-i)*8));
	} 
    can->MB[boxindex].WORD0 = word[0];
	can->MB[boxindex].WORD1 = word[1]; 
	//����id����
	if(txid>0x7FF)
    {	  //ʶ��Ϊ��չid
        can->MB[boxindex].ID &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);  
        can->MB[boxindex].ID |= (txid & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
        can->MB[boxindex].CS |= CAN_CS_IDE_MASK;
        can->MB[boxindex].CS &= ~CAN_CS_SRR_MASK;
        can->MB[boxindex].CS &= ~CAN_CS_DLC_MASK;
        can->MB[boxindex].CS |= CAN_CS_DLC(len);
        can->MB[boxindex].CS &= ~CAN_CS_CODE_MASK;
        can->MB[boxindex].CS |= CAN_CS_CODE(12);	 // ������� MB ��������
    }
    else
    {	//ʶ��Ϊ��׼11λid
        can->MB[boxindex].ID &= ~CAN_ID_STD_MASK ;  
        can->MB[boxindex].ID |= CAN_ID_STD(txid);
        can->MB[boxindex].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);
        can->MB[boxindex].CS &= ~CAN_CS_DLC_MASK;
        can->MB[boxindex].CS |= CAN_CS_DLC(len);
        can->MB[boxindex].CS &= ~CAN_CS_CODE_MASK;
        can->MB[boxindex].CS |= CAN_CS_CODE(12);	 // ������� MB ��������  
    } 
    return 0;
}  
/***********************************************************************************************
 ���ܣ�CAN��������
 �βΣ�can : CAN0 ����  CAN1
       boxindex:������0~15
       *data : �������ݻ�����
 ���أ����յ������ݳ���
 ��⣺
************************************************************************************************/
uint8_t CAN_ReadData(CAN_Type *can,uint8_t boxindex, uint8_t *Data)
{
	uint8_t len = 0;
	uint8_t code,i;
	uint32_t word[2] = {0};
    // �� MB
    code = CAN_get_code(can->MB[boxindex].CS);
    if(code != 0x02) //��֤�Ƿ���ճɹ�
    {
        len = 0;
        return len;
    }
    len = CAN_get_length(can->MB[boxindex].CS);  //��ý��յ����ݳ���
    if(len <1)
    {
        return len; //����ʧ��
    }
	word[0] = can->MB[boxindex].WORD0;   //��ȡ���յ�����
	word[1] = can->MB[boxindex].WORD1;   //��ȡ���յ�����
	code = can->TIMER;    // ȫ�ֽ��� MB ����
	//�屨�Ļ������жϱ�־
	can->IFLAG1 = (1<<boxindex);	 //�������
	for(i=0;i<len;i++)
    {  
	   if(i<4)
	   (*(Data+i))=(word[0]>>((3-i)*8));
	   else									 //���ݴ洢ת��
	   (*(Data+i))=(word[1]>>((7-i)*8));
    }
    return len;
}
/***********************************************************************************************
 ���ܣ�CANʹ�ܽ����ж�
 �βΣ�can : CAN0 ����  CAN1
 ���أ�0
 ��⣺0
************************************************************************************************/
void CAN_AbleInterrupts(CAN_Type * can)
{
	switch((uint32_t)can)
	{
		case CAN0_BASE:
			NVIC_EnableIRQ(CAN0_ORed_Message_buffer_IRQn);
			break;
		case CAN1_BASE:
			NVIC_EnableIRQ(CAN1_ORed_Message_buffer_IRQn);
			break;
		default:break;
	}
}
/***********************************************************************************************
 ���ܣ�CAN��ֹ�����ж�
 �βΣ�can : CAN0 ����  CAN1
 ���أ�0
 ��⣺0
************************************************************************************************/
void CAN_DisableInterrupts(CAN_Type * can)
{
	switch((uint32_t)can)
	{
		case CAN0_BASE:
			NVIC_DisableIRQ(CAN0_ORed_Message_buffer_IRQn);
			break;
		case CAN1_BASE:
			NVIC_DisableIRQ(CAN1_ORed_Message_buffer_IRQn);
			break;
		default:break;
	}
}
/***********************************************************************************************
 ���ܣ�CAN����жϱ�־λ
 �βΣ�can : CAN0 ����  CAN1
 ���أ�0
 ��⣺0
************************************************************************************************/
void CAN_ClearRecFlag(CAN_Type * can, uint8_t boxindex)
{
	can->IFLAG1 |= (1<<boxindex);	 //�������
}

//extern uint8_t CANRXBuffer[8];
//extern uint32_t CAN_Data_Len;
/***********************************************************************************************
 ���ܣ�CAN0 �����ж�
 �βΣ�0
 ���أ�0
 ��⣺0
************************************************************************************************/
void CAN0_ORed_Message_buffer_IRQHandler(void)
{  
    CAN_ClearRecFlag(CAN0,1); //������ձ�־λ
}
/***********************************************************************************************
 ���ܣ�CAN1 �����ж�
 �βΣ�0
 ���أ�0
 ��⣺0
************************************************************************************************/
void CAN1_ORed_Message_buffer_IRQHandler(void)
{  
    
   if((CAN1->IFLAG1)&(1<<CAN_DEFAULT_RXMSGBOXINDEX))
   {
//     CAN_Data_Len = CAN_ReadData(CAN1,CAN_DEFAULT_RXMSGBOXINDEX,CANRXBuffer); //��������
     CAN_ClearRecFlag(CAN1,CAN_DEFAULT_RXMSGBOXINDEX); //������ձ�־λ
   }
   if((CAN1->IFLAG1)&(0x01<<8))
   {
    CAN_ClearRecFlag(CAN1,8); //������ձ�־λ
   }
    
}


