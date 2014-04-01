/**
  ******************************************************************************
  * @file    ftm.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ���ļ�ΪоƬFTMģ��ĵײ㹦�ܺ���
  ******************************************************************************
  */
#include "ftm.h"
#include "common.h"
#include "gpio.h"
#include <math.h>

/* leagacy support for Kineis Z Version(inital version) */
#if (!defined(FTM_BASES))

    #if (defined(MK60DZ10))
        #define FTM_BASES {FTM0, FTM1, FTM2}
    #endif
    #if (defined(MK10D5))
        #define FTM_BASES {FTM0, FTM1}
    #endif
#endif
        
/* global vars */
FTM_Type * const FTM_InstanceTable[] = FTM_BASES;

#if (defined(MK60DZ10) || defined(MK40D10) || defined(MK60D10)|| defined(MK10D10) || defined(MK70F12) || defined(MK70F15))
static const struct reg_ops SIM_FTMClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM0_MASK},
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM1_MASK},
    {(void*)&(SIM->SCGC3), SIM_SCGC3_FTM2_MASK},
};
#elif (defined(MK10D5))
static const RegisterManipulation_Type SIM_FTMClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM0_MASK},
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM1_MASK},
};

#endif

/* FTM  ģʽѡ�� */
typedef enum
{
	kFTM_Combine,
    kFTM_Complementary,
    kFTM_DualEdgeCapture,
    kFTM_DeadTime,
    kFTM_Sync,
    kFTM_FaultControl,
}FTM_DualChlConfig_Type;

/* static functions declareation */
static void FTM_PWM_SetMode(uint8_t instance, uint8_t chl, FTM_PWM_Mode_Type mode);
void FTM_PWM_InvertPolarity(uint8_t instance, uint8_t chl, uint32_t config);

/**
 * @brief  ��ʼ��FTMģ��ʵ��PWM����
 * @note  ��Ҫ���PORT_PinMuxConfigʹ��
 * @code
 *      //����FTM2ģ���1ͨ������1000HzƵ�ʵı��ض��䷽��
 *       FTM_PWM_InitTypeDef FTM_InitStruct1; //����һ���ṹ��
 *       FTM_InitStruct1.instance = HW_FTM2;  //ѡ��FTM2ģ��
 *       FTM_InitStruct1.frequencyInHZ = 1000;//����Ƶ��Ϊ1000
 *       FTM_InitStruct1.chl = HW_FTM_CH1;    //ѡ��1ͨ��
 *       FTM_InitStruct1.mode = kPWM_EdgeAligned;//����Ϊ���ض���
 *       FTM_PWM_Init(&FTM_InitStruct1); 
 * @endcode         
 * @param  FTM_PWM_InitTypeDef  : FTM������PWMģʽ�µĽṹ��
 *         @arg instance        :FTMģ���
 *         @arg chl             :PWM����ͨ����
 *         @arg frequencyInHZ   :PWM��Ƶ��
 *         @arg mode            :PWM����ģʽ����
 * @retval None
 */
