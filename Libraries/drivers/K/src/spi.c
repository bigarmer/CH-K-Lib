/**
  ******************************************************************************
  * @file    spi.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "spi.h"
#include "gpio.h"
#include "common.h"
#include "clock.h"


#if (!defined(SPI_BASES))

    #if     (defined(MK60DZ10))
    #define SPI_BASES {SPI0, SPI1, SPI2}
    #elif   (defined(MK10D5))
    #define SPI_BASES {SPI0}
    #endif

#endif

SPI_Type * const SPI_InstanceTable[] = SPI_BASES;
static SPI_CallBackType SPI_CallBackTable[ARRAY_SIZE(SPI_InstanceTable)] = {NULL};
    
#if (defined(MK60DZ10) || defined(MK40D10) || defined(MK60D10)|| defined(MK10D10) || defined(MK70F12) || defined(MK70F15))
static const RegisterManipulation_Type SIM_SPIClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_DSPI0_MASK},
    {(void*)&(SIM->SCGC6), SIM_SCGC6_SPI1_MASK},
    {(void*)&(SIM->SCGC3), SIM_SCGC3_SPI2_MASK},
};
static const IRQn_Type SPI_IRQnTable[] = 
{
    SPI0_IRQn,
    SPI1_IRQn,
    SPI2_IRQn,
};

#elif (defined(MK10D5))
static const RegisterManipulation_Type SIM_SPIClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_SPI0_MASK},
};
static const IRQn_Type SPI_IRQnTable[] = 
{
    SPI0_IRQn,
};
#endif

/**
 * @brief  SPI ��ʼ��
 *
 * @param  SPI  ��ʼ���ṹ��
 * @retval None
 */
void SPI_Init(SPI_InitTypeDef * SPI_InitStruct)
{
    // enable clock gate
    uint32_t * SIM_SCGx = (void*) SIM_SPIClockGateTable[SPI_InitStruct->instance].register_addr;
    *SIM_SCGx |= SIM_SPIClockGateTable[SPI_InitStruct->instance].mask;
    // let all PCS low when in inactive mode
    // stop SPI
    SPI_InstanceTable[SPI_InitStruct->instance]->MCR |= SPI_MCR_HALT_MASK;
    // master or slave
    switch(SPI_InitStruct->mode)
    {
        case kSPI_Master:
            SPI_InstanceTable[SPI_InitStruct->instance]->MCR |= SPI_MCR_MSTR_MASK;
            break;
        case kSPI_Slave:
            SPI_InstanceTable[SPI_InitStruct->instance]->MCR &= ~SPI_MCR_MSTR_MASK;
            break;
        default:
    }
    //enable SPI clock
    SPI_InstanceTable[SPI_InitStruct->instance]->MCR &= ~SPI_MCR_MDIS_MASK;
    // disable FIFO and clear FIFO flag
    SPI_InstanceTable[SPI_InitStruct->instance]->MCR |= 
        SPI_MCR_PCSIS_MASK |
        SPI_MCR_HALT_MASK |
        SPI_MCR_CLR_TXF_MASK|
        SPI_MCR_CLR_RXF_MASK|
        SPI_MCR_DIS_TXF_MASK|
        SPI_MCR_DIS_RXF_MASK;
    // Set SPI clock, SPI use Busclock
    SPI_InstanceTable[SPI_InitStruct->instance]->CTAR[1] = (SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(0)|SPI_CTAR_BR(SPI_InitStruct->baudrateDivSelect));
    // data size
    SPI_InstanceTable[SPI_InitStruct->instance]->CTAR[1] |= SPI_CTAR_FMSZ(SPI_InitStruct->dataSizeInBit-1);
    // CPOL
    switch(SPI_InitStruct->CPOL)
    {
        case kSPI_CPOL_InactiveLow:
            SPI_InstanceTable[SPI_InitStruct->instance]->CTAR[1] &= ~SPI_CTAR_CPHA_MASK;
            break;
        case kSPI_CPOL_InactiveHigh:
            SPI_InstanceTable[SPI_InitStruct->instance]->CTAR[1] |= SPI_CTAR_CPHA_MASK;
            break;
        default:
            break;
    }
    //CPHA
    switch(SPI_InitStruct->CPHA)
    {
        case kSPI_CPHA_1Edge:
            SPI_InstanceTable[SPI_InitStruct->instance]->CTAR[1] &= ~SPI_CTAR_CPHA_MASK;
            break;
        case kSPI_CPHA_2Edge:
            SPI_InstanceTable[SPI_InitStruct->instance]->CTAR[1] |= SPI_CTAR_CPHA_MASK;
            break;
        default:
            break;
    }
    
    // clear all flags
    SPI_InstanceTable[SPI_InitStruct->instance]->SR = SPI_SR_EOQF_MASK   
            | SPI_SR_TFUF_MASK    
            | SPI_SR_TFFF_MASK 
            | SPI_SR_RFOF_MASK 
            | SPI_SR_RFDF_MASK
            | SPI_SR_TCF_MASK;
    // launch
    SPI_InstanceTable[SPI_InitStruct->instance]->MCR &= ~SPI_MCR_HALT_MASK;
}
 


