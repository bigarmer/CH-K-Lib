/**
  ******************************************************************************
  * @file    ftm.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ���ļ�ΪоƬFTMģ��ĵײ㹦�ܺ���
  ******************************************************************************
  */
#ifndef __CH_LIB_FTM_H_
#define	__CH_LIB_FTM_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdint.h>

/* FTM Ӳ��ģ��� */
#define HW_FTM0    (0x00)
#define HW_FTM1    (0x01) 
#define HW_FTM2    (0x02) 

/* FTM���õ�ͨ�� */
#define HW_FTM_CH0 (0x00)
#define HW_FTM_CH1 (0x01)
#define HW_FTM_CH2 (0x02)
#define HW_FTM_CH3 (0x03)
#define HW_FTM_CH4 (0x04)
#define HW_FTM_CH5 (0x05)
#define HW_FTM_CH6 (0x06)
#define HW_FTM_CH7 (0x07)


//FTM  ģʽѡ��
typedef enum
{
	kPWM_EdgeAligned,           //���ض��� ���
	kPWM_Combine,               //���ģʽ
    kPWM_Complementary,         //����ģʽ �������ģʽ ����Chl(n) �� Chl(n+1) �ǻ������
    #if 0
    kPWM_CenterAligned,         //���Ķ��� Ƶ���Ǳ��ض����һ��
    #endif
}FTM_PWM_Mode_Type;

//FTM ����ģʽѡ�� ��תռ�ձ�
#define kFTM_PWM_HighTrue       (0x00)
#define kFTM_PWM_LowTrue        (0x01)

//����PWMռ�ձ���� ��ʼ���ṹ
typedef struct
{
    uint32_t            instance;
    uint32_t            chl;
    uint32_t            frequencyInHZ;
    FTM_PWM_Mode_Type   mode;
}FTM_PWM_InitTypeDef;

typedef enum
{
    kFTM_QD_NormalPolarity,
    kFTM_QD_InvertedPolarity,
}FTM_QD_PolarityMode_Type;


/* QD  ģʽѡ�� */
typedef enum
{
	kQD_PHABEncoding,
	kQD_CountDirectionEncoding,
}FTM_QD_Mode_Type;

/* QD��ʼ�� */
typedef struct
{
    uint32_t                            instance;
    FTM_QD_PolarityMode_Type            PHA_Polarity;
    FTM_QD_PolarityMode_Type            PHB_Polarity; 
    FTM_QD_Mode_Type                    mode;
}FTM_QD_InitTypeDef;

//!< FTM PWM ���ٳ�ʼ��
#define FTM0_CH4_PB12   (0x205908U)
#define FTM0_CH5_PB13   (0x285b08U)
#define FTM0_CH5_PA00   (0x2840c0U)
#define FTM0_CH6_PA01   (0x3042c0U)
#define FTM0_CH7_PA02   (0x3844c0U)
#define FTM0_CH0_PA03   (0x46c0U)
#define FTM0_CH1_PA04   (0x848c0U)
#define FTM0_CH2_PA05   (0x104ac0U)
#define FTM0_CH3_PA06   (0x184cc0U)
#define FTM0_CH4_PA07   (0x204ec0U)
#define FTM0_CH0_PC01   (0x4310U)
#define FTM0_CH1_PC02   (0x84510U)
#define FTM0_CH2_PC03   (0x104710U)
#define FTM0_CH3_PC04   (0x184910U)
#define FTM0_CH4_PD04   (0x204918U)
#define FTM0_CH5_PD05   (0x284b18U)
#define FTM0_CH6_PD06   (0x304d18U)
#define FTM0_CH7_PD07   (0x384f18U)
#define FTM1_CH0_PB12   (0x58c9U)
#define FTM1_CH1_PB13   (0x85ac9U)
#define FTM1_CH0_PA08   (0x50c1U)
#define FTM1_CH1_PA09   (0x852c1U)
#define FTM1_CH0_PA12   (0x58c1U)
#define FTM1_CH1_PA13   (0x858c1U)
#define FTM1_CH0_PB00   (0x40c9U)
#define FTM1_CH1_PB01   (0x842c9U)
#define FTM2_CH0_PA10   (0x54c2U)
#define FTM2_CH1_PA11   (0x856c2U)
#define FTM2_CH0_PB18   (0x64caU)
#define FTM2_CH1_PB19   (0x866caU)
//!< FTM ����������ٳ�ʼ��
#define FTM1_QD_PHA_PA08_PHB_PA09       (0x9181U)
#define FTM1_QD_PHA_PA12_PHB_PA13       (0x99c1U)
#define FTM1_QD_PHA_PB00_PHB_PB01       (0x8189U)
#define FTM2_QD_PHA_PA10_PHB_PA11       (0x9582U)
#define FTM2_QD_PHA_PB18_PHB_PB19       (0xa58aU)


//!< API functions
void FTM_PWM_ChangeDuty(uint32_t instance, uint8_t chl, uint32_t pwmDuty);
uint8_t FTM_PWM_QuickInit(uint32_t FTMxMAP, uint32_t frequencyInHZ);
void FTM_PWM_Init(FTM_PWM_InitTypeDef* FTM_InitStruct);
void FTM_PWM_InvertPolarity(uint32_t instance, uint8_t chl, uint32_t config);
uint32_t FTM_QD_QuickInit(uint32_t FTMxMAP);
void FTM_QD_Init(FTM_QD_InitTypeDef * FTM_QD_InitStruct);
void FTM_QD_GetData(uint32_t instance, uint32_t* value, uint8_t* direction);
void FTM_QD_ClearCount(uint32_t instance);


#ifdef __cplusplus
}
#endif





#endif