void FTM_PWM_Init(FTM_PWM_InitTypeDef* FTM_InitStruct)
{
    int32_t pres;
    uint8_t ps;
    int32_t i;
    uint32_t clock;
    uint32_t min_val = 0xFFFF;
    /* enable clock gate */
    *(uint32_t*)SIM_FTMClockGateTable[FTM_InitStruct->instance].addr |= SIM_FTMClockGateTable[FTM_InitStruct->instance].mask;
    /* disable FTM, we must set CLKS(0) before config FTM! */
    FTM_InstanceTable[FTM_InitStruct->instance]->SC = 0;
    /* enable to access all register including enhancecd register(FTMEN bit control whather can access FTM enhanced function) */
    FTM_InstanceTable[FTM_InitStruct->instance]->MODE |= FTM_MODE_WPDIS_MASK;
    /* cal ps */
    CLOCK_GetClockFrequency(kBusClock, &clock);
    pres = (clock/FTM_InitStruct->frequencyInHZ)/FTM_MOD_MOD_MASK;
    if((clock/FTM_InitStruct->frequencyInHZ)/pres > FTM_MOD_MOD_MASK)
    {
        pres++;
    }
    for(i = 0; i < 7; i++)
    {
        if((ABS(pres - (1<<i))) < min_val)
        {
            ps = i;
            min_val = ABS(pres - (1<<i));
        }
    }
    if(pres > (1<<ps)) ps++;
    if(ps > 7) ps = 7;
#ifdef LIB_DEBUG
    printf("freq:%dHz\r\n", FTM_InitStruct->frequencyInHZ);
    printf("input_clk:%d\r\n", clock);
    printf("pres:%d\r\n", pres);
#endif
    /* set CNT and CNTIN */
    FTM_InstanceTable[FTM_InitStruct->instance]->CNT = 0;
    FTM_InstanceTable[FTM_InitStruct->instance]->CNTIN = 0;
    /* set modulo */
    FTM_InstanceTable[FTM_InitStruct->instance]->MOD = (clock/(1<<ps))/FTM_InitStruct->frequencyInHZ;
    /* set LOCK bit to load MOD value */
    FTM_InstanceTable[FTM_InitStruct->instance]->PWMLOAD = 0xFFFFFFFF;
#ifdef LIB_DEBUG
    printf("MOD Should be:%d\r\n",  (clock/(1<<ps))/FTM_InitStruct->frequencyInHZ);
    printf("MOD acutall is:%d\r\n", FTM_InstanceTable[FTM_InitStruct->instance]->MOD);
    printf("ps:%d\r\n", ps);
#endif
    /* set FTM clock to system clock */
    FTM_InstanceTable[FTM_InitStruct->instance]->SC &= ~FTM_SC_CLKS_MASK;
    FTM_InstanceTable[FTM_InitStruct->instance]->SC |= FTM_SC_CLKS(1);
    /* set ps, this must be done after set modulo */
    FTM_InstanceTable[FTM_InitStruct->instance]->SC &= ~FTM_SC_PS_MASK;
    FTM_InstanceTable[FTM_InitStruct->instance]->SC |= FTM_SC_PS(ps); 
    /* set FTM mode */
    FTM_PWM_SetMode(FTM_InitStruct->instance, FTM_InitStruct->chl, FTM_InitStruct->mode);
}

//!< ���������ʼ��
/**
 * @brief  ��ʼ��FTMģ��ʵ���������빦��
 * @note   ��Ҫ���PORT_PinMuxConfig�Ⱥ���ʹ��
 * @code
 *      //����FTM2ģ��Ϊ�������뷽ʽA��λ��B��λ��
 *       FTM_PWM_InitTypeDef FTM_QD_InitStruct; //����һ���ṹ��,
 *       FTM_QD_InitStruct.instance = HW_FTM2;  //ѡ��FTM2ģ��
 *       FTM_QD_InitStruct.PHA_Polarity = kFTM_QD_NormalPolarity;      //A��λ��
 *       FTM_QD_InitStruct.PHA_Polarity = kFTM_QD_InvertedPolarity;    //A��λ��
 *       FTM_QD_InitStruct.mode = kQD_PHABEncoding;   //����AB�����
 *       FTM_PWM_Init(&FTM_QD_InitStruct); 
 * @endcode         
 * @param  FTM_QD_InitTypeDef  : FTM��������������ģʽ�µĽṹ��
 * @retval None
 */
