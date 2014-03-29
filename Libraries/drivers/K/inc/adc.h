/**
  ******************************************************************************
  * @file    adc.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ���ļ�ΪоƬADCģ��ĵײ㹦�ܺ���
  ******************************************************************************
  */
#ifndef __CH_LIB_ADC_H__
#define __CH_LIB_ADC_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

//!< ADCģ���
#define HW_ADC0  (0) 
#define HW_ADC1  (1)
#define HW_ADC2  (2)


//!< ADC ���ٳ�ʼ����
#define ADC0_SE0_DP0        (0x0U)
#define ADC0_SE1_DP1        (0x80000U)
#define ADC0_SE3_DP3        (0x180000U)
#define ADC0_SE4B_PC2       (0x204410U)
#define ADC0_SE5B_PD1       (0x284218U)
#define ADC0_SE6B_PD5       (0x304a18U)
#define ADC0_SE7B_PD6       (0x384c18U)
#define ADC0_SE8_PB0        (0x404008U)
#define ADC0_SE9_PB1        (0x484208U)
#define ADC0_SE12_PB2       (0x604408U)
#define ADC0_SE13_PB3       (0x684608U)
#define ADC0_SE14_PC0       (0x704010U)
#define ADC0_SE15_PC1       (0x784010U)
#define ADC0_SE17_E24       (0x887020U)
#define ADC0_SE18_E25       (0x907220U)
#define ADC0_SE19_DM0       (0x980000U)
#define ADC0_SE20_DM1       (0xa00000U)
#define ADC0_SE26_TEMP      (0xd00000U)
#define ADC1_SE0_DP0        (0x1U)
#define ADC1_SE1_DP1        (0x80001U)
#define ADC1_SE3_DP3        (0x180001U)
#define ADC1_SE4_PE0        (0x204021U)
#define ADC1_SE5_PE1        (0x284221U)
#define ADC1_SE6_PE2        (0x304421U)
#define ADC1_SE7_PE3        (0x384621U)
#define ADC1_SE4B_PC8       (0x205011U)
#define ADC1_SE5B_PC9       (0x285211U)
#define ADC1_SE6B_PC10      (0x305411U)
#define ADC1_SE7B_PC11      (0x385611U)
#define ADC1_SE8_PB0        (0x404009U)
#define ADC1_SE9_PB1        (0x484209U)
#define ADC1_SE14_PB10      (0x705409U)
#define ADC1_SE15_PB11      (0x785609U)
#define ADC1_SE17_PB117     (0x886201U)
#define ADC1_SE19_DM0       (0x980001U)
#define ADC1_SE20_DM1       (0xa00001U)
#define ADC1_SE26_TEMP      (0xd00001U)

//!< ADC ת��ʱ�ӷ�Ƶ����
typedef enum
{
    kADC_ClockDiv1,
    kADC_ClockDiv2,
    kADC_ClockDiv4,
    kADC_ClockDiv8,
}ADC_ClockDiv_Type;

//!< ADCת�����ȶ���
typedef enum
{
    kADC_SingleDiff8or9 = 0,
    kADC_SingleDiff12or13 = 1,
    kADC_SingleDiff10or11 = 2,
    kADC_SingleDIff16 = 3
}ADC_ResolutionMode_Type;
//!< ADC ������ʽ����
typedef enum
{
    kADC_TriggerSoftware,
    kADC_TriggerHardware,
}ADC_TriggerSelect_Type;
//!< �Ƿ�����ת��
typedef enum
{
    kADC_ContinueConversionEnable,
    kADC_ContinueConversionDisable,
}ADC_ContinueMode_Type;
//!< ����ADC���ǲ��ADC
typedef enum
{
    kADC_Single,
    kADC_Differential,
}ADC_SingleOrDiffMode_Type;

//!< ADCͨ������ѡ��(ÿ��ADCͨ����2��ADCת���� ΪMuxA �� MuxB)
#define kADC_MuxA                (0x00)
#define kADC_MuxB                (0x01)

//!< Ӳ��ƽ��
typedef enum
{
    kADC_HardwareAverageDisable,
    kADC_HardwareAverage_4,
    kADC_HardwareAverage_8,
    kADC_HardwareAverage_16,
    kADC_HardwareAverage_32,
}ADC_HardwareAveMode_Type;

//!< ADC�жϼ�DMA����ѡ��
typedef enum
{
    kADC_IT_Disable,        //!< AD�жϹ��ܽ�ֹ
    kADC_DMA_Disable,       //!< ADC DMA���ܽ�ֹ
    kADC_IT_EOF,            //!< ��ADC ת������ж�
    kADC_DMA_EOF,           //!< ��ADC DMA ����ж�
}ADC_ITDMAConfig_Type;

//!< ADC �ص���������
typedef void (*ADC_CallBackType)(uint32_t conversionValue);

//!< ADC ��ʼ���ṹ
typedef struct
{
    uint32_t                    instance;                   //!< ģ���
    uint32_t                    chl;                        //!< ADCͨ����
    ADC_TriggerSelect_Type      triggerMode;                //!< ����ģʽ ������� �� Ӳ������
    ADC_ClockDiv_Type           clockDiv;                   //!< ADCʱ�ӷ�Ƶ
    ADC_ResolutionMode_Type     resolutionMode;             //!< ��Ƶ��ѡ�� 8 10 12 16λ���ȵ�
    ADC_SingleOrDiffMode_Type   singleOrDiffMode;           //!< ���� ���� �������
    ADC_ContinueMode_Type       continueMode;               //!< �Ƿ���������ת��
    ADC_HardwareAveMode_Type    hardwareAveMode;            //!< Ӳ��ƽ������ѡ��
}ADC_InitTypeDef;


//!< API functions
void ADC_CallbackInstall(uint8_t instance, ADC_CallBackType AppCBFun);
void ADC_Init(ADC_InitTypeDef* ADC_InitStruct);
uint8_t ADC_QuickInit(uint32_t ADCxMAP, ADC_ResolutionMode_Type resolutionMode);
void ADC_ITDMAConfig(uint8_t instance, uint32_t mux, ADC_ITDMAConfig_Type config);
int32_t ADC_QuickReadValue(uint32_t ADCxMAP);
int32_t ADC_ReadValue(uint32_t instance, uint32_t mux);
void ADC_StartConversion(uint32_t instance, uint32_t chl, uint32_t mux);
uint8_t ADC_IsConversionCompleted(uint32_t instance, uint32_t mux);



#ifdef __cplusplus
}
#endif



#endif
