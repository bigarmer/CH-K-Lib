/**
  ******************************************************************************
  * @file    tsi.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片TSI模块的底层功能函数
  ******************************************************************************
  */
#ifndef __CH_LIB_TSI_H__
#define __CH_LIB_TSI_H__

#include <stdint.h>
  
//!< TSI 触发模式选择
typedef enum
{
    kTSI_TriggerSoftware,     //软件触发
    kTSI_TriggerPeriodicalScan,//硬件周期性扫描
} TSI_TriggerMode_Type;
  
//!< TSI 中断DMA配置
typedef enum
{
    kTSI_IT_Disable,        //!< Disable Interrupt
    kTSI_IT_OutOfRange,
    kTSI_IT_EndOfScan,
}TSI_ITDMAConfig_Type;

//TSI初始化结构
typedef struct
{
    uint32_t                chl;            //!< 通道号1~15
    TSI_TriggerMode_Type    triggerMode;    //!< 触发模式选择
    uint32_t                threshld;       //!< 发生超范围中断时的判断阀值
}TSI_InitTypeDef;

//!< TSI QuickInit macro
#define TSI0_CH0_PB00   (0x00004008)
#define TSI0_CH1_PA00   (0x00084000)
#define TSI0_CH2_PA01   (0x00104200)
#define TSI0_CH3_PA02   (0x00184400)
#define TSI0_CH4_PA03   (0x00204600)
#define TSI0_CH5_PA04   (0x00284800)
#define TSI0_CH6_PB01   (0x00304208)
#define TSI0_CH7_PB02   (0x00384408)
#define TSI0_CH8_PB03   (0x00404608)
#define TSI0_CH9_PB16   (0x00486008)
#define TSI0_CH10_PB17  (0x00506208)
#define TSI0_CH11_PB18  (0x00586408)
#define TSI0_CH12_PB19  (0x00606608)
#define TSI0_CH13_PC00  (0x00684010)
#define TSI0_CH14_PC01  (0x00704210)
#define TSI0_CH15_PC02  (0x00784410)


//!< TSI CallBack Type
typedef void (*TSI_CallBackType)(uint32_t outOfRangeArray);

//!< API functions
uint32_t TSI_QuickInit(uint32_t UARTxMAP);
uint32_t TSI_GetCounter(uint32_t chl);
void TSI_ITDMAConfig(TSI_ITDMAConfig_Type config);
void TSI_Init(TSI_InitTypeDef* TSI_InitStruct);
void TSI_CallbackInstall(TSI_CallBackType AppCBFun);


#endif