void FTM_QD_Init(FTM_QD_InitTypeDef * FTM_QD_InitStruct)
{
    /* enable clock gate */
    *(uint32_t*)SIM_FTMClockGateTable[FTM_QD_InitStruct->instance].addr |= SIM_FTMClockGateTable[FTM_QD_InitStruct->instance].mask;
    FTM_InstanceTable[FTM_QD_InitStruct->instance]->MOD = FTM_MOD_MOD_MASK; //����Ϊ���
    FTM_InstanceTable[FTM_QD_InitStruct->instance]->CNTIN = FTM_CNTIN_INIT_MASK/2; //���ֵ��һ��
    FTM_InstanceTable[FTM_QD_InitStruct->instance]->MODE |= FTM_MODE_WPDIS_MASK; //��ֹд����
    FTM_InstanceTable[FTM_QD_InitStruct->instance]->MODE |= FTM_MODE_FTMEN_MASK; //FTMEN=1,�ر�TPM����ģʽ������FTM���й���
    switch(FTM_QD_InitStruct->PHA_Polarity)
    {
        case kFTM_QD_NormalPolarity:
            FTM_InstanceTable[FTM_QD_InitStruct->instance]->QDCTRL &= ~FTM_QDCTRL_PHAPOL_MASK;
            break;
        case kFTM_QD_InvertedPolarity:
            FTM_InstanceTable[FTM_QD_InitStruct->instance]->QDCTRL |= FTM_QDCTRL_PHAPOL_MASK;
            break;
        default:
            break;
    }
    switch(FTM_QD_InitStruct->PHB_Polarity)
    {
        case kFTM_QD_NormalPolarity:
            FTM_InstanceTable[FTM_QD_InitStruct->instance]->QDCTRL &= ~FTM_QDCTRL_PHBPOL_MASK;
            break;
        case kFTM_QD_InvertedPolarity:
            FTM_InstanceTable[FTM_QD_InitStruct->instance]->QDCTRL |= FTM_QDCTRL_PHBPOL_MASK;
            break;
        default:
            break;
    }
    switch(FTM_QD_InitStruct->mode)
    {
        case kQD_PHABEncoding:
            FTM_InstanceTable[FTM_QD_InitStruct->instance]->QDCTRL &= ~FTM_QDCTRL_QUADMODE_MASK;
            break;
        case kQD_CountDirectionEncoding:
            FTM_InstanceTable[FTM_QD_InitStruct->instance]->QDCTRL |= FTM_QDCTRL_QUADMODE_MASK;
            break;
        default:
            break;
    }
    FTM_InstanceTable[FTM_QD_InitStruct->instance]->QDCTRL |= FTM_QDCTRL_QUADEN_MASK;
    /* set clock source and prescaler */
    FTM_InstanceTable[FTM_QD_InitStruct->instance]->SC |= FTM_SC_CLKS(1)|FTM_SC_PS(3);
}

/**
 * @brief  �������ó�ʼ��FTMģ��ʵ���������빦��
 * @code
 *      //����FTM1ģ���PTA8/PTA9����������ģʽ��
 *      FTM_QD_QuickInit(FTM1_QD_PHA_PA08_PHB_PA09); 
 * @endcode         
 * @param  FTMxMAP  : FTM��������������ģʽ�µı��룬���ftm.h�ļ�
 * @retval None
 */
uint32_t FTM_QD_QuickInit(uint32_t FTMxMAP)
{
    uint8_t i;
    QuickInit_Type * pq = (QuickInit_Type*)&(FTMxMAP);
    FTM_QD_InitTypeDef FTM_QD_InitStruct1;
    FTM_QD_InitStruct1.instance = pq->ip_instance;
    FTM_QD_InitStruct1.mode = kQD_PHABEncoding;
    FTM_QD_InitStruct1.PHA_Polarity = kFTM_QD_NormalPolarity;
    FTM_QD_InitStruct1.PHB_Polarity = kFTM_QD_NormalPolarity;
    /* init pinmux and pull up */
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux);
        PORT_PinPullConfig(pq->io_instance, pq->io_base + i, kPullUp);
        PORT_PinOpenDrainConfig(pq->io_instance, pq->io_base + i, ENABLE);
    }
    /* init moudle */
    FTM_QD_Init(&FTM_QD_InitStruct1);
    return pq->ip_instance;
}

/**
 * @brief  ����������������
 * @code
 *      //���FTM1�������������
 *       uint32_t value��  //�洢�������������
 *       uint8_t  dire��   //�洢����
 *      FTM_QD_GetData(HW_FTM1, @value, @dire); 
 * @endcode         
 * @param  instance  :FTMģ���
 * @param  value     :�������ݴ洢��ַ
 * @param  direction :���巽��洢��ַ
 * @retval None
 */
void FTM_QD_GetData(uint32_t instance, uint32_t* value, uint8_t* direction)
{
    *direction = (FTM_InstanceTable[instance]->QDCTRL>>FTM_QDCTRL_QUADIR_SHIFT & 1);
	*value = (FTM_InstanceTable[instance]->CNT);
}