void SPI_ITDMAConfig(uint32_t instance, SPI_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kSPI_IT_TCF_Disable: 
            SPI_InstanceTable[instance]->RSER &= ~SPI_RSER_TCF_RE_MASK;
            NVIC_DisableIRQ(SPI_IRQnTable[instance]);
            break;
        case kSPI_IT_TCF: 
            NVIC_EnableIRQ(SPI_IRQnTable[instance]);
            SPI_InstanceTable[instance]->RSER |= SPI_RSER_TCF_RE_MASK;
            break;
        default:
            break;
    }
}

void SPI_CallbackInstall(uint32_t instance, SPI_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        SPI_CallBackTable[instance] = AppCBFun;
    }
}


uint16_t SPI_ReadWriteByte(uint32_t instance, uint16_t data, uint16_t CSn, uint16_t csState)
{
    uint16_t read_data;
	SPI_InstanceTable[instance]->PUSHR = (((uint32_t)(((csState))<<SPI_PUSHR_CONT_SHIFT))&SPI_PUSHR_CONT_MASK) 
            | SPI_PUSHR_CTAS(1)      
            | SPI_PUSHR_PCS(1<<CSn)
            | SPI_PUSHR_TXDATA(data);
    if(!(SPI_InstanceTable[instance]->RSER & SPI_RSER_TCF_RE_MASK)) // if it is polling mode
    {
        // wait for transfer complete
        while(!(SPI_InstanceTable[instance]->SR & SPI_SR_TCF_MASK)){};
        // clear flag
        SPI_InstanceTable[instance]->SR |= SPI_SR_TCF_MASK;
    }
    read_data = (uint16_t)SPI_InstanceTable[instance]->POPR;
    return read_data;
}

void SPI0_IRQHandler(void)
{
    SPI_InstanceTable[HW_SPI0]->SR |= SPI_SR_TCF_MASK ;
    if(SPI_CallBackTable[HW_SPI0])
    {
        SPI_CallBackTable[HW_SPI0]();
    }
}

void SPI1_IRQHandler(void)
{
    SPI_InstanceTable[HW_SPI1]->SR |= SPI_SR_TCF_MASK ;
    if(SPI_CallBackTable[HW_SPI1])
    {
        SPI_CallBackTable[HW_SPI1]();
    }
}

void SPI2_IRQHandler(void)
{
    SPI_InstanceTable[HW_SPI2]->SR |= SPI_SR_TCF_MASK ;
    if(SPI_CallBackTable[HW_SPI2])
    {
        SPI_CallBackTable[HW_SPI2]();
    }
}