void FTM_QD_ClearCount(uint32_t instance)
{
    FTM_InstanceTable[instance]->CNT = 0;
}


/*combine channel control*/
/*dual capture control*/
/**
 * @brief  �ڲ��������û��������
 */
static uint32_t get_channel_pair_index(uint8_t channel)
{
    if((channel == HW_FTM_CH0) || (channel == HW_FTM_CH1))
    {
        return 0;
    }
    else if((channel == HW_FTM_CH2) || (channel == HW_FTM_CH3)) 
    {
        return 1;
    }
    else if((channel == HW_FTM_CH4) || (channel == HW_FTM_CH5)) 
    {
        return 2;
    }
    else 
    {
        return 3;
    }
}


/*!
 * @brief enable FTM peripheral timer channel pair output combine mode.
 * @param instance The FTM peripheral instance number.
 * @param channel  The FTM peripheral channel number.
 * @param enable  true to enable channle pair to combine, false to disable.
 */
/**
 * @brief  �ڲ��������û��������
 */
#define FTM_COMBINE_CHAN_CTRL_WIDTH  (8)
static void FTM_DualChlConfig(uint8_t instance, uint8_t chl, FTM_DualChlConfig_Type mode, FunctionalState newState)
{
    uint32_t mask;
    switch(mode)
    {
        case kFTM_Combine:
            mask = FTM_COMBINE_COMBINE0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;
        case kFTM_Complementary:
            mask = FTM_COMBINE_COMP0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;    
        case kFTM_DualEdgeCapture:
            mask = FTM_COMBINE_DECAPEN0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;           
        case kFTM_DeadTime:
            mask = FTM_COMBINE_DTEN0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;    
        case kFTM_Sync:
            mask = FTM_COMBINE_SYNCEN0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;   
        case kFTM_FaultControl:
            mask = FTM_COMBINE_FAULTEN0_MASK << (get_channel_pair_index(chl) * FTM_COMBINE_CHAN_CTRL_WIDTH);
            break;    
        default:
            break;
    } 
    (newState == ENABLE)?(FTM_InstanceTable[instance]->COMBINE |= mask):(FTM_InstanceTable[instance]->COMBINE &= ~mask);
}

//!< ����FTM ����ģʽ
/**
 * @brief  �ڲ��������û��������
 */
static void FTM_PWM_SetMode(uint8_t instance, uint8_t chl, FTM_PWM_Mode_Type mode)
{
    switch(mode)
    {
        case kPWM_EdgeAligned:
            FTM_InstanceTable[instance]->MODE &= ~FTM_MODE_FTMEN_MASK;
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->SC &= ~FTM_SC_CPWMS_MASK;
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
            FTM_PWM_InvertPolarity(instance, chl, kFTM_PWM_HighTrue);
            FTM_DualChlConfig(instance, chl, kFTM_Combine, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            break;
             #if 0
        case kPWM_CenterAligned:
            FTM_InstanceTable[instance]->MODE &= ~FTM_MODE_FTMEN_MASK;
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->SC |= FTM_SC_CPWMS_MASK;
            FTM_DualChlConfig(instance, chl, kFTM_Combine, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            break;
           
        case kInputCaptureFallingEdge:   
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~(FTM_CnSC_ELSB_MASK);
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_ELSA_MASK);
            
        case kInputCaptureRisingEdge: 
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_ELSB_MASK);
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~(FTM_CnSC_ELSA_MASK);
            
        case kInputCaptureBothEdge:  
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_ELSB_MASK|FTM_CnSC_ELSA_MASK);
            /* all configuration on input capture */
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->MODE &= ~FTM_MODE_FTMEN_MASK;
            FTM_InstanceTable[instance]->SC &= ~FTM_SC_CPWMS_MASK; 
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
            FTM_DualChlConfig(instance, chl, kFTM_Combine, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            break;
            #endif
        case kPWM_Complementary:
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->SC &= ~FTM_SC_CPWMS_MASK;
            FTM_InstanceTable[instance]->MODE |= FTM_MODE_WPDIS_MASK;
            FTM_InstanceTable[instance]->MODE |= FTM_MODE_FTMEN_MASK;
            FTM_DualChlConfig(instance, chl, kFTM_Combine, ENABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, ENABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, ENABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
            FTM_PWM_InvertPolarity(instance, chl, kFTM_PWM_HighTrue);
            FTM_InstanceTable[instance]->SYNC = FTM_SYNC_CNTMIN_MASK|FTM_SYNC_CNTMAX_MASK;
			FTM_InstanceTable[instance]->SYNC |= FTM_SYNC_SWSYNC_MASK;
            break;
        case kPWM_Combine:
            FTM_InstanceTable[instance]->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;
            FTM_InstanceTable[instance]->SC &= ~FTM_SC_CPWMS_MASK;
            FTM_InstanceTable[instance]->MODE |= FTM_MODE_WPDIS_MASK;
            FTM_InstanceTable[instance]->MODE |= FTM_MODE_FTMEN_MASK;
            FTM_DualChlConfig(instance, chl, kFTM_Combine, ENABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Complementary, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DualEdgeCapture, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_DeadTime, DISABLE);
            FTM_DualChlConfig(instance, chl, kFTM_Sync, ENABLE);
            FTM_DualChlConfig(instance, chl, kFTM_FaultControl, DISABLE);
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
            FTM_PWM_InvertPolarity(instance, chl, kFTM_PWM_HighTrue);
            FTM_InstanceTable[instance]->SYNC = FTM_SYNC_CNTMIN_MASK|FTM_SYNC_CNTMAX_MASK;
			FTM_InstanceTable[instance]->SYNC |= FTM_SYNC_SWSYNC_MASK;
            break;          
        default:
            break;
    }
}

//!< ��ת FTM����
/**
 * @brief  �ڲ��������û��������
 */
void FTM_PWM_InvertPolarity(uint8_t instance, uint8_t chl, uint32_t config)
{
    switch(config)
    {
        case kFTM_PWM_HighTrue:
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~FTM_CnSC_ELSA_MASK;
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= FTM_CnSC_ELSB_MASK;
            break;
        case kFTM_PWM_LowTrue:
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC |= FTM_CnSC_ELSA_MASK;
            FTM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~FTM_CnSC_ELSB_MASK;
            break;
        default:
            break;
    }
}

/**
 * @brief  �������ó�ʼ��FTMģ��ʵ��PWM����
 * @code
 *      
 * //����FTM0ģ���3ͨ����PTA6�����в���1000HZ��pwm����
 *      FTM_PWM_QuickInit(FTM0_CH3_PA06, 1000); 
 * @endcode         
 * @param  FTMxMAP        : FTM������PWMģʽ�µı��룬���ftm.h�ļ�
 * @param  frequencyInHZ  : FTM��������Ƶ������
 * @retval None
 */
uint8_t FTM_PWM_QuickInit(uint32_t FTMxMAP, uint32_t frequencyInHZ)
{
    uint8_t i;
    QuickInit_Type * pq = (QuickInit_Type*)&(FTMxMAP);
    FTM_PWM_InitTypeDef FTM_InitStruct1;
    FTM_InitStruct1.instance = pq->ip_instance;
    FTM_InitStruct1.frequencyInHZ = frequencyInHZ;
    FTM_InitStruct1.mode = kPWM_EdgeAligned;
    FTM_InitStruct1.chl = pq->channel;
    /* init pinmux */
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux); 
    }
    /* init moudle */
    FTM_PWM_Init(&FTM_InitStruct1);
    /* set duty to 50% */
    FTM_PWM_ChangeDuty(pq->ip_instance, pq->channel, 5000);
    return pq->ip_instance;
}

/**
 * @brief  ����ָ�����ŵ�PWM����ռ�ձ�
 * @code
 *      //����FTM0ģ���3ͨ����PWM����ռ�ձ�Ϊ50%
 *      FTM_PWM_ChangeDuty(HW_FTM0, 3, 5000); 
 * @endcode         
 * @param  instance       : FTMģ���0~2
 * @param  chl            : FTMģ���µ�ͨ����
 * @param  pwmDuty        : PWM����ռ�ձ�0~10000
 * @retval None
 */
void FTM_PWM_ChangeDuty(uint8_t instance, uint8_t chl, uint32_t pwmDuty)
{
    uint32_t cv = ((FTM_InstanceTable[instance]->MOD) * pwmDuty) / 10000;
    /* combine mode */
    if(FTM_InstanceTable[instance]->COMBINE & (FTM_COMBINE_COMBINE0_MASK|FTM_COMBINE_COMBINE1_MASK|FTM_COMBINE_COMBINE2_MASK|FTM_COMBINE_COMBINE3_MASK))
    { 
        if(chl%2)
        {
            FTM_InstanceTable[instance]->CONTROLS[chl-1].CnV = 0;
            FTM_InstanceTable[instance]->CONTROLS[chl].CnV = cv;
        }
        else
        {
            FTM_InstanceTable[instance]->CONTROLS[chl].CnV = 0;
            FTM_InstanceTable[instance]->CONTROLS[chl+1].CnV = cv;  
        }
        FTM_InstanceTable[instance]->PWMLOAD = 0xFFFFFFFF;
    }
    else
    {
    /* single chl */
    FTM_InstanceTable[instance]->CONTROLS[chl].CnV = cv; 
    }
}


#if 0

/***********************************************************************************************
 ���ܣ�FTM ���������������
 �βΣ�ftm: FTMģ���
       value: �������ֵָ��
       dir��  ����ָ�� 0���� 1���� 
 ���أ�0
 ��⣺Added in 2013-12-12
************************************************************************************************/
void FTM_QDGetData(FTM_Type *ftm, uint32_t* value, uint8_t* dir)
{
	*dir = (((ftm->QDCTRL)>>FTM_QDCTRL_QUADIR_SHIFT)&1);
	*value = (ftm->CNT);
}


void FTM_ITConfig(FTM_Type* FTMx, uint16_t FTM_IT, FunctionalState NewState)
{
	//�������
	assert_param(IS_FTM_ALL_PERIPH(FTMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));
	
	switch(FTM_IT)
	{
		case FTM_IT_TOF:
			(ENABLE == NewState)?(FTMx->SC |= FTM_SC_TOIE_MASK):(FTMx->SC &= ~FTM_SC_TOIE_MASK);
			break;
		case FTM_IT_CHF0:
		case FTM_IT_CHF1:
		case FTM_IT_CHF2:
		case FTM_IT_CHF3:
		case FTM_IT_CHF4:
		case FTM_IT_CHF5:
		case FTM_IT_CHF6:
		case FTM_IT_CHF7:	
			(ENABLE == NewState)?(FTMx->CONTROLS[FTM_IT].CnSC |= FTM_CnSC_CHIE_MASK):(FTMx->CONTROLS[FTM_IT].CnSC &= ~FTM_CnSC_CHIE_MASK);		
			break;
		default:break;
	}
}

ITStatus FTM_GetITStatus(FTM_Type* FTMx, uint16_t FTM_IT)
{
	ITStatus retval;
	//�������
	assert_param(IS_FTM_ALL_PERIPH(FTMx));
	assert_param(IS_FTM_IT(FTM_IT));
	

	switch(FTM_IT)
	{
		case FTM_IT_TOF:
			(FTMx->SC & FTM_SC_TOF_MASK)?(retval = SET):(retval = RESET);
		break;
		case FTM_IT_CHF0:
		case FTM_IT_CHF1:
		case FTM_IT_CHF2:
		case FTM_IT_CHF3:
		case FTM_IT_CHF4:
		case FTM_IT_CHF5:
		case FTM_IT_CHF6:
		case FTM_IT_CHF7:	
			(FTMx->CONTROLS[FTM_IT].CnSC& FTM_CnSC_CHF_MASK)?(retval = SET):(retval = RESET);
			break;
	}
	return retval;
}

void FTM_ClearITPendingBit(FTM_Type *FTMx,uint16_t FTM_IT)
{
	uint32_t read_value = 0;
	//�������
	assert_param(IS_FTM_ALL_PERIPH(FTMx));
	assert_param(IS_FTM_IT(FTM_IT));
	
	read_value = read_value;
	switch(FTM_IT)
	{
		case FTM_IT_TOF:
			read_value = FTMx->SC;
		break;
		case FTM_IT_CHF0:
		case FTM_IT_CHF1:
		case FTM_IT_CHF2:
		case FTM_IT_CHF3:
		case FTM_IT_CHF4:
		case FTM_IT_CHF5:
		case FTM_IT_CHF6:
		case FTM_IT_CHF7:	
			read_value = FTMx->CONTROLS[FTM_IT].CnSC;
			break;
	}
}
#endif



#if 0
static const QuickInit_Type FTM_QD_QuickInitTable[] = 
{
    { 1, 0, 6,  8, 2, 0}, //FTM1_QD_PHA_PA08_PHB_PA09 6
    { 1, 0, 7, 12, 2, 0}, //FTM1_QD_PHA_PA12_PHB_PA13 7
    { 1, 1, 6,  0, 2, 0}, //FTM1_QD_PHA_PB00_PHB_PB01  6
    { 2, 0, 6, 10, 2, 0}, //FTM2_QD_PHA_PA10_PHB_PA11  6
    { 2, 1, 6, 18, 2, 0}, //FTM2_QD_PHA_PB18_PHB_PB19 6
};

static const QuickInit_Type FTM_QuickInitTable[] =
{
    { 0, 1, 4, 12, 1, 4}, //FTM0_CH4_PB12 4
    { 0, 1, 4, 13, 1, 5}, //FTM0_CH5_PB13  4
    { 0, 0, 3,  0, 1, 5}, //FTM0_CH5_PA00  3
    { 0, 0, 3,  1, 1, 6}, //FTM0_CH6_PA01  3
    { 0, 0, 3,  2, 1, 7}, //FTM0_CH7_PA02  3
    { 0, 0, 3,  3, 1, 0}, //FTM0_CH0_PA03  3
    { 0, 0, 3,  4, 1, 1}, //FTM0_CH1_PA04  3
    { 0, 0, 3,  5, 1, 2}, //FTM0_CH2_PA05  3
    { 0, 0, 3,  6, 1, 3}, //FTM0_CH3_PA06  3
    { 0, 0, 3,  7, 1, 4}, //FTM0_CH4_PA07  3
    { 0, 2, 4,  1, 1, 0}, //FTM0_CH0_PC01  4
    { 0, 2, 4,  2, 1, 1}, //FTM0_CH1_PC02  4
    { 0, 2, 4,  3, 1, 2}, //FTM0_CH2_PC03  4
    { 0, 2, 4,  4, 1, 3}, //FTM0_CH3_PC04  4
    { 0, 3, 4,  4, 1, 4}, //FTM0_CH4_PD04  4
    { 0, 3, 4,  5, 1, 5}, //FTM0_CH5_PD05  4
    { 0, 3, 4,  6, 1, 6}, //FTM0_CH6_PD06  4
    { 0, 3, 4,  7, 1, 7}, //FTM0_CH7_PD07  4
    { 1, 1, 3, 12, 1, 0}, //FTM1_CH0_PB12  3
    { 1, 1, 3, 13, 1, 1}, //FTM1_CH1_PB13  3
    { 1, 0, 3,  8, 1, 0}, //FTM1_CH0_PA08  3
    { 1, 0, 3,  9, 1, 1}, //FTM1_CH1_PA09  3
    { 1, 0, 3, 12, 1, 0}, //FTM1_CH0_PA12  3
    { 1, 0, 3, 12, 1, 1}, //FTM1_CH1_PA13  3
    { 1, 1, 3,  0, 1, 0}, //FTM1_CH0_PB00  3
    { 1, 1, 3,  1, 1, 1}, //FTM1_CH1_PB01  3
    { 2, 0, 3, 10, 1, 0}, //FTM2_CH0_PA10  3
    { 2, 0, 3, 11, 1, 1}, //FTM2_CH1_PA11  3
    { 2, 1, 3, 18, 1, 0}, //FTM2_CH0_PB18  3
    { 2, 1, 3, 19, 1, 1}, //FTM2_CH1_PB19  3
};

#endif

